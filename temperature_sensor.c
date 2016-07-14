//*****************************************************************************
//
// temperature_sensor.c - Example demonstrating the internal ADC temperature
//                        sensor.
//
// Copyright (c) 2010-2014 Texas Instruments Incorporated.  All rights reserved.

// This is part of revision 2.1.0.12573 of the Tiva Firmware Development Package.
//
//*****************************************************************************
/*
 * 	This code was made to show a simple ADC read.
 *
 * 	It was made from the example provided by TivaWare but it was a some modifications
 * like the math
 *
 *
 * Luís Afonso
 *
 *
 */


#define PART_TM4C123GH6PM

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_pwm.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/pwm.h"
#include "driverlib/ssi.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"
#include <string.h>

volatile uint32_t millis=0;

void SycTickInt(){
  millis++;
}
void SysTickbegin(){
  SysTickPeriodSet(80000);
  SysTickIntRegister(SycTickInt);
  SysTickIntEnable();
   SysTickEnable();
}

void Wait(uint32_t time){
	uint32_t temp = millis;
	while( (millis-temp) < time){
	}	
}

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void
InitConsole(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);     //select UART GPIO bank
    GPIOPinConfigure(GPIO_PA0_U0RX);                 //configures pin mux
    GPIOPinConfigure(GPIO_PA1_U0TX);                 //configures pin mux
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);    //enable UART0 to config clock
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);//uses 16000000 clock for uart clock
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);   //uart pins
    UARTStdioConfig(0, 115200, 16000000);           //initialize uart for condole i/o
}

int main(){
		uint32_t ADCValues[1];
		uint32_t TempValueC;
		uint32_t TempValueF;



	  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	  SysTickbegin();
	  InitConsole();
	    //
	    // This array is used for storing the data read from the ADC FIFO. It
	    // must be as large as the FIFO for the sequencer in use.  This example
	    // uses sequence 3 which has a FIFO depth of 1.  If another sequence
	    // was used with a deeper FIFO, then the array size must be changed.
	    //

	    UARTprintf("ADC ->\n");
	    UARTprintf("  Type: Internal Temperature Sensor\n");
	    UARTprintf("  Samples: One\n");
	    UARTprintf("  Update Rate: 250ms\n");
	    UARTprintf("  Input Pin: Internal temperature sensor\n\n");

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);    //enable ADC
	    SysCtlDelay(3);

	    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
	    // will do a single sample when the processor sends a singal to start the
	    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
	    // to sequence 3.  This example is arbitrarily using sequence 3.
	    //
	    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

	    //
	    // Configure step 0 on sequence 3.  Sample the temperature sensor
	    // (ADC_CTL_TS) and configure the interrupt flag (ADC_CTL_IE) to be set
	    // when the sample is done.  Tell the ADC logic that this is the last
	    // conversion on sequence 3 (ADC_CTL_END).  Sequence 3 has only one
	    // programmable step.  Sequence 1 and 2 have 4 steps, and sequence 0 has
	    // 8 programmable steps.  Since we are only doing a single conversion using
	    // sequence 3 we will only configure step 0.  For more information on the
	    // ADC sequences and steps, reference the datasheet.
	    //
	    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE |
	                             ADC_CTL_END);


	    //ADCHardwareOversampleConfigure(ADC0_BASE,64);

	    //
	    // Since sample sequence 3 is now configured, it must be enabled.
	    //
	    ADCSequenceEnable(ADC0_BASE, 3);

	    //
	    // Clear the interrupt status flag.  This is done to make sure the
	    // interrupt flag is cleared before we sample.
	    //
	    ADCIntClear(ADC0_BASE, 3);

	    //
	    // Sample the temperature sensor forever.  Display the value on the
	    // console.
	    //
	    while(1)
	    {
	        //
	        // Trigger the ADC conversion.
	        //
	        ADCProcessorTrigger(ADC0_BASE, 3);

	        //
	        // Wait for conversion to be completed.
	        //
	        while(!ADCIntStatus(ADC0_BASE, 3, false))
	        {
	        }

	        //
	        // Clear the ADC interrupt flag.
	        //
	        ADCIntClear(ADC0_BASE, 3);

	        //
	        // Read ADC Value.
	        //
	        ADCSequenceDataGet(ADC0_BASE, 3, ADCValues);

	        //
	        // Use non-calibrated conversion provided in the data sheet. I use floats in intermediate
	        // math but you could use intergers with multiplied by powers of 10 and divide on the end
	        // Make sure you divide last to avoid dropout.
	        //
	        TempValueC = (uint32_t)(147.5 - ((75.0*3.3 *(float)ADCValues[0])) / 4096.0);

	        //
	        // Get Fahrenheit value.  Make sure you divide last to avoid dropout.
	        //
	        TempValueF = ((TempValueC * 9) + 160) / 5;

	        //
	        // Display the temperature value on the console.
	        //
	        UARTprintf("Temperature = %3d*C or %3d*F\r", TempValueC,
	        		TempValueF);

	        //
	        // This function provides a means of generating a constant length
	        // delay.  The function delay (in cycles) = 3 * parameter.  Delay
	        // 250ms arbitrarily.
	        //
	        SysCtlDelay(80000000 / 12);
	    }



}

