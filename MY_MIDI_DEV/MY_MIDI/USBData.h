// EnumApp_enum_data.h 
// Enumeration tables & HID keyboard data
//
#include "USB_HID_keys.h"

const unsigned char DD[]=	// DEVICE Descriptor
   {0x12,	       	// bLength = 18d
    0x01,			// bDescriptorType = Device (1)
    0x00,0x01,		// bcdUSB(L/H) USB spec rev (BCD)
	0x00,0x00,0x00,	// bDeviceClass, bDeviceSubClass, bDeviceProtocol
	0x20,			// bMaxPacketSize0 EP0 is 64 bytes
	0x6A,0x0B,		// idVendor(L/H)--Maxim is 0B6A
	0x46,0x53,		// idProduct(L/H)--5346
	0x34,0x12,		// bcdDevice--1234
	1,2,3,			// iManufacturer, iProduct, iSerialNumber
	1};			    // bNumConfigurations

const unsigned char CD[]=	// CONFIGURATION Descriptor
   {0x09,			// bLength
	0x02,			// bDescriptorType = Config
	66,0x00,		// wTotalLength(L/H) = 34 bytes
	0x02,			// bNumInterfaces
	0x01,			// bConfigValue
	0x00,			// iConfiguration
	0xE0,			// bmAttributes. b7=1 b6=self-powered b5=RWU supported
	0x01,			// MaxPower is 2 ma
	
// Interface association? descriptor
	//0x08,
	//0x0B,
	//0x00,
	//0x02,
	//0x01,
	//0x03,
	//0x00,
	//0x00,
	
// INTERFACE Descriptor
	0x09,			// length = 9
	0x04,			// type = IF
	0x00,			// bInterfaceNumber IF #0
	0x00,			// bAlternate Setting
	0x00,			// bNum Endpoints
	0x01,			// bInterfaceClass = Audio
	0x01,
	0x00,		// bInterfaceSubClass = Audio Control, bInterfaceProtocol
	0x00,			// iInterface
// Class specific Audio Interface Header 
	0x09, // bLength
	0x24, // bDescriptorType
	0x01, // bDesriptorSubtype (CS_INTERFACE -> HEADER)
	0x00,
	0x01, // bcdADC 1.00
	0x09, 
	0x00, // wTotalLength
	0x01, //binCollection 0x01
	0x01, // baInterfaceNr 1

// MIDI streaming interface
	0x09, // bLength
	0x04, // bDescriptorType (IF)
	0x01, // bInterfaceNumber
	0x00, // bAlternateSetting
	0x01, // bNumEndpoints
	0x01, // bInterfaceClass (Audio)
	0x03, // bInterfaceSubClass (MIDI Streaming) 
	0x00, // bInterfaceProtocol
	0x00, // iInterface (String index)
	
// MIDI Interface Header Descriptor
	0x07, // bLength
	0x24, // bDescType
	0x01, //bDescSubtype (CS_IF -> MS_HEADER)
	0x00, 0x01, // bcdMSC 1.00
	16, 0x00, // wTotalLength 22 : all interface disc + header

// Aúdio Midi out Jack 1 CSinterface desc
	0x09, // bLength
	0x24, // bDescriptorType audio class IF
	0x03, // bDescriptorSubtype midi out jack 
	0x01, // bJackType (Embedded)
	0x12, // bJackId
	0x01,
	0x00,
	0x01,
	0x00, // bStringDescriptorIndex

// ENDPOINTS
// IN Endpoint Descriptor
	0x09,			// bLength
	0x05,			// bDescriptorType (Endpoint)
	0x83,			// bEndpointAddress (IN)		
	0x02,			// bmAttributes	(bulk)
	0x40,0,           // wMaxPacketSize (64)
	0, 0, 0, // bInterval, bRefresh, bSynchAddress : unused
	
// Class-specific MS Bulk OUT endpoint descriptor
	0x05, //bLength
	0x25, // bDescriptorType audio class endpoint
	0x01, // bDescriptorSubtype (CS_ENDPOINT -> MS_GENERAL)
	0x01, // bNumEmbMIDIJack
	0x12, // BaAssocJackID(1)

};			


const unsigned char RepD[]=   // Report descriptor 
	{
	0x05,0x01,		// Usage Page (generic desktop)
	0x09,0x06,		// Usage (keyboard)
	0xA1,0x01,		// Collection
	0x05,0x07,		//   Usage Page 7 (keyboard/keypad)
	0x19,0xE0,		//   Usage Minimum = 224
	0x29,0xE7,		//   Usage Maximum = 231
	0x15,0x00,		//   Logical Minimum = 0
	0x25,0x01,		//   Logical Maximum = 1
	0x75,0x01,		//   Report Size = 1
	0x95,0x08,		//   Report Count = 8
	0x81,0x02,		//  Input(Data,Variable,Absolute)
	0x95,0x01,		//   Report Count = 1
	0x75,0x08,		//   Report Size = 8
	0x81,0x01,		//  Input(Constant)
	0x19,0x00,		//   Usage Minimum = 0
	0x29,0x65,		//   Usage Maximum = 101
	0x15,0x00,		//   Logical Minimum = 0,
	0x25,0x65,		//   Logical Maximum = 101
	0x75,0x08,		//   Report Size = 8
	0x95,0x01,		//   Report Count = 1
	0x81,0x00,		//  Input(Data,Variable,Array)
	0xC0};			// End Collection 

// STRING descriptors. An array of string arrays

const unsigned char strDesc[][64]= {
// STRING descriptor 0--Language string
{
    0x04,			// bLength
	0x03,			// bDescriptorType = string
	0x09,0x04		// wLANGID(L/H) = English-United Sates
},
// STRING descriptor 1--Manufacturer ID
{
    11,			// bLength
	0x03,		// bDescriptorType = string
	'M',0,'a',0,'x',0,'i',0,'m',0 // text in Unicode
}, 
// STRING descriptor 2 - Product ID
{	24,			// bLength
	0x03,		// bDescriptorType = string
	'M',0,'A',0,'X',0,'3',0,'4',0,'2',0,'1',0,'E',0,' ',0,
    'E',0,'n',0,'u',0,'m',0,' ',0,'C',0,'o',0,'d',0,'e',0
},

// STRING descriptor 3 - Serial Number ID
{       20,			// bLength
	0x03,			// bDescriptorType = string
	'S',0,				
	'/',0,
	'N',0,
	' ',0,
	'3',0,
	'4',0,
	'2',0,
	'1',0,
    'E',0,
},

// STRING descriptor 4 - Midi IF string
{
	10,
	0x03,
	'M', 0,
	'I', 0,
	'D', 0,
	'I', 0
	
}
};
const unsigned char Message[]={ // each letter is 3 bytes--shiftcode, 00, HID keycode
	0x00,0x00,KEY_ENTER,     // (cr)
    0x02,0x00,KEY_T,		// T (02 is shift)
	0x00,0x00,KEY_H,		// h
	0x00,0x00,KEY_E,		// e
	0x00,0x00,KEY_SPACE,		// (sp)
	0x02,0x00,KEY_M,		// M
	0x02,0x00,KEY_A,		// A
	0x02,0x00,KEY_X,		// X
	0x00,0x00,KEY_3,		// 3
	0x00,0x00,KEY_4,		// 4
	0x00,0x00,KEY_2,		// 2
	0x00,0x00,KEY_1,		// 1
	0x02,0x00,KEY_E,		// E
	0x00,0x00,KEY_SPACE,		// (sp)
	0x00,0x00,KEY_D,		// d
	0x00,0x00,KEY_O,		// o
	0x00,0x00,KEY_E,		// e
	0x00,0x00,KEY_S,		// s
	0x00,0x00,KEY_SPACE,		// (sp)
	0x02,0x00,KEY_U,		// U
	0x02,0x00,KEY_S,		// S
	0x02,0x00,KEY_B,		// B
	0x02,0x00,KEY_1,		// !
	0x00,0x00,KEY_ENTER};	// (cr)

