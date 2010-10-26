//-----------------------------------------------------------------------------
// USB_Main.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "USB_INT_to_HID_Type.h"
#include "USB_Configuration.h"
#include "USB_Register.h"
#include "USB_Standard_Requests.h"
#include "USB_ISR.h"
#include "FIFO_RW.h"
//#include "USB_Main.h"

#if defined KEIL
#include <intrins.h>		// for _testbit_(), _nop_()
#endif // KEIL

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#if defined C8051F320_H
	#define SYSCLK	24000000L				// SYSCLK frequency in Hz
#endif







 
//-----------------------------------------------------------------------------
// fill_packet
//-----------------------------------------------------------------------------
//
// fill the IN packet
//
//	called from both of main and USB ISR (Get_Report)
//
//-----------------------------------------------------------------------------




#if defined KEIL
#include <intrins.h>
#endif



//-----------------------------------------------------------------------------
// Usb_Suspend
//-----------------------------------------------------------------------------
//
// Enter suspend mode after suspend signalling is present on the bus
//		- called from USB ISR
//
//-----------------------------------------------------------------------------


#ifdef ENABLE_SUSPEND_RESUME

void Usb_Suspend(void)
{			
	// Put the device in a low power configuration
#if (defined C8051F320_H) || (defined C8051F340_H)
	P0MDIN	= 0x00;						// Port 0 configured as analog input
	P1MDIN	= 0x00;						// Port 1 configured as analog input
	P2MDIN	= 0x00;						// Port 2 configured as analog input
	P3MDIN	= 0x00;						// Port 3 configured as analog input
#endif

//	ADC0CN &= ~0x80;					// Disable ADC0
//	REF0CN	= 0x00;						// Disable voltage reference

	OSCICN |= 0x20;						// Put oscillator to suspend

	// When the device receives a non-idle USB event, it will resume execution
	// on the instruction that follows OSCICN |= 0x20.		

	// Re-enable everything that was disabled when going into Suspend
#if defined C8051F320_H
	P0MDIN	= 0xFF;						// Port 0 configured as diital input
	P1MDIN	= 0x7F;						// Port 1 pin 7 set as analog input
	P2MDIN	= 0xFF;						// Port 2 configured as diital input
	P3MDIN	= 0xFF;						// Port 3 configured as diital input
#endif // C8051F320_H

#if defined C8051F340_H
	P0MDIN	= 0xFF;						// Port 0 configured as diital input
	P1MDIN	= 0xFF;						// Port 1 configured as diital input
	P2MDIN  = ~(0x20);					// Port 2 pin 5 set as analog input
	P3MDIN	= 0xFF;						// Port 3 configured as diital input
#endif // C8051F340_H


//	REF0CN	= 0x0E;						// Enable voltage reference VREF
//	ADC0CN |= 0x80;						// Re-enable ADC

}

#endif	// ENABLE_SUSPEND_RESUME






