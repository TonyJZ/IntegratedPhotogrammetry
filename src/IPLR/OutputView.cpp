// OutputView.cpp : implementation file
//

#include "stdafx.h"
//#include "almextensionMain.h"
#include "OutputView.h"

// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif

/////////////////////////////////////////////////////////////////////////////
// COutputView dialog
IMPLEMENT_DYNCREATE(COutputView, CBCGPFormView)

COutputView::COutputView(CWnd* pParent /*=NULL*/)
	: CBCGPFormView(COutputView::IDD)
{
	//{{AFX_DATA_INIT(COutputView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COutputView::DoDataExchange(CDataExchange* pDX)
{
	CBCGPFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputView, CBCGPFormView)
	//{{AFX_MSG_MAP(COutputView)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputView message handlers
#include "orsBase/orsIPlatform.h"
//#include "ALMGuiEvent/ALMIGuiEventService.h"

int COutputView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectClient (0, 0, lpCreateStruct->cx, lpCreateStruct->cy);
	
	// Create output pane:
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE 
						| ES_WANTRETURN | WS_VSCROLL;
	
	if (!m_wndOutput.Create (dwStyle, rectClient, this, 1))
	{
		TRACE0("Failed to create output window\n");
		return -1;      // fail to create
	}

//	m_outputMsg.bind( this, &COutputView::OnOutputMsg );	//绑定事件执行函数
	
//	ALMIGuiEventService *pGuiService = 
//		ORS_PTR_CAST(ALMIGuiEventService, getPlatform()->getService( ORS_ALM_GUIEVENT_SERVICE_GUI ));
	
//	pGuiService->GetOutputMsgEvent()->addHandler( &m_outputMsg,&m_outputMsgCookie );	//增加委托人
	
	return 0;
}


void COutputView::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPFormView::OnSize(nType, cx, cy);
	
	CRect rc;
    GetClientRect(rc);
	
	m_wndOutput.SetWindowPos(NULL,
		rc.left + 1, rc.top + 1,
		rc.Width() - 2, rc.Height () - 2,
            SWP_NOACTIVATE | SWP_NOZORDER );
	
}

// void COutputView::OnOutputMsg( alm_OutputStyle style, char* msg, alm_guiScrollPos pos )
// {
// 	m_wndOutput.AppendString(msg, Msg_Style(style), (Scroll_To)pos);
// 
// 	UpdateData(FALSE);
// }
