/*
* LAB8.c
*
* Author : Henning Hargaard
* Modified Michael Alrøe
*/
#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include "MAX3421.h"
#include "MAX3420E_registers.h"

#define TWENTY_MSEC 14200           // adjust this constant for 20 msec button checks
#define BLINKTIME 25                // blink every 500 msec

int main(void)
{
	initMAX3421();
	while (1)
	{
		if(Suspended)
		check_for_resume();
		if (MAX_Int_Pending())
		service_irqs();
		msec_timer++;
		if(msec_timer == TWENTY_MSEC)
		{
			msec_timer=0;
			
			if((rreg(rGPIO) & 0x10) == 0) // Check the pushbutton on GPI-0
			{
				inhibit_send = 0x00;      // Tell the "do_IN3" function to send the text string
				// L0_ON                     // Turn on the SEND light
			}
			blinktimer++;                 // blink the loop active light every half second
			if(blinktimer == BLINKTIME)
			{
				blinktimer=0;
				L3_BLINK
			}
		}
	}
}

