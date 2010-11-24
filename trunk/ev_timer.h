#ifndef __EV_TIMER_H__
#define __EV_TIMER_H__

#include "config.h"
#include "type.h"
#include "ev.h"
//-------------------------------------------------------------
#if(TIMER_EVENT_ENABLE==1)
typedef struct _timer_ev_unit
{
    u8 event; 	
	u16 time;   
    u16 time_bak; 
	void (*callback)(void);// callback;
}timer_ev_unit_st;

/*
typedef struct _timer_event_st
{
 	timer_ev_unit_st ev_unit[MAX_TIMER_EVENT_QUEUE];
	u8  tail;
}timer_event_st;
*/
void timer_event_init(void);
char timer_event_add(timer_ev_unit_st*ev_unit);
char timer_event_del(u8 ev_id);
void timer_event_process(void);

#endif
#endif