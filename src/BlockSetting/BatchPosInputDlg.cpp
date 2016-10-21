// BatchPosInputDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "blockSetting.h"
#include "BatchPosInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchPosInputDlg dialog


CBatchPosInputDlg::CBatchPosInputDlg(CALMPrjManager *pBlockInfo, CWnd* pParent /*=NULL*/)
	: CDialog(CBatchPosInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchPosInputDlg)
	m_RotateUnit = 0;
	m_RotateSystem = 0;
	m_strPosPath = _T("");
	//}}AFX_DATA_INIT
	m_pBlockInfo=pBlockInfo;
}


void CBatchPosInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchPosInputDlg)
	DDX_Radio(pDX, IDC_RADIO_360, m_RotateUnit);
	DDX_Radio(pDX, IDC_RADIO_YXZ, m_RotateSystem);
	DDX_Control(pDX, IDC_Grid, m_Grid);
	DDX_Text(pDX, IDC_EDIT_POSPath, m_strPosPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchPosInputDlg, CDialog)
	//{{AFX_MSG_MAP(CBatchPosInputDlg)
	ON_BN_CLICKED(IDC_BUTTON_CoordSysTrans, OnBUTTONCoordSysTrans)
	ON_BN_CLICKED(IDC_BUTTON_OpenPOS, OnBUTTONOpenPOS)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchPosInputDlg message handlers

BOOL CBatchPosInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(230, 230, 230));
	
	m_Grid.SetColumnCount(9);	//ClsID, check, color, CLsName
	//	m_Grid.SetRowCount(ClassGradation+1);
	m_Grid.SetFixedRowCount(1);
	//	m_Grid.SetFixedColumnCount(4);
	
	m_Grid.SetDefCellHeight(25);
	m_Grid.SetRowHeight(0, 25);
	m_Grid.SetColumnWidth(0, 60);	//id
	m_Grid.SetColumnWidth(1,100);	//image name
	m_Grid.SetColumnWidth(2,100);	//gps time
	m_Grid.SetColumnWidth(3,120);	//xs
	m_Grid.SetColumnWidth(4,120);	//ys
	m_Grid.SetColumnWidth(5,120);	//zs
	m_Grid.SetColumnWidth(6,120);	//phi
	m_Grid.SetColumnWidth(7,120);	//omega
	m_Grid.SetColumnWidth(8,120);	//kappa
	
	CString	str;
	GV_ITEM Item;
	
	str=_T("ID");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 0;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	str=_T("影像名");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 1;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	str=_T("GPS时间");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 2;
	Item.strText = str;
	m_Grid.SetItem(&Item);
	
	str=_T("Xs");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 3;
	Item.strText = str;
	m_Grid.SetItem(&Item);

	str=_T("Ys");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 4;
	Item.strText = str;
	m_Grid.SetItem(&Item);

	str=_T("Zs");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 5;
	Item.strText = str;
	m_Grid.SetItem(&Item);

	str=_T("Phi");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 6;
	Item.strText = str;
	m_Grid.SetItem(&Item);

	str=_T("Omega");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 7;
	Item.strText = str;
	m_Grid.SetItem(&Item);

	str=_T("Kappa");
	Item.mask = GVIF_TEXT;
	Item.row = 0;
	Item.col = 8;
	Item.strText = str;
	m_Grid.SetItem(&Item);

	m_Grid.SetFixedRowSelection(TRUE);
	m_Grid.SetFixedColumnCount(1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchPosInputDlg::OnBUTTONCoordSysTrans() 
{
	//将Grid中的外方位元素存入m_pBlockInfo中  需要指定一个POS数据格式
	
}

//bool ReadPOS_DMC(const char *pPosFileName,  vector<imagePOS> &blockPosData);

void CBatchPosInputDlg::OnBUTTONOpenPOS() 
{
// 	CFileDialog	dlg( FALSE, "pos", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("pos File (*.pos)|*.pos|All Files(*.*)|*.*||"));
// 
// 	
// 	if(dlg.DoModal()!=IDOK)
// 		return;
// 
// 	CString pPosFileName;
// 	pPosFileName=dlg.GetPathName();
// 
// 
// 	double	s=3.1415926/180;
// 	CString	strLine;
// 	CStdioFile	file;
// 	imagePOS	imgPos;
// 	int	ID;
// 	int i;
// 	double time, Xs, Ys, Zs;
// 	double omega, phi, kappa;
// 	double lat, lon;
// 	
// 	
// 	if(file.Open(pPosFileName,CFile::modeRead)==0)
// 	{
// 		return false;
// 	}
// 	
// 	file.ReadString(strLine);
// 	file.ReadString(strLine);
// 	while (file.ReadString(strLine))
// 	{
// 		
// 		sscanf(strLine,"%7d%6d%15lf%12lf%12lf%9lf%11lf%11lf%11lf%12lf%13lf\n", &ID,&i,&time,
// 			&Xs,&Ys,&Zs,&omega,&phi,&kappa,&lat,&lon);
// 		
// 		imgPos.ImageID=ID;
// 		imgPos.timeMark=time;
// 		imgPos.Xs=Xs;
// 		imgPos.Ys=Ys;
// 		imgPos.Zs=Zs;
// 		imgPos.omega=omega*s;
// 		imgPos.phi=phi*s;
// 		imgPos.kappa=kappa*s;
// 		
// 		blockPosData.push_back( imgPos );
// 	}
// 	
// 	file.Close(); 
// 
// 	int	imgNum;
// 	int i, idx;
// 	int selNum;
// 	iphImgList	*imgList;
// 	CString	imgName;
// 	CString	cmrName;
// 	int pos;
// 	
// 
// 		imgList=m_pBlockInfo->GetImgList();
// 		imgNum=imgList->GetSize();
// 		for(i=0; i<imgNum; i++)
// 		{
// 			(*imgList)[i].camera.m_x0=dlg.m_x0;
// 			(*imgList)[i].camera.m_y0=dlg.m_y0;
// 			(*imgList)[i].camera.m_f=dlg.m_f;
// 			(*imgList)[i].camera.m_k1=dlg.m_k1;
// 			(*imgList)[i].camera.m_k2=dlg.m_k2;
// 			(*imgList)[i].camera.m_k3=dlg.m_k3;
// 			(*imgList)[i].camera.m_p1=dlg.m_p1;
// 			(*imgList)[i].camera.m_p2=dlg.m_p2;
// 			(*imgList)[i].camera.m_pixelX=dlg.m_PixelWid;
// 			(*imgList)[i].camera.m_pixelY=dlg.m_PixelHei;
// 			(*imgList)[i].camera.m_imgWid=dlg.m_ImgWid;
// 			(*imgList)[i].camera.m_imgHei=dlg.m_ImgHei;
// 			
// 			imgName=(*imgList)[i].strImgName;
// 			pos=imgName.ReverseFind('.');
// 			cmrName=imgName.Left(pos);
// 			//			cmrName+=".cmr";		//像机文件后缀
// 			//			sprintf((*imgList)[i].camera.m_pCameraFileName, cmrName.GetBuffer(256));
// 			
// 			(*imgList)[i].camera.m_bInor=true;
// 		}
// 		
}


/*
bool ReadPOS_DMC(const char *pPosFileName,  vector<imagePOS> &blockPosData)
{
	double	s=3.1415926/180;
	CString	strLine;
	CStdioFile	file;
	imagePOS	imgPos;
	int	ID;
	int i;
	double time, Xs, Ys, Zs;
	double omega, phi, kappa;
	double lat, lon;
	
	
	if(file.Open(pPosFileName,CFile::modeRead)==0)
	{
		return false;
	}
	
	file.ReadString(strLine);
	file.ReadString(strLine);
	while (file.ReadString(strLine))
	{
		
		sscanf(strLine,"%7d%6d%15lf%12lf%12lf%9lf%11lf%11lf%11lf%12lf%13lf\n", &ID,&i,&time,
			&Xs,&Ys,&Zs,&omega,&phi,&kappa,&lat,&lon);
		
		imgPos.ImageID=ID;
		imgPos.timeMark=time;
		imgPos.Xs=Xs;
		imgPos.Ys=Ys;
		imgPos.Zs=Zs;
		imgPos.omega=omega*s;
		imgPos.phi=phi*s;
		imgPos.kappa=kappa*s;
		
		blockPosData.push_back( imgPos );
	}
	
	file.Close(); 
	return	true;
}*/
