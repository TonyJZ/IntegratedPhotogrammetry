// CoordSysPage.cpp : implementation file
//

#include "stdafx.h"
//#include "BlockSetting.h"
#include "CoordSysPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoordSysPage dialog


CCoordSysPage::CCoordSysPage(CWnd* pParent /*=NULL*/)
	: CPropertiesTabPage(CCoordSysPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCoordSysPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCoordSysPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCoordSysPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCoordSysPage, CDialog)
	//{{AFX_MSG_MAP(CCoordSysPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoordSysPage message handlers
void CCoordSysPage::OnActivate()
{
	
}