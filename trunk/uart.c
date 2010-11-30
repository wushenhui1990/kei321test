#include "config.h"
#include <stdarg.h>
#include <stdio.h>
#include "c8051f320.h"
#include "type.h"
#include "uart.h"
#include "bitop.h"
#include "INTRINS.H"
#include "ev.h"

#include "fpga_define.h"
#include "F3xx_USB0_ReportHandler.h"


#if(UART_DEBUG==1)


void uart_putc(char ch)
{
	if(ch=='\n')
	{
		SBUF0='\r';
		while (TI0== 0 ); 
		TI0= 0 ;
	}

	SBUF0=ch;
	while (TI0== 0 ); 
	TI0= 0 ;
	 
}

void uart_putstring(char *str)
{
    while (*str)
    {
        uart_putc(*(str++));
    }
}

/*************************************************************************
Function: uart_puthex_nibble()
Purpose: transmit lower nibble as ASCII-hex to UART
Input: byte value
Returns: none
This functions has been added by Martin Thomas <eversmith@heizung-thomas.de>
Don't blame P. Fleury if it doesn't work ;-)
**************************************************************************/
void uart_puthex_nibble(unsigned char b)
{
    unsigned char c = b & 0x0f;
    if (c>9) c += 'A'-10;
    else c += '0';
    uart_putc(c);
} /* uart_puthex_nibble */

/*************************************************************************
Function: uart_puthex_byte()
Purpose: transmit upper and lower nibble as ASCII-hex to UART
Input: byte value
Returns: none
This functions has been added by Martin Thomas <eversmith@heizung-thomas.de>
Don't blame P. Fleury if it doesn't work ;-)
**************************************************************************/
void uart_puthex_byte_(unsigned char b)
{
    uart_puthex_nibble(b>>4);
    uart_puthex_nibble(b);
}
void uart_puthex_byte(unsigned char b)
{
    if(b & 0xF0)
        uart_puthex_nibble(b>>4);
    uart_puthex_nibble(b);

	uart_putc(' ');
}

 /* uart_puthex_byte */
/*************************************************************************
Function: uart_puthex_word()
Purpose: transmit upper and lower byte as ASCII-hex to UART
Input: word value
Returns: none
This functions has been added by Matthias Urlichs.
Don't blame P. Fleury if it doesn't work ;-)
**************************************************************************/
void uart_puthex_word(u16 b)
{
    if (b&0xFF00) {
        uart_puthex_byte(b>>8);
        uart_puthex_byte_(b);
    } else {
        uart_puthex_byte(b);
    }
}

#endif


//------------------------------------------------
//uart commands
#define CMD_PBI					0x4C			//single r/w,burst w
#define CMD_BRPBI				0x58			//burst r
//pbi commands
#define PBI_APB_WRITE			0x6D			//apb w
#define PBI_APB_READ			0x6C			//apb r
#define PBI_DATA				0x75			//apb data to writ


#define ADDR_BIT_START	8


#define ERR_CODE_NO_ERR			0x00
#define ERR_CODE_READ_REG  	0x01
#define ERR_CODE_WRITE_REG	0x02

u8 code cmd_config_sensor[] = 
{
	 0x12, 0x80  ,
	 0x13, 0x00  ,
	 0x12, 0x11  ,//0x00
	 0xD5, 0x10  ,
	 0x0C, 0x42  ,//0xc2
	 0x0D, 0x34  ,
	 0x17, 0x25  ,
	 0x18, 0xA0  ,
	 0x19, 0x03  ,
	 0x1A, 0xF0  ,
	 0x1B, 0x89  ,
	 0x22, 0x03  ,
	 0x29, 0x17  ,
	 0x2B, 0x00  ,
	 0x2C, 0x01  ,
	 0x31, 0xA0  ,
	 0x32, 0xF0  ,
	 0x33, 0xC4  ,
	 0x35, 0x05  ,
	 0x04, 0x60  ,
	 0x27, 0x80  ,
	 0x3D, 0x0F  ,
	 0x3E, 0x81  ,
	 0x3F, 0x40  ,
	 0x40, 0x7F  ,
	 0x41, 0x6A  ,
	 0x42, 0x29  ,
	 0x44, 0xE5  ,
	 0x45, 0x41  ,
	 0x47, 0x42  ,
	 0x48, 0x00  ,
	 0x49, 0x61  ,
	 0x4A, 0xA1  ,
	 0x4B, 0x5E  ,
	 0x4C, 0x18  ,
	 0x4D, 0x50  ,
	 0x4E, 0x13  ,
	 0x64, 0x00  ,
	 0x67, 0x88  ,
	 0x68, 0x1A  ,
	 0x14, 0x18  ,
	 0x24, 0x40  ,
	 0x25, 0x30  ,
	 0x26, 0x72  ,
	 0x50, 0x97  ,
	 0x51, 0x7E  ,
	 0x52, 0x00  ,
	 0x53, 0x00  ,
	 0x20, 0x00  ,
	 0x21, 0x23  ,
	 0x38, 0x14  ,
	 0xE9, 0x00  ,
	 0x56, 0x55  ,
	 0x57, 0xFF  ,
	 0x58, 0xFF  ,
	 0x59, 0xFF  ,
	 0x5F, 0x04  ,
	 0xEC, 0x00  ,
	 0x13, 0x02  ,
	 0x80, 0x7D  ,
	 0x80, 0x69  ,
	 0x81, 0x3F  ,
	 0x82, 0x32  ,
	 0x83, 0x01  ,
	 0x38, 0x11  ,
	 0x84, 0x70  ,
	 0x85, 0x02  ,
	 0x86, 0x03  ,
	 0x87, 0x01  ,
	 0x88, 0x05  ,
	 0x89, 0x30  ,
	 0x8D, 0x30  ,
	 0x8F, 0x85  ,
	 0x93, 0x30  ,
	 0x95, 0x85  ,
	 0x99, 0x30  ,
	 0x9B, 0x85  ,
	 0x9C, 0x10  ,
	 0x9D, 0x1C  ,
	 0x9E, 0x33  ,
	 0x9F, 0x5C  ,
	 0xA0, 0x6B  ,
	 0xA1, 0x77  ,
	 0xA2, 0x80  ,
	 0xA3, 0x8A  ,
	 0xA4, 0x91  ,
	 0xA5, 0x98  ,
	 0xA6, 0xA5  ,
	 0xA7, 0xB1  ,
	 0xA8, 0xC6  ,
	 0xA9, 0xD9  ,
	 0xAA, 0xEA  ,
	 0xAB, 0x1D  ,
	 0xCE, 0x78  ,
	 0xCF, 0x6E  ,
	 0xD0, 0x0A  ,
	 0xD1, 0x0C  ,
	 0xD2, 0x84  ,
	 0xD3, 0x90  ,
	 0xD4, 0x1E  ,
	 0x5A, 0x24  ,
	 0x5B, 0x1F  ,
	 0x5C, 0x88  ,
	 0x5D, 0x60  ,
	 0xAC, 0x6E  ,
	 0xBE, 0xFF  ,
	 0xBF, 0x00  ,
	 0x70, 0x00  ,
	 0x71, 0x34  ,
	 0x74, 0x28  ,
	 0x75, 0x98  ,
	 0x76, 0x00  ,
	 0x77, 0x08  ,
	 0x78, 0x01  ,
	 0x79, 0xC2  ,
	 0x7D, 0x02  ,
	 0x7A, 0x4E  ,
	 0x7B, 0x1F  ,
	 0xEC, 0x00  ,
	 0x7C, 0x0C  ,
	 0x15, 0x00  ,
	 0x2B, 0x00  ,
	 0x2C, 0x02  ,
	 0xEC, 0x40  ,
	 0xCC, 0x44  ,
	 0xDA, 0x07  ,
	 0xE4, 0x02  ,
	 0x15, 0x00  ,
	 0x00, 0x3A  ,
	 0x2D, 0x00  ,
	 0x2E, 0x00  ,
	 0x0F, 0x00  ,
	 0x10, 0x25  ,	//0xC5
	 0x1b, 0x84  ,
	 0x1a, 0x0D  ,  
	 0x19, 0x77  ,
};
u8 code cmd_config_sensor_cnt = (sizeof(cmd_config_sensor)>>1);


u8 uart_send(u8 *buff,u16 len)
{
	u16 data i;
	bit easave;
	easave = EA;
	EA = 0;

    for(i=0;i<len;i++)
    {
		while (!TI0);  	
		TI0 = 0 ;
 		SBUF0=buff[i];
    }
	EA = easave;
	return ERR_CODE_NO_ERR;    
}


#if 1
u8 uart_read(u8 *buff,u16 len)
{
	u16 data i;
	u8	idata cnt;//
	bit easave;
	easave = EA;
	EA = 0;
    for(i=0;i<len;i++)
    {	
		cnt = 0xff;	

		while((--cnt)&& (!RI0));
		if(cnt)
		{
			RI0 = 0 ;
			buff[i] = SBUF0;
		}
		else
		{
			break;			 
		}
    }
	EA = easave;
	if(!cnt)
		return ERR_CODE_READ_REG;	 //wait timeout
	else
		return ERR_CODE_NO_ERR;    //ok
}
#else
u8 uart_read(u8 *buff,u16 len)
{
	u16 idata i;
    for(i=0;i<len;i++)
    {	
		while (!RI0); 
		RI0 = 0 ;
		buff[i] = SBUF0;
    }

	return ERR_CODE_NO_ERR;    //ok
}
#endif
void delay(void)
{
	u8 volatile idata cnt = 0xff;
	while(--cnt);
	//cnt = 0xff;
	//while(--cnt);
}
u8 uart_write_reg(u32 addr, u8 value)
{
	u8  data buf[6];
	u8  data err;

	buf[0] = CMD_PBI | 0x01;
	buf[1] = PBI_APB_WRITE;
	buf[2] = (u8)addr;
	buf[3] = (u8)(addr>>8);
	buf[4] = (u8)(addr>>16);
	buf[5] = value;

	err = uart_send(buf,6);

	delay();

	return err ;
}

u8 uart_read_reg(u32 addr, u8 *pvalue)
{
	u8  data buf[5];
	u8  data err;
	buf[0] = CMD_PBI;
	buf[1] = PBI_APB_READ;
	buf[2] = (u8)addr;
	buf[3] = (u8)(addr>>8);
	buf[4] = (u8)(addr>>16);

	err = uart_send(buf,5);
	if(err)
		return err;
	err = uart_read(pvalue,1);

	return err;
}
/*
void uart_burst_write(u32 addr,u8 *buf,u8 len)
{
	u8 bufh[5];

	bufh[0] = CMD_PBI;
	bufh[1] = PBI_APB_WRITE;
	bufh[2] = (u8)addr;
	bufh[3] = (u8)(addr>>8);
	bufh[4] = (u8)(addr>>16);
	uart_send(bufh,5);
	uart_send(buf,len);

}
 */
void uart_burst_read(u32 addr,u8 *buf,u16 len)
{
	u8  data bufh[5];

	uart_write_reg(UART_RLENL,len&0x00ff);
	uart_write_reg(UART_RLENH,(len&0xff00)>>8);

	bufh[0] = CMD_BRPBI;
	bufh[1] = PBI_APB_READ;
	bufh[2] = (u8)addr;
	bufh[3] = (u8)(addr>>8);
	bufh[4] = (u8)(addr>>16);
	uart_send(bufh,5);
	uart_read(buf,len);
}
/*
		else if (!strcmp(command,"i2cwd"))
		{
			fscanf(fScript,"%x",&Addr);
			fscanf(fScript,"%x",&Reg);
			sprintf(line,"write sensor reg 0x%02x 0x%02x\n",Addr,Reg);
			dlg->PrintInfo(line);
			dlg->m_Debug.WriteRegister(0xC405,0x21);
			dlg->m_Debug.WriteRegister(0xC408,(UCHAR)Addr);
			dlg->m_Debug.WriteRegister(0xC409,(UCHAR)Reg);
			dlg->m_Debug.WriteRegister(0xC406,0x14);
			dlg->m_Debug.WriteRegister(0xC404,0x01);
		else if (!strcmp(command,"i2crd"))
		{
			fscanf(fScript,"%x",&Addr);
			fscanf(fScript,"%x",&RegComp);
			sprintf(line,"read sensor reg:0x%02x=",Addr);
			dlg->PrintInfo(line);
			dlg->m_Debug.WriteRegister(0xC405,0x21);						//i2c slave address
			dlg->m_Debug.WriteRegister(0xC408,(UCHAR)Addr);					//sensor reg address
			dlg->m_Debug.WriteRegister(0xC406,0x12);						//write len 1byte
			dlg->m_Debug.WriteRegister(0xC404,0x01);						//start write
			dlg->m_Debug.WriteRegister(0xC406,0x13);						//read len 1byte
			dlg->m_Debug.WriteRegister(0xC404,0x01);						//start read
			dlg->m_Debug.ReadRegister(0xC40B,(UCHAR *)&Reg);
*/

void i2c_write_reg(u8 addr,u8 val)
{
	uart_write_reg(I2C_ADDR,0x21);	
	uart_write_reg(I2C_DATA0,addr);
	uart_write_reg(I2C_DATA1,val); 
	uart_write_reg(I2C_CFG,0x14);  
	uart_write_reg(I2C_CTRL,0x01);
	delay();


}


u8 i2c_read_reg(u8 addr,u8* val)
{
	u8 idata ret;
	uart_write_reg(I2C_ADDR,0x21);						//i2c slave address
	uart_write_reg(I2C_DATA0,addr);						//sensor reg address
	uart_write_reg(I2C_CFG,0x12);						//write len 1byte
	uart_write_reg(I2C_CTRL,0x01);						//start write
	delay();

	uart_write_reg(I2C_CFG,0x13);						//read len 1byte
	uart_write_reg(I2C_CTRL,0x01);						//start read
	delay();

	ret = uart_read_reg(I2C_RDATA,val);
	return ret;
}


void config_sensor(void)
{
 
	u8 idata idx,addr,val;
	//u8 ret;
	for(idx = 0; idx <cmd_config_sensor_cnt; idx++)
	{
		addr = 	cmd_config_sensor[idx<<1];
		val  =  cmd_config_sensor[(idx<<1)+1];
	
		i2c_write_reg(addr,val);
		
		//ret = i2c_read_reg(addr,&val);
		//_nop_();	
	}

}

extern cam_send_img_stat_st cam_status[CAM_COUNT];
//extern unsigned char idata IN_PACKET[IN_PACKET_LEN];
//u8 	b_first = 1;
//u32 b_cnt = 0;

#if 0
void get_frame_data(void)
{
	u16 volatile i ;
	u8 val = 0;
	u8 read_cnt,remain;

   	uart_write_reg(PBI_MODE,0x01);
 	uart_write_reg(SIF_FRMSTART,0x01);

  	IN_BUFFER.Ptr = IN_PACKET;	
	IN_BUFFER.Length = REPORT_ID_IN_IMAGE_LEN+1;


	while(1)
	{
		uart_read_reg(SIF_FRMSTART,&val);
		if(val==0) break;
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();

	}

	uart_write_reg(PDMC_VSYNC,0x01);

   	read_cnt = 	(IMAGE_WIDTH*IMAGE_HEIGHT)/BREAD_ONCE;
	remain   =  (IMAGE_WIDTH*IMAGE_HEIGHT)%BREAD_ONCE;


	for(i = 0; i<read_cnt;i++)
	{
	//eabak = EA;
	///	EA = 0;
		uart_burst_read(PDMC_PDATA,&IN_PACKET[4],BREAD_ONCE);

		IN_PACKET[0] = REPORT_ID_IN_IMAGE;
		IN_PACKET[1] = cam_status[0].cam_num;
		if(i==0)
		{
			IN_PACKET[1] &= ~0xc0;
			IN_PACKET[1] |= 0x80;
		}
		IN_PACKET[2] = cam_status[0].send_cur_idx;
		IN_PACKET[3] = BREAD_ONCE;
		cam_status[0].send_cur_idx ++;
		
		send_debug_info_to_host(REPORT_ID_IN_IMAGE);
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();

	//	EA = eabak;
	}
	if(remain)
	{
	//	eabak = EA;
	//	EA = 0;
		uart_burst_read(PDMC_PDATA,&IN_PACKET[4],remain);

		IN_PACKET[0] = REPORT_ID_IN_IMAGE;
		IN_PACKET[1] = cam_status[0].cam_num;
		IN_PACKET[1] &= ~0xc0;
		IN_PACKET[1] |= 0x40;

		IN_PACKET[2] = cam_status[0].send_cur_idx;
		IN_PACKET[3] = remain;
		cam_status[0].send_cur_idx ++;

		send_debug_info_to_host(REPORT_ID_IN_IMAGE);
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();
		_nop_();_nop_();_nop_();_nop_();

	//	EA = eabak;
	}

	cam_status[0].send_cur_idx%=cam_status[0].send_tot_cnt;

	uart_write_reg(PBI_MODE,0x0);

}
#else
void get_frame_data(void)
{
//	u16 volatile i ;
	u8 idata val = 0;
	//u8 read_cnt,remain;

	IN_PACKET[1] = 0;

	if(cam_status[0].send_cur_idx == 0)//first 
	{
		
		uart_write_reg(PBI_MODE,0x01);
		uart_write_reg(SIF_FRMSTART,0x01);
		while(1)
		{
			uart_read_reg(SIF_FRMSTART,&val);
			if(val==0) break;
			_nop_();_nop_();_nop_();_nop_();
			_nop_();_nop_();_nop_();_nop_();
			_nop_();_nop_();_nop_();_nop_();
			_nop_();_nop_();_nop_();_nop_();
		
		}
		
		uart_write_reg(PDMC_VSYNC,0x01);
		
		IN_PACKET[1] &= ~0xc0;	//start flag
		IN_PACKET[1] |= 0x80;
		
	}
	
	uart_burst_read(PDMC_PDATA,&IN_PACKET[4],BREAD_ONCE);			//read data

	IN_PACKET[3] = BREAD_ONCE;
	
	if(cam_status[0].send_cur_idx == cam_status[0].send_tot_cnt-1)	//last 
	{		
		uart_write_reg(PBI_MODE,0x0);
		
		IN_PACKET[1] &= ~0xc0;	//end flag
		IN_PACKET[1] |= 0x40;
		IN_PACKET[3] = cam_status[0].remain;
	}


	//---------------------------------------------------------
	IN_PACKET[0]  = REPORT_ID_IN_DBGINFO;
	IN_PACKET[1] |= cam_status[0].cam_num;
	IN_PACKET[2]  = cam_status[0].send_cur_idx;

	

  	IN_BUFFER.Ptr = IN_PACKET;	
	IN_BUFFER.Length = REPORT_ID_IN_DBGINFO+1;
	

	cam_status[0].send_cur_idx++;
	cam_status[0].send_cur_idx%=cam_status[0].send_tot_cnt;

	//event_send(EVENT_ID_RETURN_HOST_CMD);
	//send_debug_info_to_host(REPORT_ID_IN_DBGINFO);

}
#endif



