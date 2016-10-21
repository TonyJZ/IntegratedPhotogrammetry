// RegistViewerDoc.cpp : implementation of the CRegistViewerDoc class
//

#include "stdafx.h"
#include "RegistViewer.h"
#include "MainFrm.h"

#include "RegistViewerDoc.h"
#include "RegistViewerView.h"

#include "orsImage\orsIAlgImageResampler.h"
#include "orsMap\orsIMapService.h"
#include "AlignPrj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerDoc

IMPLEMENT_DYNCREATE(CRegistViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CRegistViewerDoc, CDocument)
	//{{AFX_MSG_MAP(CRegistViewerDoc)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_Add_apj, &CRegistViewerDoc::OnAddapj)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerDoc construction/destruction

CRegistViewerDoc::CRegistViewerDoc()
{
	m_pLayerCollection = getMapService()->CreateLayerCollection();

//	m_LayerAttached=false;
}

CRegistViewerDoc::~CRegistViewerDoc()
{
}

BOOL CRegistViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	GetMainFrame()->AttachLayerCollection( m_pLayerCollection.get() );

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CRegistViewerDoc serialization

void CRegistViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerDoc diagnostics

#ifdef _DEBUG
void CRegistViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRegistViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerDoc commands


#include "orsImage/orsIAlgImageFilter.h"
#include "orsImage/orsIImageSourceZoomer.h"

#include "orsImage/orsIImageSourceCache.h"

#include "orsMap\orsIImageLayer.h"

BOOL CRegistViewerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	GetMainFrame()->AttachLayerCollection( m_pLayerCollection.get() );

	m_imgPathName = lpszPathName;
	
//	m_pLayerCollection->RemoveAllLayers();
	
	ref_ptr<orsIImageLayer> imgLayer = getMapService()->CreateImgLayer();
	
	orsIImageSource *viewSpace = m_pLayerCollection->getActiveDataFrame()->getViewSpace();
	
	if( !imgLayer->OpenImage( lpszPathName, m_pLayerCollection.get(), viewSpace ) ) {
		getPlatform()->logPrint( ORS_LOG_ERROR, "Can not open image %s", lpszPathName );
		return false;
	}
	
//	m_pLayerCollection->AddDataSource( imgLayer->GetRawImageSource(), imgLayer.get() );

	m_pLayerCollection->AddLayer( imgLayer.get() );

	if(m_imgGeo.get()==NULL)
		m_imgGeo=imgLayer->GetDspImageChain()->GetImageGeometry();

	//////////////////////////////////////////////////////////////////////////
	if( NULL == viewSpace )	 {
		m_pLayerCollection->getActiveDataFrame()->setViewSpace( imgLayer->GetDspImageChain() );
		//m_viewSpace = imgLayer->GetDspImageChain();		
	}
	
	//////////////////////////////////////////////////////////////////////////
	CString str = lpszPathName;
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();
	ExtractTieObjects(theApp->m_strAlgName.GetBuffer(128));

	UpdateAllViews( NULL );
	
	return TRUE;
}

void CRegistViewerDoc::ExtractTieObjects(char *lpszPathName)
{
	CRegistViewerApp *theApp=(CRegistViewerApp*)AfxGetApp();
	int srcID;	
	Source_Type srcType;

	srcID=theApp->m_sourceID;
	srcType=theApp->m_sourceType;

	CAlignPrj algPrj;
	int i, j;
	CString strCurImgName=lpszPathName;

	if(algPrj.Open(lpszPathName))
	{
		CArray<Align_LidLine, Align_LidLine> *pLidList=algPrj.GetLidList();

		for(i=0; i<pLidList->GetSize(); i++)
		{
			Align_LidLine lidLine=pLidList->GetAt(i);

			if(lidLine.GridName==strCurImgName)
			{
				srcID=lidLine.LineID;
				break;
			}
		}


		CRegistViewerView *pView=GetMainView();
		CArray<TieObject*, TieObject*> *pTieObjList=NULL;
		pTieObjList=&(pView->m_TObjList);

		CArray<TieObjChain*, TieObjChain*> *pTieList=algPrj.GetTieObjList();
		TieObjChain *pChain;
		TieObject *pObj;
		TieObject *pTmpObj;
		int ptNum;
		POINT2D *pt2D=NULL;
		POINT3D *pt3D=NULL;

		if(theApp->m_bReproj)
		{//重新投影二维坐标
			for(i=0; i<pTieList->GetSize(); i++)
			{
				pChain=pTieList->GetAt(i);

				pObj=pChain->pChain->GetAt(0);

				//找到当前影像上的点
				if(pObj->objType==TO_POINT)
				{
					pTmpObj=new TiePoint;
					pTmpObj->objType=TO_POINT;
					pTmpObj->sourceID=srcID;
					pTmpObj->sourceType=srcType;
					pTmpObj->tieID=pObj->tieID;
					((TiePoint*)pTmpObj)->pt3D=((TiePoint*)pObj)->pt3D;

					m_imgGeo->Project(((TiePoint*)pObj)->pt3D.X, ((TiePoint*)pObj)->pt3D.Y, ((TiePoint*)pObj)->pt3D.Z, 
						&(((TiePoint*)pTmpObj)->pt2D.x), &(((TiePoint*)pTmpObj)->pt2D.y) );

					pTieObjList->Add(pTmpObj);
				}
				else if(pObj->objType==TO_LINE)
				{
					pTmpObj=new TieLine;
					pTmpObj->objType=TO_LINE;
					pTmpObj->sourceID=srcID;
					pTmpObj->sourceType=srcType;
					pTmpObj->tieID=pObj->tieID;
					ptNum=((TieLine*)pObj)->ptNum;
					((TieLine*)pTmpObj)->ptNum=ptNum;
					pt2D=new POINT2D[ptNum];
					pt3D=new POINT3D[ptNum];
					memcpy(pt2D, ((TieLine*)pObj)->pt2D, sizeof(POINT2D)*ptNum);
					memcpy(pt3D, ((TieLine*)pObj)->pt3D, sizeof(POINT3D)*ptNum);

					((TieLine*)pTmpObj)->pt2D=pt2D;
					((TieLine*)pTmpObj)->pt3D=pt3D;
					pTieObjList->Add(pTmpObj);
				}
				else if(pObj->objType==TO_PATCH)
				{
					pTmpObj=new TiePoint;
					pTmpObj->objType=TO_PATCH;
					pTmpObj->sourceID=srcID;
					pTmpObj->sourceType=srcType;
					pTmpObj->tieID=pObj->tieID;
					ptNum=((TiePatch*)pObj)->ptNum;
					((TiePatch*)pTmpObj)->ptNum=ptNum;
					pt2D=new POINT2D[ptNum];
					pt3D=new POINT3D[ptNum];
					memcpy(pt2D, ((TiePatch*)pObj)->pt2D, sizeof(POINT2D)*ptNum);
					memcpy(pt3D, ((TiePatch*)pObj)->pt3D, sizeof(POINT3D)*ptNum);

					for(int k=0; k<ptNum; k++)
					{
						m_imgGeo->Project(pt3D[k].X, pt3D[k].Y, pt3D[k].Z, &(pt2D[k].x), &(pt2D[k].y) );
					}

					((TiePatch*)pTmpObj)->pt2D=pt2D;
					((TiePatch*)pTmpObj)->pt3D=pt3D;
					pTieObjList->Add(pTmpObj);
				}
			}
		}
		else
		{
			for(i=0; i<pTieList->GetSize(); i++)
			{
				pChain=pTieList->GetAt(i);
				for(j=0; j<pChain->objNum; j++)
				{
					pObj=pChain->pChain->GetAt(j);

					if(pObj->sourceID!=srcID || pObj->sourceType!=srcType)
						continue;

					//找到当前影像上的点
					if(pObj->objType==TO_POINT)
					{
						pTmpObj=new TiePoint;
						pTmpObj->objType=TO_POINT;
						pTmpObj->sourceID=srcID;
						pTmpObj->sourceType=srcType;
						pTmpObj->tieID=pObj->tieID;
						((TiePoint*)pTmpObj)->pt2D=((TiePoint*)pObj)->pt2D;
						((TiePoint*)pTmpObj)->pt3D=((TiePoint*)pObj)->pt3D;
						pTieObjList->Add(pTmpObj);
					}
					else if(pObj->objType==TO_LINE)
					{
						pTmpObj=new TieLine;
						pTmpObj->objType=TO_LINE;
						pTmpObj->sourceID=srcID;
						pTmpObj->sourceType=srcType;
						pTmpObj->tieID=pObj->tieID;
						ptNum=((TieLine*)pObj)->ptNum;
						((TieLine*)pTmpObj)->ptNum=ptNum;
						pt2D=new POINT2D[ptNum];
						pt3D=new POINT3D[ptNum];
						memcpy(pt2D, ((TieLine*)pObj)->pt2D, sizeof(POINT2D)*ptNum);
						memcpy(pt3D, ((TieLine*)pObj)->pt3D, sizeof(POINT3D)*ptNum);

						((TieLine*)pTmpObj)->pt2D=pt2D;
						((TieLine*)pTmpObj)->pt3D=pt3D;
						pTieObjList->Add(pTmpObj);
					}
					else if(pObj->objType==TO_PATCH)
					{
						pTmpObj=new TiePoint;
						pTmpObj->objType=TO_PATCH;
						pTmpObj->sourceID=srcID;
						pTmpObj->sourceType=srcType;
						pTmpObj->tieID=pObj->tieID;
						ptNum=((TiePatch*)pObj)->ptNum;
						((TiePatch*)pTmpObj)->ptNum=ptNum;
						pt2D=new POINT2D[ptNum];
						pt3D=new POINT3D[ptNum];
						memcpy(pt2D, ((TiePatch*)pObj)->pt2D, sizeof(POINT2D)*ptNum);
						memcpy(pt3D, ((TiePatch*)pObj)->pt3D, sizeof(POINT3D)*ptNum);

						((TiePatch*)pTmpObj)->pt2D=pt2D;
						((TiePatch*)pTmpObj)->pt3D=pt3D;
						pTieObjList->Add(pTmpObj);
					}

				}
			}
		}
	}

}

CRegistViewerView *CRegistViewerDoc::GetMainView()
{
	POSITION	posi;
	CRegistViewerView	*pView=NULL;
	
	posi = GetFirstViewPosition();
	while(posi != NULL)
	{
		pView=(CRegistViewerView *)GetNextView(posi);
		if(pView->IsKindOf(RUNTIME_CLASS(CRegistViewerView)))
			break;
	}
	
	return pView;
}
void CRegistViewerDoc::OnAddapj()
{
	CFileDialog dlg(TRUE,"apj",NULL,OFN_FILEMUSTEXIST|OFN_OVERWRITEPROMPT,"align(*.apj)|*.apj|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString str = dlg.GetPathName();

	ExtractTieObjects(str.GetBuffer(128));
	UpdateAllViews( NULL );
}
