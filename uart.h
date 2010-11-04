#ifndef __UART_H__
#define __UART_H__

#include "config.h"
#include "type.h"

#if(UART_DEBUG==1)
	void uart_putstring(char *str);
	void uart_puthex_byte(u8 b);
	void uart_puthex_word(u16 b);


	void uart_write_reg(u32 addr, u8 value);

	#define FS(x)  	uart_putstring	 x
	#define FB(x) 	uart_puthex_byte x
	#define FW(x) 	uart_puthex_word x
#else
	#define F(x)
#endif



#endif