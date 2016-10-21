#include "StdAfx.h"
#include "LidStripAdj/LSA_POSDrift.h"

#include "CalibBasedef.h"
#include "orsLidarGeometry/orsILidarGeoModel.h"
#include "orsMath/orsIMatrixService.h"
//#include "Transform.h"

#include "F:\OpenRS\IntegratedPhotogrammetry\external\levmar-2.5\levmar.h"

//orsIMatrixService *g_matrixService=NULL;


// inline double TriangleInterpolate( POINT3D *Points, double x, double y )
// {
// 	POINT3D p1, p2, p3;
// 	double dx1, dy1, dx21, dx31, dy21, dy31, dz21, dz31;
// 
// 	p1 = Points[0];
// 	p2 = Points[1];
// 	p3 = Points[2];
// 
// 	dx1 = p1.X - x;	dy1 = p1.Y - y;
// 	dx21 = p2.X - p1.X;	dy21 = p2.Y - p1.Y;	dz21 = p2.Z - p1.Z;	
// 	dx31 = p3.X - p1.X;	dy31 = p3.Y - p1.Y;	dz31 = p3.Z - p1.Z;
// 
// 	return p1.Z + ( dx1*( dy21*dz31 - dy31*dz21) - dy1*( dx21*dz31 - dx31*dz21) )
// 		/ ( dx21*dy31 - dx31*dy21 );
// }


//观测值
struct LSA_globs
{
	int nTies;			//连接点总数
	int nTieGCP;		//连接点中的控制点数

	int nkeyPt;			//各条带同名观测点总数
	int nObs;			//观测方程数
	int nparams;		//未知数个数
	
	LidAdj_keyPt *pLidTies;		//连接点
	LidPt_SurvInfo *pATNPoints;	//虚拟点对应的激光点
	POINT3D *ptSrc;			
	POINT3D *ptDst;

	long *pTieLut;		//连接点对应的查找表
	double *L;			//重新计算的新观测值
	double *hx;			//用于差分求解未知数
	double *hxx;
	double *pWeight;	//权阵	2013.11.22

	unsigned long	param_type;
	orsLidSysParam   *calib_param;

	TiePoint_Adj_mode  TiePt_mode;

	int		nStrip;	//条带数
	double *ref_time;	//时间参考

	//Lidar geolocation model
	orsILidarGeoModel *pLidGeo;
};

static bool Use_Hor_Ver=true;
static bool Use_Hor=true;
static bool Use_Ver=false;
static TiePoint_Adj_mode tie_mode= LidAdj_TP_UseHorVer;

//计算观测值
//p:未知数， y改正后的观测值，m未知数个数，n观测值个数
void cal_Obs_POSDrift(double *p, double *y, int m, int n, void *data)
{
	register int i, j, k;
	struct LSA_globs *dptr;
	LidAdj_keyPt *pLidTies;
	double *pX, *pDrift=NULL;
	int nkeyPt;
	long *pTieLut;
	LidPt_SurvInfo *pATNPoints;
	POINT3D *ptDst, *ptSrc, *pTmp;
//	double xSrc[3], ySrc[3], zSrc[3], xDst[3], yDst[3], zDst[3];
//	double vX, vY, vZ;
	int nTies, nTieGCP, nObs;
	double *L;
//	double dXs, dYs, dZs;
	double *pWeight;
	unsigned long	param_type;
	int nStrip;

	
	dptr=(struct LSA_globs *)data;
//	nMisalign=dptr->nparams;
	pLidTies=dptr->pLidTies;
//	pMisalign=p;
//	pSrcMisalign=dptr->SrcParam;
	nkeyPt=dptr->nkeyPt;
	pATNPoints=dptr->pATNPoints;	
	pTieLut=dptr->pTieLut;
	ptDst=dptr->ptDst;
	ptSrc=dptr->ptSrc;
	nTies=dptr->nTies;
	nTieGCP=dptr->nTieGCP;
	nObs=dptr->nObs;
	L=dptr->L;
	pWeight = dptr->pWeight;
	param_type = dptr->param_type; //平差未知数类型
	nStrip = dptr->nStrip;  //条带数

	/////////////////////////////////////////////////////////
	////////////////////1.分解未知数////////////////////////
	pX = p;
	if(param_type & LidAdj_boreAngle)
	{
		dptr->calib_param->boresight_angle[0] = pX[0];
		dptr->calib_param->boresight_angle[1] = pX[1];
		dptr->calib_param->boresight_angle[2] = pX[2];
		pX += 3;
	}
	if(param_type & LidAdj_leverArm)
	{
		dptr->calib_param->lever_arm[0] = pX[0];
		dptr->calib_param->lever_arm[1] = pX[1];
		dptr->calib_param->lever_arm[2] = pX[2];
		pX += 3;
	}

	pDrift = pX; //漂移参数按条带存放

// 	if(param_type & LidAdj_GPS_drift && param_type & LidAdj_INS_drift)
// 	{
// 		pGPSDrift = pX;
// 		pX += 6*nStrip;   //所有条带的GPS漂移放一起
// 	}
// 	if(param_type & LidAdj_INS_drift)
// 	{
// 		pINSDrift = pX;	 //INS漂移放一起
// 	}

	///////////////////////////////////////////////////////////
	///////////////////2.根据漂移重新计算航迹//////////////////
	orsPOSParam raw_pos; //原始POS观测值
	orsPOSParam dst_pos; //改正后的POS
	double roll, pitch, heading;
	double Xgps, Ygps, Zgps;
	double t, t0;
	int iStrip;
	double *pCurGPSDrift, *pCurINSDrift;
	for(i=0; i<nkeyPt; i++)
	{
		raw_pos = (*(pLidTies[i].pLidPoints))[0].POS_Info;

		iStrip = raw_pos.stripID;
		t0	= dptr->ref_time[iStrip];
		t	= raw_pos.time;

		dst_pos = raw_pos;
		
		roll	= raw_pos.r;
		pitch	= raw_pos.p;
		heading	= raw_pos.h;

		Xgps	= raw_pos.coord.X;
		Ygps	= raw_pos.coord.Y;
		Zgps	= raw_pos.coord.Z;
		
		if(pDrift)
		{//存在GPS漂移
			pCurGPSDrift = pDrift + iStrip*12;
			
			dst_pos.coord.X=Xgps+pCurGPSDrift[0]+pCurGPSDrift[3]*(t-t0);
			dst_pos.coord.Y=Ygps+pCurGPSDrift[1]+pCurGPSDrift[4]*(t-t0);
			dst_pos.coord.Z=Zgps+pCurGPSDrift[2]+pCurGPSDrift[5]*(t-t0);
				
			pCurINSDrift = pCurGPSDrift + 6;

			dst_pos.r=roll+pCurINSDrift[0]+pCurINSDrift[3]*(t-t0);
			dst_pos.p=pitch+pCurINSDrift[1]+pCurINSDrift[4]*(t-t0);
			dst_pos.h=heading+pCurINSDrift[2]+pCurINSDrift[5]*(t-t0);
		}

		pATNPoints[i].POS_Info = dst_pos;
	}

	///////////////////////////////////////////////////
	//////////////3.计算改正后的脚点坐标//////////////
	dptr->pLidGeo->setSysParam(dptr->calib_param);
	dptr->pLidGeo->cal_Laserfootprint(pATNPoints, nkeyPt);

	///////////////////////////////////////////////////
	//////////////4.计算带权观测值/////////////////////
	for(i=0; i<nkeyPt; i++)
	{//用当前参数计算新的激光点坐标
		pLidTies[i].vX = ptDst[i].X = pATNPoints[i].x/*+dXs*/;
		pLidTies[i].vY = ptDst[i].Y = pATNPoints[i].y/*+dYs*/;
		pLidTies[i].vZ = ptDst[i].Z = pATNPoints[i].z/*+dZs*/;
	}

	//对连接点坐标加权
	for(i=0, j=0; i<nkeyPt; i++)
	{
		if(dptr->TiePt_mode == LidAdj_TP_UseHorVer)
		{
			if(pLidTies[i].VP_type == TP_type_hor_ver)
			{
				pLidTies[i].vX *= pWeight[j];	y[j]=pLidTies[i].vX; j++;
				pLidTies[i].vY *= pWeight[j];	y[j]=pLidTies[i].vY; j++;
				pLidTies[i].vZ *= pWeight[j];	y[j]=pLidTies[i].vZ; j++;
			}
			else if(pLidTies[i].VP_type == TP_type_horizon)
			{
				pLidTies[i].vX *= pWeight[j];	y[j]=pLidTies[i].vX; j++;
				pLidTies[i].vY *= pWeight[j];	y[j]=pLidTies[i].vY; j++;
			}
			else if(pLidTies[i].VP_type == TP_type_vertical)
			{
				pLidTies[i].vZ *= pWeight[j];	y[j]=pLidTies[i].vZ; j++;
			}
		}
		else if(dptr->TiePt_mode == LidAdj_TP_UseHor)
		{
			if(pLidTies[i].VP_type == TP_type_hor_ver)
			{
				pLidTies[i].vX *= pWeight[j];	y[j]=pLidTies[i].vX; j++;
				pLidTies[i].vY *= pWeight[j];	y[j]=pLidTies[i].vY; j++;
			}
			else if(pLidTies[i].VP_type == TP_type_horizon)
			{
				pLidTies[i].vX *= pWeight[j];	y[j]=pLidTies[i].vX; j++;
				pLidTies[i].vY *= pWeight[j];	y[j]=pLidTies[i].vY; j++;
			}
		}
		else if(dptr->TiePt_mode == LidAdj_TP_UseVer)
		{
			if(pLidTies[i].VP_type == TP_type_hor_ver)
			{
				pLidTies[i].vZ *= pWeight[j];	y[j]=pLidTies[i].vZ; j++;
			}
			else if(pLidTies[i].VP_type == TP_type_vertical)
			{
				pLidTies[i].vZ *= pWeight[j];	y[j]=pLidTies[i].vZ; j++;
			}
		}
	}

// 	//交换激光点坐标
// 	pTmp=dptr->ptSrc;
// 	dptr->ptSrc=dptr->ptDst;
// 	dptr->ptDst=pTmp;
}

static int iter=0;
//计算连接点期望值
void cal_EX(void *data)
{
	register int i, j, k;
	struct LSA_globs *dptr;
	LidAdj_keyPt *pLidTies;

	long *pTieLut;
	int nTies, nTieGCP;
	double *L;


	dptr=(struct LSA_globs *)data;
	pTieLut=dptr->pTieLut;
	// 	ptDst=dptr->ptDst;
	// 	ptSrc=dptr->ptSrc;
	nTies=dptr->nTies;
	nTieGCP=dptr->nTieGCP;
	L=dptr->L;
	pLidTies=dptr->pLidTies;

	//计算观测值
	double ex, ey, ez;
	int nLinks;
	//0~nTieGCP为控制点
	//	TRACE("Tie VP Coord\n");

	iter++;
	for(i=nTieGCP; i<nTies; i++)
	{
		ex=0; ey=0;ez=0;
		for(j=pTieLut[i]; j<pTieLut[i+1]; j++)
		{
			ex+=pLidTies[j].vX;
			ey+=pLidTies[j].vY;
			ez+=pLidTies[j].vZ;
		}
		nLinks=pTieLut[i+1]-pTieLut[i];
		ex/=nLinks;
		ey/=nLinks;
		ez/=nLinks;

		//		if(iter%25==0)
		//			TRACE("%d %.3f %.3f %.3f\n", i, ex, ey, ez);
		for(j=pTieLut[i]; j<pTieLut[i+1]; j++)
		{
			pLidTies[j].eX=ex;
			pLidTies[j].eY=ey;
			pLidTies[j].eZ=ez;
		}
	}

	//	pW = dptr->pWeight;
	for(i=nTieGCP; i<nTies; i++)
	{
		k=pTieLut[i];	//连接点的第一个
		if(pLidTies[k].VP_type == TP_type_hor_ver && Use_Hor_Ver)
		{
			for(j=pTieLut[i]; j<pTieLut[i+1]; j++)
			{
				*L=pLidTies[k].eX;		L++;	
				*L=pLidTies[k].eY;		L++;	
				*L=pLidTies[k].eZ;		L++;	
			}
		}
		else if(pLidTies[k].VP_type == TP_type_horizon && Use_Hor)
		{
			for(j=pTieLut[i]; j<pTieLut[i+1]; j++)
			{
				*L=pLidTies[k].eX;		L++;	
				*L=pLidTies[k].eY;		L++;	
			}
		}
		else if(pLidTies[k].VP_type == TP_type_vertical && Use_Ver)
		{
			for(j=pTieLut[i]; j<pTieLut[i+1]; j++)
			{
				*L=pLidTies[k].eZ;		L++;	
			}
		}
	}
}

//有限差分法求jac
void jac_POSDrift_diff(double *p, double *jac, int m, int n, void *data)
{
	register int i, j;
	double tmp;
	register double d;
	double delta=1E-06;
	double *hx, *hxx;
	struct LSA_globs *dptr;
	double nkeyPt;
	//	double *pTieLut;

	dptr=(struct LSA_globs *)data;
	nkeyPt=dptr->nkeyPt;
	//	pTieLut=dptr->pTieLut;
	hx=dptr->hx;
	hxx=dptr->hxx;

	cal_Obs_POSDrift(p, hx, m, n, data);

	for(j=0; j<m; ++j)
	{
		/* determine d=max(1E-04*|p[j]|, delta), see HZ */
		d=(1E-04)*p[j]; // force evaluation
		d=fabs(d);
		if(d<delta)
			d=delta;

		tmp=p[j];
		p[j]+=d;

		cal_Obs_POSDrift(p, hxx, m, n, data);

		p[j]=tmp; /* restore */

		d=(1.0)/d; /* invert so that divisions can be carried out faster as multiplications */
		for(i=0; i<n; ++i)
		{
			jac[i*m+j]=(hxx[i]-hx[i])*d;
		}
	}
}

//计算各条带同名点的坐标期望值
//用期望值来代替固定的观测值
//p:未知数， y计算的虚拟点坐标，m未知数个数，n方程个数
void cal_tiePtCoord(double *p, double *y, int m, int n, void *data)
{
	cal_Obs_POSDrift(p, y, m, n, data);	//计算观测值
	cal_EX(data);					//计算期望值
}

int  LSA_POSDrift(
		const char *pszResultFile,		/* 平差结果文件 */
		const int nkeyPt,	        /* number of tie points */
		const int ngcp,		/* number of points (starting from the 1st) whose parameters should not be modified.	
							* All B_ij (see below) with i<ncon are assumed to be zero
							控制点放在观测值的起始部分*/
		LidAdj_keyPt *pLidTies,						
		double *pX,			
		int nParams,
		unsigned long param_type,          /* 平差参数类型 */
		/*oriCalibParam oriParam*/
		orsLidSysParam oriParam,
		double *refTime,
		int nStrip,
		orsILidarGeoModel *pLidGeo
)
{
//	double iReturn=-1;
	double *L=NULL;
//	double *X=NULL;
	double p_eL2;
	double init_p_eL2;
	int i, j, k;
	double *pA=NULL, *pAT=NULL;
	double  *pdX=NULL;	//未知数改正数
	POINT3D *pTiePt=NULL;
//	CAffineTransform	affineTrans;
//	LidMC_Patch *prePatch=NULL, *postPatch=NULL, *pTPatch=NULL;
	long *pTieLut=NULL;
	LidPt_SurvInfo *pATNPoints=NULL;
//	double *SrcParam;
	POINT3D *ptSrc=NULL, *ptDst=NULL;
	double *hx=NULL, *hxx=NULL;
	double *pWeight=NULL;	//观测值权阵
	
	//统计连接面数量, tieID必须连续排列
	int tieID;
	int nTies;
	int nObs;
	int nHorVer, nHor, nVer;
	int nXDirection, nYDirection, nZDirection;
	double wXD, wYD, wZD;

// 	if(g_matrixService==NULL)
// 		g_matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	assert((param_type & LidAdj_GPS_drift) && (param_type & LidAdj_INS_drift));

	nHorVer = nHor = nVer = 0;
	tieID = -1;
	nTies = 0;
	for(i=0; i<nkeyPt; i++)
	{//统计连接点个数
		if(tieID!=pLidTies[i].tieID)
		{
			nTies++;
			tieID=pLidTies[i].tieID;
		}

		if(pLidTies[i].VP_type == TP_type_hor_ver && Use_Hor_Ver)
			nHorVer++;
		else if(pLidTies[i].VP_type == TP_type_horizon && Use_Hor)
			nHor++;
		else if(pLidTies[i].VP_type == TP_type_vertical && Use_Ver)
			nVer++;
	}

	nObs = nHorVer*3 + nHor*2 + nVer;  //统计方程数
	
	nXDirection = nHorVer + nHor;
	nYDirection = nHorVer + nHor;
	nZDirection = nHorVer + nVer;

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


	pATNPoints=new LidPt_SurvInfo[nkeyPt];
	for(i=0; i<nkeyPt; i++)
	{
		pATNPoints[i]=(*(pLidTies[i].pLidPoints))[0];

	}

	POINT3D *ptObs=NULL;
	ptObs=new POINT3D[nTies];	//连接点期望值 (取平均)
	pTieLut=new long[nTies+1];

	int nTieGcp=0;
	tieID=-1;
	for(i=0; i<ngcp; i++)
	{
		if(tieID!=pLidTies[i].tieID)
		{
			tieID=pLidTies[i].tieID;
			ptObs[tieID].X=pLidTies[i].eX;
			ptObs[tieID].Y=pLidTies[i].eY;
			ptObs[tieID].Z=pLidTies[i].eZ;

			nTieGcp++;
			pTieLut[tieID]=i;
		}
	}
	
	int nLinks=0;
	for(i=ngcp; i<nkeyPt; i++)
	{
		if(tieID!=pLidTies[i].tieID)
		{
			tieID=pLidTies[i].tieID;
			pTieLut[tieID]=i;

			ptObs[tieID].X=pLidTies[i].vX;
			ptObs[tieID].Y=pLidTies[i].vY;
			ptObs[tieID].Z=pLidTies[i].vZ;
		}
		else
		{
			ptObs[tieID].X+=pLidTies[i].vX;
			ptObs[tieID].Y+=pLidTies[i].vY;
			ptObs[tieID].Z+=pLidTies[i].vZ;
		}
	}
	pTieLut[nTies]=nkeyPt;

	for(i=nTieGcp; i<nTies; i++)
	{
		nLinks=pTieLut[i+1]-pTieLut[i];	//每个连接点对应的片数
		//连接点取平均值
		ptObs[i].X/=nLinks;
		ptObs[i].Y/=nLinks;
		ptObs[i].Z/=nLinks;

		for(j=pTieLut[i]; j<pTieLut[i+1]; j++)
		{
			pLidTies[j].eX=ptObs[i].X;
			pLidTies[j].eY=ptObs[i].Y;
			pLidTies[j].eZ=ptObs[i].Z;
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

	for(i=0, j=0; i<nkeyPt; i++)
	{
		if(pLidTies[i].VP_type == TP_type_hor_ver && Use_Hor_Ver)
		{
			L[j]=pLidTies[i].eX * wXD;	pWeight[j] = wXD;	j++;
			L[j]=pLidTies[i].eY * wYD;	pWeight[j] = wYD;	j++;
			L[j]=pLidTies[i].eZ * wZD;	pWeight[j] = wZD;	j++;
		}
		else if(pLidTies[i].VP_type == TP_type_horizon && Use_Hor)
		{
			L[j]=pLidTies[i].eX * wXD;	pWeight[j] = wXD;	j++;
			L[j]=pLidTies[i].eY * wYD;	pWeight[j] = wYD;	j++;
		}
		else if(pLidTies[i].VP_type == TP_type_vertical && Use_Ver)
		{
			L[j]=pLidTies[i].eZ * wZD;	pWeight[j] = wXD;	j++;
		}
	}


	//与激光点相关，一个虚拟点对应三个激光点
	ptSrc=new POINT3D[nkeyPt*3];	
	ptDst=new POINT3D[nkeyPt*3];

	double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
	opts[0]=LM_INIT_MU; opts[1]=1E-15; opts[2]=1E-15; opts[3]=1E-20; opts[4]= LM_DIFF_DELTA;

// 	ref_ptr<orsILidarGeoModel>  LidGeo;
// 	LidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_DEFAULT);

	struct LSA_globs data;
	data.pLidGeo = pLidGeo;

	data.nparams=nParams;
	data.nTies=nTies;
	data.nObs=nObs;
	data.nTieGCP=nTieGcp;
	data.nkeyPt=nkeyPt;
	data.pLidTies=pLidTies;
	data.pTieLut=pTieLut;
	data.pATNPoints=pATNPoints;
	data.L=L;
	
	data.ptSrc=ptSrc;
	data.ptDst=ptDst;
	data.hx=hx;
	data.hxx=hxx;
	data.pWeight = pWeight;

	data.param_type=param_type;
	data.calib_param = &oriParam;
	data.TiePt_mode  =	tie_mode;

	data.ref_time = refTime;
	data.nStrip = nStrip;

// 	double time0 = pATNPoints[0].POS_Info.time;
// 	data.ref_time = &time0;


//	CalibParam_Type calib_type = Calib_rph;
// 	_calLaserPosition(pATNPoints, nkeyPt*3, data.param, pX, param_type);
// 
// 	for(i=0; i<nkeyPt*3; i++)
// 	{//用当前参数计算新的激光点坐标
// 		ptSrc[i].X=pATNPoints[i].x;
// 		ptSrc[i].Y=pATNPoints[i].y;
// 		ptSrc[i].Z=pATNPoints[i].z;
// 	}

//#ifdef _DEBUG
	FILE *fp=NULL;
	fp=fopen(pszResultFile, "wt");
	if(fp==NULL)
	{
		assert(false);
		return -1;
	}

	fprintf(fp, "## strip adjustment: POS-supported\n");
	fprintf(fp, "Use_Hor_Ver: %d, Use_Hor: %d, Use_Ver: %d\n", Use_Hor_Ver, Use_Hor, Use_Ver);
	fprintf(fp, "## calib param type: %d\n", param_type);
	fprintf(fp, "## init calib param\n");
	for(i=0; i<nParams; i++)
	{
		fprintf(fp, "%.9lf ", pX[i]);
	}
	fprintf(fp, "\n\n");
	fprintf(fp, "tiepoints: %d; total points=%d horVer=%d hor=%d ver=%d\n", nTies, nkeyPt, nHorVer, nHor, nVer);

	//误差值
	double *pdx=new double[nkeyPt];
	double *pdy=new double[nkeyPt];
	double *pdz=new double[nkeyPt];
	double rms_x, rms_y, rms_z;
	
	cal_Obs_POSDrift(pX, hx, nParams, nObs, &data);
	rms_x=rms_y=rms_z=0;
	fprintf(fp, "before adjustment\n");
	nHorVer=0, nHor=0, nVer=0;
	bool bjump;
	for(i=0, j=0; i<nkeyPt;	i++)
	{
		bjump=true;
		if(pLidTies[i].VP_type == TP_type_hor_ver && Use_Hor_Ver)
		{
			pdx[i]=hx[j]-L[j];		j++;
			pdy[i]=hx[j]-L[j];		j++;
			pdz[i]=hx[j]-L[j];		j++;	

			nHorVer++;
			bjump=false;
		}
		else if(pLidTies[i].VP_type == TP_type_horizon && Use_Hor)
		{
			pdx[i]=hx[j]-L[j];		j++;
			pdy[i]=hx[j]-L[j];		j++;
			pdz[i]=0;	//平面点，高程误差赋0
			nHor++;
			bjump=false;
		}
		else if(pLidTies[i].VP_type == TP_type_vertical && Use_Ver)
		{//高程点，平面误差赋0
			pdx[i]=0;	
			pdy[i]=0;		
			pdz[i]=hx[j]-L[j];		j++;
			nVer++;
			bjump=false;
		}

		if(!bjump)
		{
			fprintf(fp, "%d %.3f %.3f %.3f %.3f %.3f %.3f\n", i, pLidTies[i].vX,pLidTies[i].vY, pLidTies[i].vZ, pdx[i], pdy[i], pdz[i]);

			rms_x+=pdx[i]*pdx[i];
			rms_y+=pdy[i]*pdy[i];
			rms_z+=pdz[i]*pdz[i];
		}
	}
	rms_x=sqrt(rms_x/(nHorVer+nHor));
	rms_y=sqrt(rms_y/(nHorVer+nHor));
	rms_z=sqrt(rms_z/(nHorVer+nVer));
	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);
//#endif
	//解方程L=AX

	int nUnknown = nParams;
	double *covar=NULL;
	
	covar = new double[nUnknown*nUnknown];

	int maxIter=50;
	int iter=0;
	int curObsNum = nObs;
	double rms;
	do{
		int ret=dlevmar_der(cal_tiePtCoord,	//计算L
			jac_POSDrift_diff, //计算A
			pX, L, nParams, nObs, 200, opts, info, NULL, covar, &data);

		iter++;

		rms = sqrt(info[1]/(curObsNum - nUnknown));
		double rms3 = rms*3;
		if(rms < 1.0)
			break;

		////////挑粗差//////////////
		cal_Obs_POSDrift(pX, hx, nParams, nObs, &data);
		
		for(i=0, j=0; i<nkeyPt;	i++)
		{
			if(pLidTies[i].VP_type == TP_type_hor_ver && Use_Hor_Ver)
			{
				pdx[i]=hx[j]-L[j];
				if(fabs(pdx[i]) > rms3)
				{
					pWeight[j] = 0;
					curObsNum--;
				}
				j++;
				pdy[i]=hx[j]-L[j];
				if(fabs(pdy[i]) > rms3)
				{
					pWeight[j] = 0;
					curObsNum--;
				}
				j++;
				pdz[i]=hx[j]-L[j];
				if(fabs(pdz[i]) > rms3)
				{
					pWeight[j] = 0;
					curObsNum--;
				}
				j++;	

			}
			else if(pLidTies[i].VP_type == TP_type_horizon && Use_Hor)
			{
				pdx[i]=hx[j]-L[j];
				if(fabs(pdx[i]) > rms3)
				{
					pWeight[j] = 0;
					curObsNum--;
				}
				j++;
				pdy[i]=hx[j]-L[j];
				if(fabs(pdy[i]) > rms3)
				{
					pWeight[j] = 0;
					curObsNum--;
				}
				j++;
				pdz[i]=0;	//平面点，高程误差赋0

			}
			else if(pLidTies[i].VP_type == TP_type_vertical && Use_Ver)
			{//高程点，平面误差赋0
				pdx[i]=0;	
				pdy[i]=0;		
				pdz[i]=hx[j]-L[j];
				if(fabs(pdz[i]) > rms3)
				{
					pWeight[j] = 0;
					curObsNum--;
				}
				j++;
			}
		}
	}while(iter<maxIter);

		
	double *corcoef=NULL;
	corcoef = new double[nUnknown*nUnknown];

	k=0;
	for(i=0;i<nUnknown; i++)
	{
		double Dxx = sqrt(covar[i*nUnknown+i]);
		for(j=0; j<nUnknown; j++)
		{
			double Dyy = sqrt(covar[j*nUnknown+j]);
			double Dxy = covar[i*nUnknown+j];

			corcoef[k] = Dxy/Dxx/Dyy;
			k++;
		}
	}

//	int ret=dlevmar_der(distance_PointToPlane, jac_dis_PointToPlane, X, L, pnp, nkeyPt, 1000, opts, info, NULL, NULL, &data);

//#ifdef _DEBUG
	cal_Obs_POSDrift(pX, hx, nParams, nObs, &data);
	rms_x=rms_y=rms_z=0;
	fprintf(fp, "after adjustment\n");
	for(i=0, j=0; i<nkeyPt;	i++)
	{
		bjump=true;
		if(pLidTies[i].VP_type == TP_type_hor_ver && Use_Hor_Ver)
		{
			pdx[i]=hx[j]-L[j];		j++;
			pdy[i]=hx[j]-L[j];		j++;
			pdz[i]=hx[j]-L[j];		j++;	
			bjump=false;
		}
		else if(pLidTies[i].VP_type == TP_type_horizon && Use_Hor)
		{
			pdx[i]=hx[j]-L[j];		j++;
			pdy[i]=hx[j]-L[j];		j++;
			pdz[i]=0;	//平面点，高程误差赋0
			bjump=false;
		}
		else if(pLidTies[i].VP_type == TP_type_vertical && Use_Ver)
		{//高程点，平面误差赋0
			pdx[i]=0;	
			pdy[i]=0;		
			pdz[i]=hx[j]-L[j];		j++;
			bjump=false;
		}

		if(!bjump)
		{
			fprintf(fp, "%d %.3f %.3f %.3f %.3f %.3f %.3f\n", i, pLidTies[i].vX,pLidTies[i].vY, pLidTies[i].vZ, pdx[i], pdy[i], pdz[i]);

			rms_x+=pdx[i]*pdx[i];
			rms_y+=pdy[i]*pdy[i];
			rms_z+=pdz[i]*pdz[i];
		}
	}
	rms_x=sqrt(rms_x/(nHorVer+nHor));
	rms_y=sqrt(rms_y/(nHorVer+nHor));
	rms_z=sqrt(rms_z/(nHorVer+nVer));

	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms_x, rms_y, rms_z);


	fprintf(fp, "## parameters estmation\n");
	for(i=0; i<nParams; i++)
	{
		fprintf(fp, "%.9lf ", pX[i]);
	}
	fprintf(fp, "\n\n");

	fprintf(fp, "####### RMS #######\n");
	fprintf(fp, "%.5lf\n", rms);

	fprintf(fp, "####### covariance of unknowns #######\n");
	fprintf(fp, "number of unknowns : %d\n", nUnknown);
	for(i=0; i<nUnknown; i++)
	{
		for(j=0; j<nUnknown; j++)
		{	
			k = i*nUnknown + j;
			fprintf(fp, " %e", covar[k]);
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "####### correlation coefficient of unknowns #######\n");
	k=0;
	for(i=0; i<nUnknown; i++)
	{
		for(j=0; j<nUnknown; j++)
		{
// 			if(i<pnp-1 && j>i)
// 			{
				fprintf(fp," %e", fabs(corcoef[k]));
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
	if(pTieLut)      delete pTieLut;		pTieLut=NULL;
	if(L)          delete L;				L=NULL;
//	if(X)			delete X;			X=NULL;
	if(pATNPoints) delete pATNPoints;	pATNPoints=NULL;
//	if(SrcParam) delete SrcParam;	SrcParam=NULL;
	if(ptSrc)     delete ptSrc;		ptSrc=NULL;
	if(ptDst)		delete ptDst;		ptDst=NULL;
	if(hx)			delete hx;			hx=NULL;
	if(hxx)		delete hxx;			hxx=NULL;
	if(pWeight) delete pWeight;		pWeight=NULL;

	if(covar)	delete covar;	covar=NULL;
	if(corcoef)	delete corcoef;	corcoef=NULL;
	return 1;
}