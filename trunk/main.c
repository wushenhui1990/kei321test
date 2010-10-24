#include "c8051f320.h"
#include "config.h"
#include "type.h"
#include "timer.h"
#include "uart.h"
#include "bitop.h"
#include "flash_rw.h"


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
  	u8 i = 0;

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
	
	u8 a[4] = {0x0d,0x0e,0x0a,0x0d};
	u8 b[4] = {0,0,0,0};
	sys_init();

	F(("aaaa\n"));
	F(("bbbb\n"));
	F(("cccc\n"));

	flash_read(b,0x3a00,4);
	F(("read from 0x3a00:%bx %bx %bx %bx\n",b[0],b[1],b[2],b[3]));
	flash_write(0x3a00,a,4);
	flash_read(b,0x3a00,4);
	F(("read from 0x3a00:%bx %bx %bx %bx\n",b[0],b[1],b[2],b[3]));

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