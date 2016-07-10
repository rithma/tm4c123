
/*
built up around Lab 8.6 in Ying Bai's book 'Practical Engineering with ARM technology'
- it going to take some time,   but this is in effort to learn how to use API functions,
-like SysCtlPeripheralEnable()     when i dont really grasp what to put after the parenthesis.
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define GPIO_PB0_U1RX     0x00010001
#define GPIO_PB1_U1TX     0x00010401
#define INT_UART1         22

void UARTIntHandler(void);
char rdata[26];
uint8_t rnum = 0;
bool intFlag = true;

int main (void) {
  char s_data;
  int rcdata[26];
  
//3. Use SysCtlPeripheralEnable() to enable clock and GPIO Port B
  
  
}

