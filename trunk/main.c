//#include <c8051f320.h>
#include "config.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"
#include "ev.h"
#include "ev_timer.h"

#include "USB_INT_to_HID_Type.h"
#include "USB_Configuration.h"
#include "USB_Register.h"
#include "USB_Standard_Requests.h"
#include "USB_ISR.h"
#include "USB_Descriptor.h"
#include "USB_HID_Requests.h"
//#include <intrins.h>	// for _testbit_(), _nop_()




//BYTE   idata Out_Packet[ EP1_PACKET_SIZE ];		// Last packet received from host
extern BYTE    In_Packet[ EP1_PACKET_SIZE ];		// Next packet to sent to host
extern unsigned char*	g_usb_data_send_buf;
extern unsigned int		g_usb_data_send_transfed ;
extern unsigned int		g_usb_data_send_residual ;


extern void fill_report_packet( unsigned char report_id ,rp_buff_st*rp_buff) reentrant;

//extern void fill_report_packet( unsigned char report_id );

/*
void fill_packet( )
{
	In_Packet[0] = 'h';			
	In_Packet[1] = 'e';		
	In_Packet[2] = 'l';			
	In_Packet[3] = 'l';			
	In_Packet[4] = 'o';			
}
*/


/*

void fifo_read(u8 addr, u8 uNumBytes, u8 * pData)
{
	u8 idx;

	while(USB0ADR & 0x80);				// Wait for BUSY->'0'
	USB0ADR = addr | 0xC0;				// Set address
										// Set auto-read and initiate first read
										// Read <NumBytes> from the selected FIFO
	for ( idx = 0; idx < uNumBytes; idx++ )
	{
		while(USB0ADR & 0x80);			// Wait for BUSY->'0' (read complete)
		pData[ idx ] = USB0DAT;
	}
	USB0ADR = 0;						// Clear auto-read
}
*/ 
void fifo_write(u8 addr, u8 uNumBytes, u8 * pData)
{
	u8 idx;

	while(USB0ADR & 0x80);					// Wait for BUSY->'0'
	USB0ADR = (addr & 0x3F);				// Set address (mask out bits7-6)
											// Write <NumBytes> to the selected FIFO
	for ( idx = 0; idx < uNumBytes; idx++ )
	{
		while(USB0ADR & 0x80);				// Wait for BUSY->'0' (write complete)
		USB0DAT = pData[ idx ];
	}
}


void send_packet (unsigned char ReportID)
{
	bit EAState;
//	unsigned char   ControlReg;
	rp_buff_st		rp_buff;
	//   volatile unsigned char i =0;

	EAState = EA;
	EA = 0;

//	POLL_WRITE_BYTE (INDEX, 1);         // Set index to endpoint 1 registers

	// Read contol register for EP 1
//	POLL_READ_BYTE (EINCSRL, ControlReg);

	//if (Ep_StatusIN1== EP_HALT)        // If endpoint is currently halted,
	//                               // send a stall
	//{
	//	POLL_WRITE_BYTE (EINCSRL, rbInSDSTL);
//	}

//	else if(Ep_StatusIN1== EP_IDLE)
//	if((Ep_StatusIN1!=EP_HALT) && _testbit_( IN_FIFO_empty ))
	if((Ep_StatusIN1==EP_IDLE))
	{
		//Ep_StatusIN1 = EP_TX;			// the state will be updated inside the ISR handler

		//if (ControlReg & rbInSTSTL)      // Clear sent stall if last packet returned a stall
		//{
		//	POLL_WRITE_BYTE (EINCSRL, rbInCLRDT);
		//}

	//	if (ControlReg & rbInUNDRUN)     // Clear underrun bit if it was set
	//	{
	//		POLL_WRITE_BYTE (EINCSRL, 0x00);
	//	}
		
		if(0==g_usb_data_send_residual)
		{
			//ReportHandler_IN_Foreground (ReportID);
			
			fill_report_packet( ReportID ,&rp_buff);
			g_usb_data_send_buf = rp_buff.rp_buff;
			g_usb_data_send_residual = rp_buff.rp_len;
			g_usb_data_send_transfed = 0;
		}
#if 1
		POLL_WRITE_BYTE (INDEX, 1);         // Set index to endpoint 1 registers
		// Put new data on Fifo
		if(g_usb_data_send_residual>EP1_PACKET_SIZE)
		{
			fifo_write(FIFO_EP1, EP1_PACKET_SIZE,(unsigned char *)(g_usb_data_send_buf + g_usb_data_send_transfed));
			POLL_WRITE_BYTE (EINCSRL, rbInINPRDY);
			g_usb_data_send_transfed += EP1_PACKET_SIZE;
			g_usb_data_send_residual -= EP1_PACKET_SIZE;
		}
		else
		{
			fifo_write (FIFO_EP1, g_usb_data_send_residual,(unsigned char *)(g_usb_data_send_buf+g_usb_data_send_transfed));
			POLL_WRITE_BYTE (EINCSRL, rbInINPRDY);
			g_usb_data_send_transfed += g_usb_data_send_residual;
		  	g_usb_data_send_residual = 0;
		}
#endif
		//F(("in rpt!!\n"));
	}                                   // indicating fresh data on FIFO 1

	EA = EAState;
}


/*
void send_packet (unsigned char report_id)
{
    unsigned char EUSB0_save;
	rp_buff_st rp_buff;

    if ( Ep_StatusIN1 != EP_HALT )
    {
        if ( _testbit_( IN_FIFO_empty ) )
        {
            fill_report_packet(report_id,&rp_buff);							// fill IN buffer

            										// To prevent conflict with USB interrupt
            EUSB0_save = EIE1 & 0x02;				//   save USB interrupt enable bit
            EIE1 &= ~0x02;							//   disable USB interrupt temporarily
            POLL_WRITE_BYTE( INDEX, 1 );			// Load packet to FIFO
            //fifo_write( FIFO_EP1, EP1_PACKET_SIZE, In_Packet );
            fifo_write( FIFO_EP1, rp_buff.rp_len, rp_buff.rp_buff );
           	POLL_WRITE_BYTE( EINCSRL, rbInINPRDY );	// set FIFO ready flag
            EIE1 |= EUSB0_save;						// restore USB interrupt
        }
    }

}
*/


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

	//while(i--)
//	if(Ep_StatusIN1)
//		F(("ep1_sta: 1\n"));	  
//	else
// 		F(("ep1_sta: 0\n"));	  
	//	if ( Ep_StatusIN1 != EP_HALT )
			//send_packet(REPORTID_DEBUGINFO);
			
			send_packet(REPORTID_MTOUCH);

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
