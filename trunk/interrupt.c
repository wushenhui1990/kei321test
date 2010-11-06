#include "c8051f320.h"
#include "ev_timer.h"

unsigned long g_ticks = 0 ;

//according to tool_init.c Timer_Init
 
void timer0_isr (void) interrupt 1   
{   
   TH0 = 0xF8;           // Reinit Timer0 High register   
   TL0 = 0x30;           // Reinit Timer0 Low register  
   g_ticks++; 
   timer_event_process();
}