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


#define REPORT_ID_OUT_CMD     0x01
#define REPORT_ID_IN_IMAGE    0x04
#define REPORT_ID_IN_MTOUCH   0x05

// ----------------------------------------------------------------------------
// Report Sizes (in bytes)
// ----------------------------------------------------------------------------
#define REPORT_ID_OUT_CMD_LEN    0x08
#define REPORT_ID_IN_IMAGE_LEN   0x3f
#define REPORT_ID_IN_MTOUCH_LEN  0x02


#define  OUT_PACKET_LEN		16
#define  IN_PACKET_LEN		64

typedef struct {
   unsigned char ReportID;
   void (*hdlr)();
} VectorTableEntry;

typedef struct{
   unsigned char Length;
   unsigned char* Ptr;
} BufferStructure;

extern void ReportHandler_IN_ISR(unsigned char);
extern void ReportHandler_IN_Foreground(unsigned char);
extern void ReportHandler_OUT(unsigned char);
extern void Setup_OUT_BUFFER(void);

extern BufferStructure IN_BUFFER, OUT_BUFFER;

#endif