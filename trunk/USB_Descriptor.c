//-----------------------------------------------------------------------------
// USB_Descriptor.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "USB_INT_to_HID_Type.h"
#include "USB_Configuration.h"
#include "USB_Descriptor.h"

//-----------------------------------------------------------------------------
// Macro for two byte field
//-----------------------------------------------------------------------------

#if defined BIG_ENDIAN
#define LE(x)	((((x)&0x00FF)<<8)|(((x)&0xFF00)>>8))	// convert to little endian
#else
#define LE(x)	(x)										// no conversion
#endif

//-----------------------------------------------------------------------------
// Descriptor Declarations
//-----------------------------------------------------------------------------

//
// Device descriptor
//
Tdevice_descriptor code DeviceDesc =
{
	sizeof(Tdevice_descriptor),				// bLength
	DSC_TYPE_DEVICE,						// bDescriptorType
	LE( VER_USB ),							// bcdUSB
	0x00,									// bDeviceClass
	0x00,									// bDeviceSubClass
	0x00,									// bDeviceProtocol
	EP0_PACKET_SIZE,						// bMaxPacketSize0
	LE( VID ),								// idVendor
	LE( PID ),								// idProduct
	LE( DEV_REV ),							// bcdDevice
	0x01,									// iManufacturer
	0x02,									// iProduct
	0x03,									// iSerialNumber
	0x01									// bNumConfigurations
}; //end of DeviceDesc

//
// HID report descriptor
//		placed here because the size of this array is referred in config:HID class desc
//
/*
BYTE code HID_report_desc[] =
{
	0x06, 0x00, 0xff,						// USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,								// USAGE (Vendor Usage 1)
	0xa1, 0x01,								// COLLECTION (Application)
	0x15, 0x00,								//	LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,						//	LOGICAL_MAXIMUM (255)
	0x75, 0x08,								//	REPORT_SIZE  (8)	- bits
	0x95, 0x40,								//	REPORT_COUNT (64)	- Bytes
	0x09, 0x01,								//	USAGE (Vendor Usage 1)
	0x81, 0x02,								//	INPUT (Data,Var,Abs)
	0x09, 0x01,								//	USAGE (Vendor Usage 1)
	0x91, 0x02,								//	OUTPUT (Data,Var,Abs)
	0xc0									// END_COLLECTION
};
*/
BYTE code HID_report_desc[] =
{
    0x05, 0x0d,                         // USAGE_PAGE (Digitizers)
    0x09, 0x04,                         // USAGE (Touch Screen)
    0xa1, 0x01,                         // COLLECTION (Application)

    0x85, REPORTID_MTOUCH,                 //   REPORT_ID (Touch)
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
    0x26, 0xff, 0x7f,                   //       LOGICAL_MAXIMUM (32767)
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
    0x26, 0xff, 0x7f,                   //       LOGICAL_MAXIMUM (32767)
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

	0x85, REPORTID_FEATURE,				//    Feature report ID
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
	
    0x85, REPORTID_DEBUGINFO,			// 	   REPORT_ID (Debug info)
//    0x06, 0x00, 0xff,                   //    USAGE_PAGE (Vendor Defined Page)
//    0x96, 0x3f, 0x00,					//	  IN_BLINK_SELECTORSize,   //   REPORT_COUNT ()
    0x96, (REPORT_DEBUG_INFO_LEN&0x00ff), (REPORT_DEBUG_INFO_LEN&0xff00)>>8,					//	  IN_BLINK_SELECTORSize,   //   REPORT_COUNT ()
    0x75, 0x08,                    		//   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              		//   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    		//   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    		//   USAGE (Vendor Usage 1)
    0x81, 0x02,                    		//   INPUT (Data,Var,Abs)

    0x85, REPORTID_CONTROL,        		// Report ID
//    0x06, 0x00, 0xff,                   //    USAGE_PAGE (Vendor Defined Page)
    0x95, 0x01,      					//   REPORT_COUNT ()
    0x75, 0x08,                    		//   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              		//   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    		//   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    		//   USAGE (Vendor Usage 1)
    0x91, 0x02,                    		//   OUTPUT (Data,Var,Abs)

	0xc0                                // END_COLLECTION
#endif
};

BYTE code HID_report_desc_size = sizeof( HID_report_desc );		// export report desc size
																// Original idea by Patryk
//
// Configuration descriptor
//
Tconfiguration_desc_set code ConfigDescSet =
{
  {										// Configuration descriptor
	sizeof(Tconfiguration_descriptor),		// bLength
	DSC_TYPE_CONFIG,						// bDescriptorType
	LE( sizeof(Tconfiguration_desc_set) ),	// bTotalLength
	DSC_NUM_INTERFACE,						// bNumInterfaces
	0x01,									// bConfigurationValue
	0x00,									// iConfiguration
	0x80,									// bmAttributes
	0x32,									// bMaxPower (100mA)
  },
  {										// Interface descriptor
	sizeof(Tinterface_descriptor),			// bLength
	DSC_TYPE_INTERFACE, 					// bDescriptorType
	DSC_INTERFACE_HID,						// bInterfaceNumber
	0x00,									// bAlternateSetting
	0x02,									// bNumEndpoints
	0x03,									// bInterfaceClass (HID)
	0x00,									// bInterfaceSubClass
	0x00,									// bInterfaceProcotol
	0x00,									// iInterface
  },
  {										// HID class descriptor
	sizeof(THID_class_descriptor),			// bLength
	DSC_SUBTYPE_CS_HID_CLASS,				// bDescriptorType
	LE( 0x0111 ),							// bcdHID (ver1.11)
	0x00,									// bCountryCode
	0x01,									// bNumDescriptors
	DSC_SUBTYPE_CS_HID_REPORT,				// bDescriptorType
	LE( sizeof( HID_report_desc ) )			// wDescriptorLength
  },
  {										// Endpoint1 IN descriptor
	sizeof(Tendpoint_descriptor),			// bLength
	DSC_TYPE_ENDPOINT,						// bDescriptorType
	IN_EP1,									// bEndpointAddress
	DSC_EP_INTERRUPT,						// bmAttributes
	LE( EP1_PACKET_SIZE ), 					// MaxPacketSize
	2,										// bInterval
  },
  {										// Endpoint1 OUT descriptor
	sizeof(Tendpoint_descriptor),			// bLength
	DSC_TYPE_ENDPOINT,						// bDescriptorType
	OUT_EP1,								// bEndpointAddress
	DSC_EP_INTERRUPT,						// bmAttributes
	LE( EP1_PACKET_SIZE ), 					// MaxPacketSize
	2										// bInterval
  }
}; //end of Configuration

//
// String descriptors
//

#define STR0LEN 4

static BYTE code String0Desc[STR0LEN] =
{
	STR0LEN, DSC_TYPE_STRING, 0x09, 0x04
}; //end of String0Desc

#define STR1LEN sizeof("SILICON LABORATORIES INC.")*2

static BYTE code String1Desc[STR1LEN] =
{
	STR1LEN, DSC_TYPE_STRING,
	'S', 0,
	'I', 0,
	'L', 0,
	'I', 0,
	'C', 0,
	'O', 0,
	'N', 0,
	' ', 0,
	'L', 0,
	'A', 0,
	'B', 0,
	'O', 0,
	'R', 0,
	'A', 0,
	'T', 0,
	'O', 0,
	'R', 0,
	'I', 0,
	'E', 0,
	'S', 0,
	' ', 0,
	'I', 0,
	'N', 0,
	'C', 0,
	'.', 0
}; //end of String1Desc

#define STR2LEN sizeof("C8051F320 Development Board")*2

static BYTE code String2Desc[STR2LEN] =
{
	STR2LEN, DSC_TYPE_STRING,
	'C', 0,
	'8', 0,
	'0', 0,
	'5', 0,
	'1', 0,
	'F', 0,
	'3', 0,
	'2', 0,
	'0', 0,
	' ', 0,
	'D', 0,
	'e', 0,
	'v', 0,
	'e', 0,
	'l', 0,
	'o', 0,
	'p', 0,
	'm', 0,
	'e', 0,
	'n', 0,
	't', 0,
	' ', 0,
	'B', 0,
	'o', 0,
	'a', 0,
	'r', 0,
	'd', 0
}; //end of String2Desc

// serial number string

#define STR3LEN sizeof("0001")*2

static BYTE code String3Desc[STR3LEN] =
{
	STR3LEN, DSC_TYPE_STRING,
	'0', 0,
	'0', 0,
	'0', 0,
	'1', 0
}; //end of String3Desc

BYTE code * code StringDescTable[] =
{
	String0Desc,
	String1Desc,
	String2Desc,
	String3Desc
};

BYTE code StringDescNum = sizeof( StringDescTable ) / sizeof( StringDescTable[0] );

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
