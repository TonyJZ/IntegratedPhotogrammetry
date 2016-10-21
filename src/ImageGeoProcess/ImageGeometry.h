// create by zj 2011.2
// 影像几何处理
#ifndef _IMAGE_GEOMETRY_H_ZJ_20110224_
#define _IMAGE_GEOMETRY_H_ZJ_20110224_


#ifdef _IMGGEOPROC_
#define  _ImgGeo_Dll_  __declspec(dllexport)
#else
#define  _ImgGeo_Dll_  __declspec(dllimport)	
#endif

#ifndef _IMGGEOPROC_
#ifdef _DEBUG
#pragma comment(lib,"ImageGeoProcess.lib")
#else
#pragma comment(lib,"ImageGeoProcess.lib")
#endif
#endif

#include "IPhBaseDef.h"


struct	PointPAIR	
{
	int	 isGross;
	float coef;		// as weight
	double xl, yl;	//pixel
	double xr, yr;
};

// *.rop, *.aop, relative orientation
struct	modelPARA	
{	
	double  	XsL,YsL,ZsL;
	double  	Rl[9];
	double	XsR,YsR,ZsR;
	double	Rr[9];
};

//独立模型相对定向
class _ImgGeo_Dll_ CRobustReor  
{
public:
	double m_f;

	iphCamera   *m_camera;

	modelPARA		m_modelPara;


private:
	void PixelPointsToImagePoints( PointPAIR *pixelPoints, int n,  PointPAIR *imgPoints );

public:
	CRobustReor();
	virtual ~CRobustReor();

	void  SetCamera(iphCamera *camera);

	void Compute( PointPAIR *points, int n, double vyTh);

	void GetOrientationParam(modelPARA *param);
};


#endif