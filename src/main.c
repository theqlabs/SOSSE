#include <config.h>
#include <sw.h>
#include <hal.h>
#include <t0.h>
#include <gc_memo.h>

#define CLA_PROP 0x80

/*! \brief Main function containing command interpreter loop.

	At the end of the loop, sw is sent as the status word.

	This function does never return.
*/
#if defined(CTAPI)
void sosse_main( void )
#else
int main( void )
#endif
{
	iu8 i, len, b;

	/* TODO: On error? */
	hal_init();

	/* Send ATR */
	/* TODO: Possible from EEPROM? */
	hal_io_sendByteT0( 0x3B );

#if CONF_WITH_TRANSACTIONS==1
	/* Commit transactions not yet done. */
	/* TODO: On error? */
	ta_commit();
#endif /* CONF_WITH_TRANSACTIONS */

#if CONF_WITH_PINAUTH==1
	/* Initialize authentication state. */
	/* TODO: On error? */
	auth_init();
#endif /* CONF_WITH_PINAUTH==1 */

	if( !(hal_eeprom_read( &len, ATR_LEN_ADDR, 1 ) &&
		(len<=ATR_MAXLEN)) )
		for(;;) {}					// for(;;) is acting like an error handler, if fails, loops forever

	for( i=1; i<len; i++ ) {
		if( !hal_eeprom_read( &b, ATR_ADDR+i, 1 ) ) for(;;) {}				// here too! ^
		hal_io_sendByteT0( b );
	}

	/* Command loop */					
	for(;;) {	
		
		for (i=0; i<HEADERLEN; i++) {
			header[i] = hal_io_recByteT0();
		}
	
		/*	
		if (header[1] == V_INS) {					// this doesn't fucking work
			for (i=0; i<CSCLEN; i++) {
				csc[i] = hal_io_recByteT0();
			}
		}
		*/

		

#if CONF_WITH_KEYAUTH==1
		if( challvalidity ) challvalidity--;
#endif /* CONF_WITH_KEYAUTH==1 */

#if CONF_WITH_TRNG==1
		hal_rnd_addEntropy();
#endif

		if( (header[0]&0xFC)==CLA_PROP ) {
			switch( header[1]&0xFE ) {

#if CONF_WITH_PINCMDS==1
			case INS_CHANGE_PIN:
				cmd_changeUnblockPIN();
				break;
#endif /* CONF_WITH_PINCMDS==1 */
#if CONF_WITH_CREATECMD==1
			case INS_CREATE:
				cmd_create();
				break;
#endif /* CONF_WITH_CREATECMD==1 */
#if CONF_WITH_DELETECMD==1
			case INS_DELETE:
				cmd_delete();
				break;
#endif /* CONF_WITH_DELETECMD==1 */
#if CONF_WITH_KEYCMDS==1
			case INS_EXTERNAL_AUTH:
				cmd_extAuth();
				break;
#endif /* CONF_WITH_KEYCMDS==1 */
#if CONF_WITH_KEYCMDS==1
			case INS_GET_CHALLENGE:
				cmd_getChallenge();
				break;
#endif /* CONF_WITH_KEYCMDS==1 */

#if CONF_WITH_KEYCMDS==1
			case INS_INTERNAL_AUTH:
				cmd_intAuth();
				break;
#endif /* CONF_WITH_KEYCMDS==1 */

#if CONF_WITH_PINCMDS==1
			case INS_UNBLOCK_PIN:
				cmd_changeUnblockPIN();
				break;
#endif /* CONF_WITH_PINCMDS==1 */

#if CONF_WITH_KEYCMDS==1
			case INS_VERIFY_KEY:
				cmd_verifyKeyPIN();
				break;
#endif /* CONF_WITH_KEYCMDS==1 */
#if CONF_WITH_PINCMDS==1
			case INS_VERIFY_PIN:
				cmd_verifyKeyPIN();
				break;
#endif /* CONF_WITH_PINCMDS==1 */

			case R_INS:					// GemClub Memo Read Instruction Byte (BE)
				cmd_gc_read();				// runs gemclub read function, defined in gc_memo.h
				break;					// break from switch/case

			case U_INS:					// GemClub Memo Update Instruction Byte (DE)
				cmd_gc_update();			// runs gemclub update function, defined in gc_memo.h
				break;					// break from switch/case

			case V_INS:					// GemClub Memo Verify Instruction Byte (20)
				cmd_gc_verify();			// runs gemclub verify function, defined in gc_memo.h
				break;					// break from switch/case

			default:
				//t0_sendWord( SW_WRONG_INS );
				t0_sendWord( SW_OK );
			}
		} else {
			//t0_sendWord( SW_WRONG_CLA );
			t0_sendWord( SW_OK );
		}

#if CONF_WITH_TRNG==1
		hal_rnd_addEntropy();
#endif

		/* Return the SW in sw */
		//t0_sendSw();
		t0_sendWord( SW_OK );
	}
}

