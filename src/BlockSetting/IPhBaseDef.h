#ifndef _IPLR_BASE_INFORMATION_DEFINITION_H__
#define _IPLR_BASE_INFORMATION_DEFINITION_H__

//#include <vector>
//#include <string>
//using namespace std;

#include "ALMDefine.h"

//像机类型
enum iphCameraType
{
	//0-7与DPS一致
	metricSCAN = 0,	// 量测扫描影像，有框标，需要内定向，包括航空胶卷相机，近景相机
	metricDC = 1,	// 量测数码相机，无框标，有检校数据, 包括UltraCamD, DMC, 近景数码
	nonMETRIC = 2,	// 非量测相机，无检校参数，包括胶卷，普通数码相机
	ADS40 = 3,		// 航空推扫
	SPOT4 = 4,		// 卫星推扫
	SPOT5 = 5,		// 卫星推扫
	IKONOS = 6,		// 卫星推扫
	QUCIKBIRD = 7,	// 卫星推扫
	FISHEYE	= 8		// 鱼眼像机
};

//坐标单位
enum iphUnit
{
	Unit_Meter = 0,		//单位米
	Unit_Degree360 = 1,	//单位度(360)
	Unit_Degree400 = 2,	//单位度(400)
	Unit_Radian = 3		//单位弧度
};

//转角系统
enum iphRotateSys
{
	RotateSys_YXZ = 0,	
	RotateSys_XYZ = 1
};



//像机(内参数+外方位元素)
class iphCamera
{
public:
	bool	m_bInor;			//是否进行内参数初始化
	bool	m_bExor;			//是否进行外方位元素初始化
	iphCameraType	m_Ctype;	//像机类型

	double 	m_x0, m_y0;	//principal point  (mm)
	double 	m_f;		//focal length     (mm)
	
	// lens distortion, for film and DC
	double m_k1, m_k2, m_k3;
	double m_p1, m_p2;

	//elements of exterior orientation
	double m_Xs, m_Ys, m_Zs;
	double m_phi, m_omega, m_kappa;

	char	m_HorizontalDatum[_MAX_PATH];
	iphUnit		m_HorizontalUnit;
	char	m_VerticalDatum[_MAX_PATH];
	iphUnit		m_VerticalUnit;

	iphRotateSys	m_RotateSys;
	iphUnit		m_AngleUnit;
	
	double m_R[9];		//rotate matrix
	double	m_GPSTime;

public:
	iphCamera()
	{
		m_bInor=false;
		m_bExor=false;
		m_Ctype = metricDC;

		m_x0=0;	m_y0=0;
		m_f=0;
		m_k1=m_k2=m_k3=0;
		m_p1=m_p2=0;
		
		m_Xs=m_Ys=m_Zs=0;
		m_phi=m_omega=m_kappa=0;
		m_R[0]=m_R[1]=m_R[2]=0;
		m_R[3]=m_R[4]=m_R[5]=0;
		m_R[6]=m_R[7]=m_R[8]=0;
		m_GPSTime=0;
	};

	~iphCamera()
	{
		
	};
	
	//设置内定向参数
	void SetInorParameter(double x0, double y0, double f, double k1, double k2, double k3, double p1, double p2);
	//设置外方位元素
	void SetExorParameter(double xs, double ys, double zs, double phi, double omega, double kappa);
	//设置坐标系统
	void SetCoordinateSystem(char *HorizontalDatum, char *VerticalDatum, iphUnit HorizontalUnit, iphUnit VerticalUnit);
	//设置转角系统
	void SetRotateSystem(iphRotateSys RotateSys, iphUnit AngleUnit);

};

//typedef std::vector<std::string> iphNameList;

typedef struct
{
	int nImgID;                        //影像索引(总) (从1开始) 
	CString strImgName;                //影像名
	int     nStripID;                  //线号(航带号,从1开始)
	int 	nPhoID;                    //片号(影像在航带内的ID,从1开始)  
	int    nIsReversal;                //是否旋转180度

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


#endif 
