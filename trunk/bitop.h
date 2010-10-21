#ifndef __BIT_OP_H__
#define __BIT_OP_H__

#define BIT0  0x01
#define BIT1  0x02
#define BIT2  0x04
#define BIT3  0x08
#define BIT4  0x10
#define BIT5  0x20
#define BIT6  0x40
#define BIT7  0x80


#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_get(p,m) ((p) & (m))
#define bit_flip(p,m) ((p) ^= (m))


#endif