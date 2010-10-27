
#ifndef __EV_H__
#define __EV_H__

#include "config.h"
#include "type.h"
//================================================
#define EVENT_ID_TIMER_DEBUG 		0x00
#define EVENT_ID_TIMER_5000MS		0x01
#define EVENT_ID_TIMER_UARTRCV		0x02


#define EVENT_ID_INVALID			0xFF

//================================================


typedef void (*p_ev_handle)(void); 


void event_init(void);
char event_send(u8 ev_id) reentrant;
void event_process(void) reentrant; 

char event_cb_regist(u8 ev_id,void (*pFunc)(void));
char event_cb_unregist(u8 ev_id);


#endif