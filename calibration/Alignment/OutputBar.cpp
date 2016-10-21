// OutputBar.cpp : implementation of the COutputBar class
//

#include "stdafx.h"
#include "Alignment.h"
#include "OutputBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;

/////////////////////////////////////////////////////////////////////////////
// COutputBar

BEGIN_MESSAGE_MAP(COutputBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(COutputBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputBar construction/destruction

COutputBar::COutputBar()
{
	// TODO: add one-time construction code here

}

COutputBar::~COutputBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// COutputBar message handlers

int COutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create tabs window:
// 	if (!m_wndTabs.Create (CBCGPTabWnd::STYLE_FLAT, rectDummy, this, 1))
// 	{
// 		TRACE0("Failed to create output tab window\n");
// 		return -1;      // fail to create
// 	}
// 	m_wndTabs.SetFlatFrame (FALSE, FALSE);

	// Create list windows.
	// TODO: create your own window here:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE 
						| ES_WANTRETURN | WS_VSCROLL;

	CRect rectClient (0, 0, lpCreateStruct->cx, lpCreateStruct->cy);

	if (!m_wndOutput.Create (dwViewStyle, rectClient, this, 1))
	{
		TRACE0("Failed to create output window\n");
		return -1;      // fail to create
	}
	
// 	if (!m_wndList1.Create (dwViewStyle, rectDummy, &m_wndTabs, 2) ||
// 		!m_wndList2.Create (dwViewStyle, rectDummy, &m_wndTabs, 3) ||
// 		!m_wndList3.Create (dwViewStyle, rectDummy, &m_wndTabs, 4))
// 	{
// 		TRACE0("Failed to create output view\n");
// 		return -1;      // fail to create
// 	}
// 
// 	// Attach list windows to tab:
// 	m_wndTabs.AddTab (&m_wndList1, _T("Output 1"), -1);
// 	m_wndTabs.AddTab (&m_wndList2, _T("Output 2"), -1);
// 	m_wndTabs.AddTab (&m_wndList3, _T("Output 3"), -1);

	return 0;
}

void COutputBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	CRect rc;
    GetClientRect(rc);
	
	m_wndOutput.SetWindowPos(NULL,
		rc.left + 1, rc.top + 1,
		rc.Width() - 2, rc.Height () - 2,
		SWP_NOACTIVATE | SWP_NOZORDER );
}


void COutputBar::OutputMsg( alm_OutputStyle style, char* msg, alm_guiScrollPos pos )
{
	m_wndOutput.AppendString(msg, Msg_Style(style), (Scroll_To)pos);

//	m_wndOutput.AppendString("qwqwe111eqewe\r\nadfafa\r\nsdfadfafsdafsdf\r\nadaf", msg_append, scroll_to_end);
	
	UpdateData(FALSE);
}
