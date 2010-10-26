//-----------------------------------------------------------------------------
// USB_INT_Type.h
//-----------------------------------------------------------------------------

#ifndef	USB_INT_TYPE_H
#define USB_INT_TYPE_H

// Select one of appropriate include file for the device

#include "C8051F320.h"
//#include <C8051F340.h>
//#include <C8051F326.h>

// Compiler specific definition

#if defined(__C51__) || defined(__CX51__)

	#define KEIL
	#define BIG_ENDIAN			// high byte first

#elif defined SDCC

	#define LITTLE_ENDIAN		// low byte first

#endif

// basic type definition
typedef unsigned char   BYTE;
typedef unsigned int    UINT;
typedef unsigned long   ULONG;

// WORD type definition
typedef union {UINT i; BYTE c[2];} WORD;

#if defined BIG_ENDIAN

	#define MSB		0
	#define LSB		1

#else					// little endian - low byte first

	#define MSB		1
	#define LSB		0

#endif // end of BIG_ENDIAN

// boolean
#define FALSE	0
#define TRUE	1

#endif	/* USB_INT_TYPE_H */

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
