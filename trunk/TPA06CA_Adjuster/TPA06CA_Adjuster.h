// TPA06CA_Adjuster.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTPA06CA_AdjusterApp:
// See TPA06CA_Adjuster.cpp for the implementation of this class
//

class CTPA06CA_AdjusterApp : public CWinApp
{
public:
	CTPA06CA_AdjusterApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTPA06CA_AdjusterApp theApp;