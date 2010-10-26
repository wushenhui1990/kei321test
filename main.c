//#include <c8051f320.h>
#include "config.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"
#include "flash_rw.h"

#include "USB_INT_to_HID_Type.h"
#include "USB_Configuration.h"
#include "USB_Register.h"
#include "USB_Standard_Requests.h"
#include "USB_ISR.h"
#include "FIFO_RW.h"

#include <intrins.h>		// for _testbit_(), _nop_()


extern unsigned long g_ticks ;
extern void Init_Device(void);


/*
void flash_rw_test(void)
{
   #define FLASH_WRITE_ADDR		0x3900
   #define FLASH_WRITE_LEN		0x40

   	unsigned long start = 0;
	unsigned long end = 0;
	unsigned char idx;	

	u8 src[FLASH_WRITE_LEN] =
	{
	    0xFF, 0xFF, 0x8B, 0xD8, 0x59, 0x85, 0xDB, 0x59, 0x0F, 0x85, 0x39, 0x02, 0x00, 0x00, 0x83, 0x46,
	    0x0C, 0x02, 0x8B, 0x46, 0x0C, 0x56, 0x8A, 0x78, 0xFE, 0x8A, 0x58, 0xFF, 0xE8, 0x48, 0x7E, 0xFF,
	    0xFF, 0x85, 0xDB, 0x59, 0x74, 0x41, 0x8B, 0x46, 0x08, 0x89, 0x45, 0x08, 0x8B, 0x45, 0x10, 0x03,
	    0xD8, 0x53, 0x56, 0xE8, 0xF6, 0x7D, 0xFF, 0xFF, 0x8B, 0xD8, 0x59, 0x85, 0xDB, 0x59, 0x0F, 0x85 
	};

	u8 dst[FLASH_WRITE_LEN] = {0};


 	flash_read(dst,FLASH_WRITE_ADDR,FLASH_WRITE_LEN);

	F(("read_val_1:\n"));
	for(idx=0;idx<FLASH_WRITE_LEN;idx++)
	{
		if(idx&&(!(idx%16)))
			F(("\n"));
		F(("%02bx ",dst[idx]));
	}
	F(("\n"));

	flash_update(FLASH_WRITE_ADDR,src,FLASH_WRITE_LEN);
	flash_read(dst,FLASH_WRITE_ADDR,FLASH_WRITE_LEN);


	F(("read_val_2:\n"));
	for(idx=0;idx<FLASH_WRITE_LEN;idx++)
	{
		if(idx&&(!(idx%16)))
			F(("\n"));
		if(src[idx]!=dst[idx])
			F(("* "));
		else
			F(("%02bx ",dst[idx]));
	}
	F(("\n"));

}
*/

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

void main(void)
{
	Init_Device();

	uart_init();

	F(("\nsizeof(short)=%02bx\nsizeof(int)=%02bx\nsizeof(long)=%02bx\n\n",sizeof(short),sizeof(int),sizeof(long)));

	//flash_rw_test();

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
                    FIFO_Write_generic( FIFO_EP1, EP1_PACKET_SIZE, In_Packet );
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
                    FIFO_Read_generic( FIFO_EP1, EP1_PACKET_SIZE, Out_Packet );
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