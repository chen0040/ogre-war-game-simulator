// WinSTDynDoc.cpp : implementation of the CWinSTDynDoc class
//

#include "stdafx.h"
#include "WinSTDyn.h"

#include "WinSTDynDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinSTDynDoc

IMPLEMENT_DYNCREATE(CWinSTDynDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinSTDynDoc, CDocument)
END_MESSAGE_MAP()


// CWinSTDynDoc construction/destruction

CWinSTDynDoc::CWinSTDynDoc()
{
	// TODO: add one-time construction code here

}

CWinSTDynDoc::~CWinSTDynDoc()
{
}

BOOL CWinSTDynDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CWinSTDynDoc serialization

void CWinSTDynDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CWinSTDynDoc diagnostics

#ifdef _DEBUG
void CWinSTDynDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWinSTDynDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CWinSTDynDoc commands
