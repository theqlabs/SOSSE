
//	RESEARCH VERSION - NOT FOR PUBLIC USE!
//	If you have any problems, you can go fuck yourself
//	written by: Q

// 	Brief main() contains command interpreter loop. At the end of the loop SW is sent, function never returns, why should it? 

#include <config.h>
#include <auth.h>
#include <commands.h>
#include <sw.h>
#include <fs.h>
#include <hal.h>
#include <t0.h>
#include <transaction.h>
#include <gc_memo.h>		// GemPlus GemClub Memo - Testing commands/responses

int main( void ) {

	iu8 i, len, b;							// 1-Byte variables
	hal_init();							// TODO - On Error?
			
	hal_io_sendByteT0( 0x3B );					// This isn't doing SHIT, commented out for now
									// Implemented ATR sending from EEPROM already
								 
#if CONF_WITH_LGGING==1
	log_init();
#endif

	resplen = 0;							// response length? 

#if CONF_WITH_TRANSACTIONS==1
	/* Commit transactions not yet done. */
	/* TODO: On error? */
	ta_commit();
#endif /* CONF_WITH_TRANSACTIONS */

	/* Initialize FS state in RAM. */				// Shouldn't this happen ONLY if CONF_WITH_FILESYSTEM is set to 1?
	/* TODO: On error? */
	fs_init();

	if( !(hal_eeprom_read( &len, ATR_LEN_ADDR, 1 ) &&
		(len<=ATR_MAXLEN)) )
		for(;;) {}

	for( i=1; i<len; i++ ) {
		if( !hal_eeprom_read( &b, ATR_ADDR+i, 1 ) ) for(;;) {}
		hal_io_sendByteT0( b );
	}

	/* Command loop */
	for(;;) {							// One of a number of infinite loops in this hell-hole
		
		for( i=0; i<5; i++ ) {
			header[i] = hal_io_recByteT0();
		}

		if( (header[0]&0xFC)==CLA_PROP ) {			// IF header[0] which is the FIRST BYTE EQUALS 0x80-0x83 (MASKS with 0xFC)
			switch( header[1]&0xFE ) {			// switch looks at 2ND BYTE MASKS with 0xFE (doesn't care about last bit) and if matches, chooses CASE statement

#if CONF_WITH_TESTCMDS==1

			case INS_WRITE:					// this is looking for 02 INS byte
				cmd_write();				// cmd_write() WRITES to EEPROM
				break;					// breaks from switch/case statement

			case INS_READ:					// this is looking for 04 INS byte
				cmd_read();				// cmd_read() READS from EEPROM
				break;					// break from switch/case
#endif 									

#if CONF_WITH_CREATECMD==1

			case INS_CREATE:				// this is looking for E0 INS byte
				cmd_create();				// cmd_create() creates a file, see doxygen/index.html for details
				break;					// break from switch/case
#endif 									// CONF_WITH_CREATECMD==1

#if CONF_WITH_DELETECMD==1

			case INS_DELETE:				// this is looking for E4 INS byte
				cmd_delete();				// cmd_delete() looks for a FID and deletes, see doxygen/index.html for details
				break;					// break from switch/case
#endif									// CONF_WITH_DELETECMD==1

			case INS_GET_RESPONSE:				// this is looking for C0 INS byte
				cmd_getResponse();			// Fetches data from internal authentication and select
				break;

			case INS_READ_BINARY:				// this is looking for B0 INS byte
				cmd_readBinary();			// Reads length bytes from offset of the currently selected EF (doxygen/index.html)
				break;				
			
			case INS_SELECT:				// this is looking for A4 INS byte
				cmd_select();				// it tries to select a specified file based on FID
				break;
			
			case INS_UPDATE_BINARY:				// this is looking for D6 INS byte
				cmd_updateBinary();			// write length bytes to Offset of the currently selected EF
				break;

			case R_INS:					// this is looking for BE (GemClub Memo)
				cmd_gc_read();				// Take in P1,P2,P3 ? Where do I respond
				break;

			default:					// If NONE of these case statements are triggered
				t0_sendWord( SW_WRONG_INS );		// then it sends 6D 00 as the SW (Status Word)
			}
		}							// End bracket for the IF statement 

		else { t0_sendWord( SW_WRONG_CLA ); }			// If the header[0]==CLA_PROP does not match send SW_WRONG_CLA byte 6E 00

		t0_sendSw();						// Return the SW in sw <- variable? 
	}
}

