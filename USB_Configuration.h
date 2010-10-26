//-----------------------------------------------------------------------------
// USB_Configuration.h
//-----------------------------------------------------------------------------

#ifndef USB_CONFIGURATION_H
#define USB_CONFIGURATION_H

//-----------------------------------------------------------------------------
// Definition of configuration
//-----------------------------------------------------------------------------

#define VER_USB					0x0110	// USB specification revision
#define VID						0x16C0	// Vendor ID
#define PID						0x06ED	// Product ID
#define DEV_REV					0x0000	// Device Release number

enum {									// interface number
	DSC_INTERFACE_HID,

	DSC_NUM_INTERFACE					// Number of Interfaces (for config desc)
};

										// Define Endpoint Packet Sizes
#define EP0_PACKET_SIZE			0x40	// Endpoint 0

										// Endpoint 1-3
										//	For full-speed,
										//		bulk:		8, 16, 32, 64
										//		interrupt:	0 - 64
										//		isoc:		0 - 1023 (512 for 'F32x/34x)
#define EP1_PACKET_SIZE			0x0040
//#define EP2_PACKET_SIZE			0x0040
//#define EP3_PACKET_SIZE			0x0040

//
// Endpoint usage - uncomment the def for the EPs to use
//

#define USE_OUT_EP1
//#define USE_OUT_EP2
//#define USE_OUT_EP3
#define USE_IN_EP1
//#define USE_IN_EP2
//#define USE_IN_EP3

//
// Endpoint double buffer - uncomment the def for the EPs to apply
//

//#define ENABLE_OUT_EP1_DOUBLE_BUF
//#define ENABLE_OUT_EP2_DOUBLE_BUF
//#define ENABLE_OUT_EP3_DOUBLE_BUF
//#define ENABLE_IN_EP1_DOUBLE_BUF
//#define ENABLE_IN_EP2_DOUBLE_BUF
//#define ENABLE_IN_EP3_DOUBLE_BUF

//
// Isochronous Endpoint - uncomment the def for the EPs to apply
//	When this option is not applied, the EP is set to interrupt or bulk
//

//#define ENABLE_OUT_EP1_ISO
//#define ENABLE_OUT_EP2_ISO
//#define ENABLE_OUT_EP3_ISO
//#define ENABLE_IN_EP1_ISO
//#define ENABLE_IN_EP2_ISO
//#define ENABLE_IN_EP3_ISO

//
// Endpoint interrupt - uncomment the def for the EPs to apply
//

#define ENABLE_OUT_EP1_INTERRUPT
//#define ENABLE_OUT_EP2_INTERRUPT
//#define ENABLE_OUT_EP3_INTERRUPT
#define ENABLE_IN_EP1_INTERRUPT
//#define ENABLE_IN_EP2_INTERRUPT
//#define ENABLE_IN_EP3_INTERRUPT

//
// Suspend/Resume handling - uncomment this def to apply
//

#define ENABLE_SUSPEND_RESUME

//
// SOF interrupt - uncomment this def to apply
//

//#define ENABLE_SOF_INTERRUPT

//
// Inline POLL_READ_BYTE / POLL_WRITE_BYTE - uncomment this def  to apply
//
// When this def is applied, POLL_READ_BYTE / POLL_WRITE_BYTE is defined as inline macros.
// Otherwise, these are defined as functions.
// Inline macro gives fast execution speed, but greater code size
//

#define ENABLE_INLINE_POLL_READ_WRITE


#endif	// USB_CONFIGURATION_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
