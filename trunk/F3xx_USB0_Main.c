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
#include "F3xx_USB0_ReportHandler.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Descriptor.h"

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

extern unsigned	long g_ticks ;
extern void Init_Device(void);
extern void test_func(void);
extern void Interrupts_Init();

extern u8 	g_ev_head;
extern u8 	g_ev_tail;
extern u8 	g_ev_len;

void Usb_Init(void)
{
   POLL_WRITE_BYTE(POWER,  0x08);      // Force Asynchronous USB Reset
   POLL_WRITE_BYTE(IN1IE,  0x07);      // Enable Endpoint 0-2 in interrupts
   POLL_WRITE_BYTE(OUT1IE, 0x07);      // Enable Endpoint 0-2 out interrupts
   POLL_WRITE_BYTE(CMIE,   0x07);      // Enable Reset, Resume, and Suspend
                                       // interrupts
   USB0XCN = 0xE0;                     // Enable transceiver; select full speed
   POLL_WRITE_BYTE(CLKREC, 0x80);      // Enable clock recovery, single-step
                                       // mode disabled

   EIE1 |= 0x02;                       // Enable USB0 Interrupts
  // EA = 1;                             // Global Interrupt enable
                                       // Enable USB0 by clearing the USB
                                       // Inhibit bit
   POLL_WRITE_BYTE(POWER,  0x01);      // and enable suspend detection
}


#define	TRIG_TIME	1000
/*
static void  func_for_debug(void) 
{

	FS(("status: "));
	FB((g_ev_head));
	FB((g_ev_tail));
	FB((g_ev_len));
	FS(("\n"));


	return;
}
*/
//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void)
{
	u32 cnt = 0;

	Init_Device();	  //use code generate by silicon tool.

	TI0 = 1; 			//make uart0 in send enable status
//-----------------------------------------
	event_init();
//-----------------------------------------add a timer event for printf
#if 0
{
   	timer_ev_unit_st xdata unit;
	IE |= 0x02;		//timer 0 interrupt enable
	TCON |= 0x10; 	//timer 0 enable
	timer_event_init();
	unit.event = EVENT_ID_TIMER_DEBUG;
	unit.time = TRIG_TIME;
	unit.callback = func_for_debug;
	timer_event_add(&unit);
}
#endif
//-----------------------------------------




	report_handler_init();

	while(cnt++<65536);
	config_sensor();

//	test_func();

 	Usb_Init();
	
	Interrupts_Init();	   //open relative interrupt.


	while (1)
	{
		event_process();
		//SendPacket (REPORT_ID_IN_IMAGE);	
		
		get_frame_data();	
	}
}

