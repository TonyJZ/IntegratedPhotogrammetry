#ifndef _Model_Correction_H_ZJ_2010_09_15_
#define _Model_Correction_H_ZJ_2010_09_15_

//////////////////////////////////////////////////////////////////////////
//  create by Tony J. Chang
//	data-driven methods
//  采用数学模型对激光点云进行纠正（非严格方法）
//////////////////////////////////////////////////////////////////////////


//仿射变换
int  LidMC_Affine(double *xSrc, double *ySrc,double *zSrc, int n, double *xDst, double *yDst, double *zDst, double *pAffine);




#endif