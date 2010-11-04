#include "config.h"
#include <stdarg.h>
#include <stdio.h>
#include "c8051f320.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"

#if(UART_DEBUG==1)



//#define BAUDRATE	115200

u8 xdata uart_send_buf[64];

void uart_init_user(void)
{

    TI0 = 1; 		// Indicate TX0 ready
   	SBUF0 = ' ';

}


void uart_sendchar(char ch)
{
	if(ch=='\n')
	{
		SBUF0='\r';
		while (TI0== 0 ); 
		TI0= 0 ;
	}

	SBUF0=ch;
	while (TI0== 0 ); 
	TI0= 0 ;
	 
}

void uart_sendstring(char *str)
{
    while (*str)
    {
        uart_sendchar(*(str++));
    }
}

void uart_printf(char *fmt,...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(uart_send_buf, fmt, ap);
    uart_sendstring(uart_send_buf);
    va_end(ap);
}

#else
//------------------------------------------------
//uart commands
#define CMD_PBI					0x4C			//single r/w,burst w
#define CMD_BRPBI				0x58			//burst r
//pbi commands
#define PBI_APB_WRITE			0x6D			//apb w
#define PBI_APB_READ			0x6C			//apb r
#define PBI_DATA				0x75			//apb data to writ

//unsigned char xdata aaaa[100];

typedef struct _cmd_data_st
{
	u8 	op;
	u32 delt;	
}cmd_data_st;
/*
cmd_data_st cmd_data[] = 
{
 	{ 1,(0x17<<8) | 0x25 },
 	{ 1,(0x17<<8) | 0x25 },
 	{ 1,(0x17<<8) | 0x25 },
	{ 0,(0x17<<8) | 0x25 }
};
*/


void uart_send(u8 *buff,u16 len)
{
	u16 idata i;
	
    for(i=0;i<len;i++)
    {		
		SBUF0=buff[i];			 
		while (!TI0); 
		TI0 = 0 ;
    }

	return ;
}

void uart_read(u8 *buff,u16 len)
{
	u16 idata i;
	
    for(i=0;i<len;i++)
    {		
		while (!RI0 ); 
		RI0 = 0 ;
		buff[i] = SBUF0;			 
    }

	return ;
}

void uart_write_reg(u32 addr, u8 value)
{
	u8 buf[6];

	buf[0] = CMD_PBI;
	buf[1] = PBI_APB_WRITE;
	buf[2] = (u8)addr;
	buf[3] = (u8)(addr>>8);
	buf[4] = (u8)(addr>>16);
	buf[5] = value;

	uart_send(buf,6);

	return ;
}

void uart_read_reg(u32 addr, u8 *pvalue)
{
	u8 buf[8];

	buf[0] = CMD_PBI;
	buf[1] = PBI_APB_READ;
	buf[2] = (u8)addr;
	buf[3] = (u8)(addr>>8);
	buf[4] = (u8)(addr>>16);

	uart_send(buf,5);

	uart_read(pvalue,1);

}

void uart_burst_write(u32 addr,u8 *buf,u32 len)
{
	u8 bufh[8];

	bufh[0] = CMD_PBI;
	bufh[1] = PBI_APB_WRITE;
	bufh[2] = (u8)addr;
	bufh[3] = (u8)(addr>>8);
	bufh[4] = (u8)(addr>>16);
	uart_send(bufh,5);
	uart_send(buf,len);

}

void uart_burst_read(u32 addr,u8 *buf,u32 len)
{
	u8 bufh[8];

	bufh[0] = CMD_BRPBI;
	bufh[1] = PBI_APB_READ;
	bufh[2] = (u8)addr;
	bufh[3] = (u8)(addr>>8);
	bufh[4] = (u8)(addr>>16);
	uart_send(bufh,5);
	uart_read(buf,len);
}




#endif
