 /*
 *  Author: Henning Hargaard
 *  Modified: Michael Alrøe
 */ 
#include <avr/io.h>
#include "USBData.h"
#include "MAX3420E_registers.h"

// Global variables
unsigned char SUD[8];		// Local copy of the 8 setup data read from the MAX3420E SUDFIFO
unsigned char msgidx,msglen;	// Text string in EnumApp_enum_data.h--index and length
unsigned char configval;		// Set/Get_Configuration value
unsigned char ep3stall;		// Flag for EP3 Stall, set by Set_Feature, reported back in Get_Status
unsigned char interfacenum;      // Set/Get interface value
unsigned char inhibit_send;	// Flag for the keyboard character send routine
unsigned char RWU_enabled;       // Set by Set/Clear_Feature RWU request, sent back for Get_Status-RWU
unsigned char Suspended;         // Tells the main loop to look for host resume and RWU pushbutton
unsigned int msec_timer;        // Count off time in the main loop
unsigned int blinktimer;        // Count milliseconds to blink the "loop active" light
unsigned char send3zeros;        // EP3-IN function uses this to send HID (key up) codes between keystrokes

void initSPI()
{
  // MOSI, CLK and SS pins + MAX CS are output 
  DDRB |= 0b00010111;
  // MISO pin is input
  DDRB &= 0b11110111;
  // At Mega2560: MAX Select is at PB4
  // SS high (deselect MAX)
  PORTB |= 0b00010001;
  // Disable SPI interrupts, SPI enable, MSB first, Master, Mode(0,0), f = 125 kHz
  SPCR = 0b01010011;
  SPSR = 0;  
}

// Write a MAX3420E register byte
void wreg(unsigned char reg, unsigned char dat)
{
unsigned char dummy;	
  // Slave select (MAX SS low)
  PORTB &= 0b11101111;
  // Send the register number with the DIR bit (b1) set to WRITE
  SPDR = reg | 0b00000010;
  // Await transfer complete
  while ((SPSR & 0b10000000) == 0)
  {}
  // Clear SPIF	  
  dummy = SPDR;	    
  // Send the data
  SPDR = dat;
  // Await transfer complete
  while ((SPSR & 0b10000000) == 0)
  {}
  // Clear SPIF
  dummy = SPDR;
  // Slave deselect (MAX SS high)
  PORTB |= 0b00010000;
}

// Same as 'wreg' but also set the ACKSTAT bit in the SPI command byte
void wregAS(unsigned char reg, unsigned char dat)
{
unsigned char dummy;
  // Slave select (MAX SS low)
  PORTB &= 0b11101111;
  // Send the register number with the DIR bit (b1) set to WRITE and bit 0 (ACK STATUS) set to 1
  SPDR = reg | 0b00000011;
  // Await transfer complete
  while ((SPSR & 0b10000000) == 0)
  {}
  // Clear SPIF
  dummy = SPDR;
  // Send the data
  SPDR = dat;
  // Await transfer complete
  while ((SPSR & 0b10000000) == 0)
  {}
  // Clear SPIF
  dummy = SPDR;
  // Slave deselect (MAX SS high)
  PORTB |= 0b00010000;
}

// Read a register, return its value.
unsigned char rreg(unsigned char reg)
{
unsigned char dummy;	
  // Slave select (MAX SS low)
  PORTB &= 0b11101111;
  // Send the register number with the DIR bit (b1) reset to READ
  SPDR = reg;
  // Await transfer complete
  while ((SPSR & 0b10000000) == 0)
  {}
  // Clear SPIF
  dummy = SPDR;
  // Dummy send (generate 8 clocks)
  SPDR = 0;
  // Await transfer complete  
  while ((SPSR & 0b10000000) == 0)
  {}  
  // Slave deselect (MAX SS high)
  PORTB |= 0b00010000;
  // Return read data
  return SPDR;  
}

// Same as 'rreg' but also set the ACKSTAT bit
unsigned char rregAS(unsigned char reg)
{
unsigned char dummy;
  // Slave select (MAX SS low)
  PORTB &= 0b11101111;
  // Send the register number with the DIR bit (b1) reset to READ and bit 0 (ACKSTAT) set to 1
  SPDR = reg | 0b00000001;
  // Await transfer complete
  while ((SPSR & 0b10000000) == 0)
  {}
  // Clear SPIF
  dummy = SPDR;
  // Dummy send (generate 8 clocks)
  SPDR = 0;
  // Await transfer complete
  while ((SPSR & 0b10000000) == 0)
  {}
  // Slave deselect (MAX SS high)
  PORTB |= 0b00010000;
  // Return read data
  return SPDR;
}

// Read N MAX3420E FIFO bytes into the array p
void readbytes(unsigned char reg, unsigned char N, unsigned char *p)
{
unsigned char j;
  // Slave select (MAX SS low)
  PORTB &= 0b11101111;
  // Send the register number with the DIR bit (b1) reset to READ
  SPDR = reg;
  // Await transfer complete
  while ((SPSR & 0b10000000) == 0)
  {}  
  // Clear SPIF
  j = SPDR;  
  for(j=0; j<N; j++)
  {
    // Dummy send (generate 8 clocks)
    SPDR = 0;
    // Await transfer complete
    while ((SPSR & 0b10000000) == 0)
    {}
    // store it in the data array   	
    *p = SPDR;
	p++;
  }
  // Slave deselect (MAX SS high)
  PORTB |= 0b00010000;
}

// Write N MAX3420E FIFO bytes into the array p
void writebytes(unsigned char reg, unsigned char N, const unsigned char *p)
{
  // <------------- Write code here (exercise, part 2)
  
  // Slave select (MAX SS low)
  PORTB &= 0b11101111;
  SPDR = reg | 0b00000010;
  while ((SPSR & 0b10000000) == 0)
  {}
  // Read SPSR to clear IF flag
  char j = SPSR;
  char dummy;
  for(j = 0; j < N; j++){
	  // Clear SPIF
	  dummy = SPDR;
	  // Send the data
	  SPDR = *p;
	  // Await transfer complete
	  while ((SPSR & 0b10000000) == 0)
	  {}
	  // Clear SPIF
	  dummy = SPDR;
	  p++;
  }
   // Slave deselect (MAX SS high)
   PORTB |= 0b00010000;
}

// Reset the MAX3420E
void Reset_MAX()
{
unsigned char dummy;
  wreg(rUSBCTL,0x20);	// chip reset
  wreg(rUSBCTL,0x00);	// remove the reset
  do                  // Chip reset stops the oscillator. Wait for it to stabilize.
  {
    dummy = rreg(rUSBIRQ);
	dummy &= bmOSCOKIRQ;
  }
  while (dummy == 0);
}

void EnableIRQs()
{
  wreg(rEPIEN,(bmSUDAVIE+bmIN3BAVIE)); 
  wreg(rUSBIEN,(bmURESIE+bmURESDNIE));	
}

void initMAX3421()
{
  // Set global variables
  ep3stall=0;			// EP3 initially un-halted (no stall) (CH9 testing)
  msgidx = 0;			// start of KB Message[]
  msglen = sizeof(Message);     // so we can check for the end of the message
  inhibit_send = 0x01;		// 0 means send, 1 means inhibit sending
  send3zeros=1;
  msec_timer=0;
  blinktimer=0;
  // software flags
  configval=0;                    // at pwr on OR bus reset we're unconfigured
  Suspended=0;
  RWU_enabled=0;                  // Set by host Set_Feature(enable RWU) request
  // SPI initialization
  initSPI();
  // Always set the FDUPSPI bit in the PINCTL register FIRST if you are using the SPI port in
  // full duplex mode. This configures the port properly for subsequent SPI accesses.
  wreg(rPINCTL,(bmFDUPSPI+bmINTLEVEL+gpxSOF)); // MAX3420: SPI=full-duplex, INT=neg level, GPX=SOF
  Reset_MAX();
  wreg(rGPIO,0x00);                   // LEDs off (Active HIGH)
  // This is a self-powered design, so the host could turn off Vbus while we are powered.
  // Therefore set the VBGATE bit to have the MAX3420E automatically disconnect the D+
  // pullup resistor in the absense of Vbus. Note: the VBCOMP pin must be connected to Vbus
  // or pulled high for this code to work--a low on VBCOMP will prevent USB connection.

   // ----> wreg(rUSBCTL,(bmCONNECT+bmVBGATE));  // VBGATE=1 disconnects D+ pullup if host turns off VBUS
  wreg(rUSBCTL,(bmCONNECT));		// Alrøe: Just connect. The Vbus comparator pin is floating at the USB host shield!
  EnableIRQs();
  wreg(rCPUCTL,bmIE);                 // Enable the INT pin  
}

void check_for_resume()
{
  if(rreg(rUSBIRQ) & bmBUSACTIRQ)     // THE HOST RESUMED BUS TRAFFIC
  {
    L2_OFF
	Suspended=0;                    // no longer suspended
  }
  else if(RWU_enabled)                // Only if the host enabled RWU
  {
	if((rreg(rGPIO)&0x40)==0)       // See if the Remote Wakeup button was pressed
	{
	  L2_OFF                        // turn off suspend light
	  Suspended=0;                  // no longer suspended
	  SETBIT(rUSBCTL,bmSIGRWU)      // signal RWU
	  while ((rreg(rUSBIRQ)&bmRWUDNIRQ)==0)  // spin until RWU signaling done
	  {}	
	  CLRBIT(rUSBCTL,bmSIGRWU)      // remove the RESUME signal
	  wreg(rUSBIRQ,bmRWUDNIRQ);     // clear the IRQ
	  while((rreg(rGPIO)&0x40)==0)
      {}  // hang until RWU button released
      wreg(rUSBIRQ,bmBUSACTIRQ);    // wait for bus traffic -- clear the BUS Active IRQ
      while((rreg(rUSBIRQ) & bmBUSACTIRQ)==0)
      {} // & hang here until it's set again...
    }
  }
}

// Poll the MAX3420E INT pin (set for active low level)
unsigned char MAX_Int_Pending()
{
  // Henning Hargaard: The interrupt pin is connected to PORT H, pin 6
  return ((PINH & 0b01000000) == 0 );
}

void send_descriptor()
{
unsigned int reqlen,sendlen,desclen;
const unsigned char *pDdata;					// pointer to ROM Descriptor data to send
  //
  // NOTE This function assumes all descriptors are 64 or fewer bytes and can be sent in a single packet
  desclen = 0;					// check for zero as error condition (no case statements satisfied)
  reqlen = SUD[wLengthL] + 256*SUD[wLengthH];	// 16-bit
  switch (SUD[wValueH])			// wValueH is descriptor type
  {
	case GD_DEVICE:
      desclen = DD[0];	// descriptor length
      pDdata = DD;
      break;
	case GD_CONFIGURATION:
	  desclen = CD[2];	// Config descriptor includes interface, HID, report and ep descriptors
      pDdata = CD;
	  break;
	case GD_STRING:
	  desclen = strDesc[SUD[wValueL]][0];   // wValueL=string index, array[0] is the length
      pDdata = strDesc[SUD[wValueL]];       // point to first array element
	  break;
	case GD_HID:
	  desclen = CD[18];
	  pDdata = &CD[18];
      break;
	case GD_REPORT:
	  desclen = CD[25];
      pDdata = RepD;
      break;
  }
  if (desclen !=0 )                   // one of the case statements above filled in a value
  {
    sendlen = (reqlen <= desclen) ? reqlen : desclen; // send the smaller of requested and available
	writebytes(rEP0FIFO,sendlen,pDdata);
	wregAS(rEP0BC,sendlen);   // load EP0BC to arm the EP0-IN transfer & ACKSTAT
  }
  else
    STALL_EP0  // none of the descriptor types match
}

// **********************************************************************************************
// FUNCTION: Set/Get_Feature. Call as feature(1) for Set_Feature or feature(0) for Clear_Feature.
// There are two set/clear feature requests:
//	To a DEVICE: 	Remote Wakeup (RWU).
//  	To an ENDPOINT:	Stall (EP3 only for this app)
//
void feature(unsigned char sc)
{
  unsigned char mask;
  if((SUD[bmRequestType]==0x02)	// dir=h->p, recipient = ENDPOINT
	&&  (SUD[wValueL]==0x00)	// wValueL is feature selector, 00 is EP Halt
	&&  (SUD[wIndexL]==0x83))	// wIndexL is endpoint number IN3=83
  {
    mask=rreg(rEPSTALLS);   // read existing bits
    if(sc == 1)               // set_feature
    {
      mask += bmSTLEP3IN;       // Halt EP3IN
      ep3stall=1;
    }
    else                        // clear_feature
    {
      mask &= ~bmSTLEP3IN;      // UnHalt EP3IN
      ep3stall=0;
      wreg(rCLRTOGS,bmCTGEP3IN);  // clear the EP3 data toggle
    }
    wreg(rEPSTALLS,(mask|bmACKSTAT)); // Don't use wregAS for this--directly writing the ACKSTAT bit
  }
  else if ((SUD[bmRequestType]==0x00)	// dir=h->p, recipient = DEVICE
	&&  (SUD[wValueL]==0x01))	// wValueL is feature selector, 01 is Device_Remote_Wakeup
  {
    RWU_enabled = sc<<1;	// =2 for set, =0 for clear feature. The shift puts it in the get_status bit position.
    rregAS(rFNADDR);		// dummy read to set ACKSTAT
  }
  else STALL_EP0
}

void get_status()
{
unsigned char testbyte;
  testbyte=SUD[bmRequestType];
  switch(testbyte)
  {
    case 0x80: 			// directed to DEVICE
      wreg(rEP0FIFO,(RWU_enabled+1));	// first byte is 000000rs where r=enabled for RWU and s=self-powered.
      wreg(rEP0FIFO,0x00);		// second byte is always 0
      wregAS(rEP0BC,2); 		// load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
      break;
    case 0x81: 			// directed to INTERFACE
      wreg(rEP0FIFO,0x00);		// this one is easy--two zero bytes
      wreg(rEP0FIFO,0x00);
      wregAS(rEP0BC,2); 		// load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
      break;
    case 0x82: 			// directed to ENDPOINT
      if(SUD[wIndexL] == 0x83)		// We only reported ep3, so it's the only one the host can stall IN3=83
      {
        wreg(rEP0FIFO,ep3stall);	// first byte is 0000000h where h is the halt (stall) bit
        wreg(rEP0FIFO,0x00);		// second byte is always 0
        wregAS(rEP0BC,2); 		// load byte count, arm the IN transfer, ACK the status stage of the CTL transfer
        break;
      }
      else
        STALL_EP0		// Host tried to stall an invalid endpoint (not 3)
    default:
      STALL_EP0		// don't recognize the request
  }
}

void set_interface()	// All we accept are Interface=0 and AlternateSetting=0, otherwise send STALL
{
unsigned char dumval;
  if((SUD[wValueL] == 0)		// wValueL=Alternate Setting index
	&&(SUD[wIndexL] == 0))		// wIndexL=Interface index
    dumval=rregAS(rFNADDR);	// dummy read to set the ACKSTAT bit
  else
    STALL_EP0
}

void get_interface()	// Check for Interface=0, always report AlternateSetting=0
{
  if(SUD[wIndexL] == 0)		// wIndexL=Interface index
  {
    wreg(rEP0FIFO,0);		// AS=0
    wregAS(rEP0BC,1);		// send one byte, ACKSTAT
  }
  else
    STALL_EP0
}

void get_configuration()
{
  wreg(rEP0FIFO,configval);         // Send the config value
  wregAS(rEP0BC,1);
}

void set_configuration()
{
  configval =SUD[wValueL];           // Store the config value
  if(configval != 0)                // If we are configured,
    SETBIT(rUSBIEN,bmSUSPIE);       // start looking for SUSPEND interrupts
  rregAS(rFNADDR);                  // dummy read to set the ACKSTAT bit
}

void std_request()
{
  switch(SUD[bRequest])
  {
	case SR_GET_DESCRIPTOR:
	  send_descriptor();
	   break;
	case SR_SET_FEATURE:
	  feature(1);
	  break;
	case SR_CLEAR_FEATURE:
	  feature(0);
	  break;
    case SR_GET_STATUS:	
	  get_status();
	  break;
	case SR_SET_INTERFACE:
	  set_interface();
	  break;
	case SR_GET_INTERFACE:
	  get_interface();
	  break;
	case SR_GET_CONFIGURATION:
	  get_configuration();
	  break;
	case SR_SET_CONFIGURATION:
	  set_configuration();
	  break;
	case SR_SET_ADDRESS:
	  rregAS(rFNADDR);
	  break;  // discard return value
	default: 
	  STALL_EP0
  }
}

void class_request()
{
  STALL_EP0
}

void vendor_request()
{
  STALL_EP0
}

void do_SETUP()
{
  readbytes(rSUDFIFO,8,SUD);          // got a SETUP packet. Read 8 SETUP bytes
  switch(SUD[bmRequestType] & 0x60)     // Parse the SETUP packet. For request type, look only at b6&b5
  {
    case 0x00:
	  std_request();
	  break;
	case 0x20:
	  class_request();
	  break;  // just a stub in this program
	case 0x40:
	  vendor_request();
	  break;  // just a stub in this program
	default:
	  STALL_EP0                       // unrecognized request type
  }
}

// Send keyboard characters over Endpoint 3-IN
void do_IN3()
{
  if (inhibit_send == 0x01)
  {
    wreg(rEP3INFIFO,0);			// send the "keys up" code
    wreg(rEP3INFIFO,0);
    wreg(rEP3INFIFO,0);
  }
  else
  if (send3zeros == 0x01)                         // precede every keycode with the "no keys" code
  {
    wreg(rEP3INFIFO,0);			// send the "keys up" code
    wreg(rEP3INFIFO,0);
    wreg(rEP3INFIFO,0);
    send3zeros=0;                           // next time through this function send the keycode
  }
  else
  {
    send3zeros=1;
    wreg(rEP3INFIFO,Message[msgidx++]);	// load the next keystroke (3 bytes)
    wreg(rEP3INFIFO,Message[msgidx++]);
    wreg(rEP3INFIFO,Message[msgidx++]);
    if(msgidx >= msglen)                    // check for message wrap
    {
      msgidx = 0;
      L0_OFF
      inhibit_send=1;                     // send the string once per pushbutton press
    }
  }
  wreg(rEP3INBC,3);				// arm it
}

void service_irqs()
{
unsigned char itest1,itest2;

  itest1 = rreg(rEPIRQ);            // Check the EPIRQ bits
  itest2 = rreg(rUSBIRQ);           // Check the USBIRQ bits
  if(itest1 & bmSUDAVIRQ)
  {
    wreg(rEPIRQ,bmSUDAVIRQ);     // clear the SUDAV IRQ
    do_SETUP();
  }
  if(itest1 & bmIN3BAVIRQ)          // Was an EP3-IN packet just dispatched to the host?
  {
 	do_IN3();                     // Yes--load another keystroke and arm the endpoint
  }                             // NOTE: don't clear the IN3BAVIRQ bit here--loading the EP3-IN byte
  // count register in the do_IN3() function does it.
  if((configval != 0) && (itest2&bmSUSPIRQ))   // HOST suspended bus for 3 msec
  {
    wreg(rUSBIRQ,(bmSUSPIRQ+bmBUSACTIRQ));  // clear the IRQ and bus activity IRQ
    L2_ON                         // turn on the SUSPEND light
    L3_OFF                        // turn off blinking light (in case it's on)
    Suspended=1;                  // signal the main loop
  }
  if(rreg(rUSBIRQ) & bmURESIRQ)
  {
    L1_ON                         // turn the BUS RESET light on
    L2_OFF                        // Suspend light off (if on)
    wreg(rUSBIRQ,bmURESIRQ);      // clear the IRQ
  }
  if(rreg(rUSBIRQ) & bmURESDNIRQ)
  {
    L1_OFF                        // turn the BUS RESET light off
    wreg(rUSBIRQ,bmURESDNIRQ);    // clear the IRQ bit
    Suspended = 0;                  // in case we were suspended
	EnableIRQs();                   // ...because a bus reset clears the IE bits
  }
}