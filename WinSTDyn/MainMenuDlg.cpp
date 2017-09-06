// MainMenuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WinSTDyn.h"
#include "MainMenuDlg.h"
#include "OSEnvironment/OSEnvironment.h"

// CMainMenuDlg dialog

IMPLEMENT_DYNAMIC(CMainMenuDlg, CDialog)

CMainMenuDlg::CMainMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainMenuDlg::IDD, pParent)
	, mInitialized(false)
{

}

CMainMenuDlg::~CMainMenuDlg()
{
}

void CMainMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAELUMTYPE, mCboCaelumType);
	DDX_Control(pDX, IDC_EDIT_TIMESCALE, mEditTimeScale);
	DDX_Control(pDX, IDC_EDIT_CLOUDSPEEDX, mEditCloudSpeedX);
	DDX_Control(pDX, IDC_EDIT_CLOUDSPEEDY, mEditCloudSpeedY);
	DDX_Control(pDX, IDC_EDIT_INSTRUCTION, mEditInstruction);
	DDX_Control(pDX, IDC_DATETIMEPICKER_DATE, mDatePicker);
	DDX_Control(pDX, IDC_DATETIMEPICKER_TIME, mTimePicker);
}


BEGIN_MESSAGE_MAP(CMainMenuDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CMainMenuDlg::OnBnClickedOk)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_DATE, &CMainMenuDlg::OnDtnDatetimechangeDatetimepickerDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_TIME, &CMainMenuDlg::OnDtnDatetimechangeDatetimepickerTime)
END_MESSAGE_MAP()


// CMainMenuDlg message handlers

BOOL CMainMenuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	/*
	if(mInitialized)
	{
		return TRUE;
	}
	*/

	if(!mDoc.LoadFile(OSEnvironment::getFullPath("..\\config\\MainMenu.xml").c_str()))
	{
		throw Exception(42, "Failed to Load MainMenu.xml", "CMainMenuDlg::OnInitDialog()");
	}

	TiXmlElement* xmlRoot=mDoc.RootElement();

	int index=0;
	for(TiXmlElement* xmlLevel1=xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1 = xmlLevel1->NextSiblingElement())
	{
		if(strcmp(xmlLevel1->Value(), "caelum")==0)
		{
			mCaelumType=xmlLevel1->Attribute("id");
			for(TiXmlElement* xmlLevel2=xmlLevel1->FirstChildElement(); xmlLevel2 != NULL; xmlLevel2 = xmlLevel2->NextSiblingElement())
			{
				if(strcmp(xmlLevel2->Value(), "option")==0)
				{
					mCboCaelumType.AddString(CA2CT(xmlLevel2->Attribute("id")));
					if(strcmp(xmlLevel2->Attribute("id"), "dynamic")==0)
					{
						for(TiXmlElement* xmlLevel3=xmlLevel2->FirstChildElement(); xmlLevel3 != NULL; xmlLevel3=xmlLevel3->NextSiblingElement())
						{
							if(strcmp(xmlLevel3->Value(), "time")==0)
							{
								xmlLevel3->QueryFloatAttribute("scale", &mTimeScale);
							}
							else if(strcmp(xmlLevel3->Value(), "cloud")==0)
							{
								xmlLevel3->QueryFloatAttribute("speedX", &mCloudSpeedX);
								xmlLevel3->QueryFloatAttribute("speedY", &mCloudSpeedY);
							}
						}
					}
					if(mCaelumType.compare(xmlLevel2->Attribute("id"))==0)
					{
						mCboCaelumType.SetCurSel(index);
					}
					index++;
				}
			}
		}
	}

	CString timeScale;
	timeScale.Format(_T("%f"), mTimeScale);

	CString cloudSpeedX;
	CString cloudSpeedY;
	cloudSpeedX.Format(_T("%f"), mCloudSpeedX);
	cloudSpeedY.Format(_T("%f"), mCloudSpeedY);

	mEditTimeScale.SetWindowTextW(timeScale);
	mEditCloudSpeedX.SetWindowTextW(cloudSpeedX);
	mEditCloudSpeedY.SetWindowTextW(cloudSpeedY);

	CString instruction=_T("Default value for Time Acceleration is 512 (i.e. Time will be accelerated by 512 times) and ");
	instruction += _T("Cloud_speedX=0.000025 Cloud_speedY=-0.000045, When Time Acceleration is set to 1 ");
	instruction += _T("(i.e. Time will be the same as normal time scale), recommended Cloud_SpeedX=0.01, ");
	instruction += _T("Cloud_SpeedY=-0.01) \n\rLatitude: The observer's latitude. North is positive, south is negative. ");
	instruction += _T("\nLogitude: The observer's longitude. East is positive, west is negative.");
	mEditInstruction.SetWindowTextW(instruction);
	
	CTime cealumSystemTime;
	CTime cealumSystemDate;

	mDatePicker.GetTime(cealumSystemTime);
	mTimePicker.GetTime(cealumSystemDate);

	mCaelumSystemDateTime.push_back(cealumSystemDate.GetYear());
	mCaelumSystemDateTime.push_back(cealumSystemDate.GetMonth());
	mCaelumSystemDateTime.push_back(cealumSystemDate.GetDay());

	mCaelumSystemDateTime.push_back(cealumSystemTime.GetHour());
	mCaelumSystemDateTime.push_back(cealumSystemTime.GetMinute());
	mCaelumSystemDateTime.push_back(cealumSystemTime.GetSecond());

	mInitialized=true;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMainMenuDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	CString type;
	mCboCaelumType.GetLBText(mCboCaelumType.GetCurSel(), type);
	mCaelumType=CT2CA(type.GetString());

	CString timeScale;
	CString cloudSpeedX;
	CString cloudSpeedY;
	mEditTimeScale.GetWindowTextW(timeScale);
	mEditCloudSpeedX.GetWindowTextW(cloudSpeedX);
	mEditCloudSpeedY.GetWindowTextW(cloudSpeedY);

	mTimeScale=atof(CT2CA(timeScale.GetString()));
	mCloudSpeedX=atof(CT2CA(cloudSpeedX.GetString()));
	mCloudSpeedY=atof(CT2CA(cloudSpeedY.GetString()));
	
	TiXmlElement* xmlRoot=mDoc.RootElement();

	int index=0;
	for(TiXmlElement* xmlLevel1=xmlRoot->FirstChildElement(); xmlLevel1 != NULL; xmlLevel1 = xmlLevel1->NextSiblingElement())
	{
		if(strcmp(xmlLevel1->Value(), "caelum")==0)
		{
			xmlLevel1->SetAttribute("id", mCaelumType.c_str());
			for(TiXmlElement* xmlLevel2=xmlLevel1->FirstChildElement(); xmlLevel2 != NULL; xmlLevel2 = xmlLevel2->NextSiblingElement())
			{
				if(strcmp(xmlLevel2->Value(), "option")==0 && strcmp(xmlLevel2->Attribute("id"), "dynamic")==0)
				{
					for(TiXmlElement* xmlLevel3=xmlLevel2->FirstChildElement(); xmlLevel3 != NULL; xmlLevel3=xmlLevel3->NextSiblingElement())
					{
						if(strcmp(xmlLevel3->Value(), "time")==0)
						{
							xmlLevel3->SetDoubleAttribute("scale", mTimeScale);
						}
						else if(strcmp(xmlLevel3->Value(), "cloud")==0)
						{
							xmlLevel3->SetDoubleAttribute("speedX", mCloudSpeedX);
							xmlLevel3->SetDoubleAttribute("speedY", mCloudSpeedY);
						}
					}
				}
			}
		}
	}

	mDoc.SaveFile(OSEnvironment::getFullPath("..\\config\\MainMenu.xml").c_str());

	OnOK();
}

void CMainMenuDlg::OnDtnDatetimechangeDatetimepickerDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	mCaelumSystemDateTime[0]=pDTChange->st.wYear;
	mCaelumSystemDateTime[1]=pDTChange->st.wMonth;
	mCaelumSystemDateTime[2]=pDTChange->st.wDay;

	*pResult = 0;
}

void CMainMenuDlg::OnDtnDatetimechangeDatetimepickerTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	mCaelumSystemDateTime[3]=pDTChange->st.wHour;
	mCaelumSystemDateTime[4]=pDTChange->st.wMinute;
	mCaelumSystemDateTime[5]=pDTChange->st.wSecond;

	*pResult = 0;
}

void CMainMenuDlg::setCaelumDateTime(int year, int month, int day, int hour, int minute, int second)
{
	mCaelumSystemDateTime.clear();
	mCaelumSystemDateTime.push_back(year);
	mCaelumSystemDateTime.push_back(month);
	mCaelumSystemDateTime.push_back(day);
	mCaelumSystemDateTime.push_back(hour);
	mCaelumSystemDateTime.push_back(minute);
	mCaelumSystemDateTime.push_back(second);

	this->mTimePicker.SetTime(&CTime(year, month, day, hour, minute, second));
	this->mDatePicker.SetTime(&CTime(year, month, day, hour, minute, second));
}
