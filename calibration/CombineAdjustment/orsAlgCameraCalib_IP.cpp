#include "StdAfx.h"
#include "CombineAdjustment/orsAlgCameraCalib.h"
#include "\OpenRS\external\source\sba-1.6\sba.h"
#include "orsMath/orsIMatrixService.h"
#include "dpsMatrix.h"
#include "levmar\levmar.h"

//按照IP的坐标系定义计算
inline void calcImgProj_IP(double *calibParam, double t[3], double angle[3], double objpt[3], double imgpt[2], void *adata)
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


	matrix_product(3, 3, 3, 3, Rimu, Rmis, R); //R = Rimu*Rmis  与BA一致

	double a1,a2,a3,b1,b2,b3,c1,c2,c3;

	a1 = R[0];	a2 = R[1];	a3 = R[2];
	b1 = R[3];	b2 = R[4];	b3 = R[5];
	c1 = R[6];	c2 = R[7];	c3 = R[8];

	double vec1[3], vec2[3];

	matrix_product(3, 3, 3, 1, Rimu, uvw, vec1);

	vec2[0] = objpt[0] - t[0] - vec1[0];
	vec2[1] = objpt[1] - t[1] - vec1[1];
	vec2[2] = objpt[2] - t[2] - vec1[2];

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

	_X = a1 * vec2[0] + b1 * vec2[1] + c1 * vec2[2];
	_Y = a2 * vec2[0] + b2 * vec2[1] + c2 * vec2[2];
	_Z = a3 * vec2[0] + b3 * vec2[1] + c3 * vec2[2];

	xp = - f * _X / _Z;
	yp = - f * _Y / _Z;

	imgpt[0] = xp;
	imgpt[1] = yp;
}

//计算像点坐标
//pX: 全部未知数
//projs: 像点坐标
static void _img_projs_IP(double *pX, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *projs, void *adata)
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
			calcImgProj_IP(calibParam, pt, pqr, ppt, pV, adata); //计算像点坐标近似值 
		}
	}
}

//多片前方交会计算物方坐标, 像点坐标的改正不在此函数中进行
//AX=L	最小二乘解
//				|a1f + a3x,   b1f + b3x,  c1f + c3x|
//			A=	|								   |				A=2*3    X=[X, Y, Z]^T
//				|a2f + a3y,   b2f + b3y,  c2f + c3y|
//
//              |(a1f+a3x)Xs + (b1f+b3x)Ys + (c1f+c3x)Zs |
//         L =  |									     |		L=2*1	
//              |(a2f+a3y)Xs + (b2f+b3y)Ys + (c2f+c3y)Zs |
static void _img_intersection_IP(double *pX, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *pObsV, void *adata)
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

	exor=gl->pImgExor;		//像片外方位元素
	pb=pX + 1*cnp;	//3D点
	double *pCalib = pX;	//当前标定参数

	double Rmis[9], Rimu[9], R[9];
	double uvw[3], vec1[3];

	switch(gl->calibType)
	{
	case CamGeo_rph:
		matrixService->RotateMatrix_fwk(pCalib[0], pCalib[1], pCalib[2], Rmis);

		uvw[0]=gl->calibParam->lever_arm[0];
		uvw[1]=gl->calibParam->lever_arm[1];
		uvw[2]=gl->calibParam->lever_arm[2];
		break;
	case CamGeo_rphxyz:
		matrixService->RotateMatrix_fwk(pCalib[0], pCalib[1], pCalib[2], Rmis);

		uvw[0]=pCalib[3];
		uvw[1]=pCalib[4];
		uvw[2]=pCalib[5];
		break;
	}

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
		ppt=pb + i*pnp;	//得到的物方点
		//		pb=pX+m*cnp+i*pnp;	
		nnz=sba_crsm_row_elmidxs(idxij, i, rcidxs, rcsubs); 
		if(nnz<2)
		{//无法进行前方交互
			// 			ppt[0] = pb[0];
			// 			ppt[1] = pb[1];
			// 			ppt[2] = pb[2];
			continue;
		}

		for(j=0; j<nnz; ++j)
		{
			pt=exor +  idxij->colidx[rcidxs[j]]*6;	//线元素初始值

			pqr=pt+3; 
			ptr3=pObsV + idxij->val[rcidxs[j]]*mnp;	//像点坐标

			x=ptr3[0];
			y=ptr3[1];

			ptr1=A+j*2*3;
			ptr2=L+j*2;

			matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);
			matrix_product(3, 3, 3, 3, Rimu, Rmis, R);	//传统的BA坐标系相同

			a1=R[0];		a2=R[1];		a3=R[2];
			b1=R[3];		b2=R[4];		b3=R[5];
			c1=R[6];		c2=R[7];		c3=R[8];

			matrix_product(3, 3, 3, 1, Rimu, uvw, vec1);
			Xs = pt[0] + vec1[0];	//与传统BA坐标系定义不同
			Ys = pt[1] + vec1[1];
			Zs = pt[2] + vec1[2];

			//			Xs=pt[0];	Ys=pt[1];	Zs=pt[2];

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

static void _img_3D_resection(double *proj, double *p3Ds, int ptNum, double *pX)
{

}

int orsAlgCameraCalib::calib_IP( void *adata )
{
	int ret=0;
	int maxIter0 = 300, maxIter1 = 10;

	register int i, j, ii, jj, k, l;
	int nvis, nnz, retval;

	register double *p3Dpts, *ptr1, *ptr2, *ptr3, *ptr4, sum;
	struct sba_crsm idxij;
	int *rcidxs,  /* work array for the indexes corresponding to the nonzero elements of a single row or
                 column in a sparse matrix, size max(n, m) */
    *rcsubs;  /* work array for the subscripts of nonzero elements in a single row or column of a
                 sparse matrix, size max(n, m) */
	int nobs, nvars;
	int m, n;
	int cnp, pnp, mnp;
	double *pUnknowns, *pObvs;

	imgCalib_globs *globs;
	globs = (imgCalib_globs*)adata;

	char *vmask = globs->mask;
	m = globs->nimg; 
	n = globs->ntie;
	cnp = globs->cnp;	//相片未知数个数
	pnp = globs->pnp;	//物方点未知数个数
	mnp = globs->mnp;	

	pUnknowns = globs->pUnknowns;
	pObvs = globs->pImgpts;

	/* count total number of visible image points */
	for(i=nvis=0, jj=n*m; i<jj; ++i)
		nvis+=(vmask[i]!=0);

	nobs=nvis*2;	//观测值
//	nvars=nVFrame*cnp + n*pnp;	//未知数

	double iterThresh = globs->camInfo->m_pixelX*0.2;

	FILE *fp = globs->fplog;
	if(n < 4){
		fprintf(fp, "cannot solve a problem with fewer tie points [%d] than 4\n", n);
		return SBA_ERROR;
	}
	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

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

	fprintf(fp, "tiePoint: %d nProjs: %d\n", n, nvis);

	//相片上的最大像点数不超过n
	rcidxs = new int[n];
	rcsubs = new int[n];

	double projTh= globs->camInfo->m_pixelX * 1; //重投影误差阈值
	int iter0, iter1;
	double p_eL2;
	double min_eL2=1.7e+308;
	double *exor = globs->pImgExor;

	double Rmis[9];
// 	double leverarm[3], translate[3];
// 	leverarm[0]=imgMisalign.u;
// 	leverarm[1]=imgMisalign.v;
// 	leverarm[2]=imgMisalign.w;

//	RotateMat_fwk(imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa, Rmis);

	//时间延迟解算
	double delay;	//统一时间延迟
	double minTime=0, maxTime=150*1e-6;	//0, 150microsecond
	double interval = 2*1e-6;
	double best_delay;
	
	double *e=new double[nobs];
	double *hx=new double[nobs];
	//虚拟相片上后方交会求解未知数所需要的矩阵
	double *A = new double[2*cnp*nvis];
	double *L = new double[2*nvis];
	double *X = new double[cnp];
	double *covar = new double[cnp*cnp];

	_img_projs_IP(pUnknowns, &idxij, rcidxs, rcsubs, hx, adata); 

	p_eL2=nrmL2xmy(e, pObvs, hx, nobs); /* e=x-hx, p_eL2=||e|| */

	fprintf(fp, "initial unknowns:\n");
	for(i=0; i<cnp; i++)
	{
		fprintf(fp, "%.6f ", pUnknowns[i]);
	}
	fprintf(fp, "\n");

	fprintf(fp, "initial reprojection errors: %.3f\n", sqrt(p_eL2)/nvis);


	p3Dpts = pUnknowns + cnp;
	//注意坐标系的定义与BA中不同
	//BA:	
	//	R = Rimu*Rmis
	//  [Xs,Ys,Zs]^T = [Xgps,Ygps,Zgps]^T - Rimu[u,v,w]^T
	//IP:
	//	R = Rmis*Rimu
	//  [Xs,Ys,Zs]^T = [Xgps,Ygps,Zgps]^T + Rimu[u,v,w]^T
	double vphi, vomega, vkappa;	//虚拟影像的角元素
	double vXs, vYs, vZs;			//虚拟影像的线元素
	for( iter0=0; iter0<maxIter0; iter0++ )
	{
		int nxAngles = 3;	// max: 3
		int nxXyz = 2;		// max: 3
		int nxPrinciplePoint=0;	// max:2
		int nxPrincipleLength=0;	// max:1
		int nxRadiusDistorions = 0;	// max:4
		int nxTangentDistorions = 0;// max:2

		if( iter0 > 20 )	{
			nxPrinciplePoint=2;	
			//nxPrincipleLength =1;
			nxRadiusDistorions = 4;
		}

		int ixXyz = nxAngles;
		int ixPP = ixXyz + nxXyz;
		int ixPL = ixPP + nxPrinciplePoint;
		int ixRD= ixPL + nxPrincipleLength;
		int ixTD = ixRD + nxRadiusDistorions;
		int nx = ixTD + nxTangentDistorions;

		_img_intersection_IP( pUnknowns, &idxij, rcidxs, rcsubs, pObvs, adata);		//前交得到3D点

		double f = globs->camInfo->m_f;
		int nFrame=globs->nimg;
		int n3Dpoint = globs->ntie;
		
		for(iter1=0; iter1<maxIter1; iter1++)
		{//后方交会迭代求解

			if(cnp == 3)
			{
				vphi = pUnknowns[0];
				vomega = pUnknowns[1];
				vkappa = pUnknowns[2];
				vXs = globs->calibParam->lever_arm[0];
				vYs = globs->calibParam->lever_arm[1];
				vZs = globs->calibParam->lever_arm[2];
			}
			else if(cnp == 6)
			{
				vphi = pUnknowns[0];
				vomega = pUnknowns[1];
				vkappa = pUnknowns[2];
				vXs = pUnknowns[3];
				vYs = pUnknowns[4];
				vZs = pUnknowns[5];
			}

			matrixService->RotateMatrix_fwk(vphi, vomega, vkappa, Rmis);
			double a1, a2, a3;
			double b1, b2, b3;
			double c1, c2, c3;
			
			a1 = Rmis[0];	a2 = Rmis[1];	a3 = Rmis[2];
			b1 = Rmis[3];	b2 = Rmis[4];	b3 = Rmis[5];
			c1 = Rmis[6];	c2 = Rmis[7];	c3 = Rmis[8];

			double sinf, cosf, sinw, cosw, sink, cosk;

			sinf = sin(vphi);	cosf = cos(vphi);
			sinw = sin(vomega);	cosw = cos(vomega);
			sink = sin(vkappa);	cosk = cos(vkappa);

			for(j=0; j<nFrame; ++j)
			{
				/* j-th camera exorrameters */
				double *pt = exor + j*6;	//线元素
				double *pqr = pt+3;		//角元素
				double Rimu[9];
				double Xgps, Ygps, Zgps;

				Xgps = pt[0];	Ygps = pt[1];	Zgps = pt[2];

				matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);
				
				int nnz=sba_crsm_col_elmidxs(&idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */
				for(i=0; i<nnz; ++i)
				{//构造虚拟相片的Jac和L
					ptr1 = p3Dpts + rcsubs[i]*pnp;		//物方坐标
					ptr2 = pObvs + idxij.val[rcidxs[i]]*mnp; // 像点坐标

					ptr3 = A + idxij.val[rcidxs[i]]*2*cnp;
					ptr4 = L + idxij.val[rcidxs[i]]*2;

					double vx, vy, vf;	//虚拟影像的像点坐标
					double _X, _Y, _Z;	//换元
					double vec1[3], vec2[3];
					double x_appV, y_appV;	//近似值

// 					vx = Rimu[0]*ptr2[0] + Rimu[1]*ptr2[1] - Rimu[2]*f;
// 					vy = Rimu[3]*ptr2[0] + Rimu[4]*ptr2[1] - Rimu[5]*f;
// 					vf = -(Rimu[6]*ptr2[0] + Rimu[7]*ptr2[1] - Rimu[8]*f);

					vx = ptr2[0];
					vy = ptr2[1];
					vf = f;

					vec1[0] = ptr1[0] - Xgps;
					vec1[1] = ptr1[1] - Ygps;
					vec1[2] = ptr1[2] - Zgps;

					matrix_transpose_product(3, 3, 3, 1, Rimu, vec1, vec2);
					
// 					vec1[0] = Rimu[0]*vXs + Rimu[1]*vYs + Rimu[2]*vZs;
// 					vec1[1] = Rimu[3]*vXs + Rimu[4]*vYs + Rimu[5]*vZs;
// 					vec1[2] = Rimu[6]*vXs + Rimu[7]*vYs + Rimu[8]*vZs;

					_X = a1*(vec2[0] - vXs)+b1*(vec2[1] - vYs)+c1*(vec2[2] - vZs);
					_Y = a2*(vec2[0] - vXs)+b2*(vec2[1] - vYs)+c2*(vec2[2] - vZs);
					_Z = a3*(vec2[0] - vXs)+b3*(vec2[1] - vYs)+c3*(vec2[2] - vZs);


					x_appV = -vf*_X/_Z;
					y_appV = -vf*_Y/_Z;

					ptr4[0] = vx - x_appV;	//L
					ptr4[1] = vy - y_appV;

					//A
					double *jac = ptr3;
					jac[0] = vy*sinw-(vx/vf*(vx*cosk-vy*sink) + vf*cosk)*cosw;
					jac[1] = -vf*sink - vx/vf*(vx*sink + vy*cosk);
					jac[2] = vy;

					if(cnp == 6)
					{
						//dx/du
						jac[3] = 1/_Z*(vf*a1+vx*a3);
						//dx/dy
						jac[4] = 1/_Z*(vf*b1+vx*b3);
						//dx/dz
						jac[5] = 1/_Z*(vf*c1+vx*c3);
					}
					
					jac = ptr3 + cnp;
					jac[0] = -vx*sinw-(vy/vf*(vx*cosk-vy*sink) - vf*sink)*cosw;
					jac[1] = -vf*cosk-vy/vf*(vx*sink + vy*cosk);
					jac[2] = -vx;

					if(cnp==6)
					{
						//dy/du
						jac[3] = 1/_Z*(vf*a2+vy*a3);
						//dy/dy
						jac[4] = 1/_Z*(vf*b2+vy*b3);
						//dy/dz
						jac[5] = 1/_Z*(vf*c2+vy*c3);
					}
				}
			}

			double L2=0;
			for(i=0; i<nvis*2; i++)
			{
				L2 += L[i]*L[i];
			}


			dgelsy_driver(A, L, X, nvis*2, cnp, 1);

			double ple2=0;
			for(i=0; i<cnp; i++)
			{
				ple2 += X[i]*X[i];
				pUnknowns[i] += X[i];
			}

			if( ple2/cnp<1e-20 )
				break;

			_img_projs_IP(pUnknowns, &idxij, rcidxs, rcsubs, hx, adata); 

			p_eL2=nrmL2xmy(e, pObvs, hx, nobs); /* e=x-hx, p_eL2=||e|| */
		}

		// 空间前交，后交迭代检校
		_img_projs_IP(pUnknowns, &idxij, rcidxs, rcsubs, hx, adata); 

		p_eL2=nrmL2xmy(e, pObvs, hx, nobs); /* e=x-hx, p_eL2=||e|| */

		if(sqrt(p_eL2)/nvis < iterThresh)
			break;
	}

	//转换成标准BA坐标系下的检校结果
// 	double Rrst[9];
// 	double Rtmp[9];
// 
// 	matrixService->RotateMatrix_fwk(pUnknowns[0], pUnknowns[1], pUnknowns[2], Rmis);
// 
// 	double *pt = exor;	//线元素
// 	double *pqr = pt+3;		//角元素
// 	double Rimu[9];
// 
// 	matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);
// 
// 	matrix_transpose_product(3, 3, 3, 3, Rimu, Rmis, Rtmp);
// 	matrix_product(3, 3, 3, 3, Rtmp, Rimu, Rrst);
// 
// 	matrixService->R2fwk(Rrst, pUnknowns, pUnknowns+1, pUnknowns+2);

	if(cnp==6){
		pUnknowns[4] = -pUnknowns[4];
		pUnknowns[5] = -pUnknowns[5];
		pUnknowns[6] = -pUnknowns[6];
	}

	fprintf(fp, "result unknowns:\n");
	for(i=0; i<cnp; i++)
	{
		fprintf(fp, "%.6f ", pUnknowns[i]);
	}
	fprintf(fp, "\n");

	fprintf(fp, "initial reprojection errors: %.3f\n", sqrt(p_eL2)/nvis);

FLAG_END:

	if(e)	delete[] e;		e=NULL;
	if(hx)	delete[] hx;	hx=NULL;
	if(A)	delete[] A;		A=NULL;
	if(L)	delete[] L;		L=NULL;
	if(X)	delete[] X;		X=NULL;
	if(covar)	delete[] covar;		covar=NULL;
	if(rcidxs)	delete[] rcidxs;	rcidxs=NULL;
	if(rcsubs)	delete[] rcsubs;	rcsubs=NULL;
	return ret;
}

struct sba_crsm idxij;
int *rcidxs,  /* work array for the indexes corresponding to the nonzero elements of a single row or
			  column in a sparse matrix, size max(n, m) */
	*rcsubs;  /* work array for the subscripts of nonzero elements in a single row or column of a
				sparse matrix, size max(n, m) */


//计算虚拟影像上的像点坐标   //p:未知数， y计算的像点坐标，m未知数个数，n方程个数
void cal_VirtualImgPt(double *p, double *y, int m, int n, void *data)
{
	register int i, j;
	double *p3Dpts;

	imgCalib_globs *globs;
	globs = (imgCalib_globs*)data;

	double f = globs->camInfo->m_f;

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	double vphi, vomega, vkappa, vXs, vYs, vZs;

	if(m == 3)
	{
		vphi = p[0];
		vomega = p[1];
		vkappa = p[2];
		vXs = globs->calibParam->lever_arm[0];
		vYs = globs->calibParam->lever_arm[1];
		vZs = globs->calibParam->lever_arm[2];
	}
	else if(m == 6)
	{
		vphi = p[0];
		vomega = p[1];
		vkappa = p[2];
		vXs = p[3];
		vYs = p[4];
		vZs = p[5];
	}

	double Rmis[9], R[9];
	matrixService->RotateMatrix_fwk(vphi, vomega, vkappa, Rmis);
	double a1, a2, a3;
	double b1, b2, b3;
	double c1, c2, c3;

	int nFrame = globs->nimg;
	double *exor = globs->pImgExor;
	double *ptr1, *ptr2, *ptr3, *ptr4;

	//注意在LM调用中，传入的p只包含检校参数，物方点坐标需从原始的globs中读出
	p3Dpts = globs->pUnknowns + m; //物方点坐标和检校参数存放在一个数组中
	for(j=0; j<nFrame; ++j)
	{
		/* j-th camera exorrameters */
		double *pt = exor + j*6;	//线元素
		double *pqr = pt+3;		//角元素
		double Rimu[9];
		double Xgps, Ygps, Zgps;

		Xgps = pt[0];	Ygps = pt[1];	Zgps = pt[2];

		matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);
		matrix_product(3, 3, 3, 3, Rimu, Rmis, R); //R = Rimu*Rmis  与BA一致

		a1 = R[0];	a2 = R[1];	a3 = R[2];
		b1 = R[3];	b2 = R[4];	b3 = R[5];
		c1 = R[6];	c2 = R[7];	c3 = R[8];

		int nnz=sba_crsm_col_elmidxs(&idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */
		for(i=0; i<nnz; ++i)
		{//构造虚拟相片的Jac和L
			ptr1 = p3Dpts + rcsubs[i]*3;		//物方坐标
			ptr2 = y + idxij.val[rcidxs[i]]*2; // 像点坐标

			double _X, _Y, _Z;	//换元
			double uvw[3], vec1[3], vec2[3];

			uvw[0] = vXs;	uvw[1] = vYs;	uvw[2] = vZs;
			matrix_product(3, 3, 3, 1, Rimu, uvw, vec1);

			vec2[0] = ptr1[0] - Xgps - vec1[0];
			vec2[1] = ptr1[1] - Ygps - vec1[1];
			vec2[2] = ptr1[2] - Zgps - vec1[2];

			_X = a1*vec2[0] + b1*vec2[1] + c1*vec2[2];
			_Y = a2*vec2[0] + b2*vec2[1] + c2*vec2[2];
			_Z = a3*vec2[0] + b3*vec2[1] + c3*vec2[2];

			ptr2[0] = -f*_X/_Z;
			ptr2[1] = -f*_Y/_Z;
		}
	}

}

//虚拟影像上的像点对未知数的偏导数
//p未知数，m未知数个数，n方程个数
void jac_VirtualImgPt(double *p, double *jac, int m, int n, void *data)
{
	register int i, j;
	double *p3Dpts;

	imgCalib_globs *globs;
	globs = (imgCalib_globs*)data;

	double f = globs->camInfo->m_f;
	double *pObs = globs->pImgpts;

	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	double vphi, vomega, vkappa, vXs, vYs, vZs;

	if(m == 3)
	{
		vphi = p[0];
		vomega = p[1];
		vkappa = p[2];
		vXs = globs->calibParam->lever_arm[0];
		vYs = globs->calibParam->lever_arm[1];
		vZs = globs->calibParam->lever_arm[2];
	}
	else if(m == 6)
	{
		vphi = p[0];
		vomega = p[1];
		vkappa = p[2];
		vXs = p[3];
		vYs = p[4];
		vZs = p[5];
	}

	double Rmis[9];
	matrixService->RotateMatrix_fwk(vphi, vomega, vkappa, Rmis);
	double a1, a2, a3;
	double b1, b2, b3;
	double c1, c2, c3;

	a1 = Rmis[0];	a2 = Rmis[1];	a3 = Rmis[2];
	b1 = Rmis[3];	b2 = Rmis[4];	b3 = Rmis[5];
	c1 = Rmis[6];	c2 = Rmis[7];	c3 = Rmis[8];

	double sinf, cosf, sinw, cosw, sink, cosk;

	sinf = sin(vphi);	cosf = cos(vphi);
	sinw = sin(vomega);	cosw = cos(vomega);
	sink = sin(vkappa);	cosk = cos(vkappa);

	int nFrame = globs->nimg;
	double *exor = globs->pImgExor;
	double *ptr1, *ptr2, *ptr3, *ptr4;

	//注意在LM调用中，传入的p只包含检校参数，物方点坐标需从原始的globs中读出
	p3Dpts = globs->pUnknowns + m; //物方点坐标和检校参数存放在一个数组中
	for(j=0; j<nFrame; ++j)
	{
		/* j-th camera exorrameters */
		double *pt = exor + j*6;	//线元素
		double *pqr = pt+3;		//角元素
		double Rimu[9];
		double Xgps, Ygps, Zgps;

		Xgps = pt[0];	Ygps = pt[1];	Zgps = pt[2];

		matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);

		int nnz=sba_crsm_col_elmidxs(&idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */
		for(i=0; i<nnz; ++i)
		{//构造虚拟相片的Jac和L
			ptr1 = p3Dpts + rcsubs[i]*3;		//物方坐标
			ptr2 = pObs + idxij.val[rcidxs[i]]*2; //像点坐标
			ptr3 = jac + idxij.val[rcidxs[i]]*2*m; //偏导数
	
			double vx, vy, vf;	//虚拟影像的像点坐标
			double _X, _Y, _Z;	//换元
			double vec1[3], vec2[3];
			double x_appV, y_appV;	//近似值

			vx = ptr2[0];
			vy = ptr2[1];
			vf = f;

			vec1[0] = ptr1[0] - Xgps;
			vec1[1] = ptr1[1] - Ygps;
			vec1[2] = ptr1[2] - Zgps;

			matrix_transpose_product(3, 3, 3, 1, Rimu, vec1, vec2);

			_X = a1*(vec2[0] - vXs)+b1*(vec2[1] - vYs)+c1*(vec2[2] - vZs);
			_Y = a2*(vec2[0] - vXs)+b2*(vec2[1] - vYs)+c2*(vec2[2] - vZs);
			_Z = a3*(vec2[0] - vXs)+b3*(vec2[1] - vYs)+c3*(vec2[2] - vZs);

			ptr4 = ptr3;
			ptr4[0] = vy*sinw-(vx/vf*(vx*cosk-vy*sink) + vf*cosk)*cosw;
			ptr4[1] = -vf*sink - vx/vf*(vx*sink + vy*cosk);
			ptr4[2] = vy;

			if(m == 6)
			{
				//dx/du
				ptr4[3] = 1/_Z*(vf*a1+vx*a3);
				//dx/dy
				ptr4[4] = 1/_Z*(vf*b1+vx*b3);
				//dx/dz
				ptr4[5] = 1/_Z*(vf*c1+vx*c3);
			}

			ptr4 = ptr3 + m;
			ptr4[0] = -vx*sinw-(vy/vf*(vx*cosk-vy*sink) - vf*sink)*cosw;
			ptr4[1] = -vf*cosk-vy/vf*(vx*sink + vy*cosk);
			ptr4[2] = -vx;

			if(m==6)
			{
				//dy/du
				ptr4[3] = 1/_Z*(vf*a2+vy*a3);
				//dy/dy
				ptr4[4] = 1/_Z*(vf*b2+vy*b3);
				//dy/dz
				ptr4[5] = 1/_Z*(vf*c2+vy*c3);
			}
		}

	}
}

int orsAlgCameraCalib::calib_IP_LM( void *adata )
{
	int ret=0;
	int maxIter0 = 300, maxIter1 = 1;

	register int i, j, ii, jj, k, l;
	int nvis, nnz, retval;

	register double *p3Dpts, *ptr1, *ptr2, *ptr3, *ptr4, sum;

	int nobs, nvars;
	int m, n;
	int cnp, pnp, mnp;
	double *pUnknowns, *pObvs;

	imgCalib_globs *globs;
	globs = (imgCalib_globs*)adata;

	char *vmask = globs->mask;
	m = globs->nimg; 
	n = globs->ntie;
	cnp = globs->cnp;	//相片未知数个数
	pnp = globs->pnp;	//物方点未知数个数
	mnp = globs->mnp;	

	pUnknowns = globs->pUnknowns;
	pObvs = globs->pImgpts;

	/* count total number of visible image points */
	for(i=nvis=0, jj=n*m; i<jj; ++i)
		nvis+=(vmask[i]!=0);

	nobs=nvis*2;	//观测值
//	nvars=nVFrame*cnp + n*pnp;	//未知数

	double iterThresh = globs->camInfo->m_pixelX*0.2;

	FILE *fp = globs->fplog;
	if(n < 4){
		fprintf(fp, "cannot solve a problem with fewer tie points [%d] than 4\n", n);
		return SBA_ERROR;
	}
	ref_ptr<orsIMatrixService> matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

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

	fprintf(fp, "tiePoint: %d nProjs: %d\n", n, nvis);

	//相片上的最大像点数不超过n
	rcidxs = new int[n];
	rcsubs = new int[n];

	double projTh= globs->camInfo->m_pixelX * 1; //重投影误差阈值
	int iter0, iter1;
	double p_eL2;
	double min_eL2=1.7e+308;
	double *exor = globs->pImgExor;

	double Rmis[9];
// 	double leverarm[3], translate[3];
// 	leverarm[0]=imgMisalign.u;
// 	leverarm[1]=imgMisalign.v;
// 	leverarm[2]=imgMisalign.w;

//	RotateMat_fwk(imgMisalign.phi, imgMisalign.omega, imgMisalign.kappa, Rmis);

	//时间延迟解算
	double delay;	//统一时间延迟
	double minTime=0, maxTime=150*1e-6;	//0, 150microsecond
	double interval = 2*1e-6;
	double best_delay;
	
	double *e=new double[nobs];
	double *hx=new double[nobs];
	//虚拟相片上后方交会求解未知数所需要的矩阵
// 	double *A = new double[2*cnp*nvis];
// 	double *L = new double[2*nvis];
//	double *X = new double[cnp];
	double *covar = new double[cnp*cnp];

	_img_projs_IP(pUnknowns, &idxij, rcidxs, rcsubs, hx, adata); 

	p_eL2=nrmL2xmy(e, pObvs, hx, nobs); /* e=x-hx, p_eL2=||e|| */

	fprintf(fp, "initial unknowns:\n");
	for(i=0; i<cnp; i++)
	{
		fprintf(fp, "%.6f ", pUnknowns[i]);
	}
	fprintf(fp, "\n");

	fprintf(fp, "initial reprojection errors: %.3f\n", sqrt(p_eL2)/nvis);


	p3Dpts = pUnknowns + cnp;
	//注意坐标系的定义与BA中不同
	//BA:	
	//	R = Rimu*Rmis
	//  [Xs,Ys,Zs]^T = [Xgps,Ygps,Zgps]^T - Rimu[u,v,w]^T
	//IP:
	//	R = Rmis*Rimu
	//  [Xs,Ys,Zs]^T = [Xgps,Ygps,Zgps]^T + Rimu[u,v,w]^T
	double vphi, vomega, vkappa;	//虚拟影像的角元素
	double vXs, vYs, vZs;			//虚拟影像的线元素
	for( iter0=0; iter0<maxIter0; iter0++ )
	{
		int nxAngles = 3;	// max: 3
		int nxXyz = 2;		// max: 3
		int nxPrinciplePoint=0;	// max:2
		int nxPrincipleLength=0;	// max:1
		int nxRadiusDistorions = 0;	// max:4
		int nxTangentDistorions = 0;// max:2

		if( iter0 > 20 )	{
			nxPrinciplePoint=2;	
			//nxPrincipleLength =1;
			nxRadiusDistorions = 4;
		}

		int ixXyz = nxAngles;
		int ixPP = ixXyz + nxXyz;
		int ixPL = ixPP + nxPrinciplePoint;
		int ixRD= ixPL + nxPrincipleLength;
		int ixTD = ixRD + nxRadiusDistorions;
		int nx = ixTD + nxTangentDistorions;

		_img_intersection_IP( pUnknowns, &idxij, rcidxs, rcsubs, pObvs, adata);		//前交得到3D点

		double f = globs->camInfo->m_f;
		int nFrame=globs->nimg;
		int n3Dpoint = globs->ntie;
		
		double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
		opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;

		int ret=dlevmar_der(cal_VirtualImgPt,	//计算L
			jac_VirtualImgPt, //计算A
			pUnknowns, pObvs, cnp, nobs, 200, opts, info, NULL, covar, adata);

		double rms = sqrt(info[1]/(nobs - cnp));

		// 空间前交，后交迭代检校
		_img_projs_IP(pUnknowns, &idxij, rcidxs, rcsubs, hx, adata); 

		p_eL2=nrmL2xmy(e, pObvs, hx, nobs); /* e=x-hx, p_eL2=||e|| */

		if(sqrt(p_eL2)/nvis < iterThresh)
			break;
	}

	//转换成标准BA坐标系下的检校结果
	double Rrst[9];
	double Rtmp[9];

	matrixService->RotateMatrix_fwk(pUnknowns[0], pUnknowns[1], pUnknowns[2], Rmis);

	double *pt = exor;	//线元素
	double *pqr = pt+3;		//角元素
	double Rimu[9];

	matrixService->RotateMatrix_fwk(pqr[0], pqr[1], pqr[2], Rimu);

	matrix_transpose_product(3, 3, 3, 3, Rimu, Rmis, Rtmp);
	matrix_product(3, 3, 3, 3, Rtmp, Rimu, Rrst);

	matrixService->R2fwk(Rrst, pUnknowns, pUnknowns+1, pUnknowns+2);

	if(cnp==6){
		pUnknowns[4] = -pUnknowns[4];
		pUnknowns[5] = -pUnknowns[5];
		pUnknowns[6] = -pUnknowns[6];
	}

	fprintf(fp, "result unknowns:\n");
	for(i=0; i<cnp; i++)
	{
		fprintf(fp, "%.6f ", pUnknowns[i]);
	}
	fprintf(fp, "\n");

	fprintf(fp, "initial reprojection errors: %.3f\n", sqrt(p_eL2)/nvis);

FLAG_END:

	if(e)	delete[] e;		e=NULL;
	if(hx)	delete[] hx;	hx=NULL;
// 	if(A)	delete[] A;		A=NULL;
// 	if(L)	delete[] L;		L=NULL;
// 	if(X)	delete[] X;		X=NULL;
	if(covar)	delete[] covar;		covar=NULL;
	if(rcidxs)	delete[] rcidxs;	rcidxs=NULL;
	if(rcsubs)	delete[] rcsubs;	rcsubs=NULL;
	return ret;
}