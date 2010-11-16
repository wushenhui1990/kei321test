#include "c8051f320.h"
#include "flash_rw.h"

#if 0
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

void flash_copy (u16 dest, u16 src, unsigned numbytes)
{
	u16 i;
	for (i = 0; i < numbytes; i++)
	{
		flash_bytewrite ((u16) dest+i, flash_byteread((u16) src+i));
	}
}
void flash_clear (u16 dest, unsigned numbytes)
{
	u16 dest_1_page_start; // first address in 1st page
	// containing <dest>
	u16 dest_1_page_end; // last address in 1st page
	// containing <dest>
	u16 dest_2_page_start; // first address in 2nd page
	// containing <dest>
	u16 dest_2_page_end; // last address in 2nd page
	// containing <dest>
	unsigned numbytes_remainder; // when crossing page boundary,
	// number of <src> bytes on 2nd page
	unsigned FLASH_pagesize; // size of FLASH page to update
	u16 wptr; // write address
	u16 rptr; // read address
	unsigned length;
	FLASH_pagesize = FLASH_PAGESIZE;
	dest_1_page_start = dest & ~(FLASH_pagesize - 1);
	dest_1_page_end = dest_1_page_start + FLASH_pagesize - 1;
	dest_2_page_start = (dest + numbytes) & ~(FLASH_pagesize - 1);
	dest_2_page_end = dest_2_page_start + FLASH_pagesize - 1;
	if (dest_1_page_end == dest_2_page_end)
	{
		// 1. Erase Scratch page
		flash_pageerase (FLASH_TEMP);
		// 2. Copy bytes from first byte of dest page to dest-1 to Scratch page
		wptr = FLASH_TEMP;
		rptr = dest_1_page_start;
		length = dest - dest_1_page_start;
		flash_copy (wptr, rptr, length);
		// 3. Copy from (dest+numbytes) to dest_page_end to Scratch page
		wptr = FLASH_TEMP + dest - dest_1_page_start + numbytes;
		rptr = dest + numbytes;
		length = dest_1_page_end - dest - numbytes + 1;
		flash_copy (wptr, rptr, length);
		// 4. Erase destination page
		flash_pageerase (dest_1_page_start);
		// 5. Copy Scratch page to destination page
		wptr = dest_1_page_start;
		rptr = FLASH_TEMP;
		length = FLASH_pagesize;
		flash_copy (wptr, rptr, length);
	} 
	else
	{
	 // value crosses page boundary
	// 1. Erase Scratch page
		flash_pageerase (FLASH_TEMP);
		// 2. Copy bytes from first byte of dest page to dest-1 to Scratch page
		wptr = FLASH_TEMP;
		rptr = dest_1_page_start;
		length = dest - dest_1_page_start;
		flash_copy (wptr, rptr, length);
		// 3. Erase destination page 1
		flash_pageerase (dest_1_page_start);
		// 4. Copy Scratch page to destination page 1
		wptr = dest_1_page_start;
		rptr = FLASH_TEMP;
		length = FLASH_pagesize;
		flash_copy (wptr, rptr, length);
		// now handle 2nd page
		// 5. Erase Scratch page
		flash_pageerase (FLASH_TEMP);
		// 6. Copy bytes from numbytes remaining to dest-2_page_end to Scratch page
		numbytes_remainder = numbytes - (dest_1_page_end - dest + 1);
		wptr = FLASH_TEMP + numbytes_remainder;
		rptr = dest_2_page_start + numbytes_remainder;
		length = FLASH_pagesize - numbytes_remainder;
		flash_copy (wptr, rptr, length);
		// 7. Erase destination page 2
		flash_pageerase (dest_2_page_start);
		// 8. Copy Scratch page to destination page 2
		wptr = dest_2_page_start;
		rptr = FLASH_TEMP;
		length = FLASH_pagesize;
		flash_copy (wptr, rptr, length);
	}
}

void flash_write (u16 dest, u8 *src, u16 cnt)
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
void flash_read (u8 *dest, u16 src, u16 cnt)
{
	u16 i;
	for (i = 0; i < cnt; i++)
	{
		*dest++ = flash_byteread (src+i);
	}
	return ;
}

void flash_update (u16 dest, u8 *src, u16 cnt)
{
	// 1. Erase <numbytes> starting from <dest>
	flash_clear (dest, cnt);
	// 2. Write <numbytes> from <src> to <dest>
	flash_write (dest, src, cnt);
}

#endif
