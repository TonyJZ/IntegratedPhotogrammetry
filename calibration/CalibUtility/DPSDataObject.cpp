#include "stdafx.h"
#include <stdio.h>

#include <string.h>
#include <math.h>

#include <fstream>



#include "DPSDataObject.h"

CAMERA::CAMERA() 
{
	clb_r.d = NULL;
	clb_xy.d = NULL;
};


CAMERA::~CAMERA()	
{
	if( clb_r.d )
		delete clb_r.d;
	if( clb_xy.d )
		delete clb_xy.d;
}


void CAMERA::SetOritataion( int cmrOrient )
{
	double t;
	switch( cmrOrient ) {
	case 0:	
		//x0 = x0;
		//y0 = y0;
		break;
	case 1:
		t = x0;
		x0 = y0;
		y0 = -t;
		break;
	case 2:
		x0 = -x0;
		y0 = -y0;
		break;		
	case 3:
		t = x0;
		x0 = -y0;
		y0 = t;
		break;		
	case 4:
		//x0 = x0;
		y0 = -y0;
		break;
	case 5:
		t = x0;
		x0 = -y0;
		y0 = -t;
		break;		
	case 6:
		x0 = -x0;
		//y0 = y0;
		break;		
	case 7:
		t = x0;
		x0 = y0;
		y0 = t;
		break;		
	}
}


// 框标坐标到检校后影像坐标
void CAMERA::FiducialMarkCoordToCalibrated( double xf, double yf, double *x, double *y )
{
	*x = xf - x0;
	*y = yf - y0;
	
	switch( clbType )	{
	case ctNONE:
		break;
	case ctCOEFFICIENT:
		// 径向畸变
		if( k1 != 0 )	{
			double dx, dy, x2, y2, xy, r2;
			
			x2 = *x* *x;	xy = *x * *y;	y2 = *y * *y;
			r2 = x2 + y2;
			
			dx = *x * ( k1+ ( k2 + k3*r2)*r2 )*r2;
			dy = *x * ( k1+ ( k2 + k3*r2)*r2 )*r2;
			
			// 切向畸变
			if( p1 != 0 )	{
				dx += p1*( r2 + 2*x2 ) + 2*p2*xy;
				dy += 2*p1*xy + p2*( r2 + 2*y2 );
			}
			
			*x -= dx;
			*y -= dy;
		}
		break;
	case ctPROFILE_R:	// 径向畸变查表
		{
			double r = sqrt( *x * *x + *y * *y );
			
			int i = r/clb_r.dr;
			
			double dr = r - i*clb_r.dr;	// 距离最近点的偏移
			double d;
			
			if( i < 1 )	{
				d = dr*clb_r.d[0];
			}
			else if( i >= clb_r.n ) {
				i = clb_r.n - 1;
				d = clb_r.d[i];
			}
			else	{
				i--;
				d = (1-dr)*clb_r.d[i] + dr*clb_r.d[i+1];
			}
			
			*x -= d* *x/r;	// +/- ?
			*y -= d* *y/r;
		}
		break;
	case ctGRID_XY:	// 待实现
		break;
	}	
}

void CAMERA::CalibratedToFiducialMarkCoord( double x, double y, double *xf, double *yf )
{
	*xf = x;
	*yf = y;
	switch( clbType )	{
	case ctNONE:
		break;
	case ctCOEFFICIENT:
		if( k1 != 0 )	{	
			// 径向畸变
			double dx, dy, dr, r2 = x*x + y*y;
			
			dr = ( k1+ ( k2 + k3*r2)*r2 )*r2;
			dx = x*dr;
			dy = x*dr;
			
			// 切向畸变
			if( p1 != 0 )	{
				dx += p1*( r2 + 2*x*x ) + 2*p2*x*y;
				dy += 2*p1*x*y + p2*( r2 + 2*y*y );
			}
			*xf = x + dx;	// +/- ?
			*yf = y + dy;
		}
		break;
	case ctPROFILE_R:	// 径向畸变查表
		{
			if( clb_r.n > 0 )	{
				double r = sqrt( x*x + y*y );
				
				int i = r/clb_r.dr;
				
				double dr = r - i*clb_r.dr;	// 距离最近点的偏移
				double d;
				
				if( i < 1 )	{
					d = dr*clb_r.d[0];
				}
				else if( i >= clb_r.n ) {
					i = clb_r.n - 1;
					d = clb_r.d[i];
				}
				else	{
					i--;
					d = (1-dr)*clb_r.d[i] + dr*clb_r.d[i+1];
				}
				
				*xf = x + d*x/r;	// +/- ?
				*yf = y + d*y/r;
			}
		}
		break;
	case ctGRID_XY:	// 待实现
		break;
	}
	
	
	// 主点
	*xf += x0;
	*yf += y0;	
}





CImage_Inor::CImage_Inor()
{

}

CImage_Inor::~CImage_Inor()
{

}


CMetricScanImage::CMetricScanImage()
{

}

CMetricScanImage::~CMetricScanImage()
{


}

void CMetricScanImage::FiducialMarkCoordToPixel( double xf, double yf, double *xi, double *yi )
{
	*xi = m_inor.fm2p_x[0] + m_inor.fm2p_x[1] * xf + m_inor.fm2p_x[2] * yf;
	*yi = m_inor.fm2p_y[0] + m_inor.fm2p_y[1] * xf + m_inor.fm2p_y[2] * yf;
}


void CMetricScanImage::PixelToFiducialMarkCoord( double xi, double yi, double *xf, double *yf )
{
	*xf = m_inor.p2fm_x[0] + m_inor.p2fm_x[1] * xi + m_inor.p2fm_x[2] * yi;
	*yf = m_inor.p2fm_y[0] + m_inor.p2fm_y[1] * xi + m_inor.p2fm_y[2] * yi;
}

//////////////////////////////////////////////////////////////////////////


// 框标坐标到检校后影像坐标
void CMetricScanImage::FiducialMarkCoordToCalibrated( double xf, double yf, double *x, double *y )
{
	m_cmr.FiducialMarkCoordToCalibrated( xf,yf, x,y);
}


void CMetricScanImage::PixelToImageCalibrated( double xi, double yi, double *x, double *y )
{
	double xf, yf;

	xf = m_inor.p2fm_x[0] + m_inor.p2fm_x[1] * xi + m_inor.p2fm_x[2] * yi;
	yf = m_inor.p2fm_y[0] + m_inor.p2fm_y[1] * xi + m_inor.p2fm_y[2] * yi;

	FiducialMarkCoordToCalibrated( xf, yf, x, y );
}



void CMetricScanImage::CalibratedToFiducialMarkCoord( double x, double y, double *xf, double *yf )
{
	m_cmr.CalibratedToFiducialMarkCoord( x,y,xf,yf);
}



void CMetricScanImage::ImageCalibratedToPixel( double x, double y, double *xi, double *yi )
{
	double xf, yf;
	
	CalibratedToFiducialMarkCoord( x, y, &xf, &yf );
	
	*xi = m_inor.fm2p_x[0] + m_inor.fm2p_x[1] * xf + m_inor.fm2p_x[2] * yf;
	*yi = m_inor.fm2p_y[0] + m_inor.fm2p_y[1] * xf + m_inor.fm2p_y[2] * yf;
}



bool CMetricScanImage::Initialize( const char *imageFileName, const char *prjDir )
{
	//ReadImgInfo( imageFileName, &m_imgInfo, prjDir, 0, false );
	ReadCamera( m_imgInfo.cmrFile, &m_cmr );

	m_cmr.SetOritataion( m_imgInfo.cmrOrient );

	ReadInorFile( imageFileName, &m_inor );

	return true;
}



//////////////////////////////////////////////////////////////////////////
// CStereoModel *LoadMetricFrameStereoModel( const char *modelName );
// CStereoModel *LoadEpipolarStereoModel( const char *modelName );
// CStereoModel *LoadRPCStereoModel( const char *modelName );
// CStereoModel *LoadAds40StereoModel( const char *modelName );
//
//
//	装载立体模型
//
//
// CStereoModel *LoadStereoModel( const char *modelName )
// {
// 	CStereoModel *pStereoModel = LoadEpipolarStereoModel( modelName );
// 	if( NULL != pStereoModel )
// 		return pStereoModel;
// 
// 	pStereoModel = LoadAds40StereoModel(modelName);
// 	if( NULL != pStereoModel )
// 		return pStereoModel;
// 
// 	pStereoModel = LoadRPCStereoModel( modelName );
// 
// 	return pStereoModel;
// }



//////////////////////////////////////////////////////////////////////////
// CImageRay *LoadRPC( const orsString &imageFileName );
// CImageRay *LoadExtOrElement( const orsString &imageFileName );
// CImageRay *LoadADS40( const orsString &suppertFileName );
// 
// CImageRay *LoadImageAffine( const orsString &domFileName );

//
//
//	装载影像光束
//
//
// CImageRay *LoadImageRay( const char *imageName )
// {
// 	CImageRay *pImgRay;
// 
//  	pImgRay = LoadADS40( imageName );	
//  	if( NULL != pImgRay )
//  		return pImgRay;
// 
// 	
// 	pImgRay = LoadRPC( imageName );
// 
// 	if( NULL != pImgRay )
// 		return pImgRay;
// 
// 
// 	pImgRay = LoadExtOrElement( imageName );
// 		
// 	if( NULL != pImgRay )
// 			return pImgRay;
// 		
// 	pImgRay = LoadImageAffine( imageName );
// 	
// 	if( NULL != pImgRay )	
// 		return pImgRay;
// }