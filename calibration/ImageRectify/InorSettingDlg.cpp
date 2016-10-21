// InorSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageRectify.h"
#include "InorSettingDlg.h"
#include "SetCameraDlg.h"
#include "lidBase.h"
#include "DPSDataType.h"

#include "Calib_Camera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInorSettingDlg dialog


CInorSettingDlg::CInorSettingDlg(std::vector<orsString>	*srcImgVec, _iphCamera *camera, CWnd* pParent /*=NULL*/)
	: CDialog(CInorSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInorSettingDlg)
	m_strOutputDir = _T("");
	//}}AFX_DATA_INIT
	m_srcImgVec=srcImgVec;
	m_camera=camera;
}


void CInorSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInorSettingDlg)
	DDX_Text(pDX, IDC_OutputDir, m_strOutputDir);
	DDX_Control(pDX, IDC_ImgGRID, m_Grid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInorSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CInorSettingDlg)
	ON_BN_CLICKED(IDC_AddImage, OnAddImage)
	ON_BN_CLICKED(IDC_LoadCamera, OnLoadCamera)
	ON_BN_CLICKED(IDC_SetCamera, OnSetCamera)
	ON_BN_CLICKED(IDC_SetOutputDir, OnSetOutputDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInorSettingDlg message handlers

BOOL CInorSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(230, 230, 230));
	
	m_Grid.SetColumnCount(2);	//ClsID, check, color, CLsName
	//	m_Grid.SetRowCount(ClassGradation+1);
	m_Grid.SetFixedRowCount(1);
	//	m_Grid.SetFixedColumnCount(4);
	
	m_Grid.SetDefCellHeight(25);
	m_Grid.SetRowHeight(0, 25);
	m_Grid.SetColumnWidth(0, 64);	//id
	m_Grid.SetColumnWidth(1, 1024);
// 	m_Grid.SetColumnWidth(1,230);	//image name
// 	m_Grid.SetColumnWidth(2,60);	//camera
// 	m_Grid.SetColumnWidth(3,60);	//pos
// 	
	CString	str;
	GV_ITEM Item;
	
	str="像片号";
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 0;
	Item.strText = str;
	m_Grid.SetItem(&Item);

	str="影像名";
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 1;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	
	m_Grid.SetFixedRowSelection(TRUE);
//	m_Grid.SetFixedColumnCount(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInorSettingDlg::OnAddImage() 
{
	char sBuffer[10000];	//设置足够的缓冲区，保证能将选择的文件名全部保存
	sBuffer[0]=0;
	CFileDialog	dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT, \
	_T("Image Files (*.tif)|*.tif|(*.tiff)|*.tiff|(*.jpg)|*.jpg|(*.jpeg)|*.jpeg| \
	(*.img)|*.img|(*.bmp)|*.bmp|All Files(*.*)|*.*||"));
	
	dlg.m_ofn.lpstrFile=sBuffer;
	dlg.m_ofn.nMaxFile=10000;
	
	if(dlg.DoModal()!=IDOK)
		return;
	
	
	CString	strImgName;
	orsString	imgName, name_tmp;
	std::vector<orsString>::iterator itr;
	int	imgNum=m_srcImgVec->size();

	POSITION pos=dlg.GetStartPosition();
	while(pos)   
	{   
		strImgName=dlg.GetNextPathName(pos);
		imgName=strImgName.GetBuffer(0);
		
		itr=m_srcImgVec->begin();
		while(itr!=m_srcImgVec->end())
		{
			if(imgName == *(itr))
			{
				goto Jump_This_File;
			}

			itr++;
		}
		m_srcImgVec->push_back(imgName);	
		
Jump_This_File:
		;
	}
	
	AddImageTable(imgNum);
		
}

void CInorSettingDlg::AddImageTable(int startPos)
{
	int	num, i, j, row;
	int pos;
	CString	str;
	GV_ITEM Item;
	CString	imgName;
	
	ASSERT(startPos>=0);
	if(startPos<0)
		return;
	
	pos=m_Grid.GetRowCount();
	
	num=m_srcImgVec->size();
	if(num>0)
		m_Grid.SetRowCount(num+1);
	//	else
	//		m_Grid.SetRowCount(10);
	for(i=startPos, j=0; i<num; i++, j++)
	{
		row=j+pos;
		
		str.Format("%d", row);
		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 0;
		Item.strText = str;
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(row,0, m_Grid.GetItemState(row,0) | GVIS_READONLY | GVIS_FIXED);
		
		
		imgName=(*m_srcImgVec)[i];
		str=imgName;
		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 1;
		Item.strText = str;
		m_Grid.SetItem(&Item);
//		m_Grid.SetItemState(row,1, m_Grid.GetItemState(row,1) | GVIS_READONLY | GVIS_FIXED);
	}

}

void CInorSettingDlg::OnLoadCamera() 
{
	CFileDialog dlg( TRUE,_T(".las"),NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,
		"像机文件(*.cam)|*.cam|(*.txt)|*.txt|(*.*)|*.*||");

	if(dlg.DoModal()!=IDOK)
		return;

	CString strCamName=dlg.GetPathName();
	
//	ReadCamera( strCamName.GetBuffer(0), m_camera );

	m_camera->ReadCameraFile(strCamName.GetBuffer(0));
}

void CInorSettingDlg::OnSetCamera() 
{
	CSetCameraDlg dlg;

	if(dlg.DoModal()!=IDOK)
		return;

	m_camera->m_x0=dlg.m_x0;
	m_camera->m_y0=dlg.m_y0;
	m_camera->m_f=dlg.m_f;
	m_camera->m_pixelX=dlg.m_pixelX;
	m_camera->m_pixelY=dlg.m_pixelY;
	m_camera->m_k1=dlg.m_k1;
	m_camera->m_k2=dlg.m_k2;
	m_camera->m_k3=dlg.m_k3;
	m_camera->m_p1=dlg.m_p1;
	m_camera->m_p2=dlg.m_p2;
}

void CInorSettingDlg::OnSetOutputDir() 
{
	CString	strFilt;
	
	if(_SelectFolderDialog("设置输出目录...",&strFilt))
		m_strOutputDir = strFilt;
	
	UpdateData(FALSE);
}
