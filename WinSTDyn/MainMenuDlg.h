#pragma once


// CMainMenuDlg dialog
#include <string>
#include <vector>
#include "tinyxml/tinyxml.h"
#include "afxwin.h"
#include "afxdtctl.h"

class CMainMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainMenuDlg)

public:
	CMainMenuDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainMenuDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MAINMENU };

private:
	bool mInitialized;
	std::string mCaelumType;
	float mTimeScale;
	float mCloudSpeedX;
	float mCloudSpeedY;
	TiXmlDocument mDoc;
	std::vector<int> mCaelumSystemDateTime;

public:
	std::string getCaelumType() const { return mCaelumType; }
	float getTimeScale() const { return mTimeScale; }
	float getCloudSpeedX() const { return mCloudSpeedX; }
	float getCloudSpeedY() const { return mCloudSpeedY; }
	const std::vector<int>& getCaelumDateTime() const { return mCaelumSystemDateTime; }
	void setCaelumDateTime(int year, int month, int day, int hour, int minute, int second);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox mCboCaelumType;
	afx_msg void OnBnClickedOk();
	CEdit mEditTimeScale;
	CEdit mEditCloudSpeedX;
	CEdit mEditCloudSpeedY;
	CEdit mEditInstruction;
	CDateTimeCtrl mDatePicker;
	CDateTimeCtrl mTimePicker;
	afx_msg void OnDtnDatetimechangeDatetimepickerDate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepickerTime(NMHDR *pNMHDR, LRESULT *pResult);
};
