#ifndef _ADJUSTMENT_PUBLIC_FUNCTION_INCLUDE_
#define _ADJUSTMENT_PUBLIC_FUNCTION_INCLUDE_


void calcImgProjJacKRTS(double t[3], double qr0[3], double imgpt[2], double objpt[3], 
	double jacmRT[2][6], double jacmS[2][3], void *adata);

void calcImgProj(/*double *calibParam, */double t[3], double angle[3], double objpt[3],
	double imgpt[2], void *adata);

#endif