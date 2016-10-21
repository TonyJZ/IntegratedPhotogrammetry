#ifndef _LIDAR_Calibration_Base_Define_H_zj_2010_09_21_
#define _LIDAR_Calibration_Base_Define_H_zj_2010_09_21_

//#include "LidBaseDef.h"
#include <vector>
//#include "ATN_def.h"
#include "orsBase\orsTypedef.h"
#include "orsLidarGeometry\LidGeo_def.h"


#ifndef _POINT2D_Def_
#define _POINT2D_Def_
// struct POINT2D	{
// 	double x, y;
// };
typedef orsPOINT2D POINT2D;
#endif

#ifndef _POINT3D_Def_
#define _POINT3D_Def_
// struct POINT3D	{
// 	double X, Y, Z;
// };
typedef orsPOINT3D POINT3D;
#endif

#ifndef _PLANE_Def_
#define _PLANE_Def_
struct	PLANE {
	double X0, Y0, Z0;
	float A, B, C;				// normal line，方向余弦
};
#endif


#ifndef _LINE3D_Def_
#define _LINE3D_Def_
struct	LINE3D	{
	double X0, Y0, Z0;
	float a, b, c;				// 方向矢量
};
#endif


#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif



//tie point type
#ifndef Tiepoint_Type
#define Tiepoint_Type
#define  TP_type_undefine		0
#define  TP_type_vertical		1		//高程点
#define  TP_type_horizon		2		//平面点
#define  TP_type_hor_ver		3		//平高点
#endif

//连接点平差模式
enum TiePoint_Adj_mode
{
	LidAdj_TP_UseHorVer = 0,    //用平面高程
	LidAdj_TP_UseHor,			 //只用平面
	LidAdj_TP_UseVer            //只用高程
};

#define  OBJTYPE_Undefine	0
#define  OBJTYPE_VP			1
#define  OBJTYPE_Plane		2



class LidMC_Obj
{
public:
	virtual int getType()=0;

};

//虚拟连接点
class LidMC_VP : LidMC_Obj
{
public:

	int tieID;					//连接面ID, 从0开始，顺序递增
	double  vx, vy, vz;		//虚拟观测值

	std::vector<LidPt_SurvInfo> *pLidPoints;	//实际激光点数组	
// 	void *pLidPoints; //实际激光点数组
// 	int num_LidPt;	  //点数

	double  eX, eY, eZ;	//期望值 (真值)
	char	VP_type;		//0: undefined;  1: horizontal and vertical control point;   2: horizontal control point;   3: vertical control point
	
	char	reliability;		//0: 不可靠；1: 可靠

	LidMC_VP()
	{
		pLidPoints=NULL;
		reliability=1;
	}

	~LidMC_VP()
	{
		if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
	}

	virtual int getType() { return OBJTYPE_VP; };
};

//连接片
class LidMC_Plane : LidMC_Obj
{
public:

	int tieID;								//连接面ID, 从0开始，顺序递增
	double  s1, s2, s3, s4;					//平面方程参数
	std::vector<LidPt_SurvInfo> *pLidPoints;	//实际激光点数组, 单条带
//	int sID;								//激光点所属条带号

// 	void *pLidPoints;
// 	int num_LidPt;


	LidMC_Plane()
	{
		pLidPoints=NULL;
	}

	~LidMC_Plane()
	{
		if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
	}

	virtual int getType() { return OBJTYPE_Plane; };
};

//平差的未知数模型
typedef int LiDMC_PARAMETER;
static LiDMC_PARAMETER	LiDMC_rph=0;	//r, p, h
static LiDMC_PARAMETER	LiDMC_xyzrph=1;	//tx, ty, tz, r, p, h
static LiDMC_PARAMETER	LiDMC_rph_SemiXYZ=2;		//tx, ty, tz, r, p, h, semiXYZ
static LiDMC_PARAMETER	LiDMC_xyzrph_SemiXYZ=3;			//r, p, h, semiXYZ

//像点 (像平面坐标)
typedef struct ImgPt
{
	int imgID;
	double x, y;
};

//影像匹配点
typedef struct ImgMatchPt
{
	std::vector<ImgPt>	matchPt;		//支持多片
	POINT3D		objPt;						//交会出的物方坐标
};

enum Calib_Method
{
	Lid_Calib_VP_VCP=0,		//对应点的VCP方法
	Lid_Calib_VP_NN,		//对应点的NN方法
};

//条带平差参数类型
#define LidAdj_NoneSysError					0L		//无系统参数
#define LidAdj_boreAngle					1L		//偏心角：3参数
#define LidAdj_leverArm						2L		//偏心分量：3参数
#define LidAdj_sa0							4L		//扫描角sa0: 1参数
#define LidAdj_sac							8L		//扫描角sac: 1参数
#define LidAdj_drange						16L		//测距误差: 1参数
#define LidAdj_appXYZ						32L		//整体偏移: 3参数
#define LidAdj_GPS_drift					64L		//GPS漂移：6参数
#define LidAdj_INS_drift					128L	//INS漂移：6参数




enum CalibParam_Type
{
	Calib_None = -1,
	Calib_rph=1,			//检校三个偏心角
	Calib_rphxyz=2,			//检校偏心角+偏心分量
	Calib_rph_appXYZ=33,	//偏心角+附加参数线性改正
	Calib_rphxyz_sa0=6,		//偏心分量+偏心角+扫描角归零误差
	Calib_rphxyz_sa0_sac=14,	//偏心分量+偏心角+扫描角归零误差+扫描角线性改正
	Calib_rph_sa0=5,			//偏心角+扫描角误差
	Calib_rph_drange=17,		//偏心角+测距误差
	Calib_rph_drange_sa0=21	//偏心角+测距误差+扫描角
};

//原始系统参数（测量获得的）
// struct  oriCalibParam
// {
// 	double lever_arm[3];	//偏心分量
// 	double misalign_angle[3];	//偏心角
// 	double sa0;				//扫描角误差
// 	double sac;				//扫描角线性改正系数
// 	double semiXYZ[3];		//系统平移量
// 	double drange;			//测距误差
// 
// 	POS_Param  posParam;
// };

//LIDAR自检校接口
class CLidarSelfCalibration
{
public:
	CLidarSelfCalibration() { m_pResultName = 0; };
	virtual ~CLidarSelfCalibration() 
	{ 
		if(m_pResultName) 
			free(m_pResultName);
	};

	virtual void setResultName(char *pName) 
	{
		if (pName == 0)
		{
			fprintf(stderr, "ERROR: result_file_name = NULL not supported by CLidarSelfCalibration\n");
		}
		else
		{
			free (m_pResultName);
			m_pResultName = strdup(pName);
		}
	};

	char *m_pResultName;
};

#endif