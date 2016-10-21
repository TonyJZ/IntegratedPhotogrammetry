// CameraSettingDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "BlockSetting.h"
#include "CameraSettingDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCameraSettingDialog dialog


CCameraSettingDialog::CCameraSettingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCameraSettingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCameraSettingDialog)
	m_f = 0.0;
	m_ImgHei = 0;
	m_ImgWid = 0;
	m_k1 = 0.0;
	m_k2 = 0.0;
	m_k3 = 0.0;
	m_p1 = 0.0;
	m_p2 = 0.0;
	m_PixelHei = 0.0;
	m_PixelWid = 0.0;
	m_x0 = 0.0;
	m_y0 = 0.0;
	m_Option = 0;
	//}}AFX_DATA_INIT
}


void CCameraSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCameraSettingDialog)
	DDX_Text(pDX, IDC_EDIT_f, m_f);
	DDX_Text(pDX, IDC_EDIT_ImgHei, m_ImgHei);
	DDX_Text(pDX, IDC_EDIT_ImgWid, m_ImgWid);
	DDX_Text(pDX, IDC_EDIT_k1, m_k1);
	DDX_Text(pDX, IDC_EDIT_k2, m_k2);
	DDX_Text(pDX, IDC_EDIT_k3, m_k3);
	DDX_Text(pDX, IDC_EDIT_p1, m_p1);
	DDX_Text(pDX, IDC_EDIT_p2, m_p2);
	DDX_Text(pDX, IDC_EDIT_PixelHei, m_PixelHei);
	DDX_Text(pDX, IDC_EDIT_PixelWid, m_PixelWid);
	DDX_Text(pDX, IDC_EDIT_x0, m_x0);
	DDX_Text(pDX, IDC_EDIT_y0, m_y0);
	DDX_Radio(pDX, IDC_RADIO_ALL, m_Option);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCameraSettingDialog, CDialog)
	//{{AFX_MSG_MAP(CCameraSettingDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCameraSettingDialog message handlers

void CCameraSettingDialog::OnOK() 
{
	UpdateData(TRUE);
	
	CDialog::OnOK();
}
