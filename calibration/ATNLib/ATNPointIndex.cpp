#include "stdafx.h"
#include "ATNLib/ATNPointIndex.h"
#include "orsBase/orsMemoryPool.h"
#include "math.h"
#include "assert.h"
//#include "lidbase.h"
#include "MyLine.h"

//#include "CalibBasedef.h"
//#include "math.h"

#include <sys/stat.h>
bool _IsFileExist(const char *filePath)
{
	if( NULL == filePath )
		return false;


	struct stat stFileInfo;
	bool blnReturn;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat( filePath, &stFileInfo);

	if(intStat == 0) {
		// We were able to get the file attributes
		// so the file obviously exists.
		//		_getPlatform()->logPrint( ORS_LOG_DEBUG, "\tFile %s exists\n", filePath );

		blnReturn = true;
	} else {
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.

		//		_getPlatform()->logPrint( ORS_LOG_DEBUG, "\tFile %s does not exist\n", filePath );

		blnReturn = false;
	}

	return(blnReturn); 
}

CATNPtIndex::CATNPtIndex()
{
	m_pBlockLut=NULL;
	m_pLidBlock=NULL;

	m_pTin=0;
	m_pTin = new CTINClass( "pczAenVQ" );
}

CATNPtIndex::~CATNPtIndex()
{
	Release();
	if(m_pTin)
	{
		delete m_pTin;
		m_pTin=0;
	}
}

void CATNPtIndex::Release()
{
	if(m_pBlockLut)
	{
		delete m_pBlockLut;
		m_pBlockLut=NULL;
	}
	if(m_pLidBlock)
	{
		delete m_pLidBlock;
		m_pLidBlock=NULL;
	}
	m_reader.close();

//	m_strBlkName.Empty();
	m_gridRow=0;
	m_gridCol=0;
	m_gridNum=0;
	m_blkNum=0;
}

static orsMemoryPool<ATNPt_Block>	block_buffer;
static orsMemoryPool<ATNPoint>	point_buffer;
static __int64  block_offset=0;

static void chunk_ATN_point(LidBlock* cell, const ATNPoint* pos)
{
	assert(cell->ptNum);
	ATNPt_Block  *pATN_Block=NULL;
	ATNPoint	*pItem=NULL;
	
	
	pATN_Block=block_buffer.PoolAlloc();
	pItem=point_buffer.PoolAlloc();
	pATN_Block->pItem=pItem;
	memcpy(pItem, pos, sizeof(ATNPoint));
	
	
	pATN_Block->buffer_next=cell->pData;
	cell->pData=pATN_Block;
	
	cell->ptNum--;
	
}

static void write_chunked_points(LidBlock* cell, CATNWriter *writer)
{
	assert(cell->ptNum==0);

	ATNPt_Block  *pBlock, *b;
	ATNPoint	 *pt;

	pBlock=cell->pData;
	while(pBlock)
	{
		pt=pBlock->pItem;
		
		writer->write_point(pt);
//		cell->fileOffset--;
		
		b=pBlock;
		pBlock=pBlock->buffer_next;
		point_buffer.PoolDealloc(pt);
		block_buffer.PoolDealloc(b);
	}
//	assert(cell->fileOffset==0);
	
	cell->ptNum=cell->fileOffset;
	cell->fileOffset=block_offset;
	block_offset+=cell->ptNum*ATN_RawPoint_Length;
}
//说明：索引文件  *.idx    分块文件  *_blk.las
//索引文件格式: 
/*		char pBlkFileName[256];	点云分块文件名
		D_RECT	boundary;		点云包围盒
		double  gridSize;		网格大小
		double  gridRow;		行数
		double  gridCol;		列数
		int  gridNum;			网格数，对应查找表长度
		long *pBlockLut;		查找表
		int  blkNum;			块数
		LidBlock *pLidBlock;	分块数组
  */
bool CATNPtIndex::CreateQTIdx(const char *pszOrgFileName, const char *pszIdxFileName, double gridSize)
{
	CATNReader reader;
	CATNWriter writer;
	LASheader header;
	double xmax, ymax, xmin, ymin;
	int colNum, rowNum;
	int iCol, iRow, idx;
	int i, j;
	LidBlock  *pLidBlock=NULL;
	ATNPoint atnPt;
	double x, y;
	long *pBlockLut=NULL;	//分块位置查找表
	orsString strBlkName;

	strBlkName=pszIdxFileName;
	i=strBlkName.reverseFind('.');
	strBlkName=strBlkName.left(i);
	strBlkName+=_T("_blk.las");
	
	reader.open(pszOrgFileName);
	reader.read_header(&header);
	
	xmax=header.max_x;	xmin=header.min_x;
	ymax=header.max_y;	ymin=header.min_y;

// 	while(reader.read_point(&atnPt))
// 	{
// 		x=atnPt.x;
// 		y=atnPt.y;
// 
// 		if(x<xmin) xmin=x;
// 		if(x>xmax) xmax=x;
// 		if(y<ymin) ymin=y;
// 		if(y>ymax) ymax=y;
// 	}
// 	reader.reopen();
	
	colNum=(int)ceil((xmax-xmin)/gridSize);
	rowNum=(int)ceil((ymax-ymin)/gridSize);
	
	pLidBlock=new LidBlock[colNum*rowNum];
	memset(pLidBlock, 0, sizeof(LidBlock)*colNum*rowNum);
	for(i=0; i<rowNum; i++)
	{
		for(j=0; j<colNum; j++)
		{
			pLidBlock[i*colNum+j].idx=i*colNum+j;
			pLidBlock[i*colNum+j].boundbox.XMin=xmin+j*gridSize;
			pLidBlock[i*colNum+j].boundbox.XMax=xmin+(j+1)*gridSize;
			pLidBlock[i*colNum+j].boundbox.YMax=ymax-i*gridSize;
			pLidBlock[i*colNum+j].boundbox.YMin=ymax-(i+1)*gridSize;
		}
	}
	
// 	D_RECT bb_box;	
// 	bb_box.XMin=1.7e+308;	bb_box.YMin=1.7e+308;
// 	bb_box.XMax=-1.7e+308;	bb_box.YMax=-1.7e+308;

	while(reader.read_point(&atnPt))
	{//统计每一分块的点数
		x=atnPt.x;
		y=atnPt.y;
		iCol=(int)floor((x-xmin)/gridSize);
		iRow=(int)floor((ymax-y)/gridSize);
		
		idx=iRow*colNum+iCol;
		pLidBlock[idx].ptNum++;

// 		if(x<bb_box.XMin)	bb_box.XMin=x;
// 		if(x>bb_box.XMax)	bb_box.XMax=x;
// 		if(y<bb_box.YMin)	bb_box.YMin=y;
// 		if(y>bb_box.YMax)	bb_box.YMax=y;
	}
	
	pBlockLut=new long[rowNum*colNum];
	for(i=0; i<rowNum; i++)
	{
		for(j=0; j<colNum; j++)
		{
			idx=i*colNum+j;
			if(pLidBlock[idx].ptNum==0)
				pBlockLut[idx]=-1;
			else
				pBlockLut[idx]=0;
		}
	}
	
	writer.create(strBlkName);
	writer.write_header(&header);
	reader.reopen();
	block_offset=header.offset_to_point_data;
	while (reader.read_point(&atnPt))
	{//分块写点云 
		x=atnPt.x;
		y=atnPt.y;

		iCol=(int)floor((x-xmin)/gridSize);
		iRow=(int)floor((ymax-y)/gridSize);
		
		idx=iRow*colNum+iCol;
		
		chunk_ATN_point(&(pLidBlock[idx]), &atnPt);
		pLidBlock[idx].fileOffset++;	//临时存储点数
		
		if(pLidBlock[idx].ptNum==0)
		{//write chunk points
			write_chunked_points(&(pLidBlock[idx]), &writer);
		}
		
	}
	writer.close();
	
	FILE *fp=NULL;
	D_RECT boundary;
	int gridNum, blkNum;
	fp=fopen(pszIdxFileName, "wb");
	assert(fp);
	if(fp==NULL)
		return false;
		
	fwrite(strBlkName.c_str(), sizeof(char), 256, fp);
	boundary.XMax=xmax;		boundary.XMin=xmin;
	boundary.YMax=ymax;		boundary.YMin=ymin;

	fwrite(&boundary, sizeof(boundary), 1, fp);
	fwrite(&gridSize, sizeof(gridSize), 1, fp);
	fwrite(&rowNum, sizeof(rowNum), 1, fp);
	fwrite(&colNum, sizeof(colNum), 1, fp);
	gridNum=rowNum*colNum;
	fwrite(&gridNum, sizeof(gridNum), 1, fp);

	blkNum=0;
//	int diff=0, maxdiff=0;
	for(i=0; i<gridNum; i++)
	{
// 		diff=i-blkNum;
// 		if(diff>=maxdiff)
// 			maxdiff=diff;
// 		else
// 		{
// 			TRACE("error i=%d, blkNum=%d\n", i, blkNum);
// 		}
		if(pBlockLut[i]==0)
		{
			pBlockLut[i]=blkNum;
			blkNum++;
		}
	}
//	int pos;
//	int err;
//	pos=ftell(fp);
//	err=GetLastError();
	int n=fwrite(pBlockLut, sizeof(long), gridNum, fp);
//	err=GetLastError();
//	pos=ftell(fp);
//	err=GetLastError();
	
#ifdef _DEBUG
// 	for(i=0, j=0; i<gridNum; i++)
// 	{
// 		if(pLidBlock[i].ptNum>0)
// 			j++;
// 	}
// 	
// 	assert(blkNum==j);

#endif

	fwrite(&blkNum, sizeof(blkNum), 1, fp);
	for(i=0; i<gridNum; i++)
	{
		if(pLidBlock[i].ptNum>0)
		{
			fwrite(&(pLidBlock[i]), sizeof(pLidBlock[i]), 1, fp);
		}
	}
	fclose(fp);

	if(pLidBlock)
	{
		delete pLidBlock;
		pLidBlock=NULL;
	}
	
	if(pBlockLut)
	{
		delete pBlockLut;
		pBlockLut=NULL;
	}
	
	return true;
}

bool CATNPtIndex::Open(const char *pATNFileName)
{
	orsString strAtnName, strIdxName;
	int pos;

	strAtnName=pATNFileName;

	pos=strAtnName.reverseFind('.');
	strIdxName=strAtnName.left(pos);
	strIdxName+=".idx";
	
	if(_IsFileExist(strIdxName))
	{
		return OpenIdx(strIdxName);
	}
	else
		return false;
}


bool CATNPtIndex::OpenIdx(const char *pszIdxFileName)
{
	FILE *fp=NULL;
	char pName[256];
	fp=fopen(pszIdxFileName, "rb");
	if(fp==NULL)
		return false;

	Release();

	fread(pName, sizeof(char), 256, fp);
	m_strBlkName=pName;
	

	int pos;
	m_strBlkName=pszIdxFileName;			//改为相对路径   zj  2011.8.29
	pos=m_strBlkName.reverseFind('.');
	m_strBlkName=m_strBlkName.left(pos);
	m_strBlkName+="_blk.las";


	fread(&m_boundary, sizeof(m_boundary), 1, fp);
	fread(&m_gridSize, sizeof(m_gridSize), 1, fp);
	fread(&m_gridRow, sizeof(m_gridRow), 1, fp);
	fread(&m_gridCol, sizeof(m_gridCol), 1, fp);
	fread(&m_gridNum, sizeof(m_gridNum), 1, fp);
	m_pBlockLut=new long[m_gridNum];
	fread(m_pBlockLut, sizeof(long), m_gridNum, fp);
	fread(&m_blkNum, sizeof(m_blkNum), 1, fp);
	m_pLidBlock=new LidBlock[m_blkNum];
	fread(m_pLidBlock, sizeof(LidBlock), m_blkNum, fp);

	fclose(fp);	fp=NULL;

	if(!m_reader.open(m_strBlkName.c_str()))
		return false;

	return true;
}


void CATNPtIndex::GetPoints(double x0, double y0, double radius, std::vector<ATNPoint> *ptDataVec)
{
	int iRowS, iColS, iRowE, iColE, idx, blkID;
	int i, j, k;
//	double xmin, xmax, ymin, ymax;
	D_RECT  bb_circle;
	LidBlock *pBlk=NULL;
	ATNPoint  point;
	double r2, dis2;

//	xmin=m_boundary.XMin;	xmax=m_boundary.XMax;
//	ymin=m_boundary.YMin;	ymax=m_boundary.YMax;
	
	//取圆的外接矩形
	bb_circle.XMin=x0-radius;
	bb_circle.XMax=x0+radius;
	bb_circle.YMax=y0+radius;
	bb_circle.YMin=y0-radius;


	iColS=(int)floor((bb_circle.XMin-m_boundary.XMin)/m_gridSize);
	iColE=(int)floor((bb_circle.XMax-m_boundary.XMin)/m_gridSize);
	iRowS=(int)floor((m_boundary.YMax-bb_circle.YMax)/m_gridSize);
	iRowE=(int)floor((m_boundary.YMax-bb_circle.YMin)/m_gridSize);

	r2=radius*radius;
	for(i=iRowS; i<=iRowE; i++)
	{
		for(j=iColS; j<=iColE; j++)
		{
			idx=i*m_gridCol+j;
			blkID=m_pBlockLut[idx];
			if(blkID<0)   //无数据点
				continue;

			pBlk=m_pLidBlock+blkID;
			m_reader.Seek(pBlk->fileOffset);
			for(k=0; k<pBlk->ptNum; k++)
			{
				m_reader.read_point(&point);
				
				dis2=(point.x-x0)*(point.x-x0)+(point.y-y0)*(point.y-y0);
				if(dis2<r2)
				{
					ptDataVec->push_back(point);
				}
			}
		}
	}

}

locateresult CATNPtIndex::GetTriangleVertex(POINT3D *pt, double radius, std::vector<ATNPoint> *ptDataVec, double *rmse)
{
	locateresult result;

	std::vector<ATNPoint> candidate;
	int i, j;
	ATNPoint point;

	GetPoints(pt->X, pt->Y, radius, &candidate);
	if(candidate.size()<3)
	{
		result=OUTSIDE;
		return	 result;
	}

	m_pTin->BeginAddPoints();
	for(i=0; i<candidate.size(); i++)
	{
		point=candidate[i];
		m_pTin->AddPoint(point.x, point.y, i);
	}

	m_pTin->EndAddPoints();
	m_pTin->FastConstruct();

	triEDGE searchtri;
	triPOINT *triPt[3];   
	
	result=m_pTin->LocatePoint(pt->X, pt->Y, searchtri);

	if(result==OUTSIDE) return	 result;

	triPt[0]=m_pTin->org(searchtri);
	triPt[1]=m_pTin->dest(searchtri);
	triPt[2]=m_pTin->apex(searchtri);

	for(j=0; j<3; j++)
	{
		i=triPt[j]->attr;
		point=candidate[i];
		ptDataVec->push_back(point);
	}

	//三角形内插z值
	double dx1, dy1, dx21, dx31, dy21, dy31, dz21, dz31;
	ATNPoint p1, p2, p3;

	p1 = (*ptDataVec)[0];
	p2 = (*ptDataVec)[1];
	p3 = (*ptDataVec)[2];

	dx1 = p1.x - pt->X;	dy1 = p1.y - pt->Y;
	dx21 = p2.x - p1.x;	dy21 = p2.y - p1.y;	dz21 = p2.z - p1.z;	
	dx31 = p3.x - p1.x;	dy31 = p3.y - p1.y;	dz31 = p3.z - p1.z;

	pt->Z = p1.z + ( dx1*( dy21*dz31 - dy31*dz21) - dy1*( dx21*dz31 - dx31*dz21) )
		/ ( dx21*dy31 - dx31*dy21 );

	//计算均方差
	double var=0;
	for(i=0; i<candidate.size(); i++)
	{
		point=candidate[i];
		var+=(point.z-pt->Z)*(point.z-pt->Z);
	}
	
	*rmse = sqrt(var);
	return result;
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

static bool IsPtInPolygon(double x, double y, POINT3D *pPolygon, int ptNum, D_RECT *pRect)
{
	D_3DOT	pt, xpt, ipt;
	D_3DOT	p1, p2;
	CMyGLLine	LineSeg3d, xLineSeg3d;
	int		i;
//	double xmax, ymax, xmin, ymin;
//	int ptNum;

//	ptNum=pPolygon->size();

    // 循环求取交点
	pt.x=x;
	pt.y=y;
    pt.z = 0.0;
    xpt = pt;
    xpt.x = pRect->XMax + 10.0;
    //xpt.x = 1.7e208;
    LineSeg3d.SetLine(pt,xpt);
	
	
    int nCount = 0;
	p1.x = pPolygon[0].X;
	p1.y = pPolygon[0].Y;
	p1.z = 0.0;
    for (i=1; i <= ptNum; i++)
    {
        p2.x = pPolygon[i%ptNum].X;
		p2.y = pPolygon[i%ptNum].Y;
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

void CATNPtIndex::GetPoints(POINT3D *pPolygon, int ptNum, std::vector<ATNPoint> *ptDataVec)
{
	int vertexNum;
	int iRowS, iColS, iRowE, iColE, idx, blkID;
	int i, j, k;
	//	double xmin, xmax, ymin, ymax;
	D_RECT  bb_box;
	LidBlock *pBlk=NULL;
	ATNPoint  point;
//	double r2, dis2;
	
	//	xmin=m_boundary.XMin;	xmax=m_boundary.XMax;
	//	ymin=m_boundary.YMin;	ymax=m_boundary.YMax;
	
	//取多边形的外接矩形
	vertexNum=ptNum;
	bb_box.XMin=1.7e+308;
	bb_box.XMax=-1.7e+308;
	bb_box.YMax=-1.7e+308;
	bb_box.YMin=1.7e+308;
	for(i=0; i<vertexNum; i++)
	{
		if(bb_box.XMin>pPolygon[i].X)
			bb_box.XMin=pPolygon[i].X;
		if(bb_box.XMax<pPolygon[i].X)
			bb_box.XMax=pPolygon[i].X;
		if(bb_box.YMin>pPolygon[i].Y)
			bb_box.YMin=pPolygon[i].Y;
		if(bb_box.YMax<pPolygon[i].Y)
			bb_box.YMax=pPolygon[i].Y;
	}
	
	
	iColS=(int)floor((bb_box.XMin-m_boundary.XMin)/m_gridSize);
	iColE=(int)floor((bb_box.XMax-m_boundary.XMin)/m_gridSize);
	iRowS=(int)floor((m_boundary.YMax-bb_box.YMax)/m_gridSize);
	iRowE=(int)floor((m_boundary.YMax-bb_box.YMin)/m_gridSize);
	
	ptDataVec->clear();
	//按块取
	for(i=iRowS; i<=iRowE; i++)
	{
		for(j=iColS; j<=iColE; j++)
		{
			idx=i*m_gridCol+j;
			blkID=m_pBlockLut[idx];
			if(blkID<0)   //无数据点
				continue;
			
			pBlk=m_pLidBlock+blkID;
			m_reader.Seek(pBlk->fileOffset);
			for(k=0; k<pBlk->ptNum; k++)
			{
				m_reader.read_point(&point);

				// 如果点位于最小包络矩形之外，则肯定不在多边形内
				if (( point.x > bb_box.XMax) ||
					( point.x < bb_box.XMin) ||
					( point.y > bb_box.YMax)||
					( point.y < bb_box.YMin))
					continue;
				
				if(IsPtInPolygon(point.x, point.y, pPolygon, vertexNum, &bb_box))
				{
					ptDataVec->push_back(point);
				}
			}
		}
	}
	
}

bool CATNPtIndex::QueryNNPoint(double x, double y, ATNPoint *pt)
{
	//	bool bReturn=true;
	std::vector<ATNPoint> ptDataVec;

	GetPoints(x, y, 5.0, &ptDataVec);

	int num=ptDataVec.size();


	if(num==0)
		return false;

	ATNPoint nnpt;
	double min_dis2=1.7e+308;
	double dis2;
	for(int i=0; i<num; i++)
	{
		dis2=(ptDataVec[i].x-x)*(ptDataVec[i].x-x)+(ptDataVec[i].y-y)*(ptDataVec[i].y-y);
		if(dis2<min_dis2)
		{
			min_dis2=dis2;
			nnpt=ptDataVec[i];
		}
		
	}

	*pt=nnpt;
	return	true;
}