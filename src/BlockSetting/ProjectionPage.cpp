// ProjectionPage.cpp : implementation file
//

#include "stdafx.h"
//#include "BlockSetting.h"
#include "ProjectionPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectionPage dialog


CProjectionPage::CProjectionPage(CWnd* pParent /*=NULL*/)
	: CPropertiesTabPage(CProjectionPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectionPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProjectionPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectionPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectionPage, CDialog)
	//{{AFX_MSG_MAP(CProjectionPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectionPage message handlers
void CProjectionPage::OnActivate()
{
	
}