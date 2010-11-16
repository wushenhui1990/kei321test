#include "uart.h"
#include "flash_rw.h"

#if 0
void flash_rw_test(void)
{
   #define FLASH_WRITE_ADDR		0x3900
   #define FLASH_WRITE_LEN		0x02

	unsigned char idata idx;
	unsigned char idata diff = 0;	

	u8 src[FLASH_WRITE_LEN];
	u8 dst[FLASH_WRITE_LEN];


 	flash_read(dst,FLASH_WRITE_ADDR,FLASH_WRITE_LEN);

	FS(("read_val_1:\n"));
	for(idx=0;idx<FLASH_WRITE_LEN;idx++)
	{
		if(idx&&(!(idx%16)))
			FS(("\n"));
		FB((dst[idx]));
	}
	FS(("\n"));

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
	
		FS(("read_val_2:\n"));
		for(idx=0;idx<FLASH_WRITE_LEN;idx++)
		{
			if(idx&&(!(idx%16)))
				FS(("\n"));
			if(src[idx]!=dst[idx])
				FS(("* "));
			else
				FB((dst[idx]));
		}
		FS(("\n"));
	}
}


extern void config_sensor_test(void);
void test_func(void)
{		
  	//flash_rw_test();
	config_sensor_test();
}

#else

 void test_func(void)
 {
 	return;
 }

#endif
