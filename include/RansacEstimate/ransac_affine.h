//ransac估算仿射变换参数，去除outliers
#ifndef _ESTIMATE_TRANSFORM_RANSAC_AFFINE_ZJ_2010529_DEFINE_
#define _ESTIMATE_TRANSFORM_RANSAC_AFFINE_ZJ_2010529_DEFINE_

//#include "Align_def.h"
//#include "bundler\bundler_vector.h"
#include "dpsmatrix.h"
#include <vector>
#include "PointPair_Struct.h"

#ifdef RANSACESTIMATE_EXPORTS
#define  _estimate_Dll_  __declspec(dllexport)
#else
#define  _estimate_Dll_  __declspec(dllimport)	
#endif

void align_affine(int num_pts, v3_t *r_pts, v3_t *l_pts, double *Tout, int refine) ;

static v3_t *condition_points(int num_points, v3_t *pts, double *T);

static int CountInliers(myPtPair *point_pairs, int ptNum, double *M, double thresh, 
						std::vector<int> &inliers, double &rms);


//估算仿射变换参数
//这个函数好像有问题，在openrs里用相似功能的函数  2014.12.7  zj
bool _estimate_Dll_ EstimateTransform_Affine(myPtPair *param, int ptNum, int &refineNum, 
	double Outlierth, double *Mout) ;



#endif