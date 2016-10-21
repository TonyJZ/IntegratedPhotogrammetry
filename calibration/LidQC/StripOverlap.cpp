#include "StdAfx.h"
#include "StripOverlap.h"
#include "math.h"
#include "triangulate.h"
#include "LidBaseDef.h"
#include "MyLine.h"
#include "TinClass.h"
#include "MappingFile.h"
#include "..\LidCalib\trjreader.h"
#include "..\lastool\lasreader_fast.h"
#include "..\lastool\laswriter.h"
#include "bundler\matrix.h"


static float VecDistance3fv(const float a[3], const float b[3])
{
	return( (float)sqrt((a[0]-b[0])*(a[0]-b[0]) + (a[1]-b[1])*(a[1]-b[1]) + (a[2]-b[2])*(a[2]-b[2])) );
}

static void grow_inward(int t_idx, float length)
{
	TINtriangle* t = TINget_triangle(t_idx);
	float l = VecDistance3fv(t->V[1], t->V[2]);
	if (l > length)
	{
		int t1_idx = TIN_TRIANGLE(t->N[0]);
		int c1 = TIN_CORNER(t->N[0]);
		int c1n = TIN_NEXT(c1);
		int c1p = TIN_PREV(c1);
		TINtriangle* t1 = TINget_triangle(t1_idx);
		// make sure we can really grow here
		TINtriangle* t1r = TINget_triangle(TIN_TRIANGLE(t1->N[c1n]));
		TINtriangle* t1l = TINget_triangle(TIN_TRIANGLE(t1->N[c1p]));
		if (t1r->V[0] == 0 || t1l->V[0] == 0) return;
		int t1N[3];
		t1N[0] = t1->N[0];
		t1N[1] = t1->N[1];
		t1N[2] = t1->N[2];
		float* t1V[3];
		t1V[0] = t1->V[0];
		t1V[1] = t1->V[1];
		t1V[2] = t1->V[2];
		
		t1->N[0] = t1N[c1p];
		t1->N[1] = t->N[1];
		t1->N[2] = TIN_INDEX(t_idx,1);
		
		t1->V[0] = t->V[0];
		t1->V[1] = t1V[c1];
		t1->V[2] = t->V[2];
		
		t->N[0] = t1N[c1n];
		t->N[1] = TIN_INDEX(t1_idx,2);
		t->N[2] = t->N[2];
		
		t->V[2] = t1V[c1];
		
		t1r->N[TIN_CORNER(t1N[c1n])] = TIN_INDEX(t1_idx,0);
		t1l->N[TIN_CORNER(t1N[c1p])] = TIN_INDEX(t_idx,0);
		
		t1r = TINget_triangle(TIN_TRIANGLE(t1->N[1]));
		t1r->N[TIN_CORNER(t1->N[1])] = TIN_INDEX(t1_idx,1);
		
		grow_inward(t_idx, length);
		grow_inward(t1_idx, length);
	}
}

bool CalLasBoundary(LASreader* lasreader, float concavity, CArray<POINT3D, POINT3D> *pBoundary)
{
//	int i;
//	float concavity = 50.0f;
//	Hull2D_Block pHullBlock;

	double bb_min_x_d = 0.0;
	double bb_min_y_d = 0.0;
	double coordinates[3];

	
//	CalPointsBoundaryBlock(lasreader, bkSize, &pHullBlock);
	
	lasreader->reopen();
	
	int npoints = lasreader->npoints;
	
	
	// loop over points, store points, and triangulate
	
	int count = 0;
	float* point_buffer = new float[3*npoints];
//		(float*)malloc(sizeof(float)*3*npoints);
	
	TINclean(npoints);
	

	bb_min_x_d = lasreader->header.min_x;
	bb_min_y_d = lasreader->header.min_y;
	
	while (lasreader->read_point(coordinates))
	{
		// add point to the point buffer while subtracting the min bounding box
		point_buffer[3*count+0] = (float)(coordinates[0] - lasreader->header.min_x);
		point_buffer[3*count+1] = (float)(coordinates[1] - lasreader->header.min_y);
		point_buffer[3*count+2] = (float)(coordinates[2] - lasreader->header.min_z);
		// add the point to the triangulation
		TINadd(&(point_buffer[3*count]));
		count++;
	}
	TINfinish();
	
	// grow inwards all infinite triangles
	TINtriangle* t = 0;
	TINtriangle* run = TINget_triangle(0);
	
	for (count = 0; count < TINget_size(); count++, run++)
	{
		if (run->next < 0) // if not deleted
		{
			if (run->V[0] == 0) // if infinite
			{
				t = run;
				grow_inward(count, concavity);
			}
		}
	}
	
	// output boundary polygon
	int number_points = 0;
	
	POINT3D  pt;
	TINtriangle* t1 = TINget_triangle(TIN_TRIANGLE(t->N[2]));
	while (t1 != t)
	{
			// output t->V[1]
		pt.X=bb_min_x_d+t1->V[1][0];
		pt.Y=bb_min_y_d+t1->V[1][1];
		pt.Z=t1->V[1][2];
		
		pBoundary->Add(pt);

		t1 = TINget_triangle(TIN_TRIANGLE(t1->N[2]));
	}
	 
 
	if(point_buffer)
	{	
		delete point_buffer;
		point_buffer=NULL;
	}
	return true;
}

//2010.8.30改用重叠区计算法处理
bool GetOverlapData(LASreader* pProcDataset, LASreader* pRefDataset, CArray<LASpointXYZI, LASpointXYZI> *pOverlapData)
{
	CArray<POINT3D, POINT3D> boundary;
	float concavity=5.0;
	LASpointXYZI pt;
	double bkSize=10.0;
	Overlap_Block	pOBlock;
	int i, j, idx;
	CTINClass	tin("pczAenVQ");
	
	
//	CalLasBoundary(pRefDataset, concavity, &boundary);
	
	CalOverlapBlock(pProcDataset, pRefDataset, bkSize, &pOBlock);

	double coordinates[3];
	pRefDataset->reopen();
	tin.BeginAddPoints();
	while(pRefDataset->read_point(coordinates))
	{
		j=int((coordinates[0]-pOBlock.x0)/bkSize);
		i=int((pOBlock.y0-coordinates[1])/bkSize);
		
		idx=i*pOBlock.col+j;
		if(pOBlock.pOverlap[idx]==OverlapData_Flag)
		{//可以按照只加入边界块的方法来减少构TIN的数据量  todo
			tin.AddPoint(coordinates[0], coordinates[1], coordinates[2]);
		}
	}
	tin.EndAddPoints();
	tin.FastConstruct();


	pProcDataset->reopen();
	triEDGE searchtri;
	while (pProcDataset->read_point(coordinates))
	{
		j=int((coordinates[0]-pOBlock.x0)/bkSize);
		i=int((pOBlock.y0-coordinates[1])/bkSize);
		
		idx=i*pOBlock.col+j;
		if(pOBlock.pOverlap[idx]==OverlapData_Flag)
		{
			if(tin.LocatePoint(coordinates[0], coordinates[1], searchtri)==OUTSIDE)
				continue;

			pt.x=coordinates[0];
			pt.y=coordinates[1];
			pt.z=coordinates[2];
			pProcDataset->get_intensity(&(pt.intensity));
			
			pOverlapData->Add(pt);
		}

	}
	
	return	true;
}

//判断xy坐标是否相等
static bool IsEqualXY(D_3DOT p1, D_3DOT p2)
{
	if(fabs(p1.x-p2.x)>1e-6)
		return	false;
	if(fabs(p1.y-p2.y)>1e-6)
		return	false;
	
	return	true;
}

static bool IsPtInPolygon(double Pnt[3], CArray<POINT3D, POINT3D> *pBoundary)
{
	D_3DOT	pt, xpt, ipt;
	D_3DOT	p1, p2;
	CMyGLLine	LineSeg3d, xLineSeg3d;
	int		i;
	double xmax, ymax, xmin, ymin;
	int ptNum;

	ptNum=pBoundary->GetSize();
	xmax=(*pBoundary)[0].X;
	xmin=(*pBoundary)[0].X;
	ymax=(*pBoundary)[0].Y;
	ymin=(*pBoundary)[0].Y;

	for(i=1; i<ptNum; i++)
	{
		if(xmax<(*pBoundary)[i].X)
			xmax=(*pBoundary)[i].X;
		else if(xmin>(*pBoundary)[i].X)
			xmin=(*pBoundary)[i].X;

		if(ymax<(*pBoundary)[i].Y)
			ymax=(*pBoundary)[i].Y;
		else if(ymin>(*pBoundary)[i].Y)
			ymin=(*pBoundary)[i].Y;
	}
	
	
    // 如果点位于最小包络面之外，则肯定不在区域内，直接返回 false
    if (( Pnt[0] > xmax) ||
        ( Pnt[0] < xmin) ||
        ( Pnt[1] > ymax)||
        ( Pnt[1] < ymin)
        )
        return false;
	
    // 循环求取交点
	pt.x=Pnt[0];
	pt.y=Pnt[1];
    pt.z = 0.0;
    xpt = pt;
    xpt.x = xmax + 10.0;
    //xpt.x = 1.7e208;
    LineSeg3d.SetLine(pt,xpt);
	
	
    int nCount = 0;
	p1.x = pBoundary->GetAt(0).X;
	p1.y = pBoundary->GetAt(0).Y;
	p1.z = 0.0;
    for (i=1; i < ptNum; i++)
    {
        p2.x = pBoundary->GetAt(i).X;
		p2.y = pBoundary->GetAt(i).Y;
        p2.z = 0.0;
        // 如果所给点与顶点相等，直接返回
		/*
        if (IsEqualXY(pt,p1))
		{
		return true;
	}*/
		
        xLineSeg3d.SetLine(p1, p2);
        // 如果所给点在某一条边上，直接返回
        if (xLineSeg3d.IsOnLine(pt))
        {
			return true;
        }
        // 如果构造线段与交点存在，加入交点表
        if (LineSeg3d.IsIntersectWith(xLineSeg3d, ipt))
        {
            // 如果交点正好为顶点，判断另外的端点在哪一侧
            if (IsEqualXY(ipt,p1))
            {
                if (p2.y > ipt.y)
                    nCount++;
            }
            else if(IsEqualXY(ipt,p2))
            {
                if (p1.y > ipt.y)
                    nCount++;
            }
            else
                nCount++;
        }
        p1 = p2;
    }
	
	if ((nCount % 2) == 0)
        return false; // 交点数为偶数，不在区域内
    else
        return true; // 交点数为奇数，在区域内
}

bool CutTrajectory(char *pszLasFile, char *pszTrjFile, TrajPos **pTrjBuf, int &num)
{
	CMappingFile lasMapping;
	PublicHeadBlock *las_header;
	int ptNum;
	int offset;
	double timeS, timeE;	//时间起点和终点
	float timeExtend=10.0;	//外扩的时间 (seconds)
		

	lasMapping.SetMappingSize(128*1024*1024);

	lasMapping.Open(pszLasFile, "r");
	
	las_header=(PublicHeadBlock*)lasMapping.ReadPtr(sizeof(PublicHeadBlock));
	ptNum=las_header->Number_of_point_records;
	offset=las_header->Offset_to_point_data;
	if(las_header->Point_Data_Format_ID==0)
	{
		AfxMessageBox("点类型为0，不包含GPS时间!");
		return false;
	}
	else if(las_header->Point_Data_Format_ID==1)
	{
		POINTSTRUOFLAS1	*point1;
		lasMapping.Seek(offset, SEEK_SET);
		point1=(POINTSTRUOFLAS1*)lasMapping.ReadPtr(sizeof(POINTSTRUOFLAS1));
		timeS=point1->GPSTime;

		lasMapping.Seek(offset+(ptNum-1)*sizeof(POINTSTRUOFLAS1), SEEK_SET);
		point1=(POINTSTRUOFLAS1*)lasMapping.ReadPtr(sizeof(POINTSTRUOFLAS1));
		timeE=point1->GPSTime;
	}
	else
	{
		AfxMessageBox("未知的点数据类型，当前支持0，1!");
		return false;
	}
	lasMapping.Close();

	CTrjReader  trjReader;
	int trjPosNum;
	TrajPos *pTrj;

	trjReader.readTrjPos(pszTrjFile);
	trjReader.qsort_time_stamp_ascending();
	trjPosNum=trjReader.m_TrjHeader.PosCnt;
	pTrj=trjReader.m_PosBuf;

	int i, j, idxS=-1, idxE=-1;
	
	num=0;

	if(pTrj[0].Time > timeS)	//轨迹丢失，激光点云开始的部分没有对应轨迹
		return false;

	if(pTrj[trjPosNum-1].Time < timeE)   //轨迹丢失，激光点云结束的部分没有对应轨迹
		return false;

	for(i=0; i<trjPosNum; i++)
	{
		if(pTrj[i].Time+timeExtend > timeS)
		{
			idxS=i;
			break;
		}			
	}

	if(idxS==-1)  //轨迹与点云不对应
		return false;


	for(i=idxS; i<trjPosNum; i++)
	{
		if(pTrj[i].Time-timeExtend > timeE)
		{
			idxE=i;
			break;
		}
	}
	if(idxE==-1)  //轨迹与点云不对应
		return false;

	num=idxE-idxS+1;

	*pTrjBuf=new TrajPos[num];

	for(i=idxS, j=0; i<=idxE; i++,j++)
	{
		(*pTrjBuf)[j]=pTrj[i];
	}
	
	return true;
}

#define Overlap_Class	12		//分类号与ALM一致
void OverlapClip_Centreline(char *pszLasFile1, char *pszLasFile2, char *pszTrjFile1, char *pszTrjFile2)
{
	TrajPos *pTrjBuf1=NULL, *pTrjBuf2=NULL; 
	int trjnum1, trjnum2;
	float tInterval = 10.0;		//时间间隔，认为这段时间内近似直线
	CArray<POINT3D, POINT3D> boundary1, boundary2;
	POINT3D *ptrj1=NULL, *ptrj2=NULL, *ptrjMid=NULL;
	int sampNum1, sampNum2;
	int i, j;

	//1.提取飞行轨迹
	CutTrajectory(pszLasFile1, pszTrjFile1, &pTrjBuf1, trjnum1);
	CutTrajectory(pszLasFile2, pszTrjFile2, &pTrjBuf2, trjnum2);

	//2.判断两条轨迹的方向  (利用向量的点积)
	double dx1, dy1;
	double dx2, dy2;
	dx1=pTrjBuf1[trjnum1-1].x-pTrjBuf1[0].x;
	dy1=pTrjBuf1[trjnum1-1].y-pTrjBuf1[0].y;

	dx2=pTrjBuf2[trjnum2-1].x-pTrjBuf2[0].x;
	dy2=pTrjBuf2[trjnum2-1].y-pTrjBuf2[0].y;

	//3.计算重叠区的中心线  对轨迹重采样，计算中心线
	sampNum1=int((pTrjBuf1[trjnum1-1].Time-pTrjBuf1[0].Time)/tInterval+0.5);
	sampNum2=int((pTrjBuf2[trjnum2-1].Time-pTrjBuf2[0].Time)/tInterval+0.5);
	
	ptrj1=new POINT3D[sampNum1+10];
	ptrj2=new POINT3D[sampNum2+10];

	if(sampNum1>sampNum2)
	{
		ptrjMid=new POINT3D[sampNum1+10];
	}
	else
	{
		ptrjMid=new POINT3D[sampNum2+10];
	}
	
	double time;
	time=pTrjBuf1[0].Time;
	ptrj1[0].X=pTrjBuf1[0].x;
	ptrj1[0].Y=pTrjBuf1[0].y;
	sampNum1=1;
	for(i=1; i<trjnum1-1; i++)
	{
		if(time+tInterval>pTrjBuf1[i].Time)
			continue;

		ptrj1[sampNum1].X=pTrjBuf1[i].x;
		ptrj1[sampNum1].Y=pTrjBuf1[i].y;
		sampNum1++;
		time=pTrjBuf1[i].Time;
	}

	ptrj1[sampNum1].X=pTrjBuf1[i].x;
	ptrj1[sampNum1].Y=pTrjBuf1[i].y;
	sampNum1++;
		
	time=pTrjBuf2[0].Time;
	ptrj2[0].X=pTrjBuf2[0].x;
	ptrj2[0].Y=pTrjBuf2[0].y;
	sampNum2=1;
	for(i=1; i<trjnum2-1; i++)
	{
		if(time+tInterval>pTrjBuf2[i].Time)
			continue;
		
		ptrj2[sampNum2].X=pTrjBuf2[i].x;
		ptrj2[sampNum2].Y=pTrjBuf2[i].y;
		sampNum2++;
		time=pTrjBuf2[i].Time;
	}
	
	ptrj2[sampNum2].X=pTrjBuf2[i].x;
	ptrj2[sampNum2].Y=pTrjBuf2[i].y;
	sampNum2++;

	if(dx1*dx2+dy1*dy2>0)
	{//同向
		
		if(sampNum1>sampNum2)
		{
			for(i=0; i<sampNum2; i++)
			{
				ptrjMid[i].X=(ptrj1[i].X+ptrj2[i].X)/2;
				ptrjMid[i].Y=(ptrj1[i].Y+ptrj2[i].Y)/2;
			}
			for(i=sampNum2; i<sampNum1; i++)
			{
				ptrjMid[i].X=(ptrj1[i].X+ptrj2[sampNum2-1].X)/2;
				ptrjMid[i].Y=(ptrj1[i].Y+ptrj2[sampNum2-1].Y)/2;
			}

// 			for(i=0; i<sampNum1; i++)
// 			{
// 				boundary1.Add(ptrj1[i]);
// 			}
// 			for(i=sampNum2-1; i>=0; i--)
// 			{
// 				boundary1.Add(ptrjMid[i]);
// 				boundary2.Add(ptrjMid[i]);
// 			}
// 			for(i=0; i<sampNum2; i++)
// 			{
// 				boundary2.Add(ptrj2[i]);
// 			}
		}
		else
		{
			for(i=0; i<sampNum1; i++)
			{
				ptrjMid[i].X=(ptrj1[i].X+ptrj2[i].X)/2;
				ptrjMid[i].Y=(ptrj1[i].Y+ptrj2[i].Y)/2;
			}
			for(i=sampNum1; i<sampNum2; i++)
			{
				ptrjMid[i].X=(ptrj1[sampNum1-1].X+ptrj2[i].X)/2;
				ptrjMid[i].Y=(ptrj1[sampNum1-1].Y+ptrj2[i].Y)/2;
			}

// 			for(i=0; i<sampNum1; i++)
// 			{
// 				boundary1.Add(ptrj1[i]);
// 			}
// 			for(i=sampNum1-1; i>=0; i--)
// 			{
// 				boundary1.Add(ptrjMid[i]);
// 				boundary2.Add(ptrjMid[i]);
// 			}
// 			for(i=0; i<sampNum2; i++)
// 			{
// 				boundary2.Add(ptrj2[i]);
// 			}
		}
	}
	else
	{//反向
		if(sampNum1>sampNum2)
		{
			for(i=0, j=sampNum1-1; i<sampNum2; i++, j--)
			{
				ptrjMid[i].X=(ptrj1[j].X+ptrj2[i].X)/2;
				ptrjMid[i].Y=(ptrj1[j].Y+ptrj2[i].Y)/2;
			}
			for(i=sampNum2; i<sampNum1; i++)
			{
				ptrjMid[i].X=(ptrj1[sampNum1-i-1].X+ptrj2[sampNum2-1].X)/2;
				ptrjMid[i].Y=(ptrj1[sampNum1-i-1].Y+ptrj2[sampNum2-1].Y)/2;
			}
			
// 			for(i=0; i<sampNum1; i++)
// 			{
// 				boundary1.Add(ptrj1[i]);
// 			}
// 			for(i=0; i<sampNum1; i++)
// 			{
// 				boundary1.Add(ptrjMid[i]);
// 				boundary2.Add(ptrjMid[i]);
// 			}
// 			for(i=sampNum2-1; i>=0; i--)
// 			{
// 				boundary2.Add(ptrj2[i]);
// 			}
		}
		else
		{
			for(i=0, j=sampNum2-1; i<sampNum1; i++, j--)
			{
				ptrjMid[i].X=(ptrj1[i].X+ptrj2[j].X)/2;
				ptrjMid[i].Y=(ptrj1[i].Y+ptrj2[j].Y)/2;
			}

			for(i=sampNum1; i<sampNum2; i++)
			{
				ptrjMid[i].X=(ptrj1[sampNum1-1].X+ptrj2[sampNum2-i-1].X)/2;
				ptrjMid[i].Y=(ptrj1[sampNum1-1].Y+ptrj2[sampNum2-i-1].Y)/2;
			}
			
// 			for(i=0; i<sampNum2; i++)
// 			{
// 				boundary2.Add(ptrj2[i]);
// 			}
// 			for(i=0; i<sampNum1; i++)
// 			{
// 				boundary1.Add(ptrjMid[i]);
// 				boundary2.Add(ptrjMid[i]);
// 			}
// 			for(i=sampNum1-1; i>=0; i--)
// 			{
// 				boundary1.Add(ptrj1[i]);
// 			}
		}
	}

	//4.利用最小二乘拟合直线
	int ptNum;
	if(sampNum1>sampNum2)
		ptNum=sampNum1;
	else
		ptNum=sampNum2;

#ifdef _DEBUG
// 	CTrjReader  trjReader;
// 	CTrjWriter  trjWriter;
// 	TrajHdr pHeader;
// 	TrajPos *pPosBuf=NULL;
// 
// 	pPosBuf=new TrajPos[ptNum];
// 	for(i=0; i<ptNum; i++)
// 	{
// 		pPosBuf[i].x=ptrjMid[i].X;
// 		pPosBuf[i].y=ptrjMid[i].Y;
// 		pPosBuf[i].z=ptrjMid[i].Z;
// 		TRACE("%.3lf %.3lf\n", ptrjMid[i].X, ptrjMid[i].Y);
// 	}
// 	
// 	trjReader.read_header(pszTrjFile1, &pHeader);
// 	pHeader.PosCnt=ptNum;
// 	trjWriter.open("G:\\D_experiment\\overlop\\centeiline.trj");
// 	trjWriter.write_header(&pHeader);
// 	trjWriter.writeTrjPos(pPosBuf, ptNum);
// 
// 	trjWriter.close();
#endif

// 	ptNum=4;
// 	ptrjMid[0].X=-2;	ptrjMid[0].Y=-2;
// 	ptrjMid[1].X=-1;	ptrjMid[1].Y=-1;
// 	ptrjMid[2].X=1;		ptrjMid[2].Y=1;
// 	ptrjMid[3].X=2;		ptrjMid[3].Y=2;

	dx1=fabs(ptrjMid[ptNum-1].X-ptrjMid[0].X);
	dy1=fabs(ptrjMid[ptNum-1].Y-ptrjMid[0].Y);

	double *A=new double[ptNum*2];
	double *L=new double[ptNum];
	double X[2];
	double linePara[3];	//ax+by+c=0;
	if(dx1>dy1)
	{//y=f(x)
		for(i=0; i<ptNum; i++)
		{
			A[i*2]=ptrjMid[i].X;
			A[i*2+1]=1.0;
			L[i]=ptrjMid[i].Y;
		}
		
		dgelsy_driver(A, L, X, ptNum, 2, 1);
		linePara[0]=X[0];
		linePara[1]=-1.0;
		linePara[2]=X[1];
	}
	else
	{//x=f(y)
		for(i=0; i<ptNum; i++)
		{
			A[i*2]=ptrjMid[i].Y;
			A[i*2+1]=1.0;
			L[i]=ptrjMid[i].X;
		}
		
		dgelsy_driver(A, L, X, ptNum, 2, 1);
		linePara[0]=-1.0;
		linePara[1]=X[0];
		linePara[2]=X[1];
	}
	delete A;
	delete L;
	
#ifdef _DEBUG
	if(dx1>dy1)
	{
		TRACE("%.3lf %.3lf\n", ptrjMid[0].X+1.0, X[0]*(ptrjMid[0].X+1.0)+X[1]);
		TRACE("%.3lf %.3lf\n", ptrjMid[ptNum-1].X+1.0, X[0]*(ptrjMid[ptNum-1].X+1.0)+X[1]);
	}
	else
	{
		TRACE("%.3lf %.3lf\n", X[0]*(ptrjMid[0].Y+1.0)+X[1], ptrjMid[0].Y+1.0);
		TRACE("%.3lf %.3lf\n", X[0]*(ptrjMid[ptNum-1].Y+1.0)+X[1], ptrjMid[ptNum-1].Y+1.0);
	}
#endif	

	//利用直线判断
	CMappingFile	lasMapping;
	PublicHeadBlock *las_header;
	int  offset_to_data;
	unsigned char  point_format;
	unsigned short point_Length;
	int num_point;
	double coord[3];
	double xScale, xOffset, yScale, yOffset, zScale, zOffset;

	char strip1_Flag, strip2_Flag, point_flag;
	
	if(ptrj1[sampNum1/2].X*linePara[0]+ptrj1[sampNum1/2].Y*linePara[1]+linePara[2]>0)
		strip1_Flag=1;
	else
		strip1_Flag=-1;

	if(ptrj2[sampNum1/2].X*linePara[0]+ptrj2[sampNum1/2].Y*linePara[1]+linePara[2]>0)
		strip2_Flag=1;
	else
		strip2_Flag=-1;


	lasMapping.SetMappingSize(128*1024*1024);
	lasMapping.Open(pszLasFile1, "u");	//更新分类号
	
	las_header=(PublicHeadBlock*)lasMapping.ReadPtr(sizeof(PublicHeadBlock));
	offset_to_data=las_header->Offset_to_point_data;
	point_format=las_header->Point_Data_Format_ID;	
	point_Length=las_header->Point_Data_Record_Length;
	num_point=las_header->Number_of_point_records;
	xScale=las_header->X_scale_factor;
	yScale=las_header->Y_scale_factor;
	zScale=las_header->Z_scale_factor;
	xOffset=las_header->X_offset;
	yOffset=las_header->Y_offset;
	zOffset=las_header->Z_offset;
	
	lasMapping.Seek(offset_to_data, SEEK_SET);
	if(point_format==0)
	{
		POINTSTRUOFLAS0  *point;
		for(i=0; i<num_point; i++)
		{
			point=(POINTSTRUOFLAS0*)lasMapping.ReadPtr(point_Length);
			
			coord[0] = point->x*xScale+xOffset;
			coord[1] = point->y*yScale+yOffset;
			coord[2] = point->z*zScale+zOffset;
			
			if(coord[0]*linePara[0]+coord[1]*linePara[1]+linePara[2]>0)
				point_flag=1;
			else
				point_flag=-1;

			if(point_flag!=strip1_Flag)
			{
				point->Classification=Overlap_Class;
			}
		}
	}
	else if (point_format==1)
	{
		POINTSTRUOFLAS1  *point;
		for(i=0; i<num_point; i++)
		{
			point=(POINTSTRUOFLAS1*)lasMapping.ReadPtr(point_Length);
			
			coord[0] = point->x*xScale+xOffset;
			coord[1] = point->y*yScale+yOffset;
			coord[2] = point->z*zScale+zOffset;
			
			if(coord[0]*linePara[0]+coord[1]*linePara[1]+linePara[2]>0)
				point_flag=1;
			else
				point_flag=-1;
			
			

			if(point_flag!=strip1_Flag)
			{
				point->Classification=Overlap_Class;
			}
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	
	lasMapping.Close();
	

	lasMapping.SetMappingSize(128*1024*1024);
	lasMapping.Open(pszLasFile2, "u");	//更新分类号
	
	las_header=(PublicHeadBlock*)lasMapping.ReadPtr(sizeof(PublicHeadBlock));
	offset_to_data=las_header->Offset_to_point_data;
	point_format=las_header->Point_Data_Format_ID;	
	point_Length=las_header->Point_Data_Record_Length;
	num_point=las_header->Number_of_point_records;
	xScale=las_header->X_scale_factor;
	yScale=las_header->Y_scale_factor;
	zScale=las_header->Z_scale_factor;
	xOffset=las_header->X_offset;
	yOffset=las_header->Y_offset;
	zOffset=las_header->Z_offset;
	
	lasMapping.Seek(offset_to_data, SEEK_SET);
	if(point_format==0)
	{
		POINTSTRUOFLAS0  *point;
		for(i=0; i<num_point; i++)
		{
			point=(POINTSTRUOFLAS0*)lasMapping.ReadPtr(point_Length);
			
			coord[0] = point->x*xScale+xOffset;
			coord[1] = point->y*yScale+yOffset;
			coord[2] = point->z*zScale+zOffset;
			
			if(coord[0]*linePara[0]+coord[1]*linePara[1]+linePara[2]>0)
				point_flag=1;
			else
				point_flag=-1;
			
			if(point_flag!=strip2_Flag)
			{
				point->Classification=Overlap_Class;
			}
		}
	}
	else if (point_format==1)
	{
		POINTSTRUOFLAS1  *point;
		for(i=0; i<num_point; i++)
		{
			point=(POINTSTRUOFLAS1*)lasMapping.ReadPtr(point_Length);
			
			coord[0] = point->x*xScale+xOffset;
			coord[1] = point->y*yScale+yOffset;
			coord[2] = point->z*zScale+zOffset;
			
			if(coord[0]*linePara[0]+coord[1]*linePara[1]+linePara[2]>0)
				point_flag=1;
			else
				point_flag=-1;
			
			if(point_flag!=strip2_Flag)
			{
				point->Classification=Overlap_Class;
			}
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	
	lasMapping.Close();

	if(pTrjBuf1)
	{
		delete pTrjBuf1;
		pTrjBuf1=NULL;
	}
	if(pTrjBuf2)
	{
		delete pTrjBuf2;
		pTrjBuf2=NULL;
	}
	if(ptrj1)
	{
		delete ptrj1;
		ptrj1=NULL;
	}
	if(ptrj2)
	{
		delete ptrj2;
		ptrj2=NULL;
	}
	if(ptrjMid)
	{
		delete ptrjMid;
		ptrjMid=NULL;
	}
}


void QC_CalOverlapError(char* pProcFile, char* pRefFile, CMemoryPool<POINT3D, POINT3D&> *pOverlapErr, int type)
{
	CArray<POINT3D, POINT3D> boundary;
	float concavity=5.0;
	POINT3D	*pt;
	LASpointXYZI xyzipt;
	double att;
	LASreader_fast pProcDataset, pRefDataset;
	double bkSize=10.0;
	Overlap_Block pOBlock;
	

	pProcDataset.open(pProcFile, "r");
	pRefDataset.open(pRefFile, "r");

	int wid, hei;
	D_RECT bkRect;
	
	bkRect.XMax=pProcDataset.header.max_x;
	bkRect.XMin=pProcDataset.header.min_x;
	bkRect.YMax=pProcDataset.header.max_y;
	bkRect.YMin=pProcDataset.header.min_y;
	
	if(bkRect.XMax<pRefDataset.header.max_x)
		bkRect.XMax=pRefDataset.header.max_x;
	if(bkRect.XMin>pRefDataset.header.min_x)
		bkRect.XMin=pRefDataset.header.min_x;
	if(bkRect.YMax<pRefDataset.header.max_y)
		bkRect.YMax=pRefDataset.header.max_y;
	if(bkRect.YMin>pRefDataset.header.min_y)
		bkRect.YMin=pRefDataset.header.min_y;
	
	wid=int((bkRect.XMax-bkRect.XMin)/bkSize+0.5);
	hei=int((bkRect.YMax-bkRect.YMin)/bkSize+0.5);
	
	pOBlock.row=hei;
	pOBlock.col=wid;
	pOBlock.x0=bkRect.XMin;
	pOBlock.y0=bkRect.YMax;
	pOBlock.dx=bkSize;
	pOBlock.dy=bkSize;
	
	pOBlock.pOverlap=new BYTE[wid*hei];
	memset(pOBlock.pOverlap, 0, sizeof(BYTE)*wid*hei);
	
	
	int i, j, idx;
	double coord[3];
	while(pProcDataset.read_point(coord))
	{//统计分块点数
// 		if(pProcDataset.p_count==7190030)
// 		{
// 			i=7190031;
// 		}

		j=int((coord[0]-bkRect.XMin)/bkSize);
		i=int((bkRect.YMax-coord[1])/bkSize);
		
		idx=i*wid+j;
		pOBlock.pOverlap[idx]|=ProcData_Flag;
	}
	while(pRefDataset.read_point(coord))
	{
		j=int((coord[0]-bkRect.XMin)/bkSize);
		i=int((bkRect.YMax-coord[1])/bkSize);
		
		idx=i*wid+j;
		pOBlock.pOverlap[idx]|=RefData_Flag;
	}

	CTINClass	tin("pczAenVQ");
	pRefDataset.reopen();
	tin.BeginAddPoints();
	if(type==0)
	{//高程差
		while(pRefDataset.read_point(coord))
		{
			j=int((coord[0]-pOBlock.x0)/bkSize);
			i=int((pOBlock.y0-coord[1])/bkSize);
			
			idx=i*pOBlock.col+j;
			if(pOBlock.pOverlap[idx]==OverlapData_Flag)
			{
				tin.AddPoint(coord[0], coord[1], coord[2]);
			}
		}
	}
	else if(type==1)
	{//强度差
		while(pRefDataset.read_point(coord))
		{
			j=int((coord[0]-pOBlock.x0)/bkSize);
			i=int((pOBlock.y0-coord[1])/bkSize);
			
			idx=i*pOBlock.col+j;
			if(pOBlock.pOverlap[idx]==OverlapData_Flag)
			{
				pRefDataset.get_intensity(&(xyzipt.intensity));
				tin.AddPoint(coord[0], coord[1], xyzipt.intensity);
			}
		}
	}	
	tin.EndAddPoints();
	tin.FastConstruct();
	
	
	pProcDataset.reopen();
//	triEDGE searchtri;
	if(type==0)
	{
		while (pProcDataset.read_point(coord))
		{
			j=int((coord[0]-pOBlock.x0)/bkSize);
			i=int((pOBlock.y0-coord[1])/bkSize);
			
			idx=i*pOBlock.col+j;
			if(pOBlock.pOverlap[idx]==OverlapData_Flag)
			{
				//if(tin.LocatePoint(coord[0], coord[1], searchtri)==OUTSIDE)
				//	continue;
				
				
				xyzipt.x=coord[0];
				xyzipt.y=coord[1];
				xyzipt.z=coord[2];
				pProcDataset.get_intensity(&(xyzipt.intensity));
				
				att=tin.InterpolateAttribute(xyzipt.x, xyzipt.y);
				if(att==_DUMMY_POINT_ATTR)
					continue;

				pt=pOverlapErr->PoolAlloc();

				pt->X=xyzipt.x;
				pt->Y=xyzipt.y;
				pt->Z=xyzipt.z-att;
				//pOverlapErr->Add(pt);
			}
		}
	}
	else if(type==1)
	{
		while (pProcDataset.read_point(coord))
		{
			j=int((coord[0]-pOBlock.x0)/bkSize);
			i=int((pOBlock.y0-coord[1])/bkSize);
			
			idx=i*pOBlock.col+j;
			if(pOBlock.pOverlap[idx]==OverlapData_Flag)
			{
			//	if(tin.LocatePoint(coord[0], coord[1], searchtri)==OUTSIDE)
			//		continue;
				
				
				xyzipt.x=coord[0];
				xyzipt.y=coord[1];
				xyzipt.z=coord[2];
				pProcDataset.get_intensity(&(xyzipt.intensity));
				
				att=tin.InterpolateAttribute(xyzipt.x, xyzipt.y);
				if(att==_DUMMY_POINT_ATTR)
					continue;
				
				pt=pOverlapErr->PoolAlloc();
				pt->X=xyzipt.x;
				pt->Y=xyzipt.y;
				pt->Z=xyzipt.intensity-att;
				
			//	pOverlapErr->Add(pt);
			}
		}
	}
	
	return ;
}

void QC_CalOverlapError(char* pProcFile, char* pRefFile, char* pDiffFile, int type)
{
	CArray<POINT3D, POINT3D> boundary;
	float concavity=5.0;
	POINT3D	*pt;
	LASpointXYZI xyzipt;
	double att;
	LASreader_fast pProcDataset, pRefDataset;
	LASwriter laswriter;
	double bkSize=10.0;
	Overlap_Block pOBlock;


	pProcDataset.open(pProcFile, "r");
	pRefDataset.open(pRefFile, "r");

	int wid, hei;
	D_RECT bkRect;

	bkRect.XMax=pProcDataset.header.max_x;
	bkRect.XMin=pProcDataset.header.min_x;
	bkRect.YMax=pProcDataset.header.max_y;
	bkRect.YMin=pProcDataset.header.min_y;

	if(bkRect.XMax<pRefDataset.header.max_x)
		bkRect.XMax=pRefDataset.header.max_x;
	if(bkRect.XMin>pRefDataset.header.min_x)
		bkRect.XMin=pRefDataset.header.min_x;
	if(bkRect.YMax<pRefDataset.header.max_y)
		bkRect.YMax=pRefDataset.header.max_y;
	if(bkRect.YMin>pRefDataset.header.min_y)
		bkRect.YMin=pRefDataset.header.min_y;

	wid=int((bkRect.XMax-bkRect.XMin)/bkSize+0.5);
	hei=int((bkRect.YMax-bkRect.YMin)/bkSize+0.5);

	pOBlock.row=hei;
	pOBlock.col=wid;
	pOBlock.x0=bkRect.XMin;
	pOBlock.y0=bkRect.YMax;
	pOBlock.dx=bkSize;
	pOBlock.dy=bkSize;

	pOBlock.pOverlap=new BYTE[wid*hei];
	memset(pOBlock.pOverlap, 0, sizeof(BYTE)*wid*hei);


	int i, j, idx;
	double coord[3];
	while(pProcDataset.read_point(coord))
	{//统计分块点数
		// 		if(pProcDataset.p_count==7190030)
		// 		{
		// 			i=7190031;
		// 		}

		j=int((coord[0]-bkRect.XMin)/bkSize);
		i=int((bkRect.YMax-coord[1])/bkSize);

		idx=i*wid+j;
		pOBlock.pOverlap[idx]|=ProcData_Flag;
	}
	while(pRefDataset.read_point(coord))
	{
		j=int((coord[0]-bkRect.XMin)/bkSize);
		i=int((bkRect.YMax-coord[1])/bkSize);

		idx=i*wid+j;
		pOBlock.pOverlap[idx]|=RefData_Flag;
	}

	CTINClass	tin("pczAenVQ");
	pRefDataset.reopen();
	tin.BeginAddPoints();
	if(type==0)
	{//高程差
		while(pRefDataset.read_point(coord))
		{
			j=int((coord[0]-pOBlock.x0)/bkSize);
			i=int((pOBlock.y0-coord[1])/bkSize);

			idx=i*pOBlock.col+j;
			if(pOBlock.pOverlap[idx]==OverlapData_Flag)
			{
				tin.AddPoint(coord[0], coord[1], coord[2]);
			}
		}
	}
	else if(type==1)
	{//强度差
		while(pRefDataset.read_point(coord))
		{
			j=int((coord[0]-pOBlock.x0)/bkSize);
			i=int((pOBlock.y0-coord[1])/bkSize);

			idx=i*pOBlock.col+j;
			if(pOBlock.pOverlap[idx]==OverlapData_Flag)
			{
				pRefDataset.get_intensity(&(xyzipt.intensity));
				tin.AddPoint(coord[0], coord[1], xyzipt.intensity);
			}
		}
	}	
	tin.EndAddPoints();
	tin.FastConstruct();


	FILE *fp=NULL;
	fp=fopen(pDiffFile, "wb");
	if(fp==0)
		return;

	laswriter.open(fp, NULL);


	pProcDataset.reopen();
	//	triEDGE searchtri;
	if(type==0)
	{
		while (pProcDataset.read_point(coord))
		{
			j=int((coord[0]-pOBlock.x0)/bkSize);
			i=int((pOBlock.y0-coord[1])/bkSize);

			idx=i*pOBlock.col+j;
			if(pOBlock.pOverlap[idx]==OverlapData_Flag)
			{
				//if(tin.LocatePoint(coord[0], coord[1], searchtri)==OUTSIDE)
				//	continue;


				xyzipt.x=coord[0];
				xyzipt.y=coord[1];
				xyzipt.z=coord[2];
				pProcDataset.get_intensity(&(xyzipt.intensity));

				att=tin.InterpolateAttribute(xyzipt.x, xyzipt.y);
				if(att==_DUMMY_POINT_ATTR)
					continue;

				//pt=pOverlapErr->PoolAlloc();

				//pt->X=xyzipt.x;
				//pt->Y=xyzipt.y;
				//pt->Z=xyzipt.z-att;
				//pOverlapErr->Add(pt);

				laswriter.write_point(xyzipt.x, xyzipt.y, xyzipt.z-att);
			}
		}
	}
	else if(type==1)
	{
		while (pProcDataset.read_point(coord))
		{
			j=int((coord[0]-pOBlock.x0)/bkSize);
			i=int((pOBlock.y0-coord[1])/bkSize);

			idx=i*pOBlock.col+j;
			if(pOBlock.pOverlap[idx]==OverlapData_Flag)
			{
				//	if(tin.LocatePoint(coord[0], coord[1], searchtri)==OUTSIDE)
				//		continue;


				xyzipt.x=coord[0];
				xyzipt.y=coord[1];
				xyzipt.z=coord[2];
				pProcDataset.get_intensity(&(xyzipt.intensity));

				att=tin.InterpolateAttribute(xyzipt.x, xyzipt.y);
				if(att==_DUMMY_POINT_ATTR)
					continue;

// 				pt=pOverlapErr->PoolAlloc();
// 				pt->X=xyzipt.x;
// 				pt->Y=xyzipt.y;
// 				pt->Z=xyzipt.intensity-att;

				laswriter.write_point(xyzipt.x, xyzipt.y, xyzipt.intensity-att);

				//	pOverlapErr->Add(pt);
			}
		}
	}

	laswriter.close();
	fclose(fp);
	
	return ;
}

//用这种方法计算凸包会出现问题：求取边界块以后，计算出的边界多边形会形成内边界和外边界的环
void CalPointsBoundaryBlock(LASreader *pProcDataset, /*D_RECT bkRect, */double bkSize, Hull2D_Block *pHullBlock)
{
	int *pMatrix=NULL;
	int wid, hei;
	D_RECT bkRect;

	bkRect.XMax=pProcDataset->header.max_x;
	bkRect.XMin=pProcDataset->header.min_x;
	bkRect.YMax=pProcDataset->header.max_y;
	bkRect.YMin=pProcDataset->header.min_y;

	wid=int((bkRect.XMax-bkRect.XMin)/bkSize+0.5);
	hei=int((bkRect.YMax-bkRect.YMin)/bkSize+0.5);

	pMatrix=new int[wid*hei];
	memset(pMatrix, 0, sizeof(int)*wid*hei);

	pHullBlock->row=hei;
	pHullBlock->col=wid;
// 	if(pHullBlock->pBoundary)
// 	{
// 		delete pHullBlock->pBoundary;
// 	}
	pHullBlock->pBoundary=new bool[wid*hei];
	memset(pHullBlock->pBoundary, 0, sizeof(bool)*wid*hei);

	
	int i, j, idx;
	pProcDataset->reopen();
	double coord[3];
	while(pProcDataset->read_point(coord))
	{//统计分块点数
		j=int((coord[0]-bkRect.XMin)/bkSize);
		i=int((bkRect.YMax-coord[1])/bkSize);

		idx=i*wid+j;
		pMatrix[idx]++;
	}

	//确定边界上的分块
	int *pLine;
	int beginID, endID;
	bool bFlag;
	for(i=0; i<hei; i++)
	{
		pLine=pMatrix+i*wid;
		bFlag=false;
		for(j=0; j<wid; j++)
		{
			if(pLine[j]==0 && !bFlag)
				continue;

			if(pLine[j]>0)
			{
				if(!bFlag)
				{
					beginID=j;
					endID=j;
					bFlag=true;
				}
				else
					endID=j;
			}
			else 
			{//pLine[j]==0	
				pHullBlock->pBoundary[i*wid+beginID]=true;
				pHullBlock->pBoundary[i*wid+endID]=true;
				bFlag=false;
			}

		}
		
	}

	
	if(pMatrix)
	{
		delete pMatrix;
		pMatrix=NULL;
	}
}


void CalOverlapBlock(LASreader *pProcDataset, LASreader* pRefDataset, double bkSize, Overlap_Block *pOBlock)
{
//	int *pMatrix=NULL;
	int wid, hei;
	D_RECT bkRect;
	
	bkRect.XMax=pProcDataset->header.max_x;
	bkRect.XMin=pProcDataset->header.min_x;
	bkRect.YMax=pProcDataset->header.max_y;
	bkRect.YMin=pProcDataset->header.min_y;

	if(bkRect.XMax<pRefDataset->header.max_x)
		bkRect.XMax=pRefDataset->header.max_x;
	if(bkRect.XMin>pRefDataset->header.min_x)
		bkRect.XMin=pRefDataset->header.min_x;
	if(bkRect.YMax<pRefDataset->header.max_y)
		bkRect.YMax=pRefDataset->header.max_y;
	if(bkRect.YMin>pRefDataset->header.min_y)
		bkRect.YMin=pRefDataset->header.min_y;

	wid=int((bkRect.XMax-bkRect.XMin)/bkSize+0.5);
	hei=int((bkRect.YMax-bkRect.YMin)/bkSize+0.5);

	pOBlock->row=hei;
	pOBlock->col=wid;
	pOBlock->x0=bkRect.XMin;
	pOBlock->y0=bkRect.YMax;
	pOBlock->dx=bkSize;
	pOBlock->dy=bkSize;
	
	if(pOBlock->pOverlap)	delete pOBlock->pOverlap;
	pOBlock->pOverlap=new BYTE[wid*hei];
	memset(pOBlock->pOverlap, 0, sizeof(BYTE)*wid*hei);
	
		
	int i, j, idx;
	pProcDataset->reopen();
	double coord[3];
	while(pProcDataset->read_point(coord))
	{//统计分块点数
		j=int((coord[0]-bkRect.XMin)/bkSize);
		i=int((bkRect.YMax-coord[1])/bkSize);
		
		idx=i*wid+j;
		pOBlock->pOverlap[idx]|=ProcData_Flag;
	}

	while(pRefDataset->read_point(coord))
	{
		j=int((coord[0]-bkRect.XMin)/bkSize);
		i=int((bkRect.YMax-coord[1])/bkSize);
		
		idx=i*wid+j;
		pOBlock->pOverlap[idx]|=RefData_Flag;
	}

}