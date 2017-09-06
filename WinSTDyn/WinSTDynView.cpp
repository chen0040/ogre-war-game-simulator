// WinSTDynView.cpp : implementation of the CWinSTDynView class
//

#include "stdafx.h"
#include "WinSTDyn.h"

#include "WinSTDynDoc.h"
#include "WinSTDynView.h"
#include "OSEnvironment/OSEnvironment.h"
#include "tinyxml/tinyxml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinSTDynView

IMPLEMENT_DYNCREATE(CWinSTDynView, CView)

BEGIN_MESSAGE_MAP(CWinSTDynView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_WORLD_CAELUM, &CWinSTDynView::OnWorldCaelum)
	ON_UPDATE_COMMAND_UI(ID_WORLD_CAELUM, &CWinSTDynView::OnUpdateWorldCaelum)
	ON_COMMAND(ID_WORLD_JOYSTICK, &CWinSTDynView::OnWorldJoystick)
	ON_COMMAND(ID_FILE_SAVE, &CWinSTDynView::OnFileSave)
	ON_COMMAND(ID_FILE_NEW, &CWinSTDynView::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinSTDynView::OnFileOpen)
END_MESSAGE_MAP()

// CWinSTDynView construction/destruction

CWinSTDynView::CWinSTDynView()
: mSimulatorState(SS_NULL)
, mDrawing(false)
{
	// TODO: add construction code here
	mLoadingImg.Load(CA2CT(OSEnvironment::getFullPath("..\\images\\loading.jpg").c_str()));
}

CWinSTDynView::~CWinSTDynView()
{
	mLoadingImg.Destroy();
}

BOOL CWinSTDynView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CWinSTDynView drawing

void CWinSTDynView::OnDraw(CDC* pDC)
{
	CWinSTDynDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	if(mSimulatorState==SS_MAINMENU)
	{
		CRect rect;
		GetClientRect(&rect);

		mLoadingImg.StretchBlt(pDC->m_hDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);
	}
	if(mSimulatorState==SS_SETUP)
	{
		CRect rect;
		GetClientRect(&rect);

		std::string action;
		float percentage=0;
		STDEngine* app=STDEngine::getSingletonPtr();
		bool initialized=app->incrementalSetup((unsigned long)this->m_hWnd, (unsigned long)AfxGetMainWnd()->GetSafeHwnd(), rect.Width(), rect.Height(), percentage, action);

		mLoadingImg.StretchBlt(pDC->m_hDC, 0, 0, rect.Width(), rect.Height(), SRCCOPY);

		const int barHeight=40;
		int left=rect.Width() / 4;
		int right=rect.Width() / 4 * 3;
		int top=rect.Height() / 2 + 10;
		int bottom=top + 40;
		CRect frame(left, top, right, bottom);
		CRect bar(left, top,  left + static_cast<int>((right-left) * percentage), bottom);
		bar.InflateRect(-4, -3);

		pDC->SetTextColor(COLORREF(0x00FF00)); //0xEFE98F
		pDC->SetBkMode(TRANSPARENT);

		CPen framePen;
		framePen.CreatePen(PS_SOLID, 3, COLORREF(0x00FF00)); //0xEFE98F
		CBrush barBrush;
		barBrush.CreateSolidBrush(COLORREF(0x008800)); //0xECBE81

		CPen* pOldPen=static_cast<CPen*>(pDC->SelectStockObject(NULL_PEN));
		CBrush* pOldBrush=static_cast<CBrush*>(pDC->SelectObject(&barBrush));
		pDC->Rectangle(&bar);

		pDC->SelectStockObject(NULL_BRUSH);
		pDC->SelectObject(&framePen);
		pDC->Rectangle(&frame);

		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);

		framePen.DeleteObject();
		barBrush.DeleteObject();

		pDC->DrawText(CA2CT(action.c_str()), -1, &frame, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		//pDC->TextOutW(left, top+20, CA2CT(action.c_str()), action.length());

		if(initialized)
		{
			mSimulatorState=SS_RUN;
			app->getWindow()->setActive(true);
			app->getWindow()->update();
		}
	}
	else if(mSimulatorState == SS_RUN)
	{
		if(STDEngine::getSingletonPtr()->render())
		{
			//Ogre::WindowEventUtilities::messagePump();
		}
		else
		{
			mSimulatorState = SS_EXIT;
		}
	}

	mDrawing=false;
}


// CWinSTDynView printing

BOOL CWinSTDynView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWinSTDynView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWinSTDynView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CWinSTDynView diagnostics

#ifdef _DEBUG
void CWinSTDynView::AssertValid() const
{
	CView::AssertValid();
}

void CWinSTDynView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinSTDynDoc* CWinSTDynView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinSTDynDoc)));
	return (CWinSTDynDoc*)m_pDocument;
}
#endif //_DEBUG


// CWinSTDynView message handlers

void CWinSTDynView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	Ogre::RenderWindow* ogreWin=STDEngine::getSingletonPtr()->getWindow();
	if (ogreWin != NULL)
    {
        CRect newRect;
        GetClientRect(&newRect);


        // notify "render window" instance
        STDEngine::getSingletonPtr()->injectWindowResized();

        // Adjust camera's aspect ratio, too
		Ogre::Camera* cam=STDEngine::getSingletonPtr()->getCamera();
        if (newRect.Height() != 0 && NULL != cam)
			cam->setAspectRatio((Ogre::Real)newRect.Width() / (Ogre::Real) newRect.Height());
    }
}

void CWinSTDynView::Terminate()
{
	ASSERT(AfxGetMainWnd() != NULL);
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

void CWinSTDynView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(mSimulatorState==SS_INIT)
	{
		mSimulatorState=SS_MAINMENU;
		Invalidate(FALSE);

		TiXmlDocument doc;
		if(!doc.LoadFile(OSEnvironment::getFullPath("..\\config\\MainMenu.xml").c_str()))
		{
			throw Exception(42, "Failed to load MainMenu.xml", "CWinSTDynView::OnInitialUpdate()");
		}

		TiXmlElement* xmlRoot=doc.RootElement();
		if(strcmp(xmlRoot->Attribute("visible"), "true")==0)
		{
			if(mMainMenuDlg.DoModal()==IDOK)
			{
				STDEngine* engine=STDEngine::getSingletonPtr();
				if(mMainMenuDlg.getCaelumType().compare("dynamic")==0)
				{
					engine->enableCaelumSystem(true);
					engine->setCaelumSystemCloudSpeed(Ogre::Vector2(mMainMenuDlg.getCloudSpeedX(), mMainMenuDlg.getCloudSpeedY()));
					engine->setCaelumSystemTimeScale(mMainMenuDlg.getTimeScale());
					const std::vector<int>& caelumSystemDateTime=mMainMenuDlg.getCaelumDateTime();
					engine->setCaelumSystemDateTime(caelumSystemDateTime[0], 
						caelumSystemDateTime[1], 
						caelumSystemDateTime[2], 
						caelumSystemDateTime[3],
						caelumSystemDateTime[4],
						caelumSystemDateTime[5]);
				}
				else
				{
					engine->enableCaelumSystem(false);
				}
				mSimulatorState=SS_SETUP;
			}
			else
			{
				mSimulatorState=SS_EXIT;
			}
		}
		else
		{
			mSimulatorState=SS_SETUP;
		}
	}
	else if(mSimulatorState == SS_EXIT)
	{
		Terminate();
		return;
	}
	else if(mSimulatorState==SS_SETUP || mSimulatorState==SS_RUN)
	{
		if(!mDrawing)
		{
			mDrawing=true;
			Invalidate(FALSE);
		}
	}

	CView::OnTimer(nIDEvent);
}

void CWinSTDynView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	if(mSimulatorState != SS_NULL)
	{
		return;
	}

	mSimulatorState=SS_INIT;
	SetTimer(1, 30, NULL);
}

BOOL CWinSTDynView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CView::OnEraseBkgnd(pDC);

	return TRUE;
}

void CWinSTDynView::OnWorldCaelum()
{
	// TODO: Add your command handler code here
	WorldInfoManager* wim=WorldInfoManager::getSingletonPtr();
	if(wim && wim->isCaelumSystemAvailable())
	{
		/*
		if(wim->isCaelumSystemEnabled())
		{
			wim->enableCaelumSystem(false);
		}
		else
		{
			wim->enableCaelumSystem(true);
		}
		*/

		STDEngine* engine=STDEngine::getSingletonPtr();

		int year, month, day, hour, minute, second;
		wim->getCaelumSystemDateTime(year, month, day, hour, minute, second);
		mMainMenuDlg.setCaelumDateTime(year, month, day, hour, minute, second);

		if(mMainMenuDlg.DoModal()==IDOK)
		{
			if(mMainMenuDlg.getCaelumType().compare("dynamic")==0)
			{
				wim->setCaelumSystemCloudSpeed(Ogre::Vector2(mMainMenuDlg.getCloudSpeedX(), mMainMenuDlg.getCloudSpeedY()));
				wim->setCaelumSystemTimeScale(mMainMenuDlg.getTimeScale());
				const std::vector<int>& caelumSystemDateTime=mMainMenuDlg.getCaelumDateTime();
				wim->setCaelumSystemDateTime(caelumSystemDateTime[0], 
					caelumSystemDateTime[1], 
					caelumSystemDateTime[2], 
					caelumSystemDateTime[3],
					caelumSystemDateTime[4],
					caelumSystemDateTime[5]);
				wim->enableCaelumSystem(true);
			}
			else
			{
				wim->enableCaelumSystem(false);
			}
		}
	}
	
}

void CWinSTDynView::OnUpdateWorldCaelum(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	WorldInfoManager* wim=WorldInfoManager::getSingletonPtr();
	if(wim)
	{
		if(wim->isCaelumSystemAvailable())
		{
			pCmdUI->SetCheck(wim->isCaelumSystemEnabled() ? 1 : 0);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CWinSTDynView::OnWorldJoystick()
{
	// TODO: Add your command handler code here

	if(mJoystickDlg.DoModal()==IDOK)
	{
		STDEngine* engine=STDEngine::getSingletonPtr();
		engine->enableJoystick(mJoystickDlg.isJoystickEnabled());
		engine->setJoystickAxisSensitivity(mJoystickDlg.getJoystickAxesSensitivity());
	}
}

void CWinSTDynView::OnFileSave()
{
	// TODO: Add your command handler code here
	STDEngine* engine=STDEngine::getSingletonPtr();
	engine->save();
}

void CWinSTDynView::OnFileNew()
{
	// TODO: Add your command handler code here
	STDEngine* engine=STDEngine::getSingletonPtr();
	engine->createNew();
}

void CWinSTDynView::OnFileOpen()
{
	// TODO: Add your command handler code here
	STDEngine* engine=STDEngine::getSingletonPtr();
	engine->open();
}
