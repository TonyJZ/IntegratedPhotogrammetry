//ransac¹ÀËãF¾ØÕó£¬È¥³ýoutliers
#ifndef _ESTIMATE_TRANSFORM_RANSAC_EPIPOLAR_ZJ_2010812_DEFINE_
#define _ESTIMATE_TRANSFORM_RANSAC_EPIPOLAR_ZJ_2010812_DEFINE_

//#include "Align_def.h"
//#include "bundler\bundler_vector.h"
#include "dpsmatrix.h"
#include <vector>
#include "Calib_Camera.h"
#include "PointPair_Struct.h"

// #include "orsBase\orsIPlatform.h"
// #include "orsGeometry\orsPointPairSet.h"

//#include "orsFeature2d\orsIAlg2DFeatureRegistering.h"

#ifdef RANSACESTIMATE_EXPORTS
#define  _estimate_Dll_  __declspec(dllexport)
#else
#define  _estimate_Dll_  __declspec(dllimport)	
#endif

//¹ÀËã»ù±¾¾ØÕó
//F0:  ³õÊ¼»ù±¾¾ØÕó
bool estimate_fmatrix_nonlinear(myPtPair *point_pairs, int ptNum, double *F0, double *Fout);


bool _estimate_Dll_ EstimateTransform_Epipolar(myPtPair *point_pairs, int ptNum, int &refineNum,
											_iphCamera *camera, double thresh=1.0) ;	//thresh ÏñËØ



#endif