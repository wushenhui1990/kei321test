#include "c8051f320.h"
#include "ev_timer.h"

#if 0
static timer_event_st		  xdata		g_sys_timer_event;


void timer_event_init(void)
{
	u8	idata i=0;

	for(i=0;i<MAX_TIMER_EVENT_QUEUE;i++)
    {
		g_sys_timer_event.ev_unit[i].event = 0;
		g_sys_timer_event.ev_unit[i].time = 0; 
		g_sys_timer_event.ev_unit[i].time_bak = 0;
	}
	g_sys_timer_event.tail = 0;

}
/*
	add one event which use timer in mcu
	para:
		type:	no use
		ev_id:	event_id
		time:	how long the time happen		
*/
char timer_event_add(timer_ev_unit_st*ev_unit)
{
	char idata ret = 0;
	//bit ea_save = EA;

	EA = 0;

	if(g_sys_timer_event.tail<MAX_TIMER_EVENT_QUEUE)
	{

		g_sys_timer_event.ev_unit[g_sys_timer_event.tail].event = ev_unit->event;
		g_sys_timer_event.ev_unit[g_sys_timer_event.tail].time = ev_unit->time; 
		g_sys_timer_event.ev_unit[g_sys_timer_event.tail].time_bak = ev_unit->time;
		g_sys_timer_event.ev_unit[g_sys_timer_event.tail].callback = ev_unit->callback;

		ret = event_cb_regist(ev_unit->event,ev_unit->callback);
		if(ret)
		{
		 	g_sys_timer_event.tail++;	
		}

	}

	EA =1;

	return ret;
}
/*
char timer_event_del(u8 ev_id)
{
	u8 xdata i;
	u8 xdata j;
	u8 xdata ret = 0;
 	bit ea_save = EA;

	EA =0;

	for(i=0;i< g_sys_timer_event.tail;i++)
	{
		if(g_sys_timer_event.ev_unit[i].event==ev_id)
		{
			event_cb_unregist(ev_id);

			for(j=i;j<g_sys_timer_event.tail;j++)
			{
			 	 g_sys_timer_event.ev_unit[j] =  g_sys_timer_event.ev_unit[j+1];
			}
			g_sys_timer_event.tail--;
			ret = 1;
			break;
		}
	}

	EA =ea_save;

	return ret;
}
*/
void timer_event_process(void)
{
	u8 idata i;

	for(i=0;i<g_sys_timer_event.tail;i++)
	{
		g_sys_timer_event.ev_unit[i].time--;
		
		if(g_sys_timer_event.ev_unit[i].time ==0)
		{
			g_sys_timer_event.ev_unit[i].time = g_sys_timer_event.ev_unit[i].time_bak;			
			event_send(g_sys_timer_event.ev_unit[i].event);
		}
	}

}
#endif