//平面约束的lidar条带平差，同时解算lidar系统误差和平面参数
#ifndef _Plane_Constraint_Model_h_zj_2010_10_23_
#define _Plane_Constraint_Model_h_zj_2010_10_23_


#include "CalibBasedef.h"
#include <vector>

#ifdef _LIDCALIB_
#define  _lidCalib_Dll_  __declspec(dllexport)
#else
#define  _lidCalib_Dll_  __declspec(dllimport)	
#endif

// #ifndef _LIDCALIB_
// #ifdef _DEBUG
// #pragma comment(lib,"LidCalib.lib")
// #else
// #pragma comment(lib,"LidCalib.lib")
// #endif
// #endif


//平面参数  ax+by+cz+d=0
struct PlaneParam
{
	double a, b, c, d;
};



//ransac拟合平面参数,并挑出平面内的点，默认按1.5倍rms挑粗差
int PlaneFitting_ransac(LidMC_Plane *pLidPatch);

//按照指定参数计算连接平面的平面参数
int _lidCalib_Dll_ lidPlaneFitting(LidMC_Plane *pLidPlane, int nPlane, oriCalibParam oriParam, double *Lidparam, CalibParam_Type type);

//利用ransac拟合平面，计算法向量
//int _lidCalib_Dll_ lidPlaneFitting(LidMC_Plane *pLidPlane, int nPlane);


//2013.1.4  自动根据检校参数类型计算点面距离
void distance_PointToPlane_new(double *p, double *y, int m, int n, void *data);

void _lidCalib_Dll_ LidPlane_ransac(LidMC_Plane *pLidPlane, int nPlane);

//平面点, 平面数，当前LiDAR系统参数，初始误差参数，系统误差类型，点面距离rms，同名特征数
void _lidCalib_Dll_ LidStatDis_PointToPlane(LidMC_Plane *pLidPatch, int nPatch, double *Lidparam, bool bInit, oriCalibParam oriParam,
											CalibParam_Type param_type,double *rms, int nTies, double *pDis=NULL);

class _lidCalib_Dll_ CPlaneCalib : public CLidarSelfCalibration
{
public:
     CPlaneCalib();
 	 virtual ~CPlaneCalib();
// 	 void setResultName(char *pName);


	 //解算偏心角和偏心分量, 平面约束
	 int LidMC_Misalign_plane(
		 const int nPlane,	/* number of planes */
		 LidMC_Plane *pLidPlane,
		 double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
								* aj are the image j parameters, bi are the i-th point parameters,
								* size m*cnp + n*pnp
								未知数，偏心分量和安置角    
								pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
								const int pnp			/* number of parameters 未知数个数*/
								//double *pObs,			/*观测值(虚拟观测值，加权平均值)*/
								//double errTh2			//(迭代终止条件,物方坐标改正);
								);

	 // 平面约束
	 //2013.1.5  按指定的系统误差类型进行检校，确定未知数个数
	 int LidMC_Misalign_plane(
		 const char *pszResultFile,		/* 平差结果文件 */
		 const int nPlane,	/* number of planes */
		 LidMC_Plane *pLidPlane,
		 double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
								* aj are the image j parameters, bi are the i-th point parameters,
								* size m*cnp + n*pnp
								未知数，安置角    
								pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
		oriCalibParam oriParam,	//系统误差初值
		const CalibParam_Type type	//误差类型	
								);

	 //z值约束
	 int LidMC_Misalign_plane_zconstraint(
		 const int nVP,
		 LidMC_VP *pLidVP,
		 const int nPlane,	/* number of planes */
		 LidMC_Plane *pLidPlane,
		 double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
								* aj are the image j parameters, bi are the i-th point parameters,
								* size m*cnp + n*pnp
								未知数，偏心分量和安置角    
								pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
								const int pnp			/* number of parameters 未知数个数*/
								//double *pObs,			/*观测值(虚拟观测值，加权平均值)*/
								//double errTh2			//(迭代终止条件,物方坐标改正);
								);


	 //平面平差精度检查
	 int LidMC_PlaneAdj_QA(
		 const int nPlane,	/* number of planes */
		 LidMC_Plane *pLidPlane,
		 double *pInitX,			/* 未知数初值*/
		 double *pAdjX,			/* 未知数平差值*/
		 const int pnp,			/* number of parameters 未知数个数*/
		 LiDMC_PARAMETER paramtype	/* 参数类型*/
		 //double *pObs,			/*观测值(虚拟观测值，加权平均值)*/
		 //double errTh2			//(迭代终止条件,物方坐标改正);
		 );

public:
	double	*m_covar;	//未知数的协方差阵
	int		m_pn;		//未知数个数
	double  m_rms;		//单位权中误差（观测值）
	double  *m_corcoef;	//未知数之间的相关系数
};


#endif