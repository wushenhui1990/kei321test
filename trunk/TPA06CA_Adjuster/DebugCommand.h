#pragma once
//#include "ComCommunicate.h"



class DebugCommand
{
public:
//	DebugCommand(void);
//	~DebugCommand(void);
	BOOL DebugInit(LPCTSTR ComStr,DWORD BaudRate);			//open r/w endpoints
	VOID DebugUnInit(void);
	BOOL WriteData(DWORD WriteSize, UCHAR *DataBuf);
	BOOL ReadData(DWORD ReadSize, UCHAR *DataBuf);
	BOOL WriteRegister(DWORD addr, UCHAR value);
	BOOL ReadRegister(DWORD addr, UCHAR *pvalue);
	BOOL BurstWrite(DWORD addr,UCHAR *buf,DWORD len);
	BOOL BurstRead(DWORD addr,UCHAR *buf,DWORD len);
private:
	//ComCommunicate m_com;
};