//-----------------------------------------------------------------------------
// USB_HID_Requests.h
//-----------------------------------------------------------------------------

#ifndef USB_HID_REQUEST_H
#define USB_HID_REQUEST_H

//-----------------------------------------------------------------------------
// Definition of Class specific request
//-----------------------------------------------------------------------------

// HID class specifc requests
#define HID_REQ_GET_REPORT			0x01
#define HID_REQ_GET_IDLE			0x02
#define HID_REQ_GET_PROTOCOL		0x03
#define HID_REQ_SET_REPORT			0x09
#define HID_REQ_SET_IDLE			0x0A
#define HID_REQ_SET_PROTOCOL		0x0B

// HID report type
#define HID_REPORT_TYPE_INPUT		0x01
#define HID_REPORT_TYPE_OUTPUT		0x02
#define HID_REPORT_TYPE_FEATURE		0x03

// Protocol type definition
#define HID_PROTOCOL_BOOT			0
#define HID_PROTOCOL_REPORT			1

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

extern void Class_Request( void );


#endif	// USB_HID_REQUEST_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
