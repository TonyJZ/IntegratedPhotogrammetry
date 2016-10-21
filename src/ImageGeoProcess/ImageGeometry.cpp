#include "StdAfx.h"
#include "ImageGeometry.h"
#include "Geometry.h"
// RobustReor.cpp: implementation of the CRobustReor class.
//
//////////////////////////////////////////////////////////////////////
#include <math.h>
#include "iphMatrix.h"



void ReliablityMatrix(double *A, double *P, int m, int n, double *Rlb)
{
	int i,j;

	double* ATP=new double[n*m];
	double* ATPA=new double[n*n];
	double* Temp=new double[m*n];

	// ATP = AT*P
	for(i=0; i<n; i++)
	{
		for(j=0;j<m;j++)
			ATP[i*m+j] = A[j*n+i] * P[j];
	}

	Matrixmul(ATP, A, n, m, n, ATPA);

	invers_matrix( ATPA, n );
	Matrixmul( A, ATPA, m, n, n, Temp);

	Matrixmul( Temp, ATP, m, n, m, Rlb);

	for(i=0; i<m; i++)
	{
		for(j=0; j<m; j++)
		{
			if(i == j)
				Rlb[ i*m + j] = 1.0 - Rlb[i*m+j];
			else
				Rlb[ i*m + j] = -Rlb[i*m+j];
		}
	}

	delete []ATP;
	delete []ATPA;
	delete []Temp;
}



void Correspondence(double *Rlb, int n, double *V, double *pCoefs)
{
	int i,j;

	double vMean, vvSum;

	vMean = vvSum = 0;
	for(i=0;i<n;i++)
	{
		vMean += V[i];
		vvSum += V[i]*V[i];
	}
	vMean /= n;

	double *r;
	for( i=0; i<n; i++)
	{
		double rMean, rrSum, vrSum;

		rMean = rrSum = vrSum = 0;

		r = Rlb + i*n;
		for(j=0;j<n;j++)
		{
			rMean += *r;
			rrSum += *r * *r;
			vrSum += V[j] * *r++;
		}
		rMean /= n;

		*pCoefs++ = ( vrSum - vMean*rMean) / sqrt( (vvSum - vMean*vMean) * (rrSum-rMean*rMean) );
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CRobustReor::CRobustReor()
{
	m_camera=NULL;
}



CRobustReor::~CRobustReor()
{
}

void  CRobustReor::SetCamera(iphCamera *camera)
{
	m_camera=camera;
	m_f=m_camera->m_f;
}



static double *static_pCoefs;

int compareCoefWithIndex( const void *index1, const void *index2 )
{
	double coef1 = fabs( static_pCoefs[ *( (int *)index1 ) ] );
	double coef2 = fabs( static_pCoefs[ *( (int *)index2 ) ] );

	if( coef1 > coef2 )
		return 1;
	else if( coef1 < coef2 )
		return -1;
	return 0;
}


void CRobustReor::PixelPointsToImagePoints( PointPAIR *pixelPoints, int n,  PointPAIR *imgPoints )
{
	int i;

	for( i=0; i<n; i++ )
	{
		m_camera->Image2Photo( pixelPoints[i].xl, pixelPoints[i].yl, imgPoints[i].xl, imgPoints[i].yl );
		m_camera->Image2Photo( pixelPoints[i].xr, pixelPoints[i].yr, imgPoints[i].xr, imgPoints[i].yr );
		imgPoints[i].coef = pixelPoints[i].coef;
	}
}


//////	独立像对
void CRobustReor::Compute( PointPAIR *pixelPoints, int n, double vyTh)
{
	PointPAIR *points = new PointPAIR[n];

	PixelPointsToImagePoints(pixelPoints, n, points );

	////////////////////////////////////

	int i,j, k;

	double para[5]={0,0,0,0,0};//参数

	double X1,Y1,Z1,X2,Y2,Z2;///像空间辅助坐标
	double ATPA[5*5], ATPL[5];
	double Rl[9], Rr[9];

	//与误差方程有关的一些变量
	double *A =new double[n*5];	//系数
	double  w;	//常数项

	double *Rlb=new double[n*n];
	double *P=new double[n];
	double *V=new double[n];
	double *Q=new double[n];

	static_pCoefs=new double[n];

	int pos=0;
	int count = n;
	///////////// point Index ////////
	int *pointIndex = new int[n];
	for( i=0; i<n; i++)
		pointIndex[i] = i;

	////////////////////////////////////

	double maxmVy;
	double RMS;
	for( int iter=0; iter<40; iter++)
	{	
		// 相对定向计算
		for( int times=0; times < 10; times++)
		{
			RotateMat_fwk( para[0], 0, para[1], Rl );
			RotateMat_fwk( para[2], para[3], para[4], Rr );

			//points.m_sort[i]
			memset( ATPA, 0, 5*5*sizeof(double));
			memset( ATPL, 0, 5*sizeof(double));

			double *a=A;
			for(k=0; k<count; k++)///逐点法化
			{
				// 计算左右片上像点的像空间辅助坐标
				pos = pointIndex[k];

				X1= Rl[0] * points[pos].xl + Rl[1] *points[pos].yl - Rl[2] *m_f;
				Y1= Rl[3] * points[pos].xl + Rl[4] *points[pos].yl - Rl[5] *m_f;
				Z1= Rl[6] * points[pos].xl + Rl[7] *points[pos].yl - Rl[8] *m_f;

				X2= Rr[0] * points[pos].xr + Rr[1] *points[pos].yr - Rr[2] *m_f;
				Y2= Rr[3] * points[pos].xr + Rr[4] *points[pos].yr - Rr[5] *m_f;
				Z2= Rr[6] * points[pos].xr + Rr[7] *points[pos].yr - Rr[8] *m_f;

				*a++ = -X1 * Y2;			//	f1
				*a++ =  X1 * Z2;			//	k1	
				*a++ =  X2 * Y1;			//	f2	
				*a++ =  Z1 * Z2 + Y1 * Y2;	//	w2	
				*a++ = -X2 * Z1;			//	k2	

				Q[k] = -( Y1 * Z2 - Y2 * Z1 );

				V[k] = m_f/Z1*Y1 - m_f/Z2*Y2;

				P[k] = w = points[pos].coef;

				a -= 5;
				for( i= 0 ; i<5; i++)
				{
					for( j= 0 ; j<5; j++)
						ATPA[i*5+j] += w*a[i]*a[j];
					ATPL[i] += w*a[i]* Q[k];
				}
				a += 5;
			}

			//////最小二乘解求改正值	
			Gauss( ATPA, ATPL, 5 );

			/////计算相对定向元素的新值	
			double limiterr=0;	//未知数的最大改正数
			for(int j=0;j<5;j++)
			{
				para[j] += ATPL[j];
				if( fabs(ATPL[j]) > limiterr )
				{
					limiterr = fabs(ATPL[j]);
				}
			}

			if( limiterr < 1.0E-6 )
				break;
		}

		maxmVy = 0;
		RMS = 0;
		for(i=0;i<count;i++)
		{
			if( maxmVy < fabs( V[i] ) )
				maxmVy = fabs( V[i] );
			RMS += V[i]*V[i];
		}
		RMS=sqrt(RMS/(count-5));

		if( maxmVy < vyTh )
			break;

		///////////// 粗差检测 /////////////////
		//计算可靠矩阵
		//ReliablityMatrix( A, P, count, 5, Rlb);

		//计算相关系数
		//Correspondence( Rlb, count, Q, static_pCoefs );

		//order by correspondence

		//memcpy( static_pCoefs, V, count*sizeof( double ) );
		for(k=0; k<count; k++)///逐点法化
		{
			pos = pointIndex[k];
			static_pCoefs[pos] = V[k];
		}

		qsort( pointIndex, count, sizeof(int), compareCoefWithIndex );

		/////////////////

		count -= count/20;

		/////////////////

		if( count < 6 )	{	
			AfxMessageBox("相对定向点精度或者个数不够");
			return;
		}
	}

	m_modelPara.XsL = m_modelPara.YsL = 10000;	
	m_modelPara.ZsL = 10*m_f;
	m_modelPara.XsR = m_modelPara.XsL + 5*m_f;	
	m_modelPara.YsR = m_modelPara.YsL;
	m_modelPara.ZsR = 10*m_f;

	memcpy ( m_modelPara.Rl, Rl, 9*sizeof(double) );
	memcpy ( m_modelPara.Rr, Rr, 9*sizeof(double) );

	//////////////
	for( i =0; i<count; i++ )
	{
		pos = pointIndex[i];
		pixelPoints[pos].isGross = false;
	}

	for( i=count; i<n; i++)
	{
		pos = pointIndex[i];
		pixelPoints[pos].isGross = true;
	}


	/////////释放内存
	delete  A;
	delete  Rlb;
	delete  V;
	delete  Q;
	delete  P;
	delete static_pCoefs;
	delete points;
	delete pointIndex;
}

void CRobustReor::GetOrientationParam(modelPARA *param)
{
	param->XsL=m_modelPara.XsL;	param->YsL=m_modelPara.YsL;	param->ZsL=m_modelPara.ZsL;
	param->XsR=m_modelPara.XsR;	param->YsR=m_modelPara.YsR;	param->ZsR=m_modelPara.ZsR;

	memcpy(param->Rl, m_modelPara.Rl, sizeof(double)*9);
	memcpy(param->Rr, m_modelPara.Rr, sizeof(double)*9);
}