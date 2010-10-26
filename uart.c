#include "config.h"

#if(UART_DEBUG==1)

#include <stdarg.h>
#include <stdio.h>
#include "c8051f320.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"


#define BAUDRATE	115200

u8 xdata uart_send_buf[64];

void uart_init(void)
{

    TI0 = 1; 		// Indicate TX0 ready
   	SBUF0 = ' ';

}


void uart_sendchar(char ch)
{
	if(ch=='\n')
	{
		while (TI0== 0 ); 
		TI0= 0 ;
		SBUF0='\r';
	}

	while (TI0== 0 ); 
	TI0= 0 ;
	SBUF0=ch;
	 
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

#endif
