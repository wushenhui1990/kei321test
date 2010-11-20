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
#include "fpga_define.h"
#include "INTRINS.H"




#define  OUT_PACKET_LEN		16
#define  IN_PACKET_LEN		64

u8	xdata OUT_PACKET[OUT_PACKET_LEN];	// = {0,0,0,0,0,0,0,0,0};
u8  xdata IN_PACKET[IN_PACKET_LEN];	//  = {0,0,0};
u8  xdata cur_cam_idx ;
cam_send_img_stat_st cam_status[CAM_COUNT];

//u8 IN_PACKET[8];

extern u8 code cmd_config_sensor[];
extern u8 code cmd_config_sensor_cnt;
extern u8	g_work_style;

extern void Fifo_Write_Foreground (u8 addr, unsigned int uNumBytes, u8 * pData);


// ----------------------------------------------------------------------------
// Global Variable Declaration
// ----------------------------------------------------------------------------

BufferStructure IN_BUFFER, OUT_BUFFER;

extern u8 IN_PACKET[];

void send_debug_info_to_host(u8 rid)
{
	bit EAState;
	u8 ControlReg;

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

		if(rid == REPORT_ID_IN_DBGINFO)
		{
			IN_BUFFER.Ptr = IN_PACKET;
			IN_BUFFER.Length = REPORT_ID_IN_DBGINFO_LEN + 1;
		}
		else if(rid == REPORT_ID_IN_MTOUCH)
		{
	   		IN_BUFFER.Ptr = IN_PACKET;
			IN_BUFFER.Length = REPORT_ID_IN_MTOUCH_LEN + 1;
		}
		// Put new data on Fifo
		Fifo_Write_Foreground (FIFO_EP1, IN_BUFFER.Length,(u8 *)IN_BUFFER.Ptr);

		POLL_WRITE_BYTE (EINCSR1, rbInINPRDY);
		// Set In Packet ready bit,
	}                                   // indicating fresh data on FIFO 1

	EA = EAState;

}

void handle_report_out(void)
{
	u8 	idata idx,sen_addr,reg_val,ret,cmd;
	u16 idata reg_addr;


	cmd = OUT_BUFFER.Ptr[1];
	
	if(cmd ==DATA_CMD_WRITE_REG)
	{
		reg_addr = OUT_BUFFER.Ptr[2]|(OUT_BUFFER.Ptr[3]<<8);
		reg_val  =  OUT_BUFFER.Ptr[4];
		uart_write_reg(reg_addr,reg_val);

		IN_PACKET[0] = REPORT_ID_IN_DBGINFO;		//report id
		IN_PACKET[1] = DATA_CMD_WRITE_REG;		//date type
		IN_PACKET[2] = 0; //err code				//err code
		IN_PACKET[3] = OUT_BUFFER.Ptr[2];		//addr
		IN_PACKET[4] = OUT_BUFFER.Ptr[3];		
		IN_PACKET[5] = OUT_BUFFER.Ptr[4];		//val

		//event_send(EVENT_ID_RETURN_HOST_CMD);
		send_debug_info_to_host(REPORT_ID_IN_DBGINFO);

	}
	else if(cmd ==DATA_CMD_READ_REG)
	{
		reg_addr = OUT_BUFFER.Ptr[2]|(OUT_BUFFER.Ptr[3]<<8);
		ret = uart_read_reg(reg_addr,&reg_val);	

 		IN_PACKET[0] = REPORT_ID_IN_DBGINFO;		//report id
		IN_PACKET[1] = DATA_CMD_READ_REG;		//date type
		IN_PACKET[2] = ret; 					//err code
		IN_PACKET[3] = OUT_BUFFER.Ptr[2];		//addr
		IN_PACKET[4] = OUT_BUFFER.Ptr[3];
		IN_PACKET[5] = OUT_BUFFER.Ptr[4];		//val		
		IN_PACKET[6] = reg_val;		

		//event_send(EVENT_ID_RETURN_HOST_CMD);
		send_debug_info_to_host(REPORT_ID_IN_DBGINFO);
	}
	else if(cmd ==DATA_CMD_I2C_WRITE_REG)
	{
		reg_addr = OUT_BUFFER.Ptr[2];
		reg_val  = OUT_BUFFER.Ptr[3];

		i2c_write_reg(reg_addr,reg_val);
 		IN_PACKET[0] = REPORT_ID_IN_DBGINFO;		//report id
		IN_PACKET[1] = DATA_CMD_I2C_WRITE_REG;	//date type
		IN_PACKET[2] = 0; 						//err code
		IN_PACKET[3] = OUT_BUFFER.Ptr[2];		//addr
		IN_PACKET[4] = OUT_BUFFER.Ptr[3];		//val		

		//event_send(EVENT_ID_RETURN_HOST_CMD);
		send_debug_info_to_host(REPORT_ID_IN_DBGINFO);

	}
	else if(cmd ==DATA_CMD_I2C_READ_REG)
	{
		reg_addr = OUT_BUFFER.Ptr[2];
		//reg_val  = OUT_BUFFER.Ptr[3];
		ret = i2c_read_reg(reg_addr,&reg_val);

		IN_PACKET[0] = REPORT_ID_IN_DBGINFO;		//report id
		IN_PACKET[1] = DATA_CMD_I2C_READ_REG;	//date type
		IN_PACKET[2] = ret; 					//err code
		IN_PACKET[3] = OUT_BUFFER.Ptr[2];		//addr
		IN_PACKET[4] = OUT_BUFFER.Ptr[3];		//val
		IN_PACKET[5] = reg_val;			

	//	event_send(EVENT_ID_RETURN_HOST_CMD);
		send_debug_info_to_host(REPORT_ID_IN_DBGINFO);

	}

	else if(cmd ==DATA_CMD_SWITCH_WORK_STYLE)
	{

		g_work_style = OUT_BUFFER.Ptr[2];

		if(	g_work_style == WORK_STYLE_PREVIEW)
		{
			for(idx = 0; idx <cmd_config_sensor_cnt; idx++)
			{
				sen_addr = 	cmd_config_sensor[idx<<1];
				reg_val  =  cmd_config_sensor[(idx<<1)+1];
				i2c_write_reg(sen_addr,reg_val);	
			}
		}

		IN_PACKET[0] = REPORT_ID_IN_DBGINFO;		//report id
		IN_PACKET[1] = DATA_CMD_SWITCH_WORK_STYLE;	//date type
		IN_PACKET[2] = OUT_BUFFER.Ptr[2];
		IN_PACKET[3] = 0;
		 					//err code
//		event_send(EVENT_ID_RETURN_HOST_CMD);
		send_debug_info_to_host(REPORT_ID_IN_DBGINFO);


	}



}


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
void ReportHandler_IN_ISR(u8 R_ID)
{
	if(R_ID==REPORT_ID_IN_MTOUCH)
	{
	   IN_PACKET[0] = REPORT_ID_IN_MTOUCH;
	
	   IN_BUFFER.Ptr = IN_PACKET;
	   IN_BUFFER.Length = REPORT_ID_IN_MTOUCH_LEN + 1;
	}	
	else if(R_ID == REPORT_ID_IN_FEATURE)//future
	{
		IN_PACKET[0] = R_ID;
		IN_PACKET[1] = 2;
		IN_BUFFER.Ptr = IN_PACKET;
		IN_BUFFER.Length = 2;
	
	}
}


void report_handler_init(void)
{
 	u8 idata i,read_cnt,remain;
	
   	read_cnt = 	(IMAGE_WIDTH*IMAGE_HEIGHT)/BREAD_ONCE;
	remain   =  (IMAGE_WIDTH*IMAGE_HEIGHT)%BREAD_ONCE;

	if(remain)
		read_cnt++;
	
	for(i = 0;i<CAM_COUNT;i++)
	{
	 	cam_status[i].cam_num = i;
		cam_status[i].send_cur_idx = 0;
		cam_status[i].send_tot_cnt = read_cnt;
		cam_status[i].remain = (remain? remain:BREAD_ONCE);
	}

	//event_cb_regist(EVENT_ID_RETURN_HOST_CMD,send_switch_style_cmd_return);
	event_cb_regist(EVENT_ID_RETURN_HOST_CMD,handle_report_out);

}


