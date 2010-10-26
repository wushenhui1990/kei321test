#include "c8051f320.h"
#include "config.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"
#include "flash_rw.h"

extern unsigned long g_ticks ;
extern void Init_Device(void);



void flash_rw_test(void)
{
   #define FLASH_WRITE_ADDR		0x3900
   #define FLASH_WRITE_LEN		0x40

   	unsigned long start = 0;
	unsigned long end = 0;
	unsigned char idx;	

	u8 src[FLASH_WRITE_LEN] =
	{
	    0xFF, 0xFF, 0x8B, 0xD8, 0x59, 0x85, 0xDB, 0x59, 0x0F, 0x85, 0x39, 0x02, 0x00, 0x00, 0x83, 0x46,
	    0x0C, 0x02, 0x8B, 0x46, 0x0C, 0x56, 0x8A, 0x78, 0xFE, 0x8A, 0x58, 0xFF, 0xE8, 0x48, 0x7E, 0xFF,
	    0xFF, 0x85, 0xDB, 0x59, 0x74, 0x41, 0x8B, 0x46, 0x08, 0x89, 0x45, 0x08, 0x8B, 0x45, 0x10, 0x03,
	    0xD8, 0x53, 0x56, 0xE8, 0xF6, 0x7D, 0xFF, 0xFF, 0x8B, 0xD8, 0x59, 0x85, 0xDB, 0x59, 0x0F, 0x85 
	};

	u8 dst[FLASH_WRITE_LEN] = {0};


 	flash_read(dst,FLASH_WRITE_ADDR,FLASH_WRITE_LEN);

	F(("read_val_1:\n"));
	for(idx=0;idx<FLASH_WRITE_LEN;idx++)
	{
		if(idx&&(!(idx%16)))
			F(("\n"));
		F(("%02bx ",dst[idx]));
	}
	F(("\n"));

	flash_update(FLASH_WRITE_ADDR,src,FLASH_WRITE_LEN);
	flash_read(dst,FLASH_WRITE_ADDR,FLASH_WRITE_LEN);


	F(("read_val_2:\n"));
	for(idx=0;idx<FLASH_WRITE_LEN;idx++)
	{
		if(idx&&(!(idx%16)))
			F(("\n"));
		if(src[idx]!=dst[idx])
			F(("* "));
		else
			F(("%02bx ",dst[idx]));
	}
	F(("\n"));

}

void main(void)
{
	Init_Device();

	uart_init();

	F(("\nsizeof(short)=%02bx\nsizeof(int)=%02bx\nsizeof(long)=%02bx\n\n",sizeof(short),sizeof(int),sizeof(long)));

	flash_rw_test();

	while(1)
	{
	 	if(!(g_ticks%1000))
		{
			EA = 0;
			F(("g_ticks:%ld\n",g_ticks/1000));
			EA = 1;
		}
	}

}														  