/*
 * adc_control.c
 *
 * Created: 14/03/2024 23.20.39
 *  Author: nastr
 */ 
#pragma once

#include "adc_control.h"
#include <avr/io.h>

#define ADCEN 7
//#define ADSC 6

void init_adc(){
	// Choose adc1
	ADMUX = 0b01000001;
	// Enable adc by setting bit 7 in ADCSRA reg
	ADCSRA |= (1 << ADCEN);
	ADCSRB = 0;
}

// First conversion takes 25 clock cycles, subsequent 13
int adc_read_single(){
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA >> ADSC) & 1){};
	int val = ADCW;
	//int val = ADCL;
	//val |= ADCH;
	return val;
}

// Check if conversion value is lower than threshold and return it
// Otherwise, return 0;
int check_value(int threshold){
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA >> ADSC) & 1){};
	int val = ADCW;
	if(val < threshold){
		return val;
	}
	return 0;
}
