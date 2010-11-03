/////////////////////////////////////
//  Generated Initialization File  //
/////////////////////////////////////

#include "C8051F320.h"

// Peripheral specific initialization functions,
// Called from the Init_Device() function
void PCA_Init()
{
    PCA0CN    = 0x40;
    PCA0MD    &= ~0x40;
    PCA0MD    = 0x00;
    PCA0CPM0  = 0x46;
    PCA0CPL4  = 0xE9;
    //PCA0MD    |= 0x40;
    PCA0CPH0  = 0x01;
}

void Timer_Init()
{
    TCON      = 0x50;
    TMOD      = 0x21;
    CKCON     = 0x0A;
    TL0       = 0x0D;
    TH0       = 0xFE;
    TH1       = 0x98;
}

void Port_IO_Init()
{
    // P0.0  -  Skipped,     Open-Drain, Digital
    // P0.1  -  Skipped,     Open-Drain, Digital
    // P0.2  -  Skipped,     Open-Drain, Digital
    // P0.3  -  Skipped,     Open-Drain, Digital
    // P0.4  -  TX0 (UART0), Open-Drain, Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  Skipped,     Open-Drain, Digital
    // P0.7  -  Skipped,     Open-Drain, Digital

    // P1.0  -  Skipped,     Open-Drain, Digital
    // P1.1  -  Skipped,     Open-Drain, Digital
    // P1.2  -  Skipped,     Open-Drain, Digital
    // P1.3  -  Skipped,     Open-Drain, Digital
    // P1.4  -  Skipped,     Open-Drain, Digital
    // P1.5  -  Skipped,     Open-Drain, Digital
    // P1.6  -  Skipped,     Open-Drain, Digital
    // P1.7  -  Skipped,     Open-Drain, Digital
    // P2.0  -  Skipped,     Open-Drain, Digital
    // P2.1  -  CEX0 (PCA),  Push-Pull,  Digital
    // P2.2  -  Unassigned,  Open-Drain, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital

    P2MDOUT   = 0x02;
    P0SKIP    = 0xCF;
    P1SKIP    = 0xFF;
    P2SKIP    = 0x01;
    XBR0      = 0x01;
    XBR1      = 0xC1;
}

void Oscillator_Init()
{
    int i = 0;
    CLKMUL    = 0x80;
    for (i = 0; i < 20; i++);    // Wait 5us for initialization
    CLKMUL    |= 0xC0;
    while ((CLKMUL & 0x20) == 0);
    CLKSEL    = 0x02;
    OSCICN    = 0x83;
}
/*
void Interrupts_Init()
{
    EIE1      = 0x02;
    IE        = 0x82;
}
*/
// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    PCA_Init();
    Timer_Init();
    Port_IO_Init();
    Oscillator_Init();
    //Interrupts_Init();
}
