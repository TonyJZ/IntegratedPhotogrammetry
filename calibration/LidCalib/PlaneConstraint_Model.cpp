#include "StdAfx.h"
#include "LidCalib\PlaneConstraint_Model.h"
#include "bundler\matrix.h"
#include "math.h"
//#include "LidarGeometry.h"
//#include "levmar.h"
#include "orsBase/orsIPlatform.h"
#include "orsLidarGeometry/orsILidarGeoModel.h"

#include "F:\OpenRS\IntegratedPhotogrammetry\external\levmar-2.5\levmar.h"


static int CountInliers(LidPt_SurvInfo *pPtsObvs, int ptNum, double *M,  
						std::vector<int> &inliers, double &rms)
						//						const std::vector<Keypoint> &k1, 
						// 						const std::vector<Keypoint> &k2, 
						// 						std::vector<KeypointMatch> matches,
						// 						double *M, double thresh, std::vector<int> &inliers)
{
	int i;
	double p[3];
	double q;
	double dist, dist2;

	inliers.clear();
	int count = 0;


	rms=0;
	for (i = 0; i < ptNum; i++)
	{
		p[0] = pPtsObvs[i].x;
		p[1] = pPtsObvs[i].y;
		p[2] = 1.0;


		//matrix_product(1, 3, 3, 1, p, M, q);
		q=p[0]*M[0]+p[1]*M[1]+p[2]*M[2];

		dist2 = q-pPtsObvs[i].z;
		dist2*=dist2;

		rms+=dist2;
	}
	rms=sqrt(rms/ptNum);

	double outlierthresh=1.5*rms;
	for (i = 0; i < ptNum; i++)
	{
		p[0] = pPtsObvs[i].x;
		p[1] = pPtsObvs[i].y;
		p[2] = 1.0;

		//matrix_product(3, 3, 3, 1, M, p, q);

		q=p[0]*M[0]+p[1]*M[1]+p[2]*M[2];
		dist = fabs(q-pPtsObvs[i].z);

		if (dist <= outlierthresh) 
		{
			count++;
			inliers.push_back(i);
		}
	}

	return count;
}

static int CountInliers(std::vector<LidPt_SurvInfo> *pPtsObvs, int ptNum, double *M,  
						std::vector<int> &inliers, double &rms)
//						const std::vector<Keypoint> &k1, 
// 						const std::vector<Keypoint> &k2, 
// 						std::vector<KeypointMatch> matches,
// 						double *M, double thresh, std::vector<int> &inliers)
{
	int i;
	double p[3];
	double q;
	double dist, dist2;

	inliers.clear();
	int count = 0;
	

	rms=0;
	for (i = 0; i < ptNum; i++)
	{
		p[0] = (*pPtsObvs)[i].x;
		p[1] = (*pPtsObvs)[i].y;
		p[2] = 1.0;

		
		//matrix_product(1, 3, 3, 1, p, M, q);
		q=p[0]*M[0]+p[1]*M[1]+p[2]*M[2];

		dist2 = q-(*pPtsObvs)[i].z;
		dist2*=dist2;

		rms+=dist2;
	}
	rms=sqrt(rms/ptNum);

	double outlierthresh=1.5*rms;
	for (i = 0; i < ptNum; i++)
	{
		p[0] = (*pPtsObvs)[i].x;
		p[1] = (*pPtsObvs)[i].y;
		p[2] = 1.0;

		//matrix_product(3, 3, 3, 1, M, p, q);

		q=p[0]*M[0]+p[1]*M[1]+p[2]*M[2];
		dist = fabs(q-(*pPtsObvs)[i].z);
		
		if (dist <= outlierthresh) 
		{
			count++;
			inliers.push_back(i);
		}
	}

	return count;
}

void plane_fitting(int num_pts, POINT3D *pts, double *Tout) 
{
	int m = num_pts;
	int n = 3;
	int nrhs = 1;
	int i, base;
	
	double *A = (double *)malloc(sizeof(double) * m * n);    /* Left-hand matrix */
	double *B = (double *)malloc(sizeof(double) * m ); /* Right-hand matrix */
	
//	double Ttmp[9];
//	double T1[9], T2[9];

	
	for (i = 0; i < num_pts; i++) 
	{
		base = i * n;
		A[base + 0] = pts[i].X;
		A[base + 1] = pts[i].Y;
		A[base + 2] = 1.0;
				
		B[i] = pts[i].Z;
		
	}
	
	/* Make the call to dgelsy */
	dgelsy_driver(A, B, Tout, m, n, nrhs);
	
	free(A);
	free(B);
}


//每个条带单独进行ransac挑点，避免安置误差造成不共面
int PlaneFitting_ransac(LidMC_Plane *pLidPatch)
{
	ASSERT(pLidPatch);

	int min_matches = 4;
	int ptNum;

	int max_inliers = 0;
	int match_idxs[4];
	double Mbest[3];
	double RANSACthresh=6.0*0.012;
	
	std::vector<LidPt_SurvInfo> *pLidPoints=NULL;
	std::vector<LidPt_SurvInfo> *pInliers=NULL;

	pLidPoints=pLidPatch->pLidPoints;
	ptNum=pLidPoints->size();
	if (ptNum < min_matches)
	{
		printf("Cannot estimate transform, point num is not enough\n");
		return -1;
	}

	//v3_t *r_pts = new v3_t[min_matches];
	//v3_t *l_pts = new v3_t[min_matches];
	POINT3D pts[4];
//	v3_t  l_pts[4];
	double weight[4];
	//	int ngcp;

	int i;
	std::vector<int> inliers;
	double rms;
	
	int nRANSAC=512;
	double min_rms=1.7e+308;
	for (int round = 0; round < nRANSAC; round++)
	{
		for (i = 0; i < min_matches; i++) 
		{
			bool found;
			int idx;

			do 
			{
				found = true;
				idx = rand() % ptNum;

				for (int j = 0; j < i; j++) 
				{
					if (match_idxs[j] == idx) 
					{
						found = false;
						break;
					}
				}
			} while (!found);

			match_idxs[i] = idx;
		}

		/* Solve for the motion */

		for (int i = 0; i < min_matches; i++) 
		{
			int idx=match_idxs[i];

			pts[i].X=(*pLidPoints)[idx].x;
			pts[i].Y=(*pLidPoints)[idx].y;
			pts[i].Z=(*pLidPoints)[idx].z;
			
			weight[i] = 1.0;
		}

		double Mcurr[3];

		plane_fitting(min_matches, pts, Mcurr);


		int num_inliers = CountInliers(pLidPoints, ptNum,  Mcurr, inliers, rms);

		if(rms<min_rms)
		{
			max_inliers = num_inliers;
			memcpy(Mbest, Mcurr, 3 * sizeof(double));
			min_rms=rms;
		}

	}
	
//	double rms;
//	std::vector<int> inliers;
	CountInliers(pLidPoints, ptNum,  Mbest, inliers, rms);

	pInliers=new std::vector<LidPt_SurvInfo>;
	for(i=0; i<inliers.size(); i++)
	{
		int j=inliers[i];
		pInliers->push_back((*pLidPoints)[j]);   //滤除非平面点
	}

	delete pLidPoints;
	pLidPatch->pLidPoints=pInliers;
	pLidPatch->s1=Mbest[0];
	pLidPatch->s2=Mbest[1];
	pLidPatch->s3=-1;
	pLidPatch->s4=Mbest[2];

	return 0;
}

//观测值
struct myObs
{
	int nPoints;			//总点数
	int nMisalign;			//安置参数个数
	CalibParam_Type type;	//检校参数类型
	LidPt_SurvInfo *pLidPoints;
	
	double *pPDC;		//x, y, z对安置参数的偏导数
	int ndim;			//维数  xp, yp, zp
	
	int nTiePlane;		//连接平面数
	long *pPlaneLut;			//点与平面对应的查找表

	int nTieVP;			//虚拟连接点数			
	long *pVPLut;	//真实点与虚拟点对应的查找表

	double tx, ty, tz;  //偏心分量

	double *hx, *hxx;	//为差分准备的空间

	orsLidSysParam oriParam;	//系统误差初值
	
	orsILidarGeoModel *pLidGeo;  //lidar几何模型
};

//p:未知数， y计算的距离值，m未知数个数，n方程个数
// void distance_PointToPlane(double *p, double *y, int m, int n, void *data)
// {
// 	register int i, j;
// 	struct myObs *dptr;
// 	ATNPoint *pLidPoints;
// 	double      *pMisalign;
// 	int nPoints, nTiePlane;
// 	double a, b, c, d, s;
// 	double xp, yp, zp;
// 	double *pPlane;
// 	long *pPlaneLut;
// 
// 	dptr=(struct myObs *)data; 
// 	pLidPoints=dptr->pLidPoints;
// 	pMisalign=p;
// 	nPoints=dptr->nPoints;
// 	nTiePlane=dptr->nTiePlane;
// 
// 	pPlane=p+dptr->nMisalign;
// 	pPlaneLut=dptr->pPlaneLut;
// 
// 	//重新计算激光点坐标
// 	_calLaserPosition(pLidPoints, nPoints, dptr->oriParam, p, dptr->type);
// 
// 	for(i=0; i<nTiePlane; i++)
// 	{//遍历平面
// 		a=pPlane[i*4];
// 		b=pPlane[i*4+1];
// 		c=pPlane[i*4+2];
// 		d=pPlane[i*4+3];
// 		s=1.0/sqrt(a*a+b*b+c*c);
// 		
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			xp=pLidPoints[j].x;
// 			yp=pLidPoints[j].y;
// 			zp=pLidPoints[j].z;
// 			
// 			y[j]=(a*xp+b*yp+c*zp+d)*s;
// 		}
// 	}
// }

// void distance_PointToPlane_rph(double *p, double *y, int m, int n, void *data)
// {
// 	register int i, j;
// 	struct myObs *dptr;
// 	ATNPoint *pLidPoints;
// 	double      *pMisalign;
// 	int nPoints, nTiePlane;
// 	double a, b, c, d, s;
// 	double xp, yp, zp;
// 	double *pPlane;
// 	long *pPlaneLut;
// 	double tx, ty, tz;
// 
// 	dptr=(struct myObs *)data; 
// 	pLidPoints=dptr->pLidPoints;
// 	pMisalign=p;
// 	nPoints=dptr->nPoints;
// 	nTiePlane=dptr->nTiePlane;
// 
// 	pPlane=p+dptr->nMisalign;
// 	pPlaneLut=dptr->pPlaneLut;
// 	tx=dptr->tx;
// 	ty=dptr->ty;
// 	tz=dptr->tz;
// 
// 	//重新计算激光点坐标
// 	_calLaserPosition(pLidPoints, nPoints, tx, ty, tz, pMisalign[0], pMisalign[1], pMisalign[2]);
// 
// 	for(i=0; i<nTiePlane; i++)
// 	{//遍历平面
// 		a=pPlane[i*4];
// 		b=pPlane[i*4+1];
// 		c=pPlane[i*4+2];
// 		d=pPlane[i*4+3];
// 		s=1.0/sqrt(a*a+b*b+c*c);
// 
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			xp=pLidPoints[j].x;
// 			yp=pLidPoints[j].y;
// 			zp=pLidPoints[j].z;
// 
// 			y[j]=(a*xp+b*yp+c*zp+d)*s;
// 		}
// 	}
// }

//p:未知数， y计算的距离值，m未知数个数，n方程个数
//2013.1.4  自动根据检校参数类型计算点面距离
void distance_PointToPlane_Skaloud(double *p, double *y, int m, int n, void *data)
{
	register int i, j;
	struct myObs *dptr;
	LidPt_SurvInfo *pLidPoints;
	double      *pMisalign;
	int nPoints, nTiePlane;
	double a, b, c, d, s;
	double xp, yp, zp;
	double *pPlane;
	long *pPlaneLut;
	orsILidarGeoModel *pLidGeo;
	orsLidSysParam  curParam;
	

	dptr=(struct myObs *)data; 
	pLidPoints=dptr->pLidPoints;
	pMisalign=p;
	nPoints=dptr->nPoints;
	nTiePlane=dptr->nTiePlane;

	pPlane=p+dptr->nMisalign;
	pPlaneLut=dptr->pPlaneLut;

	pLidGeo = dptr->pLidGeo;

	//重新计算激光点坐标
//	_calLaserPosition(pLidPoints, nPoints, dptr->oriParam, p, dptr->type);

	curParam = dptr->oriParam;
	switch (dptr->type)
	{
	case Calib_rph:
		curParam.boresight_angle[0] = p[0];
		curParam.boresight_angle[1] = p[1];
		curParam.boresight_angle[2] = p[2];
		break;
	case Calib_rphxyz:
		curParam.boresight_angle[0] = p[0];
		curParam.boresight_angle[1] = p[1];
		curParam.boresight_angle[2] = p[2];
		curParam.lever_arm[0] = p[3];
		curParam.lever_arm[1] = p[4];
		curParam.lever_arm[2] = p[5];
		break;
	case Calib_rph_appXYZ:
		curParam.boresight_angle[0] = p[0];
		curParam.boresight_angle[1] = p[1];
		curParam.boresight_angle[2] = p[2];
		curParam.semiXYZ[0] = p[3];
		curParam.semiXYZ[1] = p[4];
		curParam.semiXYZ[2] = p[5];
		break;
	case Calib_rphxyz_sa0:
		curParam.boresight_angle[0] = p[0];
		curParam.boresight_angle[1] = p[1];
		curParam.boresight_angle[2] = p[2];
		curParam.lever_arm[0] = p[3];
		curParam.lever_arm[1] = p[4];
		curParam.lever_arm[2] = p[5];
		curParam.sa0 = p[6];
		break;
	case Calib_rphxyz_sa0_sac:
		curParam.boresight_angle[0] = p[0];
		curParam.boresight_angle[1] = p[1];
		curParam.boresight_angle[2] = p[2];
		curParam.lever_arm[0] = p[3];
		curParam.lever_arm[1] = p[4];
		curParam.lever_arm[2] = p[5];
		curParam.sa0 = p[6];
		curParam.sac = p[7];
		break;
	case Calib_rph_sa0:
		curParam.boresight_angle[0] = p[0];
		curParam.boresight_angle[1] = p[1];
		curParam.boresight_angle[2] = p[2];
		curParam.sa0 = p[3];
		break;
	case Calib_rph_drange:
		curParam.boresight_angle[0] = p[0];
		curParam.boresight_angle[1] = p[1];
		curParam.boresight_angle[2] = p[2];
		curParam.drange = p[3];
		break;
	case Calib_rph_drange_sa0:
		curParam.boresight_angle[0] = p[0];
		curParam.boresight_angle[1] = p[1];
		curParam.boresight_angle[2] = p[2];
		curParam.drange = p[3];
		curParam.sa0 = p[4];
		break;
	default:
		;
	}
	pLidGeo->setSysParam((void*)&curParam);
	pLidGeo->cal_Laserfootprint(pLidPoints, nPoints);

	for(i=0; i<nTiePlane; i++)
	{//遍历平面
		a=pPlane[i*4];
		b=pPlane[i*4+1];
		c=pPlane[i*4+2];
		d=pPlane[i*4+3];
		s=1.0/sqrt(a*a+b*b+c*c);

		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			xp=pLidPoints[j].x;
			yp=pLidPoints[j].y;
			zp=pLidPoints[j].z;

			y[j]=(a*xp+b*yp+c*zp+d)*s;
		}
	}
}

//计算观测值
// void calObserves_PointToPlane_ZConstraint(double *p, double *y, int m, int n, void *data)
// {
// 	register int i, j;
// 	struct myObs *dptr;
// 	ATNPoint /**pLidPlane, */*pLidPoints;
// 	double      *pMisalign;
// 	int nPoints, nTiePlane, nTieVP;
// 	double a, b, c, d, s;
// 	double xp, yp, zp;
// 	double *pPlane;
// 	long *pPlaneLut, *pVPLut;
// 
// 
// 	dptr=(struct myObs *)data; 
// 	pLidPoints=dptr->pLidPoints;
// 	pMisalign=p;
// 	nPoints=dptr->nPoints;
// 	nTiePlane=dptr->nTiePlane;
// 	nTieVP=dptr->nTieVP;
// 
// 	pPlane=p+dptr->nMisalign;
// 	pPlaneLut=dptr->pPlaneLut;
// 	pVPLut=dptr->pVPLut;
// 
// 	//重新计算激光点坐标
// 	_calLaserPosition(pLidPoints, nPoints, pMisalign[0], pMisalign[1], pMisalign[2], pMisalign[3], pMisalign[4], pMisalign[5]);
// 
// 	for(i=0; i<nTieVP; i++)
// 	{
// 		for(j=pVPLut[i]; j<pVPLut[i+1]; j++)
// 		{
// 			y[j]=pLidPoints[j].z;
// 		}
// 	}
// 
// //	pLidPlane=pLidPoints+pVPLut[nTieVP];
// 	for(i=0; i<nTiePlane; i++)
// 	{//遍历平面
// 		a=pPlane[i*4];
// 		b=pPlane[i*4+1];
// 		c=pPlane[i*4+2];
// 		d=pPlane[i*4+3];
// 		s=1.0/sqrt(a*a+b*b+c*c);
// 
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			xp=pLidPoints[j].x;
// 			yp=pLidPoints[j].y;
// 			zp=pLidPoints[j].z;
// 
// 			y[j]=(a*xp+b*yp+c*zp+d)*s;
// 		}
// 	}
// }

//差分方式求导
void jac_dis_PointToPlane_diff(double *p, double *jac, int m, int n, void *data)
{
	register int i, j;
	struct myObs *dptr;
	LidPt_SurvInfo *pLidPoints;
	double      *pMisalign;
	int nPoints, nTiePlane;
// 	double a, b, c, d, s;
// 	double a2, b2, c2, s3;
// 	double xp, yp, zp;
// 	double *pPlane;
// 	long *pPlaneLut;
	int pnp, ndim;
	double *pPDC;
	double *pdX, *pdY, *pdZ;
	int ncol;	//未知数个数
	double *hx, *hxx;

	dptr=(struct myObs *)data;
	pLidPoints=dptr->pLidPoints;
	pMisalign=p;
	nPoints=dptr->nPoints;
	nTiePlane=dptr->nTiePlane;

// 	pPlane=p+dptr->nMisalign;
// 	pPlaneLut=dptr->pPlaneLut;
	pnp=dptr->nMisalign;
	ndim=dptr->ndim;
	pPDC=dptr->pPDC;
	hx = dptr->hx;
	hxx = dptr->hxx;

	ncol=pnp+4*nTiePlane;

	
	//计算当前未知数条件下的方程观测值
	distance_PointToPlane_Skaloud(p, hx, ncol, nPoints, data);

	double d, tmp;
	double delta=1E-06;
	for(j=0; j<ncol; ++j)
	{
		/* determine d=max(1E-04*|p[j]|, delta), see HZ */
		d=(1E-04)*p[j]; // force evaluation
		d=fabs(d);
		if(d<delta)
			d=delta;

		tmp=p[j];
		p[j]+=d;

		distance_PointToPlane_Skaloud(p, hxx, ncol, nPoints, data);

		p[j]=tmp; /* restore */

		d=(1.0)/d; /* invert so that divisions can be carried out faster as multiplications */
		for(i=0; i<nPoints; ++i)
		{
			jac[i*ncol+j]=(hxx[i]-hx[i])*d;
		}
	}
}

//计算线性化后的观测方程参数
// void jac_dis_PointToPlane(double *p, double *jac, int m, int n, void *data)
// {
// 	register int i, j;
// 	struct myObs *dptr;
// 	LidPt_SurvInfo *pLidPoints;
// 	double      *pMisalign;
// 	int nPoints, nTiePlane;
// 	double a, b, c, d, s;
// 	double a2, b2, c2, s3;
// 	double xp, yp, zp;
// 	double *pPlane;
// 	long *pPlaneLut;
// 	int pnp, ndim;
// 	double *pPDC;
// 	double *pdX, *pdY, *pdZ;
// 	int ncol;
// 	
// 	dptr=(struct myObs *)data;
// 	pLidPoints=dptr->pLidPoints;
// 	pMisalign=p;
// 	nPoints=dptr->nPoints;
// 	nTiePlane=dptr->nTiePlane;
// 	
// 	pPlane=p+dptr->nMisalign;
// 	pPlaneLut=dptr->pPlaneLut;
// 	pnp=dptr->nMisalign;
// 	ndim=dptr->ndim;
// 	pPDC=dptr->pPDC;
// 
// 	_calPDC_misalign(pLidPoints, nPoints, pMisalign+3, pPDC, ndim, pnp);
// 
// 	ncol=pnp+4*nTiePlane;
// 	memset(jac, 0, sizeof(double)*nPoints*ncol);
// 	//计算 B, C, L
// 	for(i=0; i<nTiePlane; i++)
// 	{//遍历平面
// 		a=pPlane[i*4];
// 		b=pPlane[i*4+1];
// 		c=pPlane[i*4+2];
// 		d=pPlane[i*4+3];
// 		
// 		a2=a*a;
// 		b2=b*b;
// 		c2=c*c;
// 		
// 		s=1.0/sqrt(a*a+b*b+c*c);
// 		s3=s*s*s;
// 		
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			pdX=pPDC+j*ndim*pnp;
// 			pdY=pdX+pnp;
// 			pdZ=pdY+pnp;
// 			
// 			xp=pLidPoints[j].x;
// 			yp=pLidPoints[j].y;
// 			zp=pLidPoints[j].z;
// 			
// 			//点
// 			jac[j*ncol]  =s*(a*pdX[0]+b*pdY[0]+c*pdZ[0]);
// 			jac[j*ncol+1]=s*(a*pdX[1]+b*pdY[1]+c*pdZ[1]);
// 			jac[j*ncol+2]=s*(a*pdX[2]+b*pdY[2]+c*pdZ[2]);
// 			jac[j*ncol+3]=s*(a*pdX[3]+b*pdY[3]+c*pdZ[3]);
// 			jac[j*ncol+4]=s*(a*pdX[4]+b*pdY[4]+c*pdZ[4]);
// 			jac[j*ncol+5]=s*(a*pdX[5]+b*pdY[5]+c*pdZ[5]);
// 			
// 			//平面
// 			jac[j*ncol+pnp+i*4]  =(xp*(b2+c2)-a*(b*yp+c*zp+d))*s3;
// 			jac[j*ncol+pnp+i*4+1]=(yp*(a2+c2)-b*(a*xp+c*zp+d))*s3;
// 			jac[j*ncol+pnp+i*4+2]=(zp*(a2+b2)-c*(a*xp+b*yp+d))*s3;
// 			jac[j*ncol+pnp+i*4+3]=s;
// 		}
// 	}
// }

// void jac_dis_PointToPlane_rph(double *p, double *jac, int m, int n, void *data)
// {
// 	register int i, j;
// 	struct myObs *dptr;
// 	LidPt_SurvInfo *pLidPoints;
// 	double      *pMisalign;
// 	int nPoints, nTiePlane;
// 	double a, b, c, d, s;
// 	double a2, b2, c2, s3;
// 	double xp, yp, zp;
// 	double *pPlane;
// 	long *pPlaneLut;
// 	int pnp, ndim;
// 	double *pPDC;
// 	double *pdX, *pdY, *pdZ;
// 	int ncol;
// 	double tx, ty, tz;
// 
// 	dptr=(struct myObs *)data;
// 	pLidPoints=dptr->pLidPoints;
// 	pMisalign=p;
// 	nPoints=dptr->nPoints;
// 	nTiePlane=dptr->nTiePlane;
// 
// 	pPlane=p+dptr->nMisalign;
// 	pPlaneLut=dptr->pPlaneLut;
// 	pnp=dptr->nMisalign;
// 	ndim=dptr->ndim;
// 	pPDC=dptr->pPDC;
// 
// 	_calPDC_misalign(pLidPoints, nPoints, pMisalign, pPDC, ndim, 6);
// 
// 	ncol=pnp+4*nTiePlane;
// 	memset(jac, 0, sizeof(double)*nPoints*ncol);
// 	//计算 B, C, L
// 	for(i=0; i<nTiePlane; i++)
// 	{//遍历平面
// 		a=pPlane[i*4];
// 		b=pPlane[i*4+1];
// 		c=pPlane[i*4+2];
// 		d=pPlane[i*4+3];
// 
// 		a2=a*a;
// 		b2=b*b;
// 		c2=c*c;
// 
// 		s=1.0/sqrt(a*a+b*b+c*c);
// 		s3=s*s*s;
// 
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			pdX=pPDC+j*ndim*6;
// 			pdY=pdX+6;
// 			pdZ=pdY+6;
// 
// 			xp=pLidPoints[j].x;
// 			yp=pLidPoints[j].y;
// 			zp=pLidPoints[j].z;
// 
// 			//点
// //			jac[j*ncol]  =s*(a*pdX[0]+b*pdY[0]+c*pdZ[0]);
// //			jac[j*ncol+1]=s*(a*pdX[1]+b*pdY[1]+c*pdZ[1]);
// //			jac[j*ncol+2]=s*(a*pdX[2]+b*pdY[2]+c*pdZ[2]);
// 			jac[j*ncol+0]=s*(a*pdX[3]+b*pdY[3]+c*pdZ[3]);
// 			jac[j*ncol+1]=s*(a*pdX[4]+b*pdY[4]+c*pdZ[4]);
// 			jac[j*ncol+2]=s*(a*pdX[5]+b*pdY[5]+c*pdZ[5]);
// 
// 			//平面
// 			jac[j*ncol+pnp+i*4]  =(xp*(b2+c2)-a*(b*yp+c*zp+d))*s3;
// 			jac[j*ncol+pnp+i*4+1]=(yp*(a2+c2)-b*(a*xp+c*zp+d))*s3;
// 			jac[j*ncol+pnp+i*4+2]=(zp*(a2+b2)-c*(a*xp+b*yp+d))*s3;
// 			jac[j*ncol+pnp+i*4+3]=s;
// 		}
// 	}
// }

// void jac_dis_PointToPlane_ZConstraint(double *p, double *jac, int m, int n, void *data)
// {
// 	register int i, j;
// 	struct myObs *dptr;
// 	LidPt_SurvInfo *pLidPoints/*, *pLidPlanePt*/;
// 	double      *pMisalign;
// 	int nPoints, nTiePlane, nTieVP;
// 	double a, b, c, d, s;
// 	double a2, b2, c2, s3;
// 	double xp, yp, zp;
// 	double *pPlane;
// 	long *pPlaneLut, *pVPLut;
// 	int pnp, ndim;
// 	double *pPDC;
// 	double *pdX, *pdY, *pdZ;
// 	int ncol;
// 
// 	dptr=(struct myObs *)data;
// 	pLidPoints=dptr->pLidPoints;
// 	pMisalign=p;
// 	nPoints=dptr->nPoints;
// 	nTiePlane=dptr->nTiePlane;
// 
// 	pPlane=p+dptr->nMisalign;
// 	pPlaneLut=dptr->pPlaneLut;
// 	pVPLut=dptr->pVPLut;
// 	pnp=dptr->nMisalign;
// 	ndim=dptr->ndim;
// 	pPDC=dptr->pPDC;
// 	nTieVP=dptr->nTieVP;
// 
// //	pLidPlanePt=pLidPoints+pVPLut[nTieVP];
// 
// 	_calPDC_misalign(pLidPoints, nPoints, pMisalign+3, pPDC, ndim, pnp);
// 
// 	ncol=pnp+4*nTiePlane;
// 	memset(jac, 0, sizeof(double)*nPoints*ncol);
// 
// 	for(i=0; i<nTieVP; i++)
// 	{
// 		for(j=pVPLut[i]; j<pVPLut[i+1]; j++)
// 		{
// 			pdX=pPDC+j*ndim*pnp;
// 			pdY=pdX+pnp;
// 			pdZ=pdY+pnp;
// 
// 			//点  dz
// 			jac[j*ncol]   =pdZ[0];
// 			jac[j*ncol+1]=pdZ[1];
// 			jac[j*ncol+2]=pdZ[2];
// 			jac[j*ncol+3]=pdZ[3];
// 			jac[j*ncol+4]=pdZ[4];
// 			jac[j*ncol+5]=pdZ[5];
// 		}
// 	}
// 
// 	//计算 B, C, L
// 	for(i=0; i<nTiePlane; i++)
// 	{//遍历平面
// 		a=pPlane[i*4];
// 		b=pPlane[i*4+1];
// 		c=pPlane[i*4+2];
// 		d=pPlane[i*4+3];
// 
// 		a2=a*a;
// 		b2=b*b;
// 		c2=c*c;
// 
// 		s=1.0/sqrt(a*a+b*b+c*c);
// 		s3=s*s*s;
// 
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			pdX=pPDC+j*ndim*pnp;
// 			pdY=pdX+pnp;
// 			pdZ=pdY+pnp;
// 
// 			xp=pLidPoints[j].x;
// 			yp=pLidPoints[j].y;
// 			zp=pLidPoints[j].z;
// 
// 			//点
// 			jac[j*ncol]  =s*(a*pdX[0]+b*pdY[0]+c*pdZ[0]);
// 			jac[j*ncol+1]=s*(a*pdX[1]+b*pdY[1]+c*pdZ[1]);
// 			jac[j*ncol+2]=s*(a*pdX[2]+b*pdY[2]+c*pdZ[2]);
// 			jac[j*ncol+3]=s*(a*pdX[3]+b*pdY[3]+c*pdZ[3]);
// 			jac[j*ncol+4]=s*(a*pdX[4]+b*pdY[4]+c*pdZ[4]);
// 			jac[j*ncol+5]=s*(a*pdX[5]+b*pdY[5]+c*pdZ[5]);
// 
// 			//平面
// 			jac[j*ncol+pnp+i*4]  =(xp*(b2+c2)-a*(b*yp+c*zp+d))*s3;
// 			jac[j*ncol+pnp+i*4+1]=(yp*(a2+c2)-b*(a*xp+c*zp+d))*s3;
// 			jac[j*ncol+pnp+i*4+2]=(zp*(a2+b2)-c*(a*xp+b*yp+d))*s3;
// 			jac[j*ncol+pnp+i*4+3]=s;
// 		}
// 	}
// }


//#define _OUTPUT_PARAM_
//s1*x + s2*y + s3*z + s4=0
// int CLidCalib_Skaloud::LidMC_Misalign_plane( const int nPlane,	/* number of planes */
// 						LidMC_Plane *pLidPlane,
// 						double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
// 											* aj are the image j parameters, bi are the i-th point parameters,
// 											* size m*cnp + n*pnp
// 											未知数，偏心分量和安置角    
// 											pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
// 			    		const int pnp
// 						)
// {
// 	int i, j, k;
// 	int ndim/*, npd*/;
// 	double *C=NULL;	 
// 	double *B=NULL, *L=NULL;
// 	
// 	double *pPlane=NULL;		//初始平面参数
// 	double *X=NULL;		//初始标定参数
// 	
// //	double a, b, c, d;	//平面参数  ax+by+cz+d=0
// //	double a2, b2, c2;
// 	
// 	int nTies;
// 	int nLidPoints;		/* 点数 */ 
// 	int nParam;			/* 平面参数总数 */
// 	
// 	ATNPoint *pLidPoints=NULL;	/* 激光点观测值	*/
// 	//	double  *pPlane=NULL;		/* 平面参数 */
// 	long *pPlaneLut=NULL;	//每个平面所属点的查找表
// 	
// 	double *dX=NULL;		//标定参数的改正数
// 	double *dPlane=NULL;	//平面参数的改正数
// 	double *pPDC=NULL;
// //	double *pdX, *pdY, *pdZ;
// //	double s, s3;
// //	double xp, yp, zp;
// 
// 	
// 	for(i=0; i<nPlane; i++)
// 	{//计算初始平面参数，用ransac挑点
// 		TRACE("%d ", (pLidPlane+i)->pLidPoints->size());
// 		PlaneFitting_ransac(pLidPlane+i);
// 		TRACE("--> %d\n", (pLidPlane+i)->pLidPoints->size());
// 	}
// 	
// 	//统计连接面数量, tieID必须连续排列
// 	int tieID=-1;
// 	nTies=0;
// 	for(i=0; i<nPlane; i++)
// 	{
// 		if(tieID!=pLidPlane[i].tieID)
// 		{
// 			nTies++;
// 			tieID=pLidPlane[i].tieID;
// 		}
// 	}
// //	nTies+=1;
// 	
// 	nParam=nTies*4;	//平面的一般方程
// // 	pPlane=new double[nParam];
//  	pPlaneLut=new long[nTies+1];
// // 	
// // 	dX=new double[pnp];
// // 	dPlane=new double[nParam];
// 
// 	X=new double[pnp+4*nTies];
// 	for(i=0; i<pnp; i++)
// 	{
// 		X[i]=pX[i];
// 	}
// 	pPlane=X+pnp;
// 	
// 	//需要保证传入的连接面按tieID顺序组织
// 	int maxPtNum=0;
// 	nLidPoints=0;
// 	for(i=0, j=-1; i<nPlane; i++)
// 	{
// 		if(j==pLidPlane[i].tieID)
// 		{
// 			if(maxPtNum<pLidPlane[i].pLidPoints->size())
// 			{//取点数最多的条带作为初始平面参数
// 				maxPtNum=pLidPlane[i].pLidPoints->size();
// 
// 				pPlane[j*4]  =pLidPlane[i].s1;
// 				pPlane[j*4+1]=pLidPlane[i].s2;
// 				pPlane[j*4+2]=pLidPlane[i].s3;
// 				pPlane[j*4+3]=pLidPlane[i].s4;
// 			}
// // 			else
// // 			{
// // 
// // 			}
// 
// 			pPlaneLut[j+1]+=pLidPlane[i].pLidPoints->size();
// 			nLidPoints+=pLidPlane[i].pLidPoints->size();
// 		}
// 		else
// 		{
// 			j=pLidPlane[i].tieID;
// 			maxPtNum=pLidPlane[i].pLidPoints->size();
// 			
// 			pPlane[j*4]  =pLidPlane[i].s1;
// 			pPlane[j*4+1]=pLidPlane[i].s2;
// 			pPlane[j*4+2]=pLidPlane[i].s3;
// 			pPlane[j*4+3]=pLidPlane[i].s4;
// 			
// 			pPlaneLut[j+1]=pLidPlane[i].pLidPoints->size()+nLidPoints;
// 			nLidPoints+=pLidPlane[i].pLidPoints->size();
// 		}
// 
// 	}
// 	pPlaneLut[0]=0;
// 	
// 	pLidPoints=new ATNPoint[nLidPoints];
// 	k=0;
// 	for(i=0; i<nPlane; i++)
// 	{
// 		std::vector<ATNPoint> *pVec=pLidPlane[i].pLidPoints;
// 		for(j=0; j<pVec->size(); j++)
// 		{
// 			pLidPoints[k++]=(*pVec)[j];
// 		}
// 	}
// 	
// 	ndim=3;		//x,y,z
// //	npd=6;		//dtx, dty, dtz, dr, dp, dh
// 	
//  	pPDC=new double[nLidPoints*ndim*pnp];
// // 	B=new double[nLidPoints*npd];
// // 	C=new double[nLidPoints*4*nTies];	//未采用压缩存储
//  	L=new double[nLidPoints];
// 	for(i=0; i<nLidPoints; i++)
// 	{
// 		L[i]=0;
// 	}
// 	
// 	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
// 	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;
// 
// 	struct myObs data;
// 	data.nMisalign=pnp;
// 	data.nTiePlane=nTies;
// 	data.nPoints=nLidPoints;
// 	data.pLidPoints=pLidPoints;
// 	data.pPlaneLut=pPlaneLut;
// 	data.pPDC=pPDC;
// 	data.ndim=3;
// 
// #ifdef _DEBUG
// 	FILE *fp=NULL;
// 	fp=fopen(/*"g:/D_experiment/adjustment/6param/plane_adjust_6p"*/m_pResultName, "wt");
// 
// 	double *dis=NULL;
// 	dis=new double[nLidPoints];
// 	double *rms=NULL;
// 	rms=new double[nTies];
// 
// 	//统计初始连接面的距离方差
// 	distance_PointToPlane(X, dis, pnp+4*nTies, nLidPoints, &data);
// 	fprintf(fp, "init plane rms\n");
// 	for(i=0; i<nTies; i++)
// 	{//遍历平面
// 		rms[i]=0;
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			rms[i]+=dis[j]*dis[j];
// 		}
// 		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
// 		rms[i]=sqrt(rms[i]);
// 		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
// 	}
// #endif
// 
// 	int ret=dlevmar_der(distance_PointToPlane, jac_dis_PointToPlane, X, L, pnp+nTies*4, nLidPoints, 1000, 
// 						opts, info, NULL, NULL, &data);
// 
// #ifdef _DEBUG 
// 	
// 	fprintf(fp, "## resolving results\n");
// 	fprintf(fp, "## misalign: tx, ty, tz, r, p, h\n");
// 	fprintf(fp, "%.9lf\n", X[0]);
// 	fprintf(fp, "%.9lf\n", X[1]);
// 	fprintf(fp, "%.9lf\n", X[2]);
// 	fprintf(fp, "%.9lf\n", X[3]);
// 	fprintf(fp, "%.9lf\n", X[4]);
// 	fprintf(fp, "%.9lf\n", X[5]);
// 
// 	pPlane=X+pnp;
// 	fprintf(fp, "## %d tie_plane param\n", nTies);
// 	for(i=0; i<nTies; i++)
// 	{
// 		fprintf(fp, "%.6lf\n", pPlane[i*4]);
// 		fprintf(fp, "%.6lf\n", pPlane[i*4+1]);
// 		fprintf(fp, "%.6lf\n", pPlane[i*4+2]);
// 		fprintf(fp, "%.6lf\n", pPlane[i*4+3]);
// 	}
// 	 	
// 	//计算每个连接面的中误差
// 	distance_PointToPlane(X, dis, pnp+4*nTies, nLidPoints, &data);
// 
// 	fprintf(fp, "## tie_plane rms\n");
// 	
// 	for(i=0; i<nTies; i++)
// 	{//遍历平面
// 		rms[i]=0;
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			rms[i]+=dis[j]*dis[j];
// 		}
// 		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
// 		rms[i]=sqrt(rms[i]);
// 		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
// 	}
// 
// 	
// 	if(fp)	fclose(fp);	fp=NULL;
// #endif
// 
// 	if(dis)		delete[] dis;	dis=NULL;
// 	if(rms)		delete[] rms;	rms=NULL;
// 
// 	if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
// 	if(pPlaneLut)	delete pPlaneLut;	pPlaneLut=NULL;
// 	if(L)			delete L;			L=NULL;
// 	if(X)			delete X;			X=NULL;
// 	if(pPDC)		delete pPDC;		pPDC=NULL;
// 	return 0;
// }

void LidStatDis_PointToPlane_Skaloud(LidMC_Plane *pLidPatch, int nPatch, double *Lidparam, bool bInit, orsLidSysParam oriParam, CalibParam_Type param_type,
							 double *rms, int nTies, double *pDis)
{
	double *X=NULL;
	int nCalibParam;
	LidPt_SurvInfo *pLidPoints=NULL;	/* 激光点观测值	*/
	long *pPlaneLut=NULL;	//每个平面所属点的查找表

	double *pPlane=NULL;	//不需要分配空间
	int i, j, k;

	switch (param_type)
	{
	case Calib_rph:
		nCalibParam = 3;
		break;
	case Calib_rphxyz:
		nCalibParam = 6;
		break;
	case Calib_rph_appXYZ:
		nCalibParam = 6;
		break;
	case Calib_rphxyz_sa0:
		nCalibParam = 7;
		break;
	case Calib_rphxyz_sa0_sac:
		nCalibParam = 8;
		break;
	case Calib_rph_sa0:
		nCalibParam = 4;
		break;
	case Calib_rph_drange:
		nCalibParam = 4;
		break;
	case Calib_rph_drange_sa0:
		nCalibParam= 5;
		break;
	default:
		nCalibParam = 0;
	}

	X = new double[nCalibParam + 4*nTies];

	pPlaneLut=new long[nTies+1];

	if(bInit)
	{
		for(i=0; i<nCalibParam+4*nTies; i++)
		{
			X[i]=Lidparam[i];
		}
	}
	else
	{
		for(i=0; i<nCalibParam; i++)
		{
			X[i]=Lidparam[i];
		}
	}

	pPlane=X+nCalibParam;
	
	//需要保证传入的连接面按tieID顺序组织
	int maxPtNum=0;
	int nLidPoints=0;
	for(i=0, j=-1; i<nPatch; i++)
	{
		if(j==pLidPatch[i].tieID)
		{
			if(maxPtNum<pLidPatch[i].pLidPoints->size())
			{//取点数最多的条带作为初始平面参数
				maxPtNum=pLidPatch[i].pLidPoints->size();

				if(!bInit)
				{
					pPlane[j*4]  =pLidPatch[i].s1;
					pPlane[j*4+1]=pLidPatch[i].s2;
					pPlane[j*4+2]=pLidPatch[i].s3;
					pPlane[j*4+3]=pLidPatch[i].s4;
				}
			}

			pPlaneLut[j+1]+=pLidPatch[i].pLidPoints->size();
			nLidPoints+=pLidPatch[i].pLidPoints->size();
		}
		else
		{
			j=pLidPatch[i].tieID;
			maxPtNum=pLidPatch[i].pLidPoints->size();

			if(!bInit)
			{
				pPlane[j*4]  =pLidPatch[i].s1;
				pPlane[j*4+1]=pLidPatch[i].s2;
				pPlane[j*4+2]=pLidPatch[i].s3;
				pPlane[j*4+3]=pLidPatch[i].s4;
			}
			
			pPlaneLut[j+1]=pLidPatch[i].pLidPoints->size()+nLidPoints;
			nLidPoints+=pLidPatch[i].pLidPoints->size();
		}

	}
	pPlaneLut[0]=0;

	pLidPoints=new LidPt_SurvInfo[nLidPoints];
	k=0;
	for(i=0; i<nPatch; i++)
	{
		std::vector<LidPt_SurvInfo> *pVec=pLidPatch[i].pLidPoints;
		for(j=0; j<pVec->size(); j++)
		{
			pLidPoints[k++]=(*pVec)[j];
		}
	}

	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_RangeVec(pLidPoints, nLidPoints);

	double *dis=NULL;
	dis=new double[nLidPoints];

	struct myObs data;
	data.nMisalign=nCalibParam;
	data.nTiePlane=nTies;
	data.nPoints=nLidPoints;
	data.pLidPoints=pLidPoints;
	data.pPlaneLut=pPlaneLut;
	data.type = param_type;
	data.oriParam = oriParam;

	data.pLidGeo = lidGeo.get();

//	data.pPDC=pPDC;
//	data.ndim=3;
//	data.tx=tx;
//	data.ty=ty;
//	data.tz=tz;

	distance_PointToPlane_Skaloud(X, dis, nCalibParam+4*nTies, nLidPoints, &data);

	if(pDis)
		memcpy(pDis, dis, sizeof(double)*nLidPoints);

	for(i=0; i<nTies; i++)
	{//遍历平面
		rms[i]=0;
		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			rms[i]+=dis[j]*dis[j];
		}
		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
		rms[i]=sqrt(rms[i]);
//		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
	}

	if(X)	delete[] X; X=NULL;
	if(pLidPoints) delete[] pLidPoints;	pLidPoints=NULL;
	if(pPlaneLut)  delete[] pPlaneLut;	pPlaneLut=NULL;
	if(dis)		delete[] dis;	dis=NULL;
}

void LidPlane_ransac(LidMC_Plane *pLidPlane, int nPlane)
{
	for(int i=0; i<nPlane; i++)
	{//计算初始平面参数，用ransac挑点
		TRACE("%d ", (pLidPlane+i)->pLidPoints->size());
		PlaneFitting_ransac(pLidPlane+i);
		TRACE("--> %d\n", (pLidPlane+i)->pLidPoints->size());	
	}
}

int lidPlaneFitting_Skaloud(LidMC_Plane *pLidPlane, int nPlane, orsLidSysParam oriParam, double *Lidparam, CalibParam_Type type)
{
	int i, j, k;

	int nTies;
	int nLidPoints;		/* 点数 */ 
	int nParam;			/* 平面参数总数 */
	
	LidPt_SurvInfo *pLidPoints=NULL;	/* 激光点观测值	*/
	//	double  *pPlane=NULL;		/* 平面参数 */
	long *pPlaneLut=NULL;	//每个平面所属点的查找表
	
	double *dX=NULL;		//标定参数的改正数
	double *dPlane=NULL;	//平面参数的改正数
	double *pPDC=NULL;

	
	//统计连接面数量, tieID必须连续排列
	int tieID=-1;
	nTies=0;
	for(i=0; i<nPlane; i++)
	{
		if(tieID!=pLidPlane[i].tieID)
		{
			nTies++;
			tieID=pLidPlane[i].tieID;
		}
	}

	pPlaneLut=new long[nTies+1];

	//需要保证传入的连接面按tieID顺序组织
	int maxPtNum=0;
	nLidPoints=0;
	for(i=0, j=-1; i<nPlane; i++)
	{
		if(j==pLidPlane[i].tieID)
		{
			pPlaneLut[j+1]+=pLidPlane[i].pLidPoints->size();
			nLidPoints+=pLidPlane[i].pLidPoints->size();
		}
		else
		{
			j=pLidPlane[i].tieID;
			
			pPlaneLut[j+1]=pLidPlane[i].pLidPoints->size()+nLidPoints;
			nLidPoints+=pLidPlane[i].pLidPoints->size();
		}
		
	}

	pPlaneLut[0]=0;
	
	pLidPoints=new LidPt_SurvInfo[nLidPoints];
	k=0;
	for(i=0; i<nPlane; i++)
	{
		std::vector<LidPt_SurvInfo> *pVec=pLidPlane[i].pLidPoints;
		for(j=0; j<pVec->size(); j++)
		{
			pLidPoints[k++]=(*pVec)[j];
		}
	}
	
	//重新计算激光点坐标
//	_calLaserPosition(pLidPoints, nLidPoints, oriParam, Lidparam, type);

	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	orsLidSysParam curParam = oriParam;
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_RangeVec(pLidPoints, nLidPoints);
	
	switch (type)
	{
	case Calib_rph:
		curParam.boresight_angle[0] = Lidparam[0];
		curParam.boresight_angle[1] = Lidparam[1];
		curParam.boresight_angle[2] = Lidparam[2];
		break;
	case Calib_rphxyz:
		curParam.boresight_angle[0] = Lidparam[0];
		curParam.boresight_angle[1] = Lidparam[1];
		curParam.boresight_angle[2] = Lidparam[2];
		curParam.lever_arm[0] = Lidparam[3];
		curParam.lever_arm[1] = Lidparam[4];
		curParam.lever_arm[2] = Lidparam[5];
		break;
	case Calib_rph_appXYZ:
		curParam.boresight_angle[0] = Lidparam[0];
		curParam.boresight_angle[1] = Lidparam[1];
		curParam.boresight_angle[2] = Lidparam[2];
		curParam.semiXYZ[0] = Lidparam[3];
		curParam.semiXYZ[1] = Lidparam[4];
		curParam.semiXYZ[2] = Lidparam[5];
		break;
	case Calib_rphxyz_sa0:
		curParam.boresight_angle[0] = Lidparam[0];
		curParam.boresight_angle[1] = Lidparam[1];
		curParam.boresight_angle[2] = Lidparam[2];
		curParam.lever_arm[0] = Lidparam[3];
		curParam.lever_arm[1] = Lidparam[4];
		curParam.lever_arm[2] = Lidparam[5];
		curParam.sa0 = Lidparam[6];
		break;
	case Calib_rphxyz_sa0_sac:
		curParam.boresight_angle[0] = Lidparam[0];
		curParam.boresight_angle[1] = Lidparam[1];
		curParam.boresight_angle[2] = Lidparam[2];
		curParam.lever_arm[0] = Lidparam[3];
		curParam.lever_arm[1] = Lidparam[4];
		curParam.lever_arm[2] = Lidparam[5];
		curParam.sa0 = Lidparam[6];
		curParam.sac = Lidparam[7];
		break;
	case Calib_rph_sa0:
		curParam.boresight_angle[0] = Lidparam[0];
		curParam.boresight_angle[1] = Lidparam[1];
		curParam.boresight_angle[2] = Lidparam[2];
		curParam.sa0 = Lidparam[3];
		break;
	case Calib_rph_drange:
		curParam.boresight_angle[0] = Lidparam[0];
		curParam.boresight_angle[1] = Lidparam[1];
		curParam.boresight_angle[2] = Lidparam[2];
		curParam.drange = Lidparam[3];
		break;
	case Calib_rph_drange_sa0:
		curParam.boresight_angle[0] = Lidparam[0];
		curParam.boresight_angle[1] = Lidparam[1];
		curParam.boresight_angle[2] = Lidparam[2];
		curParam.drange = Lidparam[3];
		curParam.sa0 = Lidparam[4];
		break;
	default:
		;
	}


	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_Laserfootprint(pLidPoints, nLidPoints);


	maxPtNum = 0;
	for(i=0; i<nTies; i++)
	{
		j = pPlaneLut[i+1]-pPlaneLut[i];
		if(maxPtNum < j)
			maxPtNum = j;
	}

	POINT3D *pts=NULL;

	pts = new POINT3D[maxPtNum];

	double Mbest[3];
	for(i=0; i<nTies; i++)
	{//遍历平面
		

		for(j=pPlaneLut[i], k=0; j<pPlaneLut[i+1]; j++, k++) 
		{
			pts[k].X = pLidPoints[j].x;
			pts[k].Y = pLidPoints[j].y;
			pts[k].Z = pLidPoints[j].z;
		}

		plane_fitting(pPlaneLut[i+1]-pPlaneLut[i], pts, Mbest);

		pLidPlane[i].s1 = Mbest[0];
		pLidPlane[i].s2 = Mbest[1];
		pLidPlane[i].s3 = -1;
		pLidPlane[i].s4 = Mbest[2];
		
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{
// 			if(pLidPlane[j].tieID == i)
// 			{
// 				pLidPlane[j].s1 = Mbest[0];
// 				pLidPlane[j].s2 = Mbest[1];
// 				pLidPlane[j].s3 = -1;
// 				pLidPlane[j].s4 = Mbest[2];
// 			}
// 		}
	}



	if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
	if(pPlaneLut)	delete pPlaneLut;	pPlaneLut=NULL;
	if(pts)			delete pts;			pts = NULL;	

	return 0;
}

// int lidPlaneFitting(LidMC_Plane *pLidPlane, int nPlane)
// {
// 	int i, j, k;
// 	int nTies;
// 	int nLidPoints;		/* 点数 */ 
// 	int nParam;			/* 平面参数总数 */
// 
// 	ATNPoint *pLidPoints=NULL;	/* 激光点观测值	*/
// 	//	double  *pPlane=NULL;		/* 平面参数 */
// 	long *pPlaneLut=NULL;	//每个平面所属点的查找表
// 
// 	double *dX=NULL;		//标定参数的改正数
// 	double *dPlane=NULL;	//平面参数的改正数
// 	double *pPDC=NULL;
// 
// 
// // 	//统计连接面数量, tieID必须连续排列
// // 	int tieID=-1;
// // 	nTies=0;
// // 	for(i=0; i<nPlane; i++)
// // 	{
// // 		if(tieID!=pLidPlane[i].tieID)
// // 		{
// // 			nTies++;
// // 			tieID=pLidPlane[i].tieID;
// // 		}
// // 	}
// // 
// // 	pPlaneLut=new long[nTies+1];
// 
// 	//需要保证传入的连接面按tieID顺序组织
// // 	int maxPtNum=0;
// // 	nLidPoints=0;
// // 	for(i=0, j=-1; i<nPlane; i++)
// // 	{
// // 		if(j==pLidPlane[i].tieID)
// // 		{
// // 			pPlaneLut[j+1]+=pLidPlane[i].pLidPoints->size();
// // 			nLidPoints+=pLidPlane[i].pLidPoints->size();
// // 		}
// // 		else
// // 		{
// // 			j=pLidPlane[i].tieID;
// // 
// // 			pPlaneLut[j+1]=pLidPlane[i].pLidPoints->size()+nLidPoints;
// // 			nLidPoints+=pLidPlane[i].pLidPoints->size();
// // 		}
// // 
// // 	}
// // 
// // 	pPlaneLut[0]=0;
// // 
// // 	pLidPoints=new ATNPoint[nLidPoints];
// // 	k=0;
// // 	for(i=0; i<nPlane; i++)
// // 	{
// // 		std::vector<ATNPoint> *pVec=pLidPlane[i].pLidPoints;
// // 		for(j=0; j<pVec->size(); j++)
// // 		{
// // 			pLidPoints[k++]=(*pVec)[j];
// // 		}
// // 	}
// 
// 
// 	int maxPtNum = 0;
// 	for(i=0; i<nPlane; i++)
// 	{
// 		std::vector<ATNPoint> *pVec=pLidPlane[i].pLidPoints;
// 
// 		j = pVec->size();
// 		if(maxPtNum < j)
// 			maxPtNum = j;
// 	}
// 
// 	POINT3D *pts=NULL;
// 
// 	pts = new POINT3D[maxPtNum];
// 
// 	double Mbest[3];
// 	for(i=0; i<nPlane; i++)
// 	{//遍历平面
// 
// 		std::vector<ATNPoint> *pVec=pLidPlane[i].pLidPoints;
// 		for(j=0; j<pVec->size(); j++)
// 		{
// 			pts[j].X=(*pVec)[j].x;
// 			pts[j].Y=(*pVec)[j].y;
// 			pts[j].Z=(*pVec)[j].z;
// 		}
// 
// 
// 		plane_fitting(pVec->size(), pts, Mbest);
// 
// 		pLidPlane[i].s1 = Mbest[0];
// 		pLidPlane[i].s2 = Mbest[1];
// 		pLidPlane[i].s3 = -1;
// 		pLidPlane[i].s4 = Mbest[2];
// 	}
// 
// 
// 
// 	if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
// 	if(pPlaneLut)	delete pPlaneLut;	pPlaneLut=NULL;
// 	if(pts)			delete pts;			pts = NULL;	
// 
// 	return 0;
// }

CLidCalib_Skaloud::CLidCalib_Skaloud()
{
	m_covar = NULL;
	m_rms = 0;
	m_corcoef = NULL;
}

CLidCalib_Skaloud::~CLidCalib_Skaloud()
{
	if(m_covar)
		delete m_covar;
	m_covar = NULL;

	if(m_corcoef)
		delete m_corcoef;
	m_corcoef = NULL;
}

int CLidCalib_Skaloud::LidMC_Misalign_plane( const char *pszResultFile,		/* 平差结果文件 */
							const int nPlane,	/* number of planes */
							LidMC_Plane *pLidPlane,
							double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
											* aj are the image j parameters, bi are the i-th point parameters,
											* size m*cnp + n*pnp
											未知数，偏心分量和安置角    
											pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
							orsLidSysParam oriParam,
			    			const CalibParam_Type type
)
{
	int i, j, k;
	int ndim/*, npd*/;
	double *C=NULL;	 
	double *B=NULL, *L=NULL;
	
	double *pPlane=NULL;		//初始平面参数
	double *X=NULL;		//初始标定参数
	
//	double a, b, c, d;	//平面参数  ax+by+cz+d=0
//	double a2, b2, c2;
	
	int nTies;
	int nLidPoints;		/* 点数 */ 
	int nParam;			/* 平面参数总数 */
	
	LidPt_SurvInfo *pLidPoints=NULL;	/* 激光点观测值	*/
	//	double  *pPlane=NULL;		/* 平面参数 */
	long *pPlaneLut=NULL;	//每个平面所属点的查找表
	
	double *dX=NULL;		//标定参数的改正数
	double *dPlane=NULL;	//平面参数的改正数
	double *pPDC=NULL;
//	double *pdX, *pdY, *pdZ;
//	double s, s3;
//	double xp, yp, zp;

/*   2013.1.4 将ransac提成单独函数，由外部调用

	for(i=0; i<nPlane; i++)
	{//计算初始平面参数，用ransac挑点
		TRACE("%d ", (pLidPlane+i)->pLidPoints->size());
		PlaneFitting_ransac(pLidPlane+i);
		TRACE("--> %d\n", (pLidPlane+i)->pLidPoints->size());	
	}
*/
	
	//统计连接面数量, tieID必须连续排列
	int tieID=-1;
	nTies=0;
	for(i=0; i<nPlane; i++)
	{
		if(tieID!=pLidPlane[i].tieID)
		{
			nTies++;
			tieID=pLidPlane[i].tieID;
		}
	}
//	nTies+=1;
	
	nParam=nTies*4;	//平面的一般方程
// 	pPlane=new double[nParam];
 	pPlaneLut=new long[nTies+1];
// 	
// 	dX=new double[pnp];
// 	dPlane=new double[nParam];

	int nCalibParam;
	switch (type)
	{
	case Calib_rph:
		nCalibParam = 3;
		break;
	case Calib_rphxyz:
		nCalibParam = 6;
		break;
	case Calib_rph_appXYZ:
		nCalibParam = 6;
		break;
	case Calib_rphxyz_sa0:
		nCalibParam = 7;
		break;
	case Calib_rphxyz_sa0_sac:
		nCalibParam = 8;
		break;
	case Calib_rph_sa0:
		nCalibParam = 4;
		break;
	case Calib_rph_drange:
		nCalibParam = 4;
		break;
	case Calib_rph_drange_sa0:
		nCalibParam= 5;
		break;
	default:
		nCalibParam = 0;
	}

//	int nMisalign=3;
	X = new double[nCalibParam + 4*nTies];
 	for(i=0; i<nCalibParam; i++)
 	{
		X[i]=pX[i];
	}
	pPlane = X+nCalibParam;
	
	//需要保证传入的连接面按tieID顺序组织
	int maxPtNum=0;
	nLidPoints=0;
	for(i=0, j=-1; i<nPlane; i++)
	{
		if(j==pLidPlane[i].tieID)
		{
			if(maxPtNum<pLidPlane[i].pLidPoints->size())
			{//取点数最多的条带作为初始平面参数
				maxPtNum=pLidPlane[i].pLidPoints->size();

				pPlane[j*4]  =pLidPlane[i].s1;
				pPlane[j*4+1]=pLidPlane[i].s2;
				pPlane[j*4+2]=pLidPlane[i].s3;
				pPlane[j*4+3]=pLidPlane[i].s4;
			}
// 			else
// 			{
// 
// 			}

			pPlaneLut[j+1]+=pLidPlane[i].pLidPoints->size();
			nLidPoints+=pLidPlane[i].pLidPoints->size();
		}
		else
		{
			j=pLidPlane[i].tieID;
			maxPtNum=pLidPlane[i].pLidPoints->size();
			
			pPlane[j*4]  =pLidPlane[i].s1;
			pPlane[j*4+1]=pLidPlane[i].s2;
			pPlane[j*4+2]=pLidPlane[i].s3;
			pPlane[j*4+3]=pLidPlane[i].s4;
			
			pPlaneLut[j+1]=pLidPlane[i].pLidPoints->size()+nLidPoints;
			nLidPoints+=pLidPlane[i].pLidPoints->size();
		}

	}
	pPlaneLut[0]=0;
	
	pLidPoints=new LidPt_SurvInfo[nLidPoints];
	k=0;
	for(i=0; i<nPlane; i++)
	{
		std::vector<LidPt_SurvInfo> *pVec=pLidPlane[i].pLidPoints;
		for(j=0; j<pVec->size(); j++)
		{
			pLidPoints[k++]=(*pVec)[j];
		}
	}

	/////////////////////////////////////////////
	//////////////////提取斜距//////////////////
	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_RangeVec(pLidPoints, nLidPoints);

	
	ndim=3;		//x,y,z
//	npd=6;		//dtx, dty, dtz, dr, dp, dh
	
 	pPDC=new double[nLidPoints*(nCalibParam + 4*nTies)];
// 	B=new double[nLidPoints*npd];
// 	C=new double[nLidPoints*4*nTies];	//未采用压缩存储
 	L=new double[nLidPoints];
	double *hx=new double[nLidPoints];
	double *hxx=new double[nLidPoints];

	for(i=0; i<nLidPoints; i++)
	{
		L[i]=0;
	}
	
	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;

	struct myObs data;
	data.nMisalign=nCalibParam;
	data.nTiePlane=nTies;
	data.nPoints=nLidPoints;
	data.pLidPoints=pLidPoints;
	data.pPlaneLut=pPlaneLut;
	data.pPDC=pPDC;
	data.ndim=3;
	data.type = type;
	data.hx = hx;
	data.hxx = hxx;
	data.oriParam = oriParam;
	data.pLidGeo = lidGeo.get();

/*
#ifdef _DEBUG
	FILE *fp=NULL;
	fp=fopen("G:/D_experiment/adjustment/3param/plane_adjust_3p", "wt");

	double *dis=NULL;
	dis=new double[nLidPoints];
	double *rms=NULL;
	rms=new double[nTies];

	//统计初始连接面的距离方差
	distance_PointToPlane_rph(X, dis, nMisalign+4*nTies, nLidPoints, &data);
	fprintf(fp, "init plane rms\n");
	for(i=0; i<nTies; i++)
	{//遍历平面
		rms[i]=0;
		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			rms[i]+=dis[j]*dis[j];
		}
		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
		rms[i]=sqrt(rms[i]);
		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
	}
#endif*/

	double *rms = new double[nTies]; 
	LidStatDis_PointToPlane_Skaloud(pLidPlane, nPlane, X, false, oriParam, type, rms, nTies);


	FILE *fp = fopen(pszResultFile, "wt");

	fprintf(fp, "## calibration method: Coplanar Constraint\n");
	fprintf(fp, "## calib param type: %d\n", type);
	fprintf(fp, "## init calib param\n");
	for(i=0; i<nCalibParam; i++)
	{
		fprintf(fp, "%.9lf\n", X[i]);
	}

	fprintf(fp, "total points number: %d\n", nLidPoints);
	fprintf(fp, "before adjustment\n");
	fprintf(fp, "init plane rms.\n");
	for(i=0; i<nTies; i++)
	{//遍历平面
		int ptNum=0;
		for(j=0; j<nPlane; j++)
		{
			if(pLidPlane[j].tieID==i)
				ptNum+=pLidPlane[j].pLidPoints->size();
		}
		fprintf(fp, "%d num:%d rms:%.6f\n", i, ptNum, rms[i]);
	}

	if(m_covar)	delete m_covar;

	m_pn = nCalibParam+nTies*4;
	m_covar = new double[m_pn*m_pn];

	int ret=dlevmar_der(distance_PointToPlane_Skaloud, jac_dis_PointToPlane_diff, X, L, nCalibParam+nTies*4, nLidPoints, 1000, 
						opts, info, NULL, m_covar, &data);


	//更新平面方程后才能得到正确的点面距离
// 	for(i=0, j=-1; i<nPlane; i++)
// 	{
// 		j=pLidPlane[i].tieID;
// 		
// 		pLidPlane[i].s1 = pPlane[j*4];
// 		pLidPlane[i].s2 = pPlane[j*4+1];
// 		pLidPlane[i].s3	= pPlane[j*4+2];
// 		pLidPlane[i].s4	= pPlane[j*4+3];
// 	}

	double *dis = new double[nLidPoints];

	LidStatDis_PointToPlane_Skaloud(pLidPlane, nPlane, X, true, oriParam, type, rms, nTies, dis);

	fprintf(fp, "after adjustment\n");

	fprintf(fp, "resolving plane rms\n");
	for(i=0; i<nTies; i++)
	{//遍历平面
		int ptNum=0;
		for(j=0; j<nPlane; j++)
		{
			if(pLidPlane[j].tieID==i)
				ptNum+=pLidPlane[j].pLidPoints->size();
		}
		fprintf(fp, "%d num:%d rms:%.6f\n", i, ptNum, rms[i]);
	}

	fprintf(fp, "## resolving results\n");
	for(i=0; i<nCalibParam; i++)
	{
		fprintf(fp, "%.9lf\n", X[i]);
	}


	m_rms = sqrt(info[1]/(nLidPoints - nCalibParam+nTies*4));


	int pnp;
	pnp = nCalibParam + nTies*4;
	
	if(m_corcoef) delete m_corcoef;
	m_corcoef = new double[pnp*pnp];

	k=0;
	for(i=0;i<pnp; i++)
	{
		double Dxx = sqrt(m_covar[i*pnp+i]);

		for(j=0; j<pnp; j++)
		{
			double Dyy = sqrt(m_covar[j*pnp+j]);
			double Dxy = m_covar[i*pnp+j];

			double Dyx = m_covar[j*pnp+i];

			if(i>j)
				Dxy = Dyx;

//			assert(Dxy < 1.2*Dyx && Dxy > 0.8*Dyx);
			
			m_corcoef[k] = Dxy/Dxx/Dyy;
			k++;
		}
	}

	fprintf(fp, "####### RMS #########\n");
	fprintf(fp, "%.5lf\n", m_rms);

	fprintf(fp, "####### covariance of unknowns #########\n");
	fprintf(fp, "number of unknowns : %d\n", m_pn);
	for(i=0; i<m_pn; i++)
	{
		for(j=0; j<m_pn; j++)
		{	
			k = i*m_pn + j;
			fprintf(fp, " %e", m_covar[k]);
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "####### correlation coefficient of unknowns #######\n");
	k=0;
	for(i=0; i<pnp; i++)
	{
		for(j=0; j<pnp; j++)
		{
// 			if(i<pnp-1 && j>i)
// 			{
				fprintf(fp," %e", fabs(m_corcoef[i*pnp+j]));
// 				k++;
// 			}
// 			else
// 				fprintf(fp, " /");
		}
		fprintf(fp, "\n");
	}

	if(fp)	fclose(fp);	fp=NULL;
		
	//检校结果更新
	for(i=0; i<nCalibParam; i++)
	{
		pX[i] = X[i];
	}

	//平面参数更新
	for(i=0; i<nPlane; i++)
	{
		j=pLidPlane[i].tieID;

		pLidPlane[i].s1 = pPlane[j*4];
		pLidPlane[i].s2 = pPlane[j*4+1];
		pLidPlane[i].s3 = pPlane[j*4+2];
		pLidPlane[i].s4 = pPlane[j*4+3];

	}

	//导出点面距离残差，用于分析
	orsString strTmp;
	orsString strPath = orsString::getDirFromPath(pszResultFile);
	orsString purename;
	strTmp = orsString::getPureFileName(pszResultFile);
	int pos = strTmp.reverseFind( '.');
	purename = strTmp.left(pos);

	char tmp[128];
	for(i=0; i<nTies; i++)
	{//遍历平面

		orsString name = strPath + "\\";
		sprintf(tmp, "_plane_%d.txt", i);
		name += purename;
		name +=	tmp;

		FILE *fout = fopen(name, "wt");
		int ptNum=0;
		for(j=0; j<nPlane; j++)
		{
			if(pLidPlane[j].tieID==i)
				ptNum+=pLidPlane[j].pLidPoints->size();
		}
		fprintf(fout, "planeID %d, point num:%d, rms:%.6f\n", i, ptNum, rms[i]);
		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			fprintf(fout, "%5f\n", dis[j]);
		}

		fclose(fout);	
	}


/*
#ifdef _DEBUG 
	 
	fprintf(fp, "## resolving results\n");
	fprintf(fp, "## misalign: r, p, h\n");
	fprintf(fp, "%.9lf\n", tx);
	fprintf(fp, "%.9lf\n", ty);
	fprintf(fp, "%.9lf\n", tz);
	fprintf(fp, "%.9lf\n", X[0]);
	fprintf(fp, "%.9lf\n", X[1]);
	fprintf(fp, "%.9lf\n", X[2]);

	pPlane=X+nMisalign;
	fprintf(fp, "## %d tie_plane param\n", nTies);
	for(i=0; i<nTies; i++)
	{
		fprintf(fp, "%.6lf\n", pPlane[i*4]);
		fprintf(fp, "%.6lf\n", pPlane[i*4+1]);
		fprintf(fp, "%.6lf\n", pPlane[i*4+2]);
		fprintf(fp, "%.6lf\n", pPlane[i*4+3]);
	}
	 	
	

	//计算每个连接面的中误差
	distance_PointToPlane_rph(X, dis, nMisalign+4*nTies, nLidPoints, &data);

	fprintf(fp, "## tie_plane rms\n");
	
	for(i=0; i<nTies; i++)
	{//遍历平面
		rms[i]=0;
		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			rms[i]+=dis[j]*dis[j];
		}
		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
		rms[i]=sqrt(rms[i]);
		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
	}

	
	if(fp)	fclose(fp);	fp=NULL;
#endif*/

//	if(dis)		delete[] dis;	dis=NULL;
//	if(rms)	delete[] rms;	rms=NULL;

	if(rms)			delete rms;		rms = NULL;
	if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
	if(pPlaneLut)		delete pPlaneLut;		pPlaneLut=NULL;
	if(L)			delete L;			L=NULL;
	if(X)			delete X;			X=NULL;
	if(pPDC)		delete pPDC;		pPDC=NULL;
	if(hx)			delete hx;			hx=NULL;
	if(hxx)			delete hxx;			hxx=NULL;
	if(dis)			delete dis;			dis=NULL;
//	if(covar)		delete covar;		covar=NULL;
	return 0;
}

//按条件平差方式解算
//A1*x1+A2*x2+B*V+w=0
//附加条件：G*x2+wc=0
// int condition_adjustment(const int nPlane, 
// 						  LidMC_Plane *pLidPlane, 
// 						  double *pX, 
// 						  const int pnp
// )
// {
// 	int i, j, k;
// 	int ndim, npd;
// 	double *A1=NULL, *A2=NULL;	//A1: 标定参数偏导， A2: 平面参数偏导 
// 	double *B=NULL, *W=NULL;
// 	double *G=NULL, *Wc=NULL;
// 
// 	double a, b, c, d;	//平面参数  ax+by+cz+d=0
// 
// 	int nLidPoints;		/* 点数 */ 
// 	int nParam;			/* 平面参数总数 */
// 
// 	ATNPoint *pLidPoints=NULL;	/* 激光点观测值	*/
// 	double  *pPlane=NULL;		/* 平面参数 */
// 	long *pPlaneLut=NULL;	//每个平面所属点的查找表
// 
// 	double *dpX=NULL;		//标定参数的改正数
// 	double *pdPlane=NULL;	//平面参数的改正数
// 	double *pPDC=NULL;
// 	double *pdX, *pdY, *pdZ;
// 	
// 	
// 	nParam=nPlane*4;	//平面的一般方程
// 	pPlane=new double[nParam];
// 	pPlaneLut=new long[nPlane+1];
// 
// 	pdX=new double[pnp];
// 	pdPlane=new double[nParam];
// 
// 	nLidPoints=0;
// 	for(i=0; i<nPlane; i++)
// 	{
// 		pPlane[i+4]=pLidPlane[i].s1;
// 		pPlane[i*4+1]=pLidPlane[i].s2;
// 		pPlane[i*4+2]=pLidPlane[i].s3;
// 		pPlane[i*4+3]=pLidPlane[i].s4;
// 
// 		pPlaneLut[i+1]=pLidPlane[i].pLidPoints->size();
// 		nLidPoints+=pLidPlane[i].pLidPoints->size();
// 	}
// 	pPlaneLut[0]=0;
// 
// 	pLidPoints=new ATNPoint[nLidPoints];
// 	k=0;
// 	for(i=0; i<nPlane; i++)
// 	{
// 		std::vector<ATNPoint> *pVec=pLidPlane[i].pLidPoints;
// 		for(j=0; j<pVec->size(); j++)
// 		{
// 			pLidPoints[k++]=(*pVec)[j];
// 		}
// 	}
// 
// 	ndim=3;		//x,y,z
// 	npd=6;		//dtx, dty, dtz, dr, dp, dh
// 	
// 	pPDC=new double[nLidPoints*ndim*npd];
// 	A1=new double[nLidPoints*npd];
// 	
// 	do 
// 	{
// 		_calLaserPosition(pLidPoints, nLidPoints, pX[0], pX[1], pX[2], pX[3], pX[4], pX[5]);
// 		
// 		_calPDC_misalign(pLidPoints, nLidPoints, pX+3, pPDC, ndim, npd);
// 
// 		//计算A1, A2, B, W
// 		for(i=0; i<nPlane; i++)
// 		{//遍历平面
// 			a=pPlane[i*4];
// 			b=pPlane[i*4+1];
// 			c=pPlane[i*4+2];
// 			d=pPlane[i*4+3];
// 
// 			for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 			{//遍历平面内的点
// 				pdX=pPDC+j*ndim*npd;
// 				pdY=pdX+npd;
// 				pdZ=pdY+npd;
// 
// 				A1[j*npd]  =a*pdX[0]+b*pdY[0]+c*pdZ[0];
// 				A1[j*npd+1]=a*pdX[1]+b*pdY[1]+c*pdZ[1];
// 				A1[j*npd+2]=a*pdX[2]+b*pdY[2]+c*pdZ[2];
// 				A1[j*npd+3]=a*pdX[3]+b*pdY[3]+c*pdZ[3];
// 				A1[j*npd+4]=a*pdX[4]+b*pdY[4]+c*pdZ[4];
// 				A1[j*npd+5]=a*pdX[5]+b*pdY[5]+c*pdZ[5];
// 			}
// 
// 		}
// 
// 		//计算G，wc
// 		
// 	} while (1);
// 
// 
// 
// 
// 
// 	if(pLidPoints) delete pLidPoints;	pLidPoints=NULL;
// 	if(pPlane)	delete pPlane;	pPlane=NULL;
// 	if(pPlaneLut)	delete pPlaneLut;	pPlaneLut=NULL;
// 	if(pdX)		delete pdX;		pdX=NULL;
// 	if(pdPlane) delete pdPlane;	pdPlane=NULL;
// 
// 	if(pPDC)	delete pPDC;	pPDC=NULL;
// 	if(A1)		delete A1;		A1=NULL;
// 	return 0;
// }

//d=(ax+by+cz+d)/sqrt(a^2+b^2+c^2)
// int indirect_adjustment(const int nPlane, 
// 						LidMC_Plane *pLidPlane, 
// 						double *pX, 
// 						const int pnp
// 						)
// {
// 	int i, j, k;
// 	int ndim, npd;
// 	double *C=NULL;	 
// 	double *B=NULL, *L=NULL;
// 	
// 	double *pPlane=NULL;		//初始平面参数
// 	double *X=NULL;		//初始标定参数
// 
// 	double a, b, c, d;	//平面参数  ax+by+cz+d=0
// 	double a2, b2, c2;
// 
// 	int nLidPoints;		/* 点数 */ 
// 	int nParam;			/* 平面参数总数 */
// 
// 	ATNPoint *pLidPoints=NULL;	/* 激光点观测值	*/
// //	double  *pPlane=NULL;		/* 平面参数 */
// 	long *pPlaneLut=NULL;	//每个平面所属点的查找表
// 
// 	double *dX=NULL;		//标定参数的改正数
// 	double *dPlane=NULL;	//平面参数的改正数
// 	double *pPDC=NULL;
// 	double *pdX, *pdY, *pdZ;
// 	double s, s3;
// 	double xp, yp, zp;
// 	
// 	
// 	nParam=nPlane*4;	//平面的一般方程
// 	pPlane=new double[nParam];
// 	pPlaneLut=new long[nPlane+1];
// 
// 	dX=new double[pnp];
// 	dPlane=new double[nParam];
// 
// 	nLidPoints=0;
// 	for(i=0; i<nPlane; i++)
// 	{
// 		pPlane[i+4]=pLidPlane[i].s1;
// 		pPlane[i*4+1]=pLidPlane[i].s2;
// 		pPlane[i*4+2]=pLidPlane[i].s3;
// 		pPlane[i*4+3]=pLidPlane[i].s4;
// 
// 		pPlaneLut[i+1]=pLidPlane[i].pLidPoints->size();
// 		nLidPoints+=pLidPlane[i].pLidPoints->size();
// 	}
// 	pPlaneLut[0]=0;
// 
// 	pLidPoints=new ATNPoint[nLidPoints];
// 	k=0;
// 	for(i=0; i<nPlane; i++)
// 	{
// 		std::vector<ATNPoint> *pVec=pLidPlane[i].pLidPoints;
// 		for(j=0; j<pVec->size(); j++)
// 		{
// 			pLidPoints[k++]=(*pVec)[j];
// 		}
// 	}
// 
// 	ndim=3;		//x,y,z
// 	npd=6;		//dtx, dty, dtz, dr, dp, dh
// 	
// 	pPDC=new double[nLidPoints*ndim*npd];
// 	B=new double[nLidPoints*npd];
// 	C=new double[nLidPoints*4*nPlane];	//未采用压缩存储
// 	L=new double[nLidPoints];
// 	
// 	do 
// 	{
// 		_calLaserPosition(pLidPoints, nLidPoints, pX[0], pX[1], pX[2], pX[3], pX[4], pX[5]);
// 		
// 		_calPDC_misalign(pLidPoints, nLidPoints, pX+3, pPDC, ndim, npd);
// 
// 		//计算 B, C, L
// 		for(i=0; i<nPlane; i++)
// 		{//遍历平面
// 			a=pPlane[i*4];
// 			b=pPlane[i*4+1];
// 			c=pPlane[i*4+2];
// 			d=pPlane[i*4+3];
// 
// 			a2=a*a;	
// 			b2=b*b;
// 			c2=c*c;
// 
// 			s=1.0/sqrt(a*a+b*b+c*c);
// 			s3=s*s*s;
// 
// 			for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 			{//遍历平面内的点
// 				pdX=pPDC+j*ndim*npd;
// 				pdY=pdX+npd;
// 				pdZ=pdY+npd;
// 
// 				xp=pLidPoints[j].x;
// 				yp=pLidPoints[j].y;
// 				zp=pLidPoints[j].z;
// 
// 				B[j*npd]  =s*(a*pdX[0]+b*pdY[0]+c*pdZ[0]);
// 				B[j*npd+1]=s*(a*pdX[1]+b*pdY[1]+c*pdZ[1]);
// 				B[j*npd+2]=s*(a*pdX[2]+b*pdY[2]+c*pdZ[2]);
// 				B[j*npd+3]=s*(a*pdX[3]+b*pdY[3]+c*pdZ[3]);
// 				B[j*npd+4]=s*(a*pdX[4]+b*pdY[4]+c*pdZ[4]);
// 				B[j*npd+5]=s*(a*pdX[5]+b*pdY[5]+c*pdZ[5]);
// 
// 				C[j*4*nPlane+i*4]  =(xp*(b2+c2)-a*(b*yp+c*zp+d))*s3;
// 				C[j*4*nPlane+i*4+1]=(yp*(a2+c2)-b*(a*xp+c*zp+d))*s3;
// 				C[j*4*nPlane+i*4+2]=(zp*(a2+b2)-c*(a*xp+b*yp+d))*s3;
// 				C[j*4*nPlane+i*4+3]=s;
// 			}
// 
// 		}
// 
// 		//计算G，wc
// 		
// 	} while (1);
// 
// 
// 
// 
// 
// 	if(pLidPoints) delete pLidPoints;	pLidPoints=NULL;
// 	if(pPlane)	delete pPlane;	pPlane=NULL;
// 	if(pPlaneLut)	delete pPlaneLut;	pPlaneLut=NULL;
// 	if(dX)		delete dX;		dX=NULL;
// 	if(dPlane)	delete dPlane;	dPlane=NULL;
// 
// 	if(pPDC)	delete pPDC;	pPDC=NULL;
// 	if(B)		delete B;		B=NULL;
// 	if(C)		delete C;		C=NULL;
// 	if(L)		delete L;		L=NULL;
// 
// 	return 0;	
// }


// int CLidCalib_Skaloud::LidMC_Misalign_plane_zconstraint(
// 				const int nVP,
// 				LidMC_VP *pLidVP,			 
// 				const int nPlane,	/* number of planes */
// 				LidMC_Plane *pLidPlane,
// 				double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
// 											* aj are the image j parameters, bi are the i-th point parameters,
// 											* size m*cnp + n*pnp
// 											未知数，偏心分量和安置角    
// 											pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
// 				const int pnp			/* number of parameters 未知数个数*/
// 				 //double *pObs,			/*观测值(虚拟观测值，加权平均值)*/
// 				 //double errTh2			//(迭代终止条件,物方坐标改正);
// )
// {
// 	int i, j, k;
// 	int ndim/*, npd*/;
// 	double *C=NULL;	 
// 	double *B=NULL, *L=NULL;
// 
// 	double *pPlane=NULL;		//初始平面参数
// 	double *X=NULL;		//初始标定参数
// 
// 	//	double a, b, c, d;	//平面参数  ax+by+cz+d=0
// 	//	double a2, b2, c2;
// 
// 	int nVPTies, nPlaneTies;
// 	int nLidPoints;		/* 点数 */ 
// 	int nParam;			/* 平面参数总数 */
// 
// 	ATNPoint *pLidPoints=NULL, *pATNVP, *pATNPlane;	/* 激光点观测值	*/
// 	//	double  *pPlane=NULL;		/* 平面参数 */
// 	long *pPlaneLut=NULL;	//每个平面所属点的查找表
// 	long *pVPLut=NULL;
// 
// 	double *dX=NULL;		//标定参数的改正数
// 	double *dPlane=NULL;	//平面参数的改正数
// 	double *pPDC=NULL;
// 	//	double *pdX, *pdY, *pdZ;
// 	//	double s, s3;
// 	//	double xp, yp, zp;
// 
// //统计VP
// 	int tieID=-1;
// 	nVPTies=0;		
// 	for(i=0; i<nVP; i++)
// 	{
// 		if(tieID!=pLidVP[i].tieID)
// 		{
// 			nVPTies++;
// 			tieID=pLidVP[i].tieID;
// 		}
// 	}
// 	
// //	ASSERT(nVPTies==nVP);
// 
// 	tieID=-1;
// 	nPlaneTies=0;
// 	for(i=0; i<nPlane; i++)
// 	{
// 		if(tieID!=pLidPlane[i].tieID)
// 		{
// 			nPlaneTies++;
// 			tieID=pLidPlane[i].tieID;
// 		}
// 	}
// 
// 
// 	nParam=nPlaneTies*4;	//平面的一般方程
// 	// 	pPlane=new double[nParam];
// 	pPlaneLut=new long[nPlaneTies+1];
// 	pVPLut=new long[nVPTies+1];
// 	// 	
// 	// 	dX=new double[pnp];
// 	// 	dPlane=new double[nParam];
// 
// 	X=new double[pnp+4*nPlaneTies];
// 	for(i=0; i<pnp; i++)
// 	{
// 		X[i]=pX[i];
// 	}
// 	pPlane=X+pnp;
// 
// 	nLidPoints=0;
// 	for(i=0, j=-1; i<nVP; i++)
// 	{
// 		if(j==pLidVP[i].tieID)
// 		{
// 			pVPLut[j+1]+=pLidVP[i].pLidPoints->size();
// 			nLidPoints+=pLidVP[i].pLidPoints->size();
// 		}
// 		else
// 		{
// 			j=pLidVP[i].tieID;
// 			
// 			pVPLut[j+1]=pLidVP[i].pLidPoints->size()+nLidPoints;
// 			nLidPoints+=pLidVP[i].pLidPoints->size();
// 		}
// 	}
// 	pVPLut[0]=0;
// 
// 	for(i=0; i<nPlane; i++)
// 	{//计算初始平面参数，用ransac挑点
// 		TRACE("%d ", (pLidPlane+i)->pLidPoints->size());
// 		PlaneFitting_ransac(pLidPlane+i);
// 		TRACE("--> %d\n", (pLidPlane+i)->pLidPoints->size());
// 	}
// 
// 	//统计连接面数量, tieID必须连续排列
// 	//需要保证传入的连接面按tieID顺序组织
// 	int maxPtNum=0;
// //	nLidPoints=0;
// 	for(i=0, j=-1; i<nPlane; i++)
// 	{
// 		if(j==pLidPlane[i].tieID)
// 		{
// 			if(maxPtNum<pLidPlane[i].pLidPoints->size())
// 			{//取点数最多的条带作为初始平面参数
// 				maxPtNum=pLidPlane[i].pLidPoints->size();
// 
// 				pPlane[j*4]  =pLidPlane[i].s1;
// 				pPlane[j*4+1]=pLidPlane[i].s2;
// 				pPlane[j*4+2]=pLidPlane[i].s3;
// 				pPlane[j*4+3]=pLidPlane[i].s4;
// 			}
// 			
// 			pPlaneLut[j+1]+=pLidPlane[i].pLidPoints->size();
// 			nLidPoints+=pLidPlane[i].pLidPoints->size();
// 		}
// 		else
// 		{
// 			j=pLidPlane[i].tieID;
// 			maxPtNum=pLidPlane[i].pLidPoints->size();
// 
// 			pPlane[j*4]  =pLidPlane[i].s1;
// 			pPlane[j*4+1]=pLidPlane[i].s2;
// 			pPlane[j*4+2]=pLidPlane[i].s3;
// 			pPlane[j*4+3]=pLidPlane[i].s4;
// 
// 			pPlaneLut[j+1]=pLidPlane[i].pLidPoints->size()+nLidPoints;
// 			nLidPoints+=pLidPlane[i].pLidPoints->size();
// 		}
// 
// 	}
// 	pPlaneLut[0]=pVPLut[nVPTies];
// 
// 	pLidPoints=new ATNPoint[nLidPoints];
// 	k=0;
// 	for(i=0; i<nVP; i++)
// 	{
// 		std::vector<ATNPoint> *pVec=pLidVP[i].pLidPoints;
// 		for(j=0; j<pVec->size(); j++)
// 		{
// 			pLidPoints[k++]=(*pVec)[j];
// 		}
// 	}
// 
// 	for(i=0; i<nPlane; i++)
// 	{
// 		std::vector<ATNPoint> *pVec=pLidPlane[i].pLidPoints;
// 		for(j=0; j<pVec->size(); j++)
// 		{
// 			pLidPoints[k++]=(*pVec)[j];
// 		}
// 	}
// 
// 	ndim=3;		//x,y,z
// 	//	npd=6;		//dtx, dty, dtz, dr, dp, dh
// 
// 	pPDC=new double[nLidPoints*ndim*pnp];
// 	// 	B=new double[nLidPoints*npd];
// 	// 	C=new double[nLidPoints*4*nTies];	//未采用压缩存储
// 	L=new double[nLidPoints];
// 	for(i=0; i<nLidPoints; i++)
// 	{
// 		L[i]=0;
// 	}
// 	
// 	//遍历连接点
// 	for(i=0; i<nVPTies; i++)
// 	{
// 		for(j=pVPLut[i]; j<pVPLut[i+1]; j++)
// 		{
// 			L[j]=pLidVP[i].eZ;		//每个点的观测值对应到连接点上
// 		}
// 	}
// 
// 	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
// 	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;
// 
// 	struct myObs data;
// 	data.nMisalign=pnp;
// 	data.nTiePlane=nPlaneTies;
// 	data.nPoints=nLidPoints;
// 	data.pLidPoints=pLidPoints;
// 	data.pPlaneLut=pPlaneLut;
// 	data.nTieVP=nVPTies;
// 	data.pVPLut=pVPLut;
// 	data.pPDC=pPDC;
// 	data.ndim=3;
// 
// 
// #ifdef _DEBUG 
// 	FILE *fp=NULL;
// 	fp=fopen(/*"g:/D_experiment/adjustment/6param/plane_adjust_6pZC"*/m_pResultName, "wt");
// 
// 	double *dis=NULL;
// 	dis=new double[nLidPoints];
// 	double *rms=NULL;
// 	rms=new double[nPlaneTies];
// 
// 	//统计初始连接面的距离方差
// 	calObserves_PointToPlane_ZConstraint(X, dis, pnp+4*nPlaneTies, nLidPoints, &data);
// 	fprintf(fp, "## init VP rms\n");
// 	for(i=0; i<nVPTies; i++)
// 	{
// 		rms[i]=0;
// 		for(j=pVPLut[i]; j<pVPLut[i+1]; j++)
// 		{
// 			rms[i]+=(dis[j]-L[j])*(dis[j]-L[j]);
// 		}
// 		rms[i]/=(pVPLut[i+1]-pVPLut[i]);
// 		rms[i]=sqrt(rms[i]);
// 		fprintf(fp, "%d num:%d rms:%.6lf\n", i, pVPLut[i+1]-pVPLut[i], rms[i]);
// 	}
// 
// 	fprintf(fp, "## init tie plane rms\n");
// 	for(i=0; i<nPlaneTies; i++)
// 	{//遍历平面
// 		rms[i]=0;
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			rms[i]+=dis[j]*dis[j];
// 		}
// 		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
// 		rms[i]=sqrt(rms[i]);
// 		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
// 		//TRACE("%.6lf\n", rms[i]);
// 	}
// #endif
// 
// 	int ret=dlevmar_der(calObserves_PointToPlane_ZConstraint, jac_dis_PointToPlane_ZConstraint, X, L, pnp+nPlaneTies*4, nLidPoints, 1000, 
// 		opts, info, NULL, NULL, &data);
// 
// #ifdef _DEBUG 
// 	
// 	fprintf(fp, "## resolving results\n");
// 	fprintf(fp, "## misalign\n");
// 	fprintf(fp, "%.9lf\n", X[0]);
// 	fprintf(fp, "%.9lf\n", X[1]);
// 	fprintf(fp, "%.9lf\n", X[2]);
// 	fprintf(fp, "%.9lf\n", X[3]);
// 	fprintf(fp, "%.9lf\n", X[4]);
// 	fprintf(fp, "%.9lf\n", X[5]);
// 
// 	pPlane=X+pnp;
// 	fprintf(fp, "## %d tie_plane param\n", nPlaneTies);
// 	for(i=0; i<nPlaneTies; i++)
// 	{
// 		fprintf(fp, "%.6lf\n", pPlane[i*4]);
// 		fprintf(fp, "%.6lf\n", pPlane[i*4+1]);
// 		fprintf(fp, "%.6lf\n", pPlane[i*4+2]);
// 		fprintf(fp, "%.6lf\n", pPlane[i*4+3]);
// 	}
// 
// 
// 	//计算每个连接面的中误差
// 	calObserves_PointToPlane_ZConstraint(X, dis, pnp+4*nPlaneTies, nLidPoints, &data);
// 	
// 	fprintf(fp, "## adjusted VP rms\n");
// 	for(i=0; i<nVPTies; i++)
// 	{
// 		rms[i]=0;
// 		for(j=pVPLut[i]; j<pVPLut[i+1]; j++)
// 		{
// 			rms[i]+=(dis[j]-L[j])*(dis[j]-L[j]);
// 		}
// 		rms[i]/=(pVPLut[i+1]-pVPLut[i]);
// 		fprintf(fp, "%d num:%d rms:%.6lf\n", i, pVPLut[i+1]-pVPLut[i], rms[i]);
// 	}
// 
// 	fprintf(fp, "## tie_plane rms\n");
// 	for(i=0; i<nPlaneTies; i++)
// 	{//遍历平面
// 		rms[i]=0;
// 		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
// 		{//遍历平面内的点
// 			rms[i]+=dis[j]*dis[j];
// 		}
// 		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
// 		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
// 	}
// 
// 	if(fp)	fclose(fp);	fp=NULL;
// #endif
// 
// 	if(dis)		delete[] dis;	dis=NULL;
// 	if(rms)	delete[] rms;	rms=NULL;
// 
// 	if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
// 	if(pPlaneLut)		delete pPlaneLut;		pPlaneLut=NULL;
// 	if(pVPLut)	delete pVPLut;		pVPLut=NULL;
// 	if(L)			delete L;				L=NULL;
// 	if(X)			delete X;			X=NULL;
// 	if(pPDC)		delete pPDC;		pPDC=NULL;
// 	return 0;
// }

//平面平差精度检查
int CLidCalib_Skaloud::LidMC_PlaneAdj_QA(
					  const int nPlane,	/* number of planes */
					  LidMC_Plane *pLidPlane,
					  double *pInitX,			/* 未知数初值*/
					  double *pAdjX,			/* 未知数平差值*/
					  const int pnp,			/* number of parameters 未知数个数*/
					  orsLidSysParam oriParam,	//系统误差初值
					  const CalibParam_Type type	//误差类型	
)
{
	int i, j, k;
	int ndim/*, npd*/;
//	double *C=NULL;	 
//	double /**B=NULL,*/ *L=NULL;

// 	double *pPlane=NULL;		//初始平面参数
// 	double *X=NULL;		//初始标定参数

	//	double a, b, c, d;	//平面参数  ax+by+cz+d=0
	//	double a2, b2, c2;

	int nTies;
	int nLidPoints;		/* 点数 */ 
	int nParam;			/* 平面参数总数 */

	LidPt_SurvInfo *pLidPoints=NULL;	/* 激光点观测值	*/
	//	double  *pPlane=NULL;		/* 平面参数 */
	long *pPlaneLut=NULL;	//每个平面所属点的查找表

//	double *dX=NULL;		//标定参数的改正数
//	double *dPlane=NULL;	//平面参数的改正数
	double tx, ty, tz, r, p, h, dxs, dys, dzs;


	for(i=0; i<nPlane; i++)
	{//计算初始平面参数，用ransac挑点
		TRACE("%d ", (pLidPlane+i)->pLidPoints->size());
		PlaneFitting_ransac(pLidPlane+i);
		TRACE("--> %d\n", (pLidPlane+i)->pLidPoints->size());
	}

	//统计连接面数量, tieID必须连续排列
	int tieID=-1;
	nTies=0;
	for(i=0; i<nPlane; i++)
	{
		if(tieID!=pLidPlane[i].tieID)
		{
			nTies++;
			tieID=pLidPlane[i].tieID;
		}
	}
	//	nTies+=1;
	j=tieID+1-nTies;
	if(j>0)
	{//连接点编号不是从0开始
		for(i=0; i<nPlane; i++)
		{//重新整理连接点号
			pLidPlane[i].tieID=pLidPlane[i].tieID-j;
		}	
	}

	nParam=nTies*4;	//平面的一般方程
	// 	pPlane=new double[nParam];
	pPlaneLut=new long[nTies+1];
	// 	
	// 	dX=new double[pnp];
	// 	dPlane=new double[nParam];

// 	X=new double[pnp+4*nTies];
// 	for(i=0; i<pnp; i++)
// 	{
// 		X[i]=pInitX[i];
// 	}
// 	pPlane=X+pnp;

	//需要保证传入的连接面按tieID顺序组织
	int maxPtNum=0;
	nLidPoints=0;
	for(i=0, j=-1; i<nPlane; i++)
	{
		if(j==pLidPlane[i].tieID)
		{
			if(maxPtNum<pLidPlane[i].pLidPoints->size())
			{//取点数最多的条带作为初始平面参数
				maxPtNum=pLidPlane[i].pLidPoints->size();

// 				pPlane[j*4]  =pLidPlane[i].s1;
// 				pPlane[j*4+1]=pLidPlane[i].s2;
// 				pPlane[j*4+2]=pLidPlane[i].s3;
// 				pPlane[j*4+3]=pLidPlane[i].s4;
			}

			pPlaneLut[j+1]+=pLidPlane[i].pLidPoints->size();
			nLidPoints+=pLidPlane[i].pLidPoints->size();
		}
		else
		{
			j=pLidPlane[i].tieID;
			maxPtNum=pLidPlane[i].pLidPoints->size();

// 			pPlane[j*4]  =pLidPlane[i].s1;
// 			pPlane[j*4+1]=pLidPlane[i].s2;
// 			pPlane[j*4+2]=pLidPlane[i].s3;
// 			pPlane[j*4+3]=pLidPlane[i].s4;

			pPlaneLut[j+1]=pLidPlane[i].pLidPoints->size()+nLidPoints;
			nLidPoints+=pLidPlane[i].pLidPoints->size();
		}

	}
	pPlaneLut[0]=0;

	pLidPoints=new LidPt_SurvInfo[nLidPoints];
	k=0;
	for(i=0; i<nPlane; i++)
	{
		std::vector<LidPt_SurvInfo> *pVec=pLidPlane[i].pLidPoints;
		for(j=0; j<pVec->size(); j++)
		{
			pLidPoints[k++]=(*pVec)[j];
		}
	}

	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_RangeVec(pLidPoints, nLidPoints);

	ndim=3;		//x,y,z
	//	npd=6;		//dtx, dty, dtz, dr, dp, dh

	struct myObs data;
	data.nMisalign=pnp;
	data.nTiePlane=nTies;
	data.nPoints=nLidPoints;
	data.pLidPoints=pLidPoints;
	data.pPlaneLut=pPlaneLut;
//	data.pPDC=pPDC;
	data.ndim=3;

	data.type=type;
	data.oriParam = oriParam;


 	CString  strFileName;
// 	if(paramtype==LiDMC_rph)
// 		strFileName="g:/D_experiment/adjustment/QA/plane_rph";
// 	else if(paramtype==LiDMC_xyzrph)
// 		strFileName="g:/D_experiment/adjustment/QA/plane_xyzrph";
// 	else if(paramtype==LiDMC_xyzrph_SemiXYZ)
// 		strFileName="g:/D_experiment/adjustment/QA/plane_xyzrph_SemiXYZ";
// 	else if(paramtype==LiDMC_rph_SemiXYZ)
// 		strFileName="g:/D_experiment/adjustment/QA/plane_rph_SemiXYZ";

	CFileDialog output(FALSE," ",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);
	output.m_ofn.lpstrTitle="保存检查平面精度结果";

	if(output.DoModal()!=IDOK)
		return -1;
	strFileName = output.GetPathName();

	FILE *fp=NULL;
	fp=fopen(/*strFileName.GetBuffer(128)*/strFileName, "wt");

	double *dis=NULL;
	dis=new double[nLidPoints];
	double *rms=NULL;
	rms=new double[nTies];


	//统计初始连接面的距离方差
//	_calLaserPosition(pLidPoints, nLidPoints, tx, ty, tz, r, p, h);
// 	if(paramtype==LiDMC_xyzrph_SemiXYZ || paramtype==LiDMC_rph_SemiXYZ)
// 	{
// 		for(i=0; i<nLidPoints; i++)
// 		{
// 			pLidPoints[i].x+=xs;
// 			pLidPoints[i].y+=ys;
// 			pLidPoints[i].z+=zs;
// 		}
// 	}

	POINT3D  *pPlanePtBuf=NULL;
	pPlanePtBuf=new POINT3D[nLidPoints];

	//计算点坐标
//	_calLaserPosition(pLidPoints, nLidPoints, oriParam, pInitX, type);
	
	orsLidSysParam curParam = oriParam;
	switch (type)
	{
	case Calib_rph:
		curParam.boresight_angle[0] = pInitX[0];
		curParam.boresight_angle[1] = pInitX[1];
		curParam.boresight_angle[2] = pInitX[2];
		break;
	case Calib_rphxyz:
		curParam.boresight_angle[0] = pInitX[0];
		curParam.boresight_angle[1] = pInitX[1];
		curParam.boresight_angle[2] = pInitX[2];
		curParam.lever_arm[0] = pInitX[3];
		curParam.lever_arm[1] = pInitX[4];
		curParam.lever_arm[2] = pInitX[5];
		break;
	case Calib_rph_appXYZ:
		curParam.boresight_angle[0] = pInitX[0];
		curParam.boresight_angle[1] = pInitX[1];
		curParam.boresight_angle[2] = pInitX[2];
		curParam.semiXYZ[0] = pInitX[3];
		curParam.semiXYZ[1] = pInitX[4];
		curParam.semiXYZ[2] = pInitX[5];
		break;
	case Calib_rphxyz_sa0:
		curParam.boresight_angle[0] = pInitX[0];
		curParam.boresight_angle[1] = pInitX[1];
		curParam.boresight_angle[2] = pInitX[2];
		curParam.lever_arm[0] = pInitX[3];
		curParam.lever_arm[1] = pInitX[4];
		curParam.lever_arm[2] = pInitX[5];
		curParam.sa0 = pInitX[6];
		break;
	case Calib_rphxyz_sa0_sac:
		curParam.boresight_angle[0] = pInitX[0];
		curParam.boresight_angle[1] = pInitX[1];
		curParam.boresight_angle[2] = pInitX[2];
		curParam.lever_arm[0] = pInitX[3];
		curParam.lever_arm[1] = pInitX[4];
		curParam.lever_arm[2] = pInitX[5];
		curParam.sa0 = pInitX[6];
		curParam.sac = pInitX[7];
		break;
	case Calib_rph_sa0:
		curParam.boresight_angle[0] = pInitX[0];
		curParam.boresight_angle[1] = pInitX[1];
		curParam.boresight_angle[2] = pInitX[2];
		curParam.sa0 = pInitX[3];
		break;
	case Calib_rph_drange:
		curParam.boresight_angle[0] = pInitX[0];
		curParam.boresight_angle[1] = pInitX[1];
		curParam.boresight_angle[2] = pInitX[2];
		curParam.drange = pInitX[3];
		break;
	case Calib_rph_drange_sa0:
		curParam.boresight_angle[0] = pInitX[0];
		curParam.boresight_angle[1] = pInitX[1];
		curParam.boresight_angle[2] = pInitX[2];
		curParam.drange = pInitX[3];
		curParam.sa0 = pInitX[4];
		break;
	default:
		;
	}
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_Laserfootprint(pLidPoints, nLidPoints);

	for(i=0; i<nLidPoints; i++)
	{
		pPlanePtBuf[i].X=pLidPoints[i].x;
		pPlanePtBuf[i].Y=pLidPoints[i].y;
		pPlanePtBuf[i].Z=pLidPoints[i].z;
	}
	double Mout[3];

	fprintf(fp, "init plane rms\n");
	fprintf(fp, "## %d tie_plane param\n", nTies);
	double a, b, c, d, s;
	double xp, yp ,zp;
	for(i=0; i<nTies; i++)
	{//遍历平面
		j=pPlaneLut[i];
		plane_fitting(pPlaneLut[i+1]-pPlaneLut[i], pPlanePtBuf+j, Mout);

		a=Mout[0];
		b=Mout[1];
		c=-1;
		d=Mout[2];
		s=1.0/sqrt(a*a+b*b+c*c);
		fprintf(fp, "%.6lf, %.6lf, %.6lf, %.6lf\n", a, b, c, d);

		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			xp=pLidPoints[j].x;
			yp=pLidPoints[j].y;
			zp=pLidPoints[j].z;

			dis[j]=(a*xp+b*yp+c*zp+d)*s;
		}
	}

	fprintf(fp, "## tie_plane rms\n");

//	distance_PointToPlane(X, dis, pnp+4*nTies, nLidPoints, &data);
	
	TRACE("distance init\n");
	for(i=0; i<nTies; i++)
	{//遍历平面

		if(i==4)
		{
			TRACE("ID=%d, num=%d\n", i, pPlaneLut[i+1]-pPlaneLut[i]);
		}

		rms[i]=0;
		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			rms[i]+=dis[j]*dis[j];

			if(i==4)
			{
				TRACE("%.3f\n", dis[j]);
			}
		}
		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
		rms[i]=sqrt(rms[i]);
		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
	}

//	_calLaserPosition(pLidPoints, nLidPoints, oriParam, pAdjX, type);
	curParam = oriParam;

	switch (type)
	{
	case Calib_rph:
		curParam.boresight_angle[0] = pAdjX[0];
		curParam.boresight_angle[1] = pAdjX[1];
		curParam.boresight_angle[2] = pAdjX[2];
		break;
	case Calib_rphxyz:
		curParam.boresight_angle[0] = pAdjX[0];
		curParam.boresight_angle[1] = pAdjX[1];
		curParam.boresight_angle[2] = pAdjX[2];
		curParam.lever_arm[0] = pAdjX[3];
		curParam.lever_arm[1] = pAdjX[4];
		curParam.lever_arm[2] = pAdjX[5];
		break;
	case Calib_rph_appXYZ:
		curParam.boresight_angle[0] = pAdjX[0];
		curParam.boresight_angle[1] = pAdjX[1];
		curParam.boresight_angle[2] = pAdjX[2];
		curParam.semiXYZ[0] = pAdjX[3];
		curParam.semiXYZ[1] = pAdjX[4];
		curParam.semiXYZ[2] = pAdjX[5];
		break;
	case Calib_rphxyz_sa0:
		curParam.boresight_angle[0] = pAdjX[0];
		curParam.boresight_angle[1] = pAdjX[1];
		curParam.boresight_angle[2] = pAdjX[2];
		curParam.lever_arm[0] = pAdjX[3];
		curParam.lever_arm[1] = pAdjX[4];
		curParam.lever_arm[2] = pAdjX[5];
		curParam.sa0 = pAdjX[6];
		break;
	case Calib_rphxyz_sa0_sac:
		curParam.boresight_angle[0] = pAdjX[0];
		curParam.boresight_angle[1] = pAdjX[1];
		curParam.boresight_angle[2] = pAdjX[2];
		curParam.lever_arm[0] = pAdjX[3];
		curParam.lever_arm[1] = pAdjX[4];
		curParam.lever_arm[2] = pAdjX[5];
		curParam.sa0 = pAdjX[6];
		curParam.sac = pAdjX[7];
		break;
	case Calib_rph_sa0:
		curParam.boresight_angle[0] = pAdjX[0];
		curParam.boresight_angle[1] = pAdjX[1];
		curParam.boresight_angle[2] = pAdjX[2];
		curParam.sa0 = pAdjX[3];
		break;
	case Calib_rph_drange:
		curParam.boresight_angle[0] = pAdjX[0];
		curParam.boresight_angle[1] = pAdjX[1];
		curParam.boresight_angle[2] = pAdjX[2];
		curParam.drange = pAdjX[3];
		break;
	case Calib_rph_drange_sa0:
		curParam.boresight_angle[0] = pAdjX[0];
		curParam.boresight_angle[1] = pAdjX[1];
		curParam.boresight_angle[2] = pAdjX[2];
		curParam.drange = pAdjX[3];
		curParam.sa0 = pAdjX[4];
		break;
	default:
		;
	}
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_Laserfootprint(pLidPoints, nLidPoints);
	
	for(i=0; i<nLidPoints; i++)
	{
		pPlanePtBuf[i].X=pLidPoints[i].x;
		pPlanePtBuf[i].Y=pLidPoints[i].y;
		pPlanePtBuf[i].Z=pLidPoints[i].z;
	}
//	double Mout[3];

	fprintf(fp, "## %d tie_plane param\n", nTies);
//	double a, b, c, d, s;
//	double xp, yp ,zp;
	for(i=0; i<nTies; i++)
	{//遍历平面
		j=pPlaneLut[i];
		plane_fitting(pPlaneLut[i+1]-pPlaneLut[i], pPlanePtBuf+j, Mout);

		a=Mout[0];
		b=Mout[1];
		c=-1;
		d=Mout[2];
		s=1.0/sqrt(a*a+b*b+c*c);
		fprintf(fp, "%.6lf, %.6lf, %.6lf, %.6lf\n", a, b, c, d);

		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			xp=pLidPoints[j].x;
			yp=pLidPoints[j].y;
			zp=pLidPoints[j].z;

			dis[j]=(a*xp+b*yp+c*zp+d)*s;
		}
	}

	fprintf(fp, "## tie_plane rms\n");

	TRACE("distance adjusted\n");
	for(i=0; i<nTies; i++)
	{//遍历平面
		rms[i]=0;
		if(i==4)
		{
			TRACE("ID=%d, num=%d\n", i, pPlaneLut[i+1]-pPlaneLut[i]);
		}

		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			rms[i]+=dis[j]*dis[j];

			if(i==4)
			{
				TRACE("%.3f\n", dis[j]);
			}
		}
		rms[i]/=(pPlaneLut[i+1]-pPlaneLut[i]);
		rms[i]=sqrt(rms[i]);
		fprintf(fp, "%d num:%d rms:%.6f\n", i, pPlaneLut[i+1]-pPlaneLut[i], rms[i]);
	}


	if(fp)	fclose(fp);	fp=NULL;
	if(pPlanePtBuf)		delete[] pPlanePtBuf;	pPlanePtBuf=NULL;
		
	if(dis)		delete[] dis;	dis=NULL;
	if(rms)	delete[] rms;	rms=NULL;

	if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
	if(pPlaneLut)		delete pPlaneLut;		pPlaneLut=NULL;
//	if(X)			delete X;			X=NULL;
	return 0;
}

