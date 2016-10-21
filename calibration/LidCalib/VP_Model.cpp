#include "StdAfx.h"
#include "LidCalib/VP_Model.h"
#include "bundler\matrix.h"
#include "Transform.h"
//#include "LidarGeometry.h"
//#include "levmar.h"
#include "F:\OpenRS\IntegratedPhotogrammetry\external\levmar-2.5\levmar.h"
#include "math.h"
#include "Geometry.h"

#include "orsBase/orsIPlatform.h"
#include "orsLidarGeometry/orsILidarGeoModel.h"

//平差中需要用到的计算函数名字前加下划线

//三角形插值
double TriangleInterpolate( std::vector<LidPt_SurvInfo> *pLidPoints, double x, double y )
{
	LidPt_SurvInfo p1, p2, p3;
	double dx1, dy1, dx21, dx31, dy21, dy31, dz21, dz31;

	p1 = (*pLidPoints)[0];
	p2 = (*pLidPoints)[1];
	p3 = (*pLidPoints)[2];

	dx1 = p1.x - x;	dy1 = p1.y - y;
	dx21 = p2.x - p1.x;	dy21 = p2.y - p1.y;	dz21 = p2.z - p1.z;	
	dx31 = p3.x - p1.x;	dy31 = p3.y - p1.y;	dz31 = p3.z - p1.z;

	return p1.z + ( dx1*( dy21*dz31 - dy31*dz21) - dy1*( dx21*dz31 - dx31*dz21) )
		/ ( dx21*dy31 - dx31*dy21 );
}

double TriangleInterpolate_time( std::vector<LidPt_SurvInfo> *pLidPoints, double x, double y )
{
	LidPt_SurvInfo p1, p2, p3;
	double dx1, dy1, dx21, dx31, dy21, dy31, dz21, dz31;

	p1 = (*pLidPoints)[0];
	p2 = (*pLidPoints)[1];
	p3 = (*pLidPoints)[2];

	dx1 = p1.x - x;	dy1 = p1.y - y;
	dx21 = p2.x - p1.x;	dy21 = p2.y - p1.y;	dz21 = p2.time - p1.time;	
	dx31 = p3.x - p1.x;	dy31 = p3.y - p1.y;	dz31 = p3.time - p1.time;

	return p1.time + ( dx1*( dy21*dz31 - dy31*dz21) - dy1*( dx21*dz31 - dx31*dz21) )
		/ ( dx21*dy31 - dx31*dy21 );
}

double TriangleInterpolate( POINT3D *Points, double x, double y )
{
	POINT3D p1, p2, p3;
	double dx1, dy1, dx21, dx31, dy21, dy31, dz21, dz31;

	p1 = Points[0];
	p2 = Points[1];
	p3 = Points[2];

	dx1 = p1.X - x;	dy1 = p1.Y - y;
	dx21 = p2.X - p1.X;	dy21 = p2.Y - p1.Y;	dz21 = p2.Z - p1.Z;	
	dx31 = p3.X - p1.X;	dy31 = p3.Y - p1.Y;	dz31 = p3.Z - p1.Z;

	return p1.Z + ( dx1*( dy21*dz31 - dy31*dz21) - dy1*( dx21*dz31 - dx31*dz21) )
		/ ( dx21*dy31 - dx31*dy21 );
}
//解算激光点坐标
// inline void _calLaserPosition(double Xs, double Ys, double Zs, double phi, double omega, double kappa, double *R,
// 							  /*double tx, double ty, double tz, double alpha, double beta, double gamma,*/
// 							  double *pCalibParam,
// 							  double range, double sAngle, double *x, double *y, double *z)
// {
// 	double  Rl[9];		//扫描镜旋转矩阵
// 	double  Rm[9];		//偏心角旋转矩阵
// 	double  Rn[9];		//imu旋转矩阵
// 	
// 	//	double  Rl[9];		//imu坐标系到局部参考坐标系旋转矩阵
// 	double  vec_range[3];
// 	double  vec_LFrame[3];	//扫描坐标系
// 	double  vec_IFrame[3];	//imu坐标系
// 	double  vec_TFrame[3];	//局部切平面坐标系
// //	double  Xgps, Ygps, Zgps;
// 	
// 	double tx, ty, tz, roll, pitch, heading;
// 
// 	tx=pCalibParam[0];	ty=pCalibParam[1];	tz=pCalibParam[2];
// 	roll=pCalibParam[3];	pitch=pCalibParam[4];	heading=pCalibParam[5];
// 
// 	///////扫描角旋转矩阵//////////////////////
// 	Rl[0]=1.0e0;	Rl[1]=0.0e0;		Rl[2]=0.0e0;
// 	Rl[3]=0.0e0;	Rl[4]=cos(sAngle);	Rl[5]=-sin(sAngle);
// 	Rl[6]=0.0e0;	Rl[7]=sin(sAngle);	Rl[8]=cos(sAngle);
// 	
// 
// 	if(R)
// 		memcpy(Rn, R, sizeof(double)*9);
// 	else
// 		RotateMat_fwk(phi, omega, kappa, Rn);
// 		
// 	vec_range[0]=vec_range[1]=0.0e0;
// 	vec_range[2]=range;
// 	
// 	matrix_product(3, 3, 3, 1, Rl, vec_range, vec_LFrame);
// 	
// 	RotateMat_rph(roll, pitch, heading, Rm);
// 
// //	matrix_ident(3, Rm);
// 
// 	matrix_product(3, 3, 3, 1, Rm, vec_LFrame, vec_IFrame);
// 	
// 	vec_IFrame[0]+=tx;
// 	vec_IFrame[1]+=ty;
// 	vec_IFrame[2]+=tz;
// 	
// 	matrix_product(3, 3, 3, 1, Rn, vec_IFrame, vec_TFrame);
// 	
// 	*x=vec_TFrame[0]+Xs;
// 	*y=vec_TFrame[1]+Ys;
// 	*z=vec_TFrame[2]+Zs;
// }


static int ptBufLen=10000;			//邻域内的最大点数不超过ptBufLen
//static POINT3D *pLaserPos=NULL;
static double delta_line_elem=0.01;
static double delta_angel_elem=0.001;
static double *pIDW_Weight=NULL;
static double *pSrcX=NULL, *pSrcY=NULL, *pSrcZ=NULL;
static double *pDstX=NULL, *pDstY=NULL,	*pDstZ=NULL;
//批量计算激光点坐标
// inline void _cal_batch_Laser_position(std::vector<ATNPoint> *pLidPoints, double pCalibPara[6], double *dstX, double *dstY, double *dstZ)
// {
// 	int j;
// 	for(j=0; j<pLidPoints->size(); j++)
// 	{//重新计算每个邻域点
// 		_calLaserPosition((*pLidPoints)[j].Xs, (*pLidPoints)[j].Ys, (*pLidPoints)[j].Zs, (*pLidPoints)[j].phi, (*pLidPoints)[j].omega, (*pLidPoints)[j].kappa,
// 			NULL, pCalibPara, (*pLidPoints)[j].range, (*pLidPoints)[j].scanAngle, dstX+j, dstY+j, dstZ+j);
// 	}
// }
//按IDW方法计算虚拟观测值:  距离反比加权法
inline double _cal_dummy_observation_IDW(double x, double y, double *pX, double *pY, double *pZ, double *pWeight, int num)
{
	int j; 
	double dis2;
	double weight_sum;		//权求和
	double z;

	weight_sum=0;
	for(j=0; j<num; j++)
	{
		dis2=(pX[j]-x)*(pX[j]-x)+(pY[j]-y)*(pY[j]-y);
		
		pWeight[j]=1.0/dis2;
		weight_sum+=pWeight[j];
		
	}
	
	z=0;
	for(j=0; j<num; j++)
	{
		z+=pWeight[j]*pZ[j]/weight_sum;
	}

	return z;
}
//计算观测方程中对未知数的偏导数——法方程系数
//采用中心差分法求解
// inline void _cal_PD_Patch_IDW(const int nPatch, 
// 							  LidMC_Patch *pLidPatch, 
// 							  const int pnp, 
// 							  double *pX, //tx0, ty0, tz0, alpha0, beta0, gamma0 
// 							  double *pA)
// {
// 	double z1, z2, dParam;
// 	int i, iPatch;
// 	std::vector<ATNPoint> *pLidPoints;
// 	double pCalibPara[6];
// 	
// 	iPatch=0;
// 	while(iPatch<nPatch)
// 	{
// 		pLidPoints=pLidPatch[iPatch].pLidPoints;
// 
// 		for(i=0; i<pnp; i++)
// 		{//对每个未知数求偏导
// 			memcpy(pCalibPara, pX, sizeof(double)*6);
// 
// 			pCalibPara[i]=pX[i]+delta_angel_elem;	//向前
// 			_cal_batch_Laser_position(pLidPoints, pCalibPara, pSrcX, pSrcY, pSrcZ); //重新计算每个邻域点坐标
// 			z1=_cal_dummy_observation_IDW(pLidPatch[iPatch].x, pLidPatch[iPatch].y, pSrcX, pSrcY, pSrcZ, pIDW_Weight, pLidPoints->size());
// 			
// 
// 			pCalibPara[i]=pX[i]-delta_angel_elem;
// 			_cal_batch_Laser_position(pLidPoints, pCalibPara, pSrcX, pSrcY, pSrcZ);
// 			z2=_cal_dummy_observation_IDW(pLidPatch[iPatch].x, pLidPatch[iPatch].y, pSrcX, pSrcY, pSrcZ, pIDW_Weight, pLidPoints->size());
// 
// 			dParam=(z1-z2)/(2*delta_angel_elem);  //差分算偏导数
// 
// 			*(pA+iPatch*pnp+i)=dParam;
// 		}
// 	
// 		iPatch++;
// 	}
// }

//同时计算误差和方差
//e=Z-Z0  (L=AX)
//POINT3D *p3DptBuf	 连接点坐标
//double *pObs  z坐标观测值
inline double _cal_object_space_z_err2(POINT3D *p3DptBuf, double *pObs, double *e, int num)
{
	double p_eL2=0;
	for(int i=0; i<num; i++)
	{
		e[i]=pObs[i]-p3DptBuf[i].Z;
		p_eL2+=e[i]*e[i];
	}

	return p_eL2;
}

//观测值
struct VPObs
{
	int nVP;					//虚拟点总数
	int nMisalign;			//安置参数个数
	LidMC_VP *pLidVP;	//虚拟点
	TiePoint_Adj_mode  TiePt_mode;
	LidPt_SurvInfo	*pATNPoints;	//虚拟点对应的激光点
	POINT3D *ptSrc;			
	POINT3D *ptDst;

//	double *SrcParam;		//上一次解算的未知数
	
	int nTies;			//连接点数
	int nObs;			//观测方程数

//	POINT3D *ptObs;		//虚拟点坐标观测值
	int   nTieGCP;			//连接点中的控制点数
	long *pLut;			//连接点对应的查找表

	double *L;			//重新计算的新观测值 （均值）
	double *hx;			//用于差分求解未知数
	double *hxx;
	double *pWeight;	//权阵	2013.11.22

	double tx, ty, tz;	//偏心分量测量值	
	double r, p, h;		//偏心角初值
	double sa0;			//扫描角归零误差
	double sa_ceof;

	unsigned long	param_type;
	orsLidSysParam   *oriParam;

	orsILidarGeoModel *pLidGeo;

//	double  mse_th;	//均方差阈值
};

// static bool Use_Hor_Ver=false;
// static bool Use_Hor=true;
// static bool Use_Ver=false;

//只计算观测值
void cal_Obs_VCP(double *p, double *y, int m, int n, void *data)
{
	register int i, j, k;
	struct VPObs *dptr;
	LidMC_VP *pLidVP;
	double      *pMisalign/*, *pSrcMisalign*/;
	int nMisalign;
	int nVP;
//	double a, b, c, d, s;
//	double xp, yp, zp;
//	double *pPlane;
	long *pLut;
	CRigidTransform_3D transform;
	LidPt_SurvInfo *pATNPoints;
	int nLidPt;
	POINT3D *ptDst, *ptSrc, *pTmp;
	double xSrc[3], ySrc[3], zSrc[3], xDst[3], yDst[3], zDst[3];
	double vx, vy, vz;
	int nTies, nTieGCP, nObs;
	double *L;
	double dXs, dYs, dZs;
	double *pWeight;
	orsILidarGeoModel *pLidGeo;


	dptr=(struct VPObs *)data;
	nMisalign=dptr->nMisalign;
	pLidVP=dptr->pLidVP;
	pMisalign=p;
//	pSrcMisalign=dptr->SrcParam;
	nVP=dptr->nVP;
	pATNPoints=dptr->pATNPoints;	
	pLut=dptr->pLut;
	ptDst=dptr->ptDst;
	ptSrc=dptr->ptSrc;
	nTies=dptr->nTies;
	nTieGCP=dptr->nTieGCP;
	nObs=dptr->nObs;
	L=dptr->L;
	pWeight = dptr->pWeight;
	pLidGeo = dptr->pLidGeo;

	nLidPt=nVP*3;		//一个虚拟点对应三个顶点
	//计算激光点坐标
	//	_calLaserPosition(pATNPoints, nLidPt, pSrcMisalign[0], pSrcMisalign[1], pSrcMisalign[2], pSrcMisalign[3], pSrcMisalign[4], pSrcMisalign[5]);

/*
	double tx, ty, tz;
	double roll, pitch, heading;
	double sa0, sa_ceof;
	if(dptr->param_type==Calib_rph)
	{
		tx=dptr->tx;	ty=dptr->ty;	tz=dptr->tz;
		roll=pMisalign[0];
		pitch=pMisalign[1];
		heading=pMisalign[2];
		dXs=dYs=dZs=0;
		sa0=0;
		sa_ceof=0;
	}
	else if(dptr->param_type==Calib_rphxyz)
	{
		tx=pMisalign[0];	ty=pMisalign[1];	tz=pMisalign[2];
		roll=pMisalign[3];
		pitch=pMisalign[4];
		heading=pMisalign[5];
		dXs=dYs=dZs=0;
		sa0=0;
		sa_ceof=0;
	}
	else if(dptr->param_type==Calib_rph_appXYZ)
	{//有线性系统改正
		tx=dptr->tx;	ty=dptr->ty;	tz=dptr->tz;
		roll=pMisalign[0];
		pitch=pMisalign[1];
		heading=pMisalign[2];
		dXs=pMisalign[6];
		dYs=pMisalign[7];
		dZs=pMisalign[8];
		sa0=0;
		sa_ceof=0;
	}
	else if(dptr->param_type==Calib_rphxyz_sa0)
	{
		tx=pMisalign[0];	ty=pMisalign[1];	tz=pMisalign[2];
		roll=pMisalign[3];
		pitch=pMisalign[4];
		heading=pMisalign[5];
		dXs=dYs=dZs=0;
		sa0=pMisalign[6];
		sa_ceof=0;
	}
	else if(dptr->param_type==Calib_rphxyz_sa0_sac)
	{
		tx=pMisalign[0];	ty=pMisalign[1];	tz=pMisalign[2];
		roll=pMisalign[3];
		pitch=pMisalign[4];
		heading=pMisalign[5];
		dXs=dYs=dZs=0;
		sa0=pMisalign[6];
		sa_ceof=pMisalign[7];
	}*/

//	_calLaserPosition(pATNPoints, nLidPt, tx, ty, tz, roll, pitch, heading, sa0, sa_ceof);
//	_calLaserPosition(pATNPoints, nLidPt, dptr->oriParam, p, dptr->param_type);
	orsLidSysParam curParam = *(dptr->oriParam);
	switch (dptr->param_type)
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
	pLidGeo->cal_Laserfootprint(pATNPoints, nLidPt);

	for(i=0; i<nLidPt; i++)
	{//用当前参数计算新的激光点坐标
		ptDst[i].X=pATNPoints[i].x/*+dXs*/;
		ptDst[i].Y=pATNPoints[i].y/*+dYs*/;
		ptDst[i].Z=pATNPoints[i].z/*+dZs*/;
	}

	//计算虚拟点坐标
	for(i=0; i<nVP; i++)
	{
		k=i*3;
		for(j=0; j<3; j++)
		{
			xSrc[j]=ptSrc[k+j].X;
			ySrc[j]=ptSrc[k+j].Y;
			zSrc[j]=ptSrc[k+j].Z;

			xDst[j]=ptDst[k+j].X;
			yDst[j]=ptDst[k+j].Y;
			zDst[j]=ptDst[k+j].Z;
		}
		transform.Initialize(xSrc, ySrc, zSrc, 3, xDst, yDst, zDst);
		transform.DoTransform(&(pLidVP[i].vx), &(pLidVP[i].vy), &(pLidVP[i].vz), 1, &vx, &vy, &vz);

		double dz=TriangleInterpolate(ptDst+k, vx, vy);
		if(pLidVP[i].VP_type & TP_type_hor_ver || pLidVP[i].VP_type & TP_type_vertical)
		{
//			ASSERT(fabs(dz-vz)<0.01 );
		}

		pLidVP[i].vx = vx;
		pLidVP[i].vy = vy;
		pLidVP[i].vz = vz;
	}

	//对原始的虚拟点坐标加权
	for(i=0, j=0; i<nVP; i++)
	{
		if(dptr->TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pLidVP[i].vx *= pWeight[j];	y[j]=pLidVP[i].vx; j++;
				pLidVP[i].vy *= pWeight[j];	y[j]=pLidVP[i].vy; j++;
				pLidVP[i].vz *= pWeight[j];	y[j]=pLidVP[i].vz; j++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				pLidVP[i].vx *= pWeight[j];	y[j]=pLidVP[i].vx; j++;
				pLidVP[i].vy *= pWeight[j];	y[j]=pLidVP[i].vy; j++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{
				pLidVP[i].vz *= pWeight[j];	y[j]=pLidVP[i].vz; j++;
			}
		}
		else if(dptr->TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pLidVP[i].vx *= pWeight[j];	y[j]=pLidVP[i].vx; j++;
				pLidVP[i].vy *= pWeight[j];	y[j]=pLidVP[i].vy; j++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				pLidVP[i].vx *= pWeight[j];	y[j]=pLidVP[i].vx; j++;
				pLidVP[i].vy *= pWeight[j];	y[j]=pLidVP[i].vy; j++;
			}
		}
		else if(dptr->TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pLidVP[i].vz *= pWeight[j];	y[j]=pLidVP[i].vz; j++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{
				pLidVP[i].vz *= pWeight[j];	y[j]=pLidVP[i].vz; j++;
			}
		}
	}

	//交换激光点坐标
	pTmp=dptr->ptSrc;
	dptr->ptSrc=dptr->ptDst;
	dptr->ptDst=pTmp;
	
/*
	for(i=0; i<nTies; i++)
	{
		for(j=pLut[i]; j<pLut[i+1]; j++)
		{//虚拟点坐标赋值
			y[j*3]=pLidVP[j].vx;
			y[j*3+1]=pLidVP[j].vy;
			y[j*3+2]=pLidVP[j].vz;
		}
		
	}*/

/*	for(i=0, j=0; i<nVP; i++)
	{
		if(pLidVP[i].VP_type & TP_type_hor_ver && Use_Hor_Ver)
		{
			y[j++]=pLidVP[i].vx;
			y[j++]=pLidVP[i].vy;
			y[j++]=pLidVP[i].vz;
		}
		else if(pLidVP[i].VP_type & TP_type_horizon && Use_Hor)
		{
			y[j++]=pLidVP[i].vx;
			y[j++]=pLidVP[i].vy;
		}
		else if(pLidVP[i].VP_type & TP_type_vertical && Use_Ver)
		{
			y[j++]=pLidVP[i].vz;
		}
	}*/
}

//只计算观测值
void cal_Obs_NN(double *p, double *y, int m, int n, void *data)
{
	register int i, j, k;
	struct VPObs *dptr;
	LidMC_VP *pLidVP;
	double      *pMisalign/*, *pSrcMisalign*/;
	int nMisalign;
	int nVP;
	long *pLut;
	CRigidTransform_3D transform;
	LidPt_SurvInfo *pATNPoints;
	int nLidPt;
	POINT3D *ptDst, *ptSrc, *pTmp;
	double xSrc[3], ySrc[3], zSrc[3], xDst[3], yDst[3], zDst[3];
	double vx, vy, vz;
	int nTies, nTieGCP, nObs;
	double *L;
	double dXs, dYs, dZs;
	double *pWeight;
	orsILidarGeoModel *pLidGeo;


	dptr=(struct VPObs *)data;
	nMisalign=dptr->nMisalign;
	pLidVP=dptr->pLidVP;
	pMisalign=p;
//	pSrcMisalign=dptr->SrcParam;
	nVP=dptr->nVP;
	pATNPoints=dptr->pATNPoints;	
	pLut=dptr->pLut;
	ptDst=dptr->ptDst;
	ptSrc=dptr->ptSrc;
	nTies=dptr->nTies;
	nTieGCP=dptr->nTieGCP;
	nObs=dptr->nObs;
	L=dptr->L;
	pWeight = dptr->pWeight;

	nLidPt=nVP;		//一个虚拟点对应一个最近顶点

	pLidGeo = dptr->pLidGeo;

//	_calLaserPosition(pATNPoints, nLidPt, dptr->oriParam, p, dptr->param_type);
	orsLidSysParam curParam = *(dptr->oriParam);
	switch (dptr->param_type)
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
	pLidGeo->cal_Laserfootprint(pATNPoints, nLidPt);

	for(i=0; i<nLidPt; i++)
	{//用当前参数计算新的激光点坐标
		pLidVP[i].vx = ptDst[i].X = pATNPoints[i].x/*+dXs*/;
		pLidVP[i].vy = ptDst[i].Y = pATNPoints[i].y/*+dYs*/;
		pLidVP[i].vz = ptDst[i].Z = pATNPoints[i].z/*+dZs*/;
	}

	//对原始的虚拟点坐标加权
	for(i=0, j=0; i<nVP; i++)
	{
		if(dptr->TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pLidVP[i].vx *= pWeight[j];	y[j]=pLidVP[i].vx; j++;
				pLidVP[i].vy *= pWeight[j];	y[j]=pLidVP[i].vy; j++;
				pLidVP[i].vz *= pWeight[j];	y[j]=pLidVP[i].vz; j++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				pLidVP[i].vx *= pWeight[j];	y[j]=pLidVP[i].vx; j++;
				pLidVP[i].vy *= pWeight[j];	y[j]=pLidVP[i].vy; j++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{
				pLidVP[i].vz *= pWeight[j];	y[j]=pLidVP[i].vz; j++;
			}
		}
		else if(dptr->TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pLidVP[i].vx *= pWeight[j];	y[j]=pLidVP[i].vx; j++;
				pLidVP[i].vy *= pWeight[j];	y[j]=pLidVP[i].vy; j++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				pLidVP[i].vx *= pWeight[j];	y[j]=pLidVP[i].vx; j++;
				pLidVP[i].vy *= pWeight[j];	y[j]=pLidVP[i].vy; j++;
			}
		}
		else if(dptr->TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pLidVP[i].vz *= pWeight[j];	y[j]=pLidVP[i].vz; j++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{
				pLidVP[i].vz *= pWeight[j];	y[j]=pLidVP[i].vz; j++;
			}
		}
	}

	//交换激光点坐标
	pTmp=dptr->ptSrc;
	dptr->ptSrc=dptr->ptDst;
	dptr->ptDst=pTmp;
	
/*
	for(i=0; i<nTies; i++)
	{
		for(j=pLut[i]; j<pLut[i+1]; j++)
		{//虚拟点坐标赋值
			y[j*3]=pLidVP[j].vx;
			y[j*3+1]=pLidVP[j].vy;
			y[j*3+2]=pLidVP[j].vz;
		}
		
	}*/

/*	for(i=0, j=0; i<nVP; i++)
	{
		if(pLidVP[i].VP_type & TP_type_hor_ver && Use_Hor_Ver)
		{
			y[j++]=pLidVP[i].vx;
			y[j++]=pLidVP[i].vy;
			y[j++]=pLidVP[i].vz;
		}
		else if(pLidVP[i].VP_type & TP_type_horizon && Use_Hor)
		{
			y[j++]=pLidVP[i].vx;
			y[j++]=pLidVP[i].vy;
		}
		else if(pLidVP[i].VP_type & TP_type_vertical && Use_Ver)
		{
			y[j++]=pLidVP[i].vz;
		}
	}*/
}

static int iter=0;
//计算连接点期望值
//更新L
void cal_EX(void *data)
{
	register int i, j, k;
	struct VPObs *dptr;
	LidMC_VP *pLidVP;
// 	double      *pMisalign, *pSrcMisalign;
// 	int nMisalign;
// 	int nVP;
//	double a, b, c, d, s;
//	double xp, yp, zp;
//	double *pPlane;
	long *pLut;
//	CRigidTransform_3D transform;
//	ATNPoint *pATNPoints;
//	int nLidPt;
//	POINT3D *ptDst, *ptSrc, *pTmp;
//	double xSrc[3], ySrc[3], zSrc[3], xDst[3], yDst[3], zDst[3];
//	double vx, vy, vz;
	int nTies, nTieGCP;
	double *L;


	dptr=(struct VPObs *)data;
// 	nMisalign=dptr->nMisalign;
// 	pLidVP=dptr->pLidVP;
// 	pMisalign=p;
// 	pSrcMisalign=dptr->SrcParam;
// 	nVP=dptr->nVP;
// 	pATNPoints=dptr->pATNPoints;	
	pLut=dptr->pLut;
// 	ptDst=dptr->ptDst;
// 	ptSrc=dptr->ptSrc;
	nTies=dptr->nTies;
	nTieGCP=dptr->nTieGCP;
	L=dptr->L;
	pLidVP=dptr->pLidVP;

	//计算观测值
	double ex, ey, ez;
	int nLinks;
	//0~nTieGCP为控制点
//	TRACE("Tie VP Coord\n");

	iter++;
	for(i=nTieGCP; i<nTies; i++)
	{
		ex=0; ey=0;ez=0;
		for(j=pLut[i]; j<pLut[i+1]; j++)
		{
			ex+=pLidVP[j].vx;
			ey+=pLidVP[j].vy;
			ez+=pLidVP[j].vz;
		}
		nLinks=pLut[i+1]-pLut[i];
		ex/=nLinks;
		ey/=nLinks;
		ez/=nLinks;

//		if(iter%25==0)
//			TRACE("%d %.3f %.3f %.3f\n", i, ex, ey, ez);
		for(j=pLut[i]; j<pLut[i+1]; j++)
		{
			pLidVP[j].eX=ex;
			pLidVP[j].eY=ey;
			pLidVP[j].eZ=ez;
		}
	}

//	pW = dptr->pWeight;
	for(i=nTieGCP; i<nTies; i++)
	{
		k=pLut[i];	//连接点的第一个
		if(dptr->TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[k].VP_type == TP_type_hor_ver)
			{
				for(j=pLut[i]; j<pLut[i+1]; j++)
				{
					*L=pLidVP[k].eX;		L++;	
					*L=pLidVP[k].eY;		L++;	
					*L=pLidVP[k].eZ;		L++;	
				}
			}
			else if(pLidVP[k].VP_type == TP_type_horizon)
			{
				for(j=pLut[i]; j<pLut[i+1]; j++)
				{
					*L=pLidVP[k].eX;		L++;	
					*L=pLidVP[k].eY;		L++;	
				}
			}
			else if(pLidVP[k].VP_type == TP_type_vertical)
			{
				for(j=pLut[i]; j<pLut[i+1]; j++)
				{
					*L=pLidVP[k].eZ;		L++;	
				}
			}
		}
		else if(dptr->TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[k].VP_type == TP_type_hor_ver)
			{
				for(j=pLut[i]; j<pLut[i+1]; j++)
				{
					*L=pLidVP[k].eX;		L++;	
					*L=pLidVP[k].eY;		L++;	
				}
			}
			else if(pLidVP[k].VP_type == TP_type_horizon)
			{
				for(j=pLut[i]; j<pLut[i+1]; j++)
				{
					*L=pLidVP[k].eX;		L++;	
					*L=pLidVP[k].eY;		L++;	
				}
			}
		}
		else if(dptr->TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[k].VP_type == TP_type_hor_ver)
			{
				for(j=pLut[i]; j<pLut[i+1]; j++)
				{
					*L=pLidVP[k].eZ;		L++;	
				}
			}
			else if(pLidVP[k].VP_type == TP_type_vertical)
			{
				for(j=pLut[i]; j<pLut[i+1]; j++)
				{
					*L=pLidVP[k].eZ;		L++;	
				}
			}
		}
	}
}

//计算虚拟点坐标   //p:未知数， y计算的虚拟点坐标，m未知数个数，n方程个数
void cal_VirtualPoint_VCP(double *p, double *y, int m, int n, void *data)
{
	cal_Obs_VCP(p, y, m, n, data);	//计算观测值
	cal_EX(data);					//计算期望值
}

void cal_VirtualPoint_NN(double *p, double *y, int m, int n, void *data)
{
	cal_Obs_NN(p, y, m, n, data);	//计算观测值
	cal_EX(data);					//计算期望值
}

void jac_VirtualPoint_VCP(double *p, double *jac, int m, int n, void *data)
{
	register int i, j;
	double tmp;
	register double d;
	double delta=1E-06;
	double *hx, *hxx;
	struct VPObs *dptr;
	double nVP;
//	double *pLut;

	dptr=(struct VPObs *)data;
	nVP=dptr->nVP;
//	pLut=dptr->pLut;
	hx=dptr->hx;
	hxx=dptr->hxx;
	
	cal_Obs_VCP(p, hx, m, n, data);

	for(j=0; j<m; ++j)
	{
		/* determine d=max(1E-04*|p[j]|, delta), see HZ */
		d=(1E-04)*p[j]; // force evaluation
		d=fabs(d);
		if(d<delta)
			d=delta;

		tmp=p[j];
		p[j]+=d;

		cal_Obs_VCP(p, hxx, m, n, data);

		p[j]=tmp; /* restore */

		d=(1.0)/d; /* invert so that divisions can be carried out faster as multiplications */
		for(i=0; i<n; ++i)
		{
			jac[i*m+j]=(hxx[i]-hx[i])*d;
		}
	}
}

void jac_VirtualPoint_NN(double *p, double *jac, int m, int n, void *data)
{
	register int i, j;
	double tmp;
	register double d;
	double delta=1E-06;
	double *hx, *hxx;
	struct VPObs *dptr;
	double nVP;
	//	double *pLut;

	dptr=(struct VPObs *)data;
	nVP=dptr->nVP;
	//	pLut=dptr->pLut;
	hx=dptr->hx;
	hxx=dptr->hxx;

	cal_Obs_NN(p, hx, m, n, data);

	for(j=0; j<m; ++j)
	{
		/* determine d=max(1E-04*|p[j]|, delta), see HZ */
		d=(1E-04)*p[j]; // force evaluation
		d=fabs(d);
		if(d<delta)
			d=delta;

		tmp=p[j];
		p[j]+=d;

		cal_Obs_NN(p, hxx, m, n, data);

		p[j]=tmp; /* restore */

		d=(1.0)/d; /* invert so that divisions can be carried out faster as multiplications */
		for(i=0; i<n; ++i)
		{
			jac[i*m+j]=(hxx[i]-hx[i])*d;
		}
	}
}

CLidCalib_VCP::CLidCalib_VCP()
{
	m_covar = NULL;
	m_rms = 0;
	m_corcoef = NULL;
}

CLidCalib_VCP::~CLidCalib_VCP()
{
	if(m_covar)
		delete m_covar;
	m_covar = NULL;

	if(m_corcoef)
		delete m_corcoef;
	m_corcoef = NULL;
}

// int CLidCalib_VCP::LidMC_Misalign_VP_Triangle_xyzrph_semiXYZ(
// 				const int nVP,	/* number of 3D points */
// 				const int ngcp,		/* number of points (starting from the 1st) whose parameters should not be modified.	
// 											* All B_ij (see below) with i<ncon are assumed to be zero
// 											控制点放在观测值的起始部分*/
// 				LidMC_VP *pLidVP,
// 				double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
// 											* aj are the image j parameters, bi are the i-th point parameters,
// 											* size m*cnp + n*pnp
// 											未知数，像片+点   [t X]^T, 
// 											t=[Xs, Ys, Zs, f, w, k]^T
// 											X=[X, Y, Z]^T
// 											（传入未知数初始值）*/
// 				const int pnp			/* number of parameters 未知数个数*/
// )
// {
// //	double iReturn=-1;
// 	double *L=NULL;
// 	double *X=NULL;
// 	double p_eL2;
// 	double init_p_eL2;
// 	int i, j, k;
// 	double *pA=NULL, *pAT=NULL;
// 	double  *pdX=NULL;	//未知数改正数
// 	POINT3D *pTiePt=NULL;
// 	CAffineTransform	affineTrans;
// //	LidMC_Patch *prePatch=NULL, *postPatch=NULL, *pTPatch=NULL;
// 	long *pLut=NULL;
// 	ATNPoint *pATNPoints=NULL;
// //	double *SrcParam;
// 	POINT3D *ptSrc=NULL, *ptDst=NULL;
// 	double *hx=NULL, *hxx=NULL;
// 	
// 	//统计连接面数量, tieID必须连续排列
// 	int tieID=-1;
// 	int nTies=0;
// 	for(i=0; i<nVP; i++)
// 	{//统计连接点个数
// 		if(tieID!=pLidVP[i].tieID)
// 		{
// 			nTies++;
// 			tieID=pLidVP[i].tieID;
// 		}
// 	}
// 
// 	pATNPoints=new ATNPoint[nVP*3];
// 	for(i=0; i<nVP; i++)
// 	{
// 		for(j=0; j<3; j++)
// 		{
// 			pATNPoints[i*3+j]=(*(pLidVP[i].pLidPoints))[j];
// 		}
// 	}
// 
// 	POINT3D *ptObs=NULL, *ptLinkObs;
// 	ptObs=new POINT3D[nTies];
// 	pLut=new long[nTies+1];
// 
// 	int nTieGcp=0;
// 	tieID=-1;
// 	for(i=0; i<ngcp; i++)
// 	{
// 		if(tieID!=pLidVP[i].tieID)
// 		{
// 			tieID=pLidVP[i].tieID;
// 			ptObs[tieID].X=pLidVP[i].eX;
// 			ptObs[tieID].Y=pLidVP[i].eY;
// 			ptObs[tieID].Z=pLidVP[i].eZ;
// 
// 			nTieGcp++;
// 			pLut[tieID]=i;
// 		}
// 	}
// 	
// 	int nLinks=0;
// // 	ptLinkObs=ptObs+nTieGcp;
// // 	memset(ptLinkObs, 0, sizeof(POINT3D)*(nTies-nTieGcp));
// 	for(i=ngcp; i<nVP; i++)
// 	{
// 		if(tieID!=pLidVP[i].tieID)
// 		{
// // 			ptObs[tieID].X/=nLinks;
// // 			ptObs[tieID].Y/=nLinks;
// // 			ptObs[tieID].Z/=nLinks;
// 
// 			tieID=pLidVP[i].tieID;
// 			pLut[tieID]=i;
// 
// 			ptObs[tieID].X=pLidVP[i].vx;
// 			ptObs[tieID].Y=pLidVP[i].vy;
// 			ptObs[tieID].Z=pLidVP[i].vz;
// 
// //			nLinks=1;
// 		}
// 		else
// 		{
// 			ptObs[tieID].X+=pLidVP[i].vx;
// 			ptObs[tieID].Y+=pLidVP[i].vy;
// 			ptObs[tieID].Z+=pLidVP[i].vz;
// //			nLinks++;
// 		}
// 	}
// 	pLut[nTies]=nVP;
// 
// 	for(i=nTieGcp; i<nTies; i++)
// 	{
// 		nLinks=pLut[i+1]-pLut[i];	//每个连接点对应的片数
// 		//连接点取平均值
// 		ptObs[i].X/=nLinks;
// 		ptObs[i].Y/=nLinks;
// 		ptObs[i].Z/=nLinks;
// 
// 		for(j=pLut[i]; j<pLut[i+1]; j++)
// 		{
// 			pLidVP[j].eX=ptObs[i].X;
// 			pLidVP[j].eY=ptObs[i].Y;
// 			pLidVP[j].eZ=ptObs[i].Z;
// 		}
// 	}
// 
// //	double *L=NULL;
// 	L=new double[nVP*3];
// 	for(i=0; i<nVP; i++)
// 	{
// 		L[i*3]=pLidVP[i].eX;
// 		L[i*3+1]=pLidVP[i].eY;
// 		L[i*3+2]=pLidVP[i].eZ;
// 	}
// 	
// 	X=new double[9];
// 	for(i=0; i<pnp; i++)
// 	{
// 		X[i]=pX[i];
// 	}
// 	X[6]=0;		X[7]=0;		X[8]=0;		//dXs, dYs, dZs
// 
// // 	SrcParam=new double[9];
// // 	for(i=0; i<9; i++)
// // 	{
// // 		SrcParam[i]=X[i];
// // 	}
// 	hx=new double[nVP*3];
// 	hxx=new double[nVP*3];
// 
// 	ptSrc=new POINT3D[nVP*3];
// 	ptDst=new POINT3D[nVP*3];
// 
// 	_calLaserPosition(pATNPoints, nVP*3, pX[0], pX[1], pX[2], pX[3], pX[4], pX[5]);
// 	for(i=0; i<nVP*3; i++)
// 	{//用当前参数计算新的激光点坐标
// 		ptSrc[i].X=pATNPoints[i].x+X[6];
// 		ptSrc[i].Y=pATNPoints[i].y+X[7];
// 		ptSrc[i].Z=pATNPoints[i].z+X[8];
// 	}
// 
// 	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
// 	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;
// 
// 	struct VPObs data;
// 	data.nMisalign=pnp;
// 	data.nTies=nTies;
// 	data.nTieGCP=nTieGcp;
// 	data.nVP=nVP;
// 	data.pLidVP=pLidVP;
// 	data.pLut=pLut;
// 	data.pATNPoints=pATNPoints;
// 	data.L=L;
// 	//	data.SrcParam=SrcParam;
// 	data.ptSrc=ptSrc;
// 	data.ptDst=ptDst;
// 	data.hx=hx;
// 	data.hxx=hxx;
// 
// #ifdef _DEBUG
// 	FILE *fp=NULL;
// 	CString strName;
// 
// 	if(ngcp>0)
// 		strName="g:/D_experiment/adjustment/VP/VTP_Tri_xyzrph_semiXYZ";
// 	else
// 		strName="g:/D_experiment/adjustment/VP/VP_Tri_xyzrph_semiXYZ";
// 
// 	fp=fopen(/*strName.GetBuffer(128)*/m_pResultName, "wt");
// 
// 	double *ex=new double[nVP];
// 	double *ey=new double[nVP];
// 	double *ez=new double[nVP];
// 	double rms_x, rms_y, rms_z;
// 
// 	cal_VPObs(X, hx, 9, nVP*3, &data);
// 	rms_x=rms_y=rms_z=0;
// 	fprintf(fp, "before adjustment\n");
// 	for(i=0; i<nVP;	i++)
// 	{
// 		j=i*3;
// 		ex[i]=hx[j]-L[j];
// 		ey[i]=hx[j+1]-L[j+1];
// 		ez[i]=hx[j+2]-L[j+2];
// 
// 		fprintf(fp, "%d %.3f %.3f %.3f\n", i, ex[i], ey[i], ez[i]);
// 
// 		rms_x+=ex[i]*ex[i];
// 		rms_y+=ey[i]*ey[i];
// 		rms_z+=ez[i]*ez[i];
// 	}
// 	rms_x=sqrt(rms_x/nVP);
// 	rms_y=sqrt(rms_y/nVP);
// 	rms_z=sqrt(rms_z/nVP);
// 	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);
// #endif
// 
// 	int ret=dlevmar_der(cal_VirtualPoint, jac_VirtualPoint, X, L, 9, nVP*3, 1000, opts, info, NULL, NULL, &data);
// //	int ret=dlevmar_der(distance_PointToPlane, jac_dis_PointToPlane, X, L, pnp, nVP, 1000, opts, info, NULL, NULL, &data);
// 
// #ifdef _DEBUG
// 	cal_VPObs(X, hx, 9, nVP*3, &data);
// 	rms_x=rms_y=rms_z=0;
// 	fprintf(fp, "after adjustment\n");
// 	for(i=0; i<nVP;	i++)
// 	{
// 		j=i*3;
// 		ex[i]=hx[j]-L[j];
// 		ey[i]=hx[j+1]-L[j+1];
// 		ez[i]=hx[j+2]-L[j+2];
// 
// 		fprintf(fp, "%d %.3f %.3f %.3f\n", i, ex[i], ey[i], ez[i]);
// 
// 		rms_x+=ex[i]*ex[i];
// 		rms_y+=ey[i]*ey[i];
// 		rms_z+=ez[i]*ez[i];
// 	}
// 	rms_x=sqrt(rms_x/nVP);
// 	rms_y=sqrt(rms_y/nVP);
// 	rms_z=sqrt(rms_z/nVP);
// 	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);
// 
// 	fprintf(fp, "misalign parameter: tx, ty, tz, r, p, h, dXs, dYs, dZs\n");
// 	for(i=0; i<9; i++)
// 	{
// 		fprintf(fp, "%.9lf\n", X[i]);
// 	}
// 
// 	if(fp)	fclose(fp);	fp=NULL;
// 	if(ex)			delete ex;			ex=NULL;
// 	if(ey)			delete ey;			ey=NULL;
// 	if(ez)			delete ez;			ez=NULL;
// #endif
// 
// 	if(ptObs)		delete ptObs;		ptObs=NULL;
// 	if(pLut)      delete pLut;		pLut=NULL;
// 	if(L)          delete L;				L=NULL;
// 	if(X)			delete X;			X=NULL;
// 	if(pATNPoints) delete pATNPoints;	pATNPoints=NULL;
// //	if(SrcParam) delete SrcParam;	SrcParam=NULL;
// 	if(ptSrc)     delete ptSrc;		ptSrc=NULL;
// 	if(ptDst)		delete ptDst;		ptDst=NULL;
// 	if(hx)			delete hx;			hx=NULL;
// 	if(hxx)		delete hxx;			hxx=NULL;
// 	return ret;
// }


int CLidCalib_VCP::LidMC_Misalign_VCP(
			    const char *pszResultFile,
				const int nVP,	/* number of 3D points */
				const int ngcp,		/* number of points (starting from the 1st) whose parameters should not be modified.	
											* All B_ij (see below) with i<ncon are assumed to be zero
											控制点放在观测值的起始部分*/
				LidMC_VP *pLidVP,
				TiePoint_Adj_mode TiePt_mode,
				double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
											* aj are the image j parameters, bi are the i-th point parameters,
											* size m*cnp + n*pnp
											未知数，像片+点   [t X]^T, 
											t=[Xs, Ys, Zs, f, w, k]^T
											X=[X, Y, Z]^T
											（传入未知数初始值）*/
//				const int pnp,			/* number of parameters 未知数个数*/
				orsLidSysParam oriParam,
				CalibParam_Type	param_type				/* 检校参数类型 */
)
{
//	double iReturn=-1;
	double *L=NULL;
	double *X=NULL;
	double p_eL2;
	double init_p_eL2;
	int i, j, k;
	double *pA=NULL, *pAT=NULL;
	double  *pdX=NULL;	//未知数改正数
	POINT3D *pTiePt=NULL;
	CAffineTransform	affineTrans;
//	LidMC_Patch *prePatch=NULL, *postPatch=NULL, *pTPatch=NULL;
	long *pLut=NULL;
	LidPt_SurvInfo *pATNPoints=NULL;
//	double *SrcParam;
	POINT3D *ptSrc=NULL, *ptDst=NULL;
	double *hx=NULL, *hxx=NULL;
	double *pWeight=NULL;	//观测值权阵
	
	//统计连接面数量, tieID必须连续排列
	int tieID=-1;
	int nTies=0;
	int nObs=0;
	int /*nHorVer,*/ nHor, nVer;
	int nXDirection, nYDirection, nZDirection;
	double wXD, wYD, wZD;


	/*nHorVer = */nHor = nVer = 0;
	for(i=0; i<nVP; i++)
	{//统计连接点个数
		if(tieID!=pLidVP[i].tieID)
		{
			nTies++;
			tieID=pLidVP[i].tieID;
		}

		if(TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver /*&& Use_Hor_Ver*/)
			{
				nHor++;
				nVer++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon /*&& Use_Hor*/)
				nHor++;
			else if(pLidVP[i].VP_type == TP_type_vertical /*&& Use_Ver*/)
				nVer++;
		}
		else if(TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver /*&& Use_Hor_Ver*/)
			{
				nHor++;
				
			}
			else if(pLidVP[i].VP_type == TP_type_horizon /*&& Use_Hor*/)
				nHor++;
		}
		else if(TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver /*&& Use_Hor_Ver*/)
			{
				
				nVer++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical /*&& Use_Ver*/)
				nVer++;
		}
	}

	nObs = /*nHorVer*3 + */nHor*2 + nVer;
	
	nXDirection = /*nHorVer + */nHor;
	nYDirection = /*nHorVer + */nHor;
	nZDirection = /*nHorVer + */nVer;

	//定权方法，三个轴向等权
	//直接修改观测值方程，权阵乘了两次，所以权值去平方根
	if(nXDirection==0)
		wXD = 0;
	else
		wXD = sqrt(1.0/nXDirection);

	if(nYDirection==0)
		wYD = 0;
	else
		wYD = sqrt(1.0/nYDirection);

	if(nZDirection==0)
		wZD = 0;
	else
		wZD = sqrt(1.0/nZDirection);


	pATNPoints=new LidPt_SurvInfo[nVP*3];
	for(i=0; i<nVP; i++)
	{
		for(j=0; j<3; j++)
		{//提取真实激光点
			pATNPoints[i*3+j]=(*(pLidVP[i].pLidPoints))[j];
		}
	}

	/////////////////////////////////////////////
	//////////////////提取斜距//////////////////
	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_RangeVec(pATNPoints, nVP*3);


	POINT3D *ptObs=NULL;
	ptObs=new POINT3D[nTies];	//虚拟点期望值 (取平均)
	pLut=new long[nTies+1];

	int nTieGcp=0;
	tieID=-1;
	for(i=0; i<ngcp; i++)
	{
		if(tieID!=pLidVP[i].tieID)
		{
			tieID=pLidVP[i].tieID;
			ptObs[tieID].X=pLidVP[i].eX;
			ptObs[tieID].Y=pLidVP[i].eY;
			ptObs[tieID].Z=pLidVP[i].eZ;

			nTieGcp++;
			pLut[tieID]=i;
		}
	}
	
	int nLinks=0;
	for(i=ngcp; i<nVP; i++)
	{
		if(tieID!=pLidVP[i].tieID)
		{
			tieID=pLidVP[i].tieID;
			pLut[tieID]=i;

			ptObs[tieID].X=pLidVP[i].vx;
			ptObs[tieID].Y=pLidVP[i].vy;
			ptObs[tieID].Z=pLidVP[i].vz;
		}
		else
		{
			ptObs[tieID].X+=pLidVP[i].vx;
			ptObs[tieID].Y+=pLidVP[i].vy;
			ptObs[tieID].Z+=pLidVP[i].vz;
		}
	}
	pLut[nTies]=nVP;

	for(i=nTieGcp; i<nTies; i++)
	{
		nLinks=pLut[i+1]-pLut[i];	//每个连接点对应的片数
		//连接点取平均值
		ptObs[i].X/=nLinks;
		ptObs[i].Y/=nLinks;
		ptObs[i].Z/=nLinks;

		for(j=pLut[i]; j<pLut[i+1]; j++)
		{
			pLidVP[j].eX=ptObs[i].X;
			pLidVP[j].eY=ptObs[i].Y;
			pLidVP[j].eZ=ptObs[i].Z;
		}
	}

//	double *L=NULL;
	//与观测方程相关
	L=new double[nObs];	//方程数
	hx=new double[nObs];
	hxx=new double[nObs];
	pWeight = new double[nObs];
	for(i=0; i<nObs; i++)
	{
		pWeight[i] = 1.0;
	}

	//加权效果不好，暂时用等权
	wXD = wYD = wZD = 1;

	for(i=0, j=0; i<nVP; i++)
	{
		if(TiePt_mode == LidAdj_TP_UseHorVer)
		{//平面点和高程点
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wZD;	j++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{
				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wXD;	j++;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseHor)
		{//平面点
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
//				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wZD;	j++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseVer)
		{//高程点
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
// 				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
// 				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wZD;	j++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{
				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wXD;	j++;
			}
		}
	}

	int pnp=0;
	if(param_type & LidAdj_boreAngle)
		pnp += 3;
	if(param_type & LidAdj_leverArm)
		pnp += 3;
	if(param_type & LidAdj_GPS_drift)
		pnp += 6;
	if(param_type & LidAdj_INS_drift)
		pnp += 6;
	
	X=new double[pnp];
	memcpy(X, pX, sizeof(double)*pnp);
	
/*
	if(param_type==Calib_rphxyz)
	{
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i];
		}
	}
	else if(param_type==Calib_rph)
	{//前三个为偏心分量
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i+3];
		}
	}
	else if(param_type==Calib_rph_appXYZ)
	{
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i+3];
		}
	}
	else if(param_type==Calib_rphxyz_sa0)
	{
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i];
		}
	}
	else if(param_type==Calib_rphxyz_sa0_sac)
	{
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i];
		}
	}*/

	//与激光点相关，一个虚拟点对应三个激光点
	ptSrc=new POINT3D[nVP*3];	
	ptDst=new POINT3D[nVP*3];

	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;

	struct VPObs data;
	data.nMisalign=pnp;
	data.nTies=nTies;
	data.nObs=nObs;
	data.nTieGCP=nTieGcp;
	data.nVP=nVP;
	data.pLidVP=pLidVP;
	data.TiePt_mode = TiePt_mode;
	data.pLut=pLut;
	data.pATNPoints=pATNPoints;
	data.L=L;
	//	data.SrcParam=SrcParam;
	data.ptSrc=ptSrc;
	data.ptDst=ptDst;
	data.hx=hx;
	data.hxx=hxx;
	data.pWeight = pWeight;

	data.param_type=param_type;
// 	data.tx=pX[0];
// 	data.ty=pX[1];
// 	data.tz=pX[2];
// 	data.r=pX[3];
// 	data.p=pX[4];
// 	data.h=pX[5];
	data.oriParam = &oriParam;
	data.pLidGeo = lidGeo.get();

	orsLidSysParam curParam = oriParam;
	switch (param_type)
	{
	case Calib_rph:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		break;
	case Calib_rphxyz:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.lever_arm[0] = pX[3];
		curParam.lever_arm[1] = pX[4];
		curParam.lever_arm[2] = pX[5];
		break;
	case Calib_rph_appXYZ:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.semiXYZ[0] = pX[3];
		curParam.semiXYZ[1] = pX[4];
		curParam.semiXYZ[2] = pX[5];
		break;
	case Calib_rphxyz_sa0:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.lever_arm[0] = pX[3];
		curParam.lever_arm[1] = pX[4];
		curParam.lever_arm[2] = pX[5];
		curParam.sa0 = pX[6];
		break;
	case Calib_rphxyz_sa0_sac:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.lever_arm[0] = pX[3];
		curParam.lever_arm[1] = pX[4];
		curParam.lever_arm[2] = pX[5];
		curParam.sa0 = pX[6];
		curParam.sac = pX[7];
		break;
	case Calib_rph_sa0:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.sa0 = pX[3];
		break;
	case Calib_rph_drange:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.drange = pX[3];
		break;
	case Calib_rph_drange_sa0:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.drange = pX[3];
		curParam.sa0 = pX[4];
		break;
	default:
		;
	}
	lidGeo->setSysParam((void*)&curParam);
	lidGeo->cal_Laserfootprint(pATNPoints, nVP*3);
//	_calLaserPosition(pATNPoints, nVP*3, &oriParam, pX, param_type);

	for(i=0; i<nVP*3; i++)
	{//用当前参数计算新的激光点坐标
		ptSrc[i].X=pATNPoints[i].x;
		ptSrc[i].Y=pATNPoints[i].y;
		ptSrc[i].Z=pATNPoints[i].z;
	}

//#ifdef _DEBUG
	FILE *fp=NULL;
	fp=fopen(pszResultFile, "wt");
	if(fp==NULL)
	{
		assert(false);
		return -1;
	}

	fprintf(fp, "## calibration method: Virtual Corresponding Points\n");
	fprintf(fp, "TiePoint mode: %d\n", TiePt_mode);
	fprintf(fp, "## calib param type: %d\n", param_type);
	fprintf(fp, "## init calib param\n");
	for(i=0; i<pnp; i++)
	{
		fprintf(fp, "%.9lf\n", pX[i]);
	}

	fprintf(fp, "tiepoints: %d; total points=%d hor=%d ver=%d\n", nTies, nVP, /*nHorVer, */nHor, nVer);

	//误差值
	double *pdx=new double[nVP];
	double *pdy=new double[nVP];
	double *pdz=new double[nVP];
	double rms, rms_x, rms_y, rms_z;
	
	cal_Obs_VCP(X, hx, pnp, nObs, &data);
	rms_x=rms_y=rms_z=0;
	fprintf(fp, "before adjustment\n");
	/*nHorVer=0,*/ nHor=0, nVer=0;
	bool bjump;
	for(i=0, j=0; i<nVP;	i++)
	{
		bjump=true;
		if(TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=hx[j]-L[j];		j++;	

				nHor++;
				nVer++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;	//平面点，高程误差赋0
				nHor++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;
				nVer++;
				bjump=false;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;			

				nHor++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type & TP_type_horizon)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;	//平面点，高程误差赋0
				nHor++;
				bjump=false;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type & TP_type_hor_ver)
			{
				pdx[i]=0;		
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;	

				nVer++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type & TP_type_vertical)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;
				nVer++;
				bjump=false;
			}
		}
		
		if(!bjump)
		{
			fprintf(fp, "%d %.3f %.3f %.3f %.3f %.3f %.3f\n", i, pLidVP[i].vx,pLidVP[i].vy, pLidVP[i].vz, pdx[i], pdy[i], pdz[i]);

			rms_x+=pdx[i]*pdx[i];
			rms_y+=pdy[i]*pdy[i];
			rms_z+=pdz[i]*pdz[i];
		}
	}
	rms=rms_x+rms_y+rms_z;

	rms_x=sqrt(rms_x/(nHor));
	rms_y=sqrt(rms_y/(nHor));
	rms_z=sqrt(rms_z/(nVer));

	rms = sqrt(rms/(2*nHor+nVer));
	fprintf(fp, "####### RMS #######\n");
	fprintf(fp, "%.5lf\n", rms);
	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);
//#endif
	//解方程L=AX

	m_pn = pnp;
	if(m_covar)	delete m_covar;
	m_covar = new double[m_pn*m_pn];

	int ret=dlevmar_der(cal_VirtualPoint_VCP,	//计算L
								jac_VirtualPoint_VCP, //计算A
								X, L, pnp, nObs, 200, opts, info, NULL, m_covar, &data);

// 	int ret=dlevmar_dif(cal_VirtualPoint_VCP,	//计算L
// 		X, L, pnp, nObs, 200, opts, info, NULL, m_covar, &data);

	m_rms = sqrt(info[1]/(nObs - pnp));

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

			m_corcoef[k] = Dxy/Dxx/Dyy;
			k++;
		}
	}

//	int ret=dlevmar_der(distance_PointToPlane, jac_dis_PointToPlane, X, L, pnp, nVP, 1000, opts, info, NULL, NULL, &data);

//#ifdef _DEBUG
	cal_Obs_VCP(X, hx, pnp, nObs, &data);
	rms_x=rms_y=rms_z=0;
	fprintf(fp, "after adjustment\n");
	for(i=0, j=0; i<nVP;	i++)
	{
		bjump=true;
		if(TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=hx[j]-L[j];		j++;	

				nHor++;
				nVer++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;	//平面点，高程误差赋0
				nHor++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;
				nVer++;
				bjump=false;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;			

				nHor++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type & TP_type_horizon)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;	//平面点，高程误差赋0
				nHor++;
				bjump=false;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type & TP_type_hor_ver)
			{
				pdx[i]=0;		
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;	

				nVer++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type & TP_type_vertical)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;
				nVer++;
				bjump=false;
			}
		}

		if(!bjump)
		{
			fprintf(fp, "%d %.3f %.3f %.3f %.3f %.3f %.3f\n", i, pLidVP[i].vx,pLidVP[i].vy, pLidVP[i].vz, pdx[i], pdy[i], pdz[i]);

			rms_x+=pdx[i]*pdx[i];
			rms_y+=pdy[i]*pdy[i];
			rms_z+=pdz[i]*pdz[i];
		}
	}
	rms_x=sqrt(rms_x/(nHor));
	rms_y=sqrt(rms_y/(nHor));
	rms_z=sqrt(rms_z/(nVer));

	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);


	fprintf(fp, "## parameters estmation\n");
	for(i=0; i<pnp; i++)
	{
		fprintf(fp, "%.9lf\n", X[i]);
	}

	fprintf(fp, "####### RMS #######\n");
	fprintf(fp, "%.5lf\n", m_rms);

	fprintf(fp, "####### covariance of unknowns #######\n");
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
				fprintf(fp," %e", fabs(m_corcoef[k]));
				k++;
// 			}
// 			else
// 				fprintf(fp, " -");
		}
		fprintf(fp, "\n");
	}


	if(fp)	fclose(fp);	fp=NULL;
	if(pdx)		delete pdx;			pdx=NULL;
	if(pdy)		delete pdy;			pdy=NULL;
	if(pdz)		delete pdz;			pdz=NULL;
//#endif

	if(ptObs)		delete ptObs;		ptObs=NULL;
	if(pLut)      delete pLut;		pLut=NULL;
	if(L)          delete L;				L=NULL;
	if(X)			delete X;			X=NULL;
	if(pATNPoints) delete pATNPoints;	pATNPoints=NULL;
//	if(SrcParam) delete SrcParam;	SrcParam=NULL;
	if(ptSrc)     delete ptSrc;		ptSrc=NULL;
	if(ptDst)		delete ptDst;		ptDst=NULL;
	if(hx)			delete hx;			hx=NULL;
	if(hxx)		delete hxx;			hxx=NULL;
	if(pWeight) delete pWeight;		pWeight=NULL;
	return ret;
}

struct TruePointData
{
	int nPoint;					//虚拟点总数
	int nMisalign;			//安置参数个数
	LidPt_SurvInfo	*pATNPoints;	//虚拟点对应的激光点
	double *pPDC;			//激光点坐标x, y, z对安置参数的偏导数
	double tx, ty, tz;
};

// void cal_TruePoint_xyzrph(double *p, double *y, int m, int n, void *data)
// {
// 	register int i;
// 	struct TruePointData *dptr;
// 	double      *pMisalign;
// 	//	int nMisalign;
// 	int nPoint;
// 	ATNPoint *pATNPoints;
// 
// 	dptr=(struct TruePointData *)data;
// 	//	nMisalign=dptr->nMisalign;
// 	nPoint=dptr->nPoint;
// 	pMisalign=p;
// 	pATNPoints=dptr->pATNPoints;	
// 
// 	_calLaserPosition(pATNPoints, nPoint, pMisalign[0], pMisalign[1], pMisalign[2], pMisalign[3], pMisalign[4], pMisalign[5]);
// 
// // 	for(i=0; i<nPoint; i++)
// // 	{
// // 		y[i*3]=pATNPoints[i].x;
// // 		y[i*3+1]=pATNPoints[i].y;
// // 		y[i*3+2]=pATNPoints[i].z;
// // 	}
// 	for(i=0; i<nPoint; i++)
// 	{
// 		y[i*3]=pATNPoints[i].x+pMisalign[6];	//delta_Xs
// 		y[i*3+1]=pATNPoints[i].y+pMisalign[7];	//delta_Ys
// 		y[i*3+2]=pATNPoints[i].z+pMisalign[8];	//delta_Zs
// 	}
// }

// void cal_TruePoint_rph(double *p, double *y, int m, int n, void *data)
// {
// 	register int i;
// 	struct TruePointData *dptr;
// 	double      *pMisalign;
// 	//	int nMisalign;
// 	int nPoint;
// 	ATNPoint *pATNPoints;
// 	double tx, ty, tz;
// 
// 
// 	dptr=(struct TruePointData *)data;
// 	//	nMisalign=dptr->nMisalign;
// 	nPoint=dptr->nPoint;
// 	pMisalign=p;
// 	pATNPoints=dptr->pATNPoints;	
// 	tx=dptr->tx;
// 	ty=dptr->ty;
// 	tz=dptr->tz;
// 
// 	_calLaserPosition(pATNPoints, nPoint, tx, ty, tz, pMisalign[0], pMisalign[1], pMisalign[2]);
// 
// 	for(i=0; i<nPoint; i++)
// 	{
// 		y[i*3]=pATNPoints[i].x+pMisalign[3];	//delta_Xs
// 		y[i*3+1]=pATNPoints[i].y+pMisalign[4];	//delta_Ys
// 		y[i*3+2]=pATNPoints[i].z+pMisalign[5];	//delta_Zs
// 	}
// }

//计算线性化后的未知数参数
// void jac_TruePoint_xyzrph(double *p, double *jac, int m, int n, void *data)
// {
// 	register int i;
// 	struct TruePointData *dptr;
// 	LidPt_SurvInfo *pLidPoints;
// 	double      *pMisalign;
// 	int nPoints;
// 	int pnp;
// 	double *pPDC, *pJac;
// 
// 	dptr=(struct TruePointData *)data;
// 	pLidPoints=dptr->pATNPoints;
// 	pMisalign=p;
// 	nPoints=dptr->nPoint;
// 	pnp=dptr->nMisalign;
// 	pPDC=dptr->pPDC;
// 
// 	_calPDC_misalign(pLidPoints, nPoints, pMisalign+3, pPDC/*, 3, pnp*/);
// 	double *pdX, *pdY, *pdZ;
// 	for(i=0; i<nPoints; i++)
// 	{
// 		pdX=pPDC+i*3*6;
// 		pdY=pdX+6;
// 		pdZ=pdY+6;
// 
// 		pJac=jac+i*3*m;
// 		pJac[0]=pdX[0];	pJac[1]=pdX[1];	pJac[2]=pdX[2];	
// 		pJac[3]=pdX[3];	pJac[4]=pdX[4];	pJac[5]=pdX[5];	
// 		pJac[6]=1;			pJac[7]=0;			pJac[8]=0;
// 
// 		pJac+=m;
// 		pJac[0]=pdY[0];	pJac[1]=pdY[1];	pJac[2]=pdY[2];	
// 		pJac[3]=pdY[3];	pJac[4]=pdY[4];	pJac[5]=pdY[5];
// 		pJac[6]=0;			pJac[7]=1;			pJac[8]=0;
// 		
// 		pJac+=m;
// 		pJac[0]=pdZ[0];	pJac[1]=pdZ[1];	pJac[2]=pdZ[2];	
// 		pJac[3]=pdZ[3];	pJac[4]=pdZ[4];	pJac[5]=pdZ[5];
// 		pJac[6]=0;			pJac[7]=0;			pJac[8]=1;
// 	}
// 
// }

//只对三个角度求偏导
// void jac_TruePoint_rph(double *p, double *jac, int m, int n, void *data)
// {
// 	register int i;
// 	struct TruePointData *dptr;
// 	LidPt_SurvInfo *pLidPoints;
// 	double      *pMisalign;
// 	int nPoints;
// 	int pnp;
// 	double *pPDC, *pJac;
// 
// 	dptr=(struct TruePointData *)data;
// 	pLidPoints=dptr->pATNPoints;
// 	pMisalign=p;
// 	nPoints=dptr->nPoint;
// 	pnp=dptr->nMisalign;
// 	pPDC=dptr->pPDC;
// 
// //	ASSERT(nPoints==n);
// 	ASSERT(pnp==m);
// 
// 	_calPDC_misalign(pLidPoints, nPoints, pMisalign, pPDC/*, 3, pnp*/);
// 	double *pdX, *pdY, *pdZ;
// 	for(i=0; i<nPoints; i++)
// 	{
// 		pdX=pPDC+i*3*6;
// 		pdY=pdX+6;
// 		pdZ=pdY+6;
// 
// 		pJac=jac+i*3*pnp;
// 		pJac[0]=pdX[3];	pJac[1]=pdX[4];	pJac[2]=pdX[5];	pJac[3]=1;	pJac[4]=0;	pJac[5]=0;
// 		pJac+=pnp;
// 		pJac[0]=pdY[3];	pJac[1]=pdY[4];	pJac[2]=pdY[5];	pJac[3]=0;	pJac[4]=1;	pJac[5]=0;
// 		pJac+=pnp;
// 		pJac[0]=pdZ[3];	pJac[1]=pdZ[4];	pJac[2]=pdZ[5];	pJac[3]=0;	pJac[4]=0;	pJac[5]=1;
// 	}
// }

int CLidCalib_VCP::LidMC_Misalign_NN(
		const char *pszResultFile,		/* 平差结果文件 */
		const int nVP,	        /* number of Virtual points */
		const int ngcp,		/* number of points (starting from the 1st) whose parameters should not be modified.	
							* All B_ij (see below) with i<ncon are assumed to be zero
							控制点放在观测值的起始部分*/
		LidMC_VP *pLidVP,
		TiePoint_Adj_mode TiePt_mode,
							//				 char *vmask,			/* visibility mask: vmask[i, j]=1 if point i visible in image j, 0 otherwise. nxm */
		double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
												* aj are the image j parameters, bi are the i-th point parameters,
												* size m*cnp + n*pnp
												未知数，偏心分量和安置角    
												pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
		//				const int pnp,			/* number of parameters 未知数个数*/
		orsLidSysParam oriParam,
		CalibParam_Type  param_type				/* 检校参数类型 */
)
{
//	double iReturn=-1;
	double *L=NULL;
	double *X=NULL;
	double p_eL2;
	double init_p_eL2;
	int i, j, k;
	double *pA=NULL, *pAT=NULL;
	double  *pdX=NULL;	//未知数改正数
	POINT3D *pTiePt=NULL;
	CAffineTransform	affineTrans;
//	LidMC_Patch *prePatch=NULL, *postPatch=NULL, *pTPatch=NULL;
	long *pLut=NULL;
	LidPt_SurvInfo *pATNPoints=NULL;
//	double *SrcParam;
	POINT3D *ptSrc=NULL, *ptDst=NULL;
	double *hx=NULL, *hxx=NULL;
	double *pWeight=NULL;	//观测值权阵
	
	//统计连接面数量, tieID必须连续排列
	int tieID=-1;
	int nTies=0;
	int nObs=0;
	int /*nHorVer, */nHor, nVer;
	int nXDirection, nYDirection, nZDirection;
	double wXD, wYD, wZD;


	/*nHorVer = */nHor = nVer = 0;
	for(i=0; i<nVP; i++)
	{//统计连接点个数
		if(tieID!=pLidVP[i].tieID)
		{
			nTies++;
			tieID=pLidVP[i].tieID;
		}

		if(TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver /*&& Use_Hor_Ver*/)
			{
				nHor++;
				nVer++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon /*&& Use_Hor*/)
				nHor++;
			else if(pLidVP[i].VP_type == TP_type_vertical /*&& Use_Ver*/)
				nVer++;
		}
		else if(TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver /*&& Use_Hor_Ver*/)
			{
				nHor++;

			}
			else if(pLidVP[i].VP_type == TP_type_horizon /*&& Use_Hor*/)
				nHor++;
		}
		else if(TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver /*&& Use_Hor_Ver*/)
			{

				nVer++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical /*&& Use_Ver*/)
				nVer++;
		}
	}

	nObs = /*nHorVer*3 + */nHor*2 + nVer;
	
	nXDirection = /*nHorVer + */nHor;
	nYDirection = /*nHorVer + */nHor;
	nZDirection = /*nHorVer + */nVer;

	//定权方法，三个轴向等权
	//直接修改观测值方程，权阵乘了两次，所以权值去平方根
	if(nXDirection==0)
		wXD = 0;
	else
		wXD = sqrt(1.0/nXDirection);

	if(nYDirection==0)
		wYD = 0;
	else
		wYD = sqrt(1.0/nYDirection);

	if(nZDirection==0)
		wZD = 0;
	else
		wZD = sqrt(1.0/nZDirection);


	pATNPoints=new LidPt_SurvInfo[nVP];
	for(i=0; i<nVP; i++)
	{
		pATNPoints[i]=(*(pLidVP[i].pLidPoints))[0];
	}

	////////////////////////////////////////////////////////////
	/////////////////计算斜距///////////////////////////////////
	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_RangeVec(pATNPoints, nVP);

	POINT3D *ptObs=NULL;
	ptObs=new POINT3D[nTies];	//虚拟点期望值 (取平均)
	pLut=new long[nTies+1];

	int nTieGcp=0;
	tieID=-1;
	for(i=0; i<ngcp; i++)
	{
		if(tieID!=pLidVP[i].tieID)
		{
			tieID=pLidVP[i].tieID;
			ptObs[tieID].X=pLidVP[i].eX;
			ptObs[tieID].Y=pLidVP[i].eY;
			ptObs[tieID].Z=pLidVP[i].eZ;

			nTieGcp++;
			pLut[tieID]=i;
		}
	}
	
	int nLinks=0;
	for(i=ngcp; i<nVP; i++)
	{
		if(tieID!=pLidVP[i].tieID)
		{
			tieID=pLidVP[i].tieID;
			pLut[tieID]=i;

			ptObs[tieID].X=pLidVP[i].vx;
			ptObs[tieID].Y=pLidVP[i].vy;
			ptObs[tieID].Z=pLidVP[i].vz;
		}
		else
		{
			ptObs[tieID].X+=pLidVP[i].vx;
			ptObs[tieID].Y+=pLidVP[i].vy;
			ptObs[tieID].Z+=pLidVP[i].vz;
		}
	}
	pLut[nTies]=nVP;

	for(i=nTieGcp; i<nTies; i++)
	{
		nLinks=pLut[i+1]-pLut[i];	//每个连接点对应的片数
		//连接点取平均值
		ptObs[i].X/=nLinks;
		ptObs[i].Y/=nLinks;
		ptObs[i].Z/=nLinks;

		for(j=pLut[i]; j<pLut[i+1]; j++)
		{
			pLidVP[j].eX=ptObs[i].X;
			pLidVP[j].eY=ptObs[i].Y;
			pLidVP[j].eZ=ptObs[i].Z;
		}
	}

//	double *L=NULL;
	//与观测方程相关
	L=new double[nObs];	//方程数
	hx=new double[nObs];
	hxx=new double[nObs];
	pWeight = new double[nObs];
	for(i=0; i<nObs; i++)
	{
		pWeight[i] = 1.0;
	}

	//加权效果不好，暂时用等权
	wXD = wYD = wZD = 1;

	for(i=0, j=0; i<nVP; i++)
	{
		if(TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wZD;	j++;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{
				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wXD;	j++;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
				
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				L[j]=pLidVP[i].eX * wXD;	pWeight[j] = wXD;	j++;
				L[j]=pLidVP[i].eY * wYD;	pWeight[j] = wYD;	j++;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wZD;	j++;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{
				L[j]=pLidVP[i].eZ * wZD;	pWeight[j] = wXD;	j++;
			}
		}
	}

	int pnp=0;
	if(param_type & LidAdj_boreAngle)
		pnp += 3;
	if(param_type & LidAdj_leverArm)
		pnp += 3;
	if(param_type & LidAdj_GPS_drift)
		pnp += 6;
	if(param_type & LidAdj_INS_drift)
		pnp += 6;
	
	X=new double[pnp];
	memcpy(X, pX, sizeof(double)*pnp);
	
/*
	if(param_type==Calib_rphxyz)
	{
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i];
		}
	}
	else if(param_type==Calib_rph)
	{//前三个为偏心分量
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i+3];
		}
	}
	else if(param_type==Calib_rph_appXYZ)
	{
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i+3];
		}
	}
	else if(param_type==Calib_rphxyz_sa0)
	{
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i];
		}
	}
	else if(param_type==Calib_rphxyz_sa0_sac)
	{
		for(i=0; i<pnp; i++)
		{
			X[i]=pX[i];
		}
	}*/

	//与激光点相关，一个虚拟点对应一个最近激光点
	ptSrc=new POINT3D[nVP];	
	ptDst=new POINT3D[nVP];

	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;

	struct VPObs data;
	data.nMisalign=pnp;
	data.nTies=nTies;
	data.nObs=nObs;
	data.nTieGCP=nTieGcp;
	data.nVP=nVP;
	data.pLidVP=pLidVP;
	data.TiePt_mode = TiePt_mode;
	data.pLut=pLut;
	data.pATNPoints=pATNPoints;
	data.L=L;
	//	data.SrcParam=SrcParam;
	data.ptSrc=ptSrc;
	data.ptDst=ptDst;
	data.hx=hx;
	data.hxx=hxx;
	data.pWeight = pWeight;

	data.param_type=param_type;
// 	data.tx=pX[0];
// 	data.ty=pX[1];
// 	data.tz=pX[2];
// 	data.r=pX[3];
// 	data.p=pX[4];
// 	data.h=pX[5];
	data.oriParam = &oriParam;
	data.pLidGeo = lidGeo.get();
/*
	if(param_type==Calib_rphxyz_sa0)
	{
		data.sa0=pX[6];
		data.sa_ceof=0;
	}
	else
	{
		data.sa0=0;	
		data.sa_ceof=0;
	}

	if(param_type==Calib_rphxyz_sa0_sac)
	{
		data.sa0=pX[6];
		data.sa_ceof=pX[7];
	}
	else
	{
		data.sa0=0;	
		data.sa_ceof=0;
	}*/

	//_calLaserPosition(pATNPoints, nVP, &oriParam, pX, param_type);
	orsLidSysParam curParam = oriParam;
	switch (param_type)
	{
	case Calib_rph:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		break;
	case Calib_rphxyz:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.lever_arm[0] = pX[3];
		curParam.lever_arm[1] = pX[4];
		curParam.lever_arm[2] = pX[5];
		break;
	case Calib_rph_appXYZ:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.semiXYZ[0] = pX[3];
		curParam.semiXYZ[1] = pX[4];
		curParam.semiXYZ[2] = pX[5];
		break;
	case Calib_rphxyz_sa0:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.lever_arm[0] = pX[3];
		curParam.lever_arm[1] = pX[4];
		curParam.lever_arm[2] = pX[5];
		curParam.sa0 = pX[6];
		break;
	case Calib_rphxyz_sa0_sac:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.lever_arm[0] = pX[3];
		curParam.lever_arm[1] = pX[4];
		curParam.lever_arm[2] = pX[5];
		curParam.sa0 = pX[6];
		curParam.sac = pX[7];
		break;
	case Calib_rph_sa0:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.sa0 = pX[3];
		break;
	case Calib_rph_drange:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.drange = pX[3];
		break;
	case Calib_rph_drange_sa0:
		curParam.boresight_angle[0] = pX[0];
		curParam.boresight_angle[1] = pX[1];
		curParam.boresight_angle[2] = pX[2];
		curParam.drange = pX[3];
		curParam.sa0 = pX[4];
		break;
	default:
		;
	}
	lidGeo->setSysParam((void*)&curParam);
	lidGeo->cal_Laserfootprint(pATNPoints, nVP);

	for(i=0; i<nVP; i++)
	{//用当前参数计算新的激光点坐标
		ptSrc[i].X=pATNPoints[i].x;
		ptSrc[i].Y=pATNPoints[i].y;
		ptSrc[i].Z=pATNPoints[i].z;
	}

//#ifdef _DEBUG
	FILE *fp=NULL;
	fp=fopen(pszResultFile, "wt");
	if(fp==NULL)
	{
		assert(false);
		return -1;
	}

	fprintf(fp, "## calibration method: Nearest Neighbour Points\n");
	fprintf(fp, "TiePoint mode: %d\n", TiePt_mode);
	fprintf(fp, "## calib param type: %d\n", param_type);
	fprintf(fp, "## init calib param\n");
	for(i=0; i<pnp; i++)
	{
		fprintf(fp, "%.9lf\n", pX[i]);
	}

	fprintf(fp, "tiepoints: %d; total points=%d hor=%d ver=%d\n", nTies, nVP, nHor, nVer);

	//误差值
	double *pdx=new double[nVP];
	double *pdy=new double[nVP];
	double *pdz=new double[nVP];
	double rms_x, rms_y, rms_z, rms;;
	
	cal_Obs_NN(X, hx, pnp, nObs, &data);
	rms = rms_x=rms_y=rms_z=0;
	fprintf(fp, "before adjustment\n");
	/*nHorVer=0, */nHor=0, nVer=0;
	bool bjump;
	for(i=0, j=0; i<nVP;	i++)
	{
		bjump=true;
		if(TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=hx[j]-L[j];		j++;	

				nHor++;
				nVer++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;	//平面点，高程误差赋0
				nHor++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;
				nVer++;
				bjump=false;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;			

				nHor++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type & TP_type_horizon)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;	//平面点，高程误差赋0
				nHor++;
				bjump=false;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type & TP_type_hor_ver)
			{
				pdx[i]=0;		
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;	

				nVer++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type & TP_type_vertical)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;
				nVer++;
				bjump=false;
			}
		}

		if(!bjump)
		{
			fprintf(fp, "%d %.3f %.3f %.3f %.3f %.3f %.3f\n", i, pLidVP[i].vx,pLidVP[i].vy, pLidVP[i].vz, pdx[i], pdy[i], pdz[i]);

			rms_x+=pdx[i]*pdx[i];
			rms_y+=pdy[i]*pdy[i];
			rms_z+=pdz[i]*pdz[i];
		}
	}
	rms = rms_x+rms_y+rms_z;
	rms_x=sqrt(rms_x/(nHor));
	rms_y=sqrt(rms_y/(nHor));
	rms_z=sqrt(rms_z/(nVer));

	rms = sqrt(rms/(2*nHor+nVer));
	fprintf(fp, "####### RMS #######\n");
	fprintf(fp, "%.5lf\n", rms);
	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);
//#endif
	//解方程L=AX

	m_pn = pnp;
	if(m_covar)	delete m_covar;
	m_covar = new double[m_pn*m_pn];

	int ret=dlevmar_der(cal_VirtualPoint_NN,	//计算L
								jac_VirtualPoint_NN, //计算A
								X, L, pnp, nObs, 200, opts, info, NULL, m_covar, &data);

	m_rms = sqrt(info[1]/(nObs - pnp));

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

			m_corcoef[k] = Dxy/Dxx/Dyy;
			k++;
		}
	}

//	int ret=dlevmar_der(distance_PointToPlane, jac_dis_PointToPlane, X, L, pnp, nVP, 1000, opts, info, NULL, NULL, &data);

//#ifdef _DEBUG
	cal_Obs_NN(X, hx, pnp, nObs, &data);
	rms_x=rms_y=rms_z=0;
	fprintf(fp, "after adjustment\n");
	for(i=0, j=0; i<nVP;	i++)
	{
		bjump=true;
		if(TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=hx[j]-L[j];		j++;	

				nHor++;
				nVer++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type == TP_type_horizon)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;	//平面点，高程误差赋0
				nHor++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type == TP_type_vertical)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;
				nVer++;
				bjump=false;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidVP[i].VP_type == TP_type_hor_ver)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;			

				nHor++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type & TP_type_horizon)
			{
				pdx[i]=hx[j]-L[j];		j++;
				pdy[i]=hx[j]-L[j];		j++;
				pdz[i]=0;	//平面点，高程误差赋0
				nHor++;
				bjump=false;
			}
		}
		else if(TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidVP[i].VP_type & TP_type_hor_ver)
			{
				pdx[i]=0;		
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;	

				nVer++;
				bjump=false;
			}
			else if(pLidVP[i].VP_type & TP_type_vertical)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];		j++;
				nVer++;
				bjump=false;
			}
		}

		if(!bjump)
		{
			fprintf(fp, "%d %.3f %.3f %.3f %.3f %.3f %.3f\n", i, pLidVP[i].vx,pLidVP[i].vy, pLidVP[i].vz, pdx[i], pdy[i], pdz[i]);

			rms_x+=pdx[i]*pdx[i];
			rms_y+=pdy[i]*pdy[i];
			rms_z+=pdz[i]*pdz[i];
		}
	}
	rms_x=sqrt(rms_x/(nHor));
	rms_y=sqrt(rms_y/(nHor));
	rms_z=sqrt(rms_z/(nVer));

	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);


	fprintf(fp, "## parameters estmation\n");
	for(i=0; i<pnp; i++)
	{
		fprintf(fp, "%.9lf\n", X[i]);
	}

	fprintf(fp, "####### RMS #######\n");
	fprintf(fp, "%.5lf\n", m_rms);

	fprintf(fp, "####### covariance of unknowns #######\n");
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
				fprintf(fp," %e", fabs(m_corcoef[k]));
				k++;
// 			}
// 			else
// 				fprintf(fp, " -");
		}
		fprintf(fp, "\n");
	}


	if(fp)	fclose(fp);	fp=NULL;
	if(pdx)		delete pdx;			pdx=NULL;
	if(pdy)		delete pdy;			pdy=NULL;
	if(pdz)		delete pdz;			pdz=NULL;
//#endif

	if(ptObs)		delete ptObs;		ptObs=NULL;
	if(pLut)      delete pLut;		pLut=NULL;
	if(L)          delete L;				L=NULL;
	if(X)			delete X;			X=NULL;
	if(pATNPoints) delete pATNPoints;	pATNPoints=NULL;
//	if(SrcParam) delete SrcParam;	SrcParam=NULL;
	if(ptSrc)     delete ptSrc;		ptSrc=NULL;
	if(ptDst)		delete ptDst;		ptDst=NULL;
	if(hx)			delete hx;			hx=NULL;
	if(hxx)		delete hxx;			hxx=NULL;
	if(pWeight) delete pWeight;		pWeight=NULL;
	return ret;
}

//2010.12.30 增加GPS系统误差解算
// int CLidCalib_VCP::LidMC_Misalign_VTP_NN_rph_semiXYZ(
// 					const int nPoint,	        /* number of Virtual points */
// 				    ATNPoint *pLidPoint,
//                     POINT3D *pGCP,
// 				    double *pX,			/* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
// 											* aj are the image j parameters, bi are the i-th point parameters,
// 											* size m*cnp + n*pnp
// 											未知数，偏心分量和安置角    
// 											pX=[Xt, Yt, Zt, r, p, h]^T 传入未知数初值）*/
// 				    const int pnp			/* number of parameters 未知数个数*/
//  //				 double *pObs,			/*观测值(虚拟观测值，加权平均值)*/
// // 				 double errTh2			//(迭代终止条件,物方坐标改正);
// //				VP_mode mode        //虚拟点的计算方式
// )
// {
// //	double iReturn=-1;
// 	int i;
// 	double *L=NULL;
// 	double *pPDC=NULL;
// 	double *rph;
// 	double *X=NULL;
// 
// 	L=new double[nPoint*3];
// 	for(i=0; i<nPoint; i++)
// 	{
// 		L[i*3]=pGCP[i].X;
// 		L[i*3+1]=pGCP[i].Y;
// 		L[i*3+2]=pGCP[i].Z;
// 	}
// 
// 	pPDC=new double[nPoint*3*pnp];
// 	X=new double[6];		//r, p, h, dXs, dYs, dZs;
// 
// 	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
// 	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;
// 
// 	struct TruePointData data;
// 	data.nMisalign=pnp;
// 	data.pATNPoints=pLidPoint;
// 	data.nPoint=nPoint;
// 	data.pPDC=pPDC;
// 	data.tx=pX[0];
// 	data.ty=pX[1];
// 	data.tz=pX[2];
// 
// 	rph=pX+3;
// 	X[0]=rph[0];	X[1]=rph[1];	X[2]=rph[2];
// 	X[3]=0;			X[4]=0;			X[5]=0;
// 	
// #ifdef _DEBUG
// 	FILE *fp=NULL;
// 	fp=fopen(/*"g:/D_experiment/adjustment/VP/VTP_NN_rph_semiXYZ"*/m_pResultName, "wt");
// 
// 	double *ex=new double[nPoint];
// 	double *ey=new double[nPoint];
// 	double *ez=new double[nPoint];
// 	double rms_x, rms_y, rms_z;
// 
// 	_calLaserPosition(pLidPoint, nPoint, pX[0], pX[1], pX[2], pX[3], pX[4], pX[5]);
// 	rms_x=rms_y=rms_z=0;
// 	fprintf(fp, "before adjustment\n");
// 	for(i=0; i<nPoint;	i++)
// 	{
// 		ex[i]=pLidPoint[i].x-pGCP[i].X;
// 		ey[i]=pLidPoint[i].y-pGCP[i].Y;
// 		ez[i]=pLidPoint[i].z-pGCP[i].Z;
// 		
// 		fprintf(fp, "%d %.3lf %.3lf %.3lf\n", i, ex[i], ey[i], ez[i]);
// 
// 		rms_x+=ex[i]*ex[i];
// 		rms_y+=ey[i]*ey[i];
// 		rms_z+=ez[i]*ez[i];
// 	}
// 	rms_x=sqrt(rms_x/nPoint);
// 	rms_y=sqrt(rms_y/nPoint);
// 	rms_z=sqrt(rms_z/nPoint);
// 	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);
// #endif
// 
// 	int ret=dlevmar_der(cal_TruePoint_rph, jac_TruePoint_rph, X, L, 6, nPoint*3, 1000, opts, info, NULL, NULL, &data);
// 	//	int ret=dlevmar_der(distance_PointToPlane, jac_dis_PointToPlane, X, L, pnp, nVP, 1000, opts, info, NULL, NULL, &data);
// 
// #ifdef _DEBUG
// 
// 	_calLaserPosition(pLidPoint, nPoint, pX[0], pX[1], pX[2], X[0], X[1], X[2]);
// 	rms_x=rms_y=rms_z=0;
// 	fprintf(fp, "after adjustment\n");
// 	for(i=0; i<nPoint;	i++)
// 	{
// 		ex[i]=pLidPoint[i].x-pGCP[i].X+X[3];
// 		ey[i]=pLidPoint[i].y-pGCP[i].Y+X[4];
// 		ez[i]=pLidPoint[i].z-pGCP[i].Z+X[5];
// 
// 		fprintf(fp, "%d %.3lf %.3lf %.3lf\n", i, ex[i], ey[i], ez[i]);
// 
// 		rms_x+=ex[i]*ex[i];
// 		rms_y+=ey[i]*ey[i];
// 		rms_z+=ez[i]*ez[i];
// 	}
// 	rms_x=sqrt(rms_x/nPoint);
// 	rms_y=sqrt(rms_y/nPoint);
// 	rms_z=sqrt(rms_z/nPoint);
// 	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);
// 
// 	fprintf(fp, "misalign parameter: r, p, h, dXs, dYs, dZs \n");
// 	for(i=0; i<pnp; i++)
// 	{
// 		fprintf(fp, "%.9lf\n", X[i]);
// 	}
// 	if(fp)	fclose(fp);	fp=NULL;
// 	if(ex)			delete ex;			ex=NULL;
// 	if(ey)			delete ey;			ey=NULL;
// 	if(ez)			delete ez;			ez=NULL;
// #endif
// 
// 	if(L)          delete L;			L=NULL;
// 	if(X)			delete X;			X=NULL;
// 	if(pPDC)		delete pPDC;		pPDC=NULL;
// 	return ret;
// }

int CLidCalib_VCP::LidMC_VPAdj_QA(
				const int nVP,
				const int ngcp,
				LidMC_VP *pLidVP,
				double *pInitX,			/* 未知数初值*/
				double *pAdjX,			/* 未知数平差值*/
				const int pnp,			/* number of parameters 未知数个数*/
				Calib_Method  mType,
				orsLidSysParam oriParam,
				CalibParam_Type  param_type	
)
{
	double *L=NULL;
//	double *X=NULL;
	double p_eL2;
	double init_p_eL2;
	int i, j, k;
	double *pA=NULL, *pAT=NULL;
	double  *pdX=NULL;	//未知数改正数
	POINT3D *pTiePt=NULL;
	CAffineTransform	affineTrans;
	//	LidMC_Patch *prePatch=NULL, *postPatch=NULL, *pTPatch=NULL;
	long *pLut=NULL;
	LidPt_SurvInfo *pATNPoints=NULL;
	//	double *SrcParam;
	POINT3D *ptSrc=NULL, *ptDst=NULL;
	double *hx=NULL, *hxx=NULL;

	//统计连接面数量, tieID必须连续排列
	int tieID=-1;
	int nTies=0;
	for(i=0; i<nVP; i++)
	{//统计连接点个数
		if(tieID!=pLidVP[i].tieID)
		{
			nTies++;
			tieID=pLidVP[i].tieID;
		}
	}

	j=tieID+1-nTies;
	if(j>0)
	{//连接点编号不是从0开始
		for(i=0; i<nVP; i++)
		{//重新整理连接点号
			pLidVP[i].tieID=pLidVP[i].tieID-j;
		}	
	}
	
	int candNum;
	switch (mType)
	{
	case Lid_Calib_VP_VCP:
		candNum=3;
		break;
	case Lid_Calib_VP_NN:
		candNum=1;
		break;
	default:
		candNum = 1;
	}

	pATNPoints=new LidPt_SurvInfo[nVP*candNum];
	for(i=0; i<nVP; i++)
	{
		for(j=0; j<candNum; j++)
		{
			pATNPoints[i*3+j]=(*(pLidVP[i].pLidPoints))[j];
		}
	}

	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	lidGeo->setSysParam((void*)&oriParam);
	lidGeo->cal_RangeVec(pATNPoints, nVP*candNum);

	POINT3D *ptObs=NULL, *ptLinkObs;
	ptObs=new POINT3D[nTies];
	pLut=new long[nTies+1];

	int nTieGcp=0;
	tieID=-1;
	for(i=0; i<ngcp; i++)
	{
		if(tieID!=pLidVP[i].tieID)
		{
			tieID=pLidVP[i].tieID;
			ptObs[tieID].X=pLidVP[i].eX;
			ptObs[tieID].Y=pLidVP[i].eY;
			ptObs[tieID].Z=pLidVP[i].eZ;

			nTieGcp++;
			pLut[tieID]=i;
		}
	}

	int nLinks=0;
	// 	ptLinkObs=ptObs+nTieGcp;
	// 	memset(ptLinkObs, 0, sizeof(POINT3D)*(nTies-nTieGcp));
	for(i=ngcp; i<nVP; i++)
	{
		if(tieID!=pLidVP[i].tieID)
		{
			// 			ptObs[tieID].X/=nLinks;
			// 			ptObs[tieID].Y/=nLinks;
			// 			ptObs[tieID].Z/=nLinks;

			tieID=pLidVP[i].tieID;
			pLut[tieID]=i;

			ptObs[tieID].X=pLidVP[i].vx;
			ptObs[tieID].Y=pLidVP[i].vy;
			ptObs[tieID].Z=pLidVP[i].vz;

			//			nLinks=1;
		}
		else
		{
			ptObs[tieID].X+=pLidVP[i].vx;
			ptObs[tieID].Y+=pLidVP[i].vy;
			ptObs[tieID].Z+=pLidVP[i].vz;
			//			nLinks++;
		}
	}
	pLut[nTies]=nVP;

	for(i=nTieGcp; i<nTies; i++)
	{
		nLinks=pLut[i+1]-pLut[i];	//每个连接点对应的片数
		//连接点取平均值
		ptObs[i].X/=nLinks;
		ptObs[i].Y/=nLinks;
		ptObs[i].Z/=nLinks;

		for(j=pLut[i]; j<pLut[i+1]; j++)
		{
			pLidVP[j].eX=ptObs[i].X;
			pLidVP[j].eY=ptObs[i].Y;
			pLidVP[j].eZ=ptObs[i].Z;
		}
	}

	L=new double[nVP*3];
	for(i=0; i<nVP; i++)
	{
		L[i*3]=pLidVP[i].eX;
		L[i*3+1]=pLidVP[i].eY;
		L[i*3+2]=pLidVP[i].eZ;
	}

	//	double *L=NULL;
	
	double tx, ty, tz;
	double r, p, h;
	double dxs, dys, dzs;

	hx=new double[nVP*3];
	hxx=new double[nVP*3];

	ptSrc=new POINT3D[nVP*3];
	ptDst=new POINT3D[nVP*3];


	//_calLaserPosition(pATNPoints, nVP*candNum, &oriParam, pInitX, param_type);
	orsLidSysParam curParam = oriParam;
	switch (param_type)
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
	lidGeo->cal_RangeVec(pATNPoints, nVP*candNum);
	
	for(i=0; i<nVP*candNum; i++)
	{//用当前参数计算新的激光点坐标
		ptSrc[i].X=pATNPoints[i].x;
		ptSrc[i].Y=pATNPoints[i].y;
		ptSrc[i].Z=pATNPoints[i].z;
	}

	ptSrc=new POINT3D[nVP*3];	
	ptDst=new POINT3D[nVP*3];

	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;

	struct VPObs data;
	data.nMisalign=pnp;
	data.nTies=nTies;
	data.nObs=nVP*3;
	data.nTieGCP=nTieGcp;
	data.nVP=nVP;
	data.pLidVP=pLidVP;
	data.pLut=pLut;
	data.pATNPoints=pATNPoints;
	data.L=L;
	//	data.SrcParam=SrcParam;
	data.ptSrc=ptSrc;
	data.ptDst=ptDst;
	data.hx=hx;
	data.hxx=hxx;
	data.pWeight = NULL;

	data.param_type=param_type;
	data.oriParam = &oriParam;


	FILE *fp=NULL;
	
	CString  strFileName;
	CFileDialog output(FALSE," ",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);
	output.m_ofn.lpstrTitle="保存检查点精度结果";

// 	if(paramtype==LiDMC_rph)
// 		strFileName="g:/D_experiment/adjustment/QA/VP_rph";
// 	else if(paramtype==LiDMC_xyzrph)
// 		strFileName="g:/D_experiment/adjustment/QA/VP_xyzrph";
// 	else if(paramtype==LiDMC_xyzrph_SemiXYZ)
// 		strFileName="g:/D_experiment/adjustment/QA/VP_xyzrph_SemiXYZ";
// 	else if(paramtype==LiDMC_rph_SemiXYZ)
// 		strFileName="g:/D_experiment/adjustment/QA/VP_rph_SemiXYZ";

	if(output.DoModal()!=IDOK)
		return -1;
	strFileName = output.GetPathName();

	fp=fopen(strFileName, "wt");

	double *ex=new double[nVP];
	double *ey=new double[nVP];
	double *ez=new double[nVP];
	double rms_x, rms_y, rms_z;

	switch (mType)
	{
	case Lid_Calib_VP_VCP:
		cal_Obs_VCP(pInitX, hx, pnp, nVP*candNum, &data);
		break;
	case Lid_Calib_VP_NN:
		cal_Obs_NN(pInitX, hx, pnp, nVP*candNum, &data);
		break;
	}
	
	rms_x=rms_y=rms_z=0;
	fprintf(fp, "before adjustment\n");
	for(i=0; i<nVP;	i++)
	{
		j=i*3;
		ex[i]=hx[j]-L[j];
		ey[i]=hx[j+1]-L[j+1];
		ez[i]=hx[j+2]-L[j+2];

		fprintf(fp, "%d %.3f %.3f %.3f\n", i, ex[i], ey[i], ez[i]);

		rms_x+=ex[i]*ex[i];
		rms_y+=ey[i]*ey[i];
		rms_z+=ez[i]*ez[i];
	}
	rms_x=sqrt(rms_x/nVP);
	rms_y=sqrt(rms_y/nVP);
	rms_z=sqrt(rms_z/nVP);
	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);

	switch (mType)
	{
	case Lid_Calib_VP_VCP:
		cal_Obs_VCP(pAdjX, hx, pnp, nVP*candNum, &data);
		break;
	case Lid_Calib_VP_NN:
		cal_Obs_NN(pAdjX, hx, pnp, nVP*candNum, &data);
		break;
	}
	
	cal_EX(&data);

	rms_x=rms_y=rms_z=0;
	fprintf(fp, "after adjustment\n");
	for(i=0; i<nVP;	i++)
	{
		j=i*3;
		ex[i]=hx[j]-L[j];
		ey[i]=hx[j+1]-L[j+1];
		ez[i]=hx[j+2]-L[j+2];

		fprintf(fp, "%d %.3f %.3f %.3f\n", i, ex[i], ey[i], ez[i]);

		rms_x+=ex[i]*ex[i];
		rms_y+=ey[i]*ey[i];
		rms_z+=ez[i]*ez[i];
	}
	rms_x=sqrt(rms_x/nVP);
	rms_y=sqrt(rms_y/nVP);
	rms_z=sqrt(rms_z/nVP);
	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);

	
	if(fp)	fclose(fp);	fp=NULL;
	if(ex)			delete ex;			ex=NULL;
	if(ey)			delete ey;			ey=NULL;
	if(ez)			delete ez;			ez=NULL;


	if(ptObs)		delete ptObs;		ptObs=NULL;
	if(pLut)      delete pLut;		pLut=NULL;
	if(L)          delete L;				L=NULL;
//	if(X)			delete X;			X=NULL;
	if(pATNPoints) delete pATNPoints;	pATNPoints=NULL;
	//	if(SrcParam) delete SrcParam;	SrcParam=NULL;
	if(ptSrc)     delete ptSrc;		ptSrc=NULL;
	if(ptDst)		delete ptDst;		ptDst=NULL;
	if(hx)			delete hx;			hx=NULL;
	if(hxx)		delete hxx;			hxx=NULL;
	return 0;
}

// void LidStatDis_VP(LidMC_VP *pLidVP, int nVP, int ngcp, double *pX, oriCalibParam oriParam, 
// 								  CalibParam_Type  param_type, double *pdx, double *pdy, double *pdz, double *rms)
// {
// //	double iReturn=-1;
// 	double *L=NULL;
// 	double *X=NULL;
// 	double p_eL2;
// 	double init_p_eL2;
// 	int i, j, k;
// 	double *pA=NULL, *pAT=NULL;
// 	double  *pdX=NULL;	//未知数改正数
// 	POINT3D *pTiePt=NULL;
// 	CAffineTransform	affineTrans;
// //	LidMC_Patch *prePatch=NULL, *postPatch=NULL, *pTPatch=NULL;
// 	long *pLut=NULL;
// 	ATNPoint *pATNPoints=NULL;
// //	double *SrcParam;
// 	POINT3D *ptSrc=NULL, *ptDst=NULL;
// 	double *hx=NULL, *hxx=NULL;
// 	
// 	//统计连接面数量, tieID必须连续排列
// 	int tieID=-1;
// 	int nTies=0;
// 	int nObs=0;
// 	for(i=0; i<nVP; i++)
// 	{//统计连接点个数
// 		if(tieID!=pLidVP[i].tieID)
// 		{
// 			nTies++;
// 			tieID=pLidVP[i].tieID;
// 		}
// 
// 		if(pLidVP[i].VP_type==TP_type_hor_ver)
// 			nObs+=3;
// 		else if(pLidVP[i].VP_type==TP_type_horizon)
// 			nObs+=2;
// 		else if(pLidVP[i].VP_type==TP_type_vertical)
// 			nObs+=1;
// 	}
// 
// 	double *pWeight = new double[nObs];
// 	for(i=0; i<nObs; i++)
// 	{
// 		pWeight[i] = 1.0;
// 	}
// 
// 	//	double *L=NULL;
// 	//与观测方程相关
// 	L=new double[nObs];	//方程数
// 	hx=new double[nObs];
// 	hxx=new double[nObs];
// 
// 	pATNPoints=new ATNPoint[nVP*3];
// 	for(i=0; i<nVP; i++)
// 	{
// 		for(j=0; j<3; j++)
// 		{//提取真实激光点
// 			pATNPoints[i*3+j]=(*(pLidVP[i].pLidPoints))[j];
// 		}
// 	}
// 
// 	POINT3D *ptObs=NULL;
// 	ptObs=new POINT3D[nTies];	//虚拟点期望值 (取平均)
// 	pLut=new long[nTies+1];
// 
// 
// 	int pnp;
// 	switch (param_type)
// 	{
// 	case Calib_rph:
// 		pnp = 3;
// 		break;
// 	case Calib_rphxyz:
// 		pnp = 6;
// 		break;
// 	case Calib_rph_appXYZ:
// 		pnp = 6;
// 		break;
// 	case Calib_rphxyz_sa0:
// 		pnp = 7;
// 		break;
// 	case Calib_rphxyz_sa0_sac:
// 		pnp = 8;
// 		break;
// 	case Calib_rph_sa0:
// 		pnp = 4;
// 		break;
// 	case Calib_rph_drange:
// 		pnp = 4;
// 		break;
// 	case Calib_rph_drange_sa0:
// 		pnp = 5;
// 		break;
// 	default:
// 		pnp = 0;
// 	}
// 	
// 	X=new double[pnp];
// 	memcpy(X, pX, sizeof(double)*pnp);
// 	
// 
// 	//与激光点相关，一个虚拟点对应三个激光点
// 	ptSrc=new POINT3D[nVP*3];	
// 	ptDst=new POINT3D[nVP*3];
// 
// 	int nTieGcp=0;
// 	tieID=-1;
// 	for(i=0; i<ngcp; i++)
// 	{
// 		if(tieID!=pLidVP[i].tieID)
// 		{
// 			tieID=pLidVP[i].tieID;
// 			ptObs[tieID].X=pLidVP[i].eX;
// 			ptObs[tieID].Y=pLidVP[i].eY;
// 			ptObs[tieID].Z=pLidVP[i].eZ;
// 
// 			nTieGcp++;
// 			pLut[tieID]=i;
// 		}
// 	}
// 	
// 	struct VPObs data;
// 	data.nMisalign=pnp;
// 	data.nTies=nTies;
// 	data.nObs=nObs;
// 	data.nTieGCP=nTieGcp;
// 	data.nVP=nVP;
// 	data.pLidVP=pLidVP;
// 	data.pLut=pLut;
// 	data.pATNPoints=pATNPoints;
// 	data.L=L;
// 	//	data.SrcParam=SrcParam;
// 	data.ptSrc=ptSrc;
// 	data.ptDst=ptDst;
// 	data.hx=hx;
// 	data.hxx=hxx;
// 
// 	data.param_type=param_type;
// 	data.oriParam = oriParam;
// 	data.pWeight = pWeight;
// 
// 	
// 	for(i=0; i<nVP*3; i++)
// 	{//用当前参数计算新的激光点坐标
// 		ptSrc[i].X=pATNPoints[i].x;
// 		ptSrc[i].Y=pATNPoints[i].y;
// 		ptSrc[i].Z=pATNPoints[i].z;
// 	}
// 
// 	double rms_x, rms_y, rms_z;
// 	
// 	cal_VPObs(X, hx, pnp, nObs, &data);
// 
// 
// 	int nLinks=0;
// 	for(i=ngcp; i<nVP; i++)
// 	{
// 		if(tieID!=pLidVP[i].tieID)
// 		{
// 			tieID=pLidVP[i].tieID;
// 			pLut[tieID]=i;
// 
// 			ptObs[tieID].X=pLidVP[i].vx;
// 			ptObs[tieID].Y=pLidVP[i].vy;
// 			ptObs[tieID].Z=pLidVP[i].vz;
// 		}
// 		else
// 		{
// 			ptObs[tieID].X+=pLidVP[i].vx;
// 			ptObs[tieID].Y+=pLidVP[i].vy;
// 			ptObs[tieID].Z+=pLidVP[i].vz;
// 		}
// 	}
// 	pLut[nTies]=nVP;
// 
// 	for(i=nTieGcp; i<nTies; i++)
// 	{
// 		nLinks=pLut[i+1]-pLut[i];	//每个连接点对应的片数
// 		//连接点取平均值
// 		ptObs[i].X/=nLinks;
// 		ptObs[i].Y/=nLinks;
// 		ptObs[i].Z/=nLinks;
// 
// 		for(j=pLut[i]; j<pLut[i+1]; j++)
// 		{
// 			pLidVP[j].eX=ptObs[i].X;
// 			pLidVP[j].eY=ptObs[i].Y;
// 			pLidVP[j].eZ=ptObs[i].Z;
// 		}
// 	}
// 
// 	
// 	for(i=0, j=0; i<nVP; i++)
// 	{
// 		if(pLidVP[i].VP_type==TP_type_hor_ver)
// 		{
// 			L[j++]=pLidVP[i].eX;
// 			L[j++]=pLidVP[i].eY;
// 			L[j++]=pLidVP[i].eZ;
// 		}
// 		else if(pLidVP[i].VP_type==TP_type_horizon)
// 		{
// 			L[j++]=pLidVP[i].eX;
// 			L[j++]=pLidVP[i].eY;
// 		}
// 		else if(pLidVP[i].VP_type==TP_type_vertical)
// 		{
// 			L[j++]=pLidVP[i].eZ;
// 		}
// 	}
// 
// 	rms_x=rms_y=rms_z=0;
// 
// 	int nHorVer=0, nHor=0, nVer=0;
// 	for(i=0, j=0; i<nVP;	i++)
// 	{
// 		if(pLidVP[i].VP_type==TP_type_hor_ver)
// 		{
// 			pdx[i]=hx[j]-L[j];		j++;
// 			pdy[i]=hx[j]-L[j];		j++;
// 			pdz[i]=hx[j]-L[j];		j++;	
// 			nHorVer++;
// 		}
// 		else if(pLidVP[i].VP_type==TP_type_horizon)
// 		{
// 			pdx[i]=hx[j]-L[j];		j++;
// 			pdy[i]=hx[j]-L[j];		j++;
// 			pdz[i]=0;	//平面点，高程误差赋0
// 			nHor++;
// 		}
// 		else if(pLidVP[i].VP_type==TP_type_vertical)
// 		{//高程点，平面误差赋0
// 			pdx[i]=0;	
// 			pdy[i]=0;		
// 			pdz[i]=hx[j]-L[j];		j++;
// 			nVer++;
// 		}
// 
// 		rms_x+=pdx[i]*pdx[i];
// 		rms_y+=pdy[i]*pdy[i];
// 		rms_z+=pdz[i]*pdz[i];
// 	}
// 	rms_x=sqrt(rms_x/(nHorVer+nHor));
// 	rms_y=sqrt(rms_y/(nHorVer+nHor));
// 	rms_z=sqrt(rms_z/(nHorVer+nVer));
// 	rms[0] = rms_x;
// 	rms[1] = rms_y;
// 	rms[2] = rms_z;
// 
// 
// // 	if(pdx)		delete pdx;			pdx=NULL;
// // 	if(pdy)		delete pdy;			pdy=NULL;
// // 	if(pdz)		delete pdz;			pdz=NULL;
// 	//#endif
// 
// 	if(ptObs)		delete ptObs;		ptObs=NULL;
// 	if(pLut)		delete pLut;		pLut=NULL;
// 	if(L)			delete L;			L=NULL;
// 	if(X)			delete X;			X=NULL;
// 	if(pATNPoints)	delete pATNPoints;	pATNPoints=NULL;
// //	if(SrcParam)	delete SrcParam;	SrcParam=NULL;
// 	if(ptSrc)		delete ptSrc;		ptSrc=NULL;
// 	if(ptDst)		delete ptDst;		ptDst=NULL;
// 	if(hx)			delete hx;			hx=NULL;
// 	if(hxx)			delete hxx;			hxx=NULL;
// 	if(pWeight)		delete pWeight;		pWeight=NULL;
// }