// SelectPointDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CalibProc.h"
#include "SelectPointDlg.h"


// CSelectPointDlg dialog

IMPLEMENT_DYNAMIC(CSelectPointDlg, CDialog)

CSelectPointDlg::CSelectPointDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectPointDlg::IDD, pParent)
	, m_strPlaneapj(_T(""))
	, m_strSIFTapj(_T(""))
	, m_strSelPtapj(_T(""))
	, m_BufSize(0)
{
// 	m_pLidVP=NULL;
// 	m_pLidPlanes=NULL;
	m_pVPTieLut=NULL;
	m_pPlaneTieLut=NULL;
}

CSelectPointDlg::~CSelectPointDlg()
{
// 	if(m_pLidVP)		delete[] m_pLidVP;		m_pLidVP=NULL;
// 	if(m_pLidPlanes)	delete[] m_pLidPlanes;	m_pLidPlanes=NULL;
	if(m_pVPTieLut)		delete[] m_pVPTieLut;	m_pVPTieLut=NULL;
	if(m_pPlaneTieLut)	delete[] m_pPlaneTieLut;m_pPlaneTieLut=NULL;
}

void CSelectPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_plane, m_strPlaneapj);
	DDX_Text(pDX, IDC_EDIT_TiePoint, m_strSIFTapj);
	DDX_Text(pDX, IDC_EDIT_SelPoint, m_strSelPtapj);
	DDX_Text(pDX, IDC_EDIT_Buffer, m_BufSize);
}


BEGIN_MESSAGE_MAP(CSelectPointDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_plane, &CSelectPointDlg::OnBnClickedButtonplane)
	ON_BN_CLICKED(IDC_BUTTON_Tiepoint, &CSelectPointDlg::OnBnClickedButtonTiepoint)
	ON_BN_CLICKED(IDC_BUTTONSelPoint, &CSelectPointDlg::OnBnClickedButtonselpoint)
	ON_BN_CLICKED(IDOK, &CSelectPointDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelectPointDlg message handlers

void CSelectPointDlg::OnBnClickedButtonplane()
{
	CFileDialog dlg(TRUE,"平面apj",NULL,OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT,"align(*.apj)|*.apj|(*.*)|*.*||",NULL);

	if(dlg.DoModal()==IDOK)
		m_strPlaneapj = dlg.GetPathName();

	UpdateData(FALSE);
}

void CSelectPointDlg::OnBnClickedButtonTiepoint()
{
	CFileDialog dlg(TRUE,"SIFT点apj",NULL,OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT,"align(*.apj)|*.apj|(*.*)|*.*||",NULL);

	if(dlg.DoModal()==IDOK)
		m_strSIFTapj = dlg.GetPathName();

	UpdateData(FALSE);
	
}

void CSelectPointDlg::OnBnClickedButtonselpoint()
{
	CFileDialog  outdlg(FALSE,"选点结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"align(*.apj)|*.apj|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()==IDOK)
		m_strSelPtapj = outdlg.GetPathName();

	UpdateData(FALSE);
}

bool CSelectPointDlg::CheckPtInPolygon(POINT3D *pt, POINT3D *polygon, int ptNum)
{
	double bbXmax, bbXmin, bbYmax, bbYmin;
	int i=0;
	
	bbXmax=bbXmin=polygon[0].X;
	bbYmax=bbYmin=polygon[0].Y;
	
	//获得BoundingBox
	for(i=1; i<ptNum; i++)
	{
		if(bbXmax < polygon[i].X)
			bbXmax = polygon[i].X;
		if(bbXmin > polygon[i].X)
			bbXmin = polygon[i].X;
		if(bbYmax < polygon[i].Y)
			bbYmax = polygon[i].Y;
		if(bbYmin > polygon[i].Y)
			bbYmin = polygon[i].Y;
	}

	bbXmax += m_BufSize;
	bbXmin -= m_BufSize;
	bbYmax += m_BufSize;
	bbYmin -= m_BufSize;

	if(pt->X>bbXmin && pt->X<bbXmax && pt->Y>bbYmin && pt->Y<bbYmax)
		return true;
	else
		return false;
}

void CSelectPointDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if(m_strPlaneapj.IsEmpty())
		return;
	if(m_strSIFTapj.IsEmpty())
		return;
	if(m_strSelPtapj.IsEmpty())
		return;

	CAlignPrj algPlane, algSIFT, algSelpt;


	algPlane.Open(m_strPlaneapj.GetBuffer(256));
	algSIFT.Open(m_strSIFTapj.GetBuffer(256));
	algSelpt.m_strPrjName = m_strSelPtapj;

	int nLine, nPoint, nPlane;
	int i, j, k;
	ExtractTiePointLut(algSIFT);
	ExtractTiePlaneLut(algPlane);

	CArray<TieObjChain*, TieObjChain*> *pTiePtList=NULL;
	CArray<TieObjChain*, TieObjChain*> *pTiePlaneList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;

	pLidLines=algPlane.GetLidList();

	pTiePlaneList=algPlane.GetTieObjList();
	pTiePtList=algSIFT.GetTieObjList();
	

	nLine=pLidLines->GetSize();
	nPoint=pTiePtList->GetSize();
	nPlane=pTiePlaneList->GetSize();

	bool *pbSel=NULL;
	pbSel = new bool[nPoint];
	memset(pbSel, 0, sizeof(bool)*nPoint);

	TieObjChain *pPtChain=NULL, *pPlaneChain=NULL;
	TieObject *ptObj=NULL, *planeObj=NULL;
	int LineID;

	int nSel=0;
	int iPlane;
	POINT3D ptCoord, *ptPolygon;
	int ptNum;
	for(i=0; i<nPoint; i++)
	{
		pPtChain=pTiePtList->GetAt(i);  //遍历每一个点链

		for(j=0; j<pPtChain->objNum; j++)
		{
			ptObj=pPtChain->pChain->GetAt(j);
			ptCoord=((TiePoint*)ptObj)->pt3D;

			if(ptObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				LineID=ptObj->sourceID;
			}

			for(iPlane=0; iPlane<nPlane; iPlane++)
			{
				//判断当前连接平面是否在指定条带上存在
				if(m_pPlaneTieLut[iPlane*nLine+LineID] < 0)
					continue;

				k = m_pPlaneTieLut[iPlane*nLine+LineID];
				pPlaneChain=pTiePlaneList->GetAt(iPlane);
				planeObj=pPlaneChain->pChain->GetAt(k);

				ptNum=((TiePatch*)planeObj)->ptNum;
				ptPolygon=((TiePatch*)planeObj)->pt3D;

				if(CheckPtInPolygon(&ptCoord, ptPolygon, ptNum))
				{
					pbSel[i]=true;
					nSel++;
					break;;
				}
			}

			if(pbSel[i])	//当前连接点已经被标记
				break;
		}
	}


	//写工程文件
	CArray<Align_LidLine, Align_LidLine>  *pLidList=algSIFT.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=algSIFT.GetImgList();
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine lidLine=pLidList->GetAt(i);

		algSelpt.GetLidList()->Add(lidLine);
	}

	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image imgItem=pImgList->GetAt(i);

		algSelpt.GetImgList()->Add(imgItem);
	}

	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	pTieList = algSelpt.GetTieObjList();
	for(i=0; i<nPoint; i++)
	{
		pPtChain=pTiePtList->GetAt(i);  //遍历每一个点链

		if(pbSel[i])
		{
			TieObjChain* pCurChain = new TieObjChain;

			pCurChain->TieID=pPtChain->TieID;
			pCurChain->type=pPtChain->type;
			pCurChain->bDel=false;
			pCurChain->pChain=new CArray<TieObject*, TieObject*>; //无连接点
			pCurChain->objNum=pPtChain->objNum;

			for(j=0; j<pCurChain->objNum; j++)
			{
				ptObj = pPtChain->pChain->GetAt(j);

				TieObject *pTmpObj=new TiePoint;
				pTmpObj->tieID=ptObj->tieID;
				pTmpObj->objType=ptObj->objType;
				pTmpObj->sourceID=ptObj->sourceID;
				pTmpObj->sourceType=ptObj->sourceType;
				((TiePoint*)pTmpObj)->pt2D=((TiePoint*)ptObj)->pt2D;
				((TiePoint*)pTmpObj)->pt3D=((TiePoint*)ptObj)->pt3D;
				((TiePoint*)pTmpObj)->tpType = ((TiePoint*)ptObj)->tpType;
				pCurChain->pChain->Add(pTmpObj);

			}
			
			pTieList->Add(pCurChain);
		}
	}

	algSelpt.Save();

	algPlane.Close();
	algSIFT.Close();

	OnOK();

	if(pbSel)	delete[] pbSel;	pbSel=NULL;
}



void CSelectPointDlg::ExtractTiePointLut(CAlignPrj &algPrj)
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	int nVP, iVP;
	//	double *pZObs=NULL;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	int nLine, nTies;
	POINT3D pt3D, *ptBuf;
	int ngcp;
	//	ATNTP_Point *pATNPoints=NULL;

	pTieList=algPrj.GetTieObjList();
	pLidLines=algPrj.GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

		
	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为-1
		pTieLut[i]=-1;
	}

	nVP=0;
	ngcp=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pTieChain=pTieList->GetAt(i);  //遍历每一个点链

		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());
		ASSERT(pTieChain->type==TO_POINT);


		if(pTieChain->type==TO_POINT)
			nVP+=pTieChain->pChain->GetSize();

		if(pTieChain->bGCP) 
			ngcp+=pTieChain->pChain->GetSize();


		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}
}

void CSelectPointDlg::ExtractTiePlaneLut(CAlignPrj &algPrj)
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
	int srcID;

	int nPatch, iPatch;
	int nLine, nTies;
	POINT3D pt3D, *ptBuf;

	pTieList=algPrj.GetTieObjList();
	pLidLines=algPrj.GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();


	m_pPlaneTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为-1
		m_pPlaneTieLut[i]=-1;
	}

	nPatch=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pTieChain=pTieList->GetAt(i);  //遍历每一个点链

		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());
		ASSERT(pTieChain->type==TO_PATCH);

		if(pTieChain->type==TO_PATCH)
		{
			nPatch+=pTieChain->pChain->GetSize();	
			
		}

		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				m_pPlaneTieLut[i*nLine+srcID]=j;
			}
		}
	}

}