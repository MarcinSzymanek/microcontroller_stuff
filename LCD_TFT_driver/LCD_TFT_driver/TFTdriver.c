/************************************************************
File name: "TFTdriver.c"

Driver for "ITDB02 320 x 240 TFT display module, Version 2"
mounted at "ITDB02 Arduino Mega2560 Shield".
Display controller = ILI 9341.

Max. uC clock frequency = 16 MHz (Tclk = 62,5 ns)

Connections:
DB15-DB8:   PORT A
DB7-DB0:    PORT C

RESETx:     PORT G, bit 0
CSx:        PORT G, bit 1
WRx:        PORT G, bit 2
RS (=D/Cx): PORT D, bit 7

Henning Hargaard
Modified Michael Alrøe
************************************************************/
#include <avr/io.h>

#define MAX_RB 0b00011111
#define MAX_G 0b00111111
#define F_CPU 16000000
#include <util/delay.h>

#include <avr/cpufunc.h>  // _NOP()
#include "TFTdriver.h"

// Data port definitions:
#define DATA_PORT_HIGH PORTA
#define DATA_PORT_LOW  PORTC

// Control port definitions:
// PG0 = reset
// PG1 = CS
// PG2 = WR
// PD7 = RS

#define WR_PORT PORTG
#define WR_BIT 2
#define DC_PORT PORTD
#define DC_BIT  7  //"DC" signal is at the shield called RS
#define CS_PORT PORTG
#define CS_BIT  1
#define RST_PORT PORTG
#define RST_BIT 0

// LOCAL FUNCTIONS /////////////////////////////////////////////////////////////

// ILI 9341 data sheet, page 238
void WriteCommand(unsigned char command)
{
	
}

// ILI 9341 data sheet, page 238
// We use 8-bit mcu so maybe don't use unsigned int
void WriteData(unsigned char data_low, unsigned char data_high)
{
	DATA_PORT_LOW = data_low;
	DATA_PORT_HIGH = data_high;
	_NOP();
	WR_PORT &= ~(1 << WR_BIT);
	_NOP();
	WR_PORT |= (1 << WR_BIT);
}

// PUBLIC FUNCTIONS ////////////////////////////////////////////////////////////

// Initializes (resets) the display
void DisplayInit()
{
	DDRA = 0xFF;
	DDRC = 0xFF;
	DDRD = 0b10000000;
	DDRG = 0b00000111;
	
	DC_PORT &= ~(1 << DC_BIT);
	
	CS_PORT &= ~(1 << CS_BIT);
	RST_PORT |= (1 << RST_BIT);
	_delay_ms(250);
	RST_PORT &= ~(1 << RST_BIT);
	_delay_ms(250);
	RST_PORT |= (1 << RST_BIT);
	_delay_ms(250);
	WriteData(1, 0);
	_delay_ms(25);
	
	// Set pixel format
	WriteData(0b00111010, 0);
	DC_PORT |= (1 << DC_BIT);
	WriteData(0b01010101, 0);
	DC_PORT &= ~(1 << DC_BIT);
}

void SetGamma(unsigned char value)
{
	
	
	WriteData(0b00100110, 0);
	
	DC_PORT |= (1 << DC_BIT);	
	WriteData(value, 0);
	DC_PORT &= ~(1 << DC_BIT);
}

void DisplayOff()
{
	WriteData(0b00101000, 0);
}

void DisplayOn()
{
	WriteData(0b00101001, 0);
}

void SleepOut()
{
	WriteData(0b00010001, 0);
	_delay_ms(10);
}

void MemoryAccessControl(unsigned char parameter)
{
}

void InterfacePixelFormat(unsigned char parameter)
{
}

void MemoryWrite()
{
}


// Red 0-31, Green 0-63, Blue 0-31
void WritePixel(unsigned char Red, unsigned char Green, unsigned char Blue)
{
	if(Red > MAX_RB) Red = MAX_RB;
	if(Blue > MAX_RB) Blue = MAX_RB;
	if(Green > MAX_G) Green = MAX_G;
	
	WriteData(0b00101100, 0);
	
	DC_PORT |= (1 << DC_BIT);
	
	unsigned char data_high = (Red << 3);
	unsigned char data_low = Blue;
	//unsigned char g_high = (Green >> 3);
	//unsigned char g_low = (Green & 0b00000111) << 5;
	data_high = 255;
	data_low = 255;
	
	WriteData(data_low, data_high);
	
	DC_PORT &= ~(1 << DC_BIT);
}

// Set Column Address (0-239), Start > End
void SetColumnAddress(unsigned int Start, unsigned int End)
{
}

// Set Page Address (0-319), Start > End
void SetPageAddress(unsigned int Start, unsigned int End)
{
}

// Fills rectangle with specified color
// (StartX,StartY) = Upper left corner. X horizontal (0-319) , Y vertical (0-239).
// Height (1-240) is vertical. Width (1-320) is horizontal.
// R-G-B = 5-6-5 bits.
void FillRectangle(unsigned int StartX, unsigned int StartY, unsigned int Width, unsigned int Height, unsigned char Red, unsigned char Green, unsigned char Blue)
{
	if(Red > MAX_RB) Red = MAX_RB;
	if(Blue > MAX_RB) Blue = MAX_RB;
	if(Green > MAX_G) Green = MAX_G;
	
	WriteData(0b00101100, 0);
	
	DC_PORT |= (1 << DC_BIT);
	
	unsigned char data_high = (Blue << 3);
	unsigned char data_low = Red;
	unsigned char g_high = (Green >> 3);
	unsigned char g_low = (Green & 0b00000111) << 5;
	data_high |= g_high;
	data_low |= g_low;
	
	
	for(int i = 0; i < Width; i++){
		for (int j = 0; j < Height; j++){
			WriteData(data_low, data_high);
		}
	}

	DC_PORT &= ~(1 << DC_BIT);
}