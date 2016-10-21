//  *************************************************************************
//  *                                                                       *
//  *  CThumbNailControl    of ThumbNail                ed. 1.0  23/10/2002 *
//  *                                                                       *
//  *************************************************************************
//
//  DESCRIPTION     : Thumbnail Control
//
//  -------------------------------------------------------------------------
//
//  COMMENTS        :
//
//    You can modify, revise & distribute any part of this piece of code 
//    provided that you will retain this header.
//
//    Scrolling part is referenced from the 'MFC GRID CONTROL' project by 
//    Chris Maunder at www.codeproject.com
//
//  -------------------------------------------------------------------------
//
//  REVISION        : Edition 1.0  23/10/2002/RFO    Layout
//
//  -------------------------------------------------------------------------
//
//  RFO  Rex Fong (rexfong@bac98.net)
//

// ThumbNailControl.cpp : implementation file
//

#include "stdafx.h"

#include <math.h>

#include "ThumbNailControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RGBTHUMBCTRLBKGD RGB( 250, 250, 250 )
#define RGBTHUMBCTRLBORDER RGB( 0, 0, 0 )

#define CTHUMBNAILCONTROL_CLASSNAME _T("CThumbNailControl")

/////////////////////////////////////////////////////////////////////////////
// CThumbNailControl

CBrush  CThumbNailControl::m_bkBrush;
CThumbNailControl::CThumbNailControl()
  {
  if( !RegisterWindowClass() )
    return;

  m_arPtrData.RemoveAll();

  m_nCol = 0;
  m_nRow = 0;

  m_nStartX = 0;
  m_nStartY = 0;

  m_nThumbWidth  = 0;
  m_nThumbHeight = 0;
  }

CThumbNailControl::~CThumbNailControl()
  {
	  clear();
  }

void CThumbNailControl::clear()
{

	for( int i=m_arPtrData.GetSize()-1; i>=0; i-- )
	{
		CThumbnailButton *pBtn = (CThumbnailButton*) m_arPtrData.GetAt(i);
		m_arPtrData.RemoveAt(i);

		if( pBtn != NULL )
			delete pBtn;
	}

	m_arPtrData.RemoveAll();
}


BEGIN_MESSAGE_MAP(CThumbNailControl, CWnd)
	//{{AFX_MSG_MAP(CThumbNailControl)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThumbNailControl message handlers

//---------------------------------------------------------------------------
//*************************
//*  InitializeVariables	*
//*************************
// 
// INPUTS       : none
// 
// RETURNS      : none
// 
void CThumbNailControl::InitializeVariables( int cX /* = DEFAULT_THUMBWIDTH */, 
                                             int cY /* = DEFAULT_THUMBHEIGHT */ )
  {
  m_arPtrData.RemoveAll();

  m_nThumbWidth     = cX;
  m_nThumbHeight    = cY;

  if( ::IsWindow( m_hWnd  ) )
    {
    CRect rect;
    GetClientRect( &rect );

    m_nCol  = (int) floor ( ((float) rect.Width()) / (m_nThumbWidth + DEFAULT_SEPERATOR) );
    }

  }

//---------------------------------------------------------------------------
//********************
//*  RecalButtonPos  *
//********************
// 
// DESCRIPTION  : Calculate button positions
// 
// INPUTS       : none
// 
// RETURNS      : none
// 
void CThumbNailControl::RecalButtonPos()
  {
  CRect rect;
  GetClientRect( &rect );

  int nX=0;
  int nY=0;
  m_nRow=0;

  for( int i=0; i<m_arPtrData.GetSize(); i++ )
    {
    if( nX == 0 )
      m_nRow++;

    CThumbnailButton *pBtn = (CThumbnailButton*) m_arPtrData.GetAt(i);

    ASSERT( AfxIsValidAddress(pBtn, sizeof(CThumbnailButton), TRUE) );
    ASSERT( ::IsWindow( pBtn->m_hWnd ) );

    pBtn->SetWindowPos( NULL, 
                        m_nStartX + DEFAULT_SEPERATOR+nX, 
                        m_nStartY + DEFAULT_SEPERATOR+nY, 
                        0, 0, SWP_NOSIZE|SWP_NOZORDER );

    nX += m_nThumbWidth + DEFAULT_SEPERATOR;

    if( ( rect.Width() - nX ) < 0.8*(m_nThumbWidth+DEFAULT_SEPERATOR) )
      {
      nX = 0;
      nY += m_nThumbHeight + DEFAULT_SEPERATOR;
      }

    }

  }

//---------------------------------------------------------------------------
//*************************
//*  RegisterWindowClass	*
//*************************
// 
// DESCRIPTION  : Register the "CThumNailControl" class name into window.
// 
// INPUTS       : none
// 
// RETURNS      : BOOL - TRUE if success, otherwise FALSE
// 
BOOL CThumbNailControl::RegisterWindowClass()
  {
  WNDCLASS wndcls;
  HINSTANCE hInst = AfxGetInstanceHandle();

  if (!(::GetClassInfo(hInst, CTHUMBNAILCONTROL_CLASSNAME, &wndcls)))
    {
    // otherwise we need to register a new class
    m_bkBrush.CreateSolidBrush( RGBTHUMBCTRLBKGD );

    wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc      = ::DefWindowProc;
    wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
    wndcls.hInstance        = hInst;
    wndcls.hIcon            = NULL;
    wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    wndcls.hbrBackground    = (HBRUSH) m_bkBrush.GetSafeHandle();
    wndcls.lpszMenuName     = NULL;
    wndcls.lpszClassName    = CTHUMBNAILCONTROL_CLASSNAME;

    if (!AfxRegisterClass(&wndcls))
      {
      AfxThrowResourceException();
      return FALSE;
      }
    }

  return TRUE;
  }


//---------------------------------------------------------------------------
//*********
//*  Add	*
//*********
// 
// SYNOPSIS     : 
// 
// DESCRIPTION  : [TODO]
// 
// INPUTS       : const CString& sPath - 
// 
// RETURNS      : [TODO]
// 
// NOTE         : [TODO]
// 
// TODO         : [TODO]
// 
void CThumbNailControl::Add( const CString& sPath, HBITMAP hBmp )
  {
  ASSERT( ::IsWindow(this->m_hWnd) );

  // Initialize Variables Not done!
  ASSERT( (m_nThumbWidth + m_nThumbHeight) > 0 );

  Invalidate( FALSE );

  long nThumbImgWidth  = (int)(0.75f*m_nThumbWidth);
  long nThumbImgHeight = (int)(0.75f*m_nThumbHeight);
  CThumbnailButton *pBtn = new CThumbnailButton( sPath, 
                                  nThumbImgWidth, nThumbImgHeight, hBmp );
  pBtn->Create( _T("CThumbnailButton"), 
                _T(""), 
                WS_CHILD|WS_VISIBLE, 
                CRect(0,0,m_nThumbWidth,m_nThumbHeight), this, 0 );

  if( pBtn->IsValid() )
    {
    VERIFY( ::IsWindow(pBtn->m_hWnd) );

    m_arPtrData.Add( (void*) pBtn );

    RecalButtonPos();
    RecalScrollBars();
    }
  else
    {
    pBtn->DestroyWindow();
    delete pBtn;
    }

  }

//---------------------------------------------------------------------------
//***************
//*  OnHScroll	*
//***************
// 
// DESCRIPTION  : Perform Horizontal Scrolling
// 
// INPUTS       : UINT nSBCode - SB_* message
//		            UINT - Not used
//		            CScrollBar* - Not used
// 
// RETURNS      : none
// 
void CThumbNailControl::OnHScroll(UINT nSBCode, UINT /* nPos */ , CScrollBar* /* pScrollBar */) 
  {
  int nScrollPos = GetScrollPos32(SB_HORZ);

  switch( nSBCode )
    {
    case SB_LEFT:
      break;

    case SB_ENDSCROLL:
      RedrawWindow();
      break;

    case SB_LINELEFT :
      SetScrollPos32(SB_HORZ, nScrollPos - 1 );
      break;

    case SB_LINERIGHT:
      SetScrollPos32(SB_HORZ, nScrollPos + 1);
      break;

    case SB_PAGELEFT :
      SetScrollPos32(SB_HORZ, nScrollPos - 20 );
      break;

    case SB_PAGERIGHT:
      SetScrollPos32(SB_HORZ, nScrollPos + 20);
      break;

    case SB_RIGHT:
      break;

    case SB_THUMBPOSITION:  // Go down...
    case SB_THUMBTRACK:
      SetScrollPos32( SB_HORZ, GetScrollPos32(SB_HORZ, TRUE) );
      break;

    default:
      break;
    }

  m_nStartX = -GetScrollPos32(SB_HORZ);
  RecalButtonPos();
  }

//---------------------------------------------------------------------------
//*********************
//*  RecalScrollBars	*
//*********************
// 
// DESCRIPTION  : Calculate scroll bars positions
// 
// INPUTS       : none
// 
// RETURNS      : none
// 
void CThumbNailControl::RecalScrollBars()
  {
  CRect rect;
  GetClientRect( &rect );
  ClientToScreen( &rect );

  long nTotalWidth  = m_nThumbWidth * m_nCol + DEFAULT_SEPERATOR * ( 1 + m_nCol );
  long nTotalHeight = m_nThumbHeight * m_nRow + DEFAULT_SEPERATOR * ( 2 + m_nRow );

  long nWidDiff = nTotalWidth  - rect.Width();
  long nHeiDiff = nTotalHeight - rect.Height();

  if( nWidDiff > DEFAULT_SEPERATOR && m_arPtrData.GetSize() >= m_nCol )
    {
    SCROLLINFO si;
    memset( &si, 0, sizeof(SCROLLINFO) );

    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nPage = (int)(0.25*(nWidDiff+14));
    si.nMin = 0;
    si.nMax = (int)(1.25*(nWidDiff+14));

    SetScrollInfo( SB_HORZ, &si, TRUE );

    EnableScrollBarCtrl( SB_HORZ );
    EnableScrollBar( SB_HORZ );
    }

  if( nHeiDiff > DEFAULT_SEPERATOR )
    {
    SCROLLINFO si;
    memset( &si, 0, sizeof(SCROLLINFO) );

    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nPage = (int)(0.25*(nHeiDiff+14));
    si.nMin = 0;
    si.nMax = (int)(1.25*(nHeiDiff+14));

    SetScrollInfo( SB_VERT, &si, TRUE );

    EnableScrollBarCtrl( SB_VERT );
    EnableScrollBar( SB_VERT );
    }

  }


//---------------------------------------------------------------------------
//********************
//*  SetScrollPos32  *
//********************
// 
// DESCRIPTION  : Set scroll bar position
// 
// INPUTS       : int nBar - SB_VERT / SB_HORZ
//		            int nPos - new position
//		            BOOL bRedraw - redraw flag
// 
// RETURNS      : [TODO]
// 
// NOTE         : [TODO]
// 
// TODO         : [TODO]
// 
BOOL CThumbNailControl::SetScrollPos32(int nBar, int nPos, BOOL bRedraw)
  {
  SCROLLINFO si;
  si.cbSize = sizeof(SCROLLINFO);
  si.fMask  = SIF_POS;
  si.nPos   = nPos;
  return SetScrollInfo(nBar, &si, bRedraw);
  }

//---------------------------------------------------------------------------
//********************
//*  GetScrollPos32  *
//********************
// 
// DESCRIPTION  : Get scroll bar position
// 
// INPUTS       : int nBar - SB_VERT / SB_HORZ
//		            BOOL bGetTrackPos - 
// 
// RETURNS      : int - position at scroll bar
// 
int CThumbNailControl::GetScrollPos32( int nBar, BOOL bGetTrackPos )
  {
  SCROLLINFO si;
  si.cbSize = sizeof(SCROLLINFO);

  if (bGetTrackPos)
    {
    if (GetScrollInfo(nBar, &si, SIF_TRACKPOS))
      return si.nTrackPos;
    }
  else
    {
    if (GetScrollInfo(nBar, &si, SIF_POS))
      return si.nPos;
    }

  return 0;
  }

//---------------------------------------------------------------------------
//***************
//*  OnVScroll	*
//***************
// 
// DESCRIPTION  : Perform Vertical Scrolling
// 
// INPUTS       : UINT nSBCode - SB_* message
//		            UINT - Not used
//		            CScrollBar* - Not used
// 
// RETURNS      : none
// 
void CThumbNailControl::OnVScroll(UINT nSBCode, UINT /*nPos*/, CScrollBar* /*pScrollBar */) 
  {
  int nScrollPos = GetScrollPos32(SB_VERT);
  switch( nSBCode )
    {
    case SB_BOTTOM:
      break;

    case SB_ENDSCROLL:
      RedrawWindow();
      break;

    case SB_LINEDOWN:
      SetScrollPos32(SB_VERT, nScrollPos + 1 );
      break;

    case SB_LINEUP:
      SetScrollPos32(SB_VERT, nScrollPos - 1 );
      break;

    case SB_PAGEDOWN:
      SetScrollPos32(SB_VERT, nScrollPos + 20 );
      break;

    case SB_PAGEUP:
      SetScrollPos32(SB_VERT, nScrollPos - 20 );
      break;

    case SB_THUMBPOSITION: // Go down..
    case SB_THUMBTRACK:
      SetScrollPos32( SB_VERT, GetScrollPos32(SB_VERT, TRUE) );
      break;

    case SB_TOP:
      break;

    default:
      break;
    }

  m_nStartY = -GetScrollPos32(SB_VERT);
  RecalButtonPos();
  }

//---------------------------------------------------------------------------
//******************
//*  OnMouseWheel  *
//******************
// 
// DESCRIPTION  : Extra support on Mouse Wheel scrolling
// 
// INPUTS       : UINT nFlags -   passed to parent OnMouseWheel function
//		            short zDelta -  degree of rotation
//		            CPoint pt -     passed to parent OnMouseWheel function
// 
// RETURNS      : BOOL - returned from parent OnMouseWheel function
// 
BOOL CThumbNailControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
  {
  int nScrollPos = GetScrollPos32(SB_VERT);
  int nStep = zDelta/WHEEL_DELTA;

  SetScrollPos32(SB_VERT, nScrollPos - nStep*5 );

  m_nStartY = -GetScrollPos32(SB_VERT);
  RecalButtonPos();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
  }
