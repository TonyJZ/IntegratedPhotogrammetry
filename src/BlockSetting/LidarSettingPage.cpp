// LidarSettingPage.cpp : implementation file
//

#include "stdafx.h"
//#include "BlockSetting.h"
#include "LidarSettingPage.h"
#include "..\..\include\ALMPrjManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLidarSettingPage dialog


CLidarSettingPage::CLidarSettingPage(CWnd* pParent /*=NULL*/)
	: CPropertiesTabPage(CLidarSettingPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLidarSettingPage)
	m_bCopyLIDAR = FALSE;
	//}}AFX_DATA_INIT
}


void CLidarSettingPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLidarSettingPage)
	DDX_Control(pDX, IDC_LidarGRID, m_Grid);
	DDX_Check(pDX, IDC_CHECK_CopyLIDAR, m_bCopyLIDAR);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLidarSettingPage, CDialog)
	//{{AFX_MSG_MAP(CLidarSettingPage)
	ON_BN_CLICKED(IDC_BUTTON_CoordTrans, OnBUTTONCoordTrans)
	ON_BN_CLICKED(IDC_BUTTON_AddLIDAR, OnBUTTONAddLIDAR)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLidarSettingPage message handlers
void CLidarSettingPage::OnActivate()
{

}

void CLidarSettingPage::OnBUTTONCoordTrans() 
{
	// TODO: Add your control notification handler code here
	
}

void CLidarSettingPage::OnBUTTONAddLIDAR() 
{
	char sBuffer[10000];	//设置足够的缓冲区，保证能将选择的文件名全部保存
	sBuffer[0]=0;
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, \
	_T("Lidar Files (*.las)|*.las|All Files(*.*)|*.*||"));
	
	dlg.m_ofn.lpstrFile=sBuffer;
	dlg.m_ofn.nMaxFile=10000;
	
	if(dlg.DoModal()!=IDOK)
		return;
	
	
	CString	strName;
	CString	LidarName, name_tmp;
	iphLIDARList  *LidarList;
	LIDARINFO	LidarInfo;
	
	LidarList=m_pBlockInfo->GetLidarList();
	int	Num=LidarList->GetSize();
	
	POSITION pos=dlg.GetStartPosition();
	while(pos)   
	{   
		strName=dlg.GetNextPathName(pos);
		LidarName=strName;
		
		for(int i=0; i<LidarList->GetSize(); i++)
		{//不添加重复的文件
			name_tmp=(*LidarList)[i].strLidarName;
			if(LidarName == name_tmp)
			{
				goto Jump_This_File;
			}
		}
		
		LidarInfo.strLidarName=LidarName;
		LidarList->Add(LidarInfo);
		
Jump_This_File:
		;
	}
	
	AddLidarFile();
	
}

BOOL CLidarSettingPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(230, 230, 230));
	
	m_Grid.SetColumnCount(2);	//ClsID, check, color, CLsName
	//	m_Grid.SetRowCount(ClassGradation+1);
	m_Grid.SetFixedRowCount(1);
	//	m_Grid.SetFixedColumnCount(4);
	
	m_Grid.SetDefCellHeight(25);
	m_Grid.SetRowHeight(0, 25);
	m_Grid.SetColumnWidth(0, 80);	//id
	m_Grid.SetColumnWidth(1,350);	//image name
//	m_Grid.SetColumnWidth(2,60);	//camera
//	m_Grid.SetColumnWidth(3,60);	//pos
	
	CString	str;
	GV_ITEM Item;
	
	str=_T("ID");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 0;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	str=_T("文件名");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 1;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	
	AddLidarFile();
	//	m_Grid.SetListMode(TRUE);
	m_Grid.SetFixedRowSelection(TRUE);
	m_Grid.SetFixedColumnCount(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CLidarSettingPage::AddLidarFile()
{
	int	num, i, j, row;
	int pos;
	CString	str;
	GV_ITEM Item;
	CString LidarName;
	iphLIDARList *lidarList;	

	pos=m_Grid.GetRowCount();
	lidarList=m_pBlockInfo->GetLidarList();
	num=lidarList->GetSize();
	if(num>0)
		m_Grid.SetRowCount(num+1);
	//	else
	//		m_Grid.SetRowCount(10);
	for(i=0, j=0; i<num; i++, j++)
	{
		row=j+pos;
		
		str.Format("%d", row);
		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 0;
		Item.strText = str;
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(row,0, m_Grid.GetItemState(row,0) | GVIS_READONLY | GVIS_FIXED);
		
		LidarName=(*lidarList)[i].strLidarName;
		str=LidarName;
		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 1;
		Item.strText = str;
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(row,1, m_Grid.GetItemState(row,1) | GVIS_READONLY | GVIS_FIXED);
		
	}
}

