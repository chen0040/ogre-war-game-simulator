#pragma once
#include "afxwin.h"


// CJoystickDlg dialog

class CJoystickDlg : public CDialog
{
	DECLARE_DYNAMIC(CJoystickDlg)

public:
	CJoystickDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CJoystickDlg();

public:
	bool isJoystickEnabled() const { return mJoystickEnabled; }
	float getJoystickAxesSensitivity() const { return mJoystickAxesSensitivity; }

// Dialog Data
	enum { IDD = IDD_DIALOG_JOYSTICK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit mEditJoystickAxesSensitivity;
	CButton mChkEnableJoysticks;
private:
	bool mJoystickEnabled;
	float mJoystickAxesSensitivity;
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
