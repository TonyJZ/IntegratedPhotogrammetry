// StereoModelView.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>
#include "resource.h"

//#include "orsImage\orsImageSource.h"

#include "imageviewerView1.h"
#include "CheckKPDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CImageViewerView

IMPLEMENT_DYNCREATE(CImageViewerView, CSingleImageView)

CImageViewerView::CImageViewerView()
{
	m_bSelPoint=false;
	m_SelectPointNo=-1;
}


CImageViewerView::~CImageViewerView()
{

}


BEGIN_MESSAGE_MAP(CImageViewerView, CSingleImageView)	//CSingleImageView)
	//{{AFX_MSG_MAP(CImageViewerView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
// 	ON_COMMAND(ID_PAN, CSingleImageView::OnPan)
// 	ON_UPDATE_COMMAND_UI(ID_PAN, CSingleImageView::OnUpdatePan)
// 	ON_COMMAND(ID_COORD, CSingleImageView::OnCoord)
// 	ON_UPDATE_COMMAND_UI(ID_COORD, CSingleImageView::OnUpdateCoord)
// 	ON_COMMAND(ID_ImageFixedMODE, CSingleImageView::OnImageFixedMode)
// 	ON_UPDATE_COMMAND_UI(ID_ImageFixedMODE, CSingleImageView::OnUpdateImageFixedMode)
// 	ON_COMMAND(ID_CursorFixedMODE, CSingleImageView::OnCursorFixedMode)
// 	ON_UPDATE_COMMAND_UI(ID_CursorFixedMODE, CSingleImageView::OnUpdateCursorFixedMode)
// 	ON_COMMAND(ID_USE_TEXTURE, CSingleImageView::OnUseTexture)
// 	ON_UPDATE_COMMAND_UI(ID_USE_TEXTURE, CSingleImageView::OnUpdateUseTexture)
// 	ON_COMMAND(ID_OPENGL, CSingleImageView::OnOpenGL)
// 	ON_UPDATE_COMMAND_UI(ID_OPENGL, CSingleImageView::OnUpdateOpenGL)
// 	ON_COMMAND(ID_ZOOM_IN, CSingleImageView::OnZoomIn)
// 	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, CSingleImageView::OnUpdateZoomIn)
// 	ON_COMMAND(ID_ZOOM_OUT, CSingleImageView::OnZoomOut)
// 	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, CSingleImageView::OnUpdateZoomOut)

END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CImageViewerView drawing


//DEL void CImageViewerView::OnInitialUpdate()
//DEL {	
//DEL 	CSingleImageView::OnInitialUpdate();
//DEL //	GetParent()->SendMessage(WM_SYSCOMMAND,SC_DEFAULT,0);
//DEL }

void CImageViewerView::OnSize(UINT nType, int cx, int cy) 
{
//	if( nType != 9999 )
	CSingleImageView::OnSize(nType, cx, cy);

}


void CImageViewerView::OnDraw(CDC* pDC)
{   
//	SetOpState(opPAN);

	CPen* oldpen;
	float	x, y;
	CPoint point;
	CString str;
//	GetClientRect(crect);
//	pDC->FillSolidRect(crect,RGB(0,0,0));

	CRect rect;
	GetClientRect(rect);

	if(m_pImg==NULL)
	{
		pDC->FillSolidRect(rect,RGB(0,0,0));
	}

	CSingleImageView::OnDraw( pDC );
	
	if (((CCheckKPDialog*)(CDialog*)GetParent())->m_bCPtFileOpen)
	{
		//m_lRay=m_pImg->GetImageGeometry();
        //m_iCPtNum=m_aryGCPData->GetSize();
		
		m_KPnum=((CCheckKPDialog*)GetParent())->m_KPnum;
		m_pKPbuf=((CCheckKPDialog*)GetParent())->m_pKPbuf;
		
		CPen pen;			
		//		CPen* oldpen;
		CPoint	point;
		//		float	x, y;
		
		
		pen.CreatePen(PS_SOLID, 1, RGB(255,0,0));		//黑色   未分类	 1
		oldpen=pDC->SelectObject(&pen);
		
		for (int i=0; i<m_KPnum; i++)
		{
			x=m_pKPbuf[i].x;
			y=m_pKPbuf[i].y;
			point.x=Img2VCS_X(x);
			point.y=Img2VCS_Y(y);
			
			if(rect.PtInRect(point))
			{
				pDC->MoveTo(point.x-7,point.y);
				pDC->LineTo(point.x+7,point.y);
				pDC->MoveTo(point.x,point.y-7);
				pDC->LineTo(point.x,point.y+7);
			}
		}
		
		pDC->SelectObject(oldpen);
	}
	
	if(m_bSelPoint)
	{
		CPen pen;			
		CPoint	point;
		
		pen.CreatePen(PS_SOLID, 1, RGB(255,255,0));		
		oldpen=pDC->SelectObject(&pen);
		
		x=m_pKPbuf[m_SelectPointNo].x;
		y=m_pKPbuf[m_SelectPointNo].y;
		point.x=Img2VCS_X(x);
		point.y=Img2VCS_Y(y);
		
		pDC->MoveTo(point.x-7,point.y);
		pDC->LineTo(point.x+7,point.y);
		pDC->MoveTo(point.x,point.y-7);
		pDC->LineTo(point.x,point.y+7);
		
		pDC->SelectObject(oldpen);
		
		
	}
}

void CImageViewerView::DrawYourContents()
{


	return;

	CPen* oldpen;
	float	x, y;
	CPoint point;
	CString str;
	CRect rect;
	GetClientRect(rect);

	CClientDC dc(this);
	OnPrepareDC(&dc);

	if (((CCheckKPDialog*)(CDialog*)GetParent())->m_bCPtFileOpen)
	{
		//m_lRay=m_pImg->GetImageGeometry();
        //m_iCPtNum=m_aryGCPData->GetSize();
		
		m_KPnum=((CCheckKPDialog*)GetParent())->m_KPnum;
		m_pKPbuf=((CCheckKPDialog*)GetParent())->m_pKPbuf;
		
		CPen pen;			
		//		CPen* oldpen;
		CPoint	point;
		//		float	x, y;
		
		
		pen.CreatePen(PS_SOLID, 1, RGB(255,0,0));		//黑色   未分类	 1
		oldpen=dc.SelectObject(&pen);
		
		for (int i=0; i<m_KPnum; i++)
		{
			x=m_pKPbuf[i].x;
			y=m_pKPbuf[i].y;
			point.x=Img2VCS_X(x);
			point.y=Img2VCS_Y(y);
			
			//if(rect.PtInRect(point))
			//{
				dc.MoveTo(point.x-7,point.y);
				dc.LineTo(point.x+7,point.y);
				dc.MoveTo(point.x,point.y-7);
				dc.LineTo(point.x,point.y+7);
			//}
		}
		
		dc.SelectObject(oldpen);
	}

	if(m_bSelPoint)
	{
		CPen pen;			
		CPoint	point;
		
		
		pen.CreatePen(PS_SOLID, 1, RGB(255,255,0));		
		oldpen=dc.SelectObject(&pen);

		x=m_pKPbuf[m_SelectPointNo].x;
		y=m_pKPbuf[m_SelectPointNo].y;
		point.x=Img2VCS_X(x);
		point.y=Img2VCS_Y(y);
	
		dc.MoveTo(point.x-7,point.y);
		dc.LineTo(point.x+7,point.y);
		dc.MoveTo(point.x,point.y-7);
		dc.LineTo(point.x,point.y+7);

		dc.SelectObject(oldpen);


	}
}
/////////////////////////////////////////////////////////////////////////////
// CImageViewerView diagnostics

#ifdef _DEBUG
void CImageViewerView::AssertValid() const
{
	CSingleImageView::AssertValid();
}

void CImageViewerView::Dump(CDumpContext& dc) const
{
	CSingleImageView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageViewerView message handlers

void CImageViewerView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
//	CGeoEngine *engine = GetGeoEngine();
//	if( engine )
//		engine->OnLButtonDblClk( nFlags, m_curPos, 0 );
	
	CSingleImageView::OnLButtonDblClk(nFlags, point);
}



//////////////////////////////////// 自动切准高程专用 /////////////////////////


/////////////////////////////////////////////// end of ....

void CImageViewerView::OnLButtonDown(UINT nFlags, CPoint point) 
{
//	::SetCursor(::LoadCursor(NULL, IDC_ARROW));	

	CSingleImageView::OnLButtonDown( nFlags,  point);
}

void CImageViewerView::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	CSingleImageView::OnLButtonUp(nFlags, point);
}


/////////////////////////////////////////////////
// clear rubber band, etc
//
// if need auto_pan, return true
/////////////////////////////////////////////////



void CImageViewerView::OnMouseMove(UINT nFlags, CPoint point) 
{
//    ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	CSingleImageView::OnMouseMove( nFlags, point );

}


BOOL CImageViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point) 
{
//	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	return CSingleImageView::OnMouseWheel( nFlags, zDelta, point );
}


//DEL void CImageViewerView::OnRButtonDown(UINT nFlags, CPoint point) 
//DEL {
//DEL 	CSingleImageView::OnRButtonDown( nFlags, point );
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL }



//DEL void CImageViewerView::OnRButtonUp(UINT nFlags, CPoint point) 
//DEL {
//DEL 	CSingleImageView::OnRButtonUp( nFlags, point );
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL 
//DEL }



//DEL void CImageViewerView::OnRButtonDblClk(UINT nFlags, CPoint point) 
//DEL {
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL //	CGeoEngine *engine = GetGeoEngine();
//DEL //	if( engine )
//DEL //		engine->OnRButtonDblClk( nFlags, m_curPos, 0 );
//DEL 	
//DEL 	CSingleImageView::OnRButtonDblClk(nFlags, point);
//DEL }


//DEL void CImageViewerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
//DEL {
//DEL 	CSingleImageView::OnKeyDown(nChar, nRepCnt, nFlags);
//DEL }

//DEL void CImageViewerView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
//DEL {
//DEL 	CSingleImageView::OnKeyUp(nChar, nRepCnt, nFlags);
//DEL }

//DEL void CImageViewerView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
//DEL {
//DEL 	CSingleImageView::OnChar(nChar, nRepCnt, nFlags);
//DEL }



//DEL BOOL CImageViewerView::OnEraseBkgnd(CDC* pDC) 
//DEL {
//DEL 	return CSingleImageView::OnEraseBkgnd(pDC);
//DEL }

//DEL void CImageViewerView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
//DEL {
//DEL 	// TODO: Add your specialized code here and/or call the base class
//DEL 	
//DEL 	CSingleImageView::OnPrepareDC(pDC, pInfo);
//DEL }



//DEL BOOL CImageViewerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
//DEL {
//DEL 	return CSingleImageView::OnSetCursor(pWnd, nHitTest, message);
//DEL 
//DEL }



//DEL int CImageViewerView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
//DEL {
//DEL 	if (CSingleImageView::OnCreate(lpCreateStruct) == -1)
//DEL 		return -1;
//DEL 
//DEL 	return 0;
//DEL }



//DEL void CImageViewerView::OnContextMenu(CWnd* pWnd, CPoint point) 
//DEL {
//DEL }