/*
 * my_test.c
 *
 * Created: 13/02/2024 19.23.21
 *  Author: nastr
 */ 
#include <avr/io.h>

#define F_CPU 16000000
#include <util/delay.h>

#include "lcd162.h"
#include "adc_control.h"

#define WAIT_1S _delay_ms(1000)
#define WAIT_2S _delay_ms(2000)

void beNiceToMyGf(){
	LCDClear();
	_delay_ms(50);
	LCDDispString("Hello Marchelka!", 16);
	LCDGotoXY(0, 1);
	LCDDispInteger(40000);
	_delay_ms(2000);
	for(int i = 0; i < 5; i++){
		showLove();
	}
}

void showLove(){
	char luv1[] = "<3 <3 <3 <3 <3";
	char luv2[] = " <3 <3 <3 <3";
	LCDClear();
	_delay_ms(50);
	LCDDispString(luv1, 14);
	LCDGotoXY(0, 1);
	LCDDispString(luv2, 12);
	_delay_ms(500);
}

void measure_velocity(int* val, int threshold){
	if(*val > threshold) return;
	int temp = *val;
	LCDGotoXY(0, 1);
	LCDDispString("***", 3);
	int lowest = temp;
	temp = adc_read_single();
	while(temp <= lowest){
		temp = adc_read_single();
		if(temp < lowest) lowest = temp;
	}
	*val = lowest;
	_delay_ms(500);
	
}

int main(void){
	char SRAMstring[] = "String in SRAM";
	int i;
	init_adc();
	// Initialize the display
	LCDInit();
	LCDDispChar('A');
	LCDGotoXY(5, 0);
	LCDDispChar('B');
	LCDGotoXY(0, 1);
	LCDDispChar('C');
	LCDGotoXY(8, 1);
	LCDDispString("Hello", 5);
	LCDClear();
	int val;
	val = adc_read_single();
	LCDDispInteger(val);
	LCDCursorRight();
	int it = 0;
	LCDOnOffControl(1, 0);
	char print = 0;
	while(1){
		int val = adc_read_single();
		
		if(val < 1000){
			print = 1;
			int* p = &val;
			measure_velocity(p, 1000);
		}
		
		if(val < 1000){
			LCDClear();
			LCDDispInteger(val);
			LCDCursorRight();
			print = 0;
		}
	}
	
	//LCDDispChar('B');
	return 0;
}