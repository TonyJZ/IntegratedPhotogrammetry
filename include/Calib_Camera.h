#ifndef _IPH_CAMERA_DEF_ZJ_20110813_H_
#define _IPH_CAMERA_DEF_ZJ_20110813_H_

#ifdef _CALIB_UTILITY_
#define  _CalibUtility_Dll_  __declspec(dllexport)
#else
#define  _CalibUtility_Dll_  __declspec(dllimport)	
#endif

#ifndef _CALIB_UTILITY_
#ifdef _DEBUG
#pragma comment(lib,"CalibUtilityD.lib")
#else
#pragma comment(lib,"CalibUtility.lib")
#endif
#endif

#if !defined(_MAX_PATH)
#if defined (MAX_PATH)
#define _MAX_PATH MAX_PATH
#else
#define _MAX_PATH 512
#endif
#endif

#ifndef _iphCameraType
#define _iphCameraType
//像机类型
enum iphCameraType
{
	//0-7与DPS一致
	iph_metricSCAN = 0,	// 量测扫描影像，有框标，需要内定向，包括航空胶卷相机，近景相机
	iph_metricDC = 1,	// 量测数码相机，无框标，有检校数据, 包括UltraCamD, DMC, 近景数码
	iph_nonMETRIC = 2,	// 非量测相机，无检校参数，包括胶卷，普通数码相机
	iph_ADS40 = 3,		// 航空推扫
	iph_SPOT4 = 4,		// 卫星推扫
	iph_SPOT5 = 5,		// 卫星推扫
	iph_IKONOS = 6,		// 卫星推扫
	iph_QUCIKBIRD = 7,	// 卫星推扫
	iph_FISHEYE	= 8		// 鱼眼像机
};
#endif

#ifndef _iphUnit
#define _iphUnit
//坐标单位
enum iphUnit
{
	Unit_Meter = 0,		//单位米
	Unit_Degree360 = 1,	//单位度(360)
	Unit_Degree400 = 2,	//单位度(400)
	Unit_Radian = 3		//单位弧度
};
#endif

#ifndef _iphRotateSys
#define _iphRotateSys
//转角系统
enum iphRotateSys
{
	RotateSys_YXZ = 0,	//phi, omega, kappa
	RotateSys_XYZ = 1,	//omega, phi, kappa
	RotateSys_ZYZ =2	//A, alpha, kappa
};
#endif

struct lensDistortion
{
	double k0, k1, k2, k3;
	double p1, p2;
	double b1, b2;
};

//像机(内参数+外方位元素)
class _CalibUtility_Dll_ _iphCamera
{
public:
	bool	m_bInor;			//是否进行内参数初始化
	bool	m_bExor;			//是否进行外方位元素初始化
	iphCameraType	m_Ctype;	//像机类型

	double 	m_x0, m_y0;	//principal point  (mm)
	double 	m_f;		//focal length     (mm)
	double	m_pixelX;	//pixel width      (mm)
	double	m_pixelY;	//pixel height	   (mm)

	int		m_imgWid;		//image width	   (pixel)		从影像中读出来
	int		m_imgHei;		//image height     (pixel)

	// lens distortion, for film and DC
	double m_k0, m_k1, m_k2, m_k3;
	double m_p1, m_p2;
	double m_b1, m_b2;

	//elements of exterior orientation
	double m_Xs, m_Ys, m_Zs;
	double m_phi, m_omega, m_kappa;		//弧度

// 	char	m_HorizontalDatum[_MAX_PATH];
// 	iphUnit		m_HorizontalUnit;
// 	char	m_VerticalDatum[_MAX_PATH];
// 	iphUnit		m_VerticalUnit;

	iphRotateSys	m_RotateSys;	//转角类型
	iphUnit		m_AngleUnit;		//转角单位

	double m_RMatrix[9];		//rotate matrix
	double	m_GPSTime;
	//	char	m_pCameraFileName[256];		//像机文件名

	// interior orientation
	double m_inorImage_a[3];	// pixel to photo
	double m_inorImage_b[3];

	double m_inorPixel_a[3];	// photo to pixel
	double m_inorPixel_b[3];


public:
	_iphCamera()
	{
		m_bInor=false;
		m_bExor=false;
		m_Ctype = iph_metricDC;
		m_RotateSys = RotateSys_YXZ;

		m_x0=0;	m_y0=0;
		m_f=0;
		m_pixelX=m_pixelY=0;

		m_k0=m_k1=m_k2=m_k3=0;
		m_p1=m_p2=0;
		m_b1=m_b2=0;

		m_imgHei=m_imgWid=0;

		m_Xs=m_Ys=m_Zs=0;
		m_phi=m_omega=m_kappa=0;
		m_RMatrix[0]=1;	m_RMatrix[1]=0;	m_RMatrix[2]=0;
		m_RMatrix[3]=0;	m_RMatrix[4]=1;	m_RMatrix[5]=0;
		m_RMatrix[6]=0;	m_RMatrix[7]=0;	m_RMatrix[8]=1;
		m_GPSTime=0;

		m_inorImage_a[0]=m_inorImage_a[1]=m_inorImage_a[2]=0;
		m_inorImage_b[0]=m_inorImage_b[1]=m_inorImage_b[2]=0;
		m_inorPixel_a[0]=m_inorPixel_a[1]=m_inorPixel_a[2]=0;
		m_inorPixel_b[0]=m_inorPixel_b[1]=m_inorPixel_b[2]=0;
		//		m_pCameraFileName[0]='\0';
	};

// 	_iphCamera(_iphCamera cam)
// 	{
// 		m_bInor=false;
// 		m_bExor=false;
// 		m_Ctype = cam.m_Ctype;
// 		m_RotateSys = cam.m_RotateSys;
// 
// 		m_x0=cam.m_x0;	m_y0=cam.m_y0;
// 		m_f=cam.m_f;
// 		m_pixelX=cam.m_pixelX;	m_pixelY=cam.m_pixelY;
// 
// 		m_k0=cam.m_k0;	m_k1=cam.m_k1;	m_k2=cam.m_k2;	m_k3=cam.m_k3;
// 		m_p1=cam.m_p1;	m_p2=cam.m_p2;
// 		m_b1=cam.m_b1;	m_b2=cam.m_b2;
// 
// 		m_imgHei=cam.m_imgHei;	m_imgWid=cam.m_imgWid;
// 
// 		m_Xs=cam.m_Xs;	m_Ys=cam.m_Ys;	m_Zs=cam.m_Zs;
// 
// 		m_phi=cam.m_phi;	m_omega=cam.m_omega;	m_kappa=cam.m_kappa;
// 
// 		memcpy( m_RMatrix,cam.m_RMatrix, sizeof(double)*9 );
// // 		m_RMatrix[0]=1;	m_RMatrix[1]=0;	m_RMatrix[2]=0;
// // 		m_RMatrix[3]=0;	m_RMatrix[4]=1;	m_RMatrix[5]=0;
// // 		m_RMatrix[6]=0;	m_RMatrix[7]=0;	m_RMatrix[8]=1;
// 		m_GPSTime=cam.m_GPSTime;
// 
// 		memcpy(m_inorImage_a, cam.m_inorImage_a, sizeof(double)*3);
// 		memcpy(m_inorImage_b, cam.m_inorImage_b, sizeof(double)*3);
// 		memcpy(m_inorPixel_a, cam.m_inorPixel_a, sizeof(double)*3);
// 		memcpy(m_inorPixel_b, cam.m_inorPixel_b, sizeof(double)*3);
// // 		m_inorImage_a[0]=m_inorImage_a[1]=m_inorImage_a[2]=0;
// // 		m_inorImage_b[0]=m_inorImage_b[1]=m_inorImage_b[2]=0;
// // 		m_inorPixel_a[0]=m_inorPixel_a[1]=m_inorPixel_a[2]=0;
// // 		m_inorPixel_b[0]=m_inorPixel_b[1]=m_inorPixel_b[2]=0;
// 		//		m_pCameraFileName[0]='\0';
// 	};

	~_iphCamera()
	{

	};

	//用相机参数初始化内参数矩阵
	void  InteriorOrientation(); 

	//像素转框标
	//采用的计算公式：
	//	x_corr = x_meas - x0 + det(x)
	//	y_corr = y_meas - y0 + det(y)
	//有些系统在检校时采用的是 - det(x)的方式，在做直接定向时要注意
	void  Image2Photo(double xi, double yi, double& xp, double& yp)
	{
		double xf, yf;
		double r2, dr, dx, dy;

		xf = m_inorImage_a[0] + m_inorImage_a[1]*xi + m_inorImage_a[2]*yi;
		yf = m_inorImage_b[0] + m_inorImage_b[1]*xi + m_inorImage_b[2]*yi;

		double x, y;
		FiducialMarkCoordToCalibrated(xf, yf, &x, &y);
		xp = x;		yp = y;

		//畸变纠正
// 		xp = xf - m_x0;
// 		yp = yf - m_y0;
// 
// 		r2 = xp*xp + yp*yp;
// 
// 		dr =m_k0 + ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2;
// 		dx = xp*dr;
// 		dy = yp*dr;
// 
// 		dx += m_p1*( r2 + 2*xp*xp ) + 2*m_p2*xp*yp;
// 		dy += 2*m_p1*xp*yp + m_p2*( r2 + 2*yp*yp );
// 
// 		dx=m_b1 * xp + m_b2 * yp;
// 
// 		xp += dx;	
// 		yp += dy;
// 		xp -= dx;	
// 		yp -= dy;
	};

	//框标转像素		有bug  用来做畸变纠正时不对
	void  Photo2Image(double xp, double yp, double& xi, double& yi)
	{
		double xf, yf;
// 		double r2, dr, dx, dy;
// 
// 		r2 = xp*xp + yp*yp;
// 
// 		dr = ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2;
// 		dx = xp*dr;
// 		dy = yp*dr;
// 
// 		dx += m_p1*( r2 + 2*xp*xp ) + 2*m_p2*xp*yp;
// 		dy += 2*m_p1*xp*yp + m_p2*( r2 + 2*yp*yp );
// 
// 		dx=m_b1 * xp + m_b2 * yp;
// 
// 		xf = xp - dx + m_x0;
// 		yf = yp - dy + m_y0;
// 		xf = xp + dx + m_x0;
// 		yf = yp + dy + m_y0;

		CalibratedToFiducialMarkCoord(xp, yp, &xf, &yf);

		xi = m_inorPixel_a[0] + m_inorPixel_a[1]*xf + m_inorPixel_a[2]*yf;
		yi = m_inorPixel_b[0] + m_inorPixel_b[1]*xf + m_inorPixel_b[2]*yf;

	};

	//消除畸变和主点偏移后的像素坐标
	void ImageCoordinateCorrect(double xi, double yi, double &xi_cor, double &yi_cor);

	// 框标坐标到检校后影像坐标
	void FiducialMarkCoordToCalibrated( double xf, double yf, double *xc, double *yc );
	
	void CalibratedToFiducialMarkCoord( double xc, double yc, double *xf, double *yf );

	//设置内定向参数
	void SetInorParameter(double x0, double y0, double f, int imgWid, int imgHei, double pixelX, double pixelY,
		double k0=0, double k1=0, double k2=0, double k3=0, double p1=0, double p3=0);
	//设置外方位元素
	void SetExorParameter(double xs, double ys, double zs, double phi, double omega, double kappa);

	//设置坐标系统
	void SetCoordinateSystem(char *HorizontalDatum, char *VerticalDatum, iphUnit HorizontalUnit, iphUnit VerticalUnit);
	//设置转角系统
	void SetRotateSystem(iphRotateSys RotateSys, iphUnit AngleUnit);

	bool WriteCameraFile(const char *pCamName);
	bool ReadCameraFile(const char *pCamName);

	void CalRotateMatrix();	//计算旋转矩阵
	void GetRotateMatrix(double *R);

	bool WriteExtOrFile_aop( const char *aopFileName );	//将外方位元素导出aop文件
	bool WriteExtOrFile_aop_ALS50( const char *aopFileName );
	bool WriteIntOrFile_iop( const char *iopFileName );	//将内定向参数导出iop文件
};



#endif