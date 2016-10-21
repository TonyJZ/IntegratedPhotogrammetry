// WorkspaceBar.cpp : implementation of the CALMWorkspaceBar class
//

#include "stdafx.h"
#include "orsBase/orsIPlatform.h"
#include "ALMWorkSpaceBar.h"
#include "orsImage\orsIImageService.h"
#include "math.h"
#include "orsPointCloud\orsIPointCloudService.h"
#include "resource.h"
//#include "orsGui/orsIGuiService.h"

//#include "ALMGuiEvent/ALMIGuiEventService.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT nIcons[] =
{
	IDI_IPLR/*, IDI_FOLDERCLOSE, IDI_FOLDEROPEN*/
};

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

const int nBorderSize = 1;

/////////////////////////////////////////////////////////////////////////////
// CALMWorkspaceBar

BEGIN_MESSAGE_MAP(CALMWorkspaceBar, CBCGPDockingControlBar)
//{{AFX_MSG_MAP(CALMWorkspaceBar)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_PAINT()
ON_NOTIFY(NM_DBLCLK, 1, OnDblclkTree)
ON_NOTIFY(NM_CLICK, 1, OnClickTree)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CALMWorkspaceBar construction/destruction

CALMWorkspaceBar::CALMWorkspaceBar()
{
//	m_pALM=0;
//	m_hImgRoot=0;
//	m_hLiDRoot=0;
	m_hBlockRoot=0;
	m_hImgRoot=0;
	m_hOrthoRoot=0;
	m_hDTMRoot=0;
}

CALMWorkspaceBar::~CALMWorkspaceBar()
{
//	m_pALM=0;
//	m_wndTree.DeleteAllItems();
}

/////////////////////////////////////////////////////////////////////////////
// CALMWorkspaceBar message handlers

// void CALMWorkspaceBar::SetALMInfo(CALMPrjManager *pALM)
// {
// 	m_pALM=pALM;
// 	CleanTree();
// 	
// 	if(m_pALM)
// 	{
// 		InitTree();
// 	}
// }

int CALMWorkspaceBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	
	// Create tree windows.
	// TODO: create your own tab windows here:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
								TVS_LINESATROOT | TVS_HASBUTTONS;
	
	if (!m_wndTree.Create (dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	CreateWorkSpaceImages();	//未找到合适的图标，未实现
	
	// Setup trees content:
//	m_hImgRoot = m_wndTree.InsertItem (_T("航片列表"));
//	m_hLiDRoot = m_wndTree.InsertItem (_T("点云列表"));
	m_hBlockRoot = m_wndTree.InsertItem (_T("Block"), 0, 0);
	m_hImgRoot=m_wndTree.InsertItem(_T("Image"), 1, 1, m_hBlockRoot);
	m_hOrthoRoot=m_wndTree.InsertItem(_T("Orthos"), 1, 1, m_hBlockRoot);
	m_hDTMRoot=m_wndTree.InsertItem(_T("DTMs"), 1, 1, m_hBlockRoot);

//	m_wndTree.InsertItem (_T("Item 1"), hRoot1);
//	m_wndTree.InsertItem (_T("Item 2"), hRoot1);
	
	return 0;
}

void CALMWorkspaceBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);
	
	// Tab control should cover a whole client area:
	m_wndTree.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CALMWorkspaceBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_wndTree.GetWindowRect (rectTree);
	ScreenToClient (rectTree);
	
	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
								::GetSysColor (COLOR_3DSHADOW));
}

//递归删除指定节点下的所有子节点
void DelItem(CTreeCtrl *treeCtrl, HTREEITEM hItem)   
{   
	if(!treeCtrl->ItemHasChildren(hItem))   
	{   
		return;   
	}
	
	HTREEITEM   hChild = treeCtrl->GetChildItem(hItem);   
	do   
	{   
		if(treeCtrl->ItemHasChildren(hChild))   
		{   
			DelItem(treeCtrl, hChild);   
		}   
		else   
		{   
			treeCtrl->DeleteItem(hChild);   
		}   
		hChild = treeCtrl->GetNextSiblingItem(hChild);   
	}while(hChild != NULL);   
    
	return ;
}

void CALMWorkspaceBar::InitTree()
{
//	CArray<IMGLIST,IMGLIST>		*pImgList=NULL;
//	CArray<LASLIST,LASLIST>		*pLidList=NULL;
	CArray<CString,CString>		*pDEMList=NULL;
	CArray<CString,CString>		*pDOMList=NULL;
	CArray<CString,CString>		*pTMList=NULL;

	
//	pImgList=m_pALM->GetImgList();
	
// 	if(pImgList==NULL)
// 	{//影像
// 		AfxMessageBox("不能获取影像列表!", MB_OK);
// 	}
// 	pLidList=m_pALM->GetLasList();
// 	if(pLidList==NULL)
// 	{//Lidar
// 		AfxMessageBox("不能获取LIDAR列表!",MB_OK);
// 	}
// 	pDEMList=m_pALM->GetDEMProduct();
// 	if(pDEMList==NULL)
// 	{
// 		AfxMessageBox("不能获取DEM产品列表!",MB_OK);
// 	}
// 	pDOMList=m_pALM->GetDEMProduct();
// 	if(pDOMList==NULL)
// 	{
// 		AfxMessageBox("不能获取DOM产品列表!",MB_OK);
// 	}
// 	pTMList=m_pALM->GetThemeProduct();
// 	if(pTMList==NULL)
// 	{
// 		AfxMessageBox("不能获取专题产品列表!",MB_OK);
// 	}
// 	
// 	LayoutImage(pImgList);
// 	LayoutLiDAR(pLidList);
	LayoutProduct(pDEMList, pDOMList, pTMList);
	
	m_wndTree.Invalidate(TRUE);
}

void CALMWorkspaceBar::CleanTree()
{
	/*
	if(m_hImgRoot)
			DelItem(&m_wndTree, m_hImgRoot);
		if(m_hLiDRoot)
			DelItem(&m_wndTree, m_hLiDRoot);
		if(m_hPrd_DEMRoot)
			DelItem(&m_wndTree, m_hPrd_DEMRoot);
		if(m_hPrd_DOMRoot)
			DelItem(&m_wndTree, m_hPrd_DOMRoot);
		if(m_hPrd_TMRoot)
			DelItem(&m_wndTree, m_hPrd_TMRoot);*/


	m_wndTree.DeleteAllItems();
//	m_hImgRoot = m_wndTree.InsertItem (_T("航片列表"));
//	m_hLiDRoot = m_wndTree.InsertItem (_T("点云列表"));
	m_hBlockRoot = m_wndTree.InsertItem (_T("Block"),0,0);
	m_hImgRoot=m_wndTree.InsertItem(_T("Image"), 1, 1, m_hBlockRoot);
	m_hOrthoRoot=m_wndTree.InsertItem(_T("Orthors"), 1, 1, m_hBlockRoot);
	m_hDTMRoot=m_wndTree.InsertItem(_T("DTMs"), 1, 1, m_hBlockRoot);
	
	m_wndTree.Invalidate(TRUE);
}

void CALMWorkspaceBar::CreateWorkSpaceImages()
{

// 	CBitmap bmp;
// 	if (!bmp.LoadBitmap (ID_IMAGE))
// 	{
// 		TRACE(_T ("Can't load bitmap: %x\n"), ID_IMAGE);
// 		ASSERT (FALSE);
// 		return;
// 	}
// 	
// 	BITMAP bmpObj;
// 	bmp.GetBitmap (&bmpObj);
// 	
// 	UINT nFlags = ILC_MASK;
// 	
// 	nFlags |= (TRUE) ? ILC_COLOR24 : ILC_COLOR4;
	
	m_workSpaceImages.Create (16, 16, TRUE, 3, 1);
//	m_workSpaceImages.Add (&bmp, RGB (255, 0, 255));
	int i;
	for (i = 0; i < _countof(nIcons); ++i)
	{
		HICON hIcon = AfxGetApp()->LoadIcon(nIcons[i]);
		ASSERT(hIcon);
		
		m_workSpaceImages.Add(hIcon);
	}
	
	m_wndTree.SetImageList (&m_workSpaceImages, TVSIL_NORMAL);

}

// void CALMWorkspaceBar::LayoutImage(CArray<IMGLIST,IMGLIST> *pImgList)
// {
// 	HTREEITEM	hParent;
// 	HTREEITEM	hImgStrip;
// 	CString		str;
// 	int		i,	imgNum;
// 	int		CurStrip;
// 	IMGLIST	imgElement;
// 	
// 	ASSERT(pImgList);
// 	
// 	imgNum=pImgList->GetSize();
// 	CurStrip=1;
// 	str.Format("影像航带%d", CurStrip);
// 	hImgStrip = m_wndTree.InsertItem(str.GetBuffer(128),m_hImgRoot);
// 	//	hParent=hImgStrip;
// 	
// 	for(i=0; i<imgNum; i++)
// 	{
// 		imgElement=pImgList->GetAt(i);
// 		if(imgElement.nStripID>CurStrip)
// 		{
// 			CurStrip=imgElement.nStripID;
// 			str.Format("影像航带%d", CurStrip);	
// 			hImgStrip=m_wndTree.InsertItem(str.GetBuffer(128),m_hImgRoot);
// 			hParent=m_wndTree.InsertItem(imgElement.strImgName.GetBuffer(128),hImgStrip);
// 		}
// 		else
// 		{
// 			hParent=m_wndTree.InsertItem(imgElement.strImgName.GetBuffer(128),hImgStrip);
// 		}
// 	}
// }
// 
// void CALMWorkspaceBar::LayoutLiDAR(CArray<LASLIST,LASLIST> *pLidList)
// {
// 	HTREEITEM	hParent;
// 	HTREEITEM	hLidStrip;
// 	CString		str;
// 	int		i,	LidNum;
// 	int		CurStrip;
// 	LASLIST	LidElement;
// 	
// 	ASSERT(pLidList);
// 	
// 	LidNum=pLidList->GetSize();
// 	CurStrip=1;
// 	str.Format("LIDAR航带%d", CurStrip);
// 	hLidStrip = m_wndTree.InsertItem(str.GetBuffer(128),m_hLiDRoot);
// 	//	hParent=hLidStrip;
// 	for(i=0; i<LidNum; i++)
// 	{
// 		LidElement=pLidList->GetAt(i);
// 		if(LidElement.nStripID>CurStrip)
// 		{
// 			CurStrip=LidElement.nStripID;
// 			str.Format("LIDAR航带%d", CurStrip);	
// 			hLidStrip=m_wndTree.InsertItem(str.GetBuffer(128),m_hLiDRoot);
// 			hParent=m_wndTree.InsertItem(LidElement.strLasName.GetBuffer(128),hLidStrip);
// 		}
// 		else
// 		{
// 			hParent=m_wndTree.InsertItem(LidElement.strLasName.GetBuffer(128),hLidStrip);
// 		}
// 	}
// }

void CALMWorkspaceBar::LayoutProduct(CArray<CString,CString> *pDEMList, CArray<CString,CString> *pDOMList, CArray<CString,CString> *pTMList)
{
	int	Num;
	int	i;
	CString	str;
	
	ASSERT(pDEMList);
	ASSERT(pDOMList);
	ASSERT(pTMList);
	
	Num=0;
	Num=pDEMList->GetSize();
	if(Num>0)
	{
		for(i=0; i<Num; i++)
		{
			str=pDEMList->GetAt(i);
			m_wndTree.InsertItem(str.GetBuffer(128),m_hImgRoot);
		}
	}

	Num=pDOMList->GetSize();
	if(Num>0)
	{
		for(i=0; i<Num; i++)
		{
			str=pDOMList->GetAt(i);
			m_wndTree.InsertItem(str.GetBuffer(128),m_hOrthoRoot);
		}
	}

	Num=pTMList->GetSize();
	if(Num>0)
	{
		for(i=0; i<Num; i++)
		{
			str=pTMList->GetAt(i);
			m_wndTree.InsertItem(str.GetBuffer(128),m_hDTMRoot);
		}
	}
}

void CALMWorkspaceBar::OnSetFocus(CWnd* pOldWnd) 
{
	CBCGPDockingControlBar::OnSetFocus(pOldWnd);
	
	m_wndTree.SetFocus ();
	
}



void CALMWorkspaceBar::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint		point;
	UINT		pflags;
	HTREEITEM	hItem, hPrtItem;
	
	GetCursorPos(&point);
	m_wndTree.ScreenToClient(&point);
	hItem = m_wndTree.HitTest(point, &pflags);

	if (NULL == hItem || m_wndTree.ItemHasChildren(hItem))
		return;

	//双击显示具体内容
	hPrtItem=m_wndTree.GetParentItem(hItem);
	if(hPrtItem==NULL)
	{//根节点直接退出，不比较()
		TRACE("no parent, unknown output info!\n");
		return;
	}
	if(m_wndTree.GetParentItem(hPrtItem)==m_hImgRoot || 
		m_wndTree.GetParentItem(hPrtItem)==m_hOrthoRoot ||
		m_wndTree.GetParentItem(hPrtItem)==m_hDTMRoot)
	{//显示影像
		DisplayImage(hItem);
	}

}

//#include "ALMGuiEvent/ALMIOutputMsgEvent.h"
void CALMWorkspaceBar::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint		point;
	UINT		pflags;
	HTREEITEM	hItem=NULL, hPrtItem=NULL;
	
	GetCursorPos(&point);
	m_wndTree.ScreenToClient(&point);
	hItem = m_wndTree.HitTest(point, &pflags);

	if (NULL == hItem || m_wndTree.ItemHasChildren(hItem))
		return;

	
	hPrtItem=m_wndTree.GetParentItem(hItem);
	if(hPrtItem==NULL)
	{//根节点直接退出，不比较
		TRACE("no parent, unknown output info!\n");
		return;
	}
	
	if(m_wndTree.GetParentItem(hPrtItem)==m_hImgRoot)
	{
		OutputImageInfo(hItem);
	}
// 	else if(m_wndTree.GetParentItem(hPrtItem)==m_hLiDRoot)
// 	{
// 		OutputLiDARInfo(hItem); 
// 	}
// 	else if(m_wndTree.GetParentItem(hPrtItem)==m_hPrdRoot)
// 	{
// 		OutputProductInfo(hItem);
// 	}
	else
	{
		TRACE("unknown output info! The father is not matched!\n");
	}


}


void CALMWorkspaceBar::OutputImageInfo(HTREEITEM hItem)
{
	CString	strFileName, strMsg, strDir, strTmp;

//	strDir=m_pALM->GetRawImageDir();
	strFileName=m_wndTree.GetItemText(hItem);
	strFileName=strDir+"\\"+strFileName;

	strMsg+=_T("文件名:");
	strMsg+=strFileName;
	strMsg+="\r\n";

	orsIImageService  *imageService = 
		ORS_PTR_CAST(orsIImageService,getPlatform()->getService(ORS_SERVICE_IMAGE) );
	ASSERT( imageService != NULL );
	orsIImageSource* imgReader = imageService->openImageFile( _T(strFileName) );
	ors_uint32	width=imgReader->getWidth();
	ors_uint32	height=imgReader->getHeight();

	strTmp.Format("行数:\t%ld\t\t\t列数:\t%ld\r\n", width,height);
	strMsg+=strTmp;

	orsDataTYPE dataType=imgReader->getInputDataType();
	strTmp=_T("像元类型:");
	strMsg+=strTmp;
	switch(dataType)
	{
	case ORS_DT_UnKNOWN:
		strTmp=_T("Unknow data type");
		break;
	case ORS_DT_BYTE:
		strTmp=_T("8 bit unsigned INTeger");
		break;
	case ORS_DT_UINT16:
		strTmp=_T("16 bit unsigned INTeger");
		break;
	case ORS_DT_INT16:
		strTmp=_T("16 bit signed INTeger");
		break;
	case ORS_DT_UINT32:
		strTmp=_T("32 bit unsigned INTeger");
		break;
	case ORS_DT_INT32:
		strTmp=_T("32 bit signed INTeger");
		break;
	case ORS_DT_FLOAT32:
		strTmp=_T("32 bit FLOATing poINT");
		break;
	case ORS_DT_FLOAT64:
		strTmp=_T("64 bit FLOATing poINT");
		break;
	case ORS_DT_CINT16:
		strTmp=_T("Complex INT16");
		break;
	case ORS_DT_CINT32:
		strTmp=_T("Complex INT32");
		break;
	case ORS_DT_CFLOAT32:
		strTmp=_T("Complex FLOAT32");
		break;
	case ORS_DT_CFLOAT64:
		strTmp=_T("Complex FLOAT64");
		break;
	case ORS_DT_COUNT:
		strTmp=_T("maximum type # + 1");
		break;
	}
	strMsg+=strTmp;
	strMsg+="\t\t";
	
	ors_uint	bands=imgReader->getNumberOfInputBands();
	strTmp.Format("波段数:\t%d", bands);
	strMsg+=strTmp;
	strMsg+="\r\n";
	for(ors_uint i=0; i<bands; i++)
	{
		double	max, min, nodata;
		max=imgReader->getMaxSampleValue(i);
		min=imgReader->getMinSampleValue(i);
		nodata=imgReader->getNullSampleValue(i);
		
		strTmp.Format("波段%d\r\n", i+1);
		strMsg+=strTmp;
		strTmp.Format("最大值: %f\t最小值: %f\t无效值: %f\r\n", max,min,nodata);
		strMsg+=strTmp;
	}

	//处理单击事件
//	ALMIGuiEventService *pGuiService=ORS_PTR_CAST(ALMIGuiEventService,getPlatform()->getService( ORS_ALM_GUIEVENT_SERVICE_GUI ));	
//	ASSERT( pGuiService != NULL );
//	pGuiService->GetOutputMsgEvent()->raiseEvent( New_Msg, strMsg.GetBuffer(1024), scroll_Begin );
}


void CALMWorkspaceBar::OutputLiDARInfo(HTREEITEM hItem)
{
	CString	strFileName, strMsg, strDir, strTmp;
	
//	strDir=m_pALM->GetRawLasDir();
	strFileName=m_wndTree.GetItemText(hItem);
	strFileName=strDir+"\\"+strFileName;
	
	strMsg+=_T("文件名:");
	strMsg+=strFileName;
	strMsg+="\r\n";
	
	orsIPointCloudService  *pointService = 
		ORS_PTR_CAST(orsIPointCloudService, getPlatform()->getService(ORS_SERVICE_POINTCLOUD) );
	ASSERT( pointService != NULL );

	orsIPointCloudSourceReader* pointReader = pointService->openPointFileForRead( _T(strFileName) );
	orsPointFileHeaderType	headerType=pointReader->getHeaderType();

	ors_int64 pointNum;
	orsPointType	pointType;
	double bb_min[3], bb_max[3];

	pointNum=pointReader->getNumberOfPoints();
	pointType=pointReader->getPointType();
	pointReader->getBoundingRect(bb_min, bb_max);

	strTmp.Format("总点数:\t\t%ld\r\n", pointNum);
	strMsg+=strTmp;

	switch (pointType)
	{
	case PT_UNKNOWNTYPE:
		strTmp=_T("未知的点数据格式");
		break;
	case PT_LASFORMAT0:
		strTmp=_T("las format0");
		break;
	case PT_LASFORMAT1:
		strTmp=_T("las format1");
		break;
	case PT_XYZ:
		strTmp=_T("txt x, y, z");
		break;
	case PT_XYZI:
		strTmp=_T("txt x, y, z, i");
		break;
	case PT_XYZC:
		strTmp=_T("txt x, y, z, c");
		break;
	case PT_XYZIC:
		strTmp=_T("txt x, y, z, i, c");
		break;
	default:
		strTmp=_T("未定义点类型");
		break;
	}
	strMsg+=_T("点数据类型:  ");
	strMsg+=strTmp;
	strMsg+="\r\n";
	
	strTmp.Format("X MIN:  %lf\t\tX MAX:  %lf\r\n", bb_min[0],bb_max[0]);
	strMsg+=strTmp;
	strTmp.Format("Y MIN:  %lf\t\tY MAX:  %lf\r\n", bb_min[1],bb_max[1]);
	strMsg+=strTmp;
	strTmp.Format("Z MIN:  %lf\t\tZ MAX:  %lf\r\n", bb_min[2],bb_max[2]);
	strMsg+=strTmp;

	double point_interval;
	point_interval = (bb_max[0]-bb_min[0])*(bb_max[1]-bb_min[1])/pointNum;
	point_interval=sqrt(point_interval);
	strMsg+="\r\n";
	strTmp.Format("平均点间距\r\n");
	strMsg+=strTmp;
	strTmp.Format("average point space: %.2f\r\n", point_interval);
	strMsg+=strTmp;
	
//	ALMIGuiEventService *pGuiService=ORS_PTR_CAST(ALMIGuiEventService,getPlatform()->getService( ORS_ALM_GUIEVENT_SERVICE_GUI ));	
//	ASSERT( pGuiService != NULL );
//	pGuiService->GetOutputMsgEvent()->raiseEvent( New_Msg, strMsg.GetBuffer(1024), scroll_Begin );
}

void CALMWorkspaceBar::OutputProductInfo(HTREEITEM hItem)
{
//	ALMIGuiEventService *pGuiService=ORS_PTR_CAST(ALMIGuiEventService,getPlatform()->getService( ORS_ALM_GUIEVENT_SERVICE_GUI ));	
//	ASSERT( pGuiService != NULL );
//	pGuiService->GetOutputMsgEvent()->raiseEvent( New_Msg, "未实现", scroll_Begin );
}

void CALMWorkspaceBar::DisplayImage(HTREEITEM hItem)
{
	CString	strFileName, strMsg, strDir, strTmp;
	
//	strDir=m_pALM->GetRawImageDir();
	strFileName=m_wndTree.GetItemText(hItem);
	strFileName=strDir+"\\"+strFileName;
	
	//处理单击事件
//	ALMIGuiEventService *pGuiService=ORS_PTR_CAST(ALMIGuiEventService,getPlatform()->getService( ORS_ALM_GUIEVENT_SERVICE_GUI ));	
//	ASSERT( pGuiService != NULL );
//	pGuiService->GetDisplayImageEvent()->raiseEvent( strFileName.GetBuffer(1024) );
}