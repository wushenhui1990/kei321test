#ifndef __CONFIG_H__
#define __CONFIG_H__


#ifndef UART_DEBUG
#define UART_DEBUG		1
#endif

#ifndef SYSCLK
#define SYSCLK          24000000  	// SYSCLK in Hz (12 MHz internal   
#endif

#ifndef MAX_TIMER_EVENT_QUEUE
#define MAX_TIMER_EVENT_QUEUE	8
#endif

#ifndef MAX_EVENT_QUEUE
#define MAX_EVENT_QUEUE		8
#endif


#endif