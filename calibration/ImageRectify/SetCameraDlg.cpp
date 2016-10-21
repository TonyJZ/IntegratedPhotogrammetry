// SetCameraDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageRectify.h"
#include "SetCameraDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetCameraDlg dialog


CSetCameraDlg::CSetCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetCameraDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetCameraDlg)
	m_f = 0.0;
	m_imgHei = 0;
	m_imgWid = 0;
	m_k1 = 0.0;
	m_k2 = 0.0;
	m_k3 = 0.0;
	m_p1 = 0.0;
	m_p2 = 0.0;
	m_pixelY = 0.0;
	m_pixelX = 0.0;
	m_x0 = 0.0;
	m_y0 = 0.0;
	//}}AFX_DATA_INIT
}


void CSetCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetCameraDlg)
	DDX_Text(pDX, IDC_EDIT_f, m_f);
	DDX_Text(pDX, IDC_EDIT_ImgHei, m_imgHei);
	DDX_Text(pDX, IDC_EDIT_ImgWid, m_imgWid);
	DDX_Text(pDX, IDC_EDIT_k1, m_k1);
	DDX_Text(pDX, IDC_EDIT_k2, m_k2);
	DDX_Text(pDX, IDC_EDIT_k3, m_k3);
	DDX_Text(pDX, IDC_EDIT_p1, m_p1);
	DDX_Text(pDX, IDC_EDIT_p2, m_p2);
	DDX_Text(pDX, IDC_EDIT_PixelHei, m_pixelY);
	DDX_Text(pDX, IDC_EDIT_PixelWid, m_pixelX);
	DDX_Text(pDX, IDC_EDIT_x0, m_x0);
	DDX_Text(pDX, IDC_EDIT_y0, m_y0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetCameraDlg, CDialog)
	//{{AFX_MSG_MAP(CSetCameraDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetCameraDlg message handlers
