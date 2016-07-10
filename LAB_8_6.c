/*

BUILT IN THE FOLDER FOR SDADCPOLL_MINE for make all files work....
please rebuild folder... etc

built up around Lab 8.6 in Ying Bai's book 'Practical Engineering with ARM technology'

*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define GPIO_PB0_U1RX     0x00010001
#define GPIO_PB1_U1TX     0x00010401
#define INT_UART1         22

void UARTIntHandler(void);
char     rdata[26];
uint8_t  rnum = 0;
bool     intFlag = true;

int main (void) {
  char   s_data;
  int    rcdata[26];

SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB | SYSCTL_PERIPH_GPIOF);
SysCtlPeripheralEnable (SYSCTL_PERIPH_UART1);
GPIOPinTypeGPIOOutput  (GPIO_PORTF_BASE, GPIO_PIN_1);
GPIOPinTypeUART        (GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_0);
GPIOPinConfigure       (GPIO_PB1_U1TX | GPIO_PB0_U1RX);

  
}

