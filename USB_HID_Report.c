#include "USB_HID_comp_Type.h"
#include "USB_Configuration.h"
#include "USB_Standard_Requests.h"
#include "USB_HID_Report.h"
#include "USB_Register.h"
#include "type.h"
#include "USB_Descriptor.h"

#define REPORT_ID_MTOUCH		0x01
#define REPORT_ID_FEATURE		0x22
#define REPORT_ID_DEBUGINFO  	0x33



BYTE xdata InOut_Packet1[ EP1_REPORT_SIZE ];	// Last packet received from host
BYTE xdata InOut_Packet2[ EP2_REPORT_SIZE ];	// Last packet received from host
BYTE xdata InOut_Packet3[ EP3_REPORT_SIZE ];	// Last packet received from host


#if (defined USE_EP1_OUT) && !(defined ENABLE_EP1_OUT_ISO)
	extern volatile bit Ep_StatusOUT1	;
#endif
#if (defined USE_EP2_OUT) && !(defined ENABLE_EP2_OUT_ISO)
	extern volatile bit Ep_StatusOUT2	;
#endif
#if (defined USE_EP3_OUT) && !(defined ENABLE_EP3_OUT_ISO)
	extern volatile bit Ep_StatusOUT3	;
#endif
#if (defined USE_EP1_IN) && !(defined ENABLE_EP1_IN_ISO)
	extern volatile bit Ep_StatusIN1	;
#endif
#if (defined USE_EP2_IN) && !(defined ENABLE_EP2_IN_ISO)
	extern volatile bit Ep_StatusIN2	;
#endif
#if (defined USE_EP3_IN) && !(defined ENABLE_EP3_IN_ISO)
	extern volatile bit Ep_StatusIN3	;
#endif


extern unsigned char*	g_usb_data_send_buf_ep1 ;
extern unsigned int	g_usb_data_send_transfed_ep1 ;
extern unsigned int	g_usb_data_send_residual_ep1 ;


extern unsigned char*	g_usb_data_send_buf_ep3 ;
extern unsigned int	g_usb_data_send_transfed_ep3 ;
extern unsigned int	g_usb_data_send_residual_ep3 ;

void fill_report_packet( unsigned char report_id ,rp_buff_st*rp_buff)  reentrant
{
	unsigned char i;

	InOut_Packet1[0] = report_id;			

	if(report_id==REPORT_ID_MTOUCH)
	{
		InOut_Packet1[1] = 'a';		
		InOut_Packet1[2] = 'a';			
		InOut_Packet1[3] = 'a';			
		InOut_Packet1[4] = 'a';	

		rp_buff->rp_buff = &InOut_Packet1[0];
		rp_buff->rp_len = EP1_REPORT_SIZE;
	}
	else if(report_id==REPORT_ID_FEATURE)
	{
		InOut_Packet2[1] = 2;		
			
		
		rp_buff->rp_buff = &InOut_Packet2[0];
		rp_buff->rp_len = EP1_REPORT_SIZE;

	}
	else if(report_id==REPORT_ID_DEBUGINFO)
	{
		InOut_Packet3[0] = report_id;			
		InOut_Packet3[1] = 'd';		
		InOut_Packet3[2] = 'e';			
		InOut_Packet3[3] = 'b';			
		InOut_Packet3[4] = 'u';	
		InOut_Packet3[5] = 'g';		
	
		for(i=6;i<EP3_REPORT_SIZE;i++)
			InOut_Packet3[i] = i;
			
		
		rp_buff->rp_buff = &InOut_Packet3[0];
		rp_buff->rp_len = EP3_REPORT_SIZE;

	}
}
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

void send_mtouch_packet (void)
	{
		bit EAState;
		rp_buff_st		rp_buff;
	
		EAState = EA;
		EA = 0;

			if((Ep_StatusIN1==EP_IDLE))
			{
				if(0==g_usb_data_send_residual_ep1)
				{					
					fill_report_packet( REPORT_ID_MTOUCH ,&rp_buff);
					g_usb_data_send_buf_ep1 = rp_buff.rp_buff;
					g_usb_data_send_residual_ep1 = rp_buff.rp_len;
					g_usb_data_send_transfed_ep1 = 0;
				}
				POLL_WRITE_BYTE (INDEX, 1); 		// Set index to endpoint 1 registers Put new data on Fifo
				if(g_usb_data_send_residual_ep1>EP1_IN_PACKET_SIZE)
				{
					fifo_write(FIFO_EP1, EP1_IN_PACKET_SIZE,(unsigned char *)(g_usb_data_send_buf_ep1 + g_usb_data_send_transfed_ep1));
					POLL_WRITE_BYTE (EINCSRL, rbInINPRDY);
					g_usb_data_send_transfed_ep1 += EP1_IN_PACKET_SIZE;
					g_usb_data_send_residual_ep1 -= EP1_IN_PACKET_SIZE;
				}
				else
				{
					fifo_write (FIFO_EP1, g_usb_data_send_residual_ep1,(unsigned char *)(g_usb_data_send_buf_ep1+g_usb_data_send_transfed_ep1));
					POLL_WRITE_BYTE (EINCSRL, rbInINPRDY);
					g_usb_data_send_transfed_ep1 += g_usb_data_send_residual_ep1;
					g_usb_data_send_residual_ep1 = 0;
				}
			}									

		EA = EAState;
	}
	
void send_image_packet (void)
{
	bit EAState;
	rp_buff_st		rp_buff;

	EAState = EA;
	EA = 0;

	if((Ep_StatusIN3==EP_IDLE))
	{
		
		if(0==g_usb_data_send_residual_ep3)
		{
			fill_report_packet( REPORT_ID_DEBUGINFO,&rp_buff);
			g_usb_data_send_buf_ep3 = rp_buff.rp_buff;
			g_usb_data_send_residual_ep3 = rp_buff.rp_len;
			g_usb_data_send_transfed_ep3 = 0;
		}
#if 1
		POLL_WRITE_BYTE (INDEX, 3);         // Set index to endpoint 1 registers Put new data on Fifo
		if(g_usb_data_send_residual_ep3>EP3_IN_PACKET_SIZE)
		{
			fifo_write(FIFO_EP3, EP3_IN_PACKET_SIZE,(unsigned char *)(g_usb_data_send_buf_ep3 + g_usb_data_send_transfed_ep3));
			POLL_WRITE_BYTE (EINCSRL, rbInINPRDY);
			g_usb_data_send_transfed_ep3 += EP3_IN_PACKET_SIZE;
			g_usb_data_send_residual_ep3 -= EP3_IN_PACKET_SIZE;
		}
		else
		{
			fifo_write (FIFO_EP3, g_usb_data_send_residual_ep3,(unsigned char *)(g_usb_data_send_buf_ep3+g_usb_data_send_transfed_ep3));
			POLL_WRITE_BYTE (EINCSRL, rbInINPRDY);
			g_usb_data_send_transfed_ep3 += g_usb_data_send_residual_ep3;
		  	g_usb_data_send_residual_ep3 = 0;
		}
#endif
	}                        

	EA = EAState;
}
	
