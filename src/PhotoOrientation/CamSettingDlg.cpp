// CamSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PhotoOrientation.h"
#include "CamSettingDlg.h"


// CCamSettingDlg dialog

IMPLEMENT_DYNAMIC(CCamSettingDlg, CDialog)

CCamSettingDlg::CCamSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCamSettingDlg::IDD, pParent)
	, m_x0(0)
	, m_y0(0)
	, m_f(0)
	, m_ImgWid(0)
	, m_ImgHei(0)
	, m_PixelWid(0)
	, m_PixelHei(0)
	, m_k1(0)
	, m_k2(0)
	, m_k3(0)
	, m_p1(0)
	, m_p2(0)
{
	//RCD105  º½Ò£
	m_f=35.8501;
	m_x0=0.0598;
	m_y0=-0.1802;
	m_ImgWid=5389;
	m_ImgHei=7162;
	m_PixelWid=0.0068;
	m_PixelHei=0.0068;
	m_k1=7.86931e-005;
	m_k2=-6.59007e-008;
	m_k3=7.96896e-012;
	m_p1=1.70817e-024;
	m_p2=4.40734e-024;
}

CCamSettingDlg::~CCamSettingDlg()
{
}

void CCamSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_x0, m_x0);
	DDX_Text(pDX, IDC_EDIT_y0, m_y0);
	DDX_Text(pDX, IDC_EDIT_f, m_f);
	DDX_Text(pDX, IDC_EDIT_ImgWid, m_ImgWid);
	DDX_Text(pDX, IDC_EDIT_ImgHei, m_ImgHei);
	DDX_Text(pDX, IDC_EDIT_PixelWid, m_PixelWid);
	DDX_Text(pDX, IDC_EDIT_PixelHei, m_PixelHei);
	DDX_Text(pDX, IDC_EDIT_k1, m_k1);
	DDX_Text(pDX, IDC_EDIT_k2, m_k2);
	DDX_Text(pDX, IDC_EDIT_k3, m_k3);
	DDX_Text(pDX, IDC_EDIT_p1, m_p1);
	DDX_Text(pDX, IDC_EDIT_p2, m_p2);
}


BEGIN_MESSAGE_MAP(CCamSettingDlg, CDialog)
END_MESSAGE_MAP()


// CCamSettingDlg message handlers
