// WinSTDynView.h : interface of the CWinSTDynView class
//


#pragma once

enum SimulatorState
{
	SS_NULL,
	SS_INIT,
	SS_MAINMENU,
	SS_SETUP,
	SS_RUN,
	SS_EXIT
};


class CWinSTDynView : public CView
{
protected: // create from serialization only
	CWinSTDynView();
	DECLARE_DYNCREATE(CWinSTDynView)

// Attributes
public:
	CWinSTDynDoc* GetDocument() const;
private:
	SimulatorState mSimulatorState;
	CImage mLoadingImg;
	bool mDrawing;
	STDEngine engine;
	CMainMenuDlg mMainMenuDlg;
	CJoystickDlg mJoystickDlg;

// Operations
public:
private:
	void Terminate();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CWinSTDynView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWorldCaelum();
	afx_msg void OnUpdateWorldCaelum(CCmdUI *pCmdUI);
	afx_msg void OnWorldJoystick();
	afx_msg void OnFileSave();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
};

#ifndef _DEBUG  // debug version in WinSTDynView.cpp
inline CWinSTDynDoc* CWinSTDynView::GetDocument() const
   { return reinterpret_cast<CWinSTDynDoc*>(m_pDocument); }
#endif

