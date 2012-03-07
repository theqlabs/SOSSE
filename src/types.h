#ifndef SOSSE_TYPES_H
#define SOSSE_TYPES_H

#define FALSE	0
#define TRUE	!FALSE

#ifdef __AVR__
#include <pgmspace.h>
//! Variable is in code space. Be carfull with pointers to this space.
#define	CODE	__ATTR_PROGMEM__
#else
#define PRG_RDB(x)	(*((iu8*)(x)))
#define	CODE
#endif

//! 1 byte unsiged data type.
typedef unsigned char iu8;
//! 2 byte unsigned data type.
typedef unsigned short iu16;
//! 4 byte unsigned data type.
typedef unsigned long iu32;
//! Boolean data type.
typedef unsigned char bool;

#endif /* SOSSE_TYPES_H */
