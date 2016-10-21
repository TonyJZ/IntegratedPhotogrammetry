// ImgSettingPage.cpp : implementation file
//

#include "stdafx.h"
#include "blockmanager.h"
#include "ImgSettingPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgSettingPage dialog


CImgSettingPage::CImgSettingPage(CWnd* pParent /*=NULL*/)
	: CPropertiesTabPage(CImgSettingPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImgSettingPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CImgSettingPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImgSettingPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImgSettingPage, CDialog)
	//{{AFX_MSG_MAP(CImgSettingPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgSettingPage message handlers

void CImgSettingPage::OnActivate()
{
	
}