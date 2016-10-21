// imgviewerdlg.cpp : implementation file
//

#include "stdafx.h"
//#include "fastdisplay.h"
#include "imgviewerdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "orsImage/orsIImageSourceMemory.h"
#include "orsImage/orsIImageSourceMapper.h"

#include "gui/orsDispalyApi.h"

// void DisplayImage( const orsChar *titleStr, const orsIImageData *pImgData, bool bWait )
// {
// 	AFX_MANAGE_STATE(AfxGetStaticModuleState());
// 
// 	ref_ptr<orsIImageSource> pImgSource;
// 	ref_ptr<orsIImageSourceMemory> pImgSourceMemory;
// 
// 	pImgSourceMemory = ORS_CREATE_OBJECT( orsIImageSourceMemory, ORS_IMAGESOURCE_MEMORY_DEFAULT );
// 	
// 	pImgSourceMemory->Init( (orsIImageData *)pImgData );
// 
// 	pImgSource = pImgSourceMemory.get();
// 
// 	if( ORS_DT_BYTE != pImgData->getDataType() )	{
// 		ref_ptr<orsIImageSourceMapper> pImgSourceMapper;
// 		
// 		pImgSourceMapper = ORS_CREATE_OBJECT( orsIImageSourceMapper, ORS_IMAGESOURCE_MAPPER_DEFAULT );
// 		
// 		pImgSourceMapper->connect( pImgSource.get() );
// 
// 		pImgSource = pImgSourceMapper.get();
// 	}
// 
// 
// 	CImgViewerDlg *dlg = new CImgViewerDlg( titleStr, pImgSource.get() );
// 	
// 	dlg->Create( CImgViewerDlg::IDD );
// 	
// 	dlg->ShowWindow( SW_SHOW );
// 
// 	if( bWait ) {
// 		MSG message;
// 		
// 		while( 1 ) {
// 			if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
// 			{
// 				::TranslateMessage(&message);
// 				::DispatchMessage(&message);
// 
// 				if( dlg->m_bQuit )
// 					break;
// 			}
// 		}		
// 	}
// }
// 
// 
// void DisplayImage( const orsChar *titleStr, BYTE *pBuf, int wid, int hei, orsDataTYPE dataType, bool bWait )
// {
// 	ref_ptr<orsIImageData> pImgData = getImageService()->CreateImageData();
// 
// 	orsRect_i rect;
// 	rect.m_xmin = 0;	rect.m_xmax = wid;
// 	rect.m_ymin = 0;	rect.m_ymax = hei;
// 
// 	orsBandSet bandset;
// 
// 	bandset.push_back( 0 );
// 	pImgData->create( dataType, rect, bandset );
// 
// 	switch( dataType )	{
// 	case ORS_DT_BYTE:
// 		memcpy( pImgData->getBandBuf(0), pBuf, wid*hei );
// 		break;
// 	case ORS_DT_INT16:
// 	case ORS_DT_UINT16:
// 		memcpy( pImgData->getBandBuf(0), pBuf, wid*hei*sizeof(ors_int16) );
// 		break;
// 	case ORS_DT_INT32:
// 	case ORS_DT_UINT32:
// 	case ORS_DT_FLOAT32:
// 		memcpy( pImgData->getBandBuf(0), pBuf, wid*hei*sizeof(ors_int32) );
// 		break;
// 	case ORS_DT_FLOAT64:
// 		memcpy( pImgData->getBandBuf(0), pBuf, wid*hei*sizeof(ors_float64) );
// 		break;
// 	}
// 
// 	/////////////////////////////////////////////
// 
// 	DisplayImage( titleStr, pImgData.get(), bWait );
// 
// }
// 
// 
// 
// void DisplayImage( const orsChar *titleStr, orsIImageSource *pImg )
// {
// 	AFX_MANAGE_STATE(AfxGetStaticModuleState());
// 
// 	ref_ptr<orsIImageSourceMapper> pImgSourceMapper;
// 
// 	if( ORS_DT_BYTE != pImg->getOutputDataType() )	{
// 		ref_ptr<orsIImageSourceMapper> pImgSourceMapper;
// 		
// 		pImgSourceMapper = ORS_CREATE_OBJECT( orsIImageSourceMapper, ORS_IMAGESOURCE_MAPPER_DEFAULT );
// 		
// 		pImgSourceMapper->connect( pImg );	
// 
// 		pImg = pImgSourceMapper.get();
// 	}
// 
// 	CImgViewerDlg *dlg = new CImgViewerDlg( titleStr, pImg );
// 	dlg->Create( CImgViewerDlg::IDD );
// 	
// 	dlg->ShowWindow( SW_SHOW );
// }

/////////////////////////////////////////////////////////////////////////////
// CImgViewerDlg dialog


CImgViewerDlg::CImgViewerDlg( const orsChar *titleStr, ref_ptr<orsIImageSource> pImg, CWnd* pParent /*=NULL*/)
	: CResizableDialog(CImgViewerDlg::IDD, pParent)
{
	m_pView = NULL;
	m_pImg = pImg;

	m_titleStr = titleStr;

	m_bQuit = false;
}

CImgViewerDlg::~CImgViewerDlg()
{

}

void CImgViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImgViewerDlg)
//	DDX_Control(pDX, IDC_ImageViewer, m_wndImgViewer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImgViewerDlg, CResizableDialog)
	//{{AFX_MSG_MAP(CImgViewerDlg)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(IDOK, OnOk)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgViewerDlg message handlers

void CImgViewerDlg::OnDestroy() 
{
	if(m_pView)   
	{   
		m_pView->UnsubclassWindow();   
		delete   m_pView;   
		m_pView=NULL;
	}

	m_pImg = NULL;
	ref_ptr<orsIImageChain>  m_imageChain = NULL;
 
	CResizableDialog::OnDestroy();
}

void CImgViewerDlg::DisplayImage(const orsChar *titleStr, ref_ptr<orsIImageSource> pImg)
{
	m_titleStr = titleStr;
	SetWindowText( titleStr );
	m_pImg = pImg;

	if( NULL != m_pImg.get() )
		SetImageSource( m_pImg );
}

void CImgViewerDlg::drawPoints(orsPOINT2D &pts2d)
{
	m_pView->DrawPoint( 0, pts2d, 0, 1);
	m_pView->DriveTo( pts2d );
}

void CImgViewerDlg::drawPoints(orsPOINT3D &pts3d)
{
	m_pView->DrawPoint( pts3d, 0, 1);
	m_pView->DriveTo( pts3d );
}

BOOL CImgViewerDlg::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon

	SetWindowText( m_titleStr );

	m_pView = new CImageViewer;
	
	m_pView->SubclassDlgItem(IDC_ImageViewer, this);

	AddAnchor( IDC_ImageViewer, TOP_LEFT, BOTTOM_RIGHT);

	if( NULL != m_pImg.get() )
		SetImageSource( m_pImg );

// 	AddAnchor(IDC_OpenImage,BOTTOM_LEFT, BOTTOM_LEFT);
// 	AddAnchor(IDOK,BOTTOM_RIGHT, BOTTOM_RIGHT);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString GetFilterExt(orsIPlatform *pPlatform)
{
	CString filterExt;
	char buf[80];	
	
	//orsFileFormatList l = GetImageFormatList();
	ref_ptr<orsIImageService> imgService = 
		ORS_PTR_CAST(orsIImageService, pPlatform->getService( ORS_SERVICE_IMAGE ) );
	
	orsFileFormatList l = imgService->getSupportedImageFormats();
	
	for( int i=0; i<l.size(); i++ )
	{
		sprintf( buf, "%s(*.%s)|*.%s|", l[i].name.c_str(), l[i].ext.c_str(),l[i].ext.c_str() );
		filterExt += buf;
	}
	
	filterExt += "|";
	return filterExt;
}

#include "orsImage/orsIImageSourceZoomer.h"
#include "orsImage/orsIImageService.h"
#include "orsImage/orsIImageSourceRotator.h"


void CImgViewerDlg::OnFileOpen() 
{

	CString filterExt = GetFilterExt( getPlatform() );	

	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filterExt, NULL);
	
	//CFileDialog dlg1(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filterExt, NULL);
	
	dlg.m_ofn.lpstrTitle = "Open an Image";
	
	if( dlg.DoModal() != IDOK ) return ;
	
	CString fileName = dlg.GetPathName();
	
	if( getPlatform() ==NULL)
		return;
	
	ref_ptr<orsIImageService>  imageService = getImageService();

	assert(imageService != NULL );
	
	ref_ptr<orsIImageSource> imgReader = imageService->openImageFile(fileName.GetBuffer(0));

	if( NULL == imgReader.get() )	{
		AfxMessageBox( "Can not open image");
		return ;
	}

	SetImageSource( imgReader );
}

#include "orsImage/orsIImageSourceMapper.h"
#include "orsImage/orsIImageSourceWarper_refImg.h"
void CImgViewerDlg::SetImageSource( ref_ptr<orsIImageSource> pImg )
{
	ref_ptr<orsIImageService>  imageService = getImageService();
	
	assert(imageService != NULL );

	if( m_pImg.get() != pImg.get() )
		m_pImg = pImg;

	ref_ptr<orsIImageChain> imageChain = imageService->CreateImageChain();

	ref_ptr<orsIImageSource> complex2Mag;
	
	// 复数？ 加上幅度 计算节点
	if( pImg->getOutputDataType(0) > ORS_DT_FLOAT64 )
		complex2Mag = ORS_CREATE_OBJECT( orsIImageSource, ORS_IMAGESOURCE_MAPPER_DEFAULT );

	// 放大器
	orsIImageSourceWarper_refImg *pWarpedImg = ORS_PTR_CAST( orsIImageSourceWarper_refImg, pImg );

	ref_ptr<orsIImageSourceZoomer>  zoomer;
	
	if( NULL == pWarpedImg )
		zoomer = ORS_CREATE_OBJECT( orsIImageSourceZoomer, ORS_IMAGESOURCE_ZOOM_DEFALUT );

	// 拉伸器
	ref_ptr<orsIImageSourceMapper> imgMapper =
		ORS_CREATE_OBJECT( orsIImageSourceMapper, "ors.dataSource.image.mapper.2stdv");
	
	// 	ref_ptr<orsIImageSource> bandSelect =
	// 		ORS_CREATE_OBJECT( orsIImageSource, pPlatform, "ors.dataSource.image.bandselect.default");
	
	// 这个和imgMapper功能重复，转换到8bit，用于显示
	ref_ptr<orsIImageSource> pLarge2Byte;
	
	if( NULL == imgMapper.get() ) {
		if( pImg->getOutputDataType(0) != ORS_DT_BYTE )
			pLarge2Byte	= ORS_CREATE_OBJECT( orsIImageSource, "ors.dataSource.image.Large2Byte");
	}
	
	imageChain->add( pImg.get() );
	
	if( complex2Mag.get() )
		m_imageChain->add( complex2Mag.get() );
	
	if( NULL != zoomer.get() )	{
		zoomer->setResampleMode( ORS_rsmNEAREST );
		imageChain->add( zoomer.get() );
	}
	
   	if( imgMapper.get() ) {
		imageChain->add( imgMapper.get() );

		imgMapper->setStretchType( ORS_STRETCHTYPE_MINMAX, 1, 0  );
	}

	if( NULL != pLarge2Byte.get() )
		imageChain->add(pLarge2Byte.get());

	m_imageChain = imageChain;
	
	m_pView->SetImage( m_imageChain.get() );
	
	m_pView->SetOpState( CSingleImageView::opPAN );

 	orsPOINT2D imgCenter;
 	imgCenter.x = m_pImg->getWidth()/2;
 	imgCenter.y = m_pImg->getHeight()/2;

	CRect rect;

	GetClientRect( &rect );
	POINT pt;

	pt.x = rect.Width() /2 ;
	pt.y = rect.Height() /2 ;

	m_pView->SetZoom( pt , 1 );

//	m_pView->DriveTo(imgCenter);

}

void CImgViewerDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CResizableDialog::OnLButtonDown(nFlags, point);
}

BOOL CImgViewerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
// 	CRect rect;
// 	GetDlgItem(IDC_ImageViewer)->GetWindowRect(&rect);
// 	
// 	if(rect.PtInRect(pt))
// 	{
// 		GetDlgItem(IDC_ImageViewer)->SetFocus();
// 	}
	
	return CResizableDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CImgViewerDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
// 	CRect rect;
// 	GetDlgItem(IDC_ImageViewer)->GetWindowRect(&rect);
// 	
// 	if(rect.PtInRect(point))
// 	{
// 		GetDlgItem(IDC_ImageViewer)->SetFocus();
// 	}
	CResizableDialog::OnMouseMove(nFlags, point);
}

void CImgViewerDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CResizableDialog::OnRButtonDown(nFlags, point);
}



void CImgViewerDlg::OnOk() 
{
	// TODO: Add your command handler code here
	m_bQuit = true;

	OnDestroy();

	CDialog::OnOK();
}

void CImgViewerDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_bQuit = true;

	OnDestroy();

	CResizableDialog::OnClose();
}
