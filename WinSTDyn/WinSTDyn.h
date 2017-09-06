// WinSTDyn.h : main header file for the WinSTDyn application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "MainMenuDlg.h"
#include "JoystickDlg.h"
#include "STDEngine/STDEngine.h"


// CWinSTDynApp:
// See WinSTDyn.cpp for the implementation of this class
//

class CWinSTDynApp : public CWinApp
{
public:
	CWinSTDynApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWinSTDynApp theApp;