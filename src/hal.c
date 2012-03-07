#include <compat/deprecated.h>
#include <config.h>
#include <hal.h>
#include <eeprom.h>
#include <io.h>
#include <string.h>
#include <tools.h>
#include <types.h>

void hal_init( void )
{
	outb(ACSR,0x80);	// Analog Comparator and Control Status Register
	outb(DDRA,0xFF);	// Data Direction Registers
	outb(DDRB,0xFF);	// DDRB
	outb(DDRC,0xFF);	// DDRC
	outb(DDRD,0xFF);	// DDRD
	outb(PORTA,0xFF);	// Port Assignments
	outb(PORTB,0xFF);	// PORTB
	outb(PORTC,0xFF);	// PORTC
	outb(PORTD,0xFF);	// PORTD

#if CONF_WITH_TRNG==1
	/* Random number gathering */
	randbytes=0;
	outp( 0, TIMSK );	/* Disable timer interrupts */
	outp( 0, TCNT0 );	/* Set timer 0 value */
	outp( 1, TCCR0 );	/* Start timer 0 */
#endif

	return;
}

/*	addr: r25:r24
	ret: r25(=0):r24
*/
extern iu8 xeread( iu16 addr );
/*	addr: r25:r24
	b: r22
*/
extern void xewrt( iu16 addr, iu8 b );

bool hal_eeprom_read( iu8 *dst, iu16 src, iu8 len )
{
	while( len-- ) {
		if( src<EEPROM_SIZE ) {
			/* Internal EEPROM */
			while( !eeprom_is_ready() ) {}

			//*dst++ = eeprom_rb( src++ );
			*dst++ = eeprom_read_byte( src++ ); 
#if CONF_WITH_I2CEEPROM==1
		} else {
			/* External I2C EEPROM */
			/* Subtract the size of the internal EEPROM to get the
			   location in external EEPROM.
			*/
			*dst++ = xeread( src-EEPROM_SIZE );
			src++;
#endif /* CONF_WITH_I2CEEPROM==1 */
		}
	}

	return TRUE;
}

/* TODO: Compare written data? Return 6581 on error. */
bool hal_eeprom_write( iu16 dst, iu8 *src, iu8 len )
{
	while( len-- ) {
		if( dst<EEPROM_SIZE ) {
			/* Internal EEPROM */
			while( !eeprom_is_ready() ) {}

			//eeprom_wb( dst++, *src++ );
			eeprom_write_byte( dst++, *src++ );
#if CONF_WITH_I2CEEPROM==1
		} else {
			/* External I2C EEPROM */
			/* Subtract the size of the internal EEPROM to get the
			   location in external EEPROM.
			*/
			xewrt( dst-EEPROM_SIZE, *src++ );
			dst++;
#endif /* CONF_WITH_I2CEEPROM==1 */
		}
	}

	return TRUE;
}

/*	b: r25(=0):r24
*/
extern void sendbytet0( iu8 b );
/*	ret: r25(=0):r24
*/
extern iu8 recbytet0( void );

void hal_io_sendByteT0( iu8 b )
{
	sendbytet0( b );
	return;
}

iu8 hal_io_recByteT0( void )
{
	return recbytet0();
}
