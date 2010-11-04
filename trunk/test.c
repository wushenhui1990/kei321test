#include "uart.h"
#include "flash_rw.h"

#if 1
void flash_rw_test(void)
{
   #define FLASH_WRITE_ADDR		0x3900
   #define FLASH_WRITE_LEN		0x40

   	//unsigned long start = 0;
//	unsigned long end = 0;
	unsigned char idx;
	unsigned char diff = 0;	

	u8 src[FLASH_WRITE_LEN];
	u8 dst[FLASH_WRITE_LEN];


 	flash_read(dst,FLASH_WRITE_ADDR,FLASH_WRITE_LEN);

	F(("read_val_1:\n"));
	for(idx=0;idx<FLASH_WRITE_LEN;idx++)
	{
		if(idx&&(!(idx%16)))
			F(("\n"));
		F(("%02bx ",dst[idx]));
	}
	F(("\n"));

	for(idx=0;idx<FLASH_WRITE_LEN;idx++)
	{
		src[idx] = idx;
	}

	for(idx=0;idx<FLASH_WRITE_LEN;idx++)
	{
		diff = src[idx] ^ dst[idx];
		if(diff) break;
	}

	if(diff)
	{
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
}


void test_func(void)
{
		
   //F(("\nsizeof(short)=%02bx\nsizeof(int)=%02bx\nsizeof(long)=%02bx\n\n",sizeof(short),sizeof(int),sizeof(long)));

  	flash_rw_test();

}

#else

 void test_func(void)
 {
 	return;
 }

#endif
