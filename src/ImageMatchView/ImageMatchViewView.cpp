// ImageMatchViewView.cpp : implementation of the CImageMatchViewView class
//

#include "stdafx.h"
#include "ImageMatchView.h"

#include "ImageMatchViewDoc.h"
#include "ImageMatchViewView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewView

IMPLEMENT_DYNCREATE(CImageMatchViewView, CView)

BEGIN_MESSAGE_MAP(CImageMatchViewView, CView)
	//{{AFX_MSG_MAP(CImageMatchViewView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewView construction/destruction

CImageMatchViewView::CImageMatchViewView()
{
	// TODO: add construction code here

}

CImageMatchViewView::~CImageMatchViewView()
{
}

BOOL CImageMatchViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewView drawing

void CImageMatchViewView::OnDraw(CDC* pDC)
{
	CImageMatchViewDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewView printing

BOOL CImageMatchViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CImageMatchViewView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CImageMatchViewView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewView diagnostics

#ifdef _DEBUG
void CImageMatchViewView::AssertValid() const
{
	CView::AssertValid();
}

void CImageMatchViewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageMatchViewDoc* CImageMatchViewView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageMatchViewDoc)));
	return (CImageMatchViewDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewView message handlers
