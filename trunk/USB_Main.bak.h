//-----------------------------------------------------------------------------
// USB_Main.h
//-----------------------------------------------------------------------------

#ifndef USB_MAIN_H
#define USB_MAIN_H

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#if defined C8051F320_H
	// USB clock selections (SFR CLKSEL)
	#define USB_4X_CLOCK			0x00		// Select 4x clock multiplier,
												// for USB Full Speed
	#define USB_INT_OSC_DIV_2		0x10		// See Oscillators in Datasheet
	#define USB_EXT_OSC				0x20
	#define USB_EXT_OSC_DIV_2		0x30
	#define USB_EXT_OSC_DIV_3		0x40
	#define USB_EXT_OSC_DIV_4		0x50

	// System clock selections (SFR CLKSEL)
	#define SYS_INT_OSC				0x00		// Select to use internal osc.
	#define SYS_EXT_OSC				0x01		// Select to use external osc.
	#define SYS_4X_DIV_2			0x02
#endif // C8051F320_H


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

extern BYTE	idata Out_Packet[];					// Last packet received from host
extern BYTE idata In_Packet[];					// Next packet to sent to host

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

void fill_packet( void );

#endif		/* USB_MAIN_H */

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
