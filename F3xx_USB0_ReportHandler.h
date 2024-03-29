//-----------------------------------------------------------------------------
// F3xx_USB0_ReportHandler.h
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Includes functions called by USB_ISR.c to handle input and output reports.//
//
// How To Test:    See Readme.txt
//
//
// FID             3XX000014
// Target:         C8051F32x/C8051F340
// Tool chain:     Keil C51 7.50 / Keil EVAL C51
//                 Silicon Laboratories IDE version 2.6
// Command Line:   See Readme.txt
// Project Name:   F3xx_BlinkyExample
//
//
// Release 1.1
//    -Added feature reports for dimming controls
//    -Added PCA dimmer functionality
//    -16 NOV 2006
// Release 1.0
//    -Initial Revision (PD)
//    -07 DEC 2005
//

#ifndef  _UUSB_REPORTHANDLER_H_
#define  _USB_REPORTHANDLER_H_

#include "type.h"
#include "panelpoint.h"

#define REPORT_ID_IN_MTOUCH   	0x01
#define REPORT_ID_IN_DBGINFO   	0x02
#define REPORT_ID_OUT_CMD     	0x03
#define REPORT_ID_IN_FEATURE	0x04

// ----------------------------------------------------------------------------
// Report Sizes (in bytes)
// ----------------------------------------------------------------------------
#define REPORT_ID_OUT_CMD_LEN		0x0f	//no including the report_id
#define REPORT_ID_IN_DBGINFO_LEN  	0x3f
#define REPORT_ID_IN_MTOUCH_LEN  	0x0d


#define  OUT_PACKET_LEN		16
#define  IN_PACKET_LEN		64

extern u8	idata OUT_PACKET[OUT_PACKET_LEN];	
extern u8	idata IN_PACKET[IN_PACKET_LEN];	
extern u8	idata cur_cam_idx ;

#define	WORK_STYLE_IDLE			0x00
#define WORK_STYLE_PREVIEW		0x01
#define WORK_STYLE_CAL			0x02
#define WORK_STYLE_NOMAL		0x03




typedef struct {
   unsigned char ReportID;
   void (*hdlr)();
} VectorTableEntry;

typedef struct{
   unsigned char	Length;
   unsigned char*	Ptr;
} BufferStructure;

 typedef enum
 {
  	CAM_LEFT 	=0,
	CAM_RIGHT	=1,
	CAM_COUNT	
 }CAM_NUM_ENUM;

 typedef struct 
 {
 	unsigned char cam_num;
 	unsigned char send_cur_idx;
	unsigned char send_tot_cnt;
	unsigned char remain;
 }cam_send_img_stat_st;

 typedef enum
{
	DATA_LEFT_IMAGE = 0,
	DATA_RIGHT_IMAGE = 1,
	DATA_CMD_WRITE_REG =2,
	DATA_CMD_READ_REG=3,
	DATA_CMD_I2C_WRITE_REG=4,
	DATA_CMD_I2C_READ_REG=5,
	DATA_CMD_SWITCH_WORK_STYLE=10,
	DATA_CMD_READ_IMAGE = 11,
	DATA_TYPE_COUNT
}COM_DATA_TYPE;

extern void ReportHandler_IN_ISR(unsigned char);
extern void ReportHandler_IN_Foreground(unsigned char);
//extern void ReportHandler_OUT(unsigned char);
extern void Setup_OUT_BUFFER(void);

extern BufferStructure IN_BUFFER, OUT_BUFFER;

extern void report_handler_init(void);
void send_debug_info_to_host(u8 rid);

char fill_hid_packet(PanelPoint *MyPoint,u8 PointNum);
//void send_debug_info_to_host_1(void);
//void recv_cmd_from_host(u8 rid);

#endif