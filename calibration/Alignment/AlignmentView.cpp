// AlignmentView.cpp : implementation of the CAlignmentView class
//

#include "stdafx.h"
#include "Alignment.h"

#include "AlignmentDoc.h"
#include "AlignmentView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlignmentView

IMPLEMENT_DYNCREATE(CAlignmentView, CView)

BEGIN_MESSAGE_MAP(CAlignmentView, CView)
	//{{AFX_MSG_MAP(CAlignmentView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlignmentView construction/destruction

CAlignmentView::CAlignmentView()
{
	// TODO: add construction code here

}

CAlignmentView::~CAlignmentView()
{
}

BOOL CAlignmentView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAlignmentView drawing

void CAlignmentView::OnDraw(CDC* pDC)
{
	CAlignmentDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAlignmentView printing

void CAlignmentView::OnFilePrintPreview() 
{
	BCGPPrintPreview (this);
}

BOOL CAlignmentView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAlignmentView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAlignmentView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAlignmentView diagnostics

#ifdef _DEBUG
void CAlignmentView::AssertValid() const
{
	CView::AssertValid();
}

void CAlignmentView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAlignmentDoc* CAlignmentView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAlignmentDoc)));
	return (CAlignmentDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAlignmentView message handlers

void CAlignmentView::OnContextMenu(CWnd*, CPoint point)
{
	theApp.ShowPopupMenu (IDR_CONTEXT_MENU, point, this);
}
