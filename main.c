//#include <c8051f320.h>
#include "config.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"
#include "ev.h"
#include "ev_timer.h"

#include "USB_HID_comp_Type.h"
#include "USB_Configuration.h"
#include "USB_Register.h"
#include "USB_Standard_Requests.h"
#include "USB_ISR.h"
#include "USB_Descriptor.h"
#include "USB_HID_Requests.h"
#include "USB_HID_Report.h"





extern unsigned	long g_ticks ;
extern void Init_Device(void);
extern void test_func(void);
extern u8 	g_ev_head;
extern u8 	g_ev_tail;
extern u8 	g_ev_len;


#define	TRIG_TIME	10
static void  func_for_debug(void) 
{

	F(("status:%ld [%02bx][%02bx][%02bx]\n",g_ticks/1000,g_ev_head,g_ev_tail,g_ev_len));
			
	//send_packet(1);
	//send_packet(0x33);
	if(g_ticks>5000)
		send_image_packet();

	return;
}

void main(void)
{
	timer_ev_unit_st xdata unit;

	Init_Device();

#if(UART_DEBUG==1)
	uart_init();
#endif

//-----------------------------------------
	event_init();
	timer_event_init();

//-----------------------------------------add a timer eve
#if 1
	unit.event = EVENT_ID_TIMER_DEBUG;
	unit.time = TRIG_TIME;
	unit.callback = func_for_debug;
	timer_event_add(&unit);
#endif
//-----------------------------------------

	test_func();

   	Usb0_Init();


	while(1)
	{
		event_process();
		
	}

}														  
