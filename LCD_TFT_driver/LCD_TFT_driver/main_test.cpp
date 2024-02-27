/*
 * LCD_TFT_driver.cpp
 *
 * Created: 14/02/2024 12:46:06
 * Author : nastr
 */ 

#include <avr/io.h>
#include "TFTdriver.c"

void drawHeart(){
	
	SetPageAddress(70, 120);
	SetColumnAddress(15, 30);
	FillSelection(0xFF, 0x0, 0x0);
	
	SetPageAddress(50, 140);
	SetColumnAddress(30, 90);
	FillSelection(0xFF, 0x0, 0x0);

	for(int i = 0; i < 6; i++){
		SetPageAddress(135, 140 + i*5);
		SetColumnAddress(60 + i*5, 90);
		FillSelection(0xFF, 0x0, 0x0);
		
		SetPageAddress(150 + i*5, 200);
		SetColumnAddress(90 - i*5, 90);
		FillSelection(0xFF, 0x0, 0x0);		
	}


	SetPageAddress(200, 250);
	SetColumnAddress(15, 30);
	FillSelection(0xFF, 0x0, 0x0);
	
	SetPageAddress(180, 270);
	SetColumnAddress(30, 90);
	FillSelection(0xFF, 0x0, 0x0);

	for(int i = 0; i < 12; i++){
		SetPageAddress(55 + i*10, 280 - i*10);
		SetColumnAddress(90 + i*10, 100 + i*10);
		FillSelection(0xFF, 0x0, 0x0);
		
	}
}

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
	
	
	
	drawHeart();
		
    /* Replace with your application code */
    while (1) 
    {
		DisplayOff();
		_delay_ms(250);
		DisplayOn();
		_delay_ms(1250);
    }
}

