// BlockSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "blockmanager.h"
#include "BlockSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlockSettingDlg dialog


CBlockSettingDlg::CBlockSettingDlg(iphBlockInfo *pBlockInfo, int nTab, CWnd* pParent /*=NULL*/)
	: CDialog(CBlockSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBlockSettingDlg)
	m_pBlockInfo = pBlockInfo;
	m_nTab = nTab;
	//}}AFX_DATA_INIT
}


void CBlockSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlockSettingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBlockSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CBlockSettingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlockSettingDlg message handlers

BOOL CBlockSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString	strTemp;
	
	strTemp.LoadString(IDS_PageName_Image);
	m_wndPageImg.Create(CImgSettingPage::IDD, &m_wndTabCtrl);
	m_wndTabCtrl.AddTab(&m_wndPageImg,strTemp.GetBuffer(128));


	m_wndTabCtrl.SetCurFocus(m_nTab);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
