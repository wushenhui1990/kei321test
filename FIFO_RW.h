/*
 *   FIFO_RW.h
 */

#ifndef FIFO_RW_H
#define FIFO_RW_H

extern void FIFO_Read_idata(    unsigned char fifo_adr, unsigned char n, unsigned char idata * ptr );
extern void FIFO_Read_pdata(    unsigned char fifo_adr, unsigned char n, unsigned char pdata * ptr );
extern void FIFO_Read_xdata(    unsigned char fifo_adr, unsigned char n, unsigned char xdata * ptr );
extern void FIFO_Read_generic(  unsigned char fifo_adr, unsigned char n, unsigned char       * ptr );

extern void FIFO_Write_idata(   unsigned char fifo_adr, unsigned char n, unsigned char idata * ptr );
extern void FIFO_Write_pdata(   unsigned char fifo_adr, unsigned char n, unsigned char pdata * ptr );
extern void FIFO_Write_xdata(   unsigned char fifo_adr, unsigned char n, unsigned char xdata * ptr );
extern void FIFO_Write_code(    unsigned char fifo_adr, unsigned char n, unsigned char code  * ptr );
extern void FIFO_Write_generic( unsigned char fifo_adr, unsigned char n, unsigned char       * ptr );

#endif	/* FIFO_RW_H */
