// DlgAtnRasterize.cpp : implementation file
//

#include "stdafx.h"
#include "Alignment.h"
#include "DlgAtnRasterize.h"
#include "afxwin.h"


// CDlgAtnRasterize dialog

IMPLEMENT_DYNAMIC(CDlgAtnRasterize, CDialog)

CDlgAtnRasterize::CDlgAtnRasterize(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAtnRasterize::IDD, pParent)
	, m_selAttType(FALSE)
	, m_gridSize(0)
{

}

CDlgAtnRasterize::~CDlgAtnRasterize()
{
}

void CDlgAtnRasterize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_E, m_selAttType);
	DDX_Text(pDX, IDC_EDIT_GRIDSIZE, m_gridSize);
	DDX_Control(pDX, IDC_ATNLIST, m_wndAtnList);
}


BEGIN_MESSAGE_MAP(CDlgAtnRasterize, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_AddATN, &CDlgAtnRasterize::OnBnClickedButtonAddatn)
	ON_BN_CLICKED(IDOK, &CDlgAtnRasterize::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_GRIDSIZE, &CDlgAtnRasterize::OnEnChangeEditGridsize)
	ON_BN_CLICKED(IDC_RADIO_E, &CDlgAtnRasterize::OnBnClickedRadioE)
	ON_BN_CLICKED(IDC_RADIO_I, &CDlgAtnRasterize::OnBnClickedRadioI)
END_MESSAGE_MAP()


// CDlgAtnRasterize message handlers


BOOL CDlgAtnRasterize::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_selAttType = 0;

	m_wndAtnList.InsertColumn(0,"Image Name",LVCFMT_CENTER, 1024);
	m_wndAtnList.SetExtendedStyle(m_wndAtnList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgAtnRasterize::OnBnClickedButtonAddatn()
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

void CDlgAtnRasterize::AddImageTable(int startPos)
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

void CDlgAtnRasterize::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void CDlgAtnRasterize::OnEnChangeEditGridsize()
{
	UpdateData(TRUE);
}


void CDlgAtnRasterize::OnBnClickedRadioE()
{
	UpdateData(TRUE);
}


void CDlgAtnRasterize::OnBnClickedRadioI()
{
	UpdateData(TRUE);
}
