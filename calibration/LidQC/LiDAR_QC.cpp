#include "StdAfx.h"
#include "LiDAR_QC.h"
#include "ATNPointIndex.h"
#include "LidCalib/PlaneConstraint_Model.h"
#include "LidCalib/VP_Model.h"

orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

void FastDspSetPlatform(orsIPlatform *pPlatform)
{
	g_pPlatform = pPlatform;
}


int LidMC_Plane_ascending_tieID(const void *p1, const void  *p2)
{
	LidMC_Plane *elem1, *elem2;
	elem1=(LidMC_Plane *)p1;
	elem2=(LidMC_Plane *)p2;

	if(elem1->tieID < elem2->tieID)
		return -1;
	else 
		if(elem1->tieID > elem2->tieID)
			return 1;
		else
			return 0;
}

int LidMC_VP_ascending_tieID(const void *p1, const void  *p2)
{
	LidMC_VP *elem1, *elem2;
	elem1=(LidMC_VP *)p1;
	elem2=(LidMC_VP *)p2;

	if(elem1->tieID < elem2->tieID)
		return -1;
	else 
		if(elem1->tieID > elem2->tieID)
			return 1;
		else
			return 0;
}

#include "orsBase/orsString.h"
//#include "ATNPointIndex.h"
void LidQC_Plane(orsLidSysParam param, CAlignPrj *algPrj, const char *pszFileName)
{
//	CAlignPrj   algPrj;
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
	CATNPtIndex	index(getPlatform());
	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch;
	//	double x0, y0;
	POINT3D *pt3D;
	int ptNum;
	int nLine, nTies;
	long *pTieLut=NULL;	//连接点的查找表矩阵


	pTieList=algPrj->GetTieObjList();
	pLidLines=algPrj->GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见

	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为-1
		pTieLut[i]=-1;
	}

	nPatch=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pChain=pTieList->GetAt(i);  //遍历每一个点链

		//		if(pChain->TieID==6) continue;
		ASSERT(pChain->objNum==pChain->pChain->GetSize());
		nPatch+=pChain->pChain->GetSize();	

		for(j=0; j<pChain->objNum; j++)
		{
			pObj=pChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	//将所有对应平面的点混到一起
	pLidPlanes=new LidMC_Plane[nTies];
	iPatch=0;
	int tieID=0;
	
	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点
			k=pTieLut[j*nLine+i];
			if(k<0)
				continue;

			pChain=pTieList->GetAt(j);
			//			if(pChain->TieID==6) continue;
			pObj=pChain->pChain->GetAt(k);

			ptNum=((TiePatch*)pObj)->ptNum;
			pt3D=((TiePatch*)pObj)->pt3D;

			if(pLidPlanes[j].pLidPoints==NULL)
				pLidPlanes[j].pLidPoints=new std::vector<LidPt_SurvInfo>;

			pLidPlanes[j].tieID=pChain->TieID-1;
			tieID++;

			index.GetPoints(pt3D, ptNum, pLidPlanes[j].pLidPoints);
		}
	}

	
	//对pLidPlanes按连接点ID排序
	qsort(pLidPlanes, nTies, sizeof(LidMC_Plane), LidMC_Plane_ascending_tieID);

	//ransac筛选出共面点
//	LidPlane_ransac(pLidPlanes, nPatch);

	double *rms = new double[nTies]; 
	double *pPlaneLut=new double[nTies+1];

	int nLidPoints=0;
	for(i=0, j=-1; i<nTies; i++)
	{
		if(j==pLidPlanes[i].tieID)
		{
			pPlaneLut[j+1] += pLidPlanes[i].pLidPoints->size();
			nLidPoints+=pLidPlanes[i].pLidPoints->size();
		}
		else
		{
			j = pLidPlanes[i].tieID;
			
			pPlaneLut[j+1] = pLidPlanes[i].pLidPoints->size()+nLidPoints;
			nLidPoints += pLidPlanes[i].pLidPoints->size();
		}
	}
	pPlaneLut[0]=0;

	double *dis = new double[nLidPoints];


	CalibParam_Type type = Calib_rph;
	double pX[3];
	int nCalib = 3;
	pX[0]=param.boresight_angle[0];
	pX[1]=param.boresight_angle[1];
	pX[2]=param.boresight_angle[2];

	nPatch = nTies;
	lidPlaneFitting(pLidPlanes, nPatch, param, pX, type);

	LidStatDis_PointToPlane(pLidPlanes, nPatch, pX, false, param, type, rms, nTies, dis);

	FILE *fp = fopen(pszFileName, "wt");


	fprintf(fp, "## calib param\n");
	fprintf(fp, "lever arm: %.6f %.6f %.6f\n", param.lever_arm[0], param.lever_arm[1], param.lever_arm[2]);
	fprintf(fp, "misalign angle: %.6f %.6f %.6f\n", param.boresight_angle[0], param.boresight_angle[1], param.boresight_angle[2]);
	fprintf(fp, "sa0: %.6f\n", param.sa0);
	fprintf(fp, "sac: %.6f\n", param.sac);
	fprintf(fp, "drho: %.6f\n", param.drange);
	fprintf(fp, "semiXYZ: %.6f %.6f %.6f\n", param.semiXYZ[0], param.semiXYZ[1], param.semiXYZ[2]);
	fprintf(fp, "###############################\n");

	for(i=0; i<nTies; i++)
	{//遍历平面
		int ptNum=pLidPlanes[i].pLidPoints->size();
		fprintf(fp, "planeID: %d point num: %d rms: %.6f s1s2s3s4: %5f %5f %5f %5f\n", i, ptNum, rms[i], pLidPlanes[i].s1, pLidPlanes[i].s2,
			pLidPlanes[i].s3, pLidPlanes[i].s4);
	}
		
	orsString strPath = orsString::getDirFromPath(pszFileName);

	char tmp[128];
	for(i=0; i<nTies; i++)
	{//遍历平面
		
		orsString name = strPath + "\\";
		
		sprintf(tmp, "plane_%d.txt", i);
		name += tmp;

		FILE *fout = fopen(name, "wt");

		int ptNum=pLidPlanes[i].pLidPoints->size();
		fprintf(fout, "planeID %d, point num:%d, rms:%.6f\n", i, ptNum, rms[i]);
		for(j=pPlaneLut[i]; j<pPlaneLut[i+1]; j++)
		{//遍历平面内的点
			fprintf(fout, "%5f\n", dis[j]);
		}

		fclose(fout);	
	}

	if(fp)	fclose(fp);	fp=NULL;

	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pTieLut)		delete[] pTieLut;		pTieLut=NULL;	
	if(rms)			delete[] rms;			rms = NULL;
	if(pPlaneLut)	delete[] pPlaneLut;		pPlaneLut=NULL;
	if(dis)			delete[] dis;			dis=NULL;
}


void LidQC_VP(oriCalibParam param, CalibParam_Type ctype, CAlignPrj *algPrj, const char *pszFileName)
{
//	CAlignPrj   algPrj;
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
	CATNPtIndex	index(getPlatform());
	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidMC_VP *pLidVP=NULL;
	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch, nVP, iVP;
	//	double *pZObs=NULL;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	int nLine, nTies;
	POINT3D pt3D, *ptBuf;
	int ngcp;

	double *pWeight=NULL;	//观测值权阵

	//	ATNTP_Point *pATNPoints=NULL;

	

	pTieList=algPrj->GetTieObjList();
	pLidLines=algPrj->GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	//	nTies=20;	//测试用

	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为1
		pTieLut[i]=-1;
	}

	nPatch=0;
	nVP=0;
	ngcp=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pTieChain=pTieList->GetAt(i);  //遍历每一个点链

		//  		TRACE("%d\n", pTieChain->TieID);
		//  		TRACE("%d\n", pTieChain->objNum);
		//  		TRACE("%d\n", pTieChain->type);
		//  		TRACE("%x\n", pTieChain->pChain);
		// 		pTieChain->pChain->GetSize();
		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());

		if(pTieChain->type==TO_PATCH)
			nPatch+=pTieChain->pChain->GetSize();	

		if(pTieChain->type==TO_POINT)
			nVP+=pTieChain->pChain->GetSize();

		// 		if(pChain->type==TO_POINT)
		// 			nVP+=pChain->pChain->GetSize();

		if(pTieChain->bGCP) 
			ngcp+=pTieChain->pChain->GetSize();


		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	if(nVP>0)
		pLidVP=new LidMC_VP[nVP];

	if(nPatch>0)
		pLidPlanes=new LidMC_Plane[nPatch];
	//	pATNPoints=new ATNTP_Point[nPatch*3];

	//	pZObs=new double[nPatch];
	int ptNum;
	locateresult   result;
	int tieID=0;
	iPatch=0; iVP=0;
	double radius=2.0;
	double rmse_th=0.1;
	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点

			// 			if(j==521)
			// 				printf("");

			k=pTieLut[j*nLine+i];
			if(k<0)  //判断当前连接点是否存在于指定条带上
				continue;

			pTieChain=pTieList->GetAt(j);
			//			if(pChain->TieID==6) continue;
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
				pt3D=((TiePoint*)pObj)->pt3D;

				pLidVP[iVP].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidVP[iVP].tieID=pTieChain->TieID-1;
				//				pLidVP[iVP].eX=pChain->gcp.X;
				//				pLidVP[iVP].eY=pChain->gcp.Y;
				//				pLidVP[iVP].eZ=pChain->gcp.Z;
				tieID++;

				double rmse;
				result=index.GetTriangleVertex(&pt3D, radius, pLidVP[iVP].pLidPoints, &rmse); 
				//				ASSERT(result!=OUTSIDE);

				if(result==OUTSIDE)
				{//扫描漏洞，剔除
					// 					pLidVP[iVP].VP_type=TP_type_horizon;
					// 					pLidVP[iVP].vx=pt3D.X;
					// 					pLidVP[iVP].vy=pt3D.Y;
					// 					pLidVP[iVP].reliability=0;
					// 
					// 					iVP++;
					continue;
				}

				pLidVP[iVP].vx=pt3D.X;
				pLidVP[iVP].vy=pt3D.Y;
				pLidVP[iVP].vz=TriangleInterpolate(pLidVP[iVP].pLidPoints, pt3D.X, pt3D.Y);
//				if(rmse<rmse_th)
					pLidVP[iVP].VP_type=TP_type_hor_ver;
// 				else
// 					pLidVP[iVP].VP_type=TP_type_horizon;
				iVP++;
			}
		}
	}

	if(nVP>iVP)	nVP=iVP;

	//对pLidVP按连接点ID排序
	qsort(pLidVP, nVP, sizeof(LidMC_VP), LidMC_VP_ascending_tieID);

	//对连接点进行检查
	int TieHorNum=0, TieHorVerNum=0, TieVerNum=0;
	int tiePtNum=0;
	for(i=0; i<nVP; )
	{
		int sPos, ePos;
		tieID=pLidVP[i].tieID;
		char vp_type=pLidVP[i].VP_type;
		bool bUpdate=false;

		tiePtNum++;
		sPos=i;
		ePos=i;

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(pLidVP[i].VP_type!=vp_type)
			{
				bUpdate=true;
			}
			ePos=i;

			i++;
		}

		if(bUpdate)
		{
			vp_type=TP_type_horizon;
			for(j=sPos; j<=ePos; j++)
			{
				pLidVP[j].VP_type=vp_type;
			}
		}

		if(vp_type & TP_type_hor_ver)	TieHorVerNum++;
		else if(vp_type & TP_type_horizon)	TieHorNum++;
		else if(vp_type & TP_type_vertical)	TieVerNum++;

	}

	//为控制点赋值
	for(i=0; i<ngcp; i++)
	{
		j=pLidVP[i].tieID;

		//		if(i<ngcp)
		//		{//控制点必须排在最开始
		pTieChain=pTieList->GetAt(j);
		ASSERT(pTieChain->bGCP);

		pLidVP[i].eX=pTieChain->gcp.X;
		pLidVP[i].eY=pTieChain->gcp.Y;
		pLidVP[i].eZ=pTieChain->gcp.Z;
		//		}
	}

	//调整连接点ID,连续排列 (漏洞点删除后占用的ID)
	for(i=0, j=0; i<nVP; )
	{
		tieID=pLidVP[i].tieID;
		if(tieID!=j)
		{	
			pLidVP[i].tieID=j;	
		}

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(tieID!=j)
			{	
				pLidVP[i].tieID=j;	
			}
			i++;
		}
		j++;
	}

// 	oriCalibParam oriParam;
// 	memset(&oriParam, 0, sizeof(oriCalibParam));

	double pX[3];	//线元素，角元素
//	memset(pX, 0, sizeof(double)*8);

 	CalibParam_Type ctype;
 	ctype=Calib_rph;

	pX[0]=param.misalign_angle[0];	
	pX[1]=param.misalign_angle[1];	
	pX[2]=param.misalign_angle[2];

	double *pdx = new double[nVP];
	double *pdy = new double[nVP];
	double *pdz = new double[nVP];
	double rms[3];

	LidStatDis_VP(pLidVP, nVP, ngcp, pX, param, ctype, pdx, pdy, pdz, rms);

	FILE *fp=NULL;
	fp=fopen(pszFileName, "wt");
// 	if(fp==NULL)
// 	{
// 		assert(false);
// 		return -1;
// 	}

	fprintf(fp, "## calib param\n");
	fprintf(fp, "lever arm: %.6f %.6f %.6f\n", param.lever_arm[0], param.lever_arm[1], param.lever_arm[2]);
	fprintf(fp, "misalign angle: %.6f %.6f %.6f\n", param.misalign_angle[0], param.misalign_angle[1], param.misalign_angle[2]);
	fprintf(fp, "sa0: %.6f\n", param.sa0);
	fprintf(fp, "sac: %.6f\n", param.sac);
	fprintf(fp, "semiXYZ: %.6f %.6f %.6f\n", param.semiXYZ[0], param.semiXYZ[1], param.semiXYZ[2]);

	fprintf(fp, "nVP=%d horVer=%d hor=%d ver=%d\n", nVP, TieHorVerNum, TieHorNum, TieVerNum);


	fprintf(fp, "VP residual error\n");


	for(i=0, j=0; i<nVP;	i++)
	{
		
		fprintf(fp, "%d %.3f %.3f %.3f\n", i, pdx[i], pdy[i], pdz[i]);

	}
	fprintf(fp, "rms_x=%f rms_y=%f rms_z=%f\n", rms[0], rms[1], rms[2]);

	if(fp)	fclose(fp);	fp=NULL;
	if(pdx)		delete pdx;			pdx=NULL;
	if(pdy)		delete pdy;			pdy=NULL;
	if(pdz)		delete pdz;			pdz=NULL;

	if(pLidVP)	delete[] pLidVP;	pLidVP=NULL;
	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pTieLut)  delete pTieLut;  pTieLut=NULL;
	//	if(pATNPoints)	delete pATNPoints;	pATNPoints=NULL;
}

int Stat_Plane_HONV( CAlignPrj *algPrj, HONV **pHONV, int &nPlane )
{
	double a, b, c; //normal vector

	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
	CATNPtIndex	index(getPlatform());
	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidMC_VP *pLidVP=NULL;
	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch, nVP, iVP;
	//	double *pZObs=NULL;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	int nLine, nTies;
	POINT3D pt3D, *ptBuf;
	int ngcp;
	//	ATNTP_Point *pATNPoints=NULL;

	pTieList=algPrj->GetTieObjList();
	pLidLines=algPrj->GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	//	nTies=20;	//测试用

	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为1
		pTieLut[i]=-1;
	}

	nPatch=0;
	nVP=0;
	ngcp=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pTieChain=pTieList->GetAt(i);  //遍历每一个点链

		//  		TRACE("%d\n", pTieChain->TieID);
		//  		TRACE("%d\n", pTieChain->objNum);
		//  		TRACE("%d\n", pTieChain->type);
		//  		TRACE("%x\n", pTieChain->pChain);
		// 		pTieChain->pChain->GetSize();
		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());

		if(pTieChain->type==TO_PATCH)
			nPatch+=pTieChain->pChain->GetSize();	

		if(pTieChain->type==TO_POINT)
			nVP+=pTieChain->pChain->GetSize();

		// 		if(pChain->type==TO_POINT)
		// 			nVP+=pChain->pChain->GetSize();

		if(pTieChain->bGCP) 
			ngcp+=pTieChain->pChain->GetSize();


		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	if(nVP>0)
		pLidVP=new LidMC_VP[nVP];

	if(nPatch>0)
		pLidPlanes=new LidMC_Plane[nPatch];
	//	pATNPoints=new ATNTP_Point[nPatch*3];

	//	pZObs=new double[nPatch];
	int ptNum;
	locateresult   result;
	int tieID=0;
	iPatch=0; iVP=0;
	double radius=2.0;
	double rmse_th=0.1;
	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点

			// 			if(j==521)
			// 				printf("");

			k=pTieLut[j*nLine+i];
			if(k<0)  //判断当前连接点是否存在于指定条带上
				continue;

			pTieChain=pTieList->GetAt(j);
			//			if(pChain->TieID==6) continue;
			pObj=pTieChain->pChain->GetAt(k);

			if(pTieChain->type==TO_PATCH)
			{
				ptNum=((TiePatch*)pObj)->ptNum;
				ptBuf=((TiePatch*)pObj)->pt3D;

				pLidPlanes[iPatch].pLidPoints=new std::vector<ATNPoint>;
				pLidPlanes[iPatch].tieID=pTieChain->TieID-1;
				tieID++;

				index.GetPoints(ptBuf, ptNum, pLidPlanes[iPatch].pLidPoints);

				iPatch++;
			}
			else if(pTieChain->type==TO_POINT)
			{
				pt3D=((TiePoint*)pObj)->pt3D;

				pLidVP[iVP].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidVP[iVP].tieID=pTieChain->TieID-1;
				//				pLidVP[iVP].eX=pChain->gcp.X;
				//				pLidVP[iVP].eY=pChain->gcp.Y;
				//				pLidVP[iVP].eZ=pChain->gcp.Z;
				tieID++;

				double rmse;
				result=index.GetTriangleVertex(&pt3D, radius, pLidVP[iVP].pLidPoints, &rmse); 
				//				ASSERT(result!=OUTSIDE);

				if(result==OUTSIDE)
				{//扫描漏洞，剔除
					// 					pLidVP[iVP].VP_type=TP_type_horizon;
					// 					pLidVP[iVP].vx=pt3D.X;
					// 					pLidVP[iVP].vy=pt3D.Y;
					// 					pLidVP[iVP].reliability=0;
					// 
					// 					iVP++;
					continue;
				}

				pLidVP[iVP].vx=pt3D.X;
				pLidVP[iVP].vy=pt3D.Y;
				pLidVP[iVP].vz=TriangleInterpolate(pLidVP[iVP].pLidPoints, pt3D.X, pt3D.Y);
				if(rmse<rmse_th)
					pLidVP[iVP].VP_type=TP_type_hor_ver;
				else
					pLidVP[iVP].VP_type=TP_type_horizon;
				iVP++;
			}
		}

	}


	//对连接点进行检查
	int TieHorNum=0, TieHorVerNum=0, TieVerNum=0;
	int tiePtNum=0;
	for(i=0; i<nVP; )
	{
		int sPos, ePos;
		tieID=pLidVP[i].tieID;
		char vp_type=pLidVP[i].VP_type;
		bool bUpdate=false;

		tiePtNum++;
		sPos=i;
		ePos=i;

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(pLidVP[i].VP_type!=vp_type)
			{
				bUpdate=true;
			}
			ePos=i;

			i++;
		}

		if(bUpdate)
		{
			vp_type=TP_type_horizon;
			for(j=sPos; j<=ePos; j++)
			{
				pLidVP[j].VP_type=vp_type;
			}
		}

		if(vp_type & TP_type_hor_ver)	TieHorVerNum++;
		else if(vp_type & TP_type_horizon)	TieHorNum++;
		else if(vp_type & TP_type_vertical)	TieVerNum++;

	}

	LidPlane_ransac(pLidPlanes, nPatch);

	nPlane = nPatch;

	*pHONV = new HONV[nPlane];

	for(i = 0; i<nPatch; i++ )
	{
		a = -pLidPlanes[i].s1;
		b = -pLidPlanes[i].s2;
		c = 1;

		(*pHONV)[i].azimuth = atan2(b, a);
		(*pHONV)[i].zenith = atan(sqrt(a*a + b*b));
		(*pHONV)[i].frequency = pLidPlanes[i].pLidPoints->size();

	}

	if(pLidVP)	delete[] pLidVP;	pLidVP=NULL;
	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pTieLut)  delete pTieLut;  pTieLut=NULL;
	//	if(pATNPoints)	delete pATNPoints;	pATNPoints=NULL;
	return 0;
}