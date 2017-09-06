// JoystickDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WinSTDyn.h"
#include "JoystickDlg.h"
#include "STDEngine/STDEngine.h"

// CJoystickDlg dialog

IMPLEMENT_DYNAMIC(CJoystickDlg, CDialog)

CJoystickDlg::CJoystickDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJoystickDlg::IDD, pParent)
	, mJoystickAxesSensitivity(0.5f)
	, mJoystickEnabled(true)
{

}

CJoystickDlg::~CJoystickDlg()
{
}

void CJoystickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_JOYSTICK_AXES_SENSITIVITY, mEditJoystickAxesSensitivity);
	DDX_Control(pDX, IDC_CHECK_ENABLE_JOYSTICK, mChkEnableJoysticks);
}


BEGIN_MESSAGE_MAP(CJoystickDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CJoystickDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CJoystickDlg message handlers

void CJoystickDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if(mChkEnableJoysticks.GetCheck() == BST_CHECKED)
	{
		mJoystickEnabled=true;
	}
	else
	{
		mJoystickEnabled=false;
	}

	CString sensitivity;
	mEditJoystickAxesSensitivity.GetWindowTextW(sensitivity);
	mJoystickAxesSensitivity=atof(CT2CA(sensitivity.GetString())); 
	OnOK();
}

BOOL CJoystickDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	mJoystickEnabled=STDEngine::getSingletonPtr()->isJoystickEnabled();
	mJoystickAxesSensitivity=STDEngine::getSingletonPtr()->getJoystickAxisSensitivity();

	if(mJoystickEnabled)
	{
		mChkEnableJoysticks.SetCheck(BST_CHECKED);
	}
	else
	{
		mChkEnableJoysticks.SetCheck(BST_UNCHECKED);
	}

	CString sensitivity;
	sensitivity.Format(_T("%f"), mJoystickAxesSensitivity);
	mEditJoystickAxesSensitivity.SetWindowTextW(sensitivity);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
