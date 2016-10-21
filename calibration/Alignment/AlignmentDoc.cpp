// AlignmentDoc.cpp : implementation of the CAlignmentDoc class
//

#include "stdafx.h"
#include "Alignment.h"

#include "AlignmentDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlignmentDoc

IMPLEMENT_DYNCREATE(CAlignmentDoc, CDocument)

BEGIN_MESSAGE_MAP(CAlignmentDoc, CDocument)
	//{{AFX_MSG_MAP(CAlignmentDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlignmentDoc construction/destruction

CAlignmentDoc::CAlignmentDoc()
{
	// TODO: add one-time construction code here

}

CAlignmentDoc::~CAlignmentDoc()
{
}

BOOL CAlignmentDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAlignmentDoc serialization

void CAlignmentDoc::Serialize(CArchive& ar)
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
// CAlignmentDoc diagnostics

#ifdef _DEBUG
void CAlignmentDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAlignmentDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAlignmentDoc commands
