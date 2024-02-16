/*
 * LCD_TFT_driver.cpp
 *
 * Created: 14/02/2024 12:46:06
 * Author : nastr
 */ 

#include <avr/io.h>
#include "TFTdriver.c"

int main(void)
{
	DisplayInit();
	_delay_ms(250);
	
	SleepOut();
	_delay_ms(500);
	DisplayOff();
	_delay_ms(500);
	
	DisplayOn();
	_delay_ms(500);
	DisplayOff();
	_delay_ms(250);
	DisplayOn();
	_delay_ms(250);
	DisplayOff();
	_delay_ms(250);
	DisplayOn();
	_delay_ms(250);
	DisplayOff();
	_delay_ms(250);
	DisplayOn();
	_delay_ms(250);
	
	
    /* Replace with your application code */
    while (1) 
    {
		FillRectangle(0, 0, 200, 200, 0x0, 0x0, 0xFF);
		_delay_ms(500);
    }
}

