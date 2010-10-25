#include "c8051f320.h"
#include "config.h"
#include "type.h"
#include "timer.h"
#include "uart.h"
#include "bitop.h"
#include "flash_rw.h"

extern unsigned long g_ticks ;

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
//	XBR0      = 0x01;
//  XBR1      = 0x40;

    P2MDOUT   = 0x02;
    P0SKIP    = 0xCF;
    P1SKIP    = 0xFF;
    P2SKIP    = 0x01;
    XBR0      = 0x01;
    XBR1      = 0xC1;
}
void pca_init(void)
{
    PCA0CN    = 0x40;
    PCA0MD    &= ~0x40;
    PCA0MD    = 0x08;
    PCA0CPM0  = 0x46;
    PCA0CPL4  = 0x00;
    //PCA0MD    |= 0x40;
    PCA0CPH0  = 0x0C;

}

void sys_init(void)
{
    EA =0;

   	PCA0MD &= ~0x40;  // WDTE = 0 (clear watchdog timer enable)
  	clk_init();

#if(UART_DEBUG==1)
	uart_init();
#endif

	pca_init();
	xbar_init();

	timer0_init();

	EA =1;

}




void flash_rw_test(void)
{
   #define FLASH_WRITE_ADDR		0x3900

	u8 a[4] = {0x0f,0x0a,0x0c,0x0e};
	u8 b[4] = {0,0,0,0};

 	flash_read(b,FLASH_WRITE_ADDR,4);
	F(("read flash1: %bx %bx %bx %bx\n",b[0],b[1],b[2],b[3]));

	flash_update(FLASH_WRITE_ADDR,a,4);
	flash_read(b,FLASH_WRITE_ADDR,4);
	F(("read flash2: %bx%bx%bx%bx\n",b[0],b[1],b[2],b[3]));


}

void main(void)
{
	
	sys_init();

	F(("\nsizeof(short)=%02bx\nsizeof(int)=%02bx\nsizeof(long)=%02bx\n",sizeof(short),sizeof(int),sizeof(long)));

	flash_rw_test();

	while(1)
	{
	 	if(!(g_ticks%1000))
		{
			EA = 0;
			F(("g_ticks:%ld\n",g_ticks/1000));
			EA = 1;
		}
	}

}														  