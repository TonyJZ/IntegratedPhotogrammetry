// InputPosDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "BlockSetting.h"
#include "InputPosDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputPosDialog dialog


CInputPosDialog::CInputPosDialog(iphCamera *camera, CWnd* pParent /*=NULL*/)
	: CDialog(CInputPosDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputPosDialog)
	m_Kappa = 0.0;
	m_Omega = 0.0;
	m_Phi = 0.0;
	m_Xs = 0.0;
	m_Ys = 0.0;
	m_Zs = 0.0;
	m_RotateUnit = 0;
	m_RotateSystem = 0;
//	m_HorizontalUnit=0;
//	m_VerticalUnit=0;
	//}}AFX_DATA_INIT

	m_strHorizontalDatum=_T("");
	m_strVerticalDatum=_T("");
	
	m_pCamera=camera;
	ASSERT(m_pCamera);

	m_Xs=m_pCamera->m_Xs;
	m_Ys=m_pCamera->m_Ys;
	m_Zs=m_pCamera->m_Zs;
	m_Phi=m_pCamera->m_phi;
	m_Omega=m_pCamera->m_omega;
	m_Kappa=m_pCamera->m_kappa;
//	m_RotateSystem=m_pCamera->m_RotateSys;

	switch(m_pCamera->m_AngleUnit)
	{
	case Unit_Degree360:
		m_RotateUnit = 0;
		break;

	case Unit_Degree400:
		m_RotateUnit = 1;
		break;

	case Unit_Radian:
		m_RotateUnit = 2;
		break;

	default :
		m_RotateUnit = 0;
		break;
	}

	switch(m_pCamera->m_RotateSys)
	{
	case RotateSys_YXZ:
		m_RotateSystem = 0;
		break;
		
	case RotateSys_XYZ:
		m_RotateSystem = 1;
		break;
		
	case RotateSys_ZYZ:
		m_RotateSystem = 2;
		break;
		
	default :
		m_RotateSystem = 0;
		break;
	}

}


void CInputPosDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputPosDialog)
	DDX_Text(pDX, IDC_EDIT_Kappa, m_Kappa);
	DDX_Text(pDX, IDC_EDIT_Omega, m_Omega);
	DDX_Text(pDX, IDC_EDIT_Phi, m_Phi);
	DDX_Text(pDX, IDC_EDIT_Xs, m_Xs);
	DDX_Text(pDX, IDC_EDIT_Ys, m_Ys);
	DDX_Text(pDX, IDC_EDIT_Zs, m_Zs);
	DDX_Radio(pDX, IDC_RADIO_360, m_RotateUnit);
	DDX_Radio(pDX, IDC_RADIO_YXZ, m_RotateSystem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputPosDialog, CDialog)
	//{{AFX_MSG_MAP(CInputPosDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputPosDialog message handlers

BOOL CInputPosDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	m_PosUnitH=Unit_Meter;
	m_PosUnitV=Unit_Meter;
	m_RotateSys=RotateSys_YXZ;
	m_AngleUnit=Unit_Degree360;


	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInputPosDialog::OnOK() 
{
	UpdateData(TRUE);

	switch(m_RotateUnit)
	{
	case 0:
		m_AngleUnit=Unit_Degree360;
		break;

	case 1:
		m_AngleUnit=Unit_Degree400;
		break;

	case 2:
		m_AngleUnit=Unit_Radian;
		break;

	default :
		m_AngleUnit=Unit_Degree360;
		break;
	}

	switch(m_RotateSystem)
	{
	case 0:
		m_RotateSys=RotateSys_YXZ;
		break;

	case 1:
		m_RotateSys=RotateSys_XYZ;
		break;

	default:
		m_RotateSys=RotateSys_YXZ;
		break;
	}
	
	m_pCamera->m_AngleUnit=m_AngleUnit;
	m_pCamera->m_RotateSys=m_RotateSys;
	m_pCamera->m_Xs=m_Xs;
	m_pCamera->m_Ys=m_Ys;
	m_pCamera->m_Zs=m_Zs;
	m_pCamera->m_phi=m_Phi;
	m_pCamera->m_omega=m_Omega;
	m_pCamera->m_kappa=m_Kappa;

	CDialog::OnOK();
}
