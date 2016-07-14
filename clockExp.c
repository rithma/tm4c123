#include <stdint.h>
#include <stdbool.h>
#include "header.h"
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>

int main (void)
{
	int clockSpeed = 0;
	int clockExperiment = 0;
	clockSpeed = SysCtlClockGet();
	while(1)
	{
		clockExperiment = clockSpeed;
		
	}
	
	
}
