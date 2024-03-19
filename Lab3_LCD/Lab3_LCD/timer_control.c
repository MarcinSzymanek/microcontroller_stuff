/*
 * timer_control.c
 *
 * Created: 15/03/2024 12.19.09
 *  Author: nastr
 */

#include "timer_control.h"
#include <avr/io.h>

void init_timer(int16_t compare_val){
	TCCR1B = 0;
	
} 

void start_timer(){
	
}

void stop_timer(){
	TCCR1B = 0;
}
