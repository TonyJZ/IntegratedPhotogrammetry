#ifndef _COMBINE_BUNDLE_ADJUSTMENT_DEFINE_INCLUDE_
#define _COMBINE_BUNDLE_ADJUSTMENT_DEFINE_INCLUDE_

#include "Calib_Camera.h"
#include "CATTStruct_def.h"

//相机系统附加参数类型
enum orsCamSysExtraParam_TYPE
{
	CamGeo_rph=0,			//三个视准轴偏角
	CamGeo_rphxyz,			//视准轴偏角+偏心分量

};

//相机系统参数（测量获得的）
struct  orsCamSysExtraParam
{
	double lever_arm[3];		//偏心分量
	double boresight_angle[3];	//视准轴偏角
};

enum CamPOS_TYPE
{
	POS_Raw=0,	//直接使用原始POS
	POS_IntepolatedFromLIDAR	//从点云航迹中插值的POS
};

/* pointers to additional data, used for computed image projections and their jacobians */
struct imgCalib_globs{
	double *pUnknowns;	//未知数数组 检校参数+物方坐标
	double *pImgpts;	//像点坐标观测值
	//	double *p3Ds;	//物方点坐标
	double *pImgExor;	//相片外方位元素
	char *mask;	//标记稀疏矩阵
	int nVFrame;	//虚拟的相片数(待求的相片)相对于安置误差来说
	int nimg, ntie, nproj; //实际相片数，连接点数，像方点数
	int cnp, pnp, mnp; /* cnp相片未知数个数，pnp物方点未知数个数, mnp像方点改正数个数 */
	int ngcp;	//控制点数
	int nfixedImg;	//固定片数

	orsCamSysExtraParam_TYPE   calibType;
	orsCamSysExtraParam		   *calibParam;
	_iphCamera  *camInfo;
	FILE *fplog;

	double *hx, *hxx; //用于计算jac
};

enum orsImageBA_TYPE
{
	ImageBA_classic=0,				//传统空三
	ImageBA_classic_W,				//选权迭代法进行粗差定位		
	ImageBA_GPS_supported,			//GPS辅助
	ImageBA_POS_supported			//POS辅助
};

//原始POS信息
struct POSinfo
{
	int survID;		//从0开始
	double Xgps, Ygps, Zgps;
	double phi, omega, kappa;
	double gpstime;
	double t0;	//时间基准点
};

struct GPSinfo
{
	int survID;		//从0开始
	double XYZ[3];
	double gpstime;
	double t0;
};

#define		ATT_GS_Undo				0			//不挑粗差
#define		ATT_GS_IterWght_LI		1			//选权迭代 （李德仁法）


/* pointers to additional data, used for computed image projections and their jacobians */
struct imgBA_globs{
	double *pUnknowns;	//未知数数组 检校参数+物方坐标
	double *pObs;	//观测值，最开始为像点坐标，(非摄影测量观测值排在后面)
	double *pAdjObs; //平差后的观测值，用于验后补偿
	//	double *p3Ds;	//物方点坐标
	//	double *pImgExor;	//相片外方位元素
	char *mask;	//标记稀疏矩阵
	//	int nVFrame;	//虚拟的相片数(待求的相片)相对于安置误差来说
	int nimg, ntie, nproj; //实际相片数，连接点数，像方点数
	int cnp, pnp, mnp; /* cnp相片未知数个数，pnp物方点未知数个数, mnp像方点改正数个数 */
	int ngcp;	//控制点数
	int nfixedImg;	//固定片数

	//GPS辅助参数
	int nGPSoffset;	//GPS偏心分量个数
	int nGPSdrift;	//GPS漂移参数个数
//	double GPSt0;	//参考时刻
//	double *pTimes;	//相片曝光时间
//	double *pGpsObs; //GPS观测值
	GPSinfo *pGpsObs; //GPS观测值
	int nSurv;       //测区数（多个时间段获取的影像）

	/////////////////辅助信息////////////////////
	_iphCamera  *camInfo;  //相机信息
	orsImageBA_TYPE	BA_type; //空三类型
	FILE *fplog;       //状态输出
	const char *pszImgPtResidual;	//平差后的像点残差文件名
	double *hx, *hxx; //用于计算jac的临时存储空间

	POSinfo *posObvs;	//pos观测信息
	std::vector<ATT_GCP> gcp;	//控制点信息

	double *sigma0;		//观测值的先验单位权中误差
	int		nObsCls;		//观测值的类别数
	double  confidence;	//挑粗差的显著水平（0.003）

	int     gross_snooping;	//粗差探测方法
	double	raw_rms;		//平差前的观测值中误差
	double	adj_rms;		//平差后的观测值中误差

	int  lens_selfCalib_param_num;		//镜头自检校参数个数
};


#endif