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

/*
  	S0MODE	-	 MCE0	REN0	TB80	RB80	TI0	 	RI0 
	Bit7 	Bit6 Bit5 	Bit4 	Bit3 	Bit2 	Bit1 	Bit0

*/
void uart_init(void)
{
/*
    SCON0 = 0x0; 

    if (SYSCLK/BAUDRATE/2/256 < 1)
    {
        TH1 = -(SYSCLK/BAUDRATE/2);

		bit_clear(CKCON,BIT0|BIT1|BIT3);
		bit_set(CKCON,BIT3);
    }
    else if (SYSCLK/BAUDRATE/2/256 < 4)
    {
        TH1 = -(SYSCLK/BAUDRATE/2/4);
		bit_clear(CKCON,BIT0|BIT1|BIT3);
		bit_set(CKCON,BIT0|BIT3);

    }
    else if (SYSCLK/BAUDRATE/2/256 < 12)
    {
        TH1 = -(SYSCLK/BAUDRATE/2/12);
		bit_clear(CKCON,BIT0|BIT1|BIT3);
    }
    else
    {
        TH1 = -(SYSCLK/BAUDRATE/2/48);
		bit_clear(CKCON,BIT0|BIT1|BIT3);
		bit_set(CKCON,BIT1);
    }

    TL1 = TH1; 		// init Timer1
	bit_clear(TMOD,BIT7|BIT6|BIT5|BIT4);// TMOD: timer 1 in 8-bit autoreload
	bit_set(TMOD,BIT5);
    TR1 = 1; 		// START Timer1

 */
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
