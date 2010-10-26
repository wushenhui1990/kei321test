#ifndef __USB_FIFO_RW_H__
#define __USB_FIFO_RW_H__

typedef unsigned char BYTE;

//typedef BYTE idata buf_t;
//typedef BYTE pdata buf_t;
typedef BYTE xdata buf_t;
//typedef BYTE code buf_t;

void FIFO_Read( BYTE fifo_adr, BYTE n, buf_t * ptr );
void FIFO_Write( BYTE fifo_adr, BYTE n, buf_t * ptr  );


#endif