//-----------------------------------------------------------------------------
// USB_ISR.c
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "USB_INT_to_HID_Type.h"
#include "USB_Configuration.h"
#include "USB_Register.h"
#include "USB_Standard_Requests.h"
#include "USB_HID_Requests.h"

//#define ALLOCATE_VARS
#include "USB_ISR.h"
//#include "FIFO_RW.h"

//-----------------------------------------------------------------------------
// configuration conditions
//-----------------------------------------------------------------------------

#if (defined USE_OUT_EP1) && !(defined ENABLE_OUT_EP1_ISO)
	#define USE_OUT_EP1_STATUS
#endif
#if (defined USE_OUT_EP2) && !(defined ENABLE_OUT_EP2_ISO)
	#define USE_OUT_EP2_STATUS
#endif
#if (defined USE_OUT_EP3) && !(defined ENABLE_OUT_EP3_ISO)
	#define USE_OUT_EP3_STATUS
#endif
#if (defined USE_IN_EP1) && !(defined ENABLE_IN_EP1_ISO)
	#define USE_IN_EP1_STATUS
#endif
#if (defined USE_IN_EP2) && !(defined ENABLE_IN_EP2_ISO)
	#define USE_IN_EP2_STATUS
#endif
#if (defined USE_IN_EP3) && !(defined ENABLE_IN_EP3_ISO)
	#define USE_IN_EP3_STATUS
#endif


#ifdef ENABLE_OUT_EP1_INTERRUPT
	#define EP1_OUT_INTEN	rbOUT1E
#else
	#define EP1_OUT_INTEN	0
#endif
#ifdef ENABLE_OUT_EP2_INTERRUPT
	#define EP2_OUT_INTEN	rbOUT2E
#else
	#define EP2_OUT_INTEN	0
#endif
#ifdef ENABLE_OUT_EP3_INTERRUPT
	#define EP3_OUT_INTEN	rbOUT3E
#else
	#define EP3_OUT_INTEN	0
#endif

#ifdef ENABLE_IN_EP1_INTERRUPT
	#define EP1_IN_INTEN	rbIN1E
#else
	#define EP1_IN_INTEN	0
#endif
#ifdef ENABLE_IN_EP2_INTERRUPT
	#define EP2_IN_INTEN	rbIN2E
#else
	#define EP2_IN_INTEN	0
#endif
#ifdef ENABLE_IN_EP3_INTERRUPT
	#define EP3_IN_INTEN	rbIN3E
#else
	#define EP3_IN_INTEN	0
#endif

#ifdef ENABLE_SOF_INTERRUPT
	#define SOF_INTEN		rbSOFE
#else
	#define SOF_INTEN		0
#endif

#ifdef ENABLE_SUSPEND_RESUME
//	#define SUSPEND_RESUME_INTEN		(rbSUSINTE | rbRSUINTE)
	#define SUSPEND_RESUME_INTEN		(rbSUSINTE)
#else
	#define SUSPEND_RESUME_INTEN		0
#endif

#define EP_OUT_INTEN	(EP1_OUT_INTEN | EP2_OUT_INTEN | EP3_OUT_INTEN)
#define EP_IN_INTEN		(EP1_IN_INTEN  | EP2_IN_INTEN  | EP3_IN_INTEN)


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

BYTE          USB_State;			// Hold current usb device state
Tsetup_buffer  Setup;				// Buffer for current device request
bit           setup_handled;		// flag that indicates setup stage is handled or not
UINT          DataSize;				// Size of data to return
BYTE*         DataPtr;				// Pointer to data to return

// Holds the status for each endpoint
volatile BYTE Ep_Status0		= EP_IDLE;

#ifdef USE_OUT_EP1_STATUS
	volatile bit Ep_StatusOUT1	= EP_HALT;
#endif
#ifdef USE_OUT_EP2_STATUS
	volatile bit Ep_StatusOUT2	= EP_HALT;
#endif
#ifdef USE_OUT_EP3_STATUS
	volatile bit Ep_StatusOUT3	= EP_HALT;
#endif
#ifdef USE_IN_EP1_STATUS
	volatile bit Ep_StatusIN1	= EP_HALT;
#endif
#ifdef USE_IN_EP2_STATUS
	volatile bit Ep_StatusIN2	= EP_HALT;
#endif
#ifdef USE_IN_EP3_STATUS
	volatile bit Ep_StatusIN3	= EP_HALT;
#endif

// FIFO status of endpoints
volatile bit IN_FIFO_empty   = TRUE;
volatile bit OUT_FIFO_loaded = FALSE;

//-----------------------------------------------------------------------------
// Static Variables in this file
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Local function prototypes
//-----------------------------------------------------------------------------

static void Handle_Setup(void);					// Handle setup packet on Endpoint 0
static void Usb_Reset(void);					// Called after USB bus reset
static void Usb_Suspend(void);
static void Handle_SOF(void);

static void Handle_Out1(void);					// Handle out packet on Endpoint 1
static void Handle_Out2(void);					// Handle out packet on Endpoint 2
static void Handle_Out3(void);					// Handle out packet on Endpoint 3
static void Handle_In1(void);					// Handle in packet on Endpoint 1
static void Handle_In2(void);					// Handle in packet on Endpoint 2
static void Handle_In3(void);					// Handle in packet on Endpoint 3

//-----------------------------------------------------------------------------
// Usb0_Init
//-----------------------------------------------------------------------------
//
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//-----------------------------------------------------------------------------
void Usb0_Init(void)
{
	POLL_WRITE_BYTE(POWER,	0x08);		// Force Asynchronous USB Reset
										// USB Interrupt enable flags are reset by USB bus reset
	POLL_WRITE_BYTE(IN1IE,	rbEP0E);	// Enable EP 0 interrupt, disable EP1-3 IN interrupt
	POLL_WRITE_BYTE(OUT1IE,	0x00);		// Disable EP 1-3 OUT interrupts

	POLL_WRITE_BYTE(CMIE,	rbRSTINTE | SUSPEND_RESUME_INTEN);	// Enable Reset and Suspend interrupts

	USB0XCN = 0xE0;						// Enable transceiver; select full speed
	POLL_WRITE_BYTE(CLKREC, 0x80);		// Enable clock recovery, single-step mode
										// disabled
										// Enable USB0 by clearing the USB 
										// Inhibit bit
#ifdef ENABLE_SUSPEND_RESUME
	POLL_WRITE_BYTE(POWER,	0x01);		// and enable suspend detection
#endif

}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Usb_ISR
//-----------------------------------------------------------------------------
//
// Called after any USB type interrupt, this handler determines which type
// of interrupt occurred, and calls the specific routine to handle it.
//
//-----------------------------------------------------------------------------

void Usb_ISR(void) interrupt 8			// Top-level USB ISR
{
	BYTE bCommon, bIn;
#if (EP_OUT_INTEN != 0)
	BYTE bOut;
#endif

	POLL_READ_BYTE(CMINT, bCommon);		// Read all interrupt registers
	POLL_READ_BYTE(IN1INT, bIn);		// this read also clears the register

#if (EP_OUT_INTEN != 0)
	POLL_READ_BYTE(OUT1INT, bOut);
#endif
										// Handle Reset interrupt
	if (bCommon & rbRSTINT) {	Usb_Reset();	}
										// Handle EP1-3 interrupt
#ifdef ENABLE_OUT_EP1_INTERRUPT
//	if (bOut & rbOUT1) {		Handle_Out1();	}
	if (bOut & rbOUT1) {		OUT_FIFO_loaded = TRUE;	}
#endif
#ifdef ENABLE_OUT_EP2_INTERRUPT
	if (bOut & rbOUT2) {		Handle_Out2();	}
#endif
#ifdef ENABLE_OUT_EP3_INTERRUPT
	if (bOut & rbOUT3) {		Handle_Out3();	}
#endif
#ifdef ENABLE_IN_EP1_INTERRUPT
//	if (bIn & rbIN1) {			Handle_In1();	}
	if (bIn & rbIN1) {			IN_FIFO_empty = TRUE;	}
#endif
#ifdef ENABLE_IN_EP2_INTERRUPT
	if (bIn & rbIN2) {			Handle_In2();	}
#endif
#ifdef ENABLE_IN_EP3_INTERRUPT
	if (bIn & rbIN3) {			Handle_In3();	}
#endif
										// Handle EP0 interrupt
	if (bIn & rbEP0) {			Handle_Setup();	}

#ifdef ENABLE_SUSPEND_RESUME
										// Handle Suspend interrupt
	if (bCommon & rbSUSINT) {	Usb_Suspend();	}
										// Handle Resume interrupt
//	if (bCommon & rbRSUINT) {	Usb_Resume();	}
#endif

										// SOF interrupt
#ifdef ENABLE_SOF_INTERRUPT
	if (bCommon & rbSOF) {		Handle_SOF();	}
#endif

}

//-----------------------------------------------------------------------------
// Support Routines for ISR
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SDCC suport
//-----------------------------------------------------------------------------
#if defined SDCC
#pragma nooverlay
#endif // SDCC
//-----------------------------------------------------------------------------
// Fifo_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters	:
//					1) BYTE addr : target address
//					2) BYTE uNumBytes : number of bytes to unload
//					3) BYTE * pData : read data destination
//
// Read from the selected endpoint FIFO
//
//-----------------------------------------------------------------------------

static void Fifo_Read(BYTE addr, BYTE uNumBytes, BYTE * pData)
{
	BYTE idx;

	while(USB0ADR & 0x80);				// Wait for BUSY->'0'
	USB0ADR = addr | 0xC0;				// Set address
										// Set auto-read and initiate first read
										// Read <NumBytes> from the selected FIFO
	for ( idx = 0; idx < uNumBytes; idx++ ) {
		while(USB0ADR & 0x80);			// Wait for BUSY->'0' (read complete)
		pData[ idx ] = USB0DAT;
	}
	USB0ADR = 0;						// Clear auto-read
}

//-----------------------------------------------------------------------------
// Fifo_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters	:
//					1) BYTE addr : target address
//					2) BYTE uNumBytes : number of bytes to unload
//					3) BYTE * pData : location of source data
//
// Write to the selected endpoint FIFO
//
//-----------------------------------------------------------------------------

static void Fifo_Write(BYTE addr, BYTE uNumBytes, BYTE * pData)
{
	BYTE idx;

	while(USB0ADR & 0x80);				// Wait for BUSY->'0'
	USB0ADR = (addr & 0x3F);			// Set address (mask out bits7-6)

	// Write <NumBytes> to the selected FIFO
	for ( idx = 0; idx < uNumBytes; idx++ ) {
		while(USB0ADR & 0x80);			// Wait for BUSY->'0' (write complete)
		USB0DAT = pData[ idx ];
	}
}


//-----------------------------------------------------------------------------
// Usb_Reset
//-----------------------------------------------------------------------------
//
// - Set state to default
// - Clear Usb Inhibit bit
//
//-----------------------------------------------------------------------------

static void Usb_Reset(void)
{
	POLL_WRITE_BYTE(IN1IE,	rbEP0E | EP_IN_INTEN );		// Enable Ep0 and EP1-3 IN
#if (EP_OUT_INTEN != 0)
	POLL_WRITE_BYTE(OUT1IE, EP_OUT_INTEN );				// Enable EP1-3 OUT
#endif
	POLL_WRITE_BYTE(CMIE,	rbRSTINTE | SUSPEND_RESUME_INTEN | SOF_INTEN);		// Enable Reset, Suspend interrupts

	POLL_WRITE_BYTE(POWER, 0x01);		// Clear usb inhibit bit to enable USB
										// suspend detection

	USB_State = DEV_DEFAULT;			// Set device state to default

	Ep_Status0 = EP_IDLE;				// Set default Endpoint Status

#ifdef USE_OUT_EP1_STATUS
	Ep_StatusOUT1 = EP_HALT;
#endif
#ifdef USE_OUT_EP2_STATUS
	Ep_StatusOUT2 = EP_HALT;
#endif
#ifdef USE_OUT_EP3_STATUS
	Ep_StatusOUT3 = EP_HALT;
#endif
#ifdef USE_IN_EP1_STATUS
	Ep_StatusIN1  = EP_HALT;
#endif
#ifdef USE_IN_EP2_STATUS
	Ep_StatusIN2  = EP_HALT;
#endif
#ifdef USE_IN_EP3_STATUS
	Ep_StatusIN3  = EP_HALT;
#endif

}
//-----------------------------------------------------------------------------
// Usb_Suspend
//-----------------------------------------------------------------------------
//
// Enter suspend mode after suspend signalling is present on the bus
//		- called from USB ISR
//
//-----------------------------------------------------------------------------


#ifdef ENABLE_SUSPEND_RESUME

static void Usb_Suspend(void)
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

//-----------------------------------------------------------------------------
// Handle_Setup
//-----------------------------------------------------------------------------
//
// - Decode Incoming Setup requests
// - Load data packets on fifo while in transmit mode
//
//-----------------------------------------------------------------------------

static bit send_eq_requested = FALSE;	// flag that indicates that the data to send on TX
										// equals to the requested by Setup wLength
										// KEIL doesn't accept static local bit variable
static void Handle_Setup(void)
{
	BYTE ControlReg, TempReg;			// Temporary storage for EP control register

	POLL_WRITE_BYTE(INDEX, 0);			// Set Index to Endpoint Zero
	POLL_READ_BYTE(E0CSR, ControlReg);	// Read control register

	if (ControlReg & rbSTSTL)			// If last packet was a sent stall, reset
	{									// STSTL bit and return EP0 to idle state
		POLL_WRITE_BYTE(E0CSR, 0);
		Ep_Status0 = EP_IDLE;
		return;
	}

	if (ControlReg & rbSUEND)			// SUEND bit is asserted after status stage by SIE
	{									// or when SIE receives early SETUP
		POLL_WRITE_BYTE(E0CSR, rbSSUEND);	// Serviced Setup End bit and return to EP_IDLE
		Ep_Status0 = EP_IDLE;
	}

	if (Ep_Status0 == EP_IDLE)			// If Endpoint 0 is in idle mode
	{
		if (ControlReg & rbOPRDY)		// Make sure that EP 0 has an Out Packet ready from host
		{								// although if EP0 is idle, this should always be the case
//			FIFO_Read(FIFO_EP0, 8, (unsigned char *)&Setup);
//			FIFO_Read_generic(FIFO_EP0, 8, (BYTE *)&Setup);
//			FIFO_Read_idata(FIFO_EP0, 8, (BYTE idata *)&Setup);
//			FIFO_Read_pdata(FIFO_EP0, 8, (BYTE pdata *)&Setup);
//			FIFO_Read_xdata(FIFO_EP0, 8, (BYTE xdata *)&Setup);
			Fifo_Read(FIFO_EP0, 8, (BYTE *)&Setup);

										// Get Setup Packet off of Fifo
#if defined BIG_ENDIAN
										// As the USB custom, multi-byte number is LSB first - little endian
			Setup.wValue.i  = ((UINT)Setup.wValue.c[1]  << 8) | Setup.wValue.c[0];
			Setup.wIndex.i  = ((UINT)Setup.wIndex.c[1]  << 8) | Setup.wIndex.c[0];
			Setup.wLength.i = ((UINT)Setup.wLength.c[1] << 8) | Setup.wLength.c[0];
#endif // end of BIG_ENDIAN

			setup_handled = FALSE;
			switch ( Setup.bmRequestType & DRT_MASK )	// Device Request Type
			{
				case DRT_STD:							// Standard device request
					Standard_Device_Request();
					break;
				case DRT_CLASS:							// class specific request
					Class_Request();
					break;
/*
				case DRT_VENDOR:						// vendor request
					Vendor_Request();
					break;
*/
				default:
					break;
			}

			POLL_WRITE_BYTE(INDEX, 0);					// Assure the indexed registers are accessed correctly
			if ( setup_handled )
			{
				POLL_WRITE_BYTE(E0CSR, rbSOPRDY);		// Tell to SIE that the setup is handled
			} else {
														// Return STALL to the host
				POLL_WRITE_BYTE(E0CSR, rbSDSTL);		// Set the send stall bit
				Ep_Status0 = EP_STALL;					// Put the endpoint in stall status
			}

			send_eq_requested = (DataSize == Setup.wLength.i);	// get this flag before DataSize 
																// is reduced in TX cycle
		}
	} // end of EP_IDLE

	if ( Ep_Status0 == EP_TX )			// See if the endpoint has data to transmit to host
	{
		if ( !(ControlReg & rbINPRDY) ) // Make sure you don't overwrite last packet
		{			
			POLL_READ_BYTE( E0CSR, ControlReg );
										// Read control register

			if ( (!(ControlReg & rbSUEND)) && (!(ControlReg & rbOPRDY)) )
										// Check to see if Setup End or Out Packet received, if so
										// do not put any new data on FIFO
			{
				TempReg = rbINPRDY;		// Add In Packet ready flag to E0CSR bitmask
										// Break Data into multiple packets if larger than Max Packet
				if (DataSize >= EP0_PACKET_SIZE)
				{									// The data size to send in this cycle is
													// just EP0_PACKET_SIZE
					Fifo_Write( FIFO_EP0, EP0_PACKET_SIZE, (BYTE *)DataPtr );
//					FIFO_Write_generic( FIFO_EP0, EP0_PACKET_SIZE, (BYTE *)DataPtr );
					DataPtr += EP0_PACKET_SIZE;			// Advance data pointer
					DataSize -= EP0_PACKET_SIZE;		// Decrement data size
					if ( send_eq_requested && (DataSize == 0) )	// In this case, no need to send ZLP,
					{											// finish TX immediately
						TempReg |= rbDATAEND;
						Ep_Status0 = EP_IDLE;	
					}
				} else {							// The data size to send in this cycle is
													// smaller than EP0_PACKET_SIZE or zero (ZLP)
					Fifo_Write( FIFO_EP0, (BYTE)DataSize, (BYTE *)DataPtr );
//					FIFO_Write_generic( FIFO_EP0, (BYTE)DataSize, (BYTE *)DataPtr );
					TempReg |= rbDATAEND;
					Ep_Status0 = EP_IDLE;
				}
				POLL_WRITE_BYTE(E0CSR, TempReg);		// Write mask to E0CSR
			}
		}
	} // end of EP_TX
/*
	if (Ep_Status0 == EP_RX)			// See if endpoint should receive
	{
		BYTE dataCount;

		POLL_READ_BYTE( E0CSR, ControlReg );	// Read control register
		if ( ControlReg & rbOPRDY )				// Verify packet was received
		{
			ControlReg = rbSOPRDY;

			POLL_READ_BYTE( E0CNT, dataCount );	// get data bytes on the FIFO
//			Fifo_Read( FIFO_EP0, dataCount, (BYTE*)DataPtr );
			FIFO_Read_generic( FIFO_EP0, dataCount, (BYTE*)DataPtr );
											// Empty the FIFO
												// FIFO must be read out just the size it has,
												// otherwize the FIFO pointer on the SIE goes out of synch.
			DataPtr += dataCount;			// Advance the pointer
			if ( DataSize > dataCount )		// Update the scheduled number to be received
				DataSize -= dataCount;
			else {							// Meet the end of the data stage
				DataSize = 0;
				ControlReg |= rbDATAEND;	// Signal end of data stage
				Ep_Status0 = EP_IDLE;		// Set Endpoint to IDLE

				if (   (Setup.bRequest == SET_LINE_CODING)		// completion routine for Set_Line_Coding
					&& (Setup.bmRequestType == OUT_CL_INTERFACE) )
					CS_Set_Line_Coding_Complete();
			}

			POLL_WRITE_BYTE ( E0CSR, ControlReg );
		}
	} // end of EP_RX
*/
}
										
//-----------------------------------------------------------------------------
// POLL_READ_BYTE, POLL_WRITE_BYTE
//-----------------------------------------------------------------------------
// When the macros are not defined, provide them as functions
//-----------------------------------------------------------------------------

#if defined( POLL_READ_BYTE_DEF )

BYTE POLL_READ_BYTE_FUNC( BYTE addr )
{
	while( USB0ADR & 0x80 );
	USB0ADR = (0x80 | addr);
	while( USB0ADR & 0x80 );
	return USB0DAT;
}

#endif

#if !defined( POLL_WRITE_BYTE )

void POLL_WRITE_BYTE( BYTE addr, BYTE dt )
{
	while(USB0ADR & 0x80);
	USB0ADR = addr;
	USB0DAT = dt;
}

#endif

//-----------------------------------------------------------------------------
// Handle_Out1
//-----------------------------------------------------------------------------
//	Handle out packet on Endpoint 1
//-----------------------------------------------------------------------------

#ifdef ENABLE_OUT_EP1_INTERRUPT
/*
static void Handle_Out1(void)
{
}
*/
#endif

//-----------------------------------------------------------------------------
// Handle_Out2
//-----------------------------------------------------------------------------
//	Handle out packet on Endpoint 2
//-----------------------------------------------------------------------------

#ifdef ENABLE_OUT_EP2_INTERRUPT

static void Handle_Out2(void)
{
}

#endif

//-----------------------------------------------------------------------------
// Handle_Out3
//-----------------------------------------------------------------------------
//	Handle out packet on Endpoint 3
//-----------------------------------------------------------------------------

#ifdef ENABLE_OUT_EP3_INTERRUPT

static void Handle_Out3(void)
{
}

#endif

//-----------------------------------------------------------------------------
// Handle_In1
//-----------------------------------------------------------------------------
//	Handle in packet on Endpoint 1
//-----------------------------------------------------------------------------

#ifdef ENABLE_IN_EP1_INTERRUPT
/*
static void Handle_In1(void)
{
}
*/
#endif

//-----------------------------------------------------------------------------
// Handle_In2
//-----------------------------------------------------------------------------
//	Handle in packet on Endpoint 2
//-----------------------------------------------------------------------------

#ifdef ENABLE_IN_EP2_INTERRUPT

static void Handle_In2(void)
{
}

#endif

//-----------------------------------------------------------------------------
// Handle_In3
//-----------------------------------------------------------------------------
//	Handle in packet on Endpoint 3
//-----------------------------------------------------------------------------

#ifdef ENABLE_IN_EP3_INTERRUPT

static void Handle_In3(void)
{
}

#endif

//-----------------------------------------------------------------------------
// Handle_SOF
//-----------------------------------------------------------------------------
//	Handle in SOF interrupt
//-----------------------------------------------------------------------------

#ifdef ENABLE_SOF_INTERRUPT

static void Handle_SOF(void)
{
}

#endif

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
