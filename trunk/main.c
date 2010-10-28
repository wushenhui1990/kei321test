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

#include <intrins.h>	// for _testbit_(), _nop_()




//BYTE   idata Out_Packet[ EP1_PACKET_SIZE ];		// Last packet received from host
BYTE   idata In_Packet[ EP1_PACKET_SIZE ];		// Next packet to sent to host


extern void fill_report_packet( unsigned char report_id );

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

void send_packet (unsigned char report_id)
{
    unsigned char EUSB0_save;
    if ( Ep_StatusIN1 != EP_HALT )
    {
        if ( _testbit_( IN_FIFO_empty ) )
        {
            fill_report_packet(report_id);							// fill IN buffer

            										// To prevent conflict with USB interrupt
            EUSB0_save = EIE1 & 0x02;				//   save USB interrupt enable bit
            EIE1 &= ~0x02;							//   disable USB interrupt temporarily
            POLL_WRITE_BYTE( INDEX, 1 );			// Load packet to FIFO
            fifo_write( FIFO_EP1, EP1_PACKET_SIZE, In_Packet );
            POLL_WRITE_BYTE( EINCSRL, rbInINPRDY );	// set FIFO ready flag
            EIE1 |= EUSB0_save;						// restore USB interrupt
        }
    }

}
extern unsigned	long g_ticks ;
extern void Init_Device(void);
extern void test_func(void);
extern u8 	g_ev_head;
extern u8 	g_ev_tail;
extern u8 	g_ev_len;

static void  func_for_debug(void) 
{
	F(("status:%ld [%02bx][%02bx][%02bx]\n",g_ticks/1000,g_ev_head,g_ev_tail,g_ev_len));
	send_packet(REPORTID_DEBUGINFO);
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
	unit.time = 1000;
	unit.callback = func_for_debug;
	timer_event_add(&unit);
#endif
//-----------------------------------------

	test_func();

   	Usb0_Init();

	while(1)
	{

		event_process();
 	#if 0	
		{
            // task1: USB IN EP handling
            unsigned char EUSB0_save;
            if ( Ep_StatusIN1 != EP_HALT )
            {
                if ( _testbit_( IN_FIFO_empty ) )
                {
                    fill_packet();							// fill IN buffer

                    										// To prevent conflict with USB interrupt
                    EUSB0_save = EIE1 & 0x02;				//   save USB interrupt enable bit
                    EIE1 &= ~0x02;							//   disable USB interrupt temporarily
                    POLL_WRITE_BYTE( INDEX, 1 );			// Load packet to FIFO
                    //FIFO_Write_idata( FIFO_EP1, EP1_PACKET_SIZE, In_Packet );
                    //FIFO_Write_generic( FIFO_EP1, EP1_PACKET_SIZE, In_Packet );
                    fifo_write( FIFO_EP1, EP1_PACKET_SIZE, In_Packet );
                    //FIFO_Write( FIFO_EP1, EP1_PACKET_SIZE, In_Packet );
                    POLL_WRITE_BYTE( EINCSRL, rbInINPRDY );	// set FIFO ready flag
                    EIE1 |= EUSB0_save;						// restore USB interrupt
                }
            }
        }
		{
            // task2: USB OUT EP handling
            unsigned char EUSB0_save;

            if ( Ep_StatusOUT1 != EP_HALT )
            {
                // atomic access handling
                if ( _testbit_( OUT_FIFO_loaded ) )  	// when FIFO is loaded
                {
                    // To prevent conflict with USB interrupt
                    EUSB0_save = EIE1 & 0x02;				//   save USB interrupt enable bit
                    EIE1 &= ~0x02;							//   disable USB interrupt temporarily
                    POLL_WRITE_BYTE( INDEX, 1 );			// unload packet from FIFO
                    //FIFO_Read_idata( FIFO_EP1, EP1_PACKET_SIZE, Out_Packet );
                    //FIFO_Read_generic( FIFO_EP1, EP1_PACKET_SIZE, Out_Packet );
					fifo_read( FIFO_EP1, EP1_PACKET_SIZE, Out_Packet );
                    POLL_WRITE_BYTE( EOUTCSRL, 0 );			// Clear Out Packet ready bit
                    EIE1 |= EUSB0_save;						// restore USB interrupt

                    // Apply data to ports
                    //if (Out_Packet[0] == 1) Led1 = 1;		// Update status of LED #1
                    //else					Led1 = 0;
                    //if (Out_Packet[1] == 1) Led2 = 1;		// Update status of LED #2
                    //else 					Led2 = 0;
                    //P1 = (Out_Packet[2] & 0x0F);			// Set Port 1 pins
                }
            }
        }
		#endif
		
	}

}														  