/*
 * MAX3421.h
 *
 *  Author: Henning Hargaard
 *  Modified: Michael Alrøe
 */ 
extern unsigned char Suspended;
extern unsigned int msec_timer;
extern unsigned char inhibit_send;  
extern unsigned int blinktimer;

void initMAX3421();
void wreg(unsigned char reg, unsigned char dat);
void check_for_resume();
unsigned char MAX_Int_Pending();
void service_irqs();
unsigned char rreg(unsigned char reg);