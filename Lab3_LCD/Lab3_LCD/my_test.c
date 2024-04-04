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

#define MEASUREMENT_COUNT 5

void measure_velocity(int* val, int threshold){
	if(*val > threshold) return;
	int temp = *val;
	LCDGotoXY(0, 1);
	LCDDispString("***", 3);
	int lowest = temp;
	int avg = temp;
	int it = 0;
	int measurements[MEASUREMENT_COUNT];
	memset(measurements, 0, sizeof(measurements));
	
	// Take avg of 20 measurements
	while(it < MEASUREMENT_COUNT){
		
		temp = adc_read_single();
		if(temp == 1024){
			continue;
		}
		measurements[it] = temp;
		//if(temp > threshold + 50){
			//break;
		//}
		avg += temp;
		avg /= 2;
		if(temp < lowest) lowest = temp;
		it++;
		_delay_us(100);
	}
	LCDGotoXY(0, 1);
	LCDDispString("   ", 3);
	*val = avg;
	
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
	int cursorx = 0;
	int cursory = 0;
	int threshold = 900;
	while(1){
		int val = adc_read_single();
		
		if(val < threshold){
			print = 1;
			int* p = &val;
			measure_velocity(p, threshold);
		}
		
		if(val < threshold){
			cursorx += 5;
			LCDGotoXY(cursorx, cursory);
			it++;
			if(it == 3){
				cursory = 1;
				cursorx = 4;
				LCDGotoXY(cursorx, cursory);			
			}
			else if(it > 5){
				LCDClear();
				cursorx = 0;
				cursory = 0;
				it = 0;
			}
			LCDDispInteger(val);
			_delay_ms(100);
			//LCDCursorRight();
			print = 0;
		}
	}
	
	//LCDDispChar('B');
	return 0;
}