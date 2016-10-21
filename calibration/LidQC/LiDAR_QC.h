#ifndef _LIDAR_QC_H_ZJ_2010_08_24_
#define _LIDAR_QC_H_ZJ_2010_08_24_

//////////////////////////////////////////////////////////////////////////
/*
	LiDAR的QC方法

*/


#ifdef _LIDQC_
#define  _lidQC_Dll_  __declspec(dllexport)
#else
#define  _lidQC_Dll_  __declspec(dllimport)	
#endif

#ifndef _LIDQC_
#ifdef _DEBUG
#pragma comment(lib,"LidQCD.lib")
#else
#pragma comment(lib,"LidQC.lib")
#endif
#endif

#include "CalibBasedef.h"
#include "AlignPrj.h"

//more details of HONV(Histogram of Oriented Normal Vectors) can be found 
// in paper "Histogram of Oriented Normal Vectors for 	 Object Recognition with a Depth Sensor"
struct HONV
{
	double azimuth;  //radian
	double zenith;
	int frequency;
};

//根据指定参数计算连接平面的误差
void _lidQC_Dll_ LidQC_Plane(orsLidSysParam param, CAlignPrj *algPrj, const char *pszFileName);


//根据指定参数计算VP点误差
void _lidQC_Dll_ LidQC_VP(orsLidSysParam param, CalibParam_Type ctype, CAlignPrj *algPrj, const char *pszFileName);



int _lidQC_Dll_ Stat_Plane_HONV( CAlignPrj *algPrj,  HONV **pHONV, int &nPlane );

#endif