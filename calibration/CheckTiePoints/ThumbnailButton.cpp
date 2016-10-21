//  *************************************************************************
//  *                                                                       *
//  *  CThumbnailButton     of FabricEye                 ed. 1.0  17/9/2002 *
//  *                                                                       *
//  *************************************************************************
//
//  DESCRIPTION     :
//
//  -------------------------------------------------------------------------
//
//  HEADER          : ThumbnailButton.h
//
//  -------------------------------------------------------------------------
//
//  COMMENTS        :
//
//  -------------------------------------------------------------------------
//
//  REVISION        : Edition 1.0  17/9/2002/RFO    Layout
//
//  -------------------------------------------------------------------------
//  RFO  Rex Fong

// ThumbnailButton.cpp : implementation file
//

#include "stdafx.h"
#include <Commctrl.h>

#include "ThumbnailButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CTHUMBNAILBUTTON_CLASSNAME _T( "CThumbnailButton" )
#define RGBTHUMBBKGD RGB( 230, 230, 230 )

CBrush  CThumbnailButton::m_bkBrush;
/////////////////////////////////////////////////////////////////////////////
// CThumbnailButton

CThumbnailButton::CThumbnailButton( const CString& sPath, const int cx, const int cy, HBITMAP hBmp )
  {
  if( !RegisterWindowClass() )
    return;

  m_bTracking  = FALSE;
  m_bMouseClicked = FALSE;

  m_cX = cx;
  m_cY = cy;

  m_sFullpath = sPath;
  ExtractFilename( sPath );

//   HBITMAP hbmp = (HBITMAP) LoadImage( NULL, (LPCTSTR) m_sFullpath, 
//                            IMAGE_BITMAP, cx, cy, 
//                            LR_LOADFROMFILE | LR_CREATEDIBSECTION ) ;
// 
   m_bmp.Attach( hBmp );

//  m_bmp.CreateCompatibleBitmap( NULL, cx, cy );


  }

CThumbnailButton::~CThumbnailButton()
  {
  }


BEGIN_MESSAGE_MAP(CThumbnailButton, CWnd)
	//{{AFX_MSG_MAP(CThumbnailButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CThumbnailButton message handlers

BOOL CThumbnailButton::OnEraseBkgnd(CDC* pDC) 
  {
  CRect rWnd;
  GetWindowRect( &rWnd );

  if( !m_bTracking )
    pDC->FillSolidRect( 1, 1, rWnd.Width()-1, rWnd.Height()-1, RGBTHUMBBKGD );
  else
    pDC->FillSolidRect( 1, 1, rWnd.Width()-1, rWnd.Height()-1, RGB(180,190,210) );

  CPen pen;
  CPen *pOPen;
  POINT pt;
  pt.x = 2;
  pt.y = 2;
  LOGPEN logPen;
  logPen.lopnStyle = PS_SOLID;
  logPen.lopnWidth = pt;

  if( !m_bTracking )
    logPen.lopnColor = RGB(33,33,33);
  else
    logPen.lopnColor = RGB(10,35,105);


  VERIFY( pen.CreatePenIndirect( &logPen ) );

  pOPen = pDC->SelectObject( &pen );

  pDC->MoveTo( 0, 0 );
  pDC->LineTo( rWnd.Width()-1, 0 );
  pDC->LineTo( rWnd.Width()-1, rWnd.Height()-1 );
  pDC->LineTo( 0, rWnd.Height()-1 );
  pDC->LineTo( 0, 0 );

  pDC->SelectObject( pOPen );
  pen.DeleteObject();

  return TRUE;
	}

void CThumbnailButton::OnPaint() 
  {
	CPaintDC dc(this); // device context for painting

  if( m_bmp.GetSafeHandle() != NULL )
    {
		//******************
		//*  Prepare Font  *
		//******************
    LOGFONT lf;
    memset( &lf, 0, sizeof(LOGFONT) );
    lf.lfHeight = 14;
    strcpy( lf.lfFaceName, _T("MS Serif") );

    dc.SetBkColor( RGBTHUMBBKGD );


    CFont font;
    CFont *pOFont;
    VERIFY( font.CreateFontIndirect( &lf ) );
    pOFont = dc.SelectObject( &font );

		//************************
		//*  Draw Thumb Picture  *
		//************************
    CRect rect;
    GetWindowRect( &rect );

    int nSX, nSY; // Starting point
    nSX = ( rect.Width() - m_cX ) / 2;
    nSY = ( rect.Height() - m_cY ) / 4;

    CBitmap *pOldbmp;

    CDC dcBmp;
    dcBmp.CreateCompatibleDC( &dc );
    pOldbmp = dcBmp.SelectObject( &m_bmp );

    dc.BitBlt( nSX, nSY, m_cX, m_cY, &dcBmp, 0, 0, SRCCOPY );

		//*********************
		//*  Write ImageName  *
		//*********************
    TEXTMETRIC   tm ;
    dc.GetTextMetrics( &tm );

    // MAXIMUM Characters = 18
     int nSTX = (rect.Width() - ( tm.tmAveCharWidth * m_sFilename.Left(18).GetLength() ))/2;

    if( m_bTracking )
      dc.SetBkColor( RGB(180,190,210) );
    else
      dc.SetBkColor( RGBTHUMBBKGD );

    // MAXIMUM Characters = 18
     dc.TextOut( nSTX, nSY+m_cY+2, m_sFilename.Left(18) );

    dc.SelectObject( pOFont );
    dcBmp.SelectObject( pOldbmp );

    font.DeleteObject();
    dcBmp.DeleteDC();
    }
	
	// Do not call CWnd::OnPaint() for painting messages
  }

BOOL CThumbnailButton::RegisterWindowClass()
  {
  WNDCLASS wndcls;
  HINSTANCE hInst = AfxGetInstanceHandle();

  if (!(::GetClassInfo(hInst, CTHUMBNAILBUTTON_CLASSNAME, &wndcls)))
    {
    // otherwise we need to register a new class
    m_bkBrush.CreateSolidBrush( RGBTHUMBBKGD );

    wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc      = ::DefWindowProc;
    wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
    wndcls.hInstance        = hInst;
    wndcls.hIcon            = NULL;
    wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    wndcls.hbrBackground    = (HBRUSH) m_bkBrush.GetSafeHandle();
    wndcls.lpszMenuName     = NULL;
    wndcls.lpszClassName    = CTHUMBNAILBUTTON_CLASSNAME;

    if (!AfxRegisterClass(&wndcls))
      {
      AfxThrowResourceException();
      return FALSE;
      }

    }

  return TRUE;
  }

BOOL CThumbnailButton::PreTranslateMessage(MSG* pMsg) 
  {
  CRect rect;
  GetWindowRect( &rect );
  GetParent()->ScreenToClient( &rect );

  switch( pMsg->message )
    {
    case WM_RBUTTONDOWN:
      {
      if( !m_bMouseClicked && m_bTracking )
        {
        m_bMouseClicked = TRUE;
        MoveWindow( rect.left+2, rect.top+2, rect.Width(), rect.Height() );
        }

      if( ::IsWindow(GetParent()->m_hWnd) )
        {
        DWORD dwResult = NULL;
        SendMessageTimeout(
          HWND_BROADCAST, UWM_ON_TNB_RCLICKED, (WPARAM)this, (LPARAM)0,
          SMTO_ABORTIFHUNG|SMTO_NORMAL, 2000, &dwResult );
        }
      }
      return TRUE;
      break;

    case WM_RBUTTONDBLCLK:

      return TRUE;
      break;

    case WM_LBUTTONDBLCLK:

      return TRUE;
      break;

    case WM_LBUTTONDOWN:
      {
      if( !m_bMouseClicked && m_bTracking )
        {
        m_bMouseClicked = TRUE;
        MoveWindow( rect.left+2, rect.top+2, rect.Width(), rect.Height() );
        }

      if( ::IsWindow(GetParent()->m_hWnd) )
        {
        DWORD dwResult = NULL;
        SendMessageTimeout(
          HWND_BROADCAST, UWM_ON_TNB_LCLICKED, (WPARAM)this, (LPARAM)0,
          SMTO_ABORTIFHUNG|SMTO_NORMAL, 2000, &dwResult );
        }

      }
      return TRUE;
      break;

    case WM_MOUSELEAVE:
      {
      if( !m_bMouseClicked )
        {
        MoveWindow( rect.left+2, rect.top+2, rect.Width(), rect.Height() );
        Invalidate();
        }

      m_bMouseClicked = FALSE;
      m_bTracking     = FALSE;
      }
      return TRUE;
      break;

    case WM_LBUTTONUP:
      {
      if( m_bMouseClicked && m_bTracking )
        {
        m_bMouseClicked = FALSE;

        MoveWindow( rect.left-2, rect.top-2, rect.Width(), rect.Height() );
        }
      }

      return TRUE;
      break;

    case WM_MOUSEMOVE:
      {
      if( !m_bTracking )
        {
        m_bMouseClicked = FALSE;

				//**************************
				//*  Track Mouse Movement  *
				//**************************
        TRACKMOUSEEVENT tme;
        memset( &tme, 0, sizeof(TRACKMOUSEEVENT) );
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd ;
        m_bTracking = _TrackMouseEvent( &tme );

        MoveWindow( rect.left-2, rect.top-2, rect.Width(), rect.Height() );
        Invalidate();
        }
      }
    return TRUE;
    break;

    default:
      //TRACE( "%s MSG: %d\n", (LPCTSTR)m_sFilename, pMsg->message );
      break;
    }

	
	return CWnd::PreTranslateMessage(pMsg);
  }

#if 0
LPCTSTR CThumbnailButton::GetBmpName() const
  {
  return ((LPCTSTR) m_sImgName);
  }
#endif

BOOL CThumbnailButton::IsValid() const
  {
  return ( m_bmp.GetSafeHandle() != NULL ) ? TRUE : FALSE;
  }

const CString& CThumbnailButton::GetFullpath() const
  {

  return m_sFullpath;
  }

void CThumbnailButton::ExtractFilename(const CString &sPath)
  {
  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

  CString sTmpPath = sPath;
  LPTSTR lptstr = sTmpPath.GetBufferSetLength( _MAX_PATH );

  _splitpath( lptstr, drive, dir, fname, ext );

  m_sFilename = CString(fname)/* + CString(ext)*/;
   
  }


void CThumbnailButton::ResetTrackFlag( void )
  {
  m_bTracking = FALSE;
  TRACKMOUSEEVENT tme;  // tracking information

  memset( &tme, 0, sizeof(TRACKMOUSEEVENT) );
  tme.cbSize = sizeof(TRACKMOUSEEVENT);
  tme.dwFlags = TME_CANCEL;
  tme.hwndTrack = m_hWnd;

  _TrackMouseEvent(  &tme  );
  Invalidate();
  }

