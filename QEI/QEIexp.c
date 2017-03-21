/* 
JACKED from 
https://sehrainder.wordpress.com/2015/11/07/quadrature-encoder-interface-tiva-c-launchpad/

Program to interface a 1024 ppr encoder using the Quadrature Encoder Interface
IDX Signal -> Port D 3
A -> Port D 6
B -> Port D 7
Clock settings -> 80Mhz
Phase Error Interrupt Enabled -> The interrupt routine is called when the A and B pulses are in phase. Some transducer might use output in phase pulses to signal an error. The Red LED on the board flashes when this happens. 
*/
 
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
 
int flag = 0;
 
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
 
//Millisecond delay subroutine 
void delay(unsigned long millisecs){
  unsigned long count;
   
  while(millisecs > 0 ) { 
    count = 333333; 
    while (count > 0) { 
      count--;
    } 
    millisecs--;
  }
}
 
//Port F (PF1) setup subroutine for Red LED
void PortF_Init(void){ 
  volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // Port F Clock Turned ON
  delay = SYSCTL_RCGC2_R;           // Delay for clock turn ON  
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // Unlock PortF   
  GPIO_PORTF_CR_R |= 0x02;          // Allow changes to PF1       
  GPIO_PORTF_AMSEL_R &= ~0x02;      // Disable analog function on PF1
  GPIO_PORTF_PCTL_R &= 0x00000000;  // GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R |= 0x02;         // PF1 set as output  
  GPIO_PORTF_AFSEL_R &= ~0x02;      // No alternate function
  GPIO_PORTF_PUR_R &= ~0x02;        // Disable pullup resistors on PF1       
  GPIO_PORTF_DEN_R |= 0x02;         // Enable digital function on PF1        
}
 
// Function to setup the QEI on PD3, PD6, PD7 i.e. QEI0
void QEI_Init()
{
    SYSCTL_RCGCQEI_R |= 0x01;       // QEI run mode clock gating control on
    SYSCTL_RCGCGPIO_R |= 0x08;      //Enable clock for PORTD
     
    GPIO_PORTD_LOCK_R = 0x4C4F434B; //Unlock port D for changes
    GPIO_PORTD_CR_R |= 0xC8;        // Allow changes to PD3,6,7      
  GPIO_PORTD_AMSEL_R &= 0x00;           // Disable analog function
    GPIO_PORTD_DEN_R |= 0xC8;       // Enable Digital Pins
    GPIO_PORTD_DIR_R &= ~0xC8;  // Configure direction of pins as Input
    GPIO_PORTD_AFSEL_R |= 0xC8;     // QEI alternate function on PD3,6,7
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0x00FF0FFF)+0x66006000; 
    // Set PCTL Register PMC7, PMC6, PMC3 to 0x06  
     
    NVIC_PRI3_R = (NVIC_PRI3_R&0xFFFF1FFF)|0xA000; //QEI 0 Interrupt priority set to 5
    NVIC_EN0_R |= 0x2000;                          //Enable QEI 0 Interrupt
     
    QEI0_CTL_R |= 0x38; 
    //Velocity Capture on, Reset on index pulse, both edges of A and B signals are counted -> pulses/rev = 4096
    QEI0_INTEN_R |= 0x08;             //Enable the Phase error interrupt
    QEI0_LOAD_R = 0x00001387;         //Velocity timer value 5000 - 1 (Speed is determined by counting the number of pulses in this amount of time.
    QEI0_MAXPOS_R = ((1024 * 4) - 1); //Maximum Position Value 
    QEI0_CTL_R |= 0x01;               //Enable QEI
}
 
//If there is a phase error then this interrupt handler is called
void Quadrature0_Handler(void)
{
    DisableInterrupts();   //Disable interrupts since we don't want an interrupt when we are in an interrupt
    if((QEI0_RIS_R&0x08))  //Check if the interrupt is caused by a Phase Error
    {
        flag = 1; //Set a flag to indicate a phase error
        QEI0_ISC_R |= 0x08; //Clearing the interrupt 
    }
}
 
 
int main()
{
    unsigned long position;
    PLL_Init(); //Initialise PLL -> main clock = 80MHz
    UART_Init(); //Initialise UART
    QEI_Init();  //Initialise quadrature encoder interface
    PortF_Init(); //Iniitalise port F
    while(1)
    {
        if(flag) //Check if the phase error flag is set
        {
            GPIO_PORTF_DATA_R |= 0x02; //Turn on Phase error LED (RED)
            delay(1); //Wait for 1ms
            flag = 0; //Reset the phase error flag 
        }
        else
        {
            position = QEI0_POS_R&0x00000FFF; //Get the position of the encoder i.e. pulse count
            UART_OutUDec(position); //Spit this position out on the UART
            UART_OutChar('\n'); 
            delay(1); //Wait for 1ms
        }
        GPIO_PORTF_DATA_R &= ~0x02; //Turn off Phase error LED.
        EnableInterrupts(); //Enable interrupts back. If this is done in the interrupt handler above then the controller is going to get stuck 
        //in the interrupt forever, so we do this here after it has notified the user of the encoder position.
    }
}
