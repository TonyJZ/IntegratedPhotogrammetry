// DlgLidCorrectYuan.cpp : implementation file
//

#include "stdafx.h"
#include "Alignment.h"
#include "DlgLidCorrectYuan.h"
#include "afxdialogex.h"

#include "lidBase.h"

// CDlgLidCorrectYuan dialog

IMPLEMENT_DYNAMIC(CDlgLidCorrectYuan, CDialog)

CDlgLidCorrectYuan::CDlgLidCorrectYuan(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLidCorrectYuan::IDD, pParent)
	, m_strCalibFile(_T(""))
	, m_strRawLasFile(_T(""))
	, m_strOutputDir(_T(""))
{

}

CDlgLidCorrectYuan::~CDlgLidCorrectYuan()
{
}

void CDlgLidCorrectYuan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CPFile_Name, m_strCalibFile);
	DDX_Text(pDX, IDC_EDIT_RAW_FILE_Name, m_strRawLasFile);
	DDX_Text(pDX, IDC_EDIT_LAS_DIR, m_strOutputDir);
}


BEGIN_MESSAGE_MAP(CDlgLidCorrectYuan, CDialog)
	ON_BN_CLICKED(IDC_OpenCPFile, &CDlgLidCorrectYuan::OnBnClickedOpencpfile)
	ON_BN_CLICKED(IDC_OpenRawLasFile, &CDlgLidCorrectYuan::OnBnClickedOpenrawlasfile)
	ON_BN_CLICKED(IDC_SetLasDir, &CDlgLidCorrectYuan::OnBnClickedSetlasdir)
END_MESSAGE_MAP()


// CDlgLidCorrectYuan message handlers


void CDlgLidCorrectYuan::OnBnClickedOpencpfile()
{
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR , \
		_T("calib param file (*.cp)|*.cp|All Files(*.*)|*.*||"));

	
	if(dlg.DoModal()!=IDOK)
		return;

	m_strCalibFile = dlg.GetPathName();


	UpdateData(FALSE);
}


void CDlgLidCorrectYuan::OnBnClickedOpenrawlasfile()
{
	char sBuffer[10000];	
	sBuffer[0]=0;
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, \
		_T("las Files (*.las)|*.las|All Files(*.*)|*.*||"));

	dlg.m_ofn.lpstrFile=sBuffer;
	dlg.m_ofn.nMaxFile=10000;

	if(dlg.DoModal()!=IDOK)
		return;

	m_strRawLasFile = dlg.GetPathName();

	m_lasNameVec.RemoveAll();

	CString lasName;
	POSITION pos=dlg.GetStartPosition();
	while(pos)   
	{   
		lasName=dlg.GetNextPathName(pos);

		m_lasNameVec.Add(lasName);	

	}

	UpdateData(FALSE);
}


void CDlgLidCorrectYuan::OnBnClickedSetlasdir()
{
	if(!_SelectFolderDialog("…Ë÷√iop ‰≥ˆƒø¬º...", &m_strOutputDir, NULL))
		return;
	UpdateData(FALSE);
}
