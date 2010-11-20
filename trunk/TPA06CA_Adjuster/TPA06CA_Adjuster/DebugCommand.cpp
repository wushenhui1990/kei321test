#include "StdAfx.h"
#include "DebugCommand.h"
#include "TPA06CA_Adjuster.h"
#include "TPA06CA_AdjusterDlg.h"

//-------------------------------------------------------------------------------
// defines
//-------------------------------------------------------------------------------
//uart commands
#define CMD_PBI					0x4C			//single r/w,burst w
#define CMD_BRPBI				0x58			//burst r
//pbi commands
#define PBI_APB_WRITE			0x6D			//apb w
#define PBI_APB_READ			0x6C			//apb r
#define PBI_DATA				0x75			//apb data to write


extern 	CHIDDevice HID_Blinky;

//-------------------------------------------------------------------------------
// function members
//-------------------------------------------------------------------------------
/*
DebugCommand::DebugCommand(void)
{
}

DebugCommand::~DebugCommand(void)
{
}
*/
BOOL DebugCommand::DebugInit(LPCTSTR ComStr,DWORD BaudRate)
{

	//if (FALSE==m_com.OpenCom(ComStr))			//modify when debug
	//	return FALSE;
	//else
	//	return m_com.ConfigCom(BaudRate,1024,1024);
}

VOID DebugCommand::DebugUnInit(void)
{
	//m_com.CloseCom();
}

BOOL DebugCommand::WriteData(DWORD WriteSize, UCHAR *DataBuf)
{
	//return m_com.WriteCom(DataBuf,WriteSize);
}

BOOL DebugCommand::ReadData(DWORD ReadSize, UCHAR *DataBuf)
{
	//return m_com.ReadCom(DataBuf,ReadSize);
}

BOOL DebugCommand::WriteRegister(DWORD addr, UCHAR value)
{
	//UCHAR buf[8];

	//buf[0] = CMD_PBI;
	//buf[1] = PBI_APB_WRITE;
	//buf[2] = (UCHAR)addr;
	//buf[3] = (UCHAR)(addr>>8);
	//buf[4] = (UCHAR)(addr>>16);
	//buf[5] = value;

	//return m_com.WriteCom(buf,6);

	unsigned char buff[8];
	buff[0] = REPORT_ID_OUT_CMD;
	buff[1] = DATA_CMD_WRITE_REG;
	buff[2] = (unsigned char)addr;
	buff[3] = (unsigned char)(addr>>8);
	buff[4] = (unsigned char)(value);
	HID_Blinky.SetOutputReport_Control(buff,REPORT_ID_OUT_CMD_LEN+1);


}

BOOL DebugCommand::ReadRegister(DWORD addr, UCHAR *pvalue)
{
	//UCHAR buf[8];

	//buf[0] = CMD_PBI;
	//buf[1] = PBI_APB_READ;
	//buf[2] = (UCHAR)addr;
	//buf[3] = (UCHAR)(addr>>8);
	//buf[4] = (UCHAR)(addr>>16);

	//if(FALSE==m_com.WriteCom(buf,5))
	//	return FALSE;
	//else
	//	return m_com.ReadCom(pvalue,1);

	unsigned char buff[8];
	buff[0] = REPORT_ID_OUT_CMD;
	buff[1] = DATA_CMD_READ_REG;
	buff[2] = (unsigned char)addr;
	buff[3] = (unsigned char)(addr>>8);
	HID_Blinky.SetOutputReport_Control(buff,REPORT_ID_OUT_CMD_LEN+1);

}

BOOL DebugCommand::BurstWrite(DWORD addr,UCHAR *buf,DWORD len)
{
	//UCHAR bufh[8];

	//bufh[0] = CMD_PBI;
	//bufh[1] = PBI_APB_WRITE;
	//bufh[2] = (UCHAR)addr;
	//bufh[3] = (UCHAR)(addr>>8);
	//bufh[4] = (UCHAR)(addr>>16);
	//if (FALSE==m_com.WriteCom(bufh,5))
	//	return FALSE;
	//else
	//	return m_com.WriteCom(buf,len);

}

BOOL DebugCommand::BurstRead(DWORD addr,UCHAR *buf,DWORD len)
{
	UCHAR bufh[8];

	bufh[0] = CMD_BRPBI;
	bufh[1] = PBI_APB_READ;
	bufh[2] = (UCHAR)addr;
	bufh[3] = (UCHAR)(addr>>8);
	bufh[4] = (UCHAR)(addr>>16);
	if (FALSE==m_com.WriteCom(bufh,5))
		return FALSE;
	else
		return m_com.ReadCom(buf,len);
}