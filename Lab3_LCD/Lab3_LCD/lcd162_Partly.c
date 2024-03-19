/*-------------------------------------------------------------------
  File name: "lcd162.c"

  Driver for "LCD Keypad Shield" alphanumeric display.
  Display controller = HD44780U (LCD-II).
  
  Max. uC clock frequency = 16 MHz (Tclk = 62,5 ns)

  Connection : PORTx (4 bit mode) :
  [LCD]        [Portx]
  RS   ------  PH 5
  RW   ------  GND
  E    ------  PH 6
  DB4  ------  PG 5
  DB5  ------  PE 3
  DB6  ------  PH 3
  DB7  ------  PH 4

  Henning Hargaard
  Modified Michael Alrøe
---------------------------------------------------------------------*/
#include <avr/io.h>

#define F_CPU 16000000
#define E_FLAG 0b01000000
#define RS_FLAG 0b00100000

#include <util/delay.h>

#include <avr/cpufunc.h>  // Enabling macro _NOP() to insert the NOP instruction
#include <stdlib.h>  // Enabling itoa()

#include "lcd162.h"

static int position = 0;

//*********************** PRIVATE (static) operations *********************
static void waitBusy()
{
  // To be implemented
  // To not be implemented for our current shield
}  

// Set enable flag, wait for timing req and disable it.
static void pulse_E()
{
	PORTH |= E_FLAG;
	
	for(int i = 0; i < 100; i++){
		_NOP();
	}	
	PORTH &= ~E_FLAG;
	for(int i = 0; i < 100; i++){
		_NOP();
	}
}

// Sets the display data pins according to the 4 lower bits of data
static void set4DataPins(unsigned char data)
{
	PORTH = (PORTH & 0b11100111) | ((data<<1) & 0b00011000);
	PORTE = (PORTE & 0b11110111) | ((data<<2) & 0b00001000);
	PORTG = (PORTG & 0b11011111) | ((data<<5) & 0b00100000);  
}

static void sendInstruction(unsigned char data)
{  
	// Make sure RS_FLAG is down - indicates we do not send DDRAM data, but an instruction   
	PORTH &= ~RS_FLAG;
	// wait > 40ns
	_NOP();
	_NOP();
	set4DataPins(data >> 4);
	_NOP();
	_NOP();
	pulse_E();
	_NOP();
	_NOP();
	set4DataPins(data);
	_NOP();
	_NOP();
	pulse_E();
	_delay_us(1000);
}

static void sendData(unsigned char data)
{      
	// Set RS_FLAG to indicate we send DDRAM data
	PORTH |= RS_FLAG;
	_NOP();
	set4DataPins(data >> 4);
	_NOP();
	pulse_E();
	set4DataPins(data);
	_NOP();
	pulse_E();
	
}

//*********************** PUBLIC functions *****************************

// Initializes the display, blanks it and sets "current display position"
// at the upper line, leftmost character (cursor invisible)
// Reference: Page 46 in the HD44780 data sheet
void LCDInit()
{
  // Initializing the used port
  DDRH |= 0b01111000;  // Outputs
  DDRE |= 0b00001000;
  DDRG |= 0b00100000;
  
  // Wait 50 ms (min. 15 ms demanded according to the data sheet)
  _delay_ms(50);
  // Function set (still 8 bit interface)
  PORTG |= 0b00100000;
  PORTE |= 0b00001000;
  pulse_E();
  
  // Wait 10 ms (min. 4,1 ms demanded according to the data sheet)
  _delay_ms(10);
  // Function set (still 8 bit interface)
  pulse_E();

  // Wait 10 ms (min. 100 us demanded according to the data sheet)
  _delay_ms(10);
  // Function set (still 8 bit interface)
  pulse_E();

  // Wait 10 ms (min. 100 us demanded according to the data sheet)
  _delay_ms(10);
  // Function set (now selecting 4 bit interface !)
  PORTG &= 0b11011111;
  pulse_E();

  // Function Set : 4 bit interface, 2 line display, 5x8 dots
  sendInstruction( 0b00101000 );
  // Display, cursor and blinking OFF
  sendInstruction( 0b00001000 );
  // Clear display and set DDRAM adr = 0	
  sendInstruction( 0b00000001 );
  // By display writes : Increment cursor / no shift
  sendInstruction( 0b00000110 );
  // Display ON, cursor and blinking OFF
  sendInstruction( 0b00001100 );
}

// Blanks the display and sets "current display position" to
// the upper line, leftmost character
void LCDClear()
{
  sendInstruction(0b00000001);
  _delay_ms(1);
}

// Sets DDRAM address to character position x and line number y
void LCDGotoXY( unsigned char x, unsigned char y )
{
	// Display is 2 lines x 16 chars so make sure we don't go overboard
	if(y > 1 || x > 15)  return;
	
	// Position 
	int rampos = y * 0x40 + x;
	
	sendInstruction(rampos | 0b10000000);	
}

// Display "ch" at "current display position"
void LCDDispChar(char ch)
{
  sendData(ch);
}

// Displays the string "str" starting at "current display position"
void LCDDispString(char* str, int len)
{
	if(len > 32){
		return;
	}
	
	for(int i = 0; i < len; i++){
		LCDDispChar(*str);
		str++;
	}
	
}

// Displays the value of integer "i" at "current display position"
// Max value is +- 32767
void LCDDispInteger(int number)
{
	if(number > 32767 || number < -32767) return;
	int len = 0;
	char string[8];
	char reversed[8];
	int cpy = number;
	if(number < 0){
		cpy = cpy * -1;
	}
	

	char c;
	for(int i = 0; i < 8; i++){
		string[i] = 0;
	}
	while(cpy != 0){
		if (cpy < 10){
			c = '0' + cpy;
			string[len] = c;
			if(number < 0){
				string[len + 1] = '-';
				len++;
			}
			break;
		}
		c = cpy % 10 + '0';
		string[len] = c;
		cpy /= 10;
		len++;
	}
	
	for(int i = len; i > -1; i--){
		if(string[i] > 0){
			reversed[len - i] = string[i];
		}
	}
	LCDDispString(reversed, len + 1);
	
	
  // Just convert the int to a string
}

// Loads one of the 8 user definable characters (UDC) with a dot-pattern,
// pre-defined in an 8 byte array in FLASH memory
void LCDLoadUDC(unsigned char UDCNo, const unsigned char *UDCTab)
{
  // To be implemented		
}

// Selects, if the cursor has to be visible, and if the character at
// the cursor position has to blink.
// "cursor" not 0 => visible cursor.
// "blink" not 0 => the character at the cursor position blinks.
void LCDOnOffControl(unsigned char cursor, unsigned char blink)
{
  // To be implemented
  char data = 0b00001000;
  if(cursor){
	  data |= (1 << 2);
  }
  if(blink){
	  data |= (1 << 1);
  }
  sendInstruction(data);
  _delay_ms(1);
}

// Moves the cursor to the left
void LCDCursorLeft()
{
  // To be implemented
}

// Moves the cursor to the right
void LCDCursorRight()
{
  // To be implemented
  sendInstruction(0b00010100);
}

// Moves the display text one position to the left
void LCDShiftLeft()
{
  // To be implemented
}

// Moves the display text one position to the right
void LCDShiftRight()
{
  // To be implemented
}

// Sets the backlight intensity to "percent" (0-100)
void setBacklight(unsigned char percent)
{
  // To be implemented
}

// Reads the status for the 5 on board keys
// Returns 0, if no key pressed
unsigned char readKeys()
{
  // To be implemented
}