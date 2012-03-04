/*

GemClub Memo - Application

 DISCLAIMER: 
NOT FOR DISTRIBUTION
	This file is not being created for the SOSSE project. This file is only to be used for research purposes
	and not on any system that you DO NOT OWN! You have been warned, I am not responsible for your stupidity
	in not following these directions.

IMPORTANT: Data is stored in the card MSB first (D3,D2,D1,D0) but the data in commands must be LSB first (D0,D1,D2,D3)

1  2  3  4  5  6        7               8
80 BE A0 04 04 BE 	00 00 00 80 	90 00			// Read 4th Byte of Issuer Area				80 00 00 00	
80 BE A0 39 04 BE 	00 00 00 00 	90 00			// Read CSC 1 Ratification Counter			00 00 00 00
80 BE A0 02 04 BE 	23 00 00 00 	90 00			// Read 2nd Byte of Issuer Area				00 00 00 23
80 BE A0 01 04 BE 	24 D2 C7 04 	90 00			// Read 1st Byte of Issuer Area				04 C7 D2 24		Card Serial: 80204324
80 20 A0 39 04 20 	CC BC D2 21 	90 00			// Verify CSC 1 (Auth)					21 D2 BC CC
80 BE A0 05 04 BE 	19 00 00 BB 	90 00			// Read Access Conditions/Protected Area		BB 00 00 19					
80 BE A0 10 04 BE 	85 00 00 00 	90 00			// Read 10th byte of User Area 1			00 00 00 85
80 BE A0 08 04 BE 	00 00 00 00 	90 00			// Read CTC 1						00 00 00 00		CTC 1 is 0
80 BE A0 0C 04 BE 	D0 07 00 00 	90 00			// Read Balance 1 (A1)					00 00 07 D0		Current Balance is $20.00
80 BE A0 0E 04 BE 	5C 61 E4 2A 	90 00			// Read Balance 1 (A2)					2A E4 61 5C

80 BE A0 0C 04 BE 	D0 07 00 00 	90 00			// Read Balance 1 (A1)					00 00 07 D0		Current Balance is $20.00
80 DE A0 0C 04 DE 	B7 07 E4 2A 	90 00			// Update Balance 1 (A1)				2A E4 07 B7		New Balance is $19.75
80 DE A0 0E 04 DE 	01 D6 61 E6 	90 00			// Update Balance 1 (A2)				E6 61 D6 01

COLUMN 1 - START BYTE (0x80)
COLUMN 2 - Read,Update,Verify (0xBE,DE,20)
COLUMN 3 - Always A0 for some reason? TODO
COLUMN 4 - Memory Byte (0x04 means perform action from COLUMN 2 on this byte of data)
COLUMN 5 - Length Byte (0x04, Expect 4 bytes of data)
COLUMN 6 - Read,Update,Verify (Not sure why this is sent twice?) TODO
COLUMN 7 - 4 Byte response from TARGET (EX: PPA CARD)
COLUMN 8 - Response WORD (EX: 0x90 00 means successful transaction) 

QUESTION: When is the CSN sent?
ANSWER: In response to READ 1ST BYTE OF ISSUER AREA - 80BEA00104BE<4BYTECSN>

QUESTION: How is CSC 1 verified? Who is contacted first? 
ANSWER: CSC 1 is SENT BY THE METER (CSN is input, <performs function>) card responds with 90 00

QUESTION: How does the meter generate CSC 1? 
TODO ANSWER: ?

QUESTION: How is ISSUER and USER MODE set?
ANSWER:	(via GC Memo Tech Specs document) A logical fuse is "blown" to go from ISSUER MODE to USER MODE. In practice, this fuse is two "mode" bits in the Issuer Area

	     	Mode Bits	Mode
		00b		Not Allowed - card permanently blocked
		01b		Issuer Mode
		10b		User Mode
		11b		Not Allowed - card permanently blocked

Apparently the change from Issuer to User Mode is "irreversible" TODO (Verify this statement)	

QUESTION: How does the card LOCK the EEPROM after Ratification Counter reaches 4? 
TODO ANSWER: ?

QUESITON: Can CSC 0 be presented to a GC_MEMO card in the field, that is in User Mode? If so, what happens? 
ANSWER: YES, CSC 0 can unlock all PROTECTED AREAS during any mode.

QUESTION: How can we sniff CSC 0? 
ANSWER: CSC 0 can NOT be read in User Mode, BUT it can be updated, provided it has been presented before-hand (DEFAULT CSC 0 is: AAh AAh AAh AAh)

*/

// SOSSE DATA TYPES
// typedef unsigned char  bool		// Boolean Data Type
// typedef unsigned long  iu32		// 4 Byte Unsigned Data Type
// typedef unsigned short iu16		// 2 Byte Unsigned Data Type
// typedef unsigned char  iu8		// 1 Byte Unsigned Data Type

// GC_MEMO Response Data in LSB, Data(D0,D1,D2,D3)

#include <types.h>
#include <sw.h>
#include <hal.h>

// COMMAND: READ - This command is used to READ a single WORD (four bytes) from memory
#define	R_CLA	0x80
#define R_INS	0xBE
#define R_P1	0xA0
#define	R_P2	0x00	// Address of the WORD to be read
#define	R_LE	0x04

// COMMAND: UPDATE - This command is used to UPDATE a single WORD (four bytes) in memory. Automatically erases the WORD before writing the new value
#define U_CLA	0x80
#define U_INS	0xDE
#define U_P1	0xA0
#define	U_P2	0x00	// Address of the WORD to be updated
#define	U_LC	0x04

// COMMAND: VERIFY - This command is used to present (VERIFY) a card secret code (CSC)
#define V_CLA	0x80
#define	V_INS	0x20
#define	V_P1	0xA0
#define	V_P2	0x00	// Address of Ratification Counter for the CSC to be verified
#define	V_P2_0	0x07	// CSC 0 is presented with a P2 value of 0x07
#define	V_P2_1	0x39	// CSC 1 is presented with a P2 value of 0x39
#define	V_P2_2	0x3B	// CSC 2 is presented with a P2 value of 0x3B
#define	V_LC	0x04

// Status Words are defined in <sw.h>

void cmd_gc_read( void ) {				// THIS ONE DOESNT WORK ATM 

	iu8 b, i;
	//iu32 respData;

	t0_sendAck();

	//b = hal_io_recByteT0();			
	hal_io_sendByteT0( 0x00 );			
	hal_io_sendByteT0( 0x00 );
	hal_io_sendByteT0( 0x00 );
	hal_io_sendByteT0( 0x80 );			// Faking the PPA response, TEST

	sw_set( SW_OK );				// Set to 90 00
	return;	

}

void cmd_gc_write( void ) { 

	// Write (DE) Command

}


void cmd_gc_verify( void ) { 

	// Verify (20) Command

}


















