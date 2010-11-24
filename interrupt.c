#include "c8051f320.h"
#include "config.h"
#include "ev_timer.h"

#if (TIMER0_ENABLE==1)

u8 idata g_ticks = 0 ;
u8 idata g_ticks_second =0;

//according to tool_init.c Timer_Init
 
void timer0_isr (void) interrupt 1   
{   
	TH0 = 0xF6;           // Reinit Timer0 High register   
	TL0 = 0x3B;           // Reinit Timer0 Low register 
	
	g_ticks++; 			 //5ms

	event_send(EVENT_ID_RETURN_MTOUCH);

/*	
	if(g_ticks==200)	 //1s
	{
		g_ticks = 0;
		g_ticks_second++;
		event_send(EVENT_ID_RETURN_MTOUCH);
	}
*/	
	#if(TIMER_EVENT_ENABLE ==1)
	timer_event_process();
	#endif
}


u32 get_ticks(void)
{
	return g_ticks;
}
#endif