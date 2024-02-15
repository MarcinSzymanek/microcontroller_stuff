/*
 * AMS_LAB2.cpp
 *
 * Created: 07/02/2024 11:55:19
 * Author : nastr
 *
 * Using simulator and Atmel-ICE
 */ 

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>



int main(void)
{
	uint8_t i = 0;
	
	DDRA = 0x00;
	DDRB = 0xFF;
    while (1) 
    {
		if(~PINA & 0b10000000) 
			i = 0;
		
		PORTB = i;
		i++;
		_delay_ms(500);
    }
}

