// PhotoLocationView.cpp : implementation of the CPhotoLocationView class
//

#include "stdafx.h"
#include "PhotoLocation.h"

#include "PhotoLocationDoc.h"
#include "PhotoLocationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationView

IMPLEMENT_DYNCREATE(CPhotoLocationView, C3DView)

BEGIN_MESSAGE_MAP(CPhotoLocationView, C3DView)
	//{{AFX_MSG_MAP(CPhotoLocationView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, C3DView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, C3DView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, C3DView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationView construction/destruction

CPhotoLocationView::CPhotoLocationView()
{
	// TODO: add construction code here

}

CPhotoLocationView::~CPhotoLocationView()
{
}

BOOL CPhotoLocationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return C3DView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationView drawing

void CPhotoLocationView::OnDraw(CDC* pDC)
{
	CPhotoLocationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	C3DView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationView printing

BOOL CPhotoLocationView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPhotoLocationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPhotoLocationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationView diagnostics

#ifdef _DEBUG
void CPhotoLocationView::AssertValid() const
{
	C3DView::AssertValid();
}

void CPhotoLocationView::Dump(CDumpContext& dc) const
{
	C3DView::Dump(dc);
}

CPhotoLocationDoc* CPhotoLocationView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPhotoLocationDoc)));
	return (CPhotoLocationDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPhotoLocationView message handlers
