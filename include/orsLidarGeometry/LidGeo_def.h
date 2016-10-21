#ifndef _LIDAR_GEOMETRY_DATA_TYPE_DEFINE_H__
#define _LIDAR_GEOMETRY_DATA_TYPE_DEFINE_H__

#include "orsBase\orsTypedef.h"

//导航数据
struct orsPOSParam
{
	int stripID;		//航带号,从0开始		
	double time;		//time stamp
	orsPOINT3D coord;	//lat, lon, h 原始数据默认为WGS84下,经纬度、椭球高
	double r, p, h;		//radian 原始POS数据默认为导航坐标系下的转角
	double R[9];

	float padding;     //对齐用填补数位
};

//原始激光脚点几何信息
struct LidGeo_RawPoint
{
	// 	double Xs, Ys, Zs;
	// 	double R[9];
	orsPOSParam eoParam; //POS参数
	double range;
	double sAngle;
	double X, Y, Z;	//脚点的三维坐标
};

//带时标的点记录
struct LidPt_XYZT
{
	double X, Y, Z;
	double t;
};

#ifndef _LidPt_SurvInfo_Def_
#define _LidPt_SurvInfo_Def_
typedef struct LidPt_SurvInfo  //激光点定位定向信息
{
	int stripID;    //激光点所属条带号，条带相关的算法需要
	double x;		//(WGS84/radians)
	double y;		//(WGS84/radians)
	double z;
	double time;	//time stamp
	orsPOSParam POS_Info;	//pos信息
	double vecRange[3]; //扫描本体坐标系下的斜距，扫描角与测距的乘积

} LidPt_SurvInfo;
#endif


//系统附加参数类型
enum orsLidSysParam_TYPE
{
	LidGeo_rph=0,			//三个视准轴偏角
	LidGeo_rphxyz,			//视准轴偏角+偏心分量
	LidGeo_rph_appXYZ,		//偏角+附加参数线性改正
	LidGeo_rphxyz_sa0,		//偏角+偏心分量+扫描角归零误差
	LidGeo_rphxyz_sa0_sac,	//偏角+偏心分量+扫描角归零误差+扫描角线性改正
	LidGeo_rph_sa0,			//偏角+扫描角误差
	LidGeo_rph_drange,		//偏角+测距误差
	LidGeo_rph_drange_sa0	//偏角+测距误差+扫描角归零误差
};

//POS系统误差  一次漂移模型
struct  POS_Param
{
	//	oriCalibParam calib_param;  //Lidar系统误差
	double *gps_drift;		//GPS误差  6N
	double *ins_drift;		//INS误差  6N

	int		stripNum;		//条带数
	double  time0;			//参考时刻
};

//严格模型
//原始系统参数（测量获得的）
struct  orsLidSysParam
{
	double lever_arm[3];	//偏心分量
	double boresight_angle[3];	//偏角
	double sa0;				//扫描角误差
	double sac;				//扫描角线性改正系数
	double drange;			//测距误差

	double semiXYZ[3];		//系统平移量, 有控制点时用

	POS_Param  posParam;
};

//Ressl model parameters
struct orsLidResslParam 
{
//	double		m_rotZ;				//条带绕Z轴的旋转角
//  每条带独立参数
	double		rotMatZ[9];		//条带绕Z轴的旋转矩阵
	orsPOINT3D  stripCenter;		//条带中心坐标（参考点）

	//unknowns，系统未知数，与条带无关
	double		ax, ay, az;
	double		aroll;
	double		ayaw;
};

//Yuan model parameters
struct orsLidYuanParam 
{
	//	double		m_rotZ;				//条带绕Z轴的旋转角
	//  每条带独立参数
	double		rotMatZ[9];		//条带绕Z轴的旋转矩阵
	orsPOINT3D  stripCenter;		//条带中心坐标（参考点）

	//unknowns，系统未知数，与条带无关
	double		ax, ay, az;
	double		kappa;
	double		omega;
};

//Jing model parameters
struct orsLidJingParam 
{
	//	double		m_rotZ;				//条带绕Z轴的旋转角
	//  每条带独立参数
	double		rotMatZ[9];		//条带绕Z轴的旋转矩阵
	orsPOINT3D  stripCenter;		//条带中心坐标（参考点）  Z为平均行高

	//unknowns，系统未知数，与条带无关
	double		phi, omega, kappa;
	
};

interface orsITrajectory;

struct orsLidTrajAidParam
{
	orsITrajectory *pTraj;		//trajectory

	double lever_arm[3];	//偏心分量
	double boresight_angle[3];	//偏角

	double drift[6];		//漂移参数
};


#endif