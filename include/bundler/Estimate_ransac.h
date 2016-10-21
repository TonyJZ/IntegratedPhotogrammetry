#ifndef _ESTIMATE_RANSAC_ZJ_2010529_DEFINE_
#define _ESTIMATE_RANSAC_ZJ_2010529_DEFINE_

#include "AT_StructureDef.h"
#include "bundler_vector.h"



void align_affine(int num_pts, v3_t *r_pts, v3_t *l_pts, double *Tout, int refine) ;
static v3_t *condition_points(int num_points, v3_t *pts, double *T);
static int CountInliers(AT_3DTiePoint *p3DTiePtsObvs, AT_ImgPoint *pImgPtsEstiV, int ptNum, double *M, double thresh, 
						std::vector<int> &inliers, double &rms);

bool EstimateTransform(AT_3DTiePoint *p3DTiePtsObvs, AT_ImgPoint *pImgPtsEstiV, int ptNum, double Outlierth) ;





#endif