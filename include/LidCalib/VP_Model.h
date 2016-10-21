#ifndef _Virtual_Point_Model_H_zj_2010_09_17_
#define _Virtual_Point_Model_H_zj_2010_09_17_

//////////////////////////////////////////////////////////////////////////
//虚拟连接点模型

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

using namespace std;


// #ifdef __cplusplus
// extern "C" {
// #endif

/*
	函数命名规则
	VP:  Virtual Point 虚拟点，无地面控制
	VTP: Virtual True Point  有地面控制点
	NN: 最近邻模型
	Tri: 三角形模型
*/

enum VP_mode 
{ 
	VP_TRIANGLE,  //三角形插值
	VP_IDW			//距离反比插值
};


void cal_Obs_VCP(double *p, double *y, int m, int n, void *data);

//按照指定参数计算VP误差
void _lidCalib_Dll_ LidStatDis_VP(LidMC_VP *pLidVP, int nVP, int ngcp, double *pX, orsLidSysParam oriParam, 
								  CalibParam_Type  param_type, double *pdx, double *pdy, double *pdz, double *rms);

double _lidCalib_Dll_ TriangleInterpolate( std::vector<LidPt_SurvInfo> *pLidPoints, double x, double y );

double _lidCalib_Dll_ TriangleInterpolate_time( std::vector<LidPt_SurvInfo> *pLidPoints, double x, double y );

class _lidCalib_Dll_ CLidCalib_VCP : public CLidarSelfCalibration
{
public:
	CLidCalib_VCP();
	virtual ~CLidCalib_VCP();

	//解算偏心角和偏心分量, 基于虚拟点(微小面片)对应方法
	//控制点需要排在链表的最开始处
// int  LidMC_Misalign_VP_Triangle_xyzrph_semiXYZ(
// 	const int nVP,	        /* number of Virtual points */
// 	const int ngcp,		/* number of points (starting from the 1st) whose parameters should not be modified.	
// 						* All B_ij (see below) with i<ncon are assumed to be zero
// 						控制点放在观测值的起始部分*/
// 	LidMC_VP *pLidPatch,
// 
// 						//				 char *vmask,			/* visibility mask: vmask[i, j]=1 if point i visible in image j, 0 otherwise. nxm */
// 	double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
// 											* aj are the image j parameters, bi are the i-th point parameters,
// 											* size m*cnp + n*pnp
// 											未知数，偏心分量和安置角    
// 											pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
// 	oriCalibParam oriParam,
// 	CalibParam_Type  param_type	
// 	);

	//只做相对平差，不进行GPS系统纠正
	//2013.11.22 对观测值按X,Y,Z三个轴向进行加权
	//虚拟连接点检校方法
	int  LidMC_Misalign_VCP(
		const char *pszResultFile,		/* 平差结果文件 */
		const int nVP,	        /* number of Virtual points */
		const int ngcp,		/* number of points (starting from the 1st) whose parameters should not be modified.	
							* All B_ij (see below) with i<ncon are assumed to be zero
							控制点放在观测值的起始部分*/
		LidMC_VP *pLidPatch,
		TiePoint_Adj_mode TiePt_mode,
							//				 char *vmask,			/* visibility mask: vmask[i, j]=1 if point i visible in image j, 0 otherwise. nxm */
		double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
												* aj are the image j parameters, bi are the i-th point parameters,
												* size m*cnp + n*pnp
												未知数，偏心分量和安置角    
												pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
		//				const int pnp,			/* number of parameters 未知数个数*/
		orsLidSysParam oriParam,
		CalibParam_Type	param_type				/* 检校参数类型 */
		);

	//最近邻检校方法
	int  LidMC_Misalign_NN(
		const char *pszResultFile,		/* 平差结果文件 */
		const int nVP,	        /* number of Virtual points */
		const int ngcp,		/* number of points (starting from the 1st) whose parameters should not be modified.	
							* All B_ij (see below) with i<ncon are assumed to be zero
							控制点放在观测值的起始部分*/
		LidMC_VP *pLidPatch,
		TiePoint_Adj_mode TiePt_mode,
							//				 char *vmask,			/* visibility mask: vmask[i, j]=1 if point i visible in image j, 0 otherwise. nxm */
		double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
												* aj are the image j parameters, bi are the i-th point parameters,
												* size m*cnp + n*pnp
												未知数，偏心分量和安置角    
												pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
		//				const int pnp,			/* number of parameters 未知数个数*/
		orsLidSysParam oriParam,
		CalibParam_Type	param_type				/* 检校参数类型 */
		);

	//只解算3个角度, 最近邻对应, 内部进行GPS系统改正
// 	int  LidMC_Misalign_VTP_NN_rph_semiXYZ(
// 		const int nPoint,	        /* number of Virtual points */
// 		ATNPoint *pLidPoint,
// 		POINT3D *pGCP,
// 		double *pX,			
// 		const int pnp=6			/* number of parameters 未知数个数*/
// 		//				 double *pObs,			/*观测值(虚拟观测值，加权平均值)*/
// 		// 				 double errTh2			//(迭代终止条件,物方坐标改正);
// 		//				VP_mode mode        //虚拟点的计算方式
// 		);

	//平差精度检查
	int  LidMC_VPAdj_QA(
		const int nVP,
		const int ngcp,
		LidMC_VP *pLidVP,
		double *pInitX,			/* 未知数初值*/
		double *pAdjX,			/* 未知数平差值*/
		const int pnp,			/* number of parameters 未知数个数*/
		Calib_Method  mType,
		orsLidSysParam oriParam,
		CalibParam_Type	param_type		/* 参数类型*/
		);

public:
	double	*m_covar;	//未知数的协方差阵
	int		m_pn;		//未知数个数
	double  m_rms;		//单位权中误差（观测值）
	double  *m_corcoef;	//未知数之间的相关系数
};


#endif