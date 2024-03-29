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
#include "PanelPoint.h"




#define  OUT_PACKET_LEN		16
#define  IN_PACKET_LEN		64

u8	idata OUT_PACKET[OUT_PACKET_LEN];	
u8  idata IN_PACKET[IN_PACKET_LEN];	
u8  idata cur_cam_idx ;

u8	idata BACKUP_PACKET[13];
u8	idata g_backup_ptnum ;


BufferStructure IN_BUFFER, OUT_BUFFER;
cam_send_img_stat_st cam_status[CAM_COUNT];


extern u8	code cmd_config_sensor[];
extern u8	code cmd_config_sensor_cnt;
extern u8	idata g_work_style;

//extern void Fifo_Write_Foreground (u8 addr, unsigned int uNumBytes, u8 * pData);
extern void Fifo_Write_Foreground (u8 addr, u8 uNumBytes,u8 idata* pData);
void cam_status_init(u16 width,u16 height);





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
		Fifo_Write_Foreground (FIFO_EP1, IN_BUFFER.Length,(u8 idata*)IN_BUFFER.Ptr);

		POLL_WRITE_BYTE (EINCSR1, rbInINPRDY);
		// Set In Packet ready bit,
	}                                   // indicating fresh data on FIFO 1

	EA = EAState;

}

void handle_report_out(void)
{
	u8 	idata reg_val,ret,cmd;
	u16 idata reg_addr;
	u16 idata w,h;
	u8  idata line;


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
			IE&=~0x2;			//(timer0 interupt disable)
			
			if(OUT_BUFFER.Ptr[3])
			{
				w = (OUT_BUFFER.Ptr[5]<<8)|(OUT_BUFFER.Ptr[4]);
				h = (OUT_BUFFER.Ptr[7]<<8)|(OUT_BUFFER.Ptr[6]);
				line = OUT_BUFFER.Ptr[8];

				cam_status_init(w,h);
	 			config_sensor();

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


PanelPoint g_panel_point;

void send_mtouch_info(void)
{
	g_panel_point.x +=5;
	//g_panel_point.y +=100;
	
	g_panel_point.x %= 1600;
	//g_panel_point.y %= 900;
	
	fill_hid_packet(&g_panel_point,1); 
	send_debug_info_to_host(REPORT_ID_IN_MTOUCH);	
	return;	
}

void cam_status_init(u16 width,u16 height)
{
 	u8 data i,read_cnt,remain;
	
   	read_cnt = 	(width*height)/BREAD_ONCE;
	remain   =  (width*height)%BREAD_ONCE;

	if(remain)
		read_cnt++;
	
	for(i = 0;i<CAM_COUNT;i++)
	{
	 	cam_status[i].cam_num = i;
		cam_status[i].send_cur_idx = 0;
		cam_status[i].send_tot_cnt = read_cnt;
		cam_status[i].remain = (remain? remain:BREAD_ONCE);
	}

	return;
}
void report_handler_init(void)
{
 	u8 data i;

	
	cam_status_init(IMAGE_WIDTH,IMAGE_HEIGHT);
	
	event_cb_regist(EVENT_ID_RETURN_HOST_CMD,handle_report_out);

	for(i = 0;i<13;i++)
	{		
		BACKUP_PACKET[i] = 0;
	}
	g_backup_ptnum = 0;
	g_panel_point.ID = 0;
 	g_panel_point.x  = 0;
	g_panel_point.y  = 500;

	event_cb_regist(EVENT_ID_RETURN_MTOUCH,send_mtouch_info);

}

extern u8 idata g_ticks_second;
char fill_hid_packet(PanelPoint *MyPoint,u8 PointNum)
{
	char data i = 0;
	char data flag = 0;

	IN_PACKET[0] = REPORT_ID_IN_MTOUCH;

	if (PointNum >= g_backup_ptnum)
	{
		IN_PACKET[13] = PointNum;
		g_backup_ptnum = PointNum;
	}
	else
	{
		IN_PACKET[13] = g_backup_ptnum;
		g_backup_ptnum = PointNum;
	}
	
	for (i=PointNum;i<2;i++)
	{
		IN_PACKET[1+6*i] = 0x0;
	}
	
	for (i=0;i<PointNum;i++)
	{
		//if(g_ticks_second % 5 ==0)
		//	IN_PACKET[1+6*i] = 0x00;
	//	else
			IN_PACKET[1+6*i] = 0x03;
		IN_PACKET[2+6*i] = MyPoint[i].ID+1;
		IN_PACKET[3+6*i] = (unsigned char)(MyPoint[i].x);
		IN_PACKET[4+6*i] = (unsigned char)(MyPoint[i].x>>8);
		IN_PACKET[5+6*i] = (unsigned char)(MyPoint[i].y);
		IN_PACKET[6+6*i] = (unsigned char)(MyPoint[i].y>>8);
	}

	for (i=0;i<13;i++)
	{
		if (BACKUP_PACKET[i]^IN_PACKET[i+1])
		{
			BACKUP_PACKET[i] = IN_PACKET[i+1];
			flag = 1;
			//break;
		}
	}

//	if (flag)
//	{
//		for (i=0;i<13;i++)
//		{
//			BACKUP_PACKET[i] = IN_PACKET[i+1];
//		}
//	}

	return flag;
}


