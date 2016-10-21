// ImgSettingPage.cpp : implementation file
//

#include "stdafx.h"
//#include "BlockSetting.h"
#include "ImgSettingPage.h"
#include "CameraSettingDialog.h"
#include "InputPosDialog.h"
#include "BatchPosInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgSettingPage dialog


CImgSettingPage::CImgSettingPage(CWnd* pParent /*=NULL*/)
	: CPropertiesTabPage(CImgSettingPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImgSettingPage)
	m_pSelRowBuf=new int[1000];
	m_SelBufLen=1000;
	m_bCopyImg = FALSE;
	//}}AFX_DATA_INIT
}

CImgSettingPage::~CImgSettingPage()
{
	if(m_pSelRowBuf)
	{
		delete[] m_pSelRowBuf;
		m_pSelRowBuf=0;
	}
}

void CImgSettingPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImgSettingPage)
	DDX_Control(pDX, IDC_ImgGRID, m_Grid);
	DDX_Check(pDX, IDC_CHECK_CopyImg, m_bCopyImg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImgSettingPage, CDialog)
	//{{AFX_MSG_MAP(CImgSettingPage)
	ON_BN_CLICKED(IDC_AddImage, OnAddImage)
	ON_BN_CLICKED(IDC_AddCamera, OnAddCamera)
	ON_BN_CLICKED(IDC_AddPOS, OnAddPOS)
	ON_BN_CLICKED(IDC_CHECK_CopyImg, OnCHECKCopyImg)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CLICK, IDC_ImgGRID, OnGridClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgSettingPage message handlers

void CImgSettingPage::OnActivate()
{
	
}

BOOL CImgSettingPage::OnInitDialog() 
{
	CPropertiesTabPage::OnInitDialog();
	
	GetDlgItem(IDC_AddImage)->EnableWindow(TRUE);

	if(m_pBlockInfo->GetImgList()->GetSize()==0)
	{
		GetDlgItem(IDC_AddCamera)->EnableWindow(FALSE);
		GetDlgItem(IDC_AddPOS)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_AddCamera)->EnableWindow(TRUE);
		GetDlgItem(IDC_AddPOS)->EnableWindow(TRUE);
	}
	

	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(230, 230, 230));
	
	m_Grid.SetColumnCount(4);	//ClsID, check, color, CLsName
//	m_Grid.SetRowCount(ClassGradation+1);
	m_Grid.SetFixedRowCount(1);
//	m_Grid.SetFixedColumnCount(4);

	m_Grid.SetDefCellHeight(25);
	m_Grid.SetRowHeight(0, 25);
	m_Grid.SetColumnWidth(0, 80);	//id
	m_Grid.SetColumnWidth(1,230);	//image name
	m_Grid.SetColumnWidth(2,60);	//camera
	m_Grid.SetColumnWidth(3,60);	//pos

	CString	str;
	GV_ITEM Item;

	str.LoadString(IDS_Image_ID);
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 0;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	str.LoadString(IDS_Image_Name);
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 1;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	str.LoadString(IDS_Image_Camera);
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 2;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	str.LoadString(IDS_Image_POS);
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 3;
	Item.strText = str;
	m_Grid.SetItem(&Item);

	AddImageTable(0);
//	m_Grid.SetListMode(TRUE);
	m_Grid.SetFixedRowSelection(TRUE);
	m_Grid.SetFixedColumnCount(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImgSettingPage::AddImageTable(int startPos)
{
	int	num, i, j, row;
	int pos;
	CString	str;
	GV_ITEM Item;
	iphImgList	*imgList;
	iphImgINFO imgInfo;
	CString	imgName;

	ASSERT(startPos>=0);
	if(startPos<0)
		return;

	pos=m_Grid.GetRowCount();
	imgList=m_pBlockInfo->GetImgList();
	num=imgList->GetSize();
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
		
		imgInfo=(*imgList)[i];
		//imgName=m_pBlockInfo->imgList[i].imgName;
		imgName=imgInfo.strImgName;
		str=imgName;
		Item.mask = GVIF_TEXT;
		Item.row = row;
		Item.col = 1;
		Item.strText = str;
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(row,1, m_Grid.GetItemState(row,1) | GVIS_READONLY | GVIS_FIXED);

		if(imgInfo.camera.m_bInor)
		{
			Item.crBkClr = RGB(0,255,0);   // 背景颜色 or - m_Grid.SetItemBkColour(row, col, clr);
			//Item.crFgClr = RGB(0,0,0);    // 字符颜色 or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
			
		}
		else
		{
			Item.crBkClr = RGB(255,0,0);
		}
		Item.mask    |= (GVIF_BKCLR|GVIF_FGCLR);
		Item.row = row;
		Item.col = 2;
		str="\0";
		Item.strText = str;
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(row,2, m_Grid.GetItemState(row,2) | GVIS_READONLY);
		

		if(imgInfo.camera.m_bExor)
		{
			Item.crBkClr = RGB(0,255,0);
		}
		else
		{
			Item.crBkClr = RGB(255,0,0);
		}
		Item.mask    |= (GVIF_BKCLR|GVIF_FGCLR);
		Item.row = row;
		Item.col = 3;
		str="\0";
		Item.strText = str;
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(row,3, m_Grid.GetItemState(row,3) | GVIS_READONLY);


	}
	
	
}

void CImgSettingPage::OnAddImage()
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

	GetDlgItem(IDC_AddCamera)->EnableWindow(TRUE);
	GetDlgItem(IDC_AddPOS)->EnableWindow(TRUE);
	
	CString	strImgName;
	CString	imgName, name_tmp;
	iphImgINFO imgInfo;
	iphImgList *imgList;
	

	imgList=m_pBlockInfo->GetImgList();
	int	imgNum=imgList->GetSize();

	POSITION pos=dlg.GetStartPosition();
	while(pos)   
	{   
		strImgName=dlg.GetNextPathName(pos);
		imgName=strImgName;

		for(int i=0; i<imgList->GetSize(); i++)
		{//不添加重复的文件
			name_tmp=(*imgList)[i].strImgName;
			if(imgName == name_tmp)
			{
				goto Jump_This_File;
			}
		}

		imgInfo.strImgName=imgName; //这里记录的是全路径文件名
//		imgInfo.useful=true;		//默认影像都能定向，并得到自由网
		
		imgList->Add(imgInfo);	

Jump_This_File:
		;
	}

	AddImageTable(imgNum);
	
}

void CImgSettingPage::OnAddCamera()
{//按钮方式：对全部影像设置像机参数	2010.4.13
	
	CCameraSettingDialog	dlg(this);

	dlg.m_Option=0;		//默认对全部像片

	if(dlg.DoModal()!=IDOK)
		return;

	int	imgNum;
	int i, idx;
	int selNum;
	iphImgList	*imgList;
	CString	imgName;
	CString	cmrName;
	int pos;

	if(0==dlg.m_Option)
	{//应用于全部像片
		imgList=m_pBlockInfo->GetImgList();
		imgNum=imgList->GetSize();
		for(i=0; i<imgNum; i++)
		{
			(*imgList)[i].camera.m_x0=dlg.m_x0;
			(*imgList)[i].camera.m_y0=dlg.m_y0;
			(*imgList)[i].camera.m_f=dlg.m_f;
			(*imgList)[i].camera.m_k1=dlg.m_k1;
			(*imgList)[i].camera.m_k2=dlg.m_k2;
			(*imgList)[i].camera.m_k3=dlg.m_k3;
			(*imgList)[i].camera.m_p1=dlg.m_p1;
			(*imgList)[i].camera.m_p2=dlg.m_p2;
			(*imgList)[i].camera.m_pixelWid=dlg.m_PixelWid;
			(*imgList)[i].camera.m_pixelHei=dlg.m_PixelHei;
			(*imgList)[i].camera.m_imgWid=dlg.m_ImgWid;
			(*imgList)[i].camera.m_imgHei=dlg.m_ImgHei;

			imgName=(*imgList)[i].strImgName;
			pos=imgName.ReverseFind('.');
			cmrName=imgName.Left(pos);
//			cmrName+=".cmr";		//像机文件后缀
//			sprintf((*imgList)[i].camera.m_pCameraFileName, cmrName.GetBuffer(256));

			(*imgList)[i].camera.m_bInor=true;
		}
		
	}
// 	else if(1==dlg.m_Option)
// 	{//应用与选中像片
// 		selNum=GetSelectedRow();
// 		imgList=m_pBlockInfo->GetImgList();
// 		for(i=0; i<selNum; i++)
// 		{//行号=索引号+1
// 			idx=m_pSelRowBuf[i]-1;
// 			(*imgList)[idx].camera.SetInorParameter(dlg.m_x0, dlg.m_y0, dlg.m_f, 
// 				dlg.m_k1, dlg.m_k2, dlg.m_k3, dlg.m_p1, dlg.m_p2);
// 
// 			imgName=(*imgList)[idx].strImgName;
// 			pos=imgName.ReverseFind('.');
// 			cmrName=imgName.Left(pos);
// 			cmrName+=".cmr";		//像机文件后缀
// 			sprintf((*imgList)[idx].camera.m_pCameraFileName, cmrName.GetBuffer(256));
// 		}
// 	}

	UpdataImageState();
}

void CImgSettingPage::OnAddPOS()
{
	CBatchPosInputDlg	dlg(m_pBlockInfo);

	if(dlg.DoModal()!=IDOK)
		return;
	
	UpdataImageState();
// 	int imgNum, i;
// 	imgNum=m_pBlockInfo->imgList.size();
// 	for(i=0; i<imgNum; i++)
// 	{
// 		m_pBlockInfo->imgList[i].camera.SetExorParameter(dlg.m_Xs, dlg.m_Ys, dlg.m_Zs, 
// 			dlg.m_Phi, dlg.m_Omega, dlg.m_Kappa);
// 		m_pBlockInfo->imgList[i].camera.SetCoordinateSystem(dlg.m_strHorizontalDatum.GetBuffer(128), (LPSTR)(LPCTSTR)(dlg.m_strVerticalDatum),
// 			dlg.m_PosUnitH, dlg.m_PosUnitV);
// 		m_pBlockInfo->imgList[i].camera.SetRotateSystem(dlg.m_RotateSys, dlg.m_AngleUnit);
// 	}
	
	
}

void CImgSettingPage::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	CGridCellBase* pCell=NULL;
//	BOOL	bCheck;
	
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if(pItem->iRow<1 || pItem->iRow>m_Grid.GetRowCount())
		return;
	
	int	imgNum;
	int i, idx;
	int selNum;
	iphImgList	*imgList;
	CString	imgName;
	CString	cmrName;
	int pos;
//	double x0, y0, f, k1, k2, k3, p1, p2;
//	double pixelX, pixelY;


	imgList=m_pBlockInfo->GetImgList();
	if(pItem->iColumn==2)
	{//set camera
		//		CColorDialog	dlg;
		CCameraSettingDialog  dlg;
		
		dlg.m_Option=1;

		if(1==dlg.m_Option)
		{//应用与选中像片
			selNum=GetSelectedRow();
			ASSERT(selNum==1);
			idx=m_pSelRowBuf[0]-1;
			
			dlg.m_x0=(*imgList)[idx].camera.m_x0;
			dlg.m_y0=(*imgList)[idx].camera.m_y0;
			dlg.m_f=(*imgList)[idx].camera.m_f;
			dlg.m_k1=(*imgList)[idx].camera.m_k1;
			dlg.m_k2=(*imgList)[idx].camera.m_k2;
			dlg.m_k3=(*imgList)[idx].camera.m_k3;
			dlg.m_p1=(*imgList)[idx].camera.m_p1;
			dlg.m_p2=(*imgList)[idx].camera.m_p2;
			dlg.m_PixelWid=(*imgList)[idx].camera.m_pixelWid;
			dlg.m_PixelHei=(*imgList)[idx].camera.m_pixelHei;
			dlg.m_ImgWid=(*imgList)[idx].camera.m_imgWid;
			dlg.m_ImgHei=(*imgList)[idx].camera.m_imgHei;

			if(dlg.DoModal()!=IDOK)
				return;

//			for(i=0; i<selNum; i++)
//			{//行号=索引号+1
//			idx=m_pSelRowBuf[i]-1;
			(*imgList)[idx].camera.m_x0=dlg.m_x0;
			(*imgList)[idx].camera.m_y0=dlg.m_y0;
			(*imgList)[idx].camera.m_f=dlg.m_f;
			(*imgList)[idx].camera.m_k1=dlg.m_k1;
			(*imgList)[idx].camera.m_k2=dlg.m_k2;
			(*imgList)[idx].camera.m_k3=dlg.m_k3;
			(*imgList)[idx].camera.m_p1=dlg.m_p1;
			(*imgList)[idx].camera.m_p2=dlg.m_p2;
			(*imgList)[idx].camera.m_pixelWid=dlg.m_PixelWid;
			(*imgList)[idx].camera.m_pixelHei=dlg.m_PixelHei;
			(*imgList)[idx].camera.m_imgWid=dlg.m_ImgWid;
			(*imgList)[idx].camera.m_imgHei=dlg.m_ImgHei;
			
			imgName=(*imgList)[i].strImgName;
			pos=imgName.ReverseFind('.');
			cmrName=imgName.Left(pos);
			cmrName+=".cmr";		//像机文件后缀
//			sprintf((*imgList)[i].camera.m_pCameraFileName, cmrName.GetBuffer(256));

			(*imgList)[idx].camera.m_bInor=true;
//			}
			
			
		}

		
		
// 		if(0==dlg.m_Option)
// 		{//应用于全部像片
// 			imgNum=imgList->GetSize();
// 			for(i=0; i<imgNum; i++)
// 			{
// 				(*imgList)[i].camera.SetInorParameter(dlg.m_x0, dlg.m_y0, dlg.m_f, 
// 					dlg.m_k1, dlg.m_k2, dlg.m_k3, dlg.m_p1, dlg.m_p2);
// 
// 				imgName=(*imgList)[i].strImgName;
// 				pos=imgName.ReverseFind('.');
// 				cmrName=imgName.Left(pos);
// 				cmrName+=".cmr";		//像机文件后缀
// 				sprintf((*imgList)[i].camera.m_pCameraFileName, cmrName.GetBuffer(256));
// 			}
// 			
// 		}
		
	}
	else if(pItem->iColumn==3)
	{//set POS
		selNum=GetSelectedRow();
		
		ASSERT(selNum==1);
		idx=m_pSelRowBuf[0]-1;
		
		CInputPosDialog	dlg(&((*imgList)[idx].camera));
		if(dlg.DoModal()!=IDOK)
			return;
		(*imgList)[idx].camera.m_bExor=true;

// 		(*imgList)[idx].camera.SetExorParameter(dlg.m_Xs, dlg.m_Ys, dlg.m_Zs, 
// 			dlg.m_Phi, dlg.m_Omega, dlg.m_Kappa);
// 		(*imgList)[idx].camera.SetCoordinateSystem(dlg.m_strHorizontalDatum.GetBuffer(128), (LPSTR)(LPCTSTR)(dlg.m_strVerticalDatum),
// 			dlg.m_PosUnitH, dlg.m_PosUnitV);
// 		(*imgList)[idx].camera.SetRotateSystem(dlg.m_RotateSys, dlg.m_AngleUnit);
	}

    UpdataImageState();
}

//提取选中的行号, 行号!=索引号 
int CImgSettingPage::GetSelectedRow()
{
	int selectCount=m_Grid.GetSelectedCount();
	int selRow;
	CCellRange	select = m_Grid.GetSelectedCellRange();
	int i, j;
	int minRow, maxRow, minCol, maxCol;
	CGridCellBase   *pCell=0;

	selRow=0;
	if(selectCount>0)
	{
		if(selectCount>m_SelBufLen)
		{//重新分配buf
			if(m_pSelRowBuf)
			{
				delete[] m_pSelRowBuf;
			}

			m_SelBufLen=selectCount;
			m_pSelRowBuf=new int[m_SelBufLen];
		}

		minRow=select.GetMinRow();
		maxRow=select.GetMaxRow();
		minCol=select.GetMinCol();
		maxCol=select.GetMaxCol();
		
		selRow=0;
		for(i=minRow; i<=maxRow; i++)
		{
			for(j=minCol; j<=maxCol; j++)
			{
				pCell=m_Grid.GetCell(i, j);
				if(pCell&&(pCell->GetState()&GVIS_SELECTED))
				{
					m_pSelRowBuf[selRow]=i;
					selRow++;
					break;
				}
			}
		}
		
	}

	return	selRow;
}

//根据像机状态对列表中的像片进行更新 (像机参数和POS)
void CImgSettingPage::UpdataImageState()
{
	int num;
	int i, j;
	GV_ITEM Item;
	iphImgINFO imgInfo;
	iphImgList *imgList;
	CString	str;

	imgList=m_pBlockInfo->GetImgList();
	num=imgList->GetSize();
	for(i=0, j=1; i<num; i++, j++)
	{
		imgInfo=(*imgList)[i];
		if(imgInfo.camera.m_bInor)
		{
			Item.crBkClr = RGB(0,255,0);   // 背景颜色 or - m_Grid.SetItemBkColour(row, col, clr);
			//Item.crFgClr = RGB(0,0,0);    // 字符颜色 or - m_Grid.SetItemFgColour(row, col, RGB(255,0,0));				    
			
		}
		else
		{
			Item.crBkClr = RGB(255,0,0);
		}
		Item.mask    |= (GVIF_BKCLR|GVIF_FGCLR);
		Item.row = j;
		Item.col = 2;
		str="\0";
		Item.strText = str;
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(j,2, m_Grid.GetItemState(j,2) | GVIS_READONLY);
		
		
		if(imgInfo.camera.m_bExor)
		{
			Item.crBkClr = RGB(0,255,0);
		}
		else
		{
			Item.crBkClr = RGB(255,0,0);
		}
		Item.mask    |= (GVIF_BKCLR|GVIF_FGCLR);
		Item.row = j;
		Item.col = 3;
		str="\0";
		Item.strText = str;
		m_Grid.SetItem(&Item);
		m_Grid.SetItemState(j,3, m_Grid.GetItemState(j,3) | GVIS_READONLY);
	}
}

void CImgSettingPage::OnCHECKCopyImg() 
{
	m_bCopyImg=!m_bCopyImg;
	
}
