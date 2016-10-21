// LidarSettingPage.cpp : implementation file
//

#include "stdafx.h"
#include "blockmanager.h"
#include "LidarSettingPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLidarSettingPage dialog


CLidarSettingPage::CLidarSettingPage(CWnd* pParent /*=NULL*/)
	: CDialog(CLidarSettingPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLidarSettingPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLidarSettingPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLidarSettingPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLidarSettingPage, CDialog)
	//{{AFX_MSG_MAP(CLidarSettingPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLidarSettingPage message handlers
