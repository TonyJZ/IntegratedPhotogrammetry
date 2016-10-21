// ATN2LASDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Alignment.h"
#include "ATN2LASDlg.h"
#include "afxdialogex.h"
#include "lidBase.h"
#include "orsGuiBase/orsIPropDlg.h"
// CATN2LASDlg dialog

IMPLEMENT_DYNAMIC(CATN2LASDlg, CDialog)

CATN2LASDlg::CATN2LASDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CATN2LASDlg::IDD, pParent)
{
	m_dp = 0.0;
	m_semi_dx = 0.0;
	m_semi_dy = 0.0;
	m_semi_dz = 0.0;
	m_misalign_heading = 0.0;
	m_misalign_pitch = 0.0;
	m_misalign_roll = 0.0;
	m_sac = 0.0;
	m_sa0 = 0.0;
	m_misalign_tx = 0.0;
	m_misalign_ty = 0.0;
	m_misalign_tz = 0.0;
	m_strATNName = _T("");
	m_strLasDir = _T("");

	m_bCheck_dp=FALSE;
	m_bCheck_sa0=FALSE;
	m_bCheck_SemiXYZ=FALSE;
	m_bMisalign_6Param=FALSE;
	m_bTP_ATN = FALSE;

	//默认是WGS84地理坐标
	m_srcWkt = "GEOGCS[\"WGS 84\", \
			   DATUM[\"WGS_1984\", \
			   SPHEROID[\"WGS 84\", 6378137,298.257223563, \
			   AUTHORITY[\"EPSG\",\"7030\"]], \
			   TOWGS84[0,0,0,0,0,0,0], \
			   AUTHORITY[\"EPSG\",\"6326\"]], \
			   PRIMEM[\"Greenwich\", 0, \
			   AUTHORITY[\"EPSG\",\"8901\"]], \
			   UNIT[\"degree\",0.0174532925199433, \
			   AUTHORITY[\"EPSG\",\"9108\"]], \
			   AUTHORITY[\"EPSG\",\"4326\"]]";
}

CATN2LASDlg::~CATN2LASDlg()
{
}

void CATN2LASDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_dp, m_bCheck_dp);
	DDX_Check(pDX, IDC_CHECK_SA0, m_bCheck_sa0);
	DDX_Check(pDX, IDC_CHECK_Semi3Param, m_bCheck_SemiXYZ);
	DDX_Text(pDX, IDC_EDIT_dp, m_dp);
	DDX_Text(pDX, IDC_EDIT_DX, m_semi_dx);
	DDX_Text(pDX, IDC_EDIT_DY, m_semi_dy);
	DDX_Text(pDX, IDC_EDIT_DZ, m_semi_dz);
	DDX_Text(pDX, IDC_EDIT_Heading, m_misalign_heading);
	DDX_Text(pDX, IDC_EDIT_Pitch, m_misalign_pitch);
	DDX_Text(pDX, IDC_EDIT_Roll, m_misalign_roll);
	DDX_Text(pDX, IDC_EDIT_SA_CEOF, m_sac);
	DDX_Text(pDX, IDC_EDIT_SA0, m_sa0);
	DDX_Text(pDX, IDC_EDIT_Tx, m_misalign_tx);
	DDX_Text(pDX, IDC_EDIT_Ty, m_misalign_ty);
	DDX_Text(pDX, IDC_EDIT_Tz, m_misalign_tz);
	DDX_Text(pDX, IDC_EDIT_ATN_Name, m_strATNName);
	DDX_Text(pDX, IDC_EDIT_LAS_DIR, m_strLasDir);
	//  DDX_Radio(pDX, IDC_RADIO_3Param, m_bMisalign_3Param);
//	DDX_Radio(pDX, IDC_RADIO_TPlaneATN, m_bTP_ATN);
	DDX_Radio(pDX, IDC_RADIO_3Param, m_bMisalign_6Param);
}


BEGIN_MESSAGE_MAP(CATN2LASDlg, CDialog)
	ON_BN_CLICKED(IDC_SetSrcProj, &CATN2LASDlg::OnBnClickedSetsrcproj)
	ON_BN_CLICKED(IDC_SetDstProj, &CATN2LASDlg::OnBnClickedSetdstproj)
	ON_BN_CLICKED(IDC_OpenATNFile, &CATN2LASDlg::OnBnClickedOpenatnfile)
	ON_BN_CLICKED(IDC_SetLasDir, &CATN2LASDlg::OnBnClickedSetlasdir)
	ON_BN_CLICKED(IDC_RADIO_3Param, &CATN2LASDlg::OnBnClickedRadio3param)
	ON_BN_CLICKED(IDC_RADIO_6Param, &CATN2LASDlg::OnBnClickedRadio6param)
	ON_BN_CLICKED(IDC_CHECK_Semi3Param, &CATN2LASDlg::OnBnClickedCheckSemi3param)
	ON_BN_CLICKED(IDC_CHECK_SA0, &CATN2LASDlg::OnBnClickedCheckSa0)
	ON_BN_CLICKED(IDC_CHECK_dp, &CATN2LASDlg::OnBnClickedCheckdp)
//	ON_BN_CLICKED(IDC_RADIO_TPlaneATN, &CATN2LASDlg::OnBnClickedRadioTplaneatn)
	ON_EN_CHANGE(IDC_EDIT_Roll, &CATN2LASDlg::OnChangeEditRoll)
	ON_EN_CHANGE(IDC_EDIT_Pitch, &CATN2LASDlg::OnChangeEditPitch)
	ON_EN_CHANGE(IDC_EDIT_Heading, &CATN2LASDlg::OnChangeEditHeading)
	ON_EN_CHANGE(IDC_EDIT_Tx, &CATN2LASDlg::OnChangeEditTx)
	ON_EN_CHANGE(IDC_EDIT_Ty, &CATN2LASDlg::OnChangeEditTy)
	ON_EN_CHANGE(IDC_EDIT_Tz, &CATN2LASDlg::OnChangeEditTz)
	ON_EN_CHANGE(IDC_EDIT_DX, &CATN2LASDlg::OnChangeEditDx)
	ON_EN_CHANGE(IDC_EDIT_DZ, &CATN2LASDlg::OnChangeEditDz)
	ON_EN_CHANGE(IDC_EDIT_DY, &CATN2LASDlg::OnChangeEditDy)
	ON_EN_CHANGE(IDC_EDIT_dp, &CATN2LASDlg::OnChangeEditDp)
	ON_EN_CHANGE(IDC_EDIT_SA0, &CATN2LASDlg::OnChangeEditSa0)
	ON_EN_CHANGE(IDC_EDIT_SA_CEOF, &CATN2LASDlg::OnChangeEditSaCeof)
END_MESSAGE_MAP()

BOOL CATN2LASDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_Tx)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Ty)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Tz)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_DX)->EnableWindow(m_bCheck_SemiXYZ);
	GetDlgItem(IDC_EDIT_DY)->EnableWindow(m_bCheck_SemiXYZ);
	GetDlgItem(IDC_EDIT_DZ)->EnableWindow(m_bCheck_SemiXYZ);

	GetDlgItem(IDC_EDIT_SA0)->EnableWindow(m_bCheck_sa0);
	GetDlgItem(IDC_EDIT_SA_CEOF)->EnableWindow(m_bCheck_sa0);

	GetDlgItem(IDC_EDIT_dp)->EnableWindow(m_bCheck_dp);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CATN2LASDlg message handlers
void CATN2LASDlg::OnBnClickedRadio3param()
{
	m_bMisalign_6Param=FALSE;

	GetDlgItem(IDC_EDIT_Tx)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Ty)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Tz)->EnableWindow(FALSE);
}


void CATN2LASDlg::OnBnClickedRadio6param()
{
	m_bMisalign_6Param=TRUE;

	GetDlgItem(IDC_EDIT_Tx)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_Ty)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_Tz)->EnableWindow(TRUE);
}


void CATN2LASDlg::OnBnClickedCheckSemi3param()
{
	m_bCheck_SemiXYZ=!m_bCheck_SemiXYZ;

	GetDlgItem(IDC_EDIT_DX)->EnableWindow(m_bCheck_SemiXYZ);
	GetDlgItem(IDC_EDIT_DY)->EnableWindow(m_bCheck_SemiXYZ);
	GetDlgItem(IDC_EDIT_DZ)->EnableWindow(m_bCheck_SemiXYZ);
}


void CATN2LASDlg::OnBnClickedCheckSa0()
{
	m_bCheck_sa0=!m_bCheck_sa0;
	GetDlgItem(IDC_EDIT_SA0)->EnableWindow(m_bCheck_sa0);
	GetDlgItem(IDC_EDIT_SA_CEOF)->EnableWindow(m_bCheck_sa0);
}


void CATN2LASDlg::OnBnClickedCheckdp()
{
	m_bCheck_dp = !m_bCheck_dp;
	GetDlgItem(IDC_EDIT_dp)->EnableWindow(m_bCheck_dp);
}

void CATN2LASDlg::OnBnClickedSetsrcproj()
{
	ref_ptr<orsIPropDlg> pDlg = ORS_CREATE_OBJECT( orsIPropDlg, "ors.extension.propDlg.hcs" );

	ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();

	pProp->addAttr("hcs", m_srcWkt );

	if( pDlg->runDlg( pProp.get(), "hcs", NULL ) )
	{
		pProp->getAttr("hcs", m_srcWkt );
	}
}


void CATN2LASDlg::OnBnClickedSetdstproj()
{
	ref_ptr<orsIPropDlg> pDlg = ORS_CREATE_OBJECT( orsIPropDlg, "ors.extension.propDlg.hcs" );

	ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();

	pProp->addAttr("hcs", m_targetWkt );

	if( pDlg->runDlg( pProp.get(), "hcs", NULL ) )
	{
		pProp->getAttr("hcs", m_targetWkt );
	}
}


void CATN2LASDlg::OnBnClickedOpenatnfile()
{
	char sBuffer[10000];	
	sBuffer[0]=0;
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, \
		_T("ATN Files (*.las)|*.las|All Files(*.*)|*.*||"));

	dlg.m_ofn.lpstrFile=sBuffer;
	dlg.m_ofn.nMaxFile=10000;

	if(dlg.DoModal()!=IDOK)
		return;

	m_strATNName = dlg.GetPathName();

	m_atnNameVec.RemoveAll();

	CString atnName, name_tmp;
	POSITION pos=dlg.GetStartPosition();
	while(pos)   
	{   
		atnName=dlg.GetNextPathName(pos);

		m_atnNameVec.Add(atnName);	

	}

	UpdateData(FALSE);
}


void CATN2LASDlg::OnBnClickedSetlasdir()
{
	if(!_SelectFolderDialog("设置iop输出目录...", &m_strLasDir, NULL))
		return;
	UpdateData(FALSE);
}

void CATN2LASDlg::OnBnClickedRadioTplaneatn()
{
	m_bTP_ATN = !m_bTP_ATN;
}


void CATN2LASDlg::OnChangeEditRoll()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditPitch()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditHeading()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditTx()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditTy()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditTz()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditDx()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditDz()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditDy()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditDp()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditSa0()
{
	UpdateData(TRUE);
}


void CATN2LASDlg::OnChangeEditSaCeof()
{
	UpdateData(TRUE);
}
