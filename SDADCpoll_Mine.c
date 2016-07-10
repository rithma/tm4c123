//************************************************************************************
//SDADCPoll_Mine.c -  Main Application for ADC0 Polling
//Created alongside the book "Practical Microcontroller engineering with ARM" by Ying Bai
//
//
// Etienne Stehelin
// June 4, 2016
//************************************************************************************


#include <stdint.h>
#include <stdbool.h>
#include "adc.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib\sysctl.h"
#include "driverlib\gpio.h"
#include "PinDefines.h"
#include "UART.h"
#include "Nokia5110.h"


int main(void) {
		
	uint32_t ui32Value, ssn = 0, sample = 0;
	
	//enable clocks for GPIO Ports B & F and ADC0...
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	
	//configure GPIO ports B & F as output ports...
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0);
	
	//Configure ADC0 - SS0 - Channel 1 & the first sample
	ADCSequenceDisable(ADC0_BASE, ssn);
	ADCSequenceConfigure(ADC0_BASE, ssn, ADC_TRIGGER_PROCESSOR,0);
  ADCSequenceStepConfigure(ADC0_BASE, ssn, sample, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH1);
  ADCSequenceEnable(ADC0_BASE, ssn);
	
	Nokia5110_Init();
	

	
	

while(1) {
//Trigger the sample Sequencer - SSI0
    ADCProcessorTrigger(ADC0_BASE, ssn);	

	//Wait until the sample sequence has finished
	while(!ADCIntStatus(ADC0_BASE, ssn, false)){}
		
	//Read the value from the ADC
	ADCSequenceDataGet(ADC0_BASE, ssn, &ui32Value);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 |GPIO_PIN_2 | GPIO_PIN_1, ui32Value >> 8);
//	GPIOPinWrite(PORTF_DATA, PF3 |PF2 | PF1, ui32Value >> 8);

	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0, ui32Value >> 8);
	ADCIntClear(ADC0_BASE, ssn);
		
	Nokia5110_Clear();
  Nokia5110_OutString("************ I love you     Luci  ********************** ");	
	Nokia5110_SetCursor(5, 5);          // five leading spaces, bottom row
  Nokia5110_OutChar('A');
}

	
}
	

