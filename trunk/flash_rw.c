#include "c8051f320.h"
#include "flash_rw.h"

//
// This routine writes <byte> to the linear FLASH address <addr>.
//
// To do:
// -- optimize to skip 0xFF bytes
//
void flash_bytewrite (u16 addr, char byte)
{
	bit EA_SAVE = EA; // preserve EA
	u8 xdata * data pwrite; // FLASH write pointer
	EA = 0; // disable interrupts
	// change clock speed to slow, then restore later
	VDM0CN = 0x80; // enable VDD monitor
	RSTSRC = 0x02; // enable VDD monitor as a reset source
	pwrite = (u8 xdata *) addr;
	FLKEY = 0xA5; // Key Sequence 1
	FLKEY = 0xF1; // Key Sequence 2
	PSCTL |= 0x01; // PSWE = 1
	VDM0CN = 0x80; // enable VDD monitor
	RSTSRC = 0x02; // enable VDD monitor as a reset source
	*pwrite = byte; // write the byte
	PSCTL &= ~0x01; // PSWE = 0
	EA = EA_SAVE; // restore interrupts
}

//
// This routine reads a <byte> from the linear FLASH address <addr>.
//
u8 flash_byteread (u16 addr)
{
	bit EA_SAVE = EA; // preserve EA
	u8 code * data pread; // FLASH read pointer
	u8 byte;
	EA = 0; // disable interrupts
	pread = (u8 code *) addr;
	byte = *pread; // read the byte
	EA = EA_SAVE; // restore interrupts
	return byte;
}
/*
//
// This routine erases the FLASH page containing the linear FLASH address
// <addr>.
//
void flash_pageerase (u16 addr)
{
	bit EA_SAVE = EA; // preserve EA
	u8 xdata * data pwrite; // FLASH write pointer
	EA = 0; // disable interrupts
	// change clock speed to slow, then restore later
	VDM0CN = 0x80; // enable VDD monitor
	RSTSRC = 0x02; // enable VDD monitor as a reset source
	pwrite = (u8 xdata *) addr;
	FLKEY = 0xA5; // Key Sequence 1
	FLKEY = 0xF1; // Key Sequence 2
	PSCTL |= 0x03; // PSWE = 1; PSEE = 1
	VDM0CN = 0x80; // enable VDD monitor
	RSTSRC = 0x02; // enable VDD monitor as a reset source
	*pwrite = 0; // initiate page erase
	PSCTL &= ~0x03; // PSWE = 0; PSEE = 0
	EA = EA_SAVE; // restore interrupts
}
*/

void flash_write (u16 dest, u8 *src, u8 cnt)
{
	u16 i;
	for (i = dest; i < dest+cnt; i++)
	{
		flash_bytewrite (i, *src++);
	}
	return;
}
//-----------------------------------------------------------------------------
// FLASH_Read
//-----------------------------------------------------------------------------
//
// This routine copies <numbytes> from the linear FLASH address <src> to
// <dest>.
//
void flash_read (u8 *dest, u16 src, u8 cnt)
{
	u16 i;
	for (i = 0; i < cnt; i++)
	{
		*dest++ = flash_byteread (src+i);
	}
	return ;
}