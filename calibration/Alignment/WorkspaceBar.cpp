// WorkspaceBar.cpp : implementation of the CWorkspaceBar class
//

#include "stdafx.h"
#include "Alignment.h"
#include "WorkspaceBar.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar

BEGIN_MESSAGE_MAP(CWorkspaceBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CWorkspaceBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_NOTIFY(NM_DBLCLK, 1, OnDblclkTree)
	ON_NOTIFY(NM_CLICK, 1, OnClickTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar construction/destruction

CWorkspaceBar::CWorkspaceBar()
{
	m_pAlg=0;
	m_hImgRoot=0;
	m_hLiDRoot=0;
	m_hPrdRoot=0;
	m_hPrd_DEMRoot=0;
	m_hPrd_DOMRoot=0;
	m_hPrd_TMRoot=0;
	
	m_pLidList=NULL;

	m_pOutputBar=NULL;
}

CWorkspaceBar::~CWorkspaceBar()
{
	m_pAlg=0;
	m_pOutputBar=NULL;

	m_pLidList=NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CWorkspaceBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	// Setup trees content:
	m_hLiDRoot = m_wndTree.InsertItem (_T("点云列表"));
	
	m_hImgRoot = m_wndTree.InsertItem (_T("航片列表"));
	
// 	m_hPrdRoot = m_wndTree.InsertItem (_T("产品列表"));
// 	m_hPrd_DEMRoot=m_wndTree.InsertItem(_T("DEM产品"), m_hPrdRoot);
// 	m_hPrd_DOMRoot=m_wndTree.InsertItem(_T("DOM产品"), m_hPrdRoot);
// 	m_hPrd_TMRoot=m_wndTree.InsertItem(_T("专题产品"), m_hPrdRoot);

	return 0;
}

void CWorkspaceBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tab control should cover a whole client area:
	m_wndTree.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CWorkspaceBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_wndTree.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
								::GetSysColor (COLOR_3DSHADOW));
}

void CWorkspaceBar::SetAlgPrj(CAlignPrj *pAlg)
{
	m_pAlg=pAlg;
	m_pLidList=m_pAlg->GetLidList();
	CleanTree();
	
	if(m_pAlg)
	{
		InitTree();
	}
}

void CWorkspaceBar::SetOutputBar(COutputBar *pOutputBar)
{
	m_pOutputBar=pOutputBar;
}

void CWorkspaceBar::Relayout()
{
	CleanTree();
	InitTree();
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

void CWorkspaceBar::InitTree()
{
	CArray<Align_LidLine, Align_LidLine> *pLidList=NULL;
	CArray<Align_Image, Align_Image> *pImgList=NULL;
	CArray<CString,CString>		*pDEMList=NULL;
	CArray<CString,CString>		*pDOMList=NULL;
	CArray<CString,CString>		*pTMList=NULL;

	
	pImgList=m_pAlg->GetImgList();
	
	if(pImgList==NULL)
	{//影像
		AfxMessageBox("不能获取影像列表!", MB_OK);
	}

	pLidList=m_pAlg->GetLidList();
	if(pLidList==NULL)
	{//Lidar
		AfxMessageBox("不能获取LIDAR列表!",MB_OK);
	}
// 	pDEMList=m_pAlg->GetDEMProduct();
// 	if(pDEMList==NULL)
// 	{
// 		AfxMessageBox("不能获取DEM产品列表!",MB_OK);
// 	}
// 	pDOMList=m_pAlg->GetDEMProduct();
// 	if(pDOMList==NULL)
// 	{
// 		AfxMessageBox("不能获取DOM产品列表!",MB_OK);
// 	}
// 	pTMList=m_pAlg->GetThemeProduct();
// 	if(pTMList==NULL)
// 	{
// 		AfxMessageBox("不能获取专题产品列表!",MB_OK);
// 	}
	
	LayoutImage(pImgList);
	LayoutLiDAR(pLidList);
//	LayoutProduct(pDEMList, pDOMList, pTMList);
	
	m_wndTree.Invalidate(TRUE);
}

void CWorkspaceBar::CleanTree()
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
	m_hLiDRoot = m_wndTree.InsertItem (_T("点云列表"));
	m_hImgRoot = m_wndTree.InsertItem (_T("航片列表"));
	
// 	m_hPrdRoot = m_wndTree.InsertItem (_T("产品列表"));
// 	m_hPrd_DEMRoot=m_wndTree.InsertItem(_T("DEM产品"), m_hPrdRoot);
// 	m_hPrd_DOMRoot=m_wndTree.InsertItem(_T("DOM产品"), m_hPrdRoot);
// 	m_hPrd_TMRoot=m_wndTree.InsertItem(_T("专题产品"), m_hPrdRoot);
// 	
	m_wndTree.Invalidate(TRUE);
}


void CWorkspaceBar::LayoutImage(CArray<Align_Image, Align_Image> *pImgList)
{
	HTREEITEM	hParent;
	HTREEITEM	hImgStrip;
	CString		str;
	int		i,	imgNum;
	int		CurStrip;
	Align_Image		imgElement;
	
	ASSERT(pImgList);
	
	imgNum=pImgList->GetSize();
/*	if(imgNum>0)
	{
		CurStrip=0;
		str.Format("Strip_%d", CurStrip);
		hImgStrip = m_wndTree.InsertItem(str.GetBuffer(0),m_hImgRoot);
	}*/
	
	CurStrip = 0;
	for(i=0; i<imgNum; i++)
	{
		imgElement=pImgList->GetAt(i);
		int nStripID = getStripID(imgElement.nImgID);
		if(nStripID>CurStrip)
		{
			CurStrip=nStripID;
			str.Format("Strip_%d", CurStrip);	
			hImgStrip=m_wndTree.InsertItem(str.GetBuffer(0),m_hImgRoot);
			int nPhoID = getPhotoID(imgElement.nImgID);
			str.Format("Photo_%d", nPhoID);	
			hParent=m_wndTree.InsertItem(str.GetBuffer(0), hImgStrip);
		}
		else
		{
			int nPhoID = getPhotoID(imgElement.nImgID);
			str.Format("Photo_%d", nPhoID);	
			hParent=m_wndTree.InsertItem(str.GetBuffer(0), hImgStrip);
		}
	}
}


void CWorkspaceBar::LayoutLiDAR(CArray<Align_LidLine, Align_LidLine> *pLidList)
{
//	HTREEITEM	hParent;
	HTREEITEM	hLidStrip;
	CString		str;
	int		i,	LidNum;
	int		CurStrip;
	Align_LidLine	LidElement;
	
	ASSERT(pLidList);
	
	LidNum=pLidList->GetSize();
// 	CurStrip=1;
// 	str.Format("Strip_%d", CurStrip);
// 	hLidStrip = m_wndTree.InsertItem(str.GetBuffer(0),m_hLiDRoot);
	//	hParent=hLidStrip;
	for(i=0; i<LidNum; i++)
	{
		LidElement=pLidList->GetAt(i);
//		if(LidElement.nStripID>CurStrip)
//		{
			CurStrip=LidElement.LineID;
			str.Format("Strip_%d", CurStrip);	
			hLidStrip=m_wndTree.InsertItem(str.GetBuffer(0),m_hLiDRoot);
//			hParent=m_wndTree.InsertItem(LidElement.strLasName.GetBuffer(0),hLidStrip);
//		}
//		else
//		{
//			hParent=m_wndTree.InsertItem(LidElement.strLasName.GetBuffer(0),hLidStrip);
//		}
	}
}

/*
void CWorkspaceBar::LayoutProduct(CArray<CString,CString> *pDEMList, CArray<CString,CString> *pDOMList, CArray<CString,CString> *pTMList)
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
			m_wndTree.InsertItem(str.GetBuffer(128),m_hPrd_DEMRoot);
		}
	}

	Num=pDOMList->GetSize();
	if(Num>0)
	{
		for(i=0; i<Num; i++)
		{
			str=pDOMList->GetAt(i);
			m_wndTree.InsertItem(str.GetBuffer(128),m_hPrd_DOMRoot);
		}
	}

	Num=pTMList->GetSize();
	if(Num>0)
	{
		for(i=0; i<Num; i++)
		{
			str=pTMList->GetAt(i);
			m_wndTree.InsertItem(str.GetBuffer(128),m_hPrd_TMRoot);
		}
	}
}*/

void CWorkspaceBar::OnSetFocus(CWnd* pOldWnd) 
{
	CBCGPDockingControlBar::OnSetFocus(pOldWnd);
	
	m_wndTree.SetFocus ();
	
}

void CWorkspaceBar::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
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

	DisplayImage(hItem);

// 	if(m_wndTree.GetParentItem(hPrtItem)==m_hImgRoot || 
// 		m_wndTree.GetParentItem(hPrtItem)==m_hPrd_DOMRoot ||
// 		m_wndTree.GetParentItem(hPrtItem)==m_hPrd_DEMRoot)
// 	{//显示影像
// 		
// 		DisplayImage(hItem);
// 	}
	
}

//在output窗口查看信息
void CWorkspaceBar::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint		point;
	UINT		pflags;
	HTREEITEM	hItem=NULL, hPrtItem=NULL;
	
	GetCursorPos(&point);
	m_wndTree.ScreenToClient(&point);
	hItem = m_wndTree.HitTest(point, &pflags);

//	m_pOutputBar->OutputMsg( New_Msg, "asadasdas", scroll_Begin);

	
	if (NULL == hItem || m_wndTree.ItemHasChildren(hItem))
		return;
	
	
	hPrtItem=m_wndTree.GetParentItem(hItem);
	if(hPrtItem==NULL)
	{//根节点直接退出，不比较
		TRACE("no parent, unknown output info!\n");
		return;
	}

	while(hPrtItem)
	{
		if(hPrtItem==m_hImgRoot || hPrtItem==m_hLiDRoot)
			break;

		hPrtItem=m_wndTree.GetParentItem(hPrtItem);
	}
	
	if(hPrtItem==m_hImgRoot)
	{
		OutputImageInfo(hItem);
	}
	else if(hPrtItem==m_hLiDRoot)
	{
		OutputLiDARInfo(hItem); 
	}
// 	else if(m_wndTree.GetParentItem(hPrtItem)==m_hPrdRoot)
// 	{
// 		OutputProductInfo(hItem);
// 	}
	else
	{
		TRACE("unknown output info! The father is not matched!\n");
	}
}


void CWorkspaceBar::OutputImageInfo(HTREEITEM hItem)
{
	CString	strItem, strTmp;
	int photoID;
	int pos;

	strItem=m_wndTree.GetItemText(hItem);
	pos=strItem.ReverseFind('_');
	strTmp=strItem.Right(strItem.GetLength()-pos-1);

	photoID=atoi(strTmp);

	HTREEITEM hPrtItem=m_wndTree.GetParentItem(hItem);
	strItem=m_wndTree.GetItemText(hPrtItem);
	pos=strItem.ReverseFind('_');
	strTmp=strItem.Right(strItem.GetLength()-pos-1);
	int stripID=atoi(strTmp);

	CString imgName;
	for(int i=0; i<m_pAlg->GetImgList()->GetSize(); i++)
	{
		Align_Image  imgItem = m_pAlg->GetImgList()->GetAt(i);
		int nStripID = getStripID(imgItem.nImgID);
		int nPhoID = getPhotoID(imgItem.nImgID);
		if(nStripID == stripID && nPhoID == photoID)
		{
			imgName = imgItem.ImgName;
			break;
		}
	}
//	Align_Image  lidLine=m_pAlg->GetImgList()->GetAt(ID-1);

//	ASSERT(lidLine.nPhoID==ID);

	CString msg;

	msg+="\r\n";
	msg+=strItem;
	msg+="\r\n";
	msg+="Image File:  ";
	msg+=imgName;
	msg+="\r\n";
	msg+="image ID:  ";
	strTmp.Format("strip:%d\tphoto:%d", stripID, photoID/*getUniqueID(stripID, photoID)*/);
	msg+=strTmp;
	msg+="\r\n\r\n";

	m_pOutputBar->OutputMsg(New_Msg, msg.GetBuffer(0), scroll_End);
	return;
}

void CWorkspaceBar::OutputLiDARInfo(HTREEITEM hItem)
{
	CString	strItem, strTmp;
	int ID;
	int pos;
	
	strItem=m_wndTree.GetItemText(hItem);
	pos=strItem.ReverseFind('_');
	strTmp=strItem.Right(strItem.GetLength()-pos-1);

	ID=atoi(strTmp);
	
	Align_LidLine  lidLine=m_pLidList->GetAt(ID-1);

	ASSERT(lidLine.LineID==ID);

	CString msg;

	msg+="\r\n";
	msg+=strItem;
	msg+="\r\n";
	msg+="Image File:  ";
	msg+=lidLine.GridName;
	msg+="\r\n";
	msg+="Las File:  ";
	msg+=lidLine.LasName;
	msg+="\r\n";
	msg+="Trj File:  ";
	msg+=lidLine.TrjName;
	msg+="\r\n";
	CString strAFH;
	strAFH.Format("%s%.2f", "AFH:", lidLine.averFH);
	msg+=strAFH;
	msg+="\r\n\r\n";

	m_pOutputBar->OutputMsg(New_Msg, msg.GetBuffer(0), scroll_End);

	return;
}

void CWorkspaceBar::OutputProductInfo(HTREEITEM hItem)
{

	return;
}

void CWorkspaceBar::DisplayImage(HTREEITEM hItem)
{	
	HTREEITEM	hPrtItem;
	int i;

	hPrtItem=m_wndTree.GetParentItem(hItem);
	while(hPrtItem)
	{
		if(hPrtItem==m_hLiDRoot || hPrtItem==m_hImgRoot)
			break;

		hPrtItem=m_wndTree.GetParentItem(hPrtItem);
	}
	
	ASSERT(hPrtItem);

	CMainFrame* pMainFrm=(CMainFrame*) AfxGetMainWnd();

	CString cmdLine;
	cmdLine = GetApp()->GetAppDir();
	
	CString	strItem, strTmp, strFileName;
	int ID;
	int pos;
	Source_Type	sType;
	
	if(hPrtItem==m_hLiDRoot)
	{
		sType = ST_LiDAR;
		strItem=m_wndTree.GetItemText(hItem);
		pos=strItem.ReverseFind('_');
		strTmp=strItem.Right(strItem.GetLength()-pos-1);

		ID=atoi(strTmp);

		Align_LidLine  lidLine=m_pLidList->GetAt(ID-1);
		
		strFileName=lidLine.GridName;
	}
	else if(hPrtItem==m_hImgRoot)
	{
		sType = ST_Image;
		strItem=m_wndTree.GetItemText(hItem);
		pos=strItem.ReverseFind('_');
		strTmp=strItem.Right(strItem.GetLength()-pos-1);
		int photoID=atoi(strTmp);

		HTREEITEM hPrtItem=m_wndTree.GetParentItem(hItem);
		strItem=m_wndTree.GetItemText(hPrtItem);
		pos=strItem.ReverseFind('_');
		strTmp=strItem.Right(strItem.GetLength()-pos-1);
		int stripID=atoi(strTmp);
		//int nPhoID = 0;

		CString imgName;
		for(i=0; i<m_pAlg->GetImgList()->GetSize(); i++)
		{
			Align_Image  imgItem = m_pAlg->GetImgList()->GetAt(i);
			int nStripID = getStripID(imgItem.nImgID);
			int nPhoID = getPhotoID(imgItem.nImgID);
			if(nStripID == stripID && nPhoID == photoID)
			{
				imgName = imgItem.ImgName;
				break;
			}
		}

		strFileName=imgName;

		ID = getUniqueID(stripID, photoID);//stripID*sFactor+ID; /*i+m_pAlg->GetLidList()->GetSize();	//按统一编号*/
	}
	
	cmdLine += "RegistViewer.exe "; 
	cmdLine += strFileName;

	char buf[20];
	HWND hWnd=GetSafeHwnd();
	_ultoa( (DWORD)hWnd, buf, 16 );
	
	
	cmdLine += " -HWND ";
	cmdLine += buf;
	cmdLine += " -TO_Type ";
	
	char tietype[8];
	_itoa(pMainFrm->m_tieType, tietype, 10);
	
	cmdLine += tietype;

	char SourceType[8];
	_itoa(sType, SourceType, 10);
	cmdLine += " -SourceType ";
	cmdLine += SourceType;

	char sourceID[8];
	_itoa(ID, sourceID, 10);

	cmdLine += " -SourceID ";
	cmdLine += sourceID;

	CString strAlgName;
	strAlgName=m_pAlg->GetFilePath();
	
	cmdLine += " ";
	cmdLine += strAlgName;

	int n=pMainFrm->m_RegistViewWindows;
	if(n<Max_RegistViewers)
	{
		int err = WinExec( cmdLine,  SW_SHOW);
		((CMainFrame*) AfxGetMainWnd())->m_RegistViewWindows+=1;
	}
	return;
}
