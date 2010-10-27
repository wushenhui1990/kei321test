#include "c8051f320.h"
#include "type.h"
#include "ev.h"
#include "uart.h"

//----------------------------------------------------------
u8				g_ev_event[MAX_EVENT_QUEUE];
u8 				g_ev_head;
u8 				g_ev_tail;
u8 				g_ev_len;

p_ev_handle		g_ev_handler[MAX_EVENT_QUEUE];	
//----------------------------------------------------------	
#define EVENT_DEBUG		1


void ev_default_handler(void)
{
 	F(("in ev_default_handler\r\n"));
	return;
}

void event_init(void)
{
	u8	xdata i;
	for(i=0;i<MAX_EVENT_QUEUE;i++)
	{
		g_ev_event[i]=EVENT_ID_INVALID;
	}
	g_ev_head=0;
	g_ev_tail=0;
	g_ev_len=0;


	for(i=0;i<MAX_EVENT_QUEUE;i++)
		g_ev_handler[i]=ev_default_handler;
}
 
 /*
	return success: 1
	return failed : 0
 */
char event_send(u8 ev_id) reentrant
{
	if(g_ev_len <MAX_EVENT_QUEUE)
	{
		g_ev_event[g_ev_tail] = ev_id;
		g_ev_tail = (g_ev_tail+1)%MAX_EVENT_QUEUE;
		g_ev_len++;
		return 1;
	}
		
	//F(("+++ err[%d] [%02d][%02d][%02d]\n",ev_id,g_ev_head,g_ev_tail,g_ev_len));

	return	0;
	
}
//static unsigned long debug_cnt=0;
void event_process(void)  reentrant
{
	u8  xdata ev_id;
	
	if(g_ev_len) 
	{
 		EA = 0;


		ev_id = g_ev_event[g_ev_head]; 					//get ev
		g_ev_event[g_ev_head]= EVENT_ID_INVALID;		//reset ev

		F(("---[%bx] [%bx][%bx][%bx]\r\n",ev_id,g_ev_head,g_ev_tail,g_ev_len));

		g_ev_head = (g_ev_head+1)% MAX_EVENT_QUEUE;	  //update header and len		
		g_ev_len--;		


		(*g_ev_handler[ev_id])();    

		EA =1;
	}
	else
	{
	  //PCON |= 1;
	  //F(("&&:%ld\n",debug_cnt++));
	}
}

/*
	return success: 1
	return failed : 0
 */
char event_cb_regist(u8 ev_id,void (*pFunc)(void))
{
	if(ev_id>= MAX_EVENT_QUEUE)
	{
	 	F(("registe invalid id:%x\r\n",ev_id));
		return 0;
	}
	if(g_ev_handler[ev_id] != ev_default_handler)
	{
		F(("has registed id:%x\r\n",ev_id));		
		return 0;
	}
	g_ev_handler[ev_id] = pFunc;
	return 1;
}

char event_cb_unregist(u8 ev_id)
{
	if(ev_id>= MAX_EVENT_QUEUE)
	{
	 	F(("unregiste invalid id:%x\r\n",ev_id));
		return 0;
	}
	if(g_ev_handler[ev_id] == ev_default_handler)
	{
		F(("has no registed:%x\r\n",ev_id));		
		return 0;
	}
	g_ev_handler[ev_id] = ev_default_handler;
	return 1;}
