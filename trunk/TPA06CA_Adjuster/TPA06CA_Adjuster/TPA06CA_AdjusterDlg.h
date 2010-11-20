// TPA06CA_AdjusterDlg.h : header file
//

#pragma once

#include "HIDDevice.h"
#include "afxcmn.h"
#include "afxwin.h"


#define REPORT_ID_OUT_CMD     0x03
#define REPORT_ID_IN_IMAGE    0x02
#define REPORT_ID_IN_MTOUCH   0x01

// ----------------------------------------------------------------------------
// Report Sizes (in bytes)
// ----------------------------------------------------------------------------
#define REPORT_ID_OUT_CMD_LEN    0x08
#define REPORT_ID_IN_IMAGE_LEN   0x3f
#define REPORT_ID_IN_MTOUCH_LEN  0x02


#define VIDEO_WIDTH		640
#define VIDEO_HEIGHT	16
typedef enum
{
	DATA_LEFT_IMAGE = 0,
	DATA_RIGHT_IMAGE = 1,
	DATA_CMD_WRITE_REG =2,
	DATA_CMD_READ_REG=3,
	DATA_CMD_I2C_WRITE_REG=4,
	DATA_CMD_I2C_READ_REG=5,
	DATA_CMD_SWITCH_WORK_STYLE = 10,
	DATA_TYPE_COUNT
}COM_DATA_TYPE;

// CTPA06CA_AdjusterDlg dialog
class CTPA06CA_AdjusterDlg : public CDialog
{
// Construction
public:
	CTPA06CA_AdjusterDlg(CWnd* pParent = NULL);	// standard constructor

	DWORD HID_InterruptGetReport (BYTE*);
	void HIDcallback (BYTE* reportbuffer);
	CHIDDevice HID_Blinky;
	HANDLE m_hNotifyDevNode;
	HANDLE HID_RX_THREAD;
	//bool RXthreadmaycontinue;

	void PrintInfo(char* str);
	void OpenUsb();
	void CloseUsb();

	CString m_IniFile;

	LPBITMAPINFO m_BmpInfo;
	LPBITMAPINFO m_BmpLine;
	CDC *m_pCDCpreview1;
	CDC *m_pCDCpreview2;

	HANDLE hloc1,hloc2,hloc3;
	RGBQUAD *argbq;

// Dialog Data
	enum { IDD = IDD_TPA06CA_ADJUSTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	void RegisterNotification (void);
	void UnregisterNotification(void);
	BOOL OnDeviceChange(UINT nEventType, DWORD dwData);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnShowVideo(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedButtonTestcmd();
	afx_msg void OnBnClickedButtonRegWrite();
	CString m_RegAddr;
	CString m_RegValue;
	afx_msg void OnBnClickedButtonRegRead();
	CRichEditCtrl m_RichEditPrintf;
	CString m_ScriptPath;
	afx_msg void OnBnClickedButtonOpenIniFile();
	afx_msg void OnBnClickedButtonRunScript();
	afx_msg LRESULT OnMainWindowFlush(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedButtonClearDebugInfo();
	afx_msg void OnBnClickedButtonPreview();
	afx_msg void OnDestroy();
	CComboBox m_work_style;
};
