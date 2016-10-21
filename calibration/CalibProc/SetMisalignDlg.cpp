// SetMisalignDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CalibProc.h"
#include "SetMisalignDlg.h"


// CSetMisalignDlg dialog

IMPLEMENT_DYNCREATE(CSetMisalignDlg, CDialog)

CSetMisalignDlg::CSetMisalignDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetMisalignDlg::IDD, pParent)
	, m_roll(0)
	, m_pitch(0)
	, m_heading(0)
	, m_tx(0)
	, m_ty(0)
	, m_tz(0)
	, m_bMisalign6Param(FALSE)
	, m_dTranslationX(0)
	, m_dTranslationY(0)
	, m_dTranslationZ(0)
	, m_bTranslationParam(FALSE)
	, m_sa0(0)
	, m_bScanAngle0(FALSE)
	, m_sa_ceof(0)
	, m_drho(0)
{

}

CSetMisalignDlg::~CSetMisalignDlg()
{
}

void CSetMisalignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Roll, m_roll);
	DDX_Text(pDX, IDC_EDIT_Pitch, m_pitch);
	DDX_Text(pDX, IDC_EDIT_Heading, m_heading);
	DDX_Text(pDX, IDC_EDIT_Tx, m_tx);
	DDX_Text(pDX, IDC_EDIT_Ty, m_ty);
	DDX_Text(pDX, IDC_EDIT_Tz, m_tz);
	DDX_Radio(pDX, IDC_RADIO_3Param, m_bMisalign6Param);
	DDX_Text(pDX, IDC_EDIT_DX, m_dTranslationX);
	DDX_Text(pDX, IDC_EDIT_DY, m_dTranslationY);
	DDX_Text(pDX, IDC_EDIT_DZ, m_dTranslationZ);
	DDX_Check(pDX, IDC_CHECK_Semi3Param, m_bTranslationParam);
	DDX_Text(pDX, IDC_EDIT_SA0, m_sa0);
	DDX_Text(pDX, IDC_EDIT_SA_CEOF, m_sa_ceof);
	DDX_Text(pDX, IDC_EDIT_dp, m_drho);
}

BOOL CSetMisalignDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//	if(m_bMisalign6Param)
//	{
	GetDlgItem(IDC_EDIT_Tx)->EnableWindow(m_bMisalign6Param);
	GetDlgItem(IDC_EDIT_Ty)->EnableWindow(m_bMisalign6Param);
	GetDlgItem(IDC_EDIT_Tz)->EnableWindow(m_bMisalign6Param);

	GetDlgItem(IDC_EDIT_DX)->EnableWindow(m_bTranslationParam);
	GetDlgItem(IDC_EDIT_DY)->EnableWindow(m_bTranslationParam);
	GetDlgItem(IDC_EDIT_DZ)->EnableWindow(m_bTranslationParam);
	
	GetDlgItem(IDC_EDIT_SA0)->EnableWindow(m_bScanAngle0);
	GetDlgItem(IDC_EDIT_SA_CEOF)->EnableWindow(m_bScanAngle0);
//	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CSetMisalignDlg, CDialog)
//	ON_BN_CLICKED(_T("ButtonOK"), OnButtonOK)
//	ON_BN_CLICKED(_T("ButtonCancel"), OnButtonCancel)
ON_BN_CLICKED(IDC_RADIO_3Param, &CSetMisalignDlg::OnBnClickedRadio3param)
ON_BN_CLICKED(IDC_RADIO_6Param, &CSetMisalignDlg::OnBnClickedRadio6param)
ON_BN_CLICKED(IDC_CHECK_Semi3Param, &CSetMisalignDlg::OnBnClickedCheckSemi3param)
ON_EN_CHANGE(IDC_EDIT_Roll, &CSetMisalignDlg::OnEnChangeEditRoll)
ON_BN_CLICKED(IDC_CHECK_SA0, &CSetMisalignDlg::OnBnClickedCheckSa0)
END_MESSAGE_MAP()

// BEGIN_DHTML_EVENT_MAP(CSetMisalignDlg)
// 	
// 	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
// END_DHTML_EVENT_MAP()



// CSetMisalignDlg message handlers

//DEL HRESULT CSetMisalignDlg::OnButtonOK(IHTMLElement* /*pElement*/)
//DEL {
//DEL 	OnOK();
//DEL 	return S_OK;
//DEL }

//DEL HRESULT CSetMisalignDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
//DEL {
//DEL 	OnCancel();
//DEL 	return S_OK;
//DEL }

void CSetMisalignDlg::OnBnClickedRadio3param()
{
	m_bMisalign6Param=FALSE;

	GetDlgItem(IDC_EDIT_Tx)->EnableWindow(m_bMisalign6Param);
	GetDlgItem(IDC_EDIT_Ty)->EnableWindow(m_bMisalign6Param);
	GetDlgItem(IDC_EDIT_Tz)->EnableWindow(m_bMisalign6Param);
}


void CSetMisalignDlg::OnBnClickedRadio6param()
{
	m_bMisalign6Param=TRUE;
	
	GetDlgItem(IDC_EDIT_Tx)->EnableWindow(m_bMisalign6Param);
	GetDlgItem(IDC_EDIT_Ty)->EnableWindow(m_bMisalign6Param);
	GetDlgItem(IDC_EDIT_Tz)->EnableWindow(m_bMisalign6Param);
}

void CSetMisalignDlg::OnBnClickedCheckSemi3param()
{
	m_bTranslationParam=!m_bTranslationParam;

	GetDlgItem(IDC_EDIT_DX)->EnableWindow(m_bTranslationParam);
	GetDlgItem(IDC_EDIT_DY)->EnableWindow(m_bTranslationParam);
	GetDlgItem(IDC_EDIT_DZ)->EnableWindow(m_bTranslationParam);
}

void CSetMisalignDlg::OnEnChangeEditRoll()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CSetMisalignDlg::OnBnClickedCheckSa0()
{
	m_bScanAngle0=!m_bScanAngle0;
	GetDlgItem(IDC_EDIT_SA0)->EnableWindow(m_bScanAngle0);
	GetDlgItem(IDC_EDIT_SA_CEOF)->EnableWindow(m_bScanAngle0);
}
