#include "c8051f320.h"


unsigned long g_ticks = 0 ;

//according to tool_init.c Timer_Init
 
void timer0_isr (void) interrupt 1   
{   
   TH0 = 0xa0;           // Reinit Timer0 High register   
   TL0 = 0x00;           // Reinit Timer0 Low register  
   g_ticks++; 
}