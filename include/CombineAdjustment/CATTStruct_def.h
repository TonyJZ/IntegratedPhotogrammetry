#ifndef _COMBINED_ATT_STRUCTURE_DEFINE_H__
#define _COMBINED_ATT_STRUCTURE_DEFINE_H__

#include "orsLidarGeometry\orsITrajectory.h"
#include <vector>
using namespace std;

//相机安装时的旋转模式
enum ATT_CameroRoateMODE	{
	ATT_rmNONE = 0,				// 不做旋转			
	ATT_rmANTICLOCKWISE90,	// 逆顺时针旋转90度
	ATT_rmCLOCKWISE90,		// 顺时针旋转90度
	ATT_rm180				// 180度
};


//激光脚点连接点
struct ATT_LaserPoint
{
	int lidID;  //从1开始
	double t;
//	double gpstime;
	double X, Y, Z;
//	orsPOSParam pos;
	double range;
	double sAngle;
};

//像点
struct ATT_ImagePoint
{
	int imgID;		//条带号+相片号，从0001开始
	double xi, yi;	//像素坐标
	double xp, yp;
};
//相片
struct ATT_ImageEXOR
{
	int survID;				//测区ID 注意：从0开始顺序编号
	char imgName[64];
	int imgID;				//相片号：条带号+影像号
	double t0;				//测区内的时间基准，用于漂移值的平差

	orsPOSParam eoParam;    //相片外方位元素
	std::vector<orsPOSParam> orientedTraj;	//定向点数组

	ATT_ImageEXOR()
	{

	}

	~ATT_ImageEXOR()
	{
		orientedTraj.clear();
	}
};

//条带
struct ATT_StripInfo
{
	int stripID;							//与Align_LidLine中的条带编号保持一致，从1开始
	std::string atnName;
	std::vector<ATT_ImageEXOR> imgEOVec;	//按GPS时间排序

	ATT_StripInfo()
	{
		atnName=_T("");
	}

	~ATT_StripInfo()
	{
		atnName.clear();
		imgEOVec.clear();
	}
};

#define		TiePtTYPE_TIE		 0L		//连接点
#define		TiePtTYPE_GCP_XY	 1L		//平面控制点
#define		TiePtTYPE_GCP_Z		 2L		//高程控制点
#define		TiePtTYPE_GCP_XYZ	 3L		//平高控制点

// enum ATT_TiePtTYPE
// {
// 	TiePtTYPE_TIE = 0L,		//连接点
// 	TiePtTYPE_GCP_XY = 1L,		//平面控制点
// 	TiePtTYPE_GCP_Z = 2L,		//高程控制点
// 	TiePtTYPE_GCP_XYZ = 3L,		//平高控制点
// };

struct ATT_tiePoint
{
	ors_int64 tieId;
	double X, Y, Z;	
	int tType;
	std::vector<ATT_LaserPoint> laspts;
	std::vector<ATT_ImagePoint> projs;

	ATT_tiePoint()
	{
		tType = TiePtTYPE_TIE;
	}
	~ATT_tiePoint()
	{
		laspts.clear();
		projs.clear();
	}
};

struct ATT_GCP
{
	ors_int64 tieId;
	double X, Y, Z;	
	int tType;
};

typedef struct tagLidImage_ATT_DATA
{
	ref_ptr<orsITrajectory>	pTraj;



}LidImage_ATT_DATA;


//赋值操作
#define MAX_DOUBLE(x)		( x=1.7E+308 )
#define MIN_DOUBLE(x)		( x=-1.7E+308 )	
#define MAX_FLOAT(x)		( x=3.3e33f )
#define MIN_FLOAT(x)		( x=-3.3e33f )
#define MAX_LONG(x)			( x=2147483647 )	
#define MIN_LONG(x)			( x=-2147483648 )
#define MAX_USHORT(x)		( x=65535 )
#define MIN_USHORT(x)		( x=0 )

#endif