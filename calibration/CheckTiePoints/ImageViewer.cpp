// ImageViewer.cpp : implementation file
//

#include "stdafx.h"
//#include "fastdisplay.h"
#include "ImageViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageViewer

IMPLEMENT_DYNCREATE(CImageViewer, CSingleImageView)

CImageViewer::CImageViewer()
{
}

CImageViewer::~CImageViewer()
{
}


BEGIN_MESSAGE_MAP(CImageViewer, CSingleImageView)
	//{{AFX_MSG_MAP(CImageViewer)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageViewer drawing

void CImageViewer::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	if(m_pImg==NULL)
	{
		CRect rect;
		GetClientRect(rect);
		pDC->FillSolidRect(rect,RGB(0,0,0));
	}
	CSingleImageView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CImageViewer diagnostics

#ifdef _DEBUG
void CImageViewer::AssertValid() const
{
	CSingleImageView::AssertValid();
}

void CImageViewer::Dump(CDumpContext& dc) const
{
	CSingleImageView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageViewer message handlers

void CImageViewer::OnSize(UINT nType, int cx, int cy) 
{
	CSingleImageView::OnSize(nType, cx, cy);
	
}

void CImageViewer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CSingleImageView::OnLButtonDown(nFlags, point);
}

// BOOL CImageViewer::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
// {
// 	// TODO: Add your specialized code here and/or call the base class
// 	
// 	return CSingleImageView::OnNotify(wParam, lParam, pResult);
// }

// int CImageViewer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
// {
// 	if (CSingleImageView::OnCreate(lpCreateStruct) == -1)
// 		return -1;
// 	
// 	
// 	
// 	return 0;
// }

void CImageViewer::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CSingleImageView::OnMouseMove(nFlags, point);
}

BOOL CImageViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CSingleImageView::OnMouseWheel(nFlags, zDelta, pt);
}

void CImageViewer::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CSingleImageView::OnRButtonDown(nFlags, point);
}
