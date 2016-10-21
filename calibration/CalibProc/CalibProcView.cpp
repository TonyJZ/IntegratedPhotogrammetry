// CalibProcView.cpp : implementation of the CCalibProcView class
//

#include "stdafx.h"
#include "CalibProc.h"

#include "CalibProcDoc.h"
#include "CalibProcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalibProcView

IMPLEMENT_DYNCREATE(CCalibProcView, CView)

BEGIN_MESSAGE_MAP(CCalibProcView, CView)
	//{{AFX_MSG_MAP(CCalibProcView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalibProcView construction/destruction

CCalibProcView::CCalibProcView()
{
	// TODO: add construction code here

}

CCalibProcView::~CCalibProcView()
{
}

BOOL CCalibProcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCalibProcView drawing

void CCalibProcView::OnDraw(CDC* pDC)
{
	CCalibProcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CCalibProcView printing

BOOL CCalibProcView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCalibProcView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCalibProcView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CCalibProcView diagnostics

#ifdef _DEBUG
void CCalibProcView::AssertValid() const
{
	CView::AssertValid();
}

void CCalibProcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCalibProcDoc* CCalibProcView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCalibProcDoc)));
	return (CCalibProcDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCalibProcView message handlers
