//-----------------------------------------------------------------------------
// USB_ISR.h
//-----------------------------------------------------------------------------

#ifndef USB_ISR_H
#define USB_ISR_H

//-----------------------------------------------------------------------------
// Setup Packet Type Definition
//-----------------------------------------------------------------------------

typedef struct
{
	BYTE bmRequestType;					// Request recipient, type, direction
	BYTE bRequest;						// Specific standard request number
	WORD wValue;						// varies according to request
	WORD wIndex;						// varies according to request
	WORD wLength;						// Number of bytes to transfer
} Tsetup_buffer;						// End of Setup Packet Type

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

extern BYTE          USB_State;			// Hold current usb device state
extern Tsetup_buffer Setup;				// Buffer for current device request
extern bit           setup_handled;		// flag that indicates setup stage is handled or not
extern UINT          DataSize;			// Size of data to return
extern BYTE*         DataPtr;			// Pointer to data to return

// Holds the status for each endpoint
extern volatile BYTE Ep_Status0;
extern volatile bit Ep_StatusOUT1;
extern volatile bit Ep_StatusIN1;
extern volatile bit Ep_StatusOUT2;
extern volatile bit Ep_StatusIN2;
extern volatile bit Ep_StatusOUT3;
extern volatile bit Ep_StatusIN3;

// FIFO status of endpoints
extern volatile bit IN1_FIFO_empty;
extern volatile bit IN2_FIFO_empty;
extern volatile bit IN3_FIFO_empty;
extern volatile bit OUT1_FIFO_loaded;
extern volatile bit OUT2_FIFO_loaded;
extern volatile bit OUT3_FIFO_loaded;

/*
//
// Unfortunately, SDCC doesn't accept token-pasting operator (##) in its pre-processor
// Though this EXTERN method works well on both compilers,
// but initialization using ## is limited to KEIL.
// Then, in this implementation, give up this method and return to usual extern definition.

// variable allocation
#ifdef ALLOCATE_VARS
#define EXTERN
#define EXT_TERM
#else
#define EXTERN extern
#define EXT_TERM	;/ ## /
#endif

// Holds the status for each endpoint
EXTERN volatile BYTE Ep_Status0    EXT_TERM	= EP_IDLE;
//EXTERN volatile bit Ep_StatusOUT1  EXT_TERM	= EP_HALT;
EXTERN volatile bit Ep_StatusIN1   EXT_TERM	= EP_HALT;
EXTERN volatile bit Ep_StatusOUT2  EXT_TERM	= EP_HALT;
//EXTERN volatile bit Ep_StatusIN2   EXT_TERM	= EP_HALT;
//EXTERN volatile bit Ep_StatusOUT3  EXT_TERM	= EP_HALT;
//EXTERN volatile bit Ep_StatusIN3   EXT_TERM	= EP_HALT;

// FIFO status of endpoints
EXTERN volatile bit IN_FIFO_empty		EXT_TERM	= TRUE;
EXTERN volatile bit OUT_FIFO_loaded		EXT_TERM	= FALSE;
*/

//-----------------------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------------------

void Usb0_Init(void);
void Usb_Suspend(void);
void Usb_Resume(void);

/*
// Used for multiple byte reads of Endpoint FIFOs
void Fifo_Read (BYTE, BYTE, BYTE *);
// Used for multiple byte writes of Endpoint FIFOs
void Fifo_Write (BYTE, BYTE, BYTE *);
*/

/*
void Vendor_Request( void );			// Vendor request
*/

#endif	// USB_ISR_H

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
