#include "StdAfx.h"
#include "CombineAdjustment/LensCalib.h"

#include "CombineAdjustment/BA_def.h"
#include "F:\OpenRS\IntegratedPhotogrammetry\external\levmar-2.5\levmar.h"



//计算畸变改正后的像点坐标(支持除主距外的畸变改正模型)
//p:未知数， y改正后的坐标，m未知数个数，n方程个数
void LensDistortion_correct(double *p, double *y, int m, int n, void *data)
{
	register int i, j;
	struct imgBA_globs *dptr;
	int nPoints;
	double xi, yi, xc, yc, x2, y2, xy, r2, r4;
	double dx=0, dy=0;
	double *imgPts, *pObs;
	
	dptr=(struct imgBA_globs *)data; 
	nPoints=dptr->nproj; //像点数
	imgPts = dptr->pObs; //原始像点坐标
	pObs = dptr->pAdjObs;	//像点坐标观测值(平差后坐标)

	double dist_x0, dist_y0, dist_k0, dist_k1=0, dist_k2, dist_p1=0, dist_p2;

	dist_x0 = p[0];	dist_y0 = p[1];
	dist_k0 = p[2];	dist_k1 = p[3];	dist_k2 = p[4];
	dist_p1 = p[5];	dist_p2 = p[6];


	for(i=0; i<nPoints; i++)
	{
		j = i*2;
		xi = imgPts[j];
		yi = imgPts[j+1];

		xc = xi - dist_x0;
		yc = yi - dist_y0;

		x2 = xc*xc;	xy = xc*yc;	y2 = yc*yc;
		r2 = x2 + y2;
		r4 = r2*r2;

		if(dist_k1 != 0)
		{
			dx = xc*(dist_k0+dist_k1*r2+dist_k2*r4);
			dy = yc*(dist_k0+dist_k1*r2+dist_k2*r4);
		}
		
		// 切向畸变
		if( dist_p1 != 0 )	{
			dx += dist_p1*( r2 + 2*x2 ) + 2*dist_p2*xy;
			dy += 2*dist_p1*xy + dist_p2*( r2 + 2*y2 );
		}

		xc += dx;
		yc += dy;

		y[j]	= /*pObs[j] - */xc;
		y[j+1]	= /*pObs[j+1] - */yc;
	}
}

//计算镜头畸变的jac
//m未知数个数，n方程个数
void jac_LensDistortion(double *p, double *jac, int m, int n, void *data)
{
	register int i, j;
	struct imgBA_globs *dptr;
	int nPoints;
	double xi, yi, r2, r4;
	double *pPDC;
	double *pdx, *pdy;
	double *hx;

	dptr=(struct imgBA_globs *)data;
	nPoints=dptr->nproj;

	hx = dptr->hx;
	//计算当前的像点坐标
	LensDistortion_correct(p, hx, m, n, data);

	for(i=0; i<nPoints; i++)
	{
		pPDC = jac + i*2*m;
		pdx = pPDC;
		pdy = pdx + m;

		j = i*2;
		xi = hx[j];
		yi = hx[j+1];
		r2 = xi*xi + yi*yi;
		r4 = r2*r2;

		pdx[0] = xi;		//k0
		pdx[1] = xi*r2;		//k1
		pdx[2] = xi*r4;		//k2

		pdy[0] = yi;
		pdy[1] = yi*r2;
		pdy[2] = yi*r4;
	}
}

//利用有限差分法计算镜头畸变的jac
//m未知数个数，n方程个数
void jac_LensDistortion_diff(double *p, double *jac, int m, int n, void *data)
{
	register int i, j;
	struct imgBA_globs *dptr;
	int nPoints;
	double xi, yi, r2, r4;
	double *pPDC;
	double *pdx, *pdy;
	double *hx, *hxx;

	dptr=(struct imgBA_globs *)data;
	nPoints=dptr->nproj;

	hx = dptr->hx;
	hxx = dptr->hxx;
	//计算当前的像点坐标
	LensDistortion_correct(p, hx, m, n, data);

	double d, tmp;
	double delta=1E-06;
	for(j=0; j<m; ++j)
	{
		/* determine d=max(1E-04*|p[j]|, delta), see HZ */
		d=(1E-04)*p[j]; // force evaluation
		d=fabs(d);
		if(d<delta)
			d=delta;

		tmp=p[j];
		p[j]+=d;

		LensDistortion_correct(p, hxx, m, n, data);

		p[j]=tmp; /* restore */

		d=(1.0)/d; /* invert so that divisions can be carried out faster as multiplications */
		for(i=0; i<n; ++i)
		{
			jac[i*m+j]=(hxx[i]-hx[i])*d;
		}
	}
}

bool Cal_LensDistortion(void *pdata, double *distParam)
{
	imgBA_globs  *gl=NULL;
	gl=(struct imgBA_globs *)pdata;

	int param_num = gl->lens_selfCalib_param_num;

	int numofimgpt = gl->nproj;
	double *L=NULL;

	L = new double[numofimgpt*2];
	for(int i=0; i<numofimgpt*2; i++)
	{
		L[i] = gl->pAdjObs[i];
	}

	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;

	
	int ret=dlevmar_der(LensDistortion_correct, jac_LensDistortion_diff, distParam, L, param_num, numofimgpt*2, 1000, 
						opts, info, NULL, NULL, pdata);

	gl->adj_rms = info[1];

	if(L)		delete[] L;		L=NULL;
	return true;
}