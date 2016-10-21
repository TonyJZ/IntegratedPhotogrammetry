#ifndef _ORS_IMAGE_RECTIFY_H_zj_2011_07_01_
#define _ORS_IMAGE_RECTIFY_H_zj_2011_07_01_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#include "DPSDataType.h"
#include "orsImage\orsIImageSourceReader.h"
#include "orsImage\orsIImageWriter.h"

#include "Calib_Camera.h"


#ifdef _Image_Rectify_Inor_LIB_
#define  _ImgRectify_  ORS_EXPORT
#else
#define  _ImgRectify_  ORS_IMPORT	
#endif

#ifndef _Image_Rectify_Inor_LIB_
#ifdef _DEBUG
#pragma comment(lib,"ImageRectifyD.lib")
#else
#pragma comment(lib,"ImageRectify.lib")
#endif
#endif


class CImgInor
{
public:
	CImgInor();
	~CImgInor();

	void Initialize(CAMERA *cam, double imgRow, double imgCol);

	bool Photo2Image( double x, double y, double *xi, double *yi );
	bool Image2Photo( double xi, double yi, double *x, double *y );
	
	// 框标坐标到检校后影像坐标
	virtual void FiducialMarkCoordToCalibrated( double xf, double yf, double *xc, double *yc );
	virtual void CalibratedToFiducialMarkCoord( double xc, double yc, double *xf, double *yf );


private:
	double m_inorImage_a[3];	// pixel to image
	double m_inorImage_b[3];
	
	double m_inorPixel_a[3];	// image to pixel
	double m_inorPixel_b[3];
	
	// principle point, focal length
	double m_x0, m_y0, m_f;

	double	m_k0, m_k1, m_k2, m_k3;
	double	m_p1, m_p2;


private:
	std::vector<orsString>	m_srcImgVec;
};


//纠正影像内部畸变
class _ImgRectify_ CImageRectify_Inor
{
public:
	CImageRectify_Inor();
	~CImageRectify_Inor();

	void Run();


	bool Rectify_Inor(const char *InputImgName, const char *OutputImgName);

	void SetSrcCamera(_iphCamera *cam);
	void SetSrcCamera(const char *pCamName);

private:
	_iphCamera		m_srcCamera;
	_iphCamera		m_dstCamera;

// 	ref_ptr<orsIImageSourceReader> m_imgReader;
// 	ref_ptr<orsIImageWriter> m_imgWriter;

// 	CImgInor	m_srcInor;
// 	CImgInor	m_dstInor;

	std::vector<orsString>	m_srcImgVec;
	orsString				m_OutputDir;
};


void _ImgRectify_ ImageRectifyInor_SetPlatform(orsIPlatform *pPlatform);

#endif