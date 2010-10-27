//#include <c8051f320.h>
#include "config.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"

#include "USB_INT_to_HID_Type.h"
#include "USB_Configuration.h"
#include "USB_Register.h"
#include "USB_Standard_Requests.h"
#include "USB_ISR.h"

#include <intrins.h>	// for _testbit_(), _nop_()


extern unsigned long g_ticks ;
extern void Init_Device(void);


BYTE   idata Out_Packet[ EP1_PACKET_SIZE ];		// Last packet received from host
BYTE   idata In_Packet[ EP1_PACKET_SIZE ];		// Next packet to sent to host


void fill_packet( void )
{
	In_Packet[0] = 'h';			
	In_Packet[1] = 'e';		
	In_Packet[2] = 'l';			
	In_Packet[3] = 'l';			
	In_Packet[4] = 'o';			
}
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
 
extern void test_func(void);

void main(void)
{
	Init_Device();

	uart_init();

	F(("\nsizeof(short)=%02bx\nsizeof(int)=%02bx\nsizeof(long)=%02bx\n\n",sizeof(short),sizeof(int),sizeof(long)));

	test_func();

   	Usb0_Init();

	while(1)
	{
	 	if(!(g_ticks%1000))
		{
			EA = 0;
			F(("g_ticks:%ld\n",g_ticks/1000));
			EA = 1;
		}
 	#if 1	
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