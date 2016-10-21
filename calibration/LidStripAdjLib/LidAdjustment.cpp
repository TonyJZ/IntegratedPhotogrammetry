#include "StdAfx.h"
#include "LidStripAdj/LidAdjustment.h"
//#include "LidarGeometry.h"
//#include "levmar\levmar.h"
#include "LidStripAdj/LidAdj_def.h"

#include "LidStripAdj/LSA_POSDrift.h"
#include "ATNPointIndex.h"
#include "GenPointIndex/KDTindex.h"

#include "orsImage/orsIImageService.h"
#include "orsImageGeometry/orsIImageGeometry.h"
#include "orsLidarGeometry/orsILidarGeoModel.h"
#include "orsMath/orsIMatrixService.h"


ORS_GET_MATRIX_SERVICE_IMPL();
ORS_GET_IMAGE_SERVICE_IMPL();

orsIPlatform *g_platform;
orsIPlatform* getPlatform()
{
	return g_platform;
}

//ref_ptr<orsILidarGeoModel> g_LidGeo;


int LidAdj_keyPt_ascending_tieID(const void *p1, const void  *p2)
{
	LidAdj_keyPt *elem1, *elem2;
	elem1=(LidAdj_keyPt *)p1;
	elem2=(LidAdj_keyPt *)p2;

	if(elem1->tieID < elem2->tieID)
		return -1;
	else 
		if(elem1->tieID > elem2->tieID)
			return 1;
		else
			return 0;
}

//按时间顺序排列
int ATNPoint_ascending_time(const void *p1, const void  *p2)
{
	ATNPoint *elem1, *elem2;
	elem1=(ATNPoint *)p1;
	elem2=(ATNPoint *)p2;

	if(elem1->gpstime < elem2->gpstime)
		return -1;
	else 
		if(elem1->gpstime> elem2->gpstime)
			return 1;
		else
			return 0;
}

//三角形插值
inline double TriangleInterpolate( std::vector<LidPt_SurvInfo> *pLidPoints, double x, double y )
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

inline double TriangleInterpolate_time( std::vector<LidPt_SurvInfo> *pLidPoints, 
	double x, double y )
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

//从ATNPoint结构中提取orsPOSParam信息
inline void getATNPointPOSParame(ATNPoint *pLidPoints, orsPOSParam *pos, int num)
{
	for(int i=0; i<num; i++)
	{
		pos[i].coord = pLidPoints[i].obsInfo.pos;
		pos[i].r = pLidPoints[i].obsInfo.roll;
		pos[i].p = pLidPoints[i].obsInfo.pitch;
		pos[i].h = pLidPoints[i].obsInfo.heading;
		pos[i].time = pLidPoints[i].gpstime;
	}
}

inline void getSurvPointPOSParame(LidPt_SurvInfo *pLidPoints, orsPOSParam *pos, int num)
{
	for(int i=0; i<num; i++)
	{
		pos[i] = pLidPoints[i].POS_Info;
	}
}

//将ATNPoint转成LidPt_SurvInfo
/*void ATN2SurvInfo(ATNPoint *pLidPoints, LidPt_SurvInfo *pSurvs, int Num)
{
	for(int i=0; i<Num; i++)
	{
		getATNPointPOSParame(pLidPoints+i, &(pSurvs[i].POS_Info), 1);

		pSurvs[i].x = pLidPoints[i].x;
		pSurvs[i].y = pLidPoints[i].y;
		pSurvs[i].z = pLidPoints[i].z;

	}

	g_LidGeo->cal_RangeVec(pSurvs, Num);
}*/

CLidAdjustment::CLidAdjustment(orsIPlatform *platform)
{
	g_platform = platform;
	m_pAlg = NULL;

	m_covar = NULL;
	m_rms = 0;
	m_corcoef = NULL;
}

CLidAdjustment::~CLidAdjustment()
{
	int i;

	//删除连接点列表
	for(i=0; i<m_TieList.GetSize(); i++)
	{
		TieObjChain *pObjChain;
		pObjChain=m_TieList[i];
		delete pObjChain;
		pObjChain=NULL;
	}
	m_TieList.RemoveAll();

	if(m_covar)
		delete m_covar;
	m_covar = NULL;

	if(m_corcoef)
		delete m_corcoef;
	m_corcoef = NULL;
}

void CLidAdjustment::setAlignPrj(CAlignPrj *pAlg)
{
	m_pAlg = pAlg;
}

bool CLidAdjustment::loadTiePoints(const char* pszFileName, int minOverlap)
{
	FILE *fp=NULL;
	char pLine[1024];
	int MAX_STRING=1024;
	int gcpNum, conNum;
	int i, j;
//	double xg,yg,zg;
	int  imgID;
//	double xp, yp, zp;

	//	int nprojs;
// 	int n3Dpts;
// 	int projN;
	int imgNum;
	//	int *pFrameProjs=NULL;

	fp = fopen(pszFileName, "rt");
	if(fp==NULL)
		return false;

	m_TieList.RemoveAll();
	m_nprojs=0;

	
	ors_int64 ftID;
	double xi, yi, zi, xs, ys, zs;
	int tieID = 1;
	int connects;
	fgets(pLine, MAX_STRING, fp);
	if(!strstr(pLine, "[tracks]"))
	{
		printf("It is not tracks file!");
		return false;
	}

	while(!feof(fp)) 
	{
		fscanf(fp, "%d\n", &conNum);

		fgets(pLine, MAX_STRING, fp);
		if(strstr(pLine, "[imageNum]"))
		{
			fscanf( fp, "%d\n", &imgNum );
		}

		for(i=0; i<conNum; i++)
		{
			fscanf(fp, "%I64d%d ", &ftID, &connects);
			//			fscanf(fp, "%d ", &connects);
			TieObjChain *pCurChain=NULL;

			if(connects >= minOverlap)
			{
				pCurChain=new TieObjChain;
				pCurChain->TieID=tieID;
				pCurChain->type=TO_POINT;
				pCurChain->bDel=false;
				pCurChain->pChain=new CArray<TieObject*, TieObject*>; //无连接点
				pCurChain->objNum=connects;
			}	

			for(j=0; j<connects; j++)
			{
				fscanf(fp, "%d %lf %lf %lf ", &imgID, &xi, &yi, &zi);

				if(connects < minOverlap)
					continue;

				TiePoint	*pTmpObj=new TiePoint;

				pTmpObj->tieID=pCurChain->TieID;
				pTmpObj->objType=pCurChain->type;
				pTmpObj->sourceID=imgID;
				pTmpObj->pt2D.x=xi;
				pTmpObj->pt2D.y=yi;

				int stripID = getStripID(imgID);
				if(stripID>0)
					pTmpObj->sourceType = ST_Image;
				else
					pTmpObj->sourceType = ST_LiDAR;

				pCurChain->pChain->Add(pTmpObj);
			}
			fscanf(fp, "\n");

			if(pCurChain)
			{
				m_TieList.Add(pCurChain);
				tieID++;
			}
		}
	};

	fclose(fp);

	return true;
}

int CLidAdjustment::Lidar_StripAdjustment(char *pszSAResultFile, orsLidSA_TYPE SA_type)
{
	int ret = 0;
	
	ref_ptr<orsILidarGeoModel> g_LidGeo;
	if(g_LidGeo.get()==0)
		g_LidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);

	ref_ptr<orsITrajectory> traj;
	traj = ORS_CREATE_OBJECT(orsITrajectory, ORS_LIDARGEOMETRY_TRAJECTORY_DEFAULT);

	orsIMatrixService *matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	ref_ptr<orsIImageSourceReader> ImgReader;

	orsIImageGeometry *pImgGeo=NULL;

//	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
//	CATNPtIndex	index(getPlatform());
	CKDTPtIndex index(getPlatform());
	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidAdj_keyPt *pLidTies=NULL;
	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch, nTies, nkeyPt, ikey;
	//	double *pZObs=NULL;
	long *pTieVisibleLut=NULL;	//连接点的查找表矩阵
	int nStrip;
	double *pRefTimeList=NULL;   //时间参考数组
	orsPOINT3D pt3D, *ptBuf;
	int ngcp;
//	ATNTP_Point *pATNPoints=NULL;


//	pTieList=m_pAlg->GetTieObjList();
	pLidLines=m_pAlg->GetLidList();

	nStrip=pLidLines->GetSize();
	assert(nStrip>0);
	pRefTimeList = new double[nStrip];

	nTies=m_TieList.GetSize();

	//	nTies=20;	//测试用

	pTieVisibleLut=new long[nTies*nStrip];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nStrip; i++)
	{//不存在连接点时为1
		pTieVisibleLut[i]=-1;
	}

	std::vector<LidPt_SurvInfo> ATN_pts;
	LidPt_SurvInfo  tempSurInfo;
	nPatch=0;
	nkeyPt=0;
	ngcp=0;
	for(i=0; i<nTies; i++)
	{//统计连接点数
		pTieChain=m_TieList.GetAt(i);  //遍历每一个点链

		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());

		if(pTieChain->type==TO_PATCH)
		{
			nPatch+=pTieChain->pChain->GetSize();	
			//			continue;
		}

		if(pTieChain->type==TO_POINT)
			nkeyPt+=pTieChain->pChain->GetSize();

		if(pTieChain->bGCP) 
			ngcp+=pTieChain->pChain->GetSize();


		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieVisibleLut[i*nStrip+srcID]=j;
			}
		}
	}

	if(nkeyPt>0)
		pLidTies=new LidAdj_keyPt[nkeyPt];

	if(nPatch>0)
		pLidPlanes=new LidMC_Plane[nPatch];
	//	pATNPoints=new ATNTP_Point[nPatch*3];

	//	pZObs=new double[nPatch];
	int ptNum;
	locateresult   result;
	int tieID=0;
	iPatch=0; ikey=0;
	double radius=2.0;
	double rmse_th=0.2;
	bool bFindRefT=false;
	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nStrip; i++)
	{
		bFindRefT = false;  //为当前条带添加时间参考

		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		ImgReader=imageService->openImageFile(lidLine.GridName.GetBuffer(0));
		if (!ImgReader.get())
			continue;

		pImgGeo=ImgReader->GetImageGeometry();

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点
			k=pTieVisibleLut[j*nStrip+i];
			if(k<0)  //判断当前连接点是否存在于指定条带上
				continue;

			pTieChain=m_TieList.GetAt(j);

			pObj=pTieChain->pChain->GetAt(k);

			if(pTieChain->type==TO_PATCH)
			{
				ptNum=((TiePatch*)pObj)->ptNum;
				ptBuf=((TiePatch*)pObj)->pt3D;

				pLidPlanes[iPatch].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidPlanes[iPatch].tieID=pTieChain->TieID-1;
				tieID++;

				index.GetPoints(ptBuf, ptNum, pLidPlanes[iPatch].pLidPoints);

				iPatch++;
			}
			else if(pTieChain->type==TO_POINT)
			{
				char tptype =  ((TiePoint*)pObj)->tpType;

				//影像坐标转3D坐标
				double xi, yi, xs, ys;
				if(pObj->sourceType == ST_LiDAR)
				{
					xi = ((TiePoint*)pObj)->pt2D.x;
					yi = ((TiePoint*)pObj)->pt2D.y;
					pImgGeo->IntersectWithZ( xi, yi, 0, &xs, &ys ) ;

					((TiePoint*)pObj)->pt3D.X = xs;
					((TiePoint*)pObj)->pt3D.Y = ys;
				}

				pt3D=((TiePoint*)pObj)->pt3D;

				pLidTies[ikey].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidTies[ikey].tieID=pTieChain->TieID-1;
				
				tieID++;

				double rmse;
				result=index.GetTriangleVertex(&pt3D, radius, &ATN_pts, &rmse); 
				//ASSERT(result!=OUTSIDE);

// 				index.GetPoints(pt3D.X, pt3D.Y, radius, &ATN_pts);
// 				index.GetPoints(&pt3D, 3, &ATN_pts);
// 				double NN_dis;
// 				index.GetNNPoint(&pt3D, radius, &ATN_pts, &NN_dis);
// 				LidPt_SurvInfo  ptt;
// 				index.QueryNNPoint(pt3D.X, pt3D.Y, &ptt);
// 				int kkk = index.get_point_contentMask();

				if(result==OUTSIDE)
				{//扫描漏洞，剔除

					continue;
				}

				if(!bFindRefT)
				{//当前条带的时间参考
					pRefTimeList[i] = ATN_pts[0].time;
					bFindRefT=true;
				}

				pLidTies[ikey].pLidPoints->assign(3, tempSurInfo);
//				ATN2SurvInfo(&ATN_pts[0], &(*(pLidTies[iTie].pLidPoints))[0], 3);

				pLidTies[ikey].vX=pt3D.X;
				pLidTies[ikey].vY=pt3D.Y;
				pLidTies[ikey].vZ=TriangleInterpolate(&ATN_pts, pt3D.X, pt3D.Y);

				/////////////////////////////////////////////////////
				//用时间内插出观测值
				orsPOSParam pos_t, pos0, pos1;

				pos_t.time = TriangleInterpolate_time(&ATN_pts, pt3D.X, pt3D.Y);
				qsort(&ATN_pts[0], 3, sizeof(ATNPoint), ATNPoint_ascending_time);

				assert(pos_t.time>ATN_pts[0].time
					&& pos_t.time<ATN_pts[2].time);


				if(pos_t.time < ATN_pts[1].time)
				{
					getSurvPointPOSParame(&ATN_pts[0], &pos0, 1);
					getSurvPointPOSParame(&ATN_pts[1], &pos1, 1);
				}
				else
				{
// 					pos0 = (*(pLidVP[iVP].pLidPoints))[1].POS_Info;
// 					pos1 = (*(pLidVP[iVP].pLidPoints))[2].POS_Info;
					getSurvPointPOSParame(&ATN_pts[1], &pos0, 1);
					getSurvPointPOSParame(&ATN_pts[2], &pos1, 1);
				}

				matrixService->RotateMatrix_rph(pos0.r, pos0.p, pos0.h, pos0.R);
				matrixService->RotateMatrix_rph(pos1.r, pos1.p, pos1.h, pos1.R);
				traj->Interpolate_linear(&pos_t, &pos0, &pos1);

				matrixService->R2rph(pos_t.R, &pos_t.r, &pos_t.p, &pos_t.h);

				(*(pLidTies[ikey].pLidPoints))[0].x=pLidTies[ikey].vX;
				(*(pLidTies[ikey].pLidPoints))[0].y=pLidTies[ikey].vY;
				(*(pLidTies[ikey].pLidPoints))[0].z=pLidTies[ikey].vZ;
				(*(pLidTies[ikey].pLidPoints))[0].POS_Info = pos_t;
				
				g_LidGeo->cal_RangeVec(&(*(pLidTies[ikey].pLidPoints))[0], 1);

				(*(pLidTies[ikey].pLidPoints))[0].POS_Info.stripID = i;	//激光点条带号

				if(!tptype)
				{//未定义连接点类型，通过粗糙度计算
					if(rmse<rmse_th)
						pLidTies[ikey].VP_type=TP_type_hor_ver;
					else
						pLidTies[ikey].VP_type=TP_type_horizon;
				}
				else
				{//直接使用预先定义的连接点类型
					pLidTies[ikey].VP_type=tptype;
				}
				ikey++;
			}
		}

	}

	if(nkeyPt>ikey)	nkeyPt=ikey;

	//对pLidTies按连接点ID排序
	qsort(pLidTies, nkeyPt, sizeof(LidAdj_keyPt), LidAdj_keyPt_ascending_tieID);

	//对连接点进行检查
	//删除重叠度小于2的点，并重新对特征点顺序编号
	int TieHorNum=0, TieHorVerNum=0, TieVerNum=0;
	int tiePtNum=0;
	tieID = -1;
	int overlap;
	int newID=0;	//顺序编号
	ikey=0;
	for(i=0; i<nkeyPt; )
	{
		int sPos, ePos;
		char vp_type;
		bool bUpdate=false;

		if(tieID!=pLidTies[i].tieID)
		{//连接点链中的第一个特征点
			tieID=pLidTies[i].tieID;
			vp_type=pLidTies[i].VP_type;
			overlap=1;

			tiePtNum++;

			sPos=i;
			ePos=i;

			ikey++;
		}
		

		i++;
		while(pLidTies[i].tieID==tieID)
		{
			if(pLidTies[i].VP_type!=vp_type)
			{
				bUpdate=true;
			}
			ePos=i;

			overlap++;
			i++;
			ikey++;
		}

		if(overlap<2)
		{
			tiePtNum--; //删除该连接点链
			for(j=sPos; j<=ePos; j++)
			{
				pLidTies[j].tieID=nTies+1;//放到末尾
				ikey--;
			}
			continue;
		}

		if(bUpdate || newID!=tieID)
		{
			vp_type=TP_type_horizon;
			for(j=sPos; j<=ePos; j++)
			{
				pLidTies[j].VP_type=vp_type;
				pLidTies[j].tieID = newID;
			}
		}

		newID++;

		if(vp_type & TP_type_hor_ver)	TieHorVerNum++;
		else if(vp_type & TP_type_horizon)	TieHorNum++;
		else if(vp_type & TP_type_vertical)	TieVerNum++;

	}

	qsort(pLidTies, nkeyPt, sizeof(LidAdj_keyPt), LidAdj_keyPt_ascending_tieID);
	nTies = tiePtNum;
	nkeyPt = ikey;

	TRACE("total tie point num: %d\n", tiePtNum);
	TRACE("horizon and vertical tie point num: %d\n", TieHorVerNum);
	TRACE("horizon tie point num: %d\n", TieHorNum);
	TRACE("vertical tie point num: %d\n", TieVerNum);

	//为控制点赋值
	for(i=0; i<ngcp; i++)
	{
		j=pLidTies[i].tieID;

		//		if(i<ngcp)
		//		{//控制点必须排在最开始
		pTieChain=m_TieList.GetAt(j);
		ASSERT(pTieChain->bGCP);

		pLidTies[i].eX=pTieChain->gcp.X;
		pLidTies[i].eY=pTieChain->gcp.Y;
		pLidTies[i].eZ=pTieChain->gcp.Z;
		//		}
	}

	//调整连接点ID,连续排列 (漏洞点删除后占用的ID)
	for(i=0, j=0; i<nTies; )
	{
		tieID=pLidTies[i].tieID;
		if(tieID!=j)
		{	
			pLidTies[i].tieID=j;	
		}

		i++;
		while(pLidTies[i].tieID==tieID)
		{
			if(tieID!=j)
			{	
				pLidTies[i].tieID=j;	
			}
			i++;
		}
		j++;
	}

	orsLidSysParam oriParam;
	memset(&oriParam, 0, sizeof(orsLidSysParam));

	double *pX=NULL;	//线元素，角元素
	int nParams=0;

	//平差的未知数类型
	unsigned long param_type = LidAdj_boreAngle|LidAdj_leverArm|LidAdj_GPS_drift|LidAdj_INS_drift;


	switch(SA_type)
	{
	case LidSA_POSDrift:
		nParams = 3 + 3 + 6*nStrip + 6*nStrip;
		pX = new double[nParams];
		memset(pX, 0,sizeof(double)*nParams);
		LSA_POSDrift(pszSAResultFile, nkeyPt, ngcp, pLidTies, pX, nParams, param_type,
			oriParam, pRefTimeList, nStrip, g_LidGeo.get());
		break;
	case LidSA_3DSimilarityTrans:
		break;
	case LidSA_OriAnchor:
		break;
	}


FUNC_END:
	if(pLidTies)	delete[] pLidTies;	pLidTies=NULL;
	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pTieVisibleLut)  delete pTieVisibleLut;  pTieVisibleLut=NULL;
	//	if(pATNPoints)	delete pATNPoints;	pATNPoints=NULL;
	if(pRefTimeList)	delete pRefTimeList;	pRefTimeList=NULL;
	if(pX)			delete pX;			pX = NULL;
	return ret;
}
