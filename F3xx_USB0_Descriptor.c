//-----------------------------------------------------------------------------
// F3xx_USB0_Descriptor.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// Source file for USB firmware. Includes descriptor data.
//
//
// How To Test:    See Readme.txt
//
//
// FID:            3XX000004
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
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "F3xx_USB0_Register.h"
#include "F3xx_USB0_InterruptServiceRoutine.h"
#include "F3xx_USB0_Descriptor.h"
#include "F3xx_USB0_ReportHandler.h"
//-----------------------------------------------------------------------------
// Descriptor Declarations
//-----------------------------------------------------------------------------

#define LE(x)	((((x)&0x00FF)<<8)|(((x)&0xFF00)>>8))	// convert to little endian

code const device_descriptor DEVICEDESC =
{
   18,                                 // bLength
   0x01,                               // bDescriptorType
   0x1001,                             // bcdUSB
   0x00,                               // bDeviceClass
   0x00,                               // bDeviceSubClass
   0x00,                               // bDeviceProtocol
   EP0_PACKET_SIZE,                    // bMaxPacketSize0
   0xC410,                             // idVendor
   0xCD82,                             // idProduct
   0x0000,                             // bcdDevice
   0x01,                               // iManufacturer
   0x02,                               // iProduct
   0x00,                               // iSerialNumber
   0x01                                // bNumConfigurations
}; //end of DEVICEDESC
/*
unsigned char code HID_report_desc[] =
//code const hid_report_descriptor HIDREPORTDESC =
{
    0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)

    0x85, REPORT_ID_OUT_CMD,       // Report ID
    0x95, REPORT_ID_OUT_CMD_LEN,   //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x85, REPORT_ID_IN_DBGINFO,     // Report ID
    0x95, REPORT_ID_IN_DBGINFO_LEN,   //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)


    0x85, REPORT_ID_IN_MTOUCH,        // Report ID
    0x95, REPORT_ID_IN_MTOUCH_LEN,      //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xC0                           //   end Application Collection
};
*/

unsigned char code HID_report_desc[] =
{
    0x05, 0x0d,                         // USAGE_PAGE (Digitizers)
    0x09, 0x04,                         // USAGE (Touch Screen)
    0xa1, 0x01,                         // COLLECTION (Application)

    0x85, REPORT_ID_IN_MTOUCH,                 //   REPORT_ID (Touch)
	0x09, 0x22, 						//	 USAGE (Finger)
    0xa1, 0x02,                         //     COLLECTION (Logical)
    0x09, 0x42,                         //       USAGE (Tip Switch)
    0x15, 0x00,                         //       LOGICAL_MINIMUM (0)
    0x25, 0x01,                         //       LOGICAL_MAXIMUM (1)
    0x75, 0x01,                         //       REPORT_SIZE (1)
    0x95, 0x01,                         //       REPORT_COUNT (1)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x09, 0x32,                         //       USAGE (In Range)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
//    0x09, 0x47,                         //       USAGE (Touch Valid)
//    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
//    0x95, 0x05,                         //       REPORT_COUNT (5)
	0x95, 0x06, 						//		 REPORT_COUNT (6)
    0x81, 0x03,                         //       INPUT (Cnst,Ary,Abs)
    0x75, 0x08,                         //       REPORT_SIZE (8)
    0x09, 0x51,                         //       USAGE (Contact Identifier)
    0x95, 0x01,                         //       REPORT_COUNT (1)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x05, 0x01,                         //       USAGE_PAGE (Generic Desk..
    0x26, 0x40, 0x06,                   //       LOGICAL_MAXIMUM (32767)
    0x75, 0x10,                         //       REPORT_SIZE (16)
    0x55, 0x00,                         //       UNIT_EXPONENT (0)
    0x65, 0x00,                         //       UNIT (None)
    0x09, 0x30,                         //       USAGE (X)
    0x35, 0x00,                         //       PHYSICAL_MINIMUM (0)
    0x46, 0x00, 0x00,                   //       PHYSICAL_MAXIMUM (0)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x09, 0x31,                         //       USAGE (Y)
    0x46, 0x00, 0x00,                   //       PHYSICAL_MAXIMUM (0)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0xc0,                               //    END_COLLECTION
    0xa1, 0x02,                         //    COLLECTION (Logical)
    0x05, 0x0d,                         //     USAGE_PAGE (Digitizers)
    0x09, 0x42,                         //       USAGE (Tip Switch)
    0x15, 0x00,                         //       LOGICAL_MINIMUM (0)
    0x25, 0x01,                         //       LOGICAL_MAXIMUM (1)
    0x75, 0x01,                         //       REPORT_SIZE (1)
    0x95, 0x01,                         //       REPORT_COUNT (1)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x09, 0x32,                         //       USAGE (In Range)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
//    0x09, 0x47,                         //       USAGE (Touch Valid)
//    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
//    0x95, 0x05,                         //       REPORT_COUNT (5)
	0x95, 0x06, 						//		 REPORT_COUNT (6)
    0x81, 0x03,                         //       INPUT (Cnst,Ary,Abs)
    0x75, 0x08,                         //       REPORT_SIZE (8)
    0x09, 0x51,                         //       USAGE ( Cotact Identifier)
    0x95, 0x01,                         //       REPORT_COUNT (1)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x05, 0x01,                         //       USAGE_PAGE (Generic Desk..
    0x26, 0x40, 0x06,                   //       LOGICAL_MAXIMUM (32767)
    0x75, 0x10,                         //       REPORT_SIZE (16)
    0x55, 0x00,                         //       UNIT_EXPONENT (0)
    0x65, 0x00,                         //       UNIT (None)
    0x09, 0x30,                         //       USAGE (X)
    0x35, 0x00,                         //       PHYSICAL_MINIMUM (0)
    0x46, 0x00, 0x00,                   //       PHYSICAL_MAXIMUM (0)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0x09, 0x31,                         //       USAGE (Y)
    0x46, 0x00, 0x00,                   //       PHYSICAL_MAXIMUM (0)
    0x81, 0x02,                         //       INPUT (Data,Var,Abs)
    0xc0,                               //    END_COLLECTION
	0x05, 0x0d, 						//	  USAGE_PAGE (Digitizers)
	0x09, 0x54, 						//	  USAGE (Contact Count)
	0x95, 0x01, 						//	  REPORT_COUNT (1)
	0x75, 0x08, 						//	  REPORT_SIZE (8)
	0x15, 0x00, 						//	  LOGICAL_MINIMUM (0)
	0x25, 0x02, 						//	  LOGICAL_MAXIMUM (2)
	0x81, 0x02, 						//	  INPUT (Data,Var,Abs)
//	0x95, 0x76, 						//	  REPORT_COUNT (1)
//	0x81, 0x03,                         //       INPUT (Cnst,Ary,Abs)

	0x85, REPORT_ID_IN_FEATURE,				//    Feature report ID
//    0x05, 0x0d,                         //    USAGE_PAGE (Digitizers)
//    0x09, 0x54,                         //    USAGE (Contact Count)
//    0x95, 0x01,                         //    REPORT_COUNT (1)
//    0x75, 0x08,                         //    REPORT_SIZE (8)
//    0x15, 0x00,                         //    LOGICAL_MINIMUM (0)
//    0x25, 0x08,                         //    LOGICAL_MAXIMUM (8)
//    0x81, 0x02,                         //    INPUT (Data,Var,Abs)
    0x09, 0x55,                         //    USAGE(Contact Count Maximum)
    0xb1, 0x02,                         //    FEATURE (Data,Var,Abs)
   
    0xc0,                               // END_COLLECTION
# if 1
	0x06, 0x00, 0xff,					// USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,							// USAGE (Vendor Usage 1)
	0xa1, 0x01,							// COLLECTION (Application)
	
    0x85, REPORT_ID_IN_DBGINFO,			// 	   REPORT_ID (Debug info)
	0x95, REPORT_ID_IN_DBGINFO_LEN,
    0x75, 0x08,                    		//   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              		//   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    		//   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    		//   USAGE (Vendor Usage 1)
    0x81, 0x02,                    		//   INPUT (Data,Var,Abs)

    0x85, REPORT_ID_OUT_CMD,        		// Report ID
    0x95, REPORT_ID_OUT_CMD_LEN,      	//   REPORT_COUNT ()
    0x75, 0x08,                    		//   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              		//   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    		//   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    		//   USAGE (Vendor Usage 1)
    0x91, 0x02,                    		//   OUTPUT (Data,Var,Abs)

	0xc0                                // END_COLLECTION
#endif
};
unsigned char code HID_report_desc_size = sizeof( HID_report_desc );		// export report desc size

// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
// "When a Get_Descriptor(Configuration) request is issued,
// it returns the Configuration descriptor, all Interface descriptors,
// all Endpoint descriptors, and the HID descriptor for each interface."
code const hid_configuration_descriptor HIDCONFIGDESC =
{

{ // configuration_descriptor hid_configuration_descriptor
   0x09,                               // Length
   0x02,                               // Type
   0x2900,                             // Totallength (= 9+9+9+7+7)
   0x01,                               // NumInterfaces
   0x01,                               // bConfigurationValue
   0x00,                               // iConfiguration
   0x80,                               // bmAttributes
   0x20                                // MaxPower (in 2mA units)
},

{ // interface_descriptor hid_interface_descriptor
   0x09,                               // bLength
   0x04,                               // bDescriptorType
   0x00,                               // bInterfaceNumber
   0x00,                               // bAlternateSetting
   0x02,                               // bNumEndpoints
   0x03,                               // bInterfaceClass (3 = HID)
   0x00,                               // bInterfaceSubClass
   0x00,                               // bInterfaceProcotol
   0x00                                // iInterface
},

{ // class_descriptor hid_descriptor
   0x09,	                           // bLength
   0x21,	                           // bDescriptorType
   0x0101,	                           // bcdHID
   0x00,	                           // bCountryCode
   0x02,	                           // bNumDescriptors
   0x22,                               // bDescriptorType
   LE(sizeof( HID_report_desc ))       // wItemLength (tot. len. of report
                                       // descriptor)
},

// IN endpoint (mandatory for HID)
{ // endpoint_descriptor hid_endpoint_in_descriptor
   0x07,                               // bLength
   0x05,                               // bDescriptorType
   0x81,                               // bEndpointAddress
   0x03,                               // bmAttributes
   LE(EP1_IN_PACKET_SIZE),                 // MaxPacketSize (LITTLE ENDIAN)
   1                                  // bInterval
},

// OUT endpoint (optional for HID)
{ // endpoint_descriptor hid_endpoint_out_descriptor
   0x07,                               // bLength
   0x05,                               // bDescriptorType
   0x01,                               // bEndpointAddress
   0x03,                               // bmAttributes
   LE(EP1_OUT_PACKET_SIZE),                 // MaxPacketSize (LITTLE ENDIAN)
   1                                  // bInterval
}

};


#define STR0LEN 4

code const unsigned char String0Desc [STR0LEN] =
{
   STR0LEN, 0x03, 0x09, 0x04
}; // End of String0Desc

#define STR1LEN sizeof ("SLAB") * 2

code const unsigned char String1Desc [STR1LEN] =
{
   STR1LEN, 0x03,
   'S', 0,
   'L', 0,
   'A', 0,
   'B', 0
}; // End of String1Desc

#define STR2LEN sizeof ("F320") * 2

code const unsigned char String2Desc [STR2LEN] =
{
   STR2LEN, 0x03,
   'F', 0,
   '3', 0,
   '2', 0,
   '0', 0
}; // End of String2Desc

unsigned char* const STRINGDESCTABLE [] =
{
   String0Desc,
   String1Desc,
   String2Desc
};
