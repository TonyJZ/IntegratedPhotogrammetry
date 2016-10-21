// IPLRView.cpp : implementation of the CIPLRView class
//

#include "stdafx.h"
#include "IPLR.h"

#include "IPLRDoc.h"
#include "IPLRView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPLRView

IMPLEMENT_DYNCREATE(CIPLRView, CView)

BEGIN_MESSAGE_MAP(CIPLRView, CView)
	//{{AFX_MSG_MAP(CIPLRView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPLRView construction/destruction

CIPLRView::CIPLRView()
{
	// TODO: add construction code here

}

CIPLRView::~CIPLRView()
{
}

BOOL CIPLRView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CIPLRView drawing

void CIPLRView::OnDraw(CDC* pDC)
{
	CIPLRDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CIPLRView printing

BOOL CIPLRView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CIPLRView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CIPLRView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CIPLRView diagnostics

#ifdef _DEBUG
void CIPLRView::AssertValid() const
{
	CView::AssertValid();
}

void CIPLRView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIPLRDoc* CIPLRView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIPLRDoc)));
	return (CIPLRDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIPLRView message handlers
