#ifndef _IPLR_BASE_INFORMATION_DEFINITION_H__
#define _IPLR_BASE_INFORMATION_DEFINITION_H__

//#include <vector>
//#include <string>
//using namespace std;

//#include "ALMDefine.h"
#include "AFXTEMPL.H"

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


//像机(内参数+外方位元素)
class iphCamera
{
public:
	bool	m_bInor;			//是否进行内参数初始化
	bool	m_bExor;			//是否进行外方位元素初始化
	iphCameraType	m_Ctype;	//像机类型

	double 	m_x0, m_y0;	//principal point  (mm)
	double 	m_f;		//focal length     (mm)
	double	m_pixelWid;	//pixel width      (mm)
	double	m_pixelHei;	//pixel height	   (mm)

	int		m_imgWid;		//image width	   (pixel)		从影像中读出来
	int		m_imgHei;		//image height     (pixel)

	// lens distortion, for film and DC
	double m_k1, m_k2, m_k3;
	double m_p1, m_p2;
	double m_b1, m_b2;

	//elements of exterior orientation
	double m_Xs, m_Ys, m_Zs;
	double m_phi, m_omega, m_kappa;

	char	m_HorizontalDatum[_MAX_PATH];
	iphUnit		m_HorizontalUnit;
	char	m_VerticalDatum[_MAX_PATH];
	iphUnit		m_VerticalUnit;

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
	iphCamera()
	{
		m_bInor=false;
		m_bExor=false;
		m_Ctype = iph_metricDC;
		m_RotateSys = RotateSys_YXZ;

		m_x0=0;	m_y0=0;
		m_f=0;
		m_pixelWid=m_pixelHei=0;

		m_k1=m_k2=m_k3=0;
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

	~iphCamera()
	{
		
	};

	//用相机参数初始化内参数矩阵
	void  InteriorOrientation(); 

	//像素转框标
	//采用的计算公式：
	//	x_corr = x_meas - x0 - det(x)
	//	y_corr = y_meas - y0 - det(y)
	//有些系统在检校时采用的是 + det(x)的方式，在做直接定向时要注意
	inline void  Image2Photo(double xi, double yi, double& xp, double& yp)
	{
		double xf, yf;
		double r2, dr, dx, dy;

		xf = m_inorImage_a[0] + m_inorImage_a[1]*xi + m_inorImage_a[2]*yi;
		yf = m_inorImage_b[0] + m_inorImage_b[1]*xi + m_inorImage_b[2]*yi;

		//畸变纠正
		xp = xf - m_x0;
		yp = yf - m_y0;

		r2 = xp*xp + yp*yp;

		dr = ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2;
		dx = xp*dr;
		dy = yp*dr;

		dx += m_p1*( r2 + 2*xp*xp ) + 2*m_p2*xp*yp;
		dy += 2*m_p1*xp*yp + m_p2*( r2 + 2*yp*yp );

		dx=m_b1 * xp + m_b2 * yp;

		xp -= dx;	
		yp -= dy;
	};

	//框标转像素
	inline void  Photo2Image(double xp, double yp, double& xi, double& yi)
	{
		double xf, yf;
		double r2, dr, dx, dy;

		r2 = xp*xp + yp*yp;

		dr = ( m_k1+ ( m_k2 + m_k3*r2)*r2 )*r2;
		dx = xp*dr;
		dy = yp*dr;

		dx += m_p1*( r2 + 2*xp*xp ) + 2*m_p2*xp*yp;
		dy += 2*m_p1*xp*yp + m_p2*( r2 + 2*yp*yp );

		dx=m_b1 * xp + m_b2 * yp;

		xf=xp+dx+m_x0;
		yf=yp+dy+m_y0;

		xi = m_inorPixel_a[0] + m_inorPixel_a[1]*xf + m_inorPixel_a[2]*yf;
		yi = m_inorPixel_b[0] + m_inorPixel_b[1]*xf + m_inorPixel_b[2]*yf;

	};
	
	//设置内定向参数
	void SetInorParameter(double x0, double y0, double f, double k1, double k2, double k3, double p1, double p2);
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

//由旋转角计算旋转矩阵，弧度
void _CalRotateMatrix(iphRotateSys Rsys, double A1, double A2, double A3, double *R);
//由旋转矩阵计算旋转角，弧度
void _CalRotateAngle(iphRotateSys Rsys, double *R, double *A1, double *A2, double *A3);

//typedef std::vector<std::string> iphNameList;

typedef struct
{
	int nImgID;                        //影像索引(总) (从1开始) 
	CString strImgName;                //影像名
	int     nStripID;                  //线号(航带号,从1开始)
	int 	nPhoID;                    //片号(影像在航带内的ID,从1开始)  
	int    nIsReversal;                //是否旋转180度
//	bool   useful;					   //自动定向标志，true/false ：能与其他影像进行定向/不能定向
	
	iphCamera		camera;

} iphImgINFO;

typedef CArray<iphImgINFO, iphImgINFO&>	iphImgList;


//测区信息
// class iphBlockInfo
// {
// public:
// 	char name[_MAX_PATH];			//工程名
// 	iphImgList	imgList;			//影像列表
// 	iphNameList	lidarNameList;		//las名列表
// 
// public:
// 	iphBlockInfo()
// 	{
// 		name[0]=0;
// 	};
// 
// 	~iphBlockInfo()
// 	{
// 		imgList.clear();
// 		lidarNameList.clear();
// 	};
// 
// 	void reinitialize()
// 	{
// 		name[0]=0;
// 		imgList.clear();
// 		lidarNameList.clear();
// 	};
// 
// 	iphBlockInfo& operator=(const iphBlockInfo& __x)
// 	{
// 		strncpy(name, __x.name, sizeof(char)*_MAX_PATH);
// 		imgList=__x.imgList;
// 		lidarNameList=__x.lidarNameList;
// 
// 		return	*this;
// 	};
// };

#ifndef _KEYPOINT_def
#define	_KEYPOINT_def
struct KEYPOINT
{
	float x, y;				/* Subpixel location of keypoint. 像素坐标*/
	float scale, ori;		/* Scale and orientation (range [-PI,PI]) */
//	unsigned char m_r, m_g, m_b;  /* Image Color of this key */
	
//	unsigned char *m_descriptors;
};
#endif

#ifndef _KEYPOINTWithDesc_def
#define	_KEYPOINTWithDesc_def
struct KEYPOINTWithDesc
{
	float x, y;				/* Subpixel location of keypoint. 像素坐标*/
	float scale, ori;		/* Scale and orientation (range [-PI,PI]) */
	
	unsigned char dsnum;	//描述子长度
	unsigned char *pDesc;

};
#endif

#ifndef _KEYPOINTMATCH_def
#define _KEYPOINTMATCH_def
struct KEYPOINTMATCH
{
	int m_idx1, m_idx2;	//点索引号

	KEYPOINT m_kp1, m_kp2;
	double m_disRatio;
};
#endif

#ifndef _AT_IMGPOINT_
#define _AT_IMGPOINT_
struct AT_ImgPoint
{
	int  imgID;
	double x, y;			
	int relflag;
};
#endif


#ifndef _AT_TIEPOINT_
#define _AT_TIEPOINT_
struct AT_TiePoint
{
	double X, Y, Z;
	int  nImgPt;
	AT_ImgPoint *pImgPt;

	int  marker;

	AT_TiePoint::AT_TiePoint()
	{
		pImgPt=0;
	}

	AT_TiePoint::~AT_TiePoint()
	{
		if(pImgPt)	delete[] pImgPt;
	}
};
#endif

//ALS50系统提供的POS数据
struct POSData_ALS50 {
	char ImageName[128];
	int ImageID;
	double GPSTime;
	double Xs,Ys,Zs;
	double omega, phi, kappa;		//radian
	double Lon, Lat;		//radian
	double hei;			//大地高
};



#endif 
