#ifndef __FLASH_RW_H
#define __FLASH_RW_H

#include "type.h"


#ifndef FLASH_PAGESIZE
#define FLASH_PAGESIZE 512
#endif
#ifndef FLASH_TEMP
#define FLASH_TEMP 0x03a00L
#endif
#ifndef FLASH_LAST
#define FLASH_LAST 0x03c00L
#endif

void flash_read (u8 *dest, u16 src, u16 cnt);
void flash_update (u16 dest, u8 *src, u16 numbytes);

#endif