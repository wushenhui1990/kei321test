#include "c8051f320.h"
#include "config.h"
#include "timer.h"
#include "uart.h"
#include "bitop.h"


//---------------------------------------------------------

/*
  	R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	R/W 	Reset Value	   
	- 		USBCLK 					- 		- 			CLKSL 		00000000
	Bit7 	Bit6 	Bit5 	Bit4 	Bit3	 Bit2 	Bit1 	Bit0

	USBCLK--BIT4,BIT5,BIT6
	CLKSL---BIT0,BIT1

	CLKSL 		Selected Clock
	00 			Internal Oscillator (as determined by the IFCN bits in register OSCICN)
	01 			External Oscillator
	10 			4x Clock Multiplier / 2
	11 			RESERVED

*/

void clk_init(void)
{
  	unsigned char i = 0;

    bit_set(CLKMUL,BIT7);  			//mul enable,BIT0 BIT1 is 00 as default.

    for (i = 0; i < 20; i++);    	// Wait 5us for initialization

	bit_set(CLKMUL,BIT7|BIT6);

	while (!bit_get(CLKMUL ,BIT5));
	
	bit_clear(CLKSEL,BIT0|BIT1);   //4x Clock Multiplier / 2
	bit_set(CLKSEL,BIT1);

  	bit_set(OSCICN,BIT0|BIT1|BIT7);  //internal osc as system clock ,24MHz    		
	
   	bit_set(RSTSRC,BIT2);  			// enable missing clock detector
}

void xbar_init(void)
{
	XBR0      = 0x01;
    XBR1      = 0x40;
}

void sys_init(void)
{
    EA =0;

   	PCA0MD &= ~0x40;  // WDTE = 0 (clear watchdog timer enable)
  	clk_init();

#if(UART_DEBUG==1)
	uart_init();
#endif

	xbar_init();

	timer0_init();

	EA =1;

}


extern unsigned long g_ticks ;

void main(void)
{
	sys_init();

	F(("aaaa\n"));
	F(("bbbb\n"));
	F(("cccc\n"));
	while(1)
	{
	 	if(!(g_ticks%1000))
		{
			EA = 0;
			F(("g_ticks:%ld\n",g_ticks));
			EA = 1;
		}
	}

}														  