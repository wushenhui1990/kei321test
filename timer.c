#include "c8051f320.h"
#include "config.h"
#include "timer.h"
#include "bitop.h"


#define TIMER_PRESCALER            		48  			// Based on Timer CKCON settings   
#define TIMER_TICKS_PER_MS  SYSCLK/TIMER_PRESCALER/1000   
   
   
#define AUX1     TIMER_TICKS_PER_MS   
#define AUX2     -AUX1   
#define AUX3     AUX2&0x00FF   
#define AUX4     ((AUX2&0xFF00)>>8)   
   
#define TIMER0_RELOAD_HIGH       AUX4  // Reload value for Timer0 high byte   
#define TIMER0_RELOAD_LOW        AUX3  // Reload value for Timer0 low byte   

unsigned long g_ticks = 0;
/*
								TMOD
GATE1 	C/T1	 T1M1	 T1M0 	GATE0 	C/T0 	T0M1 	T0M0 
Bit7 	Bit6	 Bit5	 Bit4 	Bit3 	Bit2 	Bit1 	Bit0 
*/

/*
								CKCON
 T3MH 	T3ML 	T2MH 	T2ML 	T1M 	T0M 	SCA1 	SCA0 
 Bit7 	Bit6 	Bit5 	Bit4 	Bit3 	Bit2 	Bit1 	Bit0

*/
void timer0_init(void)   
{   
   TH0 = TIMER0_RELOAD_HIGH;           // Init Timer0 High register   
   TL0 = TIMER0_RELOAD_LOW ;           // Init Timer0 Low register   
   //TMOD = 0x01;                      // Timer0 in 16-bit mode 
   bit_clear(TMOD,BIT0|BIT1);
   bit_set(TMOD,BIT0);
   
   bit_clear(CKCON,BIT2|BIT1|BIT0);
   bit_set(CKCON,BIT1);  
   //CKCON = 0x02;                       // Timer0 uses a 1:48 prescaler     
    
   ET0 = 1;                            // Timer0 interrupt enabled   
   TR0 = 1;                        		// Timer0 ON   
}   

void timer0_isr (void) interrupt 1   
{   
   TH0 = TIMER0_RELOAD_HIGH;           // Reinit Timer0 High register   
   TL0 = TIMER0_RELOAD_LOW;            // Reinit Timer0 Low register  
   g_ticks++; 
} 
/*
//----------------------------------------------------------
unsigned long t1cnt = 0;

void timer1_init(void)   
{   
   TH1 = 0;           // Init Timer1 High register  
    
   TMOD &= ~0x30;                        // mode2 Timer1 in 8-bit autoload 
   TMOD |= 0x20;
     
   CKCON |= 0x08;                       // Timer1 uses system clock   
   ET1 = 1;                            // Timer1 interrupt enabled   
   TR1 = 1;                        		// Timer1 ON   
}   

void timer1_isr (void) interrupt 3   
{   
   t1cnt++; 
}
*/