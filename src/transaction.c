/*
	Simple Operating System for Smartcard Education
	Copyright (C) 2002  Matthias Bruestle <m@mbsks.franken.de>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*! @file
	\brief Transaction functions.

	$Id: transaction.c,v 1.11 2002/12/22 15:42:56 m Exp $
*/

#include <config.h>
#include <sw.h>
#include <hal.h>
#include <transaction.h>
#include <types.h>

#if CONF_WITH_TRANSACTIONS==1

bool ta_setdata( iu8 *data, iu8 len, iu16 dst )
{
	iu8 cleanstate=TRANSAC_STATE_CLEAN;
	iu8 datastate=TRANSAC_STATE_DATA;

	if( len>(TRANSAC_DATA_LEN-3) ) {
		sw_set( SW_FILE_TO_SHORT );
		return FALSE;
	}

	if( !( hal_eeprom_write( TRANSAC_STATE_ADDR, &cleanstate, 1 ) &&
		hal_eeprom_write( TRANSAC_DATA_ADDR, (iu8*) &dst, 2 ) &&
		hal_eeprom_write( TRANSAC_DATA_ADDR+2, &len, 1 ) &&
		hal_eeprom_write( TRANSAC_DATA_ADDR+3, data, len ) &&
		hal_eeprom_write( TRANSAC_STATE_ADDR, &datastate, 1 ) ) ) return FALSE;

	return TRUE;
}

bool ta_commit( void )
{
	iu8 state;
	iu16 addr;
	iu8 len;
	iu8 buffer[TRANSAC_DATA_LEN-3];

	if( !hal_eeprom_read( &state, TRANSAC_STATE_ADDR, 1 ) ) return FALSE;

	if( state==TRANSAC_STATE_CLEAN ) return TRUE;

	if( state!=TRANSAC_STATE_DATA ) {
		state = TRANSAC_STATE_CLEAN;
		return hal_eeprom_write( TRANSAC_STATE_ADDR, &state, 1 );
	}

	if( !( hal_eeprom_read( (iu8*)&addr, TRANSAC_DATA_ADDR, 2 ) &&
		hal_eeprom_read( &len, TRANSAC_DATA_ADDR+2, 1 ) &&
		hal_eeprom_read( buffer, TRANSAC_DATA_ADDR+3, sizeof(buffer) ) ) )
		return FALSE;

	if( len>sizeof(buffer) ) {
		sw_set( SW_OTHER );
		return FALSE;
	}

	state=TRANSAC_STATE_CLEAN;

	if( !( hal_eeprom_write( addr, buffer, len ) &&
		hal_eeprom_write( TRANSAC_STATE_ADDR, &state, 1 ) ) ) return FALSE;

	return TRUE;
}

#if 0 /* Uncalled */
bool ta_rollback( void )
{
	iu8 state;

	if( !hal_eeprom_read( &state, TRANSAC_STATE_ADDR, 1 ) ) return FALSE;

	if( state==TRANSAC_STATE_CLEAN ) return TRUE;

	state=TRANSAC_STATE_CLEAN;

	if( !hal_eeprom_write( TRANSAC_STATE_ADDR, &state, 1 ) ) return FALSE;

	return TRUE;
}
#endif

#endif /* CONF_WITH_TRANSACTIONS==1 */

