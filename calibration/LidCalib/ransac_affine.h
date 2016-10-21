//ransac估算仿射变换参数，去除outliers
#ifndef _ESTIMATE_TRANSFORM_RANSAC_AFFINE_ZJ_2010529_DEFINE_
#define _ESTIMATE_TRANSFORM_RANSAC_AFFINE_ZJ_2010529_DEFINE_

#include "Align_def.h"
//#include "bundler\bundler_vector.h"
#include "dpsmatrix.h"
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

void align_affine(int num_pts, v3_t *r_pts, v3_t *l_pts, double *Tout, int refine) ;

static v3_t *condition_points(int num_points, v3_t *pts, double *T);

static int CountInliers(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, double *M, double thresh, 
						std::vector<int> &inliers, double &rms);


//估算仿射变换参数
bool _lidCalib_Dll_ EstimateTransform_Affine(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, int &refineNum,
											 double *AffineMat, double Outlierth=2.0) ;



#endif