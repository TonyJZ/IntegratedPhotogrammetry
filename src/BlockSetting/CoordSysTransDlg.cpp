// CoordSysTransDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "BlockSetting.h"
#include "CoordSysTransDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoordSysTransDlg dialog


CCoordSysTransDlg::CCoordSysTransDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCoordSysTransDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCoordSysTransDlg)
	m_strHorizontalDatum = _T("");
	m_strVerticalDatum = _T("");
	//}}AFX_DATA_INIT
}


void CCoordSysTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCoordSysTransDlg)
	DDX_Control(pDX, IDC_VerticalUnit, m_wndVerticalUnit);
	DDX_Control(pDX, IDC_HorizontalUnit, m_wndHorizontalUnit);
	DDX_Text(pDX, IDC_EDIT_HorizontalDatum, m_strHorizontalDatum);
	DDX_Text(pDX, IDC_EDIT_VerticalDatum, m_strVerticalDatum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCoordSysTransDlg, CDialog)
	//{{AFX_MSG_MAP(CCoordSysTransDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoordSysTransDlg message handlers

BOOL CCoordSysTransDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndHorizontalUnit.SetCurSel(0);
	m_wndVerticalUnit.SetCurSel(0);
	
	m_PosUnitH=Unit_Meter;
	m_PosUnitV=Unit_Meter;
	m_RotateSys=RotateSys_YXZ;
	m_AngleUnit=Unit_Degree360;
	
	
	UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCoordSysTransDlg::OnOK() 
{
	UpdateData(TRUE);

	int HorizontalUnit;
	int	VerticalUnit;
	
	HorizontalUnit=m_wndHorizontalUnit.GetCurSel();
	VerticalUnit=m_wndVerticalUnit.GetCurSel();
	
	switch(HorizontalUnit)
	{
	case 0:
		m_PosUnitH=Unit_Meter;
		break;
	case 1:
		m_PosUnitH=Unit_Degree360;
		break;
	case 2:
		m_PosUnitH=Unit_Radian;
		
	default :
		m_PosUnitH=Unit_Meter;
		break;
	}
	
	switch(VerticalUnit)
	{
	case 0:
		m_PosUnitV=Unit_Meter;
		break;
		
	default :
		m_PosUnitV=Unit_Meter;
		break;
	}
	
	CDialog::OnOK();
}
