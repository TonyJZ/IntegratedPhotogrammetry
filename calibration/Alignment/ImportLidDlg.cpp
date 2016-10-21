// ImportLidDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Alignment.h"
#include "ImportLidDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportLidDlg dialog


CImportLidDlg::CImportLidDlg(int FileSetID, CWnd* pParent /*=NULL*/)
	: CDialog(CImportLidDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportLidDlg)
	m_strFileSetName = _T("");
	m_strImgName = _T("");
	m_strAtnName = _T("");
	//}}AFX_DATA_INIT

	m_strFileSetName.Format("Strip_%d", FileSetID);
	m_strTrajName = _T("");
	m_dAFH = 0.0;
}


void CImportLidDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportLidDlg)
	DDX_Text(pDX, IDC_EDIT_FileSetName, m_strFileSetName);
	DDX_Text(pDX, IDC_EDIT_ImgName, m_strImgName);
	DDX_Text(pDX, IDC_EDIT_ATNName, m_strAtnName);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_ImgLIST, m_wndImgList);
	DDX_Text(pDX, IDC_EDIT_Trajectory, m_strTrajName);
	DDX_Text(pDX, IDC_EDIT_AFH, m_dAFH);
}


BEGIN_MESSAGE_MAP(CImportLidDlg, CDialog)
	//{{AFX_MSG_MAP(CImportLidDlg)
	ON_BN_CLICKED(IDC_BUTTON_AtnName, OnBUTTONAtnName)
	ON_BN_CLICKED(IDC_BUTTON_ImgName, OnBUTTONImgName)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_AddImg, &CImportLidDlg::OnBnClickedButtonAddimg)
	ON_BN_CLICKED(IDC_BUTTON_Trajectory, &CImportLidDlg::OnBnClickedButtonTrajectory)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportLidDlg message handlers

BOOL CImportLidDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndImgList.InsertColumn(0,"Image Name",LVCFMT_CENTER, 1024);
	m_wndImgList.SetExtendedStyle(m_wndImgList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImportLidDlg::OnBUTTONAtnName() 
{
	CFileDialog  dlg(TRUE,"_ATN",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"_ATN(*.LAS)|*.LAS|(*.*)|*.*||",NULL);
	
	if(dlg.DoModal()!=IDOK)
		return;
	
	m_strAtnName=dlg.GetPathName();

	UpdateData(FALSE);
}

void CImportLidDlg::OnBUTTONImgName() 
{
	CFileDialog  dlg(TRUE,"Image",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.tif)|*.tif|(*.img)|*.img|(*.*)|*.*||",NULL);
	
	if(dlg.DoModal()!=IDOK)
		return;
	
	m_strImgName=dlg.GetPathName();
	
	UpdateData(FALSE);
}

void CImportLidDlg::OnBnClickedButtonAddimg()
{
	char sBuffer[10000];	
	sBuffer[0]=0;
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, \
		_T("Image Files (*.tif)|*.tif|(*.tiff)|*.tiff|(*.jpg)|*.jpg|(*.jpeg)|*.jpeg| \
		   (*.img)|*.img|(*.bmp)|*.bmp|All Files(*.*)|*.*||"));

	dlg.m_ofn.lpstrFile=sBuffer;
	dlg.m_ofn.nMaxFile=10000;

	if(dlg.DoModal()!=IDOK)
		return;

	int	imgNum=m_imgNameVec.GetSize();
	
	CString strImgName, name_tmp;
	POSITION pos=dlg.GetStartPosition();
	while(pos)   
	{   
		strImgName=dlg.GetNextPathName(pos);

		for(int i=0; i<imgNum; i++)
		{//
			name_tmp=m_imgNameVec[i];
			if(strImgName == name_tmp)
			{
				goto Jump_This_File;
			}
		}

		m_imgNameVec.Add(strImgName);	

Jump_This_File:
		;
	}

	AddImageTable(imgNum);
}

void CImportLidDlg::AddImageTable(int startPos)
{
	int	num, i, j, row;
	int pos;
//	CString	str;
	CString	imgName;

	ASSERT(startPos>=0);
	if(startPos<0)
		return;

	LVITEM	lvi;

	pos=m_wndImgList.GetItemCount();
	
	num=m_imgNameVec.GetSize();
	for(i=startPos, j=0; i<num; i++, j++)
	{
		row=j+pos;

		imgName=m_imgNameVec[i];
		lvi.mask=LVIF_TEXT;
		lvi.iItem=row;	lvi.iSubItem=0;
		lvi.pszText=imgName.GetBuffer(0);
		m_wndImgList.InsertItem(&lvi);
	}
	UpdateData(FALSE);
}


void CImportLidDlg::OnBnClickedButtonTrajectory()
{
	CFileDialog  dlg(TRUE,"trj",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"trj(*.trj)|*.trj|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	m_strTrajName=dlg.GetPathName();

	UpdateData(FALSE);

}



void CImportLidDlg::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}
