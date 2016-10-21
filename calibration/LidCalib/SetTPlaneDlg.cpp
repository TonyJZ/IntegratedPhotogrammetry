// SetTPlaneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "lidcalib.h"
#include "SetTPlaneDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetTPlaneDlg dialog


CSetTPlaneDlg::CSetTPlaneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetTPlaneDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetTPlaneDlg)
	m_alt0 = 0;
	m_lat0 = 36.133888889;
	m_lon0 = 114.341111111;
	//}}AFX_DATA_INIT
}


void CSetTPlaneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetTPlaneDlg)
	DDX_Text(pDX, IDC_alt0, m_alt0);
	DDX_Text(pDX, IDC_lat0, m_lat0);
	DDV_MinMaxDouble(pDX, m_lat0, -90., 90.);
	DDX_Text(pDX, IDC_lon0, m_lon0);
	DDV_MinMaxDouble(pDX, m_lon0, 0., 360.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetTPlaneDlg, CDialog)
	//{{AFX_MSG_MAP(CSetTPlaneDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetTPlaneDlg message handlers

void CSetTPlaneDlg::OnOK() 
{
	UpdateData(TRUE);
	
	CDialog::OnOK();
}
