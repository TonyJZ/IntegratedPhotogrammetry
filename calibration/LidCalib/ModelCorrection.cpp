#include "StdAfx.h"
#include "ModelCorrection.h"
#include "bundler\matrix.h"

/*
	|m11  m12  m13  m14|
	|m21  m22  m23  m24|
	|m31  m32  m33  m34|
	|m41  m42  m43  m44|
*/
//DPS中的仿射变换参数解算，为什么那么解？  2010.9.15
//   m11 + m12*xs + m13*ys + m14*zs = xd
int LidMC_Affine(double *xSrc, double *ySrc,double *zSrc, int n, double *xDst, double *yDst, double *zDst, double *pAffine)
{
	double *A=NULL;
	double *L=NULL;
	double *X=NULL;

	A=new double[4*n];
	L=new double[n];
	
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<n; j++)
		{
			A[j*4]=1;
			A[j*4+1]=xSrc[j];
			A[j*4+2]=ySrc[j];
			A[j*4+3]=zSrc[j];

			L[j*4]=xDst[j];

		}
		X=pAffine+i*4;
		dgelsy_driver(A, L, X, n, 4, 1);
	}

	if(A)
	{
		delete A;
		A=0;
	}
	if(L)
	{
		delete L;
		L=0;
	}

	return 0;
}