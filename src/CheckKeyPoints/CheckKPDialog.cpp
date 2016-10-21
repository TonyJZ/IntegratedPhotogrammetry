// CheckKPDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CheckKeyPoints.h"
#include "CheckKPDialog.h"
#include "orsImage/orsIAlgImageResampler.h"
#include "gui\orsDispalyApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckKPDialog dialog
// orsIPlatform* g_Platform=NULL;
// orsISpatialReferenceService *g_pSRService = NULL;
// orsISpatialReferenceService *getSRService()
// {
// 	if( NULL == g_pSRService )
// 		g_pSRService = ORS_PTR_CAST(orsISpatialReferenceService, g_Platform->getService( ORS_SERVICE_SRS ) );
// 	
// 	return g_pSRService;
// } 
orsIPlatform *g_pPlatform = NULL;
orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

#include "orsSRS\orsISpatialReferenceService.h"
orsISpatialReferenceService *g_pSRService = NULL;
orsISpatialReferenceService *getSRService()
{
	if( NULL == g_pSRService )
		g_pSRService = ORS_PTR_CAST(orsISpatialReferenceService, getPlatform()->getService( ORS_SERVICE_SRS ) );
	
	return g_pSRService;
}

void CheckKeyPoints(orsIPlatform* pPlatform)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	g_pPlatform=pPlatform;

	CCheckKPDialog dlg(pPlatform);
	//	dlg.m_pPlatform = pPlatform;
	
	if( dlg.DoModal() == IDOK )
	{
	}
}

CCheckKPDialog::CCheckKPDialog(orsIPlatform* platform, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CCheckKPDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckKPDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCPtFileOpen=false;
	m_pPlatform=platform;

	FastDspSetPlatform(m_pPlatform);

	m_pKPbuf=0;
	m_KPnum=0;
}


void CCheckKPDialog::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckKPDialog)
	DDX_Control(pDX, IDC_LIST_CTRL, m_ListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckKPDialog, CResizableDialog)
	//{{AFX_MSG_MAP(CCheckKPDialog)
	ON_BN_CLICKED(IDC_OpenKP, OnOpenKP)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_OpenImage, OnOpenImage)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_CLICK, IDC_LIST_CTRL, OnClickListCtrl)
	ON_BN_CLICKED(IDC_ImageViewer, OnImageViewer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckKPDialog message handlers

void CCheckKPDialog::OnOpenKP() 
{
	CFileDialog cdlg( TRUE, NULL, NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("sift key Points Files (*.sift.txt)|*.sift.txt|(*.sift.bin)|*.sift.bin|All Files(*.*)|*.*||"));   
	int i;
	CString		strFilePath;

	if(cdlg.DoModal() == IDOK)
	{
		strFilePath = cdlg.GetPathName();
	}
	else
		return;
	
	
	FILE	*fp=NULL;
	int	kpNum, descriptorNum;
	double	x,y,s,o;
	int	*iDiscribles=0;
	
	fp=fopen(strFilePath.GetBuffer(256), "rt");
	
	fscanf(fp, "%d %d\n", &kpNum, &descriptorNum);
	
	if(kpNum>m_KPnum)
	{
		if(m_pKPbuf)
		{
			delete[] m_pKPbuf;
		}
		m_KPnum=kpNum;
		m_pKPbuf=new KeyPoint[m_KPnum];
	}
	if(descriptorNum>0)
	{
		iDiscribles=new int[descriptorNum];	//描述符
	}
	
	for(i=0; i<m_KPnum; i++)
	{
		fscanf(fp, "%lf %lf %lf %lf\n", &x, &y,  &s, &o);
		m_pKPbuf[i].x=x;
		m_pKPbuf[i].y=y;
		m_pKPbuf[i].scale=s;
		m_pKPbuf[i].orientation=o;
		
		for (int line = 0; line < 7; line++)
		{			
			if (line < 6)
			{
				fscanf(fp, 
					"%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n", 
					//"%.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf", 
					&iDiscribles[line*20], &iDiscribles[line*20+1], &iDiscribles[line*20+2], 
					&iDiscribles[line*20+3], &iDiscribles[line*20+4], &iDiscribles[line*20+5], 
					&iDiscribles[line*20+6], &iDiscribles[line*20+7], &iDiscribles[line*20+8], 
					&iDiscribles[line*20+9], &iDiscribles[line*20+10], &iDiscribles[line*20+11], 
					&iDiscribles[line*20+12], &iDiscribles[line*20+13], &iDiscribles[line*20+14], 
					&iDiscribles[line*20+15], &iDiscribles[line*20+16], &iDiscribles[line*20+17], 
					&iDiscribles[line*20+18], &iDiscribles[line*20+19]);			
			}
			else 
			{
				fscanf(fp, 
					"%3d %3d %3d %3d %3d %3d %3d %3d\n",
					//"%.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf\n",
					&iDiscribles[line*20], &iDiscribles[line*20+1], &iDiscribles[line*20+2], 
					&iDiscribles[line*20+3], &iDiscribles[line*20+4], &iDiscribles[line*20+5], 
					&iDiscribles[line*20+6], &iDiscribles[line*20+7]);
			}
		}
		
	}
	
	m_bCPtFileOpen=true;
	
	if(fp)
	{
		fclose(fp);
		fp=NULL;
	}
	if(iDiscribles)
	{
		delete[] iDiscribles;
		iDiscribles=0;
	}
	
	SetKeyPointsList();
	Invalidate(TRUE);
}

void CCheckKPDialog::SetKeyPointsList()
{
	for (int i=0; i<m_KPnum; i++)
	{	
		CString sPtName;
		sPtName.Format("%d", i);
		m_ListCtrl.InsertItem(i,sPtName.GetBuffer(128));
        
		CString sX;
		sX.Format("%.4lf",m_pKPbuf[i].x);
		m_ListCtrl.SetItemText(i,1, sX.GetBuffer(128));
		
		CString sY;
		sY.Format("%.4lf",m_pKPbuf[i].y);
		m_ListCtrl.SetItemText(i,2,sY.GetBuffer(128));
		
		CString scZ;
		scZ.Format("%.4lf",m_pKPbuf[i].scale);
		m_ListCtrl.SetItemText(i,3,scZ.GetBuffer(128));
		
		CString slZ;
		slZ.Format("%.4lf",m_pKPbuf[i].orientation);
		m_ListCtrl.SetItemText(i,4,slZ.GetBuffer(128));
	}
	
	UpdateData(FALSE);		
}

BOOL CCheckKPDialog::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon

    m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle()|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	m_ListCtrl.InsertColumn(0,"ID",LVCFMT_LEFT,50);
	m_ListCtrl.InsertColumn(1,"X",LVCFMT_LEFT,100); 
	m_ListCtrl.InsertColumn(2,"Y",LVCFMT_LEFT,100); 
	m_ListCtrl.InsertColumn(3,"scale",LVCFMT_LEFT,100); 
	m_ListCtrl.InsertColumn(4,"orientation",LVCFMT_LEFT,100);
	
	m_pView = new CImageViewerView;  
	m_pView->SubclassDlgItem(IDC_ImageViewer, this);
	
	GetDlgItem(IDC_OpenKP)->EnableWindow(FALSE);


	AddAnchor(IDC_ImageViewer,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_LIST_CTRL,BOTTOM_LEFT,BOTTOM_RIGHT);
    AddAnchor(IDC_OpenImage,BOTTOM_LEFT,BOTTOM_LEFT);
    AddAnchor(IDC_OpenKP,BOTTOM_CENTER,BOTTOM_CENTER);
	AddAnchor(IDOK,BOTTOM_RIGHT,BOTTOM_RIGHT);

	
  	              // EXCEPTION: OCX Property Pages should return FALSE
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCheckKPDialog::OnDestroy() 
{
	if(m_pView)   
	{   
		m_pView->UnsubclassWindow();   
		delete   m_pView;   
	}   

	if(m_pKPbuf)
	{
		delete[] m_pKPbuf;
		m_pKPbuf=NULL;
	}

	CResizableDialog::OnDestroy();
}

#include "orsImage/orsIImageSourceZoomer.h"
#include "orsImage/orsIImageSourceRotator.h"
void CCheckKPDialog::OnOpenImage() 
{
	CFileDialog	dlg(TRUE,"影像",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"jpg file(*.jpg)|*.jpg|tif file(*.tif)|*.tif|img file(*.img)|*.img|* file(*.*)|*.*||",NULL);
	
	//CFileDialog dlg1(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filterExt, NULL);
	
	dlg.m_ofn.lpstrTitle = "Open an Image";
	
	if( dlg.DoModal() != IDOK ) return ;
	
	CString fileName = dlg.GetPathName();
	
	if(m_pPlatform==NULL)
		return;
	
	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, m_pPlatform->getService(ORS_SERVICE_IMAGE) );
	
	assert(imageService != NULL );
	
	//创建影像链
	ref_ptr<orsIImageChain> imageChain = imageService->CreateImageChain();
	
	
	
	//	ref_ptr<orsIImageSourceRotator> rotator = 
	//		ORS_CREATE_OBJECT(orsIImageSourceRotator, m_pPlatform, ORS_IMAGESOURCE_ROTATER_DEFAULT );
	
	//	rotator->setRotateMode( ORS_rmNONE );		//影像旋转
	
	ref_ptr<orsIImageSourceZoomer>  zoomer = 
 		ORS_CREATE_OBJECT( orsIImageSourceZoomer,   ORS_IMAGESOURCE_ZOOM_DEFALUT );
	
	zoomer->setResampleMode( ORS_rsmNEAREST );
	
	//	ref_ptr<orsIImageSource>  imageCache = imageService->CreateImageCache();
	
	//	ref_ptr<orsIImageCache> pImageCache  = ORS_PTR_CAST(orsIImageCache, imageCache.get());
	//////////////////////////////////////////////////////////////////////////
	//读取影像
	ref_ptr<orsIImageSourceReader> imgReader = imageService->openImageFile(fileName);
	
	if( NULL == imgReader.get() )	{
		AfxMessageBox( "Can not open image");
		return;
	}
	
	// 	ref_ptr<orsIImageSource> imgMapper = 
	// 		ORS_CREATE_OBJECT( orsIImageSource, m_pPlatform, "ors.dataSource.image.mapper.2stdv");
	// 	
	// 	ref_ptr<orsIImageSource> bandSelect = 
	// 		ORS_CREATE_OBJECT( orsIImageSource, m_pPlatform, "ors.dataSource.image.bandselect.default");
	// 	
	// 	ref_ptr<orsIImageSource> pLarge2Byte
	// 		= ORS_CREATE_OBJECT( orsIImageSource, m_pPlatform, "ors.dataSource.image.Large2Byte");
	
	//	ref_ptr<orsIImageSource> complexFiter
	//		= ORS_CREATE_OBJECT( orsIImageSource, m_pPlatform, "ors.dataSource.image.ComplexBandDecompose");	
	
	
	//	ref_ptr<orsIAlgImageFilter> algFilter = 
	//		ORS_CREATE_OBJECT(orsIAlgImageFilter, m_pPlatform, "ors.algorithm.imageFilter.mean"  );
	
	//	ref_ptr<orsIImageSource> sourceFilter = ORS_PTR_CAST( orsIImageSource, m_pPlatform->CreateConnectableObject( ORS_PTR_CAST(orsIObject, algFilter) ) );
	
	ref_ptr<orsIImageSource> imgMapper = 
		ORS_CREATE_OBJECT( orsIImageSource,   "ors.dataSource.image.mapper.2stdv");
	
	ref_ptr<orsIImageSource> bandSelect = 
		ORS_CREATE_OBJECT( orsIImageSource,   "ors.dataSource.image.bandselect.default");
	
	ref_ptr<orsIImageSource> pLarge2Byte
		= ORS_CREATE_OBJECT( orsIImageSource,   "ors.dataSource.image.Large2Byte");
	
	ref_ptr<orsIImageSource> complexFiter
		= ORS_CREATE_OBJECT( orsIImageSource,   "ors.dataSource.image.complex2Magnitude");	
	
		
//	ref_ptr<orsIImageSource> sourceFilter = ORS_PTR_CAST( orsIImageSource, pPlatform->CreateConnectableObject( ORS_PTR_CAST(orsIObject, algFilter) ) );


	imageChain->add( imgReader.get() );
	//	m_imageChain->add(complexFiter.get());
	//	m_imageChain->add( bandSelect.get());
	
	imageChain->add( zoomer.get() );

	imageChain->add(imgMapper.get());
	
	//	m_imageChain->add( sourceFilter.get() );
	
	//	m_imageChain->add( imgMapper.get());
	
	// 	if (pLarge2Byte.get())
	// 	{
	// 		m_imageChain->add(pLarge2Byte.get());
	//	}

	GetDlgItem(IDC_OpenKP)->EnableWindow(TRUE);
//	m_pImg =  ORS_PTR_CAST( orsIImageSource, imgReader );
	m_pImg =  ORS_PTR_CAST( orsIImageSource, imageChain );
	m_pView->SetImage(m_pImg.get());
	m_pView->SetOpState(CSingleImageView::opPAN);
//	m_pView->ShowCursor();
	
	
	
//	UpdateData(FALSE);
	
}

void CCheckKPDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CResizableDialog::OnOK();
}

void CCheckKPDialog::OnSize(UINT nType, int cx, int cy) 
{
	CResizableDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}

void CCheckKPDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetDlgItem(IDC_ImageViewer)->GetWindowRect(&rect);
	
	if(rect.PtInRect(point))
	{
		GetDlgItem(IDC_ImageViewer)->SetFocus();
	}
	
	CResizableDialog::OnMouseMove(nFlags, point);
}

BOOL CCheckKPDialog::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	CRect rect;
	GetDlgItem(IDC_ImageViewer)->GetWindowRect(&rect);
	
	if(rect.PtInRect(pt))
	{
		GetDlgItem(IDC_ImageViewer)->SetFocus();
	}
	
	return CResizableDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CCheckKPDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetDlgItem(IDC_ImageViewer)->GetWindowRect(&rect);
	
	if(rect.PtInRect(point))
	{
		GetDlgItem(IDC_ImageViewer)->SetFocus();
	}

	CResizableDialog::OnLButtonDown(nFlags, point);
}

void CCheckKPDialog::OnClickListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;   
    UINT flag;   
    int nItem = m_ListCtrl.HitTest(pNMListView->ptAction,&flag); 
	
	if(m_pView->m_SelectPointNo == nItem)
	{
		m_pView->m_bSelPoint=false;
	}
	else
	{
		m_pView->m_bSelPoint=true;
		m_pView->m_SelectPointNo = nItem;
	}

    if(flag & LVHT_ONITEMSTATEICON)   
    {   
        m_ListCtrl.SetItem(nItem, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);//??CheckBox????Item   
    }   
	Invalidate();
    *pResult = 0;   
}

void CCheckKPDialog::OnImageViewer() 
{
	GetDlgItem(IDC_ImageViewer)->SetFocus();
	
}
