//-----------------------------------------------------------------------------
// USB_HID_Requests.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "USB_INT_to_HID_Type.h"
#include "USB_Configuration.h"
#include "USB_Standard_Requests.h"
#include "USB_ISR.h"
//#include "USB_Main.h"
#include "USB_HID_Requests.h"
#include "USB_Descriptor.h"
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static Variables in this file
//-----------------------------------------------------------------------------

/*
#define CS_TEMP_BUF_SIZE			0x10
static BYTE cs_temp_buffer[ CS_TEMP_BUF_SIZE ];
*/

//-----------------------------------------------------------------------------
// Local function prototypes
//-----------------------------------------------------------------------------

static void Get_Report(void);
static void Get_Idle( void );
static void Get_Protocol( void );
static void Set_Report( void );
static void Set_Idle( void );
static void Set_Protocol( void );

//-----------------------------------------------------------------------------
// SDCC suport
//-----------------------------------------------------------------------------
#if defined SDCC
#pragma nooverlay
#endif // SDCC

//-----------------------------------------------------------------------------
// class specific request dispatcher
//-----------------------------------------------------------------------------

void Class_Request( void )
{
	if ( Setup.wIndex.i == DSC_INTERFACE_HID )		// interface index must match to the HID IF
	{
		switch( Setup.bRequest )					// dispatch according to the bRequest
		{
			case HID_REQ_GET_REPORT:		Get_Report();		break;
/*
			case HID_REQ_GET_IDLE:			Get_Idle();			break;
			case HID_REQ_GET_PROTOCOL:		Get_Protocol();		break;
			case HID_REQ_SET_REPORT:		Set_Report();		break;
			case HID_REQ_SET_IDLE: 			Set_Idle();			break;
			case HID_REQ_SET_PROTOCOL: 		Set_Protocol();		break;
*/
			default:											break;
		}
	}
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Get_Report
//-----------------------------------------------------------------------------
//
// HID class specific request - mandatory
// return specified report - Input or Feature report
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Get_Report
//-----------------------------------------------------------------------------
//
//	mandatory to HID class
//	Send report to host via control pipe
//	Input and feature report are supported
//
//-----------------------------------------------------------------------------
//extern BYTE   idata Out_Packet[ EP1_PACKET_SIZE ];		// Last packet received from host
BYTE   In_Packet[ EP1_PACKET_SIZE+32 ];		// Next packet to sent to host


/*
#define REPORTID_MTOUCH		0x01
#define REPORTID_DEBUGINFO  0x22
#define REPORTID_FEATURE	0x33
#define REPORTID_CONTROL	0x44
*/


void fill_report_packet( unsigned char report_id ,rp_buff_st*rp_buff)
{
	unsigned char i;
	if(report_id==REPORTID_MTOUCH)
	{
		In_Packet[0] = REPORTID_MTOUCH;			
		In_Packet[1] = 'a';		
		In_Packet[2] = 'a';			
		In_Packet[3] = 'a';			
		In_Packet[4] = 'a';	

		rp_buff->rp_buff = &In_Packet[0];
		rp_buff->rp_len = 0;
	}
	else if(report_id==REPORTID_DEBUGINFO)
	{
		In_Packet[0] = REPORTID_DEBUGINFO;			
		In_Packet[1] = 'd';		
		In_Packet[2] = 'e';			
		In_Packet[3] = 'b';			
		In_Packet[4] = 'u';	
		In_Packet[5] = 'g';		
	
		for(i=6;i<=REPORT_DEBUG_INFO_LEN;i++)
			In_Packet[i] = i;
			
		
		rp_buff->rp_buff = &In_Packet[0];
		rp_buff->rp_len = REPORT_DEBUG_INFO_LEN+1;

	}
	else if(report_id==REPORTID_FEATURE)
	{
		In_Packet[0] = REPORTID_FEATURE;			
		In_Packet[1] = 2;		
			
		
		rp_buff->rp_buff = &In_Packet[0];
		rp_buff->rp_len = 2;

	}
	else if(report_id==REPORTID_CONTROL)
	{
		In_Packet[0] = REPORTID_CONTROL;			
		In_Packet[1] = 'd';		
		In_Packet[2] = 'd';			
		In_Packet[3] = 'd';			
		In_Packet[4] = 'd';	
		
		rp_buff->rp_buff = &In_Packet[0];
		rp_buff->rp_len = 0;
	}
}

static void Get_Report( void )
{
	if (   (Setup.bmRequestType == IN_CL_INTERFACE)	)
	//	&& (Setup.wValue.c[LSB] == 0) )				// Report ID
	{
		rp_buff_st rp_buff;
		
		switch ( Setup.wValue.c[MSB] )
		{
			case HID_REPORT_TYPE_INPUT:				// Input report
				fill_report_packet(Setup.wValue.c[LSB],&rp_buff);
				DataPtr = rp_buff.rp_buff;
				DataSize = rp_buff.rp_len;//EP1_PACKET_SIZE;
				setup_handled = TRUE;
				break;

			case HID_REPORT_TYPE_FEATURE:			// Feature report
				fill_report_packet(Setup.wValue.c[LSB],&rp_buff);
				DataPtr = rp_buff.rp_buff;
				DataSize = rp_buff.rp_len;;
				setup_handled = TRUE;
				break;

			default:
				break;
		}
	}

	if ( setup_handled )
	{
		// Set serviced Setup Packet, Endpoint 0 intransmit mode, 
		Ep_Status0 = EP_TX;						
		if ( DataSize > Setup.wLength.i )
			DataSize = Setup.wLength.i;				// Send only requested amount of data
	}
}

/*
//-----------------------------------------------------------------------------
// Set_Report
//-----------------------------------------------------------------------------
//
//	Receive report from host via control pipe
//	Output and feature report are supported
//
//-----------------------------------------------------------------------------

static void Set_Report( void )
{
	if (   (Setup.bmRequestType == OUT_CL_INTERFACE)
		&& (Setup.wValue.c[LSB] == 0) )				// Report ID
	{
		switch ( Setup.wValue.c[MSB] )
		{
			case HID_REPORT_TYPE_OUTPUT:			// Output report
				DataPtr = (BYTE *)cs_temp_buffer;
				setup_handled = TRUE;
				break;

			case HID_REPORT_TYPE_FEATURE:			// Feature report
				DataPtr = (BYTE *)cs_temp_buffer;
				setup_handled = TRUE;
				break;

			default:
				break;
		}
	}

	if ( setup_handled )
	{
		// Set serviced Setup Packet, Endpoint 0 intransmit mode, 
		Ep_Status0 = EP_RX;						
		DataSize = CS_TEMP_BUF_SIZE;
		if ( DataSize > Setup.wLength.i )
			DataSize = Setup.wLength.i;				// Send only requested amount of data
	}
}

//-----------------------------------------------------------------------------
// Get_Idle
//-----------------------------------------------------------------------------
//	
//	return current idle rate for a specified input report over interrupt IN pipe
//
//-----------------------------------------------------------------------------

static void Get_Idle( void )
{
	if (   (Setup.bmRequestType == IN_CL_INTERFACE)
		&& (Setup.wValue.c[MSB] == 0)			// must be 0
		&& (Setup.wLength.i == 1) )				// request size is always 1
	{
		switch ( Setup.wValue.c[LSB] )				// Report ID
		{
			case 1:
				cs_temp_buffer[0] = IDLE_RATE_1;
				DataPtr = (BYTE *)cs_temp_buffer;
				setup_handled = TRUE;
				break;

			default:
				break;
	}

	if ( setup_handled )
	{
		// Set serviced Setup Packet, Endpoint 0 intransmit mode, 
		Ep_Status0 = EP_TX;						
		DataSize = 1;
	}
}

//-----------------------------------------------------------------------------
// Set_Idle
//-----------------------------------------------------------------------------
//
//	Set idle rate to specified duration
//	  When duration equals to zero, stop the report
//
//-----------------------------------------------------------------------------

static void Set_Idle( void )
{
	if (   (Setup.bmRequestType == OUT_CL_INTERFACE)
		&& (Setup.wLength.i == 0) )					// Length must be 0
	{
		switch ( Setup.wValue.c[LSB] )					// Report ID
		{
			case 0:
				IDLE_RATE_1 = Setup.wValue.c[MSB];		// set Duration to all report ID
				IDLE_RATE_2 = IDLE_RATE_1;
				setup_handled = TRUE;
				break;

			case 1:
				IDLE_RATE_1 = Setup.wValue.c[MSB];		// set Duration to specified report ID
				setup_handled = TRUE;
				break;

			default:
				break;
		}
	}
}

//-----------------------------------------------------------------------------
// Get_Protocol
//-----------------------------------------------------------------------------
//
//	Just for boot device (mouse/keyboard)
//	return active protocol, either boot or report
//
//-----------------------------------------------------------------------------

bit cs_boot_protocol = HID_PROTOCOL_REPORT;

static uchar code cs_zero[1] = { 0 };
static uchar code cs_one[1]  = { 1 };

static void Get_Protocol( void )
{
	if (   (Setup.bmRequestType == IN_CL_INTERFACE)
		&& (Setup.wValue.i == 0)				// must be 0
		&& (Setup.wLength.i == 1) )				// request size is always 1
	{
		if ( cs_boot_protocol == HID_PROTOCOL_BOOT )
			DataPtr = (BYTE *)cs_zero;
		else
			DataPtr = (BYTE *)cs_one;
		DataSize = 1;
		Ep_Status0 = EP_TX;						
		setup_handled = TRUE;
	}
}

//-----------------------------------------------------------------------------
// Set_Protocol
//-----------------------------------------------------------------------------
//
//	Just for boot device
//	Switch active protocol
//
//-----------------------------------------------------------------------------

static void Set_Protocol( void )
{
	if (   (Setup.bmRequestType == OUT_CL_INTERFACE)
		&& (Setup.wValue.c[MSB] == 0)			// upper byte of protocol selector
		&& (Setup.wLength.i == 0) )				// Length must be 0
	{
		switch ( Setup.wValue.c[LSB] )			// 
		{
			case HID_PROTOCOL_BOOT:
				cs_boot_protocol = HID_PROTOCOL_BOOT;
				setup_handled = TRUE;
				break;

			case HID_PROTOCOL_REPORT:
				cs_boot_protocol = HID_PROTOCOL_REPORT;
				setup_handled = TRUE;
				break;

			default:
				break;
		}
	}
}

*/


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
