#ifndef __UART_H__
#define __UART_H__

#include "config.h"

#if(UART_DEBUG==1)
void uart_init(void);
void uart_printf(char *fmt,...);
#define F(x)  uart_printf x
#else
#define F(x)
#endif

#endif