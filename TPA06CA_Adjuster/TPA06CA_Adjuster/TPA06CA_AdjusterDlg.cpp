// TPA06CA_AdjusterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TPA06CA_Adjuster.h"
#include "TPA06CA_AdjusterDlg.h"
#include <shlwapi.h>
#include <afxmt.h>
#include "dbt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define HID_Blinky_VID 0x10C4
#define HID_Blinky_PID 0x82CD








#define WM_MAIN_WINDOW_FLUSH_MSG	WM_USER+208 
#define WM_SHOWVIDEO				WM_USER+209

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTPA06CA_AdjusterDlg dialog




CTPA06CA_AdjusterDlg::CTPA06CA_AdjusterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTPA06CA_AdjusterDlg::IDD, pParent)
	, m_RegAddr(_T(""))
	, m_RegValue(_T(""))
	, m_ScriptPath(_T(""))
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	hloc1=hloc2=hloc3 = NULL;
	argbq = NULL;
}

void CTPA06CA_AdjusterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_REG_ADDR, m_RegAddr);
	DDX_Text(pDX, IDC_EDIT_REG_VAL, m_RegValue);
	DDX_Control(pDX, IDC_RICHEDIT_DEBUG_INFO, m_RichEditPrintf);
	DDX_Text(pDX, IDC_EDIT_INI_PATH, m_ScriptPath);
	DDX_Control(pDX, IDC_COMBO_WORK_STYLE, m_work_style);
}

BEGIN_MESSAGE_MAP(CTPA06CA_AdjusterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DEVICECHANGE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_TESTCMD, &CTPA06CA_AdjusterDlg::OnBnClickedButtonTestcmd)
	ON_BN_CLICKED(IDC_BUTTON_REG_WRITE, &CTPA06CA_AdjusterDlg::OnBnClickedButtonRegWrite)
	ON_BN_CLICKED(IDC_BUTTON_REG_READ, &CTPA06CA_AdjusterDlg::OnBnClickedButtonRegRead)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_INI_FILE, &CTPA06CA_AdjusterDlg::OnBnClickedButtonOpenIniFile)
	ON_BN_CLICKED(IDC_BUTTON_RUN_SCRIPT, &CTPA06CA_AdjusterDlg::OnBnClickedButtonRunScript)

	ON_MESSAGE(WM_MAIN_WINDOW_FLUSH_MSG,&CTPA06CA_AdjusterDlg::OnMainWindowFlush)
//	ON_MESSAGE(WM_MAIN_WINDOW_FLUSH_MSG,OnMainWindowRecv) 
ON_BN_CLICKED(IDC_BUTTON_CLEAR_DEBUG_INFO, &CTPA06CA_AdjusterDlg::OnBnClickedButtonClearDebugInfo)
ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CTPA06CA_AdjusterDlg::OnBnClickedButtonPreview)
ON_MESSAGE(WM_SHOWVIDEO,&CTPA06CA_AdjusterDlg::OnShowVideo)
ON_WM_DESTROY()
END_MESSAGE_MAP()


LRESULT CTPA06CA_AdjusterDlg:: OnMainWindowFlush(WPARAM wParam,LPARAM lParam)
{

	UpdateData(FALSE);
	//PrintInfo("recv msg\n");
	return 1;
}
// CTPA06CA_AdjusterDlg message handlers

#define MSG_WRITE_REG			WM_USER+100
#define MSG_READ_REG			WM_USER+101
#define MSG_I2C_WRITE_REG		WM_USER+102
#define MSG_I2C_READ_REG		WM_USER+103
#define MSG_SWITCH_WORK_STYLE	WM_USER+104

#define MSG_MAIN_WORK_EXIT		WM_USER+110

#define WORK_STYLE_PREVIEW		0x01
#define WORK_STYLE_CAL			0x02
#define WORK_STYLE_NOMAL		0x03


CWinThread* p_thread_main_work = NULL;
CWinThread* p_RunScriptThread = NULL;
CWinThread* p_thread_preview =NULL;
CEvent g_event_reg_rdwr(TRUE,FALSE,0,0);
CEvent g_event_config_sensor_success(FALSE,FALSE,0,0);

unsigned char g_cmd_buff[32] ;

#define TIMEOUT_REG_READ_WRITE	1000

char debugstr[128] = {0};
unsigned char * m_pYdata =NULL;
//signal,auto,
unsigned int main_work_thread(void *param)
{
    MSG msg;
	DWORD	status;
	unsigned char buff[8] = {0};
	unsigned char reportbuffer[256] = {0};
	char*pInfo = NULL;
	int addr,val,retval;
	char str[64];
	int cnt;


	CTPA06CA_AdjusterDlg*pthis = (CTPA06CA_AdjusterDlg*)param;

    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    
    while(true)
    {
        if(GetMessage(&msg,0,0,0)) //get msg from message queue
        {
            switch(msg.message)
            {
            case MSG_WRITE_REG:
				{
					pInfo = (char *)msg.wParam;
					buff[0] = REPORT_ID_OUT_CMD;
					buff[1] = DATA_CMD_WRITE_REG;
					buff[2] = pInfo[0];	//addr
					buff[3] = pInfo[1];
					buff[4] = pInfo[2]; //val

					sprintf_s(debugstr,"write reg[%x][%02x] ",buff[2]|(buff[3]<<8),buff[4]);
					pthis->PrintInfo(debugstr);

					pthis->HID_Blinky.SetOutputReport_Interrupt(buff,REPORT_ID_OUT_CMD_LEN+1);	//write

					status = pthis->HID_InterruptGetReport (reportbuffer);						//read
					if(status == HID_DEVICE_SUCCESS)
					{
						if((reportbuffer[0]==REPORT_ID_IN_IMAGE) &&				//
						   ((reportbuffer[1]&0x3f) == DATA_CMD_WRITE_REG) &&		
						   (reportbuffer[2] == 0))
						{
							addr = (reportbuffer[3])|(reportbuffer[4]<<8);
							val  = reportbuffer[5];
							
							pthis->PrintInfo("ok");
						}
						else
						{
							pthis->PrintInfo("some err1\n");
						}
					}

					::SendMessage(pthis->m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);
					g_event_reg_rdwr.SetEvent();

					break;
				}

            case MSG_READ_REG:
				{
					pInfo = (char *)msg.wParam;
					buff[0] = REPORT_ID_OUT_CMD;
					buff[1] = DATA_CMD_READ_REG;
					buff[2] = pInfo[0];	//addr
					buff[3] = pInfo[1];
					buff[4] = pInfo[2];	//comp val

					sprintf_s(debugstr,"read  reg[%x]",buff[2]|(buff[3]<<8));
					pthis->PrintInfo(debugstr);

					pthis->HID_Blinky.SetOutputReport_Interrupt(buff,REPORT_ID_OUT_CMD_LEN+1);

					status = pthis->HID_InterruptGetReport (reportbuffer);
					if(status == HID_DEVICE_SUCCESS)
					{
						if((reportbuffer[0]==REPORT_ID_IN_IMAGE) &&				//
						   ((reportbuffer[1]&0x3f) == DATA_CMD_READ_REG) &&		
						   (reportbuffer[2] == 0))
						{
							addr = (reportbuffer[3])|(reportbuffer[4]<<8);
							val  = reportbuffer[5];
							retval = reportbuffer[6];

							pthis->m_RegValue.Format(_T("0x%x"),retval);

							sprintf_s(str,"[%02x][%02x] ok\n",val,retval);

							pthis->PrintInfo(str);

							//pthis->PrintInfo("ok");
						}
						else
						{
							pthis->PrintInfo("some err2\n");
						}
					}

					::SendMessage(pthis->m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);
					g_event_reg_rdwr.SetEvent();

					break;
				}
			case MSG_I2C_WRITE_REG:
				{
					pInfo = (char *)msg.wParam;
					buff[0] = REPORT_ID_OUT_CMD;
					buff[1] = DATA_CMD_I2C_WRITE_REG;
					buff[2] = pInfo[0];	//addr
					buff[3] = pInfo[1]; //val

					sprintf_s(debugstr,"i2c write reg[%02x][%02x] ",buff[2],buff[3]);
					pthis->PrintInfo(debugstr);

					pthis->HID_Blinky.SetOutputReport_Interrupt(buff,REPORT_ID_OUT_CMD_LEN+1);

					status = pthis->HID_InterruptGetReport (reportbuffer);						//read
					if(status == HID_DEVICE_SUCCESS)
					{
						if((reportbuffer[0]==REPORT_ID_IN_IMAGE) &&				//
						   ((reportbuffer[1]&0x3f) == DATA_CMD_I2C_WRITE_REG) &&		
						   (reportbuffer[2] == 0))
						{
							addr = reportbuffer[3];
							val  = reportbuffer[4];
							
							pthis->PrintInfo("ok\n");
						}
						else
						{
							pthis->PrintInfo("some err3\n");
						}
					}

					::SendMessage(pthis->m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);
					g_event_reg_rdwr.SetEvent();

					break;

				}
			case MSG_I2C_READ_REG:
				{
					pInfo = (char *)msg.wParam;
					buff[0] = REPORT_ID_OUT_CMD;
					buff[1] = DATA_CMD_I2C_READ_REG;
					buff[2] = pInfo[0]; //addr
					buff[3] = pInfo[1];	//comp val

					sprintf_s(debugstr,"i2c read reg[%02x]",buff[2]);
					pthis->PrintInfo(debugstr);

					pthis->HID_Blinky.SetOutputReport_Interrupt(buff,REPORT_ID_OUT_CMD_LEN+1);

					status = pthis->HID_InterruptGetReport (reportbuffer);						//read
					if(status == HID_DEVICE_SUCCESS)
					{
						if((reportbuffer[0]==REPORT_ID_IN_IMAGE) &&				//
						   ((reportbuffer[1]&0x3f) == DATA_CMD_I2C_READ_REG) &&		
						   (reportbuffer[2] == 0))
						{
							addr = reportbuffer[3];
							val  = reportbuffer[4];
							retval = reportbuffer[5];
							pthis->m_RegValue.Format(_T("0x%x"),retval);

							sprintf_s(str,"[%02x][%02x] ok\n",val,retval);
							pthis->PrintInfo(str);
							
						}
						else
						{
							pthis->PrintInfo("some err4\n");
						}
					}

					::SendMessage(pthis->m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);
					g_event_reg_rdwr.SetEvent();
					break;

				}
			case MSG_SWITCH_WORK_STYLE:
				{
					pInfo = (char *)msg.wParam;
					buff[0] = REPORT_ID_OUT_CMD;
					buff[1] = DATA_CMD_SWITCH_WORK_STYLE;
					buff[2] = pInfo[0];

					//pthis->PrintInfo("switch work style cmd ");

					pthis->HID_Blinky.SetOutputReport_Interrupt(buff,REPORT_ID_OUT_CMD_LEN+1);
					
					cnt = 256;
					while(cnt--)
					{
						status = pthis->HID_InterruptGetReport (reportbuffer);	

						TRACE("status1 %x %x %x %x %x\n",status,reportbuffer[0],reportbuffer[1],reportbuffer[2] ,reportbuffer[3] ); 
						//get return report from 51.

						if(	(status == HID_DEVICE_SUCCESS )			&&							
							(reportbuffer[0]==REPORT_ID_IN_IMAGE)	&&	
							((reportbuffer[1]&0x3f) == DATA_CMD_SWITCH_WORK_STYLE) )
						{
							if(reportbuffer[3] == 0)
							{
								pthis->PrintInfo("ok\n");
								g_event_config_sensor_success.SetEvent();
							}
							else
								pthis->PrintInfo("some err5\n");

							break;
						}

						if((status == HID_DEVICE_TRANSFER_TIMEOUT)|| (cnt ==0))			//time out or get no report in 256 reports.
						{
							pthis->PrintInfo("time out\n");
							break;
						}
					}

					//i think usb has a buffer for save data i dont fetch. test as follows
					{
						if((reportbuffer[2]&0x3f)==0) //work style. //idle
						{
							cnt = 256;
							while(cnt--)
							{
								status = pthis->HID_InterruptGetReport (reportbuffer);
								if(status!=HID_DEVICE_SUCCESS)
									break;

							//TRACE("%x %x\n",status,pthis->HID_Blinky.GetInputReportBufferLength()); 
								TRACE("status2 %x %x %x %x %x\n",status,reportbuffer[0],reportbuffer[1],reportbuffer[2] ,reportbuffer[3] ); 
							}

						}
					}

					::PostMessage(pthis->m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);
					g_event_reg_rdwr.SetEvent();
	
					break;
				}
			case MSG_MAIN_WORK_EXIT:
				goto exit;


            }
        }
    };
exit:
	TRACE("main work thread exit\n");
    return 0;
}

unsigned int fdebug_cnt=0;
FILE*fdebug = NULL;//fopen("f:\\test.hex","wb");
unsigned int cur_data_offset = 0;
unsigned int isfirst = 1;
unsigned int frame_cnt =0;
void CTPA06CA_AdjusterDlg::HIDcallback (BYTE* reportbuffer)
{
	COM_DATA_TYPE data_type;
	unsigned char flag ;
	unsigned char sn_num = 0;
	unsigned char len = 0;
	int addr,val;
	int retval;
	char err;
	char str[64];
	// check report ID and determine
	switch(reportbuffer[0])
	{
		// new blink pattern has been requested, process request
		case(REPORT_ID_IN_IMAGE): //report id
		{
			flag = reportbuffer[1]; //data type

			data_type = (COM_DATA_TYPE)(flag&0x3f);

			if(data_type == DATA_LEFT_IMAGE || data_type==DATA_RIGHT_IMAGE)
			{
				sn_num  = reportbuffer[2]|(reportbuffer[3]&0xc0<<2);

				len		= reportbuffer[3]&0x3f;

				if(flag&0x80)	//start flag
				{
					cur_data_offset = 0;
					if(m_pYdata)
					{
						memcpy(m_pYdata+cur_data_offset,&reportbuffer[4],len);
						cur_data_offset += len;
					}

				}
				else if(flag&0x40) //end flag
				{
					if(m_pYdata)
					{
						memcpy(m_pYdata+cur_data_offset,&reportbuffer[4],len);
						cur_data_offset = 0;
					}

					::PostMessage(m_hWnd,WM_SHOWVIDEO,0,0);
				}
				else
				{
					if(m_pYdata && cur_data_offset<VIDEO_WIDTH*VIDEO_HEIGHT )
					{
						memcpy(m_pYdata+cur_data_offset,&reportbuffer[4],len);
						cur_data_offset += len;
					}

				}
				

				//TRACE("get image data:%d %d %d\n",data_type,sn_num,len);

				//g_event_reg_rdwr.SetEvent();

			}/*
			else if(data_type == DATA_CMD_WRITE_REG)
			{
				err = reportbuffer[2]; //err code
				if(!err)
				{
					addr = (reportbuffer[3])|(reportbuffer[4]<<8);
					val  = reportbuffer[5];

					//m_RegValue.Format(_T("0x%x"),val);
					PrintInfo("ok\n");
				}
				else
				{
					sprintf_s(str,"err!\n");
					PrintInfo(str);
				}

				::SendMessage(m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);

				g_event_reg_rdwr.SetEvent();
			}
			else if(data_type == DATA_CMD_READ_REG)
			{
				err = reportbuffer[2]; //err code
				if(!err)
				{
					addr = (reportbuffer[3])|(reportbuffer[4]<<8);
					val  = reportbuffer[5];
					retval = reportbuffer[6];

					m_RegValue.Format(_T("0x%x"),retval);

					sprintf_s(str,"[%02x][%02x] ok\n",val,retval);

					PrintInfo(str);
				}
				else
				{
					sprintf_s(str,"err!\n");
					PrintInfo(str);
				}

				::SendMessage(m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);

				g_event_reg_rdwr.SetEvent();
		
			}
			else if(data_type == DATA_CMD_I2C_WRITE_REG)
			{
				err = reportbuffer[2]; //err code
				if(!err)
				{
					addr = reportbuffer[3];
					val  = reportbuffer[4];

					//m_RegValue.Format(_T("0x%x"),val);
					PrintInfo("ok\n");
				}
				else
				{
					sprintf_s(str,"err!\n");
					PrintInfo(str);
				}

				::SendMessage(m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);

				g_event_reg_rdwr.SetEvent();
			}
			else if(data_type == DATA_CMD_I2C_READ_REG)
			{
				err = reportbuffer[2]; //err code
				if(!err)
				{
					addr = reportbuffer[3];
					val  = reportbuffer[4];
					retval = reportbuffer[5];
					m_RegValue.Format(_T("0x%x"),retval);

					sprintf_s(str,"[%02x][%02x] ok\n",val,retval);
					PrintInfo(str);
				}
				else
				{
					sprintf_s(str,"err!\n");
					PrintInfo(str);
				}

				::SendMessage(m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);

				g_event_reg_rdwr.SetEvent();
			}
			else if(data_type == DATA_CMD_SWITCH_WORK_STYLE)
			{
				err = reportbuffer[2]; //err code
				if(!err)
				{
					//addr = reportbuffer[3];
					//val  = reportbuffer[4];
					//retval = reportbuffer[5];
					//m_RegValue.Format(_T("0x%x"),retval);

					//sprintf_s(str,"[%x][%x] ok\n",val,retval);
					PrintInfo("ok\n");
					g_event_config_sensor_success.SetEvent();
				}
				else
				{
					sprintf_s(str,"err!\n");
					PrintInfo(str);
				}

				::SendMessage(m_hWnd,WM_MAIN_WINDOW_FLUSH_MSG,0,0);

				g_event_reg_rdwr.SetEvent();
			}
			*/
			else
			{
				TRACE("unknown date type return!:%d\n",data_type);
			}

			
		}
		break;
		default:
		break;
	}			

}
//------------------------------------------------------------------------------------
// run script thread
//------------------------------------------------------------------------------------
UINT RunScriptThread(LPVOID pParam)
{
	CTPA06CA_AdjusterDlg *dlg = (CTPA06CA_AdjusterDlg *)pParam;
	FILE *fScript = NULL;
	char line[512];
	DWORD Addr = 0;
	DWORD Reg = 0;
	DWORD RegComp = 0;
	DWORD ErrFlag = 0;
	DWORD Delay = 0;
	DWORD header = 0;
	char command[64];
	DWORD i = 0;
	DWORD wret;


	//const size_t strsize=(str.GetLength()+1)*2; // 宽字符的长度;
	//char * pstr= new char[strsize]; //分配空间;
	size_t sz=0;
	wcstombs_s(&sz,line,256,dlg->m_ScriptPath,_TRUNCATE);

	fScript = fopen(line,"r");
	if (NULL==fScript)
	{
		//dlg->m_StatusInfo = "Script Open Fail!";
		//dlg->UpdateData(FALSE);
		dlg->PrintInfo("Script Open Fail!");
		return 0;
	}

	while(!feof(fScript))
	{
		fscanf(fScript,"%d",&header);
		if(1!=header)
		{
			fgets(line,512,fScript);
			continue;
		}
		
		fscanf(fScript,"%s",command); //read the command
		if (!strcmp(command,"rd"))
		{
			fscanf(fScript,"%x",&Addr);
			fscanf(fScript,"%x",&RegComp);
			wret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
			if(wret==WAIT_OBJECT_0)
			{
				g_cmd_buff[0] = (unsigned char)Addr;
				g_cmd_buff[1] = (unsigned char)(Addr>>8);
				g_cmd_buff[2] = (unsigned char)RegComp;
				PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_READ_REG,(WPARAM)g_cmd_buff,NULL);

			}
			else if(wret == WAIT_TIMEOUT)
			{
				dlg->PrintInfo("\n last cmd no return when run script read\n");
			}
			else
			{
				dlg->PrintInfo("\n other err when run script write\n");
			}

		}
		else if (!strcmp(command,"wd"))
		{
			fscanf(fScript,"%x",&Addr);
			fscanf(fScript,"%x",&Reg);

			wret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
			if(wret==WAIT_OBJECT_0)
			{
				g_cmd_buff[0] = (unsigned char)Addr;
				g_cmd_buff[1] = (unsigned char)(Addr>>8);
				g_cmd_buff[2] = (unsigned char)(Reg);

				PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_WRITE_REG,(WPARAM)g_cmd_buff,NULL);

			}
			else if(wret == WAIT_TIMEOUT)
			{
				dlg->PrintInfo("\n last cmd no return when run script write\n");
			}
			else
			{
				dlg->PrintInfo("\n other err when run script write\n");
			}

		}
		else if (!strcmp(command,"#"))
		{
			fscanf(fScript,"%d",&Delay);
			Sleep(Delay/1000);

			sprintf_s(line,"delay %d clk\n",Delay);

			wret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
			if(wret == WAIT_OBJECT_0)
			{
				dlg->PrintInfo(line);
			}
			g_event_reg_rdwr.SetEvent();

		}
		else if (!strcmp(command,"i2cwd"))
		{
			fscanf(fScript,"%x",&Addr);
			fscanf(fScript,"%x",&Reg);
			wret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
			if(wret==WAIT_OBJECT_0)
			{
				g_cmd_buff[0] = (unsigned char)Addr;
				g_cmd_buff[1] = (unsigned char)Reg;

				PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_I2C_WRITE_REG,(WPARAM)g_cmd_buff,NULL);

			}
			else if(wret == WAIT_TIMEOUT)
			{
				dlg->PrintInfo("\n last cmd no return when run script i2c write\n");
			}
			else
			{
				dlg->PrintInfo("\n other err when run script i2c write\n");
			}

			//sprintf_s(line,"write sensor reg 0x%02x 0x%02x\n",Addr,Reg);
			//dlg->PrintInfo(line);
			//dlg->m_Debug.WriteRegister(0xC405,0x21);
			//dlg->m_Debug.WriteRegister(0xC408,(UCHAR)Addr);
			//dlg->m_Debug.WriteRegister(0xC409,(UCHAR)Reg);
			//dlg->m_Debug.WriteRegister(0xC406,0x14);
			//dlg->m_Debug.WriteRegister(0xC404,0x01);
		}
		else if (!strcmp(command,"i2crd"))
		{
			fscanf(fScript,"%x",&Addr);
			fscanf(fScript,"%x",&RegComp);
			wret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
			if(wret==WAIT_OBJECT_0)
			{
				g_cmd_buff[0] = (unsigned char)Addr;
				g_cmd_buff[1] = (unsigned char)RegComp;

				PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_I2C_READ_REG,(WPARAM)g_cmd_buff,NULL);

			}
			else if(wret == WAIT_TIMEOUT)
			{
				dlg->PrintInfo("\n last cmd no return when run script i2c read\n");
			}
			else
			{
				dlg->PrintInfo("\n other err when run script i2c read\n");
			}
			//sprintf_s(line,"read sensor reg:0x%02x=",Addr);
			//dlg->PrintInfo(line);
			//dlg->m_Debug.WriteRegister(0xC405,0x21);						//i2c slave address
			//dlg->m_Debug.WriteRegister(0xC408,(UCHAR)Addr);					//sensor reg address
			//dlg->m_Debug.WriteRegister(0xC406,0x12);						//write len 1byte
			//dlg->m_Debug.WriteRegister(0xC404,0x01);						//start write
			//dlg->m_Debug.WriteRegister(0xC406,0x13);						//read len 1byte
			//dlg->m_Debug.WriteRegister(0xC404,0x01);						//start read
			//dlg->m_Debug.ReadRegister(0xC40B,(UCHAR *)&Reg);
			//sprintf_s(line,"0x%02x\n",Reg);
			//dlg->PrintInfo(line);
		}
/*		else if (!strcmp(command,"i2cbwd"))
		{
			fscanf(fScript,"%x",&Addr);
			fscanf(fScript,"%x",&Reg);
			fscanf(fScript,"%x",&RegComp);
			sprintf_s(line,"burst write sensor reg 0x%02x: 0x%02x 0x%02x\n",Addr,Reg,RegComp);
			dlg->PrintInfo(line);
			dlg->m_Debug.WriteRegister(0xC405,0x21);
			dlg->m_Debug.WriteRegister(0xC408,(UCHAR)Addr);
			dlg->m_Debug.WriteRegister(0xC409,(UCHAR)Reg);
			dlg->m_Debug.WriteRegister(0xC40A,(UCHAR)RegComp);
			dlg->m_Debug.WriteRegister(0xC406,0x16);
			dlg->m_Debug.WriteRegister(0xC404,0x01);
		}*/
		header = fgetc(fScript);
		if(0x0a!=header && 0x0d!=header)
		fgets(line,512,fScript);
//		dlg->UpdateWindow();
	}

	if (ErrFlag)
	{
		dlg->PrintInfo("Fail!!!\n");
	}
	else
	{
		 wret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
		 if(wret == WAIT_OBJECT_0)
		 {
			dlg->PrintInfo("Success!!!\n");
		 }
		 g_event_reg_rdwr.SetEvent();
	}

	fclose(fScript);

	p_RunScriptThread = NULL;
	return 1;
}
BOOL CTPA06CA_AdjusterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// TODO: Add extra initialization here
	GetCurrentDirectory(256,m_IniFile.GetBuffer(256));
	m_IniFile.ReleaseBuffer();
	m_IniFile = m_IniFile + _T("\\TPA06Verify.ini");
	//m_StatusInfo = "Open Com first before debug";
	//m_ComStr = "COM4";
	//m_BaudRate = "1000000";
/*	GetPrivateProfileString("COM","Port","",m_ComStr.GetBuffer(MAX_PATH),MAX_PATH,m_IniFile);
	m_ComStr.ReleaseBuffer(MAX_PATH);
	GetPrivateProfileString("COM","BaudRate","",m_ComStr.GetBuffer(MAX_PATH),MAX_PATH,m_IniFile);
	m_BaudRate.ReleaseBuffer(MAX_PATH);
*/

	//GetPrivateProfileString("DefaultPath","FirmwareDir","",m_FwPath.GetBuffer(MAX_PATH),MAX_PATH,m_IniFile);
	//m_FwPath.ReleaseBuffer(MAX_PATH);
	GetPrivateProfileString(_T("DefaultPath"),_T("ScriptDir"),_T(""),m_ScriptPath.GetBuffer(MAX_PATH),MAX_PATH,m_IniFile);	m_ScriptPath.ReleaseBuffer(MAX_PATH);
	m_RegAddr = "0x";
	m_RegValue = "0x";


	if(fdebug == NULL)
	fdebug = fopen("f:\\test.hex","wb");
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	int i;
	//HANDLE hloc1,hloc2,hloc3;
	//RGBQUAD *argbq;

	hloc1 = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
	argbq = (RGBQUAD *) LocalLock(hloc1);

	for(i=0;i<256;i++)
	{
		argbq[i].rgbBlue=i;
		argbq[i].rgbGreen=i;
		argbq[i].rgbRed=i;
		argbq[i].rgbReserved=0;
	}

	//GetDlgItem(IDC_STATIC_PREVIEW1)->MoveWindow(15,370,800,80,1);
	//GetDlgItem(IDC_STATIC_PREVIEW2)->MoveWindow(15,460,800,80,1);
	m_pCDCpreview1 = GetDlgItem(IDC_STATIC_PREVIEW1)->GetDC();
	m_pCDCpreview2 = GetDlgItem(IDC_STATIC_PREVIEW2)->GetDC();
	m_pYdata = (unsigned char*)calloc(VIDEO_WIDTH*VIDEO_HEIGHT,1);
	if (NULL==m_pYdata)
		AfxMessageBox(_T("mem alloc error!"));

    hloc2 = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
	m_BmpInfo = (LPBITMAPINFO) GlobalLock(hloc2);
	m_BmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_BmpInfo->bmiHeader.biPlanes = 1;
	m_BmpInfo->bmiHeader.biBitCount = 8;
	m_BmpInfo->bmiHeader.biCompression = BI_RGB;
	m_BmpInfo->bmiHeader.biWidth = VIDEO_WIDTH;
	m_BmpInfo->bmiHeader.biHeight = -VIDEO_HEIGHT;
    
	hloc3 = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
	m_BmpLine = (LPBITMAPINFO) GlobalLock(hloc3);
	m_BmpLine->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_BmpLine->bmiHeader.biPlanes = 1;
	m_BmpLine->bmiHeader.biBitCount = 8;
	m_BmpLine->bmiHeader.biCompression = BI_RGB;
	m_BmpLine->bmiHeader.biWidth = VIDEO_WIDTH;
	m_BmpLine->bmiHeader.biHeight = -1;
	


	m_pCDCpreview1->SetStretchBltMode(STRETCH_DELETESCANS);
	m_pCDCpreview2->SetStretchBltMode(STRETCH_DELETESCANS);
	memcpy(m_BmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);
	memcpy(m_BmpLine->bmiColors, argbq, sizeof(RGBQUAD) * 256);


	//m_pYdata = (unsigned char*)calloc(VIDEO_WIDTH*VIDEO_HEIGHT,1);

	//-------------------------------------------------------------------------
	p_thread_main_work = AfxBeginThread(main_work_thread,this,0,0,0,0);

	OpenUsb();
	RegisterNotification();


	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTPA06CA_AdjusterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTPA06CA_AdjusterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTPA06CA_AdjusterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


DWORD CTPA06CA_AdjusterDlg::HID_InterruptGetReport (BYTE* reportbuffer)
{
	DWORD results;
	return HID_Blinky.GetInputReport_Interrupt(reportbuffer,
		HID_Blinky.GetInputReportBufferLength(),1,&results);
}
static DWORD WINAPI InterruptThreadProc(LPVOID lpParameter)
{
	CTPA06CA_AdjusterDlg* dlg;
	dlg = (CTPA06CA_AdjusterDlg*)lpParameter;
	BYTE reportbuffer[256];
	DWORD status;

	dlg->RXthreadmaycontinue = TRUE;   // Variable will be cleared whenever
									   // device is disconnected

	WaitForSingleObject(g_event_config_sensor_success.m_hObject,INFINITE);

	while (dlg->RXthreadmaycontinue == TRUE)
	{
		/*
		// Attempt to retrieve a report
		status = dlg->HID_InterruptGetReport (reportbuffer);
		// If a report has been recieved, call the callback routine
		// to process the data stored in reportbuffer
		if (status == HID_DEVICE_SUCCESS)
		{
			dlg->HIDcallback (reportbuffer);
			//dlg->UpdateData(FALSE);
		}
		*/
		Sleep(5000);

	}

	return 1;
}

void CTPA06CA_AdjusterDlg::OpenUsb()
{
		DWORD results;

		if(HID_Blinky.IsOpened())
		{
			HID_Blinky.Close();
		}

		// Check to see if any HID Blinky devices are attached to USB
		results = GetNumHidDevices(HID_Blinky_VID, HID_Blinky_PID);
		// Number of attached devices with matching PID and VID is stored
		// in results
		if (results != 0)
		{
			// Attempt to connect to attached device
			results = HID_Blinky.Open(results - 1, HID_Blinky_VID, HID_Blinky_PID);

			// If successfully attached, initialize system for operation
			if (results == HID_DEVICE_SUCCESS)
			{
				// Set Get Report and Set Report time-out values to 100 ms
				HID_Blinky.SetTimeouts (1000,1000);

				// Create thread that polls for INTERRUPT IN reports
				HID_RX_THREAD = CreateThread (NULL,NULL,InterruptThreadProc,this,0,0);

				GetDlgItem(IDC_BUTTON_TESTCMD)->SetWindowText(_T("Disconnect"));
				PrintInfo("Open USB OK\n");
				g_event_reg_rdwr.SetEvent();
				cur_data_offset = 0;

			}
		}
}
void CTPA06CA_AdjusterDlg::CloseUsb()
{
	if (HID_Blinky.IsOpened())
	{
		if (HID_Blinky.Close() == HID_DEVICE_SUCCESS)
		{
			// Terminate the receive reports thread
			RXthreadmaycontinue = FALSE;
			TerminateThread (HID_RX_THREAD, NULL);

			// Update "Device Status" text box
			GetDlgItem(IDC_BUTTON_TESTCMD)->SetWindowText(_T("Connect"));
			PrintInfo("Close USB OK\n");
			//UpdateWindow = TRUE;
			g_event_reg_rdwr.ResetEvent();
			cur_data_offset = 0;
		}
		else
		{
			// Do nothing
		}
	}

}
void CTPA06CA_AdjusterDlg::OnBnClickedButtonTestcmd()
{
	CString str;
	GetDlgItem(IDC_BUTTON_TESTCMD)->GetWindowText(str.GetBuffer(32),32);
	if(str==_T("Connect"))
	{
		OpenUsb();
	}
	else if(str==_T("Disconnect"))
	{
		CloseUsb();
	}
	str.ReleaseBuffer();
}







void CTPA06CA_AdjusterDlg::OnBnClickedButtonRegWrite()
{
	/*
	int a[4] = {24000000,6000000,2000000,500000};
	float val;
	for(int m =0;m<4;m++)
	{
		for(int i = 1;i<=256;i++)
		{
			val = (float)a[m]/i;
			if((val>870000) && (val<900000))
				TRACE3("val = %08f m=%d i =%d  \n",val,m,i);
		}
	}
	printf("");
	*/
	
	int  Addr = 0;
	int  Value = 0;
	DWORD ret;
	BOOL ret1;
	BOOL checked;

	//char*buff = new char[8];// addr = new

	if (!HID_Blinky.IsOpened())
	{
		PrintInfo("Please Open USB First!\n");
		return;
	}
	
	ret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
	if(ret == WAIT_OBJECT_0)
	{
		UpdateData(TRUE);

		StrToIntEx(m_RegAddr,STIF_SUPPORT_HEX,&Addr);
		StrToIntEx(m_RegValue,STIF_SUPPORT_HEX,&Value);
		
		checked =((CButton*)GetDlgItem(IDC_EDIT_SENSOR_RW))->GetCheck();
		if(checked)
		{
			g_cmd_buff[0] = (unsigned char)Addr;
			g_cmd_buff[1] = (unsigned char)Value;

			ret1 = PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_I2C_WRITE_REG,(WPARAM)g_cmd_buff,0);//post thread msg
			if(ret1 == FALSE)
				PrintInfo("send message: write_i2c_reg err\n");

		}
		else
		{
			g_cmd_buff[0] = (unsigned char)Addr;
			g_cmd_buff[1] = (unsigned char)(Addr>>8);
			g_cmd_buff[2] = (unsigned char)Value;

			ret1 = PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_WRITE_REG,(WPARAM)g_cmd_buff,0);//post thread msg
			if(ret1 == FALSE)
				PrintInfo("send message: write_reg err\n");

		}
	}
	else if(ret == WAIT_TIMEOUT)
	{
		PrintInfo("\nlast rw cmd no return!\n");
	}
	else
	{
		PrintInfo("unknown err when wait rw event!\n");
	}

}

void CTPA06CA_AdjusterDlg::OnBnClickedButtonRegRead()
{
	int  Addr = 0;
	DWORD ret;
	BOOL ret1;
	BOOL checked;

	//char*buff = new char[8];// addr = new

	if (!HID_Blinky.IsOpened())
	{
		PrintInfo("Please Open USB First!\n");
		return;
	}

	ret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
	if(ret == WAIT_OBJECT_0)
	{
		UpdateData(TRUE);

		StrToIntEx(m_RegAddr,STIF_SUPPORT_HEX,&Addr);
		
		checked =((CButton*)GetDlgItem(IDC_EDIT_SENSOR_RW))->GetCheck();
		if(checked)
		{
			g_cmd_buff[0] = (unsigned char)Addr;
			g_cmd_buff[1] = 0x00;

			ret1 = PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_I2C_READ_REG,(WPARAM)g_cmd_buff,0);//post thread msg
			if(ret1 == FALSE)
				PrintInfo("send message: read_i2c_reg err\n");

		}
		else
		{
			g_cmd_buff[0] = (unsigned char)Addr;
			g_cmd_buff[1] = (unsigned char)(Addr>>8);
			g_cmd_buff[2] = 0x00;

			ret1 = PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_READ_REG,(WPARAM)g_cmd_buff,0);//post thread msg
			if(ret1 == FALSE)
				PrintInfo("send message: read_i2c_reg err\n");
		}
	}
	else if(ret == WAIT_TIMEOUT)
	{
		PrintInfo("\nlast rw cmd no return!\n");
	}
	else
	{
		PrintInfo("unknown err when wait rw event!\n");
	}


}

void CTPA06CA_AdjusterDlg::PrintInfo(char*str)
{
	CString sbak;
	sbak = str;
	m_RichEditPrintf.SetSel(-1,-1);
	m_RichEditPrintf.ReplaceSel(sbak);
	m_RichEditPrintf.SendMessage(WM_VSCROLL,SB_BOTTOM,0);
	UpdateWindow();
}

void CTPA06CA_AdjusterDlg::OnBnClickedButtonOpenIniFile()
{
	CFileDialog FirmwareDir(TRUE,_T("ini"),_T("*.ini"),OFN_HIDEREADONLY,_T("Ini Files(*.ini)|*.ini||"));
	if(!(GetPrivateProfileString(_T("DefaultPath"),_T("ScriptDir"),_T(""),m_ScriptPath.GetBuffer(MAX_PATH),MAX_PATH,m_IniFile)))
		m_ScriptPath = "c:\\";
	m_ScriptPath.ReleaseBuffer();
	FirmwareDir.m_ofn.lpstrInitialDir = m_ScriptPath;

	if(FirmwareDir.DoModal() == IDOK)
	{
		m_ScriptPath = FirmwareDir.GetPathName();
		if(!WritePrivateProfileString(_T("DefaultPath"),_T("ScriptDir"),m_ScriptPath,m_IniFile))
			MessageBox(_T("Write INI file ERROR"),NULL,MB_OK);
	}

	UpdateData(FALSE);
}

void CTPA06CA_AdjusterDlg::OnBnClickedButtonRunScript()
{
//check if COM opened
	if (!HID_Blinky.IsOpened())
	{
		MessageBox(_T("Please Open USB First!"),_T("Warning"),0);
		return;
	}
	if(""==m_ScriptPath)
	{
		MessageBox(_T("Please Select Script!"),_T("Warning"),MB_OK);
		return;
	}
	else
	{
		if (NULL == p_RunScriptThread)
		{
			p_RunScriptThread = AfxBeginThread((AFX_THREADPROC)RunScriptThread,this,0,0,0,0);
		}
		else
		{
			PrintInfo("is running,waiting....\n");
		}
	}
}

void CTPA06CA_AdjusterDlg::OnBnClickedButtonClearDebugInfo()
{
	GetDlgItem(IDC_RICHEDIT_DEBUG_INFO)->SetWindowTextW(_T(""));
}

//-----------------------------------------------------------------------------
// NOTIFICATION ROUTINES
//-----------------------------------------------------------------------------
//
// These routines enable the dialog box to monitor and respond to USB
// devices being connected to and disconnected from the USB.

//-----------------------------------------------------------------------------
// RegisterNotification
//-----------------------------------------------------------------------------
//
// Routine registers the notification handle so that device
// connect/disconnect information  initiates a call
// to the OnDeviceChange routine.  This allows the system to
// respond to devices being connected or unexpectedly removed
// from the USB.
//
void CTPA06CA_AdjusterDlg::RegisterNotification()
{
	// Register device notification
	DEV_BROADCAST_DEVICEINTERFACE devIF = {0};
	GUID		hidGuid;

	GetHidGuid(&hidGuid);

	devIF.dbcc_size = sizeof(devIF);
	devIF.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	devIF.dbcc_classguid  = hidGuid;

	m_hNotifyDevNode = RegisterDeviceNotification(GetSafeHwnd(),&devIF, DEVICE_NOTIFY_WINDOW_HANDLE);
}

//-----------------------------------------------------------------------------
// UnregisterNotification
//-----------------------------------------------------------------------------
//
// Routine unregisters the notification handle so that device
// connect/disconnect information no longer initiates a call
// to the OnDeviceChange routine.
//
void CTPA06CA_AdjusterDlg::UnregisterNotification()
{
	// Unegister device notification
	if(NULL != m_hNotifyDevNode)
	{
		UnregisterDeviceNotification(m_hNotifyDevNode);
		m_hNotifyDevNode = INVALID_HANDLE_VALUE;
	}
}

//-----------------------------------------------------------------------------
// OnDeviceChange
//-----------------------------------------------------------------------------
//
// Routine is called whenever the dialog box receives notification that
// a device has either been attached to or removed from USB.  This fuction
// requires that the function RegisterNotification has been executed
// to operate properly.
//
BOOL CTPA06CA_AdjusterDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	//UCHAR con = 0;
	switch(nEventType)
	{
		// A device has been inserted and is now available.
		case DBT_DEVICEARRIVAL:
			// Check to see if newly connected device has VID and PID
			// matching HID_Blinky's VID and PID
			if (GetNumHidDevices(HID_Blinky_VID,HID_Blinky_PID))
			{
				// Update Device Status text
				//m_DeviceStatus = "Connected, Idle";
				//UpdateWindow = TRUE;
				OpenUsb();
				//PrintInfo("hid device is arrival\n");
			}
			break;

		// A device has been removed from USB.
		case DBT_DEVICEREMOVECOMPLETE:

			// Check that the HID_Blinky device was the removed device
			if (!GetNumHidDevices(HID_Blinky_VID,HID_Blinky_PID))
			{
				// Call routine that will terminate communication and
				// destroy a receive thread, if one has been created
				//OnBnClickedDisconnect ();
				//m_DeviceStatus = "Disconnected";
				//UpdateWindow = TRUE;
				CloseUsb();
				//PrintInfo("hid device remove\n");
				//con = 3;
			}
			break;

		default:
			break;
	}

	//TRACE1("con = %d \n",con);

	return TRUE;
}
/*
unsigned int preview_thread(void *param)
{
	CTPA06CA_AdjusterDlg*pthis = (CTPA06CA_AdjusterDlg*)param;
	DWORD ret;
	unsigned int cur_idx = 0;
	
	#define	READ_ONCE	60

	unsigned int tot_cnt = (VIDEO_WIDTH*VIDEO_HEIGHT)/READ_ONCE;
	unsigned int rem = (VIDEO_WIDTH*VIDEO_HEIGHT)%READ_ONCE;
	if(rem)	
	{
		tot_cnt++;
	}


	WaitForSingleObject(g_event_config_sensor_success.m_hObject,INFINITE);

	TRACE("config sensor success\n");
	while(1)
	{
		ret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
		if(ret == WAIT_OBJECT_0)
		{
			g_cmd_buff[0] = REPORT_ID_OUT_CMD;
			g_cmd_buff[1] = DATA_CMD_READ_IMAGE;
			g_cmd_buff[2] = (unsigned char) cur_idx;
			g_cmd_buff[3] = (unsigned char)(cur_idx>>8);

			//----------------------------------------------------
			g_cmd_buff[4] = READ_ONCE;
			g_cmd_buff[5] &= (~0xc0);

			if(cur_idx == 0)				//start frame
			{
				g_cmd_buff[5] |= 0x80;
			}
			else if(cur_idx+1 == tot_cnt)  //end frame
			{
				g_cmd_buff[5] |= 0x40;
				if(rem)
					g_cmd_buff[4] = rem;
			}
			//----------------------------------------------------
			cur_idx = (cur_idx+1)%tot_cnt;

			pthis->HID_Blinky.SetOutputReport_Interrupt(g_cmd_buff,REPORT_ID_OUT_CMD_LEN+1);
		}
	
		//Sleep(1000);
		//TRACE("...\n");

	}
	
	return 0;
}
*/
void CTPA06CA_AdjusterDlg::OnBnClickedButtonPreview()
{
	// TODO: 在此添加控件通知处理程序代码
	int  Addr = 0;
	int  Value = 0;
	DWORD ret;
	BOOL ret1;
	CString strTemp;

	if (!HID_Blinky.IsOpened())
	{
		PrintInfo("Please Open USB First!\n");
		return;
	}

	ret = WaitForSingleObject(g_event_reg_rdwr.m_hObject,TIMEOUT_REG_READ_WRITE);
	if(ret == WAIT_OBJECT_0)
	{
		
		m_work_style.GetWindowText(strTemp);

		if(strTemp==_T("idle"))				{	g_cmd_buff[0] = 0;					PrintInfo("switch to	idle ");		}
		else if(strTemp==_T("preview"))		{	g_cmd_buff[0] = WORK_STYLE_PREVIEW; PrintInfo("switch to	preview ");		}
		else if(strTemp==_T("calibration")) {	g_cmd_buff[0] = WORK_STYLE_CAL;		PrintInfo("switch to	calibration ");	}
		else if(strTemp==_T("nomal"))		{	g_cmd_buff[0] = WORK_STYLE_NOMAL;	PrintInfo("switch to	nomal ");		}
		else{	PrintInfo("select none\n"); g_event_reg_rdwr.SetEvent();	return;}

		//g_cmd_buff[0] = WORK_STYLE_PREVIEW;
		//g_cmd_buff[0] = 0;
		ret1 = PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_SWITCH_WORK_STYLE,(WPARAM)g_cmd_buff,0);//post thread msg
		if(ret1 == FALSE)
			PrintInfo("send message:config_sensor err\n");
	}
	else if(ret == WAIT_TIMEOUT)
	{
		PrintInfo("\nlast rw cmd no return switch work style!\n");
	}
	else
	{
		PrintInfo("unknown err when wait switch work style!\n");
	}

	//p_thread_preview = AfxBeginThread(preview_thread,this,0,0,0,0);

}

//------------------------------------------------------------------------------------------------
// video display message
//------------------------------------------------------------------------------------------------
LRESULT CTPA06CA_AdjusterDlg::OnShowVideo(WPARAM wParam,LPARAM lParam)
{
	
	unsigned char lrflag = (unsigned char)wParam;
//	FINGER_INFO *pfingerinfo = (FINGER_INFO*)lParam;
	int i = 0;

	m_BmpInfo->bmiHeader.biBitCount = 8;			//gray
//	HPEN hpen;
//	hpen = CreatePen(PS_SOLID, 1, RGB(200, 200, 0));

//	if (0x1==lrflag)
//	{
//		StretchDIBits(m_pCDCpreview1->m_hDC,0,0,800,80,
//		0,0,VIDEO_WIDTH,VIDEO_HEIGHT,
//		m_pYdata,
//		m_BmpInfo,
//		DIB_RGB_COLORS,
//		SRCCOPY);
//		
//		if(m_pYdata)	GetX(m_pYdata);
//
//#if 0
//		hpenOld = (HPEN)SelectObject(m_pCDCstatic->m_hDC,(HGDIOBJ)hpen);
//
//		for(i=0;i<pfingerinfo->pointnum;i++)
//		{
//			MoveToEx(m_pCDCstatic->m_hDC,pfingerinfo->pointpos[i].x/2 - pfingerinfo->pointpos[i].width/2,0,NULL);
//			LineTo(m_pCDCstatic->m_hDC,pfingerinfo->pointpos[i].x/2 + pfingerinfo->pointpos[i].width/2,0);
//			LineTo(m_pCDCstatic->m_hDC,pfingerinfo->pointpos[i].x/2 + pfingerinfo->pointpos[i].width/2,VIDEO_HEIGHT-6);
//			LineTo(m_pCDCstatic->m_hDC,pfingerinfo->pointpos[i].x/2 - pfingerinfo->pointpos[i].width/2,VIDEO_HEIGHT-6);
//			LineTo(m_pCDCstatic->m_hDC,pfingerinfo->pointpos[i].x/2 - pfingerinfo->pointpos[i].width/2,0);
////			Rectangle(m_pCDCstatic->m_hDC, (pfingerinfo->pointpos[i].x - pfingerinfo->pointpos[i].width/2), 10, (pfingerinfo->pointpos[i].x + pfingerinfo->pointpos[i].width/2), 30);
////			Rectangle(m_pCDCstatic->m_hDC, 40, 0, 60, 30);
//		}
//		SelectObject(m_pCDCstatic->m_hDC, hpenOld);
//
//
//#endif
//	}
//	else if (0x2==lrflag)
//	{
		StretchDIBits(m_pCDCpreview2->m_hDC,0,0,VIDEO_WIDTH,VIDEO_HEIGHT,
		0,0,VIDEO_WIDTH,VIDEO_HEIGHT,
		m_pYdata,
		m_BmpInfo,
		DIB_RGB_COLORS,
		SRCCOPY); 
#if 0
		hpenOld = (HPEN)SelectObject(m_pCDCstatic2->m_hDC, (HGDIOBJ)hpen);
		for(i=0;i<pfingerinfo->pointnum;i++)
		{
			MoveToEx(m_pCDCstatic2->m_hDC,pfingerinfo->pointpos[i].x/2 - pfingerinfo->pointpos[i].width/2,0,NULL);
			LineTo(m_pCDCstatic2->m_hDC,pfingerinfo->pointpos[i].x/2 + pfingerinfo->pointpos[i].width/2,0);
			LineTo(m_pCDCstatic2->m_hDC,pfingerinfo->pointpos[i].x/2 + pfingerinfo->pointpos[i].width/2,VIDEO_HEIGHT-6);
			LineTo(m_pCDCstatic2->m_hDC,pfingerinfo->pointpos[i].x/2 - pfingerinfo->pointpos[i].width/2,VIDEO_HEIGHT-6);
			LineTo(m_pCDCstatic2->m_hDC,pfingerinfo->pointpos[i].x/2 - pfingerinfo->pointpos[i].width/2,0);
//			Rectangle(m_pCDCstatic2->m_hDC, (pfingerinfo->pointpos[i].x - pfingerinfo->pointpos[i].width/2), 10, (pfingerinfo->pointpos[i].x + pfingerinfo->pointpos[i].width/2), 30);
//			Rectangle(m_pCDCstatic->m_hD C, 40, 0, 60, 30);
		}
		SelectObject(m_pCDCstatic2->m_hDC, hpenOld);
#endif
	//}
	//DeleteObject(hpen);
	
	return 0;
}
void CTPA06CA_AdjusterDlg::OnDestroy()
{
	DWORD ret;
	CDialog::OnDestroy();

	PostThreadMessage(p_thread_main_work->m_nThreadID,MSG_MAIN_WORK_EXIT,NULL,NULL);
	ret= WaitForSingleObject(p_thread_main_work->m_hThread,1000);
	if(ret != WAIT_OBJECT_0)
	{			
		TRACE("wait p_thread_main_work failed\n");
	}

	CloseUsb();

	if(m_pYdata)
	{
		free(m_pYdata);
		m_pYdata = NULL;
	}
	if(hloc1)
	{
		LocalFree(hloc1);
		hloc1 = NULL;
	}
	if(hloc2)
	{
		LocalFree(hloc2);
		hloc2 = NULL;
	}
	if(hloc3)
	{
		LocalFree(hloc3);
		hloc3 = NULL;
	}



	// TODO: 在此处添加消息处理程序代码
}
