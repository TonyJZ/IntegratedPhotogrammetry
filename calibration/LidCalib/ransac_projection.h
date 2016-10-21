//ransac估算P矩阵，去除outliers
#ifndef _ESTIMATE_TRANSFORM_RANSAC_PROJECTION_ZJ_2010815_DEFINE_
#define _ESTIMATE_TRANSFORM_RANSAC_PROJECTION_ZJ_2010815_DEFINE_

#include "Align_def.h"
#include "Calib_Camera.h"

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


//pBuf1：物方点；		pBuf2：像方点
bool _lidCalib_Dll_ EstimateTransform_Projection(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, int &refineNum,
																_iphCamera *camera, double thresh=1.0) ;	//thresh 像素



#endif


