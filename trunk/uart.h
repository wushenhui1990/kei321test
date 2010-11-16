#ifndef __UART_H__
#define __UART_H__

#include "config.h"
#include "type.h"

#if(UART_DEBUG==1)
	void uart_putstring(char *str);
	void uart_puthex_byte(u8 b);
	void uart_puthex_word(u16 b);

	#define FS(x)  	uart_putstring	 x
	#define FB(x) 	uart_puthex_byte x
	#define FW(x) 	uart_puthex_word x
#else
	#define FS(x)
	#define FB(x)
	#define FW(x)
#endif


	u8 uart_write_reg(u32 addr, u8 value);
	u8 uart_read_reg(u32 addr,  u8 *pvalue);
	void i2c_write_reg(u8 addr,u8 val);
	u8 i2c_read_reg(u8 addr,u8* val);
	void config_sensor(void);
	void uart_burst_write(u32 addr,u8 *buf,u8 len);
	void uart_burst_read(u32 addr,u8 *buf,u16 len);

  	#define IMAGE_WIDTH		640
	#define IMAGE_HEIGHT	16
	#define BREAD_ONCE		60

   void get_frame_data(void);

#endif