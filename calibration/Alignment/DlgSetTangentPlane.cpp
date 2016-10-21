// DlgSetTangentPlane.cpp : implementation file
//

#include "stdafx.h"
#include "Alignment.h"
#include "DlgSetTangentPlane.h"
#include "afxdialogex.h"
#include "lidBase.h"

// CDlgSetTangentPlane dialog

IMPLEMENT_DYNAMIC(CDlgSetTangentPlane, CDialog)

CDlgSetTangentPlane::CDlgSetTangentPlane(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetTangentPlane::IDD, pParent)
	, m_longitude(0)
	, m_latitude(0)
	, m_strOutputDir(_T(""))
{

}

CDlgSetTangentPlane::~CDlgSetTangentPlane()
{
}

void CDlgSetTangentPlane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_lon, m_longitude);
	DDX_Text(pDX, IDC_EDIT_lat, m_latitude);
	DDX_Control(pDX, IDC_ATNLIST, m_wndAtnList);
	DDX_Text(pDX, IDC_EDIT_Dir, m_strOutputDir);
}


BEGIN_MESSAGE_MAP(CDlgSetTangentPlane, CDialog)
	ON_EN_CHANGE(IDC_EDIT_lat, &CDlgSetTangentPlane::OnEnChangeEditlat)
	ON_EN_CHANGE(IDC_EDIT_lon, &CDlgSetTangentPlane::OnEnChangeEditlon)
	ON_BN_CLICKED(IDC_BUTTON_AddATN, &CDlgSetTangentPlane::OnBnClickedButtonAddatn)
	ON_BN_CLICKED(IDC_BUTTON_OutputDir, &CDlgSetTangentPlane::OnBnClickedButtonOutputdir)
END_MESSAGE_MAP()


// CDlgSetTangentPlane message handlers


BOOL CDlgSetTangentPlane::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndAtnList.InsertColumn(0,"Image Name",LVCFMT_CENTER, 1024);
	m_wndAtnList.SetExtendedStyle(m_wndAtnList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSetTangentPlane::OnEnChangeEditlat()
{
	UpdateData(TRUE);
}


void CDlgSetTangentPlane::OnEnChangeEditlon()
{
	UpdateData(TRUE);
}


void CDlgSetTangentPlane::OnBnClickedButtonAddatn()
{
	char sBuffer[10000];	
	sBuffer[0]=0;
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, \
		_T("ATN Files (*.las)|*.las|All Files(*.*)|*.*||"));

	dlg.m_ofn.lpstrFile=sBuffer;
	dlg.m_ofn.nMaxFile=10000;

	if(dlg.DoModal()!=IDOK)
		return;

	int	imgNum=m_atnNameVec.GetSize();

	CString strImgName, name_tmp;
	POSITION pos=dlg.GetStartPosition();
	while(pos)   
	{   
		strImgName=dlg.GetNextPathName(pos);

		for(int i=0; i<imgNum; i++)
		{//
			name_tmp=m_atnNameVec[i];
			if(strImgName == name_tmp)
			{
				goto Jump_This_File;
			}
		}

		m_atnNameVec.Add(strImgName);	

Jump_This_File:
		;
	}

	AddImageTable(imgNum);
}

void CDlgSetTangentPlane::AddImageTable(int startPos)
{
	int	num, i, j, row;
	int pos;
	//	CString	str;
	CString	imgName;

	ASSERT(startPos>=0);
	if(startPos<0)
		return;

	LVITEM	lvi;

	pos=m_wndAtnList.GetItemCount();

	num=m_atnNameVec.GetSize();
	for(i=startPos, j=0; i<num; i++, j++)
	{
		row=j+pos;

		imgName=m_atnNameVec[i];
		lvi.mask=LVIF_TEXT;
		lvi.iItem=row;	lvi.iSubItem=0;
		lvi.pszText=imgName.GetBuffer(0);
		m_wndAtnList.InsertItem(&lvi);
	}
	UpdateData(FALSE);
}

void CDlgSetTangentPlane::OnBnClickedButtonOutputdir()
{
//	CString strDir;
	if(!_SelectFolderDialog("ÉèÖÃiopÊä³öÄ¿Â¼...", &m_strOutputDir, NULL))
		return;
	UpdateData(FALSE);
}
