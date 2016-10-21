#include "StdAfx.h"
#include "imageRectify\orsImageRectify_Inor.h"
#include "orsImage\orsIImageService.h"
#include "orsImage\orsIAlgImageResampler.h"
#include "orsGuiBase\orsIProcessMsgBar.h"

#include "InorSettingDlg.h"
#include "math.h"

ORS_GET_IMAGE_SERVICE_IMPL();

orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

void ImageRectifyInor_SetPlatform(orsIPlatform *pPlatform)
{
	g_pPlatform = pPlatform;
}


///////////////////////CImgInor/////////////////////////////////////////////////
CImgInor::CImgInor()
{

}

CImgInor::~CImgInor()
{

}

void CImgInor::Initialize(CAMERA *cam, double imgRow, double imgCol)
{
	m_x0=cam->x0;
	m_y0=cam->y0;
	m_f=cam->f;
	m_k0=-cam->k0;
	m_k1=-cam->k1;
	m_k2=-cam->k2;
	m_k3=-cam->k3;
	m_p1=-cam->p1;
	m_p2=-cam->p2;

	

	//x方向
	m_inorImage_a[0] = -imgCol*cam->pixelX/2;
	m_inorImage_a[1] = cam->pixelX;
	m_inorImage_a[2] = 0;	
	//y方向
	m_inorImage_b[0] = imgRow*cam->pixelY/2;
	m_inorImage_b[1] = 0;
	m_inorImage_b[2] = -cam->pixelY;
	
	m_inorPixel_a[0] = imgCol*1.0/2;		//保证不丢失精度
	m_inorPixel_a[1] = 1/cam->pixelX;
	m_inorPixel_a[2] = 0;

	m_inorPixel_b[0] = imgRow*1.0/2;
	m_inorPixel_b[1] = 0;
	m_inorPixel_b[2] = -1/cam->pixelY;
}

bool CImgInor::Photo2Image( double x, double y, double *xi, double *yi )
{
	double xf, yf;
	CalibratedToFiducialMarkCoord( x, y, &xf, &yf );	//反算
	
	
	*xi = m_inorPixel_a[0] + m_inorPixel_a[1]*xf + m_inorPixel_a[2]*yf;
	*yi = m_inorPixel_b[0] + m_inorPixel_b[1]*xf + m_inorPixel_b[2]*yf;
	
	return true;
}

bool CImgInor::Image2Photo( double xi, double yi, double *x, double *y )
{
	double xf, yf;
	//以像片中心为原点的框标坐标系下坐标
	xf = m_inorImage_a[0] + m_inorImage_a[1]*xi + m_inorImage_a[2]*yi;
	yf = m_inorImage_b[0] + m_inorImage_b[1]*xi + m_inorImage_b[2]*yi;
	
	//畸变纠正
	FiducialMarkCoordToCalibrated(xf, yf, x, y);
	
	return true;
}

void CImgInor::FiducialMarkCoordToCalibrated( double xf, double yf, double *xc, double *yc )
{
	*xc = xf - m_x0;
	*yc = yf - m_y0;
	
	// 径向畸变
	if( m_k1 != 0 )	{
		double dx, dy, x2, y2, xy, r2;
		
		x2 = *xc* *xc;	xy = *xc * *yc;	y2 = *yc * *yc;
		r2 = x2 + y2;
		
		//Dx=x*(k1*r^2+k2*r^4+k3*r^6+...)
		//Dy=y*(k1*r^2+k2*r^4+k3*r^6+...)
		dx = *xc *(m_k0 + ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2);
		dy = *yc *(m_k0 + ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2);
		
		// 切向畸变
		if( m_p1 != 0 )	{
			dx += m_p1*( r2 + 2*x2 ) + 2*m_p2*xy;
			dy += 2*m_p1*xy + m_p2*( r2 + 2*y2 );
		}
		
		*xc += dx;
		*yc += dy;
	}
	
}

void CImgInor::CalibratedToFiducialMarkCoord( double xc, double yc, double *xf, double *yf )
{
// 	*xf = xc + m_x0;
// 	*yf = yc + m_y0;

 	*xf = xc;
 	*yf = yc;

	double xbest, ybest;
	double dxTh=m_inorImage_a[1]*0.1;	//cam->pixelX;
	double dyTh=-m_inorImage_b[2]*0.1;	//-cam->pixelY;
	double min_r2=1000, r2;
	int maxIter=50;

	if( m_k1 != 0 )	
	{	
		// 径向畸变
		double dx, dy, dr, r2 = xc*xc + yc*yc, r;
		double x2, xy, y2;
		
		double dfx, dfy;
		

		while(maxIter--)
		{
			x2 = *xf* *xf;	xy = *xf * *yf;	y2 = *yf * *yf;
			r2 = *xf* *xf + *yf* *yf;
			r=sqrt(r2);

			dr = m_k0 + ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2;
//			dr = m_k0 + ( m_k1+ ( m_k2 + m_k3*r)*r )*r;

			dx = *xf*dr;
			dy = *yf*dr;

			// 切向畸变
			if( m_p1 != 0 )	
			{
				dx += m_p1*( r2 + 2*x2 ) + 2*m_p2*xy;
				dy += 2*m_p1*xy + m_p2*( r2 + 2*y2 );
			}

			dfx = xc - ( *xf + dx);
			dfy = yc - ( *yf + dy );

			if( fabs(dfx)< dxTh && fabs(dfy)<dyTh)
				break;

			*xf = xc - dx;	
			*yf = yc - dy;

			r2=dfx*dfx+dfy*dfy;
			if(r2<min_r2)
			{
				min_r2=r2;
				xbest=*xf;
				ybest=*yf;
			}
		}
	}
	
	if(maxIter<=0)
	{
		*xf=xbest;
		*yf=ybest;
	}

	*xf += m_x0;
	*yf += m_y0;
}


////////////////////////////////////////////////////////
//CImageRectify_Inor

CImageRectify_Inor::CImageRectify_Inor()
{

}

CImageRectify_Inor::~CImageRectify_Inor()
{

}

void CImageRectify_Inor::SetSrcCamera(_iphCamera *cam)
{
	m_srcCamera.m_x0=cam->m_x0;
	m_srcCamera.m_y0=cam->m_y0;
	m_srcCamera.m_f=cam->m_f;

	m_srcCamera.m_pixelX=cam->m_pixelX;
	m_srcCamera.m_pixelY=cam->m_pixelY;
	m_srcCamera.m_k0=cam->m_k0;
	m_srcCamera.m_k1=cam->m_k1;
	m_srcCamera.m_k2=cam->m_k2;
	m_srcCamera.m_k3=cam->m_k3;
	m_srcCamera.m_p1=cam->m_p1;
	m_srcCamera.m_p2=cam->m_p2;

	m_srcCamera.InteriorOrientation(); 
}

void CImageRectify_Inor::SetSrcCamera(const char *pCamName)
{
//	ReadCamera( pCamName, &m_srcCamera );
	m_srcCamera.ReadCameraFile(pCamName);
}

bool CImageRectify_Inor::Rectify_Inor(const char *InputImgName, const char *OutputImgName)
{
	ref_ptr<orsIImageSourceReader> imgReader;
//	ref_ptr<orsIImageWriter> imgWriter;

	imgReader = getImageService()->openImageFile( InputImgName );
	
	if(imgReader.get()==0)
	{
		char str[128];
		sprintf(str, "can't open %s!\n", InputImgName);
//		AfxMessageBox(str);
		return false;
	}

	ref_ptr<orsIImageWriter> imgWriter = ORS_CREATE_OBJECT( orsIImageWriter,  ORS_IMAGE_WRITER_DEFAULT );
	
	if( !imgWriter->Create( OutputImgName, imgReader.get() ) )
	{





		getPlatform()->logPrint( ORS_LOG_ERROR, "Can not create image %s", OutputImgName );
		return false;
	}

	ref_ptr<orsIAlgImageResampler> resampler = ORS_CREATE_OBJECT(orsIAlgImageResampler, ORS_ALG_IMAGEREAMPLER_BILINEAR );
	
	if( NULL == resampler.get() )	{
		getPlatform()->logPrint( ORS_LOG_ERROR, "Can not create image resampler: %s", ORS_ALG_IMAGEREAMPLER_BILINEAR );
		return false;
	}

	ors_uint nBands=imgReader->getNumberOfOutputBands();
	int bytesOfpixel=getPlatform()->getSizeOfType(imgReader->getOutputDataType());
	BYTE *buf = new BYTE[ imgReader->getWidth()*bytesOfpixel*nBands ];
	
	orsBandSet bandSet;
	int i;
	for( i=0; i<nBands; i++ )
	{
		bandSet.push_back( i );
	}

	orsRect_i  rect=imgReader->getBoundingRect();
	orsIImageData *pSrcData = imgReader->getImageData( rect, 1.0, bandSet );
	if( NULL == pSrcData )
		return false;

	resampler->setImage( pSrcData );

	ors_int32 nRows=rect.height();
	ors_int32 nCols=rect.width();

	double xi, yi;
	double xs, ys;
	BYTE *pixelBuf=new BYTE[bytesOfpixel*nBands];
// 	BYTE *dstBuf=new BYTE[nRows*nCols*nBands];
// 	memset(dstBuf, 0, nRows*nCols*nBands);

	//无畸变像素坐标 -> 无畸变框标坐标 -> 有畸变像素坐标
	for( int iRow=0; iRow < nRows; iRow++ )
	{
		memset( buf, 0,  nCols*bytesOfpixel*nBands );
		for( int iCol=0; iCol < nCols; iCol++)
		{
			xi=iCol;
			yi=iRow;
			
			double xp, yp;
// 			m_srcInor.Image2Photo(x, y, &xs, &ys);
// 			m_srcInor.Photo2Image(xs, ys, &xi, &yi);
// 
// 			ASSERT(fabs(x-xi)<1e-4);
// 			ASSERT(fabs(y-yi)<1e-4);
			
 //			m_srcInor.Image2Photo(x, y, &xi, &yi);
//			m_dstInor.Photo2Image(xi, yi, &x, &y);

			m_dstCamera.Image2Photo(xi, yi, xp, yp);
			m_srcCamera.Photo2Image(xp, yp, xs, ys);
// 
// 			ASSERT(fabs(x-xs)<1e-4);
// 			ASSERT(fabs(y-ys)<1e-4);

// 			x=floor(xs+0.5);
// 			y=floor(ys+0.5);
// 
// 			if( x < 0 || x > nCols-1 || y < 0 || y > nRows-1 ) 
// 			{
// 				continue;						
// 			}
// 			else	
// 			{
// 				if(x==857 && y==7161)
// 				{
// 					printf("here");
// 				}
				resampler->interpolate( xs, ys, pixelBuf );
				for( int iband=0; iband<nBands; iband++)
					memcpy(buf+iband*nCols*bytesOfpixel+iCol*bytesOfpixel, pixelBuf+iband*bytesOfpixel, bytesOfpixel);
 		
// 				int i=y, j=x;
//  				int bandoffset=nCols*nRows;
//  
//  				dstBuf[i*nCols+j]=*pSrcData->getPixelValues(0, iRow*nCols+iCol);
//  				dstBuf[i*nCols+j+bandoffset]=*pSrcData->getPixelValues(1, iRow*nCols+iCol);
//  				dstBuf[i*nCols+j+bandoffset*2]=*pSrcData->getPixelValues(2, iRow*nCols+iCol);
//			}
		}
		//写一行影像
//		for( i=0; i<nBands; i++)
//		{
			imgWriter->WriteBSQRect(0, iRow, nCols, 1, buf);
//		}
	}

//	imgWriter->WriteBSQRect(0, 0, nCols, nRows, dstBuf);

	imgWriter->Close();

	if(buf)		delete buf;		buf=NULL;
	if(pixelBuf) delete pixelBuf;	pixelBuf=NULL;
//	if(dstBuf)	delete dstBuf;	dstBuf=NULL;
	return true;
}

void CImageRectify_Inor::Run()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CInorSettingDlg dlg(&m_srcImgVec, &m_srcCamera);

	if(dlg.DoModal()!=IDOK)
		return;
	
	m_OutputDir=dlg.m_strOutputDir.GetBuffer(0);

	if(m_srcImgVec.size()==0)
		return;

	ref_ptr<orsIImageSourceReader> imgReader;
	
	imgReader = getImageService()->openImageFile( m_srcImgVec[0] );
	if(imgReader.get()==0)
	{
		char str[128];
		sprintf(str, "can't open %s!\n", m_srcImgVec[0]);
		return;
	}
	
	ors_int32 imgWid=imgReader->getWidth();
	ors_int32 imgHei=imgReader->getHeight();

//	m_srcInor.Initialize(&m_srcCamera, imgHei, imgWid);
	
	m_dstCamera.m_x0=0;
	m_dstCamera.m_y0=0;
	m_dstCamera.m_f=m_srcCamera.m_f;
	m_dstCamera.m_pixelX=m_srcCamera.m_pixelX;
	m_dstCamera.m_pixelY=m_srcCamera.m_pixelY;
	m_dstCamera.m_k0=0;
	m_dstCamera.m_k1=0;
	m_dstCamera.m_k2=0;
	m_dstCamera.m_k3=0;
	m_dstCamera.m_p1=0;
	m_dstCamera.m_p2=0;
	m_dstCamera.m_imgWid = m_srcCamera.m_imgWid;
	m_dstCamera.m_imgHei = m_srcCamera.m_imgHei;

	m_dstCamera.InteriorOrientation(); 

//	m_dstCamera.Initialize(&m_dstCamera, imgHei, imgWid);
	
	ref_ptr<orsIProcessMsgBar> progressBar;
	progressBar = ORS_CREATE_OBJECT( orsIProcessMsgBar, ORS_PROCESSMSG_BAR_DEFAULT );
	progressBar->InitProgressBar( "影像纠正", "...", m_srcImgVec.size() );


	orsString srcName, dstName;
	int length, pos;
	for(int i=0; i<m_srcImgVec.size(); i++)
	{
		srcName=m_srcImgVec[i];
		
		length=srcName.length();
		pos=srcName.reverseFind('\\');
		if(pos==-1)	pos=srcName.reverseFind('/');
				
		srcName=m_srcImgVec[i].right(length-pos-1);
	
		dstName=m_OutputDir+"\\";
		dstName=dstName+srcName;

		progressBar->SetPos(i+1);
		Rectify_Inor(m_srcImgVec[i], dstName);
	}

}