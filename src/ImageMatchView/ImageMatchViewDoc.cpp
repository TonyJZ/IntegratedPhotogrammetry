// ImageMatchViewDoc.cpp : implementation of the CImageMatchViewDoc class
//

#include "stdafx.h"
#include "ImageMatchView.h"

#include "ImageMatchViewDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewDoc

IMPLEMENT_DYNCREATE(CImageMatchViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageMatchViewDoc, CDocument)
	//{{AFX_MSG_MAP(CImageMatchViewDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewDoc construction/destruction

CImageMatchViewDoc::CImageMatchViewDoc()
{
	// TODO: add one-time construction code here

}

CImageMatchViewDoc::~CImageMatchViewDoc()
{
}

BOOL CImageMatchViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewDoc serialization

void CImageMatchViewDoc::Serialize(CArchive& ar)
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

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewDoc diagnostics

#ifdef _DEBUG
void CImageMatchViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageMatchViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewDoc commands
