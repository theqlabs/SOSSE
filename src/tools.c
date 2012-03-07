#include <config.h>
#include <tools.h>
#include <types.h>

#ifdef ENDIAN_LITTLE
void hton_us( iu16 *us, iu8 num )
{
	iu16 local;

	while( num-- ) {
		local=*us;
		*us++=swap_us(local);
	}
}

#if defined(__i386__)
void hton_ul( iu32 *ul, iu8 num )
{
	iu32 local;

	while( num-- ) {
		local=*ul;
		*ul++=swap_ul(local);
	}
}
#endif
#endif /* ENDIAN_LITTLE */

