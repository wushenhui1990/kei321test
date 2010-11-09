//-----------------------------------------------------------------------------
// F3xx_USB0_ReportHandler.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Contains functions and variables dealing with Input and Output
// HID reports.
// How To Test:    See Readme.txt
//
//
// FID:            3XX000007
// Target:         C8051F340
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_BlinkyExample
//
// Release 1.1
//    -Added feature reports for dimming controls
//    -Added PCA dimmer functionality
//    -16 NOV 2006
// Release 1.0
//    -Initial Revision (PD)
//    -07 DEC 2005
//


// ----------------------------------------------------------------------------
// Header files
// ----------------------------------------------------------------------------
#include "c8051f3xx.h"
#include "F3xx_USB0_Register.h"
#include "F3xx_USB0_ReportHandler.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
//#include "F3xx_USB0_ReportHandler.h"
#include "uart.h"
#include "type.h"
#include "ev.h"




#define  OUT_PACKET_LEN		16
#define  IN_PACKET_LEN		64

unsigned char xdata OUT_PACKET[OUT_PACKET_LEN];// = {0,0,0,0,0,0,0,0,0};
unsigned char xdata IN_PACKET[IN_PACKET_LEN];//  = {0,0,0};
unsigned char xdata cur_cam_idx ;

cam_send_img_stat_st cam_status[CAM_COUNT];


extern u8 code cmd_config_sensor[];
extern u8 code cmd_config_sensor_cnt;

extern void Fifo_Write_Foreground (unsigned char addr, unsigned int uNumBytes, unsigned char * pData);

// ----------------------------------------------------------------------------
// Local Function Prototypes
// ----------------------------------------------------------------------------

// ****************************************************************************
// Add custom Report Handler Prototypes Here
// ****************************************************************************

void IN_Report(void);
void OUT_Report(void);

void send_image_to_host(void);
//void OUT_BLINK_ENABLE(void);
void recv_cmd_from_host(void);
//void OUT_BLINK_RATE(void);
void send_mtouch_to_host(void);
//void FEATURE_BLINK_DIMMER_INPUT (void);
//void FEATURE_BLINK_DIMMER_OUTPUT (void);
// ----------------------------------------------------------------------------
// Local Definitions
// ----------------------------------------------------------------------------

// ****************************************************************************
// Set Definitions to sizes corresponding to the number of reports
// ****************************************************************************

//#define IN_VECTORTABLESize 3
//#define OUT_VECTORTABLESize 4
#define IN_VECTORTABLESize 2
#define OUT_VECTORTABLESize 1

// ----------------------------------------------------------------------------
// Global Constant Declaration
// ----------------------------------------------------------------------------


// ****************************************************************************
// Link all Report Handler functions to corresponding Report IDs
// ****************************************************************************

const VectorTableEntry IN_VECTORTABLE[IN_VECTORTABLESize] =
{
   // FORMAT: Report ID, Report Handler
   REPORT_ID_IN_IMAGE, send_image_to_host,
   REPORT_ID_IN_MTOUCH, send_mtouch_to_host,
//   FEATURE_BLINK_DIMMERID, FEATURE_BLINK_DIMMER_INPUT
};

// ****************************************************************************
// Link all Report Handler functions to corresponding Report IDs
// ****************************************************************************
const VectorTableEntry OUT_VECTORTABLE[OUT_VECTORTABLESize] =
{
   // FORMAT: Report ID, Report Handler
  // OUT_BLINK_ENABLEID, OUT_BLINK_ENABLE,
   REPORT_ID_OUT_CMD, recv_cmd_from_host,
 //  OUT_BLINK_RATEID, OUT_BLINK_RATE,
 //  FEATURE_BLINK_DIMMERID, FEATURE_BLINK_DIMMER_OUTPUT

};


// ----------------------------------------------------------------------------
// Global Variable Declaration
// ----------------------------------------------------------------------------

BufferStructure IN_BUFFER, OUT_BUFFER;

// ----------------------------------------------------------------------------
// Local Variable Declaration
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Local Functions
// ----------------------------------------------------------------------------

// ****************************************************************************
// Add all Report Handler routines here
// ****************************************************************************


// ****************************************************************************
// For Input Reports:
// Point IN_BUFFER.Ptr to the buffer containing the report
// Set IN_BUFFER.Length to the number of bytes that will be
// transmitted.
//
// REMINDER:
// Systems using more than one report must define Report IDs
// for each report.  These Report IDs must be included as the first
// byte of an IN report.
// Systems with Report IDs should set the FIRST byte of their buffer
// to the value for the Report ID
// AND
// must transmit Report Size + 1 to include the full report PLUS
// the Report ID.
//
// ****************************************************************************

// ----------------------------------------------------------------------------
// send_image_to_host()
// ----------------------------------------------------------------------------
// This handler formats a BLINK_SELECTOR report, which requests a new
// blink pattern from the host application.
//
//	id, cam, idx, len, +data
//
//-----------------------------------------------------------------------------
void send_image_to_host(void)
{
	unsigned char idata i;

	cur_cam_idx = 0;

	IN_PACKET[0] = REPORT_ID_IN_IMAGE;	 //id
	
	IN_PACKET[1] = cam_status[cur_cam_idx].cam_num;	   		//which sensor ,left or right
	IN_PACKET[2] = cam_status[cur_cam_idx].send_cur_idx++;
	IN_PACKET[3] = 60;

	cam_status[cur_cam_idx].send_cur_idx%=cam_status[cur_cam_idx].send_tot_cnt;


	for(i=4;i<=REPORT_ID_IN_IMAGE_LEN;i++)
	{
		IN_PACKET[i] = i;
	}
	
	IN_BUFFER.Ptr = IN_PACKET;
	
	IN_BUFFER.Length = REPORT_ID_IN_IMAGE_LEN+1;
}

// ----------------------------------------------------------------------------
// IN_Blink_Stats()
// ----------------------------------------------------------------------------
// This routine sends statistics calculated in Blink_Control_...c to
// the host application.
//-----------------------------------------------------------------------------
void send_mtouch_to_host(void)
{
   IN_PACKET[0] = REPORT_ID_IN_MTOUCH;

   IN_BUFFER.Ptr = IN_PACKET;
   IN_BUFFER.Length = REPORT_ID_IN_MTOUCH_LEN + 1;
}


// ****************************************************************************
// For Output Reports:
// Data contained in the buffer OUT_BUFFER.Ptr will not be
// preserved after the Report Handler exits.
// Any data that needs to be preserved should be copyed from
// the OUT_BUFFER.Ptr and into other user-defined memory.
//
// ****************************************************************************


// ----------------------------------------------------------------------------
// OUT_BLINK_ENABLE()
// ----------------------------------------------------------------------------
// This handler saves the Blink enable state sent from the host.
//-----------------------------------------------------------------------------
/*
void OUT_BLINK_ENABLE(void)
{

   BLINK_ENABLE = OUT_BUFFER.Ptr[1];

}
*/


void send_debug_info_to_host(void)
{
	bit EAState;
	unsigned char ControlReg;

	EAState = EA;
	EA = 0;

	POLL_WRITE_BYTE (INDEX, 1);         // Set index to endpoint 1 registers

	// Read contol register for EP 1
	POLL_READ_BYTE (EINCSR1, ControlReg);

	if (EP_STATUS[1] == EP_HALT)        // If endpoint is currently halted,send a stall
	{
		POLL_WRITE_BYTE (EINCSR1, rbInSDSTL);
	}

	else if(EP_STATUS[1] == EP_IDLE)
	{
		// the state will be updated inside the ISR handler
		EP_STATUS[1] = EP_TX;

		if (ControlReg & rbInSTSTL)      // Clear sent stall if last  packet returned a stall
		{
			POLL_WRITE_BYTE (EINCSR1, rbInCLRDT);
		}

		if (ControlReg & rbInUNDRUN)     // Clear underrun bit if it was set
		{
			POLL_WRITE_BYTE (EINCSR1, 0x00);
		}

		// ReportHandler_IN_Foreground (ReportID);

		// Put new data on Fifo
		Fifo_Write_Foreground (FIFO_EP1, IN_BUFFER.Length,(unsigned char *)IN_BUFFER.Ptr);

		POLL_WRITE_BYTE (EINCSR1, rbInINPRDY);
		// Set In Packet ready bit,
	}                                   // indicating fresh data on FIFO 1

	EA = EAState;

}



// ----------------------------------------------------------------------------
// recv_cmd_from_host()
// ----------------------------------------------------------------------------
// This handler saves a blinking pattern sent by the host application.
//-----------------------------------------------------------------------------
extern u8 g_can_send_data;

void recv_cmd_from_host(void)
{

	u8 	idata idx,sen_addr,reg_val,ret;
	u16  reg_addr;

	u8 cmd = OUT_BUFFER.Ptr[1];
	//u8 len = OUT_BUFFER.Ptr[2];

	//FS(("recv cmd:"));
	//FB((cmd));
	//FS(("\n"));
	
	if(cmd ==DATA_CMD_WRITE_REG)
	{
		reg_addr = OUT_BUFFER.Ptr[2]|(OUT_BUFFER.Ptr[3]<<8);
		reg_val  =  OUT_BUFFER.Ptr[4];
		uart_write_reg(reg_addr,reg_val);


		IN_PACKET[0] = REPORT_ID_IN_IMAGE;		//report id
		IN_PACKET[1] = DATA_CMD_WRITE_REG;		//date type
		IN_PACKET[2] = 0; //err code				//err code
		IN_PACKET[3] = OUT_BUFFER.Ptr[2];		//addr
		IN_PACKET[4] = OUT_BUFFER.Ptr[3];		
		IN_PACKET[5] = OUT_BUFFER.Ptr[4];		//val
		IN_BUFFER.Ptr = IN_PACKET;
		IN_BUFFER.Length = REPORT_ID_IN_IMAGE_LEN + 1;

		event_send(EVENT_ID_RETURN_HOST_CMD);

	}
	else if(cmd ==DATA_CMD_READ_REG)
	{
		reg_addr = OUT_BUFFER.Ptr[2]|(OUT_BUFFER.Ptr[3]<<8);
		ret = uart_read_reg(reg_addr,&reg_val);	


		IN_PACKET[0] = REPORT_ID_IN_IMAGE;		//report id
		IN_PACKET[1] = DATA_CMD_READ_REG;		//date type
		IN_PACKET[2] = ret; 					//err code
		IN_PACKET[3] = OUT_BUFFER.Ptr[2];		//addr
		IN_PACKET[4] = OUT_BUFFER.Ptr[3];
		IN_PACKET[5] = OUT_BUFFER.Ptr[4];		//val		
		IN_PACKET[6] = reg_val;		
		IN_BUFFER.Ptr = IN_PACKET;
		IN_BUFFER.Length = REPORT_ID_IN_IMAGE_LEN + 1;

		event_send(EVENT_ID_RETURN_HOST_CMD);


	}
	else if(cmd ==DATA_CMD_I2C_WRITE_REG)
	{
		reg_addr = OUT_BUFFER.Ptr[2];
		reg_val  = OUT_BUFFER.Ptr[3];

		i2c_write_reg(reg_addr,reg_val);

		IN_PACKET[0] = REPORT_ID_IN_IMAGE;		//report id
		IN_PACKET[1] = DATA_CMD_I2C_WRITE_REG;	//date type
		IN_PACKET[2] = 0; 						//err code
		IN_PACKET[3] = OUT_BUFFER.Ptr[2];		//addr
		IN_PACKET[4] = OUT_BUFFER.Ptr[3];		//val		
		IN_BUFFER.Ptr = IN_PACKET;
		IN_BUFFER.Length = REPORT_ID_IN_IMAGE_LEN + 1;

		event_send(EVENT_ID_RETURN_HOST_CMD);

	}
	else if(cmd ==DATA_CMD_I2C_READ_REG)
	{
		reg_addr = OUT_BUFFER.Ptr[2];
		//reg_val  = OUT_BUFFER.Ptr[3];
		ret = i2c_read_reg(reg_addr,&reg_val);

		IN_PACKET[0] = REPORT_ID_IN_IMAGE;		//report id
		IN_PACKET[1] = DATA_CMD_I2C_READ_REG;	//date type
		IN_PACKET[2] = ret; 					//err code
		IN_PACKET[3] = OUT_BUFFER.Ptr[2];		//addr
		IN_PACKET[4] = OUT_BUFFER.Ptr[3];		//val
		IN_PACKET[5] = reg_val;			
		IN_BUFFER.Ptr = IN_PACKET;
		IN_BUFFER.Length = REPORT_ID_IN_IMAGE_LEN + 1;

		event_send(EVENT_ID_RETURN_HOST_CMD);

	}

	else if(cmd ==DATA_CMD_CONFIG_SENSOR)
	{
		for(idx = 0; idx <cmd_config_sensor_cnt; idx++)
		{
			sen_addr = 	cmd_config_sensor[idx<<1];
			reg_val  =  cmd_config_sensor[(idx<<1)+1];
			i2c_write_reg(sen_addr,reg_val);	
		}
		IN_PACKET[0] = REPORT_ID_IN_IMAGE;		//report id
		IN_PACKET[1] = DATA_CMD_CONFIG_SENSOR;	//date type
		IN_PACKET[2] = 0; 					//err code
		IN_BUFFER.Ptr = IN_PACKET;
		IN_BUFFER.Length = REPORT_ID_IN_IMAGE_LEN + 1;

		event_send(EVENT_ID_RETURN_HOST_CMD);

	   	g_can_send_data = 1;
	

	}		

}



// ----------------------------------------------------------------------------
// OUT_BLINK_RATE()
// ----------------------------------------------------------------------------
// This handler saves the two-byte blinking rate sent by the host.
//-----------------------------------------------------------------------------
/*
void OUT_BLINK_RATE(void)
{

   BLINK_RATE = OUT_BUFFER.Ptr[1];
   BLINK_RATE = BLINK_RATE<<8;
   BLINK_RATE = BLINK_RATE+OUT_BUFFER.Ptr[2];
}
  */
// ----------------------------------------------------------------------------
// FEATURE_BLINK_DIMMER_OUTPUT
// ----------------------------------------------------------------------------
// This handler receives a new value for the brightness of the LEDs.
//-----------------------------------------------------------------------------
/*
void FEATURE_BLINK_DIMMER_OUTPUT (void) 
{
   BLINK_DIMMER = OUT_BUFFER.Ptr[1];
   BLINK_DIMMER_SUCCESS = 1;
}
*/
// ----------------------------------------------------------------------------
// FEATURE_BLINK_DIMMER_INPUT
// ----------------------------------------------------------------------------
// This handler outputs a value that tells the host that the new dimmer
// value has been successfully received.
//-----------------------------------------------------------------------------
/*
void FEATURE_BLINK_DIMMER_INPUT (void) 
{
   IN_PACKET[0] = FEATURE_BLINK_DIMMERID;
   IN_PACKET[1] = BLINK_DIMMER_SUCCESS;
   BLINK_DIMMER_SUCCESS = 0;

   IN_BUFFER.Ptr = IN_PACKET;
   IN_BUFFER.Length = FEATURE_BLINK_DIMMERSIZE + 1;

}
*/
// ----------------------------------------------------------------------------
// Global Functions
// ----------------------------------------------------------------------------

// ****************************************************************************
// Configure Setup_OUT_BUFFER
//
// Reminder:
// This function should set OUT_BUFFER.Ptr so that it
// points to an array in data space big enough to store
// any output report.
// It should also set OUT_BUFFER.Length to the size of
// this buffer.
//
// ****************************************************************************

void Setup_OUT_BUFFER(void)
{
   OUT_BUFFER.Ptr = OUT_PACKET;
   OUT_BUFFER.Length = OUT_PACKET_LEN;
}

// ----------------------------------------------------------------------------
// Vector Routines
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ReportHandler_IN...
// ----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - Report ID
//
// These functions match the Report ID passed as a parameter
// to an Input Report Handler.
// the ...FG function is called in the SendPacket foreground routine,
// while the ...ISR function is called inside the USB ISR.  A lock
// is set whenever one function is called to prevent a call from the
// other from disrupting the routine.
// However, this should never occur, as interrupts are disabled by SendPacket
// before USB operation begins.
// ----------------------------------------------------------------------------
void ReportHandler_IN_ISR(unsigned char R_ID)
{
   unsigned char index;

   index = 0;

   while(index <= IN_VECTORTABLESize)
   {
      // Check to see if Report ID passed into function
  	  // matches the Report ID for this entry in the Vector Table
      if(IN_VECTORTABLE[index].ReportID == R_ID)
      {
         IN_VECTORTABLE[index].hdlr();
         break;
      }

      // If Report IDs didn't match, increment the index pointer
      index++;
   }

}
void ReportHandler_IN_Foreground(unsigned char R_ID)
{
   unsigned char index;

   index = 0;

   while(index <= IN_VECTORTABLESize)
   {
      // Check to see if Report ID passed into function
      // matches the Report ID for this entry in the Vector Table
      if(IN_VECTORTABLE[index].ReportID == R_ID)
      {
         IN_VECTORTABLE[index].hdlr();
         break;
      }

      // If Report IDs didn't match, increment the index pointer
      index++;
   }

}

// ----------------------------------------------------------------------------
// ReportHandler_OUT
// ----------------------------------------------------------------------------
//
// Return Value - None
// Parameters - None
//
// This function matches the Report ID passed as a parameter
// to an Output Report Handler.
//
// ----------------------------------------------------------------------------
void ReportHandler_OUT(unsigned char R_ID){

   unsigned char index;

   index = 0;

   while(index <= OUT_VECTORTABLESize)
   {
      // Check to see if Report ID passed into function
      // matches the Report ID for this entry in the Vector Table
      if(OUT_VECTORTABLE[index].ReportID == R_ID)
      {
         OUT_VECTORTABLE[index].hdlr();
         break;
      }

      // If Report IDs didn't match, increment the index pointer
      index++;
   }
}


void report_handler_init(void)
{
 	u8 idata i;
	for(i = 0;i<CAM_COUNT;i++)
	{
	 	cam_status[i].cam_num = i;
		cam_status[i].send_cur_idx = 0;
		cam_status[i].send_tot_cnt = (IMAGE_WIDTH*IMAGE_HEIGHT+(BREAD_ONCE>>1))/BREAD_ONCE;
	}

	event_cb_regist(EVENT_ID_RETURN_HOST_CMD,send_debug_info_to_host);
}
