#include "uart.h"
#include "flash_rw.h"
#include "F3xx_USB0_ReportHandler.h"
#include "test.h"
#if 1
/*
void flash_rw_test(void)
{
   #define FLASH_WRITE_ADDR		0x3C00
   #define FLASH_WRITE_LEN		0x40

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
		//flash_update(FLASH_WRITE_ADDR,src,FLASH_WRITE_LEN);
		flash_pageerase(FLASH_WRITE_ADDR);
		flash_write(FLASH_WRITE_ADDR,src,FLASH_WRITE_LEN);
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

 */
/*

u8 idata BACKUP_PACKET[13] = {0};
u8 idata g_backup_ptnum = 0;


char FillHidPacket(PanelPoint *MyPoint,u8 PointNum)
{
	char i = 0;
	char flag = 0;

	IN_PACKET[0] = REPORT_ID_IN_MTOUCH;

	if (PointNum >= g_backup_ptnum)
	{
		IN_PACKET[13] = PointNum;
		g_backup_ptnum = PointNum;
	}
	else
	{
		IN_PACKET[13] = g_backup_ptnum;
		g_backup_ptnum = PointNum;
	}
	
	for (i=PointNum;i<2;i++)
	{
		IN_PACKET[1+6*i] = 0x0;
	}
	
	for (i=0;i<PointNum;i++)
	{
		IN_PACKET[1+6*i] = 0x03;
		IN_PACKET[2+6*i] = MyPoint[i].ID+1;
		IN_PACKET[3+6*i] = (unsigned char)(MyPoint[i].x);
		IN_PACKET[4+6*i] = (unsigned char)(MyPoint[i].x>>8);
		IN_PACKET[5+6*i] = (unsigned char)(MyPoint[i].y);
		IN_PACKET[6+6*i] = (unsigned char)(MyPoint[i].y>>8);
	}

	for (i=0;i<13;i++)
	{
		if (BACKUP_PACKET[i]!=IN_PACKET[i+1])
		{
			flag = 1;
			break;
		}
	}

	if (flag)
	{
		for (i=0;i<13;i++)
		{
			BACKUP_PACKET[i] = IN_PACKET[i+1];
		}
	}

	return flag;
}
*/
void test_func(void)
{		
  	//flash_rw_test();
	//config_sensor_test();
}

#else

 void test_func(void)
 {
 	return;
 }

#endif
