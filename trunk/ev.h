
#ifndef __EV_H__
#define __EV_H__

#include "config.h"
#include "type.h"
//================================================
#define EVENT_ID_TIMER_DEBUG 			0x00
#define EVENT_ID_RETURN_HOST_CMD		0x01
#define EVENT_ID_RETURN_MTOUCH			0x02


#define EVENT_ID_INVALID			0xFF

//================================================


void event_init(void);
char event_send(u8 ev_id) reentrant;
void event_process(void) ; 

char event_cb_regist(u8 ev_id,void (*pFunc)(void));
char event_cb_unregist(u8 ev_id);


#endif