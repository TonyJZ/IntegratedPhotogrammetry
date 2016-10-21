//ransac¹ÀËãF¾ØÕó£¬È¥³ýoutliers
#ifndef _ESTIMATE_TRANSFORM_RANSAC_EPIPOLAR_ZJ_2010812_DEFINE_
#define _ESTIMATE_TRANSFORM_RANSAC_EPIPOLAR_ZJ_2010812_DEFINE_

#include "Align_def.h"
//#include "bundler\bundler_vector.h"
#include "dpsmatrix.h"
#include <vector>
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

//¹ÀËã»ù±¾¾ØÕó
//F0:  ³õÊ¼»ù±¾¾ØÕó
bool _lidCalib_Dll_ estimate_fmatrix_nonlinear(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, double *F0, double *Fout);


bool _lidCalib_Dll_ EstimateTransform_Epipolar(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, int &refineNum,
											_iphCamera *camera, double thresh=1.0) ;	//thresh ÏñËØ



#endif