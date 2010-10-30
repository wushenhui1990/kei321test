//-----------------------------------------------------------------------------
// USB_Descriptor.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "USB_HID_comp_Type.h"
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
BYTE code HID_report_desc0[] =
{
	0x06, 0x00, 0xff,						// USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,								// USAGE (Vendor Usage 1)
	0xa1, 0x01,								// COLLECTION (Application)
	0x15, 0x00,								//	LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,						//	LOGICAL_MAXIMUM (255)
	0x75, 0x08,								//	REPORT_SIZE  (8)	- bits
	0x95, EP1_REPORT_SIZE,					//	REPORT_COUNT (64)	- Bytes
	0x09, 0x01,								//	USAGE (Vendor Usage 1)
	0x81, 0x02,								//	INPUT (Data,Var,Abs)
	0x09, 0x01,								//	USAGE (Vendor Usage 1)
	0x91, 0x02,								//	OUTPUT (Data,Var,Abs)
	0xc0									// END_COLLECTION
};

BYTE code HID_report_desc1[] =
{
	0x06, 0x00, 0xff,						// USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,								// USAGE (Vendor Usage 1)
	0xa1, 0x01,								// COLLECTION (Application)
	0x15, 0x00,								//	LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,						//	LOGICAL_MAXIMUM (255)
	0x75, 0x08,								//	REPORT_SIZE  (8)	- bits
	0x95, EP2_REPORT_SIZE,					//	REPORT_COUNT (64)	- Bytes
	0x09, 0x01,								//	USAGE (Vendor Usage 1)
	0x81, 0x02,								//	INPUT (Data,Var,Abs)
	0x09, 0x01,								//	USAGE (Vendor Usage 1)
	0x91, 0x02,								//	OUTPUT (Data,Var,Abs)
	0xc0									// END_COLLECTION
};

BYTE code HID_report_desc2[] =
{
	0x06, 0x00, 0xff,						// USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,								// USAGE (Vendor Usage 1)
	0xa1, 0x01,								// COLLECTION (Application)
	0x15, 0x00,								//	LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,						//	LOGICAL_MAXIMUM (255)
	0x75, 0x08,								//	REPORT_SIZE  (8)	- bits
	0x95, EP3_REPORT_SIZE,					//	REPORT_COUNT (64)	- Bytes
	0x09, 0x01,								//	USAGE (Vendor Usage 1)
	0x81, 0x02,								//	INPUT (Data,Var,Abs)
	0x09, 0x01,								//	USAGE (Vendor Usage 1)
	0x91, 0x02,								//	OUTPUT (Data,Var,Abs)
	0xc0									// END_COLLECTION
};


BYTE code HID_report_desc0_size = sizeof( HID_report_desc0 );		// export report desc size
BYTE code HID_report_desc1_size = sizeof( HID_report_desc1 );		// export report desc size
BYTE code HID_report_desc2_size = sizeof( HID_report_desc2 );		// export report desc size
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
	DSC_CNFG_ATR_BASE | DSC_CNFG_ATR_BUS_POWERED,	// bmAttributes
	DSC_CNFG_MAXPOWER( 100 ),				// bMaxPower (mA)
											// <= 100mA: Low power
											// <= 500mA: High power
  },
								// -----  Interface 0  -----
  {										// Interface descriptor
	sizeof(Tinterface_descriptor),			// bLength
	DSC_TYPE_INTERFACE, 					// bDescriptorType
	DSC_INTERFACE_HID0,						// bInterfaceNumber
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
	LE( sizeof( HID_report_desc0 ) ),		// wDescriptorLength
  },
  {										// Endpoint1 IN descriptor
	sizeof(Tendpoint_descriptor),			// bLength
	DSC_TYPE_ENDPOINT,						// bDescriptorType
	EP1_IN,									// bEndpointAddress
	DSC_EP_INTERRUPT,						// bmAttributes
	LE( EP1_IN_PACKET_SIZE ), 				// MaxPacketSize
	1,										// bInterval
  },
  {										// Endpoint1 OUT descriptor
	sizeof(Tendpoint_descriptor),			// bLength
	DSC_TYPE_ENDPOINT,						// bDescriptorType
	EP1_OUT,								// bEndpointAddress
	DSC_EP_INTERRUPT,						// bmAttributes
	LE( EP1_OUT_PACKET_SIZE ), 				// MaxPacketSize
	1										// bInterval
  },
								// -----  Interface 1  -----
  {										// Interface descriptor
	sizeof(Tinterface_descriptor),			// bLength
	DSC_TYPE_INTERFACE, 					// bDescriptorType
	DSC_INTERFACE_HID1,						// bInterfaceNumber
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
	LE( sizeof( HID_report_desc1 ) ),		// wDescriptorLength
  },
  {										// Endpoint1 IN descriptor
	sizeof(Tendpoint_descriptor),			// bLength
	DSC_TYPE_ENDPOINT,						// bDescriptorType
	EP2_IN,									// bEndpointAddress
	DSC_EP_INTERRUPT,						// bmAttributes
	LE( EP1_IN_PACKET_SIZE ), 				// MaxPacketSize
	1,										// bInterval
  },
  {										// Endpoint1 OUT descriptor
	sizeof(Tendpoint_descriptor),			// bLength
	DSC_TYPE_ENDPOINT,						// bDescriptorType
	EP2_OUT,								// bEndpointAddress
	DSC_EP_INTERRUPT,						// bmAttributes
	LE( EP1_OUT_PACKET_SIZE ), 				// MaxPacketSize
	1										// bInterval
  },
								// -----  Interface 2  -----
  {										// Interface descriptor
	sizeof(Tinterface_descriptor),			// bLength
	DSC_TYPE_INTERFACE, 					// bDescriptorType
	DSC_INTERFACE_HID2,						// bInterfaceNumber
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
	LE( sizeof( HID_report_desc2 ) ),		// wDescriptorLength
  },
  {										// Endpoint1 IN descriptor
	sizeof(Tendpoint_descriptor),			// bLength
	DSC_TYPE_ENDPOINT,						// bDescriptorType
	EP3_IN,									// bEndpointAddress
	DSC_EP_INTERRUPT,						// bmAttributes
	LE( EP1_IN_PACKET_SIZE ), 				// MaxPacketSize
	1,										// bInterval
  },
  {										// Endpoint1 OUT descriptor
	sizeof(Tendpoint_descriptor),			// bLength
	DSC_TYPE_ENDPOINT,						// bDescriptorType
	EP3_OUT,								// bEndpointAddress
	DSC_EP_INTERRUPT,						// bmAttributes
	LE( EP1_OUT_PACKET_SIZE ), 				// MaxPacketSize
	1										// bInterval
  },
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
