//-----------------------------------------------------------------------------
// F3xx_USB_Main.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This application will communicate with a PC across the USB interface.
// The device will appear to be a mouse, and will manipulate the cursor
// on screen.
//
// How To Test:    See Readme.txt
//
//
// FID:            3XX000006
// Target:         C8051F32x/C8051F340
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_BlinkyExample
//
//
// Release 1.1
//    -Added feature reports for dimming controls
//    -Added PCA dimmer functionality
//    -16 NOV 2006
// Release 1.0
//    -Initial Revision (PD)
//    -07 DEC 2005
//
//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------
#include "config.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"
#include "ev.h"
#include "ev_timer.h"


#include "c8051f3xx.h"
#include "F3xx_USB0_Register.h"
#include "F3xx_Blink_Control.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Descriptor.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

extern unsigned	long g_ticks ;
extern void Init_Device(void);
extern void test_func(void);
extern u8 	g_ev_head;
extern u8 	g_ev_tail;
extern u8 	g_ev_len;


#define	TRIG_TIME	1000
static void  func_for_debug(void) 
{

	F(("status:%ld [%02bx][%02bx][%02bx]\n",g_ticks/1000,g_ev_head,g_ev_tail,g_ev_len));
			

	return;
}

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{

   //System_Init ();
   //Usb_Init ();

   //EA = 1;
   	timer_ev_unit_st xdata unit;

	Init_Device();

#if(UART_DEBUG==1)
	uart_init();
#endif

//-----------------------------------------
	event_init();
	timer_event_init();

//-----------------------------------------add a timer eve
#if 0
	unit.event = EVENT_ID_TIMER_DEBUG;
	unit.time = TRIG_TIME;
	unit.callback = func_for_debug;
	timer_event_add(&unit);
#endif
//-----------------------------------------

	test_func();

	Usb_Init();

	EA = 1;

   while (1)
   {
      //if (BLINK_SELECTORUPDATE)
      //{
       //  BLINK_SELECTORUPDATE = 0;
         SendPacket (IN_BLINK_SELECTORID);
      //}

   			event_process();
   }
}

