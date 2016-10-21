// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "RegistViewer.h"

#include "MainFrm.h"
#include "math.h"
#include "Align_def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_COPYDATA()
	ON_COMMAND(ID_TiePatch, OnTiePatch)
	ON_COMMAND(ID_TieLine, OnTieLine)
	ON_COMMAND(ID_TiePOINT, OnTiePOINT)
	ON_UPDATE_COMMAND_UI(ID_TiePOINT, OnUpdateTiePOINT)
	ON_UPDATE_COMMAND_UI(ID_TieLine, OnUpdateTieLine)
	ON_UPDATE_COMMAND_UI(ID_TiePatch, OnUpdateTiePatch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame* GetMainFrame()	{  return (CMainFrame*) AfxGetMainWnd();	};

static linkMSG s_msg;

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
//	m_AlgWindow=0;
	m_LayerAttached=false;
}

CMainFrame::~CMainFrame()
{
	COPYDATASTRUCT cpd;
	
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &s_msg;
	s_msg.id=ORS_LM_RegistViewer_Exit;

	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();
	if(theApp->m_AlgWindow)
		::SendMessage( theApp->m_AlgWindow, WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
}


BOOL CALLBACK EnumWinsCB(
						 HWND hwnd,      // handle to parent window
						 LPARAM lParam  )
{
	char windowText[256];
	
	GetWindowText( hwnd, windowText, 255 );
	
	if( NULL != strstr( windowText, RegistWINDOW_TEXT ) )
	{
		s_msg.hLinkWindows[s_msg.nWindows] = hwnd;
		s_msg.bWindowLinkOn[s_msg.nWindows] = 1;
		s_msg.nWindows++;
	}
	
	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CBCGPToolBar::EnableQuickCustomization ();
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

// 	if (!m_layerTreeBar.Create (_T("Layers"), this, CRect (0, 0, 200, 200),
// 		TRUE, ID_VIEW_LAYERTREE_BAR,
// 		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
// 	{
// 		TRACE0("Failed to create Workspace bar\n");
// 		return -1;      // fail to create
// 	}

	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	m_layerTreeBar.EnableDocking(CBRS_ALIGN_ANY);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);

	LoadGuiExtensions( "orsIMainFrameExtension" );

	DockControlBar(&m_wndToolBar);
//	DockControlBar(&m_layerTreeBar);

	CWnd::DragAcceptFiles();

	EnumWindows( (WNDENUMPROC)EnumWinsCB, (LPARAM)this );
	

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::DisplayCoord( double x, double y, double z, float coef, double Xt, double Yt, double Zt)
{
	char coordStr[256];
	
	if( fabs(x) < 180 && fabs(y) < 180 )
		sprintf( coordStr, "%13.8lf %13.8lf %10.3lf %.2f %13.8lf %13.8lf %10.3lf", x,y,z, coef, Xt, Yt, Zt );
	else
		sprintf( coordStr, "%13.3lf %13.3lf %10.3lf %.2f %13.3lf %13.3lf %10.3lf", x,y,z, coef, Xt, Yt, Zt );
	
	m_wndStatusBar.SetPaneText( 0, coordStr );

// 	sprintf( coordStr, "%13.3lf %13.3lf %10.3lf", Xt,Yt,Zt );
// 	m_wndStatusBar.SetTipText( 1, coordStr );
	
	m_wndStatusBar.Invalidate();
}


BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	linkMSG *msg = (linkMSG *)pCopyDataStruct->lpData;
	
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();

	theApp->m_tieType=	msg->objType;

	if(msg->id==ORS_LM_Measure_Info)
		m_tieID=msg->tieID;
	
	GetActiveView()->Invalidate();

	return S_OK;
}

void CMainFrame::OnTiePatch() 
{
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();
	theApp->m_tieType=TO_PATCH;

	COPYDATASTRUCT cpd;
	
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &s_msg;

	s_msg.id=ORS_LM_ObjType;
	
//	s_msg.id = ORS_LM_MOUSEMOVE;
	s_msg.objType=	TO_PATCH;
	//s_msg.ptNum=0;
	//s_msg.pTObj=NULL;
	
	for( int i=0; i < s_msg.nWindows; i++ )
	{
		// 只能用SendMessage
		//::PostMessage( m_linkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
		if( m_hWnd != s_msg.hLinkWindows[i] && s_msg.bWindowLinkOn[i] )
			::SendMessage( s_msg.hLinkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
	}

	if(theApp->m_AlgWindow)
		::SendMessage( theApp->m_AlgWindow, WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
}


void CMainFrame::OnTieLine() 
{
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();
	theApp->m_tieType=TO_LINE;
	
	COPYDATASTRUCT cpd;
	
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &s_msg;
	
	//	s_msg.id = ORS_LM_MOUSEMOVE;
	s_msg.id=ORS_LM_ObjType;
	s_msg.objType=	TO_LINE;
	
	for( int i=0; i < s_msg.nWindows; i++ )
	{
		// 只能用SendMessage
		//::PostMessage( m_linkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
		if( m_hWnd != s_msg.hLinkWindows[i] && s_msg.bWindowLinkOn[i] )
			::SendMessage( s_msg.hLinkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
	}

	if(theApp->m_AlgWindow)
		::SendMessage( theApp->m_AlgWindow, WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
}

void CMainFrame::OnTiePOINT() 
{
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();
	theApp->m_tieType=TO_POINT;
	
	COPYDATASTRUCT cpd;
	
	cpd.dwData = 0;
	cpd.cbData = sizeof( linkMSG );
	cpd.lpData = &s_msg;
	
	//	s_msg.id = ORS_LM_MOUSEMOVE;
	s_msg.id=ORS_LM_ObjType;
	s_msg.objType=	TO_POINT;
	
	for( int i=0; i < s_msg.nWindows; i++ )
	{
		// 只能用SendMessage
		//::PostMessage( m_linkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
		if( m_hWnd != s_msg.hLinkWindows[i] && s_msg.bWindowLinkOn[i] )
			::SendMessage( s_msg.hLinkWindows[i], WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
	}
	
	if(theApp->m_AlgWindow)
		::SendMessage( theApp->m_AlgWindow, WM_COPYDATA, (UINT)m_hWnd, (LPARAM)&cpd );
}

void CMainFrame::OnUpdateTiePOINT(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();
	
	if(theApp->m_tieType==TO_POINT)
		pCmdUI->SetCheck(TRUE);
	else 
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateTieLine(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();
	
	if(theApp->m_tieType==TO_LINE)
		pCmdUI->SetCheck(TRUE);
	else 
		pCmdUI->SetCheck(FALSE);
	
}

void CMainFrame::OnUpdateTiePatch(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();

	if(theApp->m_tieType==TO_PATCH)
		pCmdUI->SetCheck(TRUE);
	else 
		pCmdUI->SetCheck(FALSE);
	
}

#include "orsGuiBase/orsIViewerExtension.h"
#include "orsGuiBase/orsIMainFrameExtension.h"
void CMainFrame::AttachLayerCollection( orsILayerCollection *pLayerCollection )
{
	if (m_LayerAttached) return;

//	GetLayerTreeBar().AttachLayerCollection( pLayerCollection );
//	GetDataSourceTreeBar().AttachLayerCollection( pLayerCollection );

	int i;	
	for( i=0; i<m_vExtensions.size(); i++ )
	{
		orsIViewerExtension *pExt = ORS_PTR_CAST( orsIViewerExtension, m_vExtensions[i].get() );

		if( NULL != pExt ) {
			pExt->SetLayerCollection( pLayerCollection );
		}
		else	{
			orsIMainFrameExtension *pExt = ORS_PTR_CAST( orsIMainFrameExtension, m_vExtensions[i].get() );

			if( NULL != pExt )
				pExt->SetLayerCollection( pLayerCollection );
		}
	}

// 	if( m_pPageView ) {
// 		m_pPageView->SetLayerCollection( pLayerCollection );
// 	}

	m_LayerAttached=true;
}

