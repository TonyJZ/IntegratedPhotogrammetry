#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "CombineAdjustment/orsAlgCameraCalib.h"
#include "\OpenRS\external\source\sba-1.6\sba.h"
#include "\OpenRS\external\source\sba-1.6\sba_chkjac.h"
#include "\OpenRS\external\source\sba-1.6\compiler.h"
#include "orsMath/orsIMatrixService.h"
#include "dpsMatrix.h"

#define MAXITER         100
#define MAXITER2        150

#define SBA_EPSILON       1E-12
#define SBA_EPSILON_SQ    ( (SBA_EPSILON)*(SBA_EPSILON) )

#define SBA_ONE_THIRD     0.3333333334 /* 1.0/3.0 */


#define emalloc(sz)       emalloc_(__FILE__, __LINE__, sz)

#define FABS(x)           (((x)>=0)? (x) : -(x))

#define ROW_MAJOR         0
#define COLUMN_MAJOR      1
#define MAT_STORAGE       COLUMN_MAJOR

/* contains information necessary for computing a finite difference approximation to a jacobian,
 * e.g. function to differentiate, problem dimensions and pointers to working memory buffers
 */
struct fdj_data_x_ {
  void (*func)(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata); /* function to differentiate */
  int cnp, pnp, mnp;  /* parameter numbers */
  int *func_rcidxs,
      *func_rcsubs;   /* working memory for func invocations.
                       * Notice that this has to be different
                       * than the working memory used for
                       * evaluating the jacobian!
                       */
  double *hx, *hxx;   /* memory to save results in */
  void *adata;
};

/* auxiliary memory allocation routine with error checking */
inline static void *emalloc_(char *file, int line, size_t sz)
{
	void *ptr;

	ptr=(void *)malloc(sz);
	if(ptr==NULL){
		fprintf(stderr, "SBA: memory allocation request for %u bytes failed in file %s, line %d, exiting", sz, file, line);
		exit(1);
	}

	return ptr;
}

/* Given a parameter vector p made up of the 3D coordinates of n points and the parameters of m cameras, compute in
 * jac the jacobian of the predicted measurements, i.e. the jacobian of the projections of 3D points in the m images.
 * The jacobian is approximated with the aid of finite differences and is returned in the order
 * (A_11, B_11, ..., A_1m, B_1m, ..., A_n1, B_n1, ..., A_nm, B_nm),
 * where A_ij=dx_ij/da_j and B_ij=dx_ij/db_i (see HZ).
 * Notice that depending on idxij, some of the A_ij, B_ij might be missing
 *
 * Problem-specific information is assumed to be stored in a structure pointed to by "dat".
 *
 * NOTE: The jacobian (for n=4, m=3) in matrix form has the following structure:
 *       A_11  0     0     B_11 0    0    0
 *       0     A_12  0     B_12 0    0    0
 *       0     0     A_13  B_13 0    0    0
 *       A_21  0     0     0    B_21 0    0
 *       0     A_22  0     0    B_22 0    0
 *       0     0     A_23  0    B_23 0    0
 *       A_31  0     0     0    0    B_31 0
 *       0     A_32  0     0    0    B_32 0
 *       0     0     A_33  0    0    B_33 0
 *       A_41  0     0     0    0    0    B_41
 *       0     A_42  0     0    0    0    B_42
 *       0     0     A_43  0    0    0    B_43
 *       To reduce the total number of objective function evaluations, this structure can be
 *       exploited as follows: A certain d is added to the j-th parameters of all cameras and
 *       the objective function is evaluated at the resulting point. This evaluation suffices
 *       to compute the corresponding columns of *all* A_ij through finite differences. A similar
 *       strategy allows the computation of the B_ij. Overall, only cnp+pnp+1 objective function
 *       evaluations are needed to compute the jacobian, much fewer compared to the m*cnp+n*pnp+1
 *       that would be required by the naive strategy of computing one column of the jacobian
 *       per function evaluation. See Nocedal-Wright, ch. 7, pp. 169. Although this approach is
 *       much faster compared to the naive strategy, it is not preferable to analytic jacobians,
 *       since the latter are considerably faster to compute and result in fewer LM iterations.
 */
static void sba_fdjac_x(
    double *p,                /* I: current parameter estimate, (m*cnp+n*pnp)x1 */
    struct sba_crsm *idxij,   /* I: sparse matrix containing the location of x_ij in hx */
    int    *rcidxs,           /* work array for the indexes of nonzero elements of a single sparse matrix row/column */
    int    *rcsubs,           /* work array for the subscripts of nonzero elements in a single sparse matrix row/column */
    double *jac,              /* O: array for storing the approximated jacobian */
    void   *dat)              /* I: points to a "fdj_data_x_" structure */
{
  register int i, j, ii, jj;
  double *pa, *pb, *pqr, *ppt;
  register double *pAB, *phx, *phxx;
  int n, m, nm, nnz, Asz, Bsz, ABsz, idx;

  double *tmpd;
  register double d;

  struct fdj_data_x_ *fdjd;
  void (*func)(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
  double *hx, *hxx;
  int cnp, pnp, mnp;
  void *adata;


  /* retrieve problem-specific information passed in *dat */
  fdjd=(struct fdj_data_x_ *)dat;
  func=fdjd->func;
  cnp=fdjd->cnp; pnp=fdjd->pnp; mnp=fdjd->mnp;
  hx=fdjd->hx;
  hxx=fdjd->hxx;
  adata=fdjd->adata;

  n=idxij->nr; m=idxij->nc;
  pa=p; pb=p+m*cnp;
  Asz=mnp*cnp; Bsz=mnp*pnp; ABsz=Asz+Bsz;

  nm=(n>=m)? n : m; // max(n, m);
  tmpd=(double *)emalloc(nm*sizeof(double));

  (*func)(p, idxij, fdjd->func_rcidxs, fdjd->func_rcsubs, hx, adata); // evaluate supplied function on current solution

  if(cnp){ // is motion varying?
    /* compute A_ij */
    for(jj=0; jj<cnp; ++jj){
      for(j=0; j<m; ++j){
        pqr=pa+j*cnp; // j-th camera parameters
        /* determine d=max(SBA_DELTA_SCALE*|pqr[jj]|, SBA_MIN_DELTA), see HZ */
        d=(double)(SBA_DELTA_SCALE)*pqr[jj]; // force evaluation
        d=FABS(d);
        if(d<SBA_MIN_DELTA) d=SBA_MIN_DELTA;

        tmpd[j]=d;
        pqr[jj]+=d;
      }

      (*func)(p, idxij, fdjd->func_rcidxs, fdjd->func_rcsubs, hxx, adata);

      for(j=0; j<m; ++j){
        pqr=pa+j*cnp; // j-th camera parameters
        pqr[jj]-=tmpd[j]; /* restore */
        d=1.0/tmpd[j]; /* invert so that divisions can be carried out faster as multiplications */

        nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero A_ij, i=0...n-1 */
        for(i=0; i<nnz; ++i){
          idx=idxij->val[rcidxs[i]];
          phx=hx + idx*mnp; // set phx to point to hx_ij
          phxx=hxx + idx*mnp; // set phxx to point to hxx_ij
          pAB=jac + idx*ABsz; // set pAB to point to A_ij

          for(ii=0; ii<mnp; ++ii)
            pAB[ii*cnp+jj]=(phxx[ii]-phx[ii])*d;
        }
      }
    }
  }

  if(pnp){ // is structure varying?
    /* compute B_ij */
    for(jj=0; jj<pnp; ++jj){
      for(i=0; i<n; ++i){
        ppt=pb+i*pnp; // i-th point parameters
        /* determine d=max(SBA_DELTA_SCALE*|ppt[jj]|, SBA_MIN_DELTA), see HZ */
        d=(double)(SBA_DELTA_SCALE)*ppt[jj]; // force evaluation
        d=FABS(d);
        if(d<SBA_MIN_DELTA) d=SBA_MIN_DELTA;

        tmpd[i]=d;
        ppt[jj]+=d;
      }

      (*func)(p, idxij, fdjd->func_rcidxs, fdjd->func_rcsubs, hxx, adata);

      for(i=0; i<n; ++i){
        ppt=pb+i*pnp; // i-th point parameters
        ppt[jj]-=tmpd[i]; /* restore */
        d=1.0/tmpd[i]; /* invert so that divisions can be carried out faster as multiplications */

        nnz=sba_crsm_row_elmidxs(idxij, i, rcidxs, rcsubs); /* find nonzero B_ij, j=0...m-1 */
        for(j=0; j<nnz; ++j){
          idx=idxij->val[rcidxs[j]];
          phx=hx + idx*mnp; // set phx to point to hx_ij
          phxx=hxx + idx*mnp; // set phxx to point to hxx_ij
          pAB=jac + idx*ABsz + Asz; // set pAB to point to B_ij

          for(ii=0; ii<mnp; ++ii)
            pAB[ii*pnp+jj]=(phxx[ii]-phx[ii])*d;
        }
      }
    }
  }

  free(tmpd);
}

/* Compute e=x-y for two n-vectors x and y and return the squared L2 norm of e.
 * e can coincide with either x or y. 
 * Uses loop unrolling and blocking to reduce bookkeeping overhead & pipeline
 * stalls and increase instruction-level parallelism; see http://www.abarnett.demon.co.uk/tutorial.html
 */
static double nrmL2xmy(double *const e, const double *const x, const double *const y, const int n)
{
const int blocksize=8, bpwr=3; /* 8=2^3 */
register int i;
int j1, j2, j3, j4, j5, j6, j7;
int blockn;
register double sum0=0.0, sum1=0.0, sum2=0.0, sum3=0.0;

  /* n may not be divisible by blocksize, 
   * go as near as we can first, then tidy up.
   */
  blockn = (n>>bpwr)<<bpwr; /* (n / blocksize) * blocksize; */

  /* unroll the loop in blocks of `blocksize'; looping downwards gains some more speed */
  for(i=blockn-1; i>0; i-=blocksize){
            e[i ]=x[i ]-y[i ]; sum0+=e[i ]*e[i ];
    j1=i-1; e[j1]=x[j1]-y[j1]; sum1+=e[j1]*e[j1];
    j2=i-2; e[j2]=x[j2]-y[j2]; sum2+=e[j2]*e[j2];
    j3=i-3; e[j3]=x[j3]-y[j3]; sum3+=e[j3]*e[j3];
    j4=i-4; e[j4]=x[j4]-y[j4]; sum0+=e[j4]*e[j4];
    j5=i-5; e[j5]=x[j5]-y[j5]; sum1+=e[j5]*e[j5];
    j6=i-6; e[j6]=x[j6]-y[j6]; sum2+=e[j6]*e[j6];
    j7=i-7; e[j7]=x[j7]-y[j7]; sum3+=e[j7]*e[j7];
  }

  /*
   * There may be some left to do.
   * This could be done as a simple for() loop, 
   * but a switch is faster (and more interesting) 
   */

  i=blockn;
  if(i<n){ 
  /* Jump into the case at the place that will allow
   * us to finish off the appropriate number of items. 
   */
    switch(n - i){ 
      case 7 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
      case 6 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
      case 5 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
      case 4 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
      case 3 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
      case 2 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
      case 1 : e[i]=x[i]-y[i]; sum0+=e[i]*e[i]; ++i;
    }
  }

  return sum0+sum1+sum2+sum3;
}

/* Compute e=W*(x-y) for two n-vectors x and y and return the squared L2 norm of e.
 * This norm equals the squared C norm of x-y with C=W^T*W, W being block diagonal
 * matrix with nvis mnp*mnp blocks: e^T*e=(x-y)^T*W^T*W*(x-y)=||x-y||_C.
 * Note that n=nvis*mnp; e can coincide with either x or y.
 *
 * Similarly to nrmL2xmy() above, uses loop unrolling and blocking
 */
static double nrmCxmy(double *const e, const double *const x, const double *const y,
                      const double *const W, const int mnp, const int nvis)
{
const int n=nvis*mnp;
const int blocksize=8, bpwr=3; /* 8=2^3 */
register int i, ii, k;
int j1, j2, j3, j4, j5, j6, j7;
int blockn, mnpsq;
register double norm, sum;
register const double *Wptr, *auxptr;
register double *eptr;

  /* n may not be divisible by blocksize, 
   * go as near as we can first, then tidy up.
   */
  blockn = (n>>bpwr)<<bpwr; /* (n / blocksize) * blocksize; */

  /* unroll the loop in blocks of `blocksize'; looping downwards gains some more speed */
  for(i=blockn-1; i>0; i-=blocksize){
            e[i ]=x[i ]-y[i ];
    j1=i-1; e[j1]=x[j1]-y[j1];
    j2=i-2; e[j2]=x[j2]-y[j2];
    j3=i-3; e[j3]=x[j3]-y[j3];
    j4=i-4; e[j4]=x[j4]-y[j4];
    j5=i-5; e[j5]=x[j5]-y[j5];
    j6=i-6; e[j6]=x[j6]-y[j6];
    j7=i-7; e[j7]=x[j7]-y[j7];
  }

  /*
   * There may be some left to do.
   * This could be done as a simple for() loop, 
   * but a switch is faster (and more interesting) 
   */

  i=blockn;
  if(i<n){ 
  /* Jump into the case at the place that will allow
   * us to finish off the appropriate number of items. 
   */
    switch(n - i){ 
      case 7 : e[i]=x[i]-y[i]; ++i;
      case 6 : e[i]=x[i]-y[i]; ++i;
      case 5 : e[i]=x[i]-y[i]; ++i;
      case 4 : e[i]=x[i]-y[i]; ++i;
      case 3 : e[i]=x[i]-y[i]; ++i;
      case 2 : e[i]=x[i]-y[i]; ++i;
      case 1 : e[i]=x[i]-y[i]; ++i;
    }
  }

  /* compute w_x_ij e_ij in e and its L2 norm.
   * Since w_x_ij is upper triangular, the products can be safely saved
   * directly in e_ij, without the need for intermediate storage
   */
  mnpsq=mnp*mnp;
  /* Wptr, eptr point to w_x_ij, e_ij below */
  for(i=0, Wptr=W, eptr=e, norm=0.0; i<nvis; ++i, Wptr+=mnpsq, eptr+=mnp){
    for(ii=0, auxptr=Wptr; ii<mnp; ++ii, auxptr+=mnp){ /* auxptr=Wptr+ii*mnp */
      for(k=ii, sum=0.0; k<mnp; ++k) // k>=ii since w_x_ij is upper triangular
        sum+=auxptr[k]*eptr[k]; //Wptr[ii*mnp+k]*eptr[k];
      eptr[ii]=sum;
      norm+=sum*sum;
    }
  }

  return norm;
}

/* auxiliary routine for clearing an array of doubles */
inline static void _dblzero(register double *arr, register int count)
{
	while(--count>=0)
		*arr++=0.0;
}

/* auxiliary routine for computing the mean reprojection error; used for debugging */
static double sba_mean_repr_error(int n, int mnp, double *x, double *hx, struct sba_crsm *idxij, int *rcidxs, int *rcsubs)
{
	register int i, j;
	int nnz, nprojs;
	double *ptr1, *ptr2;
	double err;

	for(i=nprojs=0, err=0.0; i<n; ++i){
		nnz=sba_crsm_row_elmidxs(idxij, i, rcidxs, rcsubs); /* find nonzero x_ij, j=0...m-1 */
		nprojs+=nnz;
		for(j=0; j<nnz; ++j){ /* point i projecting on camera rcsubs[j] */
			ptr1=x + idxij->val[rcidxs[j]]*mnp;
			ptr2=hx + idxij->val[rcidxs[j]]*mnp;

			err+=sqrt((ptr1[0]-ptr2[0])*(ptr1[0]-ptr2[0]) + (ptr1[1]-ptr2[1])*(ptr1[1]-ptr2[1]));
		}
	}

	return err/((double)(nprojs));
}

/* search for & print image projection components that are infinite; useful for identifying errors */
static void sba_print_inf(FILE *fp, double *hx, int nimgs, int mnp, struct sba_crsm *idxij, int *rcidxs, int *rcsubs)
{
	register int i, j, k;
	int nnz;
	double *phxij;

	for(j=0; j<nimgs; ++j){
		nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */
		for(i=0; i<nnz; ++i){
			phxij=hx + idxij->val[rcidxs[i]]*mnp;
			for(k=0; k<mnp; ++k)
				if(!SBA_FINITE(phxij[k]))
					fprintf(fp, "SBA: component %d of the estimated projection of point %d on camera %d is invalid!\n", k, rcsubs[i], j);
		}
	}
}

inline void calcImgProj(double *calibParam, double t[3], double angle[3], double objpt[3], double imgpt[2], void *adata)
{
	double uvw[3];
	double Rmis[9], Rimu[9], R[9];
	struct imgCalib_globs *gl;

	gl=(struct imgCalib_globs *)adata;

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	
	assert(matrixService.get());

	switch(gl->calibType)
	{
	case CamGeo_rph:
		matrixService->RotateMatrix_fwk(calibParam[0], calibParam[1], calibParam[2], Rmis);
		
		uvw[0]=gl->calibParam->lever_arm[0];
		uvw[1]=gl->calibParam->lever_arm[1];
		uvw[2]=gl->calibParam->lever_arm[2];
		break;
	case CamGeo_rphxyz:
		matrixService->RotateMatrix_fwk(calibParam[0], calibParam[1], calibParam[2], Rmis);

		uvw[0]=calibParam[3];
		uvw[1]=calibParam[4];
		uvw[2]=calibParam[5];
		break;
	}
	
	matrixService->RotateMatrix_fwk(angle[0], angle[1], angle[2], Rimu);

	matrix_product(3, 3, 3, 3, Rimu, Rmis, R);

	double a1,a2,a3,b1,b2,b3,c1,c2,c3;

	a1 = R[0];	a2 = R[1];	a3 = R[2];
	b1 = R[3];	b2 = R[4];	b3 = R[5];
	c1 = R[6];	c2 = R[7];	c3 = R[8];

	double vec1[3], vec2[3];

	matrix_product(3, 3, 3, 1, Rimu, uvw, vec1);

	vec2[0]=objpt[0]-t[0]+vec1[0];
	vec2[1]=objpt[1]-t[1]+vec1[1];
	vec2[2]=objpt[2]-t[2]+vec1[2];

	double _X;
	double _Y;
	double _Z;

	double xp, yp;
	//	double x, y, deltax, deltay;
	double x0, y0, f, k1, k2, k3, p1, p2, _b1, _b2;
	double dx, dy, dr;
	double r2;

	x0 = gl->camInfo->m_x0;
	y0 = gl->camInfo->m_y0;
	f = gl->camInfo->m_f;
// 	k1 = camInfo.k1;
// 	k2 = camInfo.k2;
// 	k3 = camInfo.k3;
// 	p1 = camInfo.p1;
// 	p2 = camInfo.p2;
// 	_b1 = camInfo._b1;
// 	_b2 = camInfo._b2;

	_X = a1 * vec2[0] + b1 * vec2[1] + c1 * vec2[2];
	_Y = a2 * vec2[0] + b2 * vec2[1] + c2 * vec2[2];
	_Z = a3 * vec2[0] + b3 * vec2[1] + c3 * vec2[2];

	xp = - f * _X / _Z;
	yp = - f * _Y / _Z;

// 	r2 = xp*xp + yp*yp;
// 
// 	dr = ( k1+ ( k2 + k3*r2)*r2 )*r2;
// 	dx = xp*dr;
// 	dy = yp*dr;
// 
// 	dx += p1*( r2 + 2*xp*xp ) + 2*p2*xp*yp;
// 	dy += 2*p1*xp*yp + p2*( r2 + 2*yp*yp );
// 
// 	dx=_b1 * xp + _b2 * yp;
// 
// 	imgpt[0] = xp + x0 + dx;	//校正前的像平面坐标
// 	imgpt[1] = yp + y0 + dy;

	imgpt[0] = xp;
	imgpt[1] = yp;
}

// void jac_ImgProjCalib(double *p, double *jac, int m, int n, void *data)
// {
// 	register int i, j;
// 	double tmp;
// 	register double d;
// 	double delta=1E-06;
// 	double *hx, *hxx;
// 	struct imgCalib_globs *dptr;
// //	double nVP;
// 	//	double *pLut;
// 
// 	dptr=(struct imgCalib_globs *)data;
// //	nVP=dptr->nVP;
// 	//	pLut=dptr->pLut;
// 	hx=dptr->hx;
// 	hxx=dptr->hxx;
// 
// 	cal_VPObs(p, hx, m, n, data);
// 
// 	for(j=0; j<m; ++j)
// 	{
// 		/* determine d=max(1E-04*|p[j]|, delta), see HZ */
// 		d=(1E-04)*p[j]; // force evaluation
// 		d=fabs(d);
// 		if(d<delta)
// 			d=delta;
// 
// 		tmp=p[j];
// 		p[j]+=d;
// 
// 		cal_VPObs(p, hxx, m, n, data);
// 
// 		p[j]=tmp; /* restore */
// 
// 		d=(1.0)/d; /* invert so that divisions can be carried out faster as multiplications */
// 		for(i=0; i<n; ++i)
// 		{
// 			jac[i*m+j]=(hxx[i]-hx[i])*d;
// 		}
// 	}
// 
// }

// inline void calcImgProjJacKRTS(camera_param camInfo, double t[3], double qr0[3], double imgpt[2],
// 						  double objpt[3], double jacmRT[2][6], double jacmS[2][3])
// {
// 	double phi;
// 	double omega;
// 	double kappa;
// 	double a1;
// 	double a2;
// 	double a3;
// 	double b1;
// 	double b2;
// 	double b3;
// 	double c1;
// 	double c2;
// 	double c3;
// 
// 	double X;
// 	double Y;
// 	double Z;
// 	double Xs;
// 	double Ys;
// 	double Zs;
// 	double _X;
// 	double _Y;
// 	double _Z;
// 
// 	double x, y;
// 	double x0, y0, f;
// 
// 	x0=camInfo.x0;
// 	y0=camInfo.y0;
// 	f=camInfo.f;
// 	
// 	phi = qr0[0];
// 	omega = qr0[1];
// 	kappa = qr0[2];
// 
// 	X = objpt[0];
// 	Y = objpt[1];
// 	Z = objpt[2];
// 	Xs = t[0];
// 	Ys = t[1];
// 	Zs = t[2];
// 
// 	a1 = cos(phi)*cos(kappa)-sin(phi)*sin(omega)*sin(kappa);
// 	a2 = -cos(phi)*sin(kappa)-sin(phi)*sin(omega)*cos(kappa);
// 	a3 = -sin(phi)*cos(omega);
// 	b1 = cos(omega)*sin(kappa);
// 	b2 = cos(omega)*cos(kappa);
// 	b3 = -sin(omega);
// 	c1 = sin(phi)*cos(kappa)+cos(phi)*sin(omega)*sin(kappa);
// 	c2 = -sin(phi)*sin(kappa)+cos(phi)*sin(omega)*cos(kappa);
// 	c3 = cos(phi)*cos(omega); 
// 
// 	_X = a1 * (X - Xs) + b1 * (Y - Ys) + c1 * (Z - Zs);
// 	_Y = a2 * (X - Xs) + b2 * (Y - Ys) + c2 * (Z - Zs);
// 	_Z = a3 * (X - Xs) + b3 * (Y - Ys) + c3 * (Z - Zs);
// 
// 	/*x = - f * _X / _Z;
// 	y = - f * _Y / _Z;*/
// 
// 	x = imgpt[0] - x0;
// 	y = imgpt[1] - y0;
// 
// 	jacmRT[0][0] = 1 / _Z * (a1 * f + a3 * x);
// 	jacmRT[0][1] = 1 / _Z * (b1 * f + b3 * x);
// 	jacmRT[0][2] = 1 / _Z * (c1 * f + c3 * x);
// 
// 	jacmRT[0][3] = y * sin(omega) - (x / f * (x * cos(kappa) - y * sin(kappa)) + f * cos(kappa)) * cos(omega);
// 	jacmRT[0][4] = - f * sin(kappa) - x / f * (x * sin(kappa) + y * cos(kappa));
// 	jacmRT[0][5] = y;
// 
// 	jacmRT[1][0] = 1 / _Z * (a2 * f + a3 * y);
// 	jacmRT[1][1] = 1 / _Z * (b2 * f + b3 * y);
// 	jacmRT[1][2] = 1 / _Z * (c2 * f + c3 * y);
// 
// 	jacmRT[1][3] = - x * sin(omega) - (y / f * (x * cos(kappa) - y * sin(kappa)) - f * sin(kappa)) * cos(omega);
// 	jacmRT[1][4] = - f * cos(kappa) - y / f * (x * sin(kappa) + y * cos(kappa));
// 	jacmRT[1][5] = - x;
// 
// 	jacmS[0][0] = - jacmRT[0][0];
// 	jacmS[0][1] = - jacmRT[0][1];
// 	jacmS[0][2] = - jacmRT[0][2];
// 
// 	jacmS[1][0] = - jacmRT[1][0];
// 	jacmS[1][1] = - jacmRT[1][1];
// 	jacmS[1][2] = - jacmRT[1][2];
// }

//多片前方交会计算物方坐标, 像点坐标的改正不在此函数中进行
//AX=L	最小二乘解
//				|a1f + a3x,   b1f + b3x,  c1f + c3x|
//			A=	|								   |				A=2*3    X=[X, Y, Z]^T
//				|a2f + a3y,   b2f + b3y,  c2f + c3y|
//
//              |(a1f+a3x)Xs + (b1f+b3x)Ys + (c1f+c3x)Zs |
//         L =  |									     |		L=2*1	
//              |(a2f+a3y)Xs + (b2f+b3y)Ys + (c2f+c3y)Zs |
static void _img_intersection(double *pX, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *pObsV, double *p3D, void *adata)
{
	register int i, j;
	int cnp, pnp, mnp;
	double *exor, *pb, *pqr, *pt, *ppt/*, *pmeas*/;
	double *ptr1, *ptr2, *ptr3;
	int n;
	int m, nnz, max_nnz;
	struct imgCalib_globs *gl;
	//	double x0, y0,f;
	double f;
	double	*A=0, *L=0;
	double R[9];
	double x, y;
	double a1, a2, a3, b1, b2, b3, c1, c2, c3;
	double Xs, Ys, Zs;
	//	double u, v, w;
	//	double phi, omega, kappa;

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	gl=(struct imgCalib_globs *)adata;
	cnp=gl->cnp; 
	pnp=gl->pnp; 
	mnp=gl->mnp;
	// 	x0=gl->camInfo.x0;
	// 	y0=gl->camInfo.y0;
	//	f=gl->camInfo.f;
	f=gl->camInfo->m_f;

	n=idxij->nr;		//3D点数
	m=idxij->nc;	//像片数

	exor=pX;		//像片
	pb=pX+m*cnp;	//3D点

	//统计物方点中在多少张相片上可见片，记录最大值
	for(i=0, max_nnz=0; i<n; i++)
	{
		nnz=sba_crsm_row_elmidxs(idxij, i, rcidxs, rcsubs);
		if(max_nnz<nnz)
			max_nnz=nnz;
	}

	A=(double*)malloc(2*3*max_nnz*sizeof(double));
	L=(double*)malloc(2*max_nnz*sizeof(double));

	for(i=0; i<n; ++i)
	{
		ppt=p3D+i*pnp;	//得到的物方点
		pb=pX+m*cnp+i*pnp;	
		nnz=sba_crsm_row_elmidxs(idxij, i, rcidxs, rcsubs); 
		if(nnz<2)
		{//无法进行前方交互
			ppt[0] = pb[0];
			ppt[1] = pb[1];
			ppt[2] = pb[2];
			continue;
		}

		for(j=0; j<nnz; ++j)
		{
			pt=exor +  idxij->colidx[rcidxs[j]]*cnp;	//线元素初始值

			pqr=pt+3; 
			ptr3=pObsV + idxij->val[rcidxs[j]]*mnp;	//像点坐标
			x=ptr3[0];
			y=ptr3[1];

			ptr1=A+j*2*3;
			ptr2=L+j*2;

			matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], R);
			a1=R[0];		a2=R[1];		a3=R[2];
			b1=R[3];		b2=R[4];		b3=R[5];
			c1=R[6];		c2=R[7];		c3=R[8];
			Xs=pt[0];	Ys=pt[1];	Zs=pt[2];

			ptr1[0]=a1*f+a3*x;
			ptr1[1]=b1*f+b3*x;
			ptr1[2]=c1*f+c3*x;
			ptr1[3]=a2*f+a3*y;
			ptr1[4]=b2*f+b3*y;
			ptr1[5]=c2*f+c3*y;

			ptr2[0]=(a1*f+a3*x)*Xs+(b1*f+b3*x)*Ys+(c1*f+c3*x)*Zs;
			ptr2[1]=(a2*f+a3*y)*Xs+(b2*f+b3*y)*Ys+(c2*f+c3*y)*Zs;

		}
		dgelsy_driver(A, L, ppt, 2*nnz, 3, 1);

	}

	if(A)	free(A);
	if(L)	free(L);
}

//计算像点坐标
//pX: 全部未知数
//projs: 像点坐标
static void _img_projs(double *pX, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *projs, void *adata)
{
	register int i, j;
	int cnp, pnp, mnp;
	double *exor, *pb, *pqr, *pt, *ppt, *pV;
// 	int n;
// 	int m, nnz;
	struct imgCalib_globs *gl;
	double *calibParam = pX;

	gl=(struct imgCalib_globs *)adata;
	cnp=gl->cnp; 
	pnp=gl->pnp; 
	mnp=gl->mnp;

// 	x0=gl->camInfo.x0;
// 	y0=gl->camInfo.y0;
// 	f=gl->camInfo.f;

// 	n=idxij->nr;
// 	m=idxij->nc;


	exor=gl->pImgExor;				//像片

	pb=pX+gl->nVFrame*cnp;	//3D点

	for(j=0; j<gl->nimg; ++j)
	{
		/* j-th camera exorrameters */
		pt=exor+j*6;	//线元素
		pqr=pt+3;		//角元素

		int nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

		for(i=0; i<nnz; ++i)
		{
			ppt=pb + rcsubs[i]*pnp;	//物方坐标
			pV=projs + idxij->val[rcidxs[i]]*mnp; // set pmeas to point to hx_ij

			//calcMyProjFullR(pr0, pt, ppt, pmeas); // evaluate Q in pmeas
			calcImgProj(calibParam, pt, pqr, ppt, pV, adata); //计算像点坐标近似值 
		}
	}
}

//计算误差方程的系数jac
//对像片的R，T，以及物方点S求导数
//pX: 未知数。 安装角，偏心分量，物方点
static void _img_projsRTS_jac(double *pX, /*double *pV,*/ struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata)
{
// 	int cnp, pnp, mnp;
// 	double *pa, *pb, *pqr, *pt, *ppt, *pA, *pB, impt[2];
 	int Asz, Bsz, ABCsz;
//	register int i, j;
	int cnp, pnp, mnp;
	double *exor, *pb, *pqr, *pt, *ppt, *pV;
	double *uvw;
	double tempJac[12];

	double Rimu[9];
	double L1, L2, L3;
	double t1, t2, t3;
	double _X, _Y, _Z, _Z2;
	double _XPartial[9], _YPartial[9], _ZPartial[9];
	double L1_Partial[6], L2_Partial[6], L3_Partial[6]; 
	double da1[3], da2[3], da3[3], db1[3], db2[3], db3[3], dc1[3], dc2[3], dc3[3]; //角元素的偏导数


	//计算安装角偏导数
	double cosf, sinf, cosw, sinw, cosk, sink;
	cosf = cos(pX[0]); sinf = sin(pX[0]);
	cosw = cos(pX[1]); sinw = sin(pX[1]);
	cosk = cos(pX[2]); sink = sin(pX[2]);

	//a1= cosf*cosk - sinf*sinw*sink;
	da1[0] = -sinf*cosk-cosf*sinw*sink;
	da1[1] = -sinf*cosw*sink;
	da1[2] = -cosf*sink - sinf*sinw*cosk;

	//a2= -cosf*sink - sinf*sinw*cosk;
	da2[0] = sinf*sink - cosf*sinw*cosk;
	da2[1] = -sinf*cosw*cosk;
	da2[2] = -cosf*cosk + sinf*sinw*sink;

	//a3= -sinf*cosw;
	da3[0] = -cosf*cosw;
	da3[1] = sinf*sinw;
	da3[2] = 0;
	
	//b1= cosw*sink;
	db1[0] = 0;
	db1[1] = -sinw*sink;
	db1[2] = cosw*cosk;

	//b2= cosw*cosk;
	db2[0] = 0;
	db2[1] = -sinw*cosk;
	db2[2] = cosw*sink;

	//b3= -sinw;
	db3[0] = 0;
	db3[1] = -cosw;
	db3[2] = 0;

	//c1= sinf*cosk + cosf*sinw*sink;
	dc1[0] = cosf*cosk-sinf*sinw*sink;
	dc1[1] = cosf*cosw*sink;
	dc1[2] = -sinf*sink+cosf*sinw*cosk;

	//c2= -sinf*sink + cosf*sinw*cosk;
	dc2[0] = -cosf*sink-sinf*sinw*cosk;
	dc2[1] = cosf*cosw*cosk;
	dc2[2] = -sinf*cosk-cosf*sinw*sink;

	//c3= cosf*cosw;
	dc3[0] = -sinf*cosw;
	dc3[1] = -cosf*sinw;
	dc3[2] = 0;
/////////////////////////////////////////////////////////////////////
	
	struct imgCalib_globs *gl;
	gl=(struct imgCalib_globs *)adata;
	
	register int i, j, k;
	int m, n;
	double tmp;
	register double d;
	double delta=1E-06;
	double *hx, *hxx;
	double Rmis[9];
	double f;

	f=gl->camInfo->m_f;
	exor=gl->pImgExor;				//像片

	m = gl->cnp*gl->nVFrame+gl->pnp*gl->ntie;		//未知数个数
	n = gl->nproj*gl->mnp;							//观测值个数

	pb = pX+gl->cnp*gl->nVFrame; //物方坐标

	if(gl->cnp==3)
	{
		uvw = gl->calibParam->lever_arm;
	}
	else if(gl->cnp==6)
	{
		uvw = pX+3;
	}

	Asz=gl->mnp*gl->cnp;		//像片偏导数个数
	Bsz=gl->mnp*gl->pnp;		//点偏导数个数
	ABCsz=Asz+Bsz;

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	matrixService->RotateMatrix_fwk(pX[0], pX[1], pX[2], Rmis);

	//根据未知数的排列，Jac为:	a14, a15, a16, a11, a12, a13
	//							a24, a25, a26, a21, a22, a23
	for(j=0; j<gl->nimg; ++j)
	{
		/* j-th camera exorrameters */
		pt=exor+j*6;	//线元素
		pqr=pt+3;		//角元素

		matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);

		//换元后的偏导数  x = -fX/Z
		L1_Partial[0] = Rimu[0]*Rimu[0]+Rimu[3]*Rimu[1]+Rimu[6]*Rimu[2];
		L1_Partial[1] = Rimu[0]*Rimu[3]+Rimu[3]*Rimu[4]+Rimu[6]*Rimu[5];
		L1_Partial[2] = Rimu[0]*Rimu[6]+Rimu[3]*Rimu[7]+Rimu[6]*Rimu[8];
		//对X,Y,Z的偏导
		L1_Partial[3] = Rimu[0];
		L1_Partial[4] = Rimu[3];
		L1_Partial[5] = Rimu[6];

		L2_Partial[0] = Rimu[1]*Rimu[0]+Rimu[4]*Rimu[1]+Rimu[7]*Rimu[2];
		L2_Partial[1] = Rimu[1]*Rimu[3]+Rimu[4]*Rimu[4]+Rimu[7]*Rimu[5];
		L2_Partial[2] = Rimu[1]*Rimu[6]+Rimu[4]*Rimu[7]+Rimu[7]*Rimu[8];

		L2_Partial[3] = Rimu[1];
		L2_Partial[4] = Rimu[4];
		L2_Partial[5] = Rimu[7];

		L3_Partial[0] = Rimu[2]*Rimu[0]+Rimu[5]*Rimu[1]+Rimu[8]*Rimu[2];
		L3_Partial[1] = Rimu[2]*Rimu[3]+Rimu[5]*Rimu[4]+Rimu[8]*Rimu[5];
		L3_Partial[2] = Rimu[2]*Rimu[6]+Rimu[5]*Rimu[7]+Rimu[8]*Rimu[8];

		L3_Partial[3] = Rimu[2];
		L3_Partial[4] = Rimu[5];
		L3_Partial[5] = Rimu[8];

		//X_, Y_, Z_ 对 u, v, w
		_XPartial[0] = Rmis[0]*L1_Partial[0]+Rmis[3]*L2_Partial[0]+Rmis[6]*L3_Partial[0];
		_XPartial[1] = Rmis[0]*L1_Partial[1]+Rmis[3]*L2_Partial[1]+Rmis[6]*L3_Partial[1];
		_XPartial[2] = Rmis[0]*L1_Partial[2]+Rmis[3]*L2_Partial[2]+Rmis[6]*L3_Partial[2];

		//对X, Y, Z
		_XPartial[6] = Rmis[0]*L1_Partial[3]+Rmis[3]*L2_Partial[3]+Rmis[6]*L3_Partial[3];
		_XPartial[7] = Rmis[0]*L1_Partial[4]+Rmis[3]*L2_Partial[4]+Rmis[6]*L3_Partial[4];
		_XPartial[8] = Rmis[0]*L1_Partial[5]+Rmis[3]*L2_Partial[5]+Rmis[6]*L3_Partial[5];

		_YPartial[0] = Rmis[1]*L1_Partial[0]+Rmis[4]*L2_Partial[0]+Rmis[7]*L3_Partial[0];
		_YPartial[1] = Rmis[1]*L1_Partial[1]+Rmis[4]*L2_Partial[1]+Rmis[7]*L3_Partial[1];
		_YPartial[2] = Rmis[1]*L1_Partial[2]+Rmis[4]*L2_Partial[2]+Rmis[7]*L3_Partial[2];

		_YPartial[6] = Rmis[1]*L1_Partial[3]+Rmis[4]*L2_Partial[3]+Rmis[7]*L3_Partial[3];
		_YPartial[7] = Rmis[1]*L1_Partial[4]+Rmis[4]*L2_Partial[4]+Rmis[7]*L3_Partial[4];
		_YPartial[8] = Rmis[1]*L1_Partial[5]+Rmis[4]*L2_Partial[5]+Rmis[7]*L3_Partial[5];

		_ZPartial[0] = Rmis[2]*L1_Partial[0]+Rmis[5]*L2_Partial[0]+Rmis[8]*L3_Partial[0];
		_ZPartial[1] = Rmis[2]*L1_Partial[1]+Rmis[5]*L2_Partial[1]+Rmis[8]*L3_Partial[1];
		_ZPartial[2] = Rmis[2]*L1_Partial[2]+Rmis[5]*L2_Partial[2]+Rmis[8]*L3_Partial[2];

		_ZPartial[6] = Rmis[2]*L1_Partial[3]+Rmis[5]*L2_Partial[3]+Rmis[8]*L3_Partial[3];
		_ZPartial[7] = Rmis[2]*L1_Partial[4]+Rmis[5]*L2_Partial[4]+Rmis[8]*L3_Partial[4];
		_ZPartial[8] = Rmis[2]*L1_Partial[5]+Rmis[5]*L2_Partial[5]+Rmis[8]*L3_Partial[5];

		//一张相片上的像点数
		int nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

		for(i=0; i<nnz; ++i)
		{
			ppt=pb + rcsubs[i]*gl->pnp;	//物方坐标
			double *Jacimg=jac + idxij->val[rcidxs[i]]*ABCsz;
			double *Jacpts=Jacimg+Asz;

			//计算t1,t2,t3, l1,l2,l3, _X, _Y, _Z, _Z2
			t1=ppt[0]-pt[0]+Rimu[0]*uvw[0]+Rimu[3]*uvw[1]+Rimu[6]*uvw[2];
			t2=ppt[1]-pt[1]+Rimu[1]*uvw[0]+Rimu[4]*uvw[1]+Rimu[7]*uvw[2];
			t3=ppt[2]-pt[2]+Rimu[2]*uvw[0]+Rimu[5]*uvw[1]+Rimu[8]*uvw[2];

			L1=Rimu[0]*t1+Rimu[3]*t2+Rimu[6]*t3;
			L2=Rimu[1]*t1+Rimu[4]*t2+Rimu[7]*t3;
			L3=Rimu[2]*t1+Rimu[5]*t2+Rimu[8]*t3;

			_X=Rmis[0]*L1+Rmis[3]*L2+Rmis[6]*L3;
			_Y=Rmis[1]*L1+Rmis[4]*L2+Rmis[7]*L3;
			_Z=Rmis[2]*L1+Rmis[5]*L2+Rmis[8]*L3;
			_Z2=_Z*_Z;

			//角元素的偏导数
			_XPartial[3]=L1*da1[0]+L2*db1[0]+L3*dc1[0];
			_XPartial[4]=L1*da1[1]+L2*db1[1]+L3*dc1[1];
			_XPartial[5]=L1*da1[2]+L2*db1[2]+L3*dc1[2];

			_YPartial[3]=L1*da2[0]+L2*db2[0]+L3*dc2[0];
			_YPartial[4]=L1*da2[1]+L2*db2[1]+L3*dc2[1];
			_YPartial[5]=L1*da2[2]+L2*db2[2]+L3*dc2[2];

			_ZPartial[3]=L1*da3[0]+L2*db3[0]+L3*dc3[0];
			_ZPartial[4]=L1*da3[1]+L2*db3[1]+L3*dc3[1];
			_ZPartial[5]=L1*da3[2]+L2*db3[2]+L3*dc3[2];

			//a14,a15,a16,a11,a12,a13
			tempJac[0]= -f*(_XPartial[3]*_Z-_X*_ZPartial[3])/_Z2;
			tempJac[1]= -f*(_XPartial[4]*_Z-_X*_ZPartial[4])/_Z2;
			tempJac[2]= -f*(_XPartial[5]*_Z-_X*_ZPartial[5])/_Z2;
			tempJac[3]= -f*(_XPartial[0]*_Z-_X*_ZPartial[0])/_Z2;
			tempJac[4]= -f*(_XPartial[1]*_Z-_X*_ZPartial[1])/_Z2;
			tempJac[5]= -f*(_XPartial[2]*_Z-_X*_ZPartial[2])/_Z2;

			tempJac[6]= -f*(_YPartial[3]*_Z-_Y*_ZPartial[3])/_Z2;
			tempJac[7]= -f*(_YPartial[4]*_Z-_Y*_ZPartial[4])/_Z2;
			tempJac[8]= -f*(_YPartial[5]*_Z-_Y*_ZPartial[5])/_Z2;
			tempJac[9]= -f*(_YPartial[0]*_Z-_Y*_ZPartial[0])/_Z2;
			tempJac[10]= -f*(_YPartial[1]*_Z-_Y*_ZPartial[1])/_Z2;
			tempJac[11]= -f*(_YPartial[2]*_Z-_Y*_ZPartial[2])/_Z2;

			for(k=0; k<gl->mnp; k++)
			{
				memcpy(Jacimg+k*gl->cnp, tempJac+k*6, sizeof(double)*gl->cnp);
			}

			//物方点的偏导数
			Jacpts[0]= -f*(_XPartial[6]*_Z-_X*_ZPartial[6])/_Z2;
			Jacpts[1]= -f*(_XPartial[7]*_Z-_X*_ZPartial[7])/_Z2;
			Jacpts[2]= -f*(_XPartial[8]*_Z-_X*_ZPartial[8])/_Z2;

			Jacpts[3]= -f*(_YPartial[6]*_Z-_Y*_ZPartial[6])/_Z2;
			Jacpts[4]= -f*(_YPartial[7]*_Z-_Y*_ZPartial[7])/_Z2;
			Jacpts[5]= -f*(_YPartial[8]*_Z-_Y*_ZPartial[8])/_Z2;
		}
	}
}

typedef int (*PLS)(double *A, double *B, double *x, int m, int iscolmaj);

int sba_motstr_levmar_x_imgCalib(
    const int n,   /* number of points */
    const int ncon,/* number of points (starting from the 1st) whose parameters should not be modified.
                   * All B_ij (see below) with i<ncon are assumed to be zero
                   */
    const int m,   /* number of images */
    const int mcon,/* number of images (starting from the 1st) whose parameters should not be modified.
					          * All A_ij (see below) with j<mcon are assumed to be zero
					          */
    char *vmask,  /* visibility mask: vmask[i, j]=1 if point i visible in image j, 0 otherwise. nxm */
    double *p,    /* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
                   * aj are the image j parameters, bi are the i-th point parameters,
                   * size m*cnp + n*pnp
                   */
    const int cnp,/* number of parameters for ONE camera; e.g. 6 for Euclidean cameras */
    const int pnp,/* number of parameters for ONE point; e.g. 3 for Euclidean points */
    double *x,    /* measurements vector: (x_11^T, .. x_1m^T, ..., x_n1^T, .. x_nm^T)^T where
                   * x_ij is the projection of the i-th point on the j-th image.
                   * NOTE: some of the x_ij might be missing, if point i is not visible in image j;
                   * see vmask[i, j], max. size n*m*mnp
                   */
    double *covx, /* measurements covariance matrices: (Sigma_x_11, .. Sigma_x_1m, ..., Sigma_x_n1, .. Sigma_x_nm),
                   * where Sigma_x_ij is the mnp x mnp covariance of x_ij stored row-by-row. Set to NULL if no
                   * covariance estimates are available (identity matrices are implicitly used in this case).
                   * NOTE: a certain Sigma_x_ij is missing if the corresponding x_ij is also missing;
                   * see vmask[i, j], max. size n*m*mnp*mnp
                   */
    const int mnp,/* number of parameters for EACH measurement; usually 2 */
    void (*func)(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata),
                                              /* functional relation describing measurements. Given a parameter vector p,
                                               * computes a prediction of the measurements \hat{x}. p is (m*cnp + n*pnp)x1,
                                               * \hat{x} is (n*m*mnp)x1, maximum
                                               * rcidxs, rcsubs are max(m, n) x 1, allocated by the caller and can be used
                                               * as working memory
                                               */
    void (*fjac)(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata),
                                              /* function to evaluate the sparse jacobian dX/dp.
                                               * The Jacobian is returned in jac as
                                               * (dx_11/da_1, ..., dx_1m/da_m, ..., dx_n1/da_1, ..., dx_nm/da_m,
                                               *  dx_11/db_1, ..., dx_1m/db_1, ..., dx_n1/db_n, ..., dx_nm/db_n), or (using HZ's notation),
                                               * jac=(A_11, B_11, ..., A_1m, B_1m, ..., A_n1, B_n1, ..., A_nm, B_nm)
                                               * Notice that depending on idxij, some of the A_ij and B_ij might be missing.
                                               * Note also that A_ij and B_ij are mnp x cnp and mnp x pnp matrices resp. and they
                                               * should be stored in jac in row-major order.
                                               * rcidxs, rcsubs are max(m, n) x 1, allocated by the caller and can be used
                                               * as working memory
                                               *
                                               * If NULL, the jacobian is approximated by repetitive func calls and finite
                                               * differences. This is computationally inefficient and thus NOT recommended.
                                               */
    void *adata,       /* pointer to possibly additional data, passed uninterpreted to func, fjac */ 

    const int itmax,   /* I: maximum number of iterations. itmax==0 signals jacobian verification followed by immediate return */
    const int verbose, /* I: verbosity */
    const double opts[SBA_OPTSSZ],
	                     /* I: minim. options [\mu, \epsilon1, \epsilon2, \epsilon3, \epsilon4]. Respectively the scale factor for initial \mu,
                        * stopping thresholds for ||J^T e||_inf, ||dp||_2, ||e||_2 and (||e||_2-||e_new||_2)/||e||_2
                        */
    double info[SBA_INFOSZ]
	                     /* O: information regarding the minimization. Set to NULL if don't care
                        * info[0]=||e||_2 at initial p.
                        * info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
                        * info[5]= # iterations,
                        * info[6]=reason for terminating: 1 - stopped by small gradient J^T e
                        *                                 2 - stopped by small dp
                        *                                 3 - stopped by itmax
                        *                                 4 - stopped by small relative reduction in ||e||_2
                        *                                 5 - stopped by small ||e||_2
                        *                                 6 - too many attempts to increase damping. Restart with increased mu
                        *                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
                        * info[7]= # function evaluations
                        * info[8]= # jacobian evaluations
			                  * info[9]= # number of linear systems solved, i.e. number of attempts	for reducing error
                        */
)
{
register int i, j, ii, jj, k, l;
int nvis, nnz, retval;

/* The following are work arrays that are dynamically allocated by sba_motstr_levmar_x() */
double *jac;  /* work array for storing the jacobian, max. size n*m*(mnp*cnp + mnp*pnp) */
double *U;    /* work array for storing the U_j in the order U_1, ..., U_m, size m*cnp*cnp */
double *V;    /* work array for storing the *strictly upper triangles* of V_i in the order V_1, ..., V_n, size n*pnp*pnp.
               * V also stores the lower triangles of (V*_i)^-1 in the order (V*_1)^-1, ..., (V*_n)^-1.
               * Note that diagonal elements of V_1 are saved in diagUV
               */

double *e;    /* work array for storing the e_ij in the order e_11, ..., e_1m, ..., e_n1, ..., e_nm,
                 max. size n*m*mnp */
double *eab;  /* work array for storing the ea_j & eb_i in the order ea_1, .. ea_m eb_1, .. eb_n size m*cnp + n*pnp */

double *E;   /* work array for storing the e_j in the order e_1, .. e_m, size m*cnp */

/* Notice that the blocks W_ij, Y_ij are zero iff A_ij (equivalently B_ij) is zero. This means
 * that the matrix consisting of blocks W_ij is itself sparse, similarly to the
 * block matrix made up of the A_ij and B_ij (i.e. jac)
 */
double *W;    /* work array for storing the W_ij in the order W_11, ..., W_1m, ..., W_n1, ..., W_nm,
                 max. size n*m*cnp*pnp */
double *Yj;   /* work array for storing the Y_ij for a *fixed* j in the order Y_1j, Y_nj,
                 max. size n*cnp*pnp */
double *YWt;  /* work array for storing \sum_i Y_ij W_ik^T, size cnp*cnp */
double *S;    /* work array for storing the block array S_jk, size m*m*cnp*cnp */
double *dp;   /* work array for storing the parameter vector updates da_1, ..., da_m, db_1, ..., db_n, size m*cnp + n*pnp */
double *Wtda; /* work array for storing \sum_j W_ij^T da_j, size pnp */
double *wght= /* work array for storing the weights computed from the covariance inverses, max. size n*m*mnp*mnp */
            NULL;

/* Of the above arrays, jac, e, W, Yj, wght are sparse and
 * U, V, eab, E, S, dp are dense. Sparse arrays (except Yj) are indexed
 * through idxij (see below), that is with the same mechanism as the input 
 * measurements vector x
 */

double *pa, *pb, *ea, *eb, *dpa, *dpb; /* pointers into p, jac, eab and dp respectively */

/* submatrices sizes */
int Asz, Bsz, ABsz, Usz, Vsz,
    Wsz, Ysz, esz, easz, ebsz,
    YWtsz, Wtdasz, Sblsz, covsz;

int Sdim; /* S matrix actual dimension */

register double *ptr1, *ptr2, *ptr3, *ptr4, sum;
struct sba_crsm idxij; /* sparse matrix containing the location of x_ij in x. This is also
                        * the location of A_ij, B_ij in jac, etc.
                        * This matrix can be thought as a map from a sparse set of pairs (i, j) to a continuous
                        * index k and it is used to efficiently lookup the memory locations where the non-zero
                        * blocks of a sparse matrix/vector are stored
                        */
int maxCvis, /* max. of projections of a single point  across cameras, <=m */
    maxPvis, /* max. of projections in a single camera across points,  <=n */
    maxCPvis, /* max. of the above */
    *rcidxs,  /* work array for the indexes corresponding to the nonzero elements of a single row or
                 column in a sparse matrix, size max(n, m) */
    *rcsubs;  /* work array for the subscripts of nonzero elements in a single row or column of a
                 sparse matrix, size max(n, m) */

/* The following variables are needed by the LM algorithm */
register int itno;  /* iteration counter */
int issolved;
/* temporary work arrays that are dynamically allocated */
double *hx,         /* \hat{x}_i, max. size m*n*mnp */
       *diagUV,     /* diagonals of U_j, V_i, size m*cnp + n*pnp */
       *pdp;        /* p + dp, size m*cnp + n*pnp */

double *diagU, *diagV; /* pointers into diagUV */

register double mu,  /* damping constant */
                tmp; /* mainly used in matrix & vector multiplications */
double p_eL2, eab_inf, pdp_eL2; /* ||e(p)||_2, ||J^T e||_inf, ||e(p+dp)||_2 */
double p_L2, dp_L2=DBL_MAX, dF, dL;
double tau=FABS(opts[0]), eps1=FABS(opts[1]), eps2=FABS(opts[2]), eps2_sq=opts[2]*opts[2],
       eps3_sq=opts[3]*opts[3], eps4_sq=opts[4]*opts[4];
double init_p_eL2;
int nu=2, nu2, stop=0, nfev, njev=0, nlss=0;
int nobs, nvars;
const int mmcon=/*m-mcon*/1;
PLS linsolver=NULL;
int (*matinv)(double *A, int m)=NULL;

int nVFrame = 1;

struct imgCalib_globs *gl;
gl=(struct imgCalib_globs *)adata;
FILE *fp = gl->fplog;

struct fdj_data_x_ fdj_data;
void *jac_adata;

/* Initialization */
  mu=eab_inf=0.0; /* -Wall */

  /* block sizes */
  Asz=mnp * cnp; Bsz=mnp * pnp; ABsz=Asz + Bsz;
  Usz=cnp * cnp; Vsz=pnp * pnp;
  Wsz=cnp * pnp; Ysz=cnp * pnp;
  esz=mnp;
  easz=cnp; ebsz=pnp;
  YWtsz=cnp * cnp;
  Wtdasz=pnp;
  Sblsz=cnp * cnp;
  Sdim=mmcon * cnp;
  covsz=mnp * mnp;

  /* count total number of visible image points */
  for(i=nvis=0, jj=n*m; i<jj; ++i)
    nvis+=(vmask[i]!=0);

  nobs=nvis*mnp;	//观测值
  nvars=nVFrame*cnp + n*pnp;	//未知数
  if(nobs<nvars){
    fprintf(fp, "SBA: sba_motstr_levmar_x() cannot solve a problem with fewer measurements [%d] than unknowns [%d]\n", nobs, nvars);
    return SBA_ERROR;
  }

  /* allocate & fill up the idxij structure */
  sba_crsm_alloc(&idxij, n, m, nvis);
  for(i=k=0; i<n; ++i){
    idxij.rowptr[i]=k;
    ii=i*m;
    for(j=0; j<m; ++j)
      if(vmask[ii+j]){
        idxij.val[k]=k;
        idxij.colidx[k++]=j;
      }
  }
  idxij.rowptr[n]=nvis;

  /* find the maximum number (for all cameras) of visible image projections coming from a single 3D point */
  for(i=maxCvis=0; i<n; ++i)
    if((k=idxij.rowptr[i+1]-idxij.rowptr[i])>maxCvis) maxCvis=k;

  /* find the maximum number (for all points) of visible image projections in any single camera */
  for(j=maxPvis=0; j<m; ++j){
    for(i=ii=0; i<n; ++i)
      if(vmask[i*m+j]) ++ii;
    if(ii>maxPvis) maxPvis=ii;
  }
  maxPvis = n;	//每个连接点在等效相片上都可见

  maxCPvis=(maxCvis>=maxPvis)? maxCvis : maxPvis;

  maxCPvis=(maxCPvis>=n)? maxCPvis : n;	//每个连接点在等效相片上都可见

#if 0
  /* determine the density of blocks in matrix S */
  for(j=mcon, ii=0; j<m; ++j){
    ++ii; /* block Sjj is surely nonzero */
    for(k=j+1; k<m; ++k)
      if(sba_crsm_common_row(&idxij, j, k)) ii+=2; /* blocks Sjk & Skj are nonzero */
  }
  printf("\nS block density: %.5g\n", ((double)ii)/(mmcon*mmcon)); fflush(stdout);
#endif

  /* allocate work arrays */
  /* W is big enough to hold both jac & W. Note also the extra Wsz, see the initialization of jac below for explanation */
//  W=(double *)emalloc((nvis*((Wsz>=ABsz)? Wsz : ABsz) + Wsz)*sizeof(double));
  W=(double *)emalloc(n*Wsz*sizeof(double));
  jac=(double *)emalloc(nvis*ABsz*sizeof(double));
  U=(double *)emalloc(nVFrame*Usz*sizeof(double));
  V=(double *)emalloc(n*Vsz*sizeof(double));
  e=(double *)emalloc(nobs*sizeof(double));
  eab=(double *)emalloc(nvars*sizeof(double));
  E=(double *)emalloc(nVFrame*cnp*sizeof(double));
  Yj=(double *)emalloc(maxPvis*Ysz*sizeof(double));
  YWt=(double *)emalloc(YWtsz*sizeof(double));
  S=(double *)emalloc(nVFrame*nVFrame*Sblsz*sizeof(double));
  dp=(double *)emalloc(nvars*sizeof(double));
  Wtda=(double *)emalloc(pnp*sizeof(double));
  rcidxs=(int *)emalloc(maxCPvis*sizeof(int));
  rcsubs=(int *)emalloc(maxCPvis*sizeof(int));
#ifndef SBA_DESTROY_COVS
  if(covx!=NULL) wght=(double *)emalloc(nvis*covsz*sizeof(double));
#else
  if(covx!=NULL) wght=covx;
#endif /* SBA_DESTROY_COVS */


  hx=(double *)emalloc(nobs*sizeof(double));
  diagUV=(double *)emalloc(nvars*sizeof(double));
  pdp=(double *)emalloc(nvars*sizeof(double));

  /* to save resources, W and jac share the same memory: First, the jacobian
   * is computed in some working memory that is then overwritten during the
   * computation of W. To account for the case of W being larger than jac,
   * extra memory is reserved "before" jac.
   * Care must be taken, however, to ensure that storing a certain W_ij
   * does not overwrite the A_ij, B_ij used to compute it. To achieve
   * this is, note that if p1 and p2 respectively point to the first elements
   * of a certain W_ij and A_ij, B_ij pair, we should have p2-p1>=Wsz.
   * There are two cases:
   * a) Wsz>=ABsz: Then p1=W+k*Wsz and p2=jac+k*ABsz=W+Wsz+nvis*(Wsz-ABsz)+k*ABsz
   *    for some k (0<=k<nvis), thus p2-p1=(nvis-k)*(Wsz-ABsz)+Wsz. 
   *    The right side of the last equation is obviously > Wsz for all 0<=k<nvis
   *
   * b) Wsz<ABsz: Then p1=W+k*Wsz and p2=jac+k*ABsz=W+Wsz+k*ABsz and
   *    p2-p1=Wsz+k*(ABsz-Wsz), which is again > Wsz for all 0<=k<nvis
   *
   * In conclusion, if jac is initialized as below, the memory allocated to all
   * W_ij is guaranteed not to overlap with that allocated to their corresponding
   * A_ij, B_ij pairs
   */
//  jac=W + Wsz + ((Wsz>ABsz)? nvis*(Wsz-ABsz) : 0);

  /* set up auxiliary pointers */
  pa=p; pb=p+nVFrame*cnp;
  ea=eab; eb=eab+nVFrame*cnp;
  dpa=dp; dpb=dp+nVFrame*cnp;

  diagU=diagUV; diagV=diagUV + nVFrame*cnp;

  /* if no jacobian function is supplied, prepare to compute jacobian with finite difference */
  if(!fjac){
    fdj_data.func=func;
    fdj_data.cnp=cnp;
    fdj_data.pnp=pnp;
    fdj_data.mnp=mnp;
    fdj_data.hx=hx;
    fdj_data.hxx=(double *)emalloc(nobs*sizeof(double));
    fdj_data.func_rcidxs=(int *)emalloc(2*maxCPvis*sizeof(int));
    fdj_data.func_rcsubs=fdj_data.func_rcidxs+maxCPvis;
    fdj_data.adata=adata;

    fjac=sba_fdjac_x;
    jac_adata=(void *)&fdj_data;
  }
  else{
    fdj_data.hxx=NULL;
    jac_adata=adata;
  }

  if(itmax==0){ /* verify jacobian */
    sba_motstr_chkjac_x(func, fjac, p, &idxij, rcidxs, rcsubs, ncon, mcon, cnp, pnp, mnp, adata, jac_adata);
    retval=0;
    goto freemem_and_return;
  }

  /* covariances Sigma_x_ij are accommodated by computing the Cholesky decompositions of their
   * inverses and using the resulting matrices w_x_ij to weigh A_ij, B_ij, and e_ij as w_x_ij A_ij,
   * w_x_ij*B_ij and w_x_ij*e_ij. In this way, auxiliary variables as U_j=\sum_i A_ij^T A_ij
   * actually become \sum_i (w_x_ij A_ij)^T w_x_ij A_ij= \sum_i A_ij^T w_x_ij^T w_x_ij A_ij =
   * A_ij^T Sigma_x_ij^-1 A_ij
   *
   * ea_j, V_i, eb_i, W_ij are weighted in a similar manner
   */
  if(covx!=NULL){
    for(i=0; i<n; ++i){
      nnz=sba_crsm_row_elmidxs(&idxij, i, rcidxs, rcsubs); /* find nonzero x_ij, j=0...m-1 */
      for(j=0; j<nnz; ++j){
        /* set ptr1, ptr2 to point to cov_x_ij, w_x_ij resp. */
        ptr1=covx + (k=idxij.val[rcidxs[j]]*covsz);
        ptr2=wght + k;
        if(!sba_mat_cholinv(ptr1, ptr2, mnp)){ /* compute w_x_ij s.t. w_x_ij^T w_x_ij = cov_x_ij^-1 */
			    fprintf(fp, "SBA: invalid covariance matrix for x_ij (i=%d, j=%d) in sba_motstr_levmar_x()\n", i, rcsubs[j]);
          retval=SBA_ERROR;
          goto freemem_and_return;
        }
      }
    }
    sba_mat_cholinv(NULL, NULL, 0); /* cleanup */
  }

  fprintf(fp, "initial unknowns: calib param = %d tie points = %d\n", cnp, n-ncon);
  for(i=0; i<cnp; i++)
  {
	  fprintf(fp, "%.6f ", p[i]);
  }
  fprintf(fp, "\n");
  fprintf(fp, "initial tie points coordinates\n");

  ptr1 = p+cnp*nVFrame;
  for(i=0; i<n; i++)
  {
	  ptr2 = ptr1+i*pnp;
	  fprintf(fp, "%.3f %.3f %.3f\n", ptr2[0], ptr2[1], ptr2[2]);
  }
  

  /* compute the error vectors e_ij in hx */
  (*func)(p, &idxij, rcidxs, rcsubs, hx, adata); nfev=1;
  /* ### compute e=x - f(p) [e=w*(x - f(p)] and its L2 norm */
  if(covx==NULL)
    p_eL2=nrmL2xmy(e, x, hx, nobs); /* e=x-hx, p_eL2=||e|| */
  else
    p_eL2=nrmCxmy(e, x, hx, wght, mnp, nvis); /* e=wght*(x-hx), p_eL2=||e||=||x-hx||_Sigma^-1 */
  if(verbose) fprintf(fp, "initial motstr-SBA error %g [%g]\n", p_eL2, p_eL2/nvis);
  init_p_eL2=p_eL2;
  if(!SBA_FINITE(p_eL2)) stop=7;

  for(itno=0; itno<itmax && !stop; ++itno){
    /* Note that p, e and ||e||_2 have been updated at the previous iteration */

    /* compute derivative submatrices A_ij, B_ij */
    (*fjac)(p, &idxij, rcidxs, rcsubs, jac, jac_adata); ++njev;

    if(covx!=NULL){
      /* compute w_x_ij A_ij and w_x_ij B_ij.
       * Since w_x_ij is upper triangular, the products can be safely saved
       * directly in A_ij, B_ij, without the need for intermediate storage
       */
      for(i=0; i<nvis; ++i){
        /* set ptr1, ptr2, ptr3 to point to w_x_ij, A_ij, B_ij, resp. */
        ptr1=wght + i*covsz;
        ptr2=jac  + i*ABsz;
        ptr3=ptr2 + Asz; // ptr3=jac  + i*ABsz + Asz;

        /* w_x_ij is mnp x mnp, A_ij is mnp x cnp, B_ij is mnp x pnp
         * NOTE: Jamming the outter (i.e., ii) loops did not run faster!
         */
        /* A_ij */
        for(ii=0; ii<mnp; ++ii)
          for(jj=0; jj<cnp; ++jj){
            for(k=ii, sum=0.0; k<mnp; ++k) // k>=ii since w_x_ij is upper triangular
              sum+=ptr1[ii*mnp+k]*ptr2[k*cnp+jj];
            ptr2[ii*cnp+jj]=sum;
          }

        /* B_ij */
        for(ii=0; ii<mnp; ++ii)
          for(jj=0; jj<pnp; ++jj){
            for(k=ii, sum=0.0; k<mnp; ++k) // k>=ii since w_x_ij is upper triangular
              sum+=ptr1[ii*mnp+k]*ptr3[k*pnp+jj];
            ptr3[ii*pnp+jj]=sum;
          }
      }
    }

    /* compute U_j = \sum_i A_ij^T A_ij */ // \Sigma here!
    /* U_j is symmetric, therefore its computation can be sped up by
     * computing only the upper part and then reusing it for the lower one.
     * Recall that A_ij is mnp x cnp
     */
    /* Also compute ea_j = \sum_i A_ij^T e_ij */ // \Sigma here!
    /* Recall that e_ij is mnp x 1
     */
    _dblzero(U, nVFrame*Usz); /* clear all U_j */
    _dblzero(ea, nVFrame*easz); /* clear all ea_j */
    for(j=0; j<nVFrame; ++j){
      ptr1=U + j*Usz; // set ptr1 to point to U_j
      ptr2=ea + j*easz; // set ptr2 to point to ea_j

//      nnz=sba_crsm_col_elmidxs(&idxij, j, rcidxs, rcsubs); /* find nonzero A_ij, i=0...n-1 */
      for(i=0; i<nvis; ++i){
        /* set ptr3 to point to A_ij, actual row number in rcsubs[i] */
        ptr3=jac + i*ABsz; //ABsz: 一个像点对应的未知数个数

        /* compute the UPPER TRIANGULAR PART of A_ij^T A_ij and add it to U_j */
        for(ii=0; ii<cnp; ++ii){
          for(jj=ii; jj<cnp; ++jj){
            for(k=0, sum=0.0; k<mnp; ++k)
              sum+=ptr3[k*cnp+ii]*ptr3[k*cnp+jj];
            ptr1[ii*cnp+jj]+=sum;
          }

          /* copy the LOWER TRIANGULAR PART of U_j from the upper one */
          for(jj=0; jj<ii; ++jj)
            ptr1[ii*cnp+jj]=ptr1[jj*cnp+ii];
        }

        ptr4=e + i*esz; /* set ptr4 to point to e_ij */
        /* compute A_ij^T e_ij and add it to ea_j */
        for(ii=0; ii<cnp; ++ii){
          for(jj=0, sum=0.0; jj<mnp; ++jj)
            sum+=ptr3[jj*cnp+ii]*ptr4[jj];
          ptr2[ii]+=sum;
        }
      }
    }

    /* compute V_i = \sum_j B_ij^T B_ij */ // \Sigma here!
    /* V_i is symmetric, therefore its computation can be sped up by
     * computing only the upper part and then reusing it for the lower one.
     * Recall that B_ij is mnp x pnp
     */
    /* Also compute eb_i = \sum_j B_ij^T e_ij */ // \Sigma here!
    /* Recall that e_ij is mnp x 1
     */
	  _dblzero(V, n*Vsz); /* clear all V_i */
	  _dblzero(eb, n*ebsz); /* clear all eb_i */
    for(i=ncon; i<n; ++i){
      ptr1=V + i*Vsz; // set ptr1 to point to V_i
      ptr2=eb + i*ebsz; // set ptr2 to point to eb_i

      nnz=sba_crsm_row_elmidxs(&idxij, i, rcidxs, rcsubs); /* find nonzero B_ij, j=0...m-1 */
      for(j=0; j<nnz; ++j){
        /* set ptr3 to point to B_ij, actual column number in rcsubs[j] */
        ptr3=jac + idxij.val[rcidxs[j]]*ABsz + Asz;
      
        /* compute the UPPER TRIANGULAR PART of B_ij^T B_ij and add it to V_i */
        for(ii=0; ii<pnp; ++ii){
          for(jj=ii; jj<pnp; ++jj){
            for(k=0, sum=0.0; k<mnp; ++k)
              sum+=ptr3[k*pnp+ii]*ptr3[k*pnp+jj];
            ptr1[ii*pnp+jj]+=sum;
          }
        }

        ptr4=e + idxij.val[rcidxs[j]]*esz; /* set ptr4 to point to e_ij */
        /* compute B_ij^T e_ij and add it to eb_i */
        for(ii=0; ii<pnp; ++ii){
          for(jj=0, sum=0.0; jj<mnp; ++jj)
            sum+=ptr3[jj*pnp+ii]*ptr4[jj];
          ptr2[ii]+=sum;
        }
      }
    }

    /* compute W_ij =  A_ij^T B_ij */ // \Sigma here!
    /* Recall that A_ij is mnp x cnp and B_ij is mnp x pnp
     */
	// W 长度： cnp*(pnp*nties)
	_dblzero(W, n*Wsz); /* clear all eb_i */
    for(i=ncon; i<n; ++i){
      nnz=sba_crsm_row_elmidxs(&idxij, i, rcidxs, rcsubs); /* find nonzero W_ij, j=0...m-1 */
	  ptr1=W + i*Wsz;	

      for(j=0; j<nnz; ++j){
        /* set ptr1 to point to W_ij, actual column number in rcsubs[j] */
//        ptr1=W + idxij.val[rcidxs[j]]*Wsz;//Ai^T * Bi 

        if(rcsubs[j]<mcon){ /* A_ij is zero */
          //_dblzero(ptr1, Wsz); /* clear W_ij */
          continue;
        }

        /* set ptr2 & ptr3 to point to A_ij & B_ij resp. */
        ptr2=jac  + idxij.val[rcidxs[j]]*ABsz;
        ptr3=ptr2 + Asz;
        /* compute A_ij^T B_ij and store it in W_ij
         * Recall that storage for A_ij, B_ij does not overlap with that for W_ij,
         * see the comments related to the initialization of jac above
         */
        /* assert(ptr2-ptr1>=Wsz); */
        for(ii=0; ii<cnp; ++ii)
          for(jj=0; jj<pnp; ++jj){
            for(k=0, sum=0.0; k<mnp; ++k)
              sum+=ptr2[k*cnp+ii]*ptr3[k*pnp+jj];
            ptr1[ii*pnp+jj] += sum;
          }
      }
    }

    /* Compute ||J^T e||_inf and ||p||^2 */
    for(i=0, p_L2=eab_inf=0.0; i<nvars; ++i){
      if(eab_inf < (tmp=FABS(eab[i]))) eab_inf=tmp;
      p_L2+=p[i]*p[i];
    }
    //p_L2=sqrt(p_L2);

    /* save diagonal entries so that augmentation can be later canceled.
     * Diagonal entries are in U_j and V_i
     */
    for(j=mcon; j<nVFrame; ++j){
      ptr1=U + j*Usz; // set ptr1 to point to U_j
      ptr2=diagU + j*cnp; // set ptr2 to point to diagU_j
      for(i=0; i<cnp; ++i)
        ptr2[i]=ptr1[i*cnp+i];
    }
    for(i=ncon; i<n; ++i){
      ptr1=V + i*Vsz; // set ptr1 to point to V_i
      ptr2=diagV + i*pnp; // set ptr2 to point to diagV_i
      for(j=0; j<pnp; ++j)
        ptr2[j]=ptr1[j*pnp+j];
    }

/*
if(!(itno%100)){
  printf("Current estimate: ");
  for(i=0; i<nvars; ++i)
    printf("%.9g ", p[i]);
  printf("-- errors %.9g %0.9g\n", eab_inf, p_eL2);
}
*/

    /* check for convergence */
    if((eab_inf <= eps1)){
      dp_L2=0.0; /* no increment for p in this case */
      stop=1;
      break;
    }

   /* compute initial damping factor */
    if(itno==0){
      /* find max diagonal element */
      for(i=mcon*cnp, tmp=DBL_MIN; i<nVFrame*cnp; ++i)
        if(diagUV[i]>tmp) tmp=diagUV[i];
      for(i=nVFrame*cnp + ncon*pnp; i<nvars; ++i) /* tmp is not re-initialized! */
        if(diagUV[i]>tmp) tmp=diagUV[i];
      mu=tau*tmp;
    }

    /* determine increment using adaptive damping */
    while(1){
      /* augment U, V */
      for(j=mcon; j<nVFrame; ++j){
        ptr1=U + j*Usz; // set ptr1 to point to U_j
        for(i=0; i<cnp; ++i)
          ptr1[i*cnp+i]+=mu;
      }
      for(i=ncon; i<n; ++i){
        ptr1=V + i*Vsz; // set ptr1 to point to V_i
        for(j=0; j<pnp; ++j)
          ptr1[j*pnp+j]+=mu;

		    /* compute V*_i^-1.
         * Recall that only the upper triangle of the symmetric pnp x pnp matrix V*_i
         * is stored in ptr1; its (also symmetric) inverse is saved in the lower triangle of ptr1
         */
        /* inverting V*_i with LDLT seems to result in faster overall execution compared to when using LU or Cholesky */
        //j=sba_symat_invert_LU(ptr1, pnp); matinv=sba_symat_invert_LU;
        //j=sba_symat_invert_Chol(ptr1, pnp); matinv=sba_symat_invert_Chol;
        j=sba_symat_invert_BK(ptr1, pnp); matinv=sba_symat_invert_BK;
		    if(!j){
			    fprintf(fp, "SBA: singular matrix V*_i (i=%d) in sba_motstr_levmar_x(), increasing damping\n", i);
          goto moredamping; // increasing damping will eventually make V*_i diagonally dominant, thus nonsingular
          //retval=SBA_ERROR;
          //goto freemem_and_return;
		    }
      }

	  //E = ea - W * (V^-1) * eb
      _dblzero(E, nVFrame*easz); /* clear all e_j */
      /* compute the mmcon x mmcon block matrix S and e_j */

      /* Note that S is symmetric, therefore its computation can be
       * sped up by computing only the upper part and then reusing
       * it for the lower one.
		   */
      for(j=mcon; j<nVFrame; ++j){
        int mmconxUsz=mmcon*Usz;

// 		nnz=sba_crsm_col_elmidxs(&idxij, j, rcidxs, rcsubs); /* find nonzero Y_ij, i=0...n-1 */
// 
// 		nnz = nvis;

        /* get rid of all Y_ij with i<ncon that are treated as zeros.
         * In this way, all rcsubs[i] below are guaranteed to be >= ncon
         */
        if(ncon){
          for(i=ii=0; i<nnz; ++i){
            if(rcsubs[i]>=ncon){
              rcidxs[ii]=rcidxs[i];
              rcsubs[ii++]=rcsubs[i];
            }
          }
          nnz=ii;
        }

        /* compute all Y_ij = W_ij (V*_i)^-1 for a *fixed* j.
         * To save memory, the block matrix consisting of the Y_ij
         * is not stored. Instead, only a block column of this matrix
         * is computed & used at each time: For each j, all nonzero
         * Y_ij are computed in Yj and then used in the calculations
         * involving S_jk and e_j.
         * Recall that W_ij is cnp x pnp and (V*_i) is pnp x pnp
         */
		//按物方点遍历
        for(i=0; i<n; ++i){
          /* set ptr3 to point to (V*_i)^-1, actual row number in rcsubs[i] */
          ptr3=V + i*Vsz;

          /* set ptr1 to point to Y_ij, actual row number in rcsubs[i] */
          ptr1=Yj + i*Ysz;
          /* set ptr2 to point to W_ij resp. */
          ptr2=W + i*Wsz;
          /* compute W_ij (V*_i)^-1 and store it in Y_ij.
           * Recall that only the lower triangle of (V*_i)^-1 is stored
           */
          for(ii=0; ii<cnp; ++ii){
            ptr4=ptr2+ii*pnp;
            for(jj=0; jj<pnp; ++jj){
              for(k=0, sum=0.0; k<=jj; ++k)
                sum+=ptr4[k]*ptr3[jj*pnp+k]; //ptr2[ii*pnp+k]*ptr3[jj*pnp+k];
              for( ; k<pnp; ++k)
                sum+=ptr4[k]*ptr3[k*pnp+jj]; //ptr2[ii*pnp+k]*ptr3[k*pnp+jj];
              ptr1[ii*pnp+jj]=sum;
            }
          }
        }

        /* compute the UPPER TRIANGULAR PART of S */
        for(k=j; k<nVFrame; ++k){ // j>=mcon
          /* compute \sum_i Y_ij W_ik^T in YWt. Note that for an off-diagonal block defined by j, k
           * YWt (and thus S_jk) is nonzero only if there exists a point that is visible in both the
           * j-th and k-th images
           */
          
          /* Recall that Y_ij is cnp x pnp and W_ik is cnp x pnp */ 
          _dblzero(YWt, YWtsz); /* clear YWt */

		  //按物方点遍历
          for(i=0; i<n; ++i){
            register double *pYWt;

            /* find the min and max column indices of the elements in row i (actually rcsubs[i])
             * and make sure that k falls within them. This test handles W_ik's which are
             * certain to be zero without bothering to call sba_crsm_elmidx()
             */

//             ii=idxij.colidx[idxij.rowptr[rcsubs[i]]];
//             jj=idxij.colidx[idxij.rowptr[rcsubs[i]+1]-1];
//             if(k<ii || k>jj) continue; /* W_ik == 0 */
// 
//             /* set ptr2 to point to W_ik */
//             l=sba_crsm_elmidxp(&idxij, rcsubs[i], k, j, rcidxs[i]);
//             //l=sba_crsm_elmidx(&idxij, rcsubs[i], k);
//             if(l==-1) continue; /* W_ik == 0 */

            ptr2=W + i*Wsz;
            /* set ptr1 to point to Y_ij, actual row number in rcsubs[i] */
            ptr1=Yj + i*Ysz;
            for(ii=0; ii<cnp; ++ii){
              ptr3=ptr1+ii*pnp;
              pYWt=YWt+ii*cnp;

              for(jj=0; jj<cnp; ++jj){
                ptr4=ptr2+jj*pnp;
                for(l=0, sum=0.0; l<pnp; ++l)
                  sum+=ptr3[l]*ptr4[l]; //ptr1[ii*pnp+l]*ptr2[jj*pnp+l];
                pYWt[jj]+=sum; //YWt[ii*cnp+jj]+=sum;
              }
            }
          }
		  
		      /* since the linear system involving S is solved with lapack,
		       * it is preferable to store S in column major (i.e. fortran)
		       * order, so as to avoid unecessary transposing/copying.
           */
#if MAT_STORAGE==COLUMN_MAJOR
          ptr2=S + (k-mcon)*mmconxUsz + (j-mcon)*cnp; // set ptr2 to point to the beginning of block j,k in S
#else
          ptr2=S + (j-mcon)*mmconxUsz + (k-mcon)*cnp; // set ptr2 to point to the beginning of block j,k in S
#endif
		  
          if(j!=k){ /* Kronecker */
            for(ii=0; ii<cnp; ++ii, ptr2+=Sdim)
              for(jj=0; jj<cnp; ++jj)
                ptr2[jj]=
#if MAT_STORAGE==COLUMN_MAJOR
				                -YWt[jj*cnp+ii];
#else
				                -YWt[ii*cnp+jj];
#endif
          }
          else{
            ptr1=U + j*Usz; // set ptr1 to point to U_j

            for(ii=0; ii<cnp; ++ii, ptr2+=Sdim)
              for(jj=0; jj<cnp; ++jj)
                ptr2[jj]=
#if MAT_STORAGE==COLUMN_MAJOR
				                ptr1[jj*cnp+ii] - YWt[jj*cnp+ii];
#else
				                ptr1[ii*cnp+jj] - YWt[ii*cnp+jj];
#endif
          }
        }

        /* copy the LOWER TRIANGULAR PART of S from the upper one */
        for(k=mcon; k<j; ++k){
#if MAT_STORAGE==COLUMN_MAJOR
          ptr1=S + (k-mcon)*mmconxUsz + (j-mcon)*cnp; // set ptr1 to point to the beginning of block j,k in S
          ptr2=S + (j-mcon)*mmconxUsz + (k-mcon)*cnp; // set ptr2 to point to the beginning of block k,j in S
#else
          ptr1=S + (j-mcon)*mmconxUsz + (k-mcon)*cnp; // set ptr1 to point to the beginning of block j,k in S
          ptr2=S + (k-mcon)*mmconxUsz + (j-mcon)*cnp; // set ptr2 to point to the beginning of block k,j in S
#endif
          for(ii=0; ii<cnp; ++ii, ptr1+=Sdim)
            for(jj=0, ptr3=ptr2+ii; jj<cnp; ++jj, ptr3+=Sdim)
              ptr1[jj]=*ptr3;
        }

        /* compute e_j=ea_j - \sum_i Y_ij eb_i */
        /* Recall that Y_ij is cnp x pnp and eb_i is pnp x 1 */
        ptr1=E + j*easz; // set ptr1 to point to e_j

        for(i=0; i<n; ++i){
          /* set ptr2 to point to Y_ij, actual row number in rcsubs[i] */
          ptr2=Yj + i*Ysz;

          /* set ptr3 to point to eb_i */
          ptr3=eb + i*ebsz;
          for(ii=0; ii<cnp; ++ii){
            ptr4=ptr2+ii*pnp;
            for(jj=0, sum=0.0; jj<pnp; ++jj)
              sum+=ptr4[jj]*ptr3[jj]; //ptr2[ii*pnp+jj]*ptr3[jj];
            ptr1[ii]+=sum;
          }
        }

        ptr2=ea + j*easz; // set ptr2 to point to ea_j
        for(i=0; i<easz; ++i)
          ptr1[i]=ptr2[i] - ptr1[i];
      }

#if 0
      if(verbose>1){ /* count the nonzeros in S */
        for(i=ii=0; i<Sdim*Sdim; ++i)
          if(S[i]!=0.0) ++ii;
        printf("\nS density: %.5g\n", ((double)ii)/(Sdim*Sdim)); fflush(stdout);
      }
#endif

      /* solve the linear system S dpa = E to compute the da_j.
       *
       * Note that if MAT_STORAGE==1 S is modified in the following call;
       * this is OK since S is recomputed for each iteration
       */
	    //issolved=sba_Axb_LU(S, E+mcon*cnp, dpa+mcon*cnp, Sdim, MAT_STORAGE); linsolver=sba_Axb_LU;
      issolved=sba_Axb_Chol(S, E+mcon*cnp, dpa+mcon*cnp, Sdim, MAT_STORAGE); linsolver=sba_Axb_Chol;
      //issolved=sba_Axb_BK(S, E+mcon*cnp, dpa+mcon*cnp, Sdim, MAT_STORAGE); linsolver=sba_Axb_BK;
      //issolved=sba_Axb_QRnoQ(S, E+mcon*cnp, dpa+mcon*cnp, Sdim, MAT_STORAGE); linsolver=sba_Axb_QRnoQ;
      //issolved=sba_Axb_QR(S, E+mcon*cnp, dpa+mcon*cnp, Sdim, MAT_STORAGE); linsolver=sba_Axb_QR;
	    //issolved=sba_Axb_SVD(S, E+mcon*cnp, dpa+mcon*cnp, Sdim, MAT_STORAGE); linsolver=sba_Axb_SVD;
	    //issolved=sba_Axb_CG(S, E+mcon*cnp, dpa+mcon*cnp, Sdim, (3*Sdim)/2, 1E-10, SBA_CG_JACOBI, MAT_STORAGE); linsolver=(PLS)sba_Axb_CG;

      ++nlss;

	    _dblzero(dpa, mcon*cnp); /* no change for the first mcon camera params */

      if(issolved){

        /* compute the db_i */
		  //计算物方点坐标改正数
        for(i=ncon; i<n; ++i){
          ptr1=dpb + i*ebsz; // set ptr1 to point to db_i

          /* compute \sum_j W_ij^T da_j */
          /* Recall that W_ij is cnp x pnp and da_j is cnp x 1 */
          _dblzero(Wtda, Wtdasz); /* clear Wtda */
//           nnz=sba_crsm_row_elmidxs(&idxij, i, rcidxs, rcsubs); /* find nonzero W_ij, j=0...m-1 */
//           for(j=0; j<nnz; ++j){
            /* set ptr2 to point to W_ij, actual column number in rcsubs[j] */
//			      if(rcsubs[j]<mcon) continue; /* W_ij is zero */

            ptr2=W + i*Wsz;

            /* set ptr3 to point to da_j */
            ptr3=dpa/* + rcsubs[j]*cnp*/;	//只有一张等效片

            for(ii=0; ii<pnp; ++ii){
              ptr4=ptr2+ii;
              for(jj=0, sum=0.0; jj<cnp; ++jj)
                sum+=ptr4[jj*pnp]*ptr3[jj]; //ptr2[jj*pnp+ii]*ptr3[jj];
              Wtda[ii]+=sum;
            }
//          }

          /* compute eb_i - \sum_j W_ij^T da_j = eb_i - Wtda in Wtda */
          ptr2=eb + i*ebsz; // set ptr2 to point to eb_i
          for(ii=0; ii<pnp; ++ii)
            Wtda[ii]=ptr2[ii] - Wtda[ii];

          /* compute the product (V*_i)^-1 Wtda = (V*_i)^-1 (eb_i - \sum_j W_ij^T da_j).
           * Recall that only the lower triangle of (V*_i)^-1 is stored
           */
          ptr2=V + i*Vsz; // set ptr2 to point to (V*_i)^-1
          for(ii=0; ii<pnp; ++ii){
            for(jj=0, sum=0.0; jj<=ii; ++jj)
              sum+=ptr2[ii*pnp+jj]*Wtda[jj];
            for( ; jj<pnp; ++jj)
              sum+=ptr2[jj*pnp+ii]*Wtda[jj];
            ptr1[ii]=sum;
          }
        }
	      _dblzero(dpb, ncon*pnp); /* no change for the first ncon point params */

        /* parameter vector updates are now in dpa, dpb */

        /* compute p's new estimate and ||dp||^2 */
        for(i=0, dp_L2=0.0; i<nvars; ++i){
          pdp[i]=p[i] + (tmp=dp[i]);
          dp_L2+=tmp*tmp;
        }
        //dp_L2=sqrt(dp_L2);

        if(dp_L2<=eps2_sq*p_L2){ /* relative change in p is small, stop */
        //if(dp_L2<=eps2*(p_L2 + eps2)){ /* relative change in p is small, stop */
          stop=2;
          break;
        }

        if(dp_L2>=(p_L2+eps2)/SBA_EPSILON_SQ){ /* almost singular */
        //if(dp_L2>=(p_L2+eps2)/SBA_EPSILON){ /* almost singular */
          fprintf(fp, "SBA: the matrix of the augmented normal equations is almost singular in sba_motstr_levmar_x(),\n"
                          "     minimization should be restarted from the current solution with an increased damping term\n");
          retval=SBA_ERROR;
          goto freemem_and_return;
       }

        (*func)(pdp, &idxij, rcidxs, rcsubs, hx, adata); ++nfev; /* evaluate function at p + dp */
        if(verbose>1)
          fprintf(fp, "mean reprojection error %g\n", sba_mean_repr_error(n, mnp, x, hx, &idxij, rcidxs, rcsubs));
        /* ### compute ||e(pdp)||_2 */
        if(covx==NULL)
          pdp_eL2=nrmL2xmy(hx, x, hx, nobs); /* hx=x-hx, pdp_eL2=||hx|| */
        else
          pdp_eL2=nrmCxmy(hx, x, hx, wght, mnp, nvis); /* hx=wght*(x-hx), pdp_eL2=||hx|| */
        if(!SBA_FINITE(pdp_eL2)){
          if(verbose) /* identify the offending point projection */
            sba_print_inf(fp, hx, nVFrame, mnp, &idxij, rcidxs, rcsubs);

          stop=7;
          break;
        }

        for(i=0, dL=0.0; i<nvars; ++i)
          dL+=dp[i]*(mu*dp[i]+eab[i]);

        dF=p_eL2-pdp_eL2;

        if(verbose>1)
          printf("\ndamping term %8g, gain ratio %8g, errors %8g / %8g = %g\n", mu, dL!=0.0? dF/dL : dF/DBL_EPSILON, p_eL2/nvis, pdp_eL2/nvis, p_eL2/pdp_eL2);

        if(dL>0.0 && dF>0.0){ /* reduction in error, increment is accepted */
          tmp=(2.0*dF/dL-1.0);
          tmp=1.0-tmp*tmp*tmp;
          mu=mu*( (tmp>=SBA_ONE_THIRD)? tmp : SBA_ONE_THIRD );
          nu=2;

          /* the test below is equivalent to the relative reduction of the RMS reprojection error: sqrt(p_eL2)-sqrt(pdp_eL2)<eps4*sqrt(p_eL2) */
          if(pdp_eL2-2.0*sqrt(p_eL2*pdp_eL2)<(eps4_sq-1.0)*p_eL2) stop=4;
          
          for(i=0; i<nvars; ++i) /* update p's estimate */
            p[i]=pdp[i];

          for(i=0; i<nobs; ++i) /* update e and ||e||_2 */
            e[i]=hx[i];
          p_eL2=pdp_eL2;
          break;
        }
      } /* issolved */

moredamping:
      /* if this point is reached (also via an explicit goto!), either the linear system could
       * not be solved or the error did not reduce; in any case, the increment must be rejected
       */

      mu*=nu;
      nu2=nu<<1; // 2*nu;
      if(nu2<=nu){ /* nu has wrapped around (overflown) */
        fprintf(fp, "SBA: too many failed attempts to increase the damping factor in sba_motstr_levmar_x()! Singular Hessian matrix?\n");
        //exit(1);
        stop=6;
        break;
      }
      nu=nu2;

#if 0
      /* restore U, V diagonal entries */
      for(j=mcon; j<m; ++j){
        ptr1=U + j*Usz; // set ptr1 to point to U_j
        ptr2=diagU + j*cnp; // set ptr2 to point to diagU_j
        for(i=0; i<cnp; ++i)
          ptr1[i*cnp+i]=ptr2[i];
      }
      for(i=ncon; i<n; ++i){
        ptr1=V + i*Vsz; // set ptr1 to point to V_i
        ptr2=diagV + i*pnp; // set ptr2 to point to diagV_i
        for(j=0; j<pnp; ++j)
          ptr1[j*pnp+j]=ptr2[j];
      }
#endif
    } /* inner while loop */

    if(p_eL2<=eps3_sq) stop=5; // error is small, force termination of outer loop
  }

  if(itno>=itmax) stop=3;

  /* restore U, V diagonal entries */
  for(j=mcon; j<nVFrame; ++j){
    ptr1=U + j*Usz; // set ptr1 to point to U_j
    ptr2=diagU + j*cnp; // set ptr2 to point to diagU_j
    for(i=0; i<cnp; ++i)
      ptr1[i*cnp+i]=ptr2[i];
  }
  for(i=ncon; i<n; ++i){
    ptr1=V + i*Vsz; // set ptr1 to point to V_i
    ptr2=diagV + i*pnp; // set ptr2 to point to diagV_i
    for(j=0; j<pnp; ++j)
     ptr1[j*pnp+j]=ptr2[j];
  }

  if(info){
    info[0]=init_p_eL2;
    info[1]=p_eL2;
    info[2]=eab_inf;
    info[3]=dp_L2;
    for(j=mcon, tmp=DBL_MIN; j<nVFrame; ++j){
      ptr1=U + j*Usz; // set ptr1 to point to U_j
      for(i=0; i<cnp; ++i)
        if(tmp<ptr1[i*cnp+i]) tmp=ptr1[i*cnp+i];
    }
    for(i=ncon; i<n; ++i){
      ptr1=V + i*Vsz; // set ptr1 to point to V_i
      for(j=0; j<pnp; ++j)
        if(tmp<ptr1[j*pnp+j]) tmp=ptr1[j*pnp+j];
      }
    info[4]=mu/tmp;
    info[5]=itno;
    info[6]=stop;
    info[7]=nfev;
    info[8]=njev;
    info[9]=nlss;
  }
                                                               
  //sba_print_sol(n, m, p, cnp, pnp, x, mnp, &idxij, rcidxs, rcsubs);
  retval=(stop!=7)?  itno : SBA_ERROR;

  fprintf(fp, "stop=%d: ", stop);
  switch(stop)
  {
  case 0:
	  fprintf(fp, "Quit adjustment\n");
	  break;
  case 1:
	  fprintf(fp, "no increment for p\n");
	  break;
  case 2:
	  fprintf(fp, "relative change in p is rather small\n");
	  break;
  case 3:
	  fprintf(fp, "iterater > itmax\n");
	  break;
  case 4:
	  fprintf(fp, "the RMS reprojection error is convergency\n");
	  break;
  case 5:
	  fprintf(fp, "error is small\n");
	  break;
  case 6:
	  fprintf(fp, "LM failed\n");
	  break;
  case 7:
	  fprintf(fp, "RMS reprojection error is devergency\n");
	  break;
  }

  fprintf(fp, "result unknowns: calib param = %d tie points = %d\n", cnp, n-ncon);
  for(i=0; i<cnp; i++)
  {
	  fprintf(fp, "%.6f ", p[i]);
  }
  fprintf(fp, "\n");
  fprintf(fp, "result tie points coordinates\n");

  ptr1 = p+cnp*nVFrame;
  for(i=0; i<n; i++)
  {
	  ptr2 = ptr1+i*pnp;
	  fprintf(fp, "%.3f %.3f %.3f\n", ptr2[0], ptr2[1], ptr2[2]);
  }

  if(verbose) fprintf(fp, "result motstr-SBA error %g [%g]\n", pdp_eL2, pdp_eL2/nvis);

freemem_and_return: /* NOTE: this point is also reached via a goto! */

   /* free whatever was allocated */
  free(W);   free(U);  free(V);	  free(jac);
  free(e);   free(eab);
  free(E);   free(Yj); free(YWt);
  free(S);   free(dp); free(Wtda);
  free(rcidxs); free(rcsubs);
#ifndef SBA_DESTROY_COVS
  if(wght) free(wght);
#else
  /* nothing to do */
#endif /* SBA_DESTROY_COVS */

  free(hx); free(diagUV); free(pdp);
  if(fdj_data.hxx){ // cleanup
    free(fdj_data.hxx);
    free(fdj_data.func_rcidxs);
  }

  sba_crsm_free(&idxij);

  /* free the memory allocated by the matrix inversion & linear solver routines */
  if(matinv) (*matinv)(NULL, 0);
  if(linsolver) (*linsolver)(NULL, NULL, NULL, 0, 0);

  return retval;
}


orsAlgCameraCalib::orsAlgCameraCalib()
{

}

orsAlgCameraCalib::~orsAlgCameraCalib()
{

}

int orsAlgCameraCalib::bundle( void *adata )
{
	int retval;
	double opts[SBA_OPTSSZ], info[SBA_INFOSZ];
	int howto, expert, analyticjac, fixedcal, havedist, n, prnt, verbose=1;
	int nframes, numpts3D, numprojs, nvars;
	const int nconstframes=0;
	register int i;
	FILE *fp;

	clock_t start_time, end_time;

	imgCalib_globs *globs;
	globs = (imgCalib_globs*)adata;
	

	/* call sparse LM routine */
	opts[0]=SBA_INIT_MU;		//初始mu 
	opts[1]=SBA_STOP_THRESH;	//||J^T e||_inf    JX = e
	opts[2]=SBA_STOP_THRESH;    //||dp||_2	未知数增量
	opts[3]=SBA_STOP_THRESH;	//||e||_2 重投影误差
	//opts[3]=0.05*pixsize; 
	opts[4]=0.0;				//(||e||_2-||e_new||_2)/||e||_2
	//opts[4]=1E-05; // uncomment to force termination if the relative reduction in the RMS reprojection error drops below 1E-05

	start_time=clock();

	nframes = globs->nimg;
	numpts3D = globs->ntie;
	numprojs = globs->nproj;
	//未知数个数
	nvars=nframes*globs->cnp+numpts3D*globs->pnp;
	
	double *covimgpts=(double *)malloc(globs->nproj*globs->mnp*globs->mnp*sizeof(double));
	
	retval=sba_motstr_levmar_x_imgCalib(numpts3D, 0, nframes, nconstframes, globs->mask, globs->pUnknowns, globs->cnp, globs->pnp,
			globs->pImgpts, NULL, globs->mnp,
			_img_projs,
			_img_projsRTS_jac,
			adata, MAXITER2, verbose, opts, info);
	
	end_time=clock();

	fprintf(stdout, "Elapsed time: %.2lf seconds\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC);

cleanup:
	if(covimgpts)	free(covimgpts);
	return retval;
}

int orsAlgCameraCalib::bundle_SparseLM( void *adata )
{


	return 0;
}