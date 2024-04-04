/******************************************
AMS, LAB3
Testing the LCD driver "lcd162.c"

Henning Hargaard
Modified Michael Alrøe
*****************************************/
#include <avr/io.h>

#define F_CPU 16000000
#include <util/delay.h>

#include "lcd162.h"

#define WAIT_1S _delay_ms(1000)
#define WAIT_2S _delay_ms(2000)

// User defined characters : Æ Ø Å æ ø å (special danish characters)
const unsigned char capital_ae[8]= {
	0b0001111,
	0b0010100,
	0b0010100,
	0b0011111,
	0b0010100,
	0b0010100,
	0b0010111,
0b0000000};
const unsigned char capital_oe[8]= {
	0b0001110,
	0b0010001,
	0b0010011,
	0b0010101,
	0b0011001,
	0b0010001,
	0b0001110,
0b0000000};
const unsigned char capital_aa[8]= {
	0b0000100,
	0b0001110,
	0b0010001,
	0b0011111,
	0b0010001,
	0b0010001,
	0b0010001,
0b0000000};
const unsigned char minor_ae[8]= {
	0b0000000,
	0b0000000,
	0b0001110,
	0b0010101,
	0b0010101,
	0b0010101,
	0b0001111,
0b0000000};
const unsigned char minor_oe[8]= {
	0b0000000,
	0b0000000,
	0b0001110,
	0b0010011,
	0b0010101,
	0b0011001,
	0b0001110,
0b0000000};
const unsigned char minor_aa[8]= {
	0b0000100,
	0b0000000,
	0b0001110,
	0b0010010,
	0b0010010,
	0b0010010,
	0b0001101,
0b0000000};

int summmain()
{
	char SRAMstring[] = "String in SRAM";
	int i;
	// Initialize the display
	LCDInit();

	// Loading UDC (CGRAM) with the dot patterns for Æ,Ø,Å,æ,ø,å
	LCDLoadUDC(0,capital_ae);
	LCDLoadUDC(1,capital_oe);
	LCDLoadUDC(2,capital_aa);
	LCDLoadUDC(3,minor_ae);
	LCDLoadUDC(4,minor_oe);
	LCDLoadUDC(5,minor_aa);

	while (1)
	{
		// Display clear
		LCDClear();
		// Goto line 1, 3.character
		LCDGotoXY(2,0);
		// Testing LCD_DispString()
		LCDDispString("X=2, Y=0", 8);
		WAIT_2S;

		// Display clear
		LCDClear();
		// Goto line 2, 4.character
		LCDGotoXY(3,1);
		LCDDispString("X=3, Y=1", 8);
		WAIT_2S;

		// Display clear
		LCDClear();
		// Testing LCDDispChar() and the UDC characters
		LCDDispChar('A');
		LCDDispChar('B');
		LCDDispChar('C');
		LCDDispChar(' ');
		LCDDispChar(0);
		LCDDispChar(1);
		LCDDispChar(2);
		LCDDispChar(3);
		LCDDispChar(4);
		LCDDispChar(5);

		// Goto line 2 start
		LCDGotoXY(0,1);
		// Testing LCDDispString()
		LCDDispString(SRAMstring, sizeof(SRAMstring));
		WAIT_2S;

		// Visible cursor
		LCDOnOffControl(1,0);
		WAIT_1S;
		// Goto line 2, 4.character
		LCDGotoXY(3,1);
		// Blinking characters
		LCDOnOffControl(0,1);
		WAIT_2S;
		// Blinking character with visible cursor
		LCDOnOffControl(1,1);
		WAIT_2S;

		// Move cursor 2 positions to the left
		LCDCursorLeft();
		LCDCursorLeft();
		_delay_ms(1000);

		// Move cursor 4 positions to the right
		LCDCursorRight();
		LCDCursorRight();
		LCDCursorRight();
		LCDCursorRight();
		WAIT_1S;

		// Shift the display text 2 positions to the right
		LCDShiftRight();
		LCDShiftRight();
		WAIT_1S;
		// Shift the display text 1 position to the left
		LCDShiftLeft();
		WAIT_1S;

		// No blinking characters or cursor
		LCDOnOffControl(0,0);
		// Display clear
		LCDClear();
		// Testing LCDDispInteger()
		for ( i = 0; i < 10; i++ )
		{
			// Goto line 1, 5.character
			LCDGotoXY(4,0);
			LCDDispInteger(i-30000);
			// Goto line 2, 5.character
			LCDGotoXY(4,1);
			LCDDispInteger(i+30000);
			_delay_ms(500);
		}
		// Testing setBacklight()
		for ( i = 0; i <= 100; i++ )
		{
			setBacklight(i);
			_delay_ms(40);
		}
		for ( i = 0; i <= 100; i++ )
		{
			setBacklight(100-i);
			_delay_ms(40);
		}
		setBacklight(25);
		// Testing the 5 keys
		// Display clear
		LCDClear();
		while (readKeys() != 1)
		{
			LCDClear();
			LCDDispChar(readKeys()+'0');
			_delay_ms(100);
		}
	}
}