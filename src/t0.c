
#include <config.h>
#include <sw.h>
#include <hal.h>
#include <t0.h>
#include <types.h>

iu8 header[HEADERLEN];
iu8 csc[CSCLEN]; 

/* Saves 4 bytes per ACK and costs itself 8 bytes. */
void t0_sendAck( void )
{
	hal_io_sendByteT0( header[1] );
}

/* Saves 4 bytes per ~ACK and costs itself 10 bytes. */
void t0_sendCAck( void )
{
	hal_io_sendByteT0( ~header[1] );
}

void t0_sendSw( void )
{
#if CONF_WITH_RETCODEMGR==1
	iu16 sw2;

	sw2 = sw_get();

	hal_io_sendByteT0( (sw2>>8)&0xFF );
	hal_io_sendByteT0( sw2&0xFF );
#else
	hal_io_sendByteT0( (sw>>8)&0xFF );
	hal_io_sendByteT0( sw&0xFF );
#endif

	return;
}

void t0_sendWord( iu16 w )
{
	hal_io_sendByteT0( (w>>8)&0xFF );
	hal_io_sendByteT0( w&0xFF );

	return;
}

void t0_recBlock( iu8 *dst, iu8 len )
{
	iu8 i, b;

	/* Receive */
	for( i=0; i<len; i++ ) {
		/* Data */
		b = hal_io_recByteT0();

		*dst++ = b;
	}
}

bool t0_testP1P2( iu16 p1p2 )
{
	if( (((iu16)header[2]<<8)|header[3])==p1p2 )
		return TRUE;

	sw_set( SW_WRONG_P1P2 );
	return FALSE;
}

bool t0_testP3( iu8 p3 )
{
	if( header[4]==p3 )
		return TRUE;

	sw_set( SW_WRONG_LEN );
	return FALSE;
}

