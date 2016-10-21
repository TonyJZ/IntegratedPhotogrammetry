// DlgATN2UTM.cpp : implementation file
//

#include "stdafx.h"
#include "Alignment.h"
#include "DlgATN2UTM.h"
#include "afxdialogex.h"
#include "lidBase.h"
#include "orsGuiBase/orsIPropDlg.h"

// CDlgATN2UTM dialog

IMPLEMENT_DYNAMIC(CDlgATN2UTM, CDialog)

CDlgATN2UTM::CDlgATN2UTM(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgATN2UTM::IDD, pParent)
{

	m_strOutDir = _T("");
//	m_centralMeridian = 0.0;
}

CDlgATN2UTM::~CDlgATN2UTM()
{
}

void CDlgATN2UTM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Dir, m_strOutDir);
	//  DDX_Control(pDX, IDC_EDIT_CentralMeridian, m_centralMeridian);
//	DDX_Text(pDX, IDC_EDIT_CentralMeridian, m_centralMeridian);
	DDX_Control(pDX, IDC_ATNLIST, m_wndAtnList);
}


BEGIN_MESSAGE_MAP(CDlgATN2UTM, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_AddATN, &CDlgATN2UTM::OnBnClickedButtonAddatn)
	ON_BN_CLICKED(IDC_BUTTON_DstProj, &CDlgATN2UTM::OnBnClickedButtonDstproj)
	ON_BN_CLICKED(IDC_BUTTON_OutputDir, &CDlgATN2UTM::OnBnClickedButtonOutputdir)
//	ON_EN_CHANGE(IDC_EDIT_CentralMeridian, &CDlgATN2UTM::OnChangeEditCentralmeridian)
END_MESSAGE_MAP()


// CDlgATN2UTM message handlers
void CDlgATN2UTM::AddImageTable(int startPos)
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

void CDlgATN2UTM::OnBnClickedButtonAddatn()
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


void CDlgATN2UTM::OnBnClickedButtonDstproj()
{
	ref_ptr<orsIPropDlg> pDlg = ORS_CREATE_OBJECT( orsIPropDlg, "ors.extension.propDlg.hcs" );

	ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();

	pProp->addAttr("hcs", m_targetWkt );

	if( pDlg->runDlg( pProp.get(), "hcs", NULL ) )
	{
		pProp->getAttr("hcs", m_targetWkt );
	}
}


void CDlgATN2UTM::OnBnClickedButtonOutputdir()
{
	if(!_SelectFolderDialog("…Ë÷√iop ‰≥ˆƒø¬º...", &m_strOutDir, NULL))
		return;
	UpdateData(FALSE);
}


BOOL CDlgATN2UTM::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_wndAtnList.InsertColumn(0,"Image Name",LVCFMT_CENTER, 1024);
	m_wndAtnList.SetExtendedStyle(m_wndAtnList.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


// void CDlgATN2UTM::OnChangeEditCentralmeridian()
// {
// 	UpdateData(TRUE);
// }
