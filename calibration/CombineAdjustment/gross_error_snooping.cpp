#include "StdAfx.h"
#include "gross_error_snooping.h"
#include "CombineAdjustment/BA_def.h"

#include <gsl/gsl_randist.h>


//计算改正数的协因数阵
int cal_Qvv(void *pdata, double *jac, double *Qvv)
{

	return -1;
}

int fun_weitht_Li(double *W, void *pdata, double *jac, double *e)
{
	double *Qvv=NULL;
	int i, j, k;
	int nobs, nvar;
	double norm;
	double eL2_i, eL2;
	double *sigma2=NULL;	//各组观测值的单位权方差
	double *r=NULL;			//各组观测值的多余观测值
	double *p=NULL;			//各组观测值的验后权
	double *fprob=NULL;		//各组观测值的概率密度
	double sigma0_L2;		//总的单位权方差
	double r0;				//总的多余观测值

	double *ptr1, *ptr2, *ptr3;

	struct imgBA_globs *gl;
	gl=(struct imgBA_globs *)pdata;

	int ncls = gl->nObsCls;
	sigma2 = new double[ncls];
	r = new double[ncls];
	fprob = new double[ncls];
	p = new double[ncls];

	cal_Qvv(pdata, jac, Qvv);

	//计算ri, sigma_0和sigma_i
	sigma0_L2 = 0;
	switch(gl->BA_type)
	{
	case ImageBA_classic_W:

		nobs = gl->nproj*gl->mnp;
		nvar = gl->nimg*gl->cnp+gl->ntie*gl->pnp;
		r0 = nobs - nvar;

		ptr1 = r;
		*ptr1 = 0;
		ptr2 = e;
		ptr3 = sigma2; 
		for(i=0, j=0, norm=0.0; i<gl->nproj*gl->mnp; i++, j++)
		{//像点观测值
			(*ptr1) += Qvv[i*nobs+i]; //tr(Qvv)

			norm += ptr2[j]*ptr2[j];
		}

		*ptr3 = norm/(*ptr1);  //第i类观测值的方差
		sigma0_L2 += norm;

		
		sigma0_L2 /= r0;

		//计算验后权
		p[0] = sigma0_L2/sigma2[0];

		//第i类观测值的置信度指标
		fprob[0] = gsl_ran_fdist_pdf(gl->confidence, 1, *ptr1);

		for(i=0; i<gl->nproj*gl->mnp; i++)
		{
			ptr1 = e+i;	//v_ij
			double v2_ij = (*ptr1)*(*ptr1); //v_ij^2
			double r_ij = Qvv[i*nobs+i];	//r_ij
			double T_ij = v2_ij*p[0]/(sigma0_L2*r_ij);

			j = i/2;	//第j个像点
			k = i%2;	//x或y
			ptr2 = W + j*gl->mnp*gl->mnp;
			ptr3 = ptr2 + k*gl->mnp + k;

			if(T_ij<fprob[0])
			{
				*ptr3 = sigma0_L2 / sigma2[0];

			}
			else
			{
				*ptr3 = sigma0_L2*r_ij/v2_ij;
			}
		}

		break;
	case ImageBA_GPS_supported:

		nobs = gl->nproj*gl->mnp+gl->nimg*3;
		nvar = gl->nimg*gl->cnp+gl->ntie*gl->pnp+gl->nGPSoffset+gl->nGPSdrift*gl->nSurv;

		r0 = nobs - nvar; //总的多余观测值

		ptr1 = r;
		*ptr1 = 0;
		ptr2 = e;
		ptr3 = sigma2; 
		for(i=0, j=0, norm=0.0; i<gl->nproj*gl->mnp; i++, j++)
		{//像点观测值
			(*ptr1) += Qvv[i*nobs+i]; //tr(Qvv)

			norm += ptr2[j]*ptr2[j];
		}

		*ptr3 = norm/(*ptr1);  //第1类观测值的方差
		sigma0_L2 += norm;

		ptr1++;
		*ptr1 = 0.0;
		ptr2 = e+gl->nproj*gl->mnp;
		ptr3++;

		//gps观测值
		for(i=gl->nproj*gl->mnp, j=0, norm=0.0; i<nobs; i++, j++)
		{//像点观测值
			(*ptr1) += Qvv[i*nobs+i]; //tr(Qvv)

			norm += ptr2[j]*ptr2[j];
		}
		*ptr3 = norm/(*ptr1);  //第2类观测值的方差
		sigma0_L2 += norm;

		sigma0_L2 /= r0;

		//计算验后权
		p[0] = sigma0_L2/sigma2[0];
		p[1] = sigma0_L2/sigma2[1];

		//第i类观测值的置信度指标
		fprob[0] = gsl_ran_fdist_pdf(gl->confidence, 1, r[0]);
		fprob[1] = gsl_ran_fdist_pdf(gl->confidence, 1, r[1]);

		for(i=0; i<gl->nproj*gl->mnp; i++)
		{
			ptr1 = e+i;	//v_ij
			double v2_ij = (*ptr1)*(*ptr1); //v_ij^2
			double r_ij = Qvv[i*nobs+i];	//r_ij
			double T_ij = v2_ij*p[0]/(sigma0_L2*r_ij);

			j = i/2;	//第j个像点
			k = i%2;	//x或y
			ptr2 = W + j*gl->mnp*gl->mnp;
			ptr3 = ptr2 + k*gl->mnp + k;

			if(T_ij<fprob[0])
			{
				*ptr3 = sigma0_L2 / sigma2[0];

			}
			else
			{
				*ptr3 = sigma0_L2*r_ij/v2_ij;
			}
		}

		for(i=gl->nproj*gl->mnp; i<nobs; i++)
		{
			ptr1 = e+i;	//v_ij
			double v2_ij = (*ptr1)*(*ptr1); //v_ij^2
			double r_ij = Qvv[i*nobs+i];	//r_ij
			double T_ij = v2_ij*p[1]/(sigma0_L2*r_ij);

			j = i/3;	//第j个gps
			k = i%3;	//X, Y, Z
			ptr2 = W + gl->nproj*gl->mnp*gl->mnp + j*3*3;
			ptr3 = ptr2 + k*gl->mnp + k;

			if(T_ij<fprob[1])
			{
				*ptr3 = sigma0_L2 / sigma2[1];

			}
			else
			{
				*ptr3 = sigma0_L2*r_ij/v2_ij;
			}
		}

		break;
	default:
		break;
	}


	if(sigma2)	delete sigma2;	sigma2=NULL;
	if(r)		delete r;		r=NULL;
	if(fprob)	delete fprob;	fprob=NULL;
	if(p)		delete p;		p=NULL;
	return 1;
}