#ifndef _LIDAR_STRIP_ADJUSTMENT_POS_DRIFT_MODEL_INCLUDE_H__
#define _LIDAR_STRIP_ADJUSTMENT_POS_DRIFT_MODEL_INCLUDE_H__

#include "CalibBasedef.h"
#include "LidAdj_def.h"

interface orsILidarGeoModel;

//pos漂移的条带平差模型
	int  LSA_POSDrift(
		const char *pszResultFile,		/* 平差结果文件 */
		const int nkeyPt,	 /* 激光点总数 */
		const int ngcp,		/* number of points (starting from the 1st) whose parameters should not be modified.	
							* All B_ij (see below) with i<ncon are assumed to be zero
							控制点放在观测值的起始部分*/
		LidAdj_keyPt *pLidTies,	//				 char *vmask,			/* visibility mask: vmask[i, j]=1 if point i visible in image j, 0 otherwise. nxm */
		double *pX,				/* unknowns */
		int nParams,            //number of unknowns   
		unsigned long param_type,       /* 条带平差类型 */
		/*oriCalibParam oriParam*/
		orsLidSysParam oriParam,
		double *refTime,         /* 各条带的参考时间数组 */
		int nStrip,               /*  条带数 */
		orsILidarGeoModel *pLidGeo
		);



#endif