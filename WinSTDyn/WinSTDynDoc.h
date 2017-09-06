// WinSTDynDoc.h : interface of the CWinSTDynDoc class
//


#pragma once


class CWinSTDynDoc : public CDocument
{
protected: // create from serialization only
	CWinSTDynDoc();
	DECLARE_DYNCREATE(CWinSTDynDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CWinSTDynDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


