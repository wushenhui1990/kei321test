#include "c8051f320.h"
#include "config.h"
#include "ev_timer.h"

#if (TIMER0_ENABLE==1)

static u32  g_ticks = 0 ;

//according to tool_init.c Timer_Init
 
void timer0_isr (void) interrupt 1   
{   
   TH0 = 0xF8;           // Reinit Timer0 High register   
   TL0 = 0x30;           // Reinit Timer0 Low register  
   g_ticks++; 

   #if(TIMER_EVENT_ENABLE ==1)
   timer_event_process();
   #endif
}


u32 get_ticks(void)
{
   return g_ticks;
}
#endif