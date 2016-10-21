#include "stdafx.h"
#include "GenPointIndex/KDTindex.h"


#include "math.h"
#include "assert.h"
//#include "lidbase.h"
#include "MyLine.h"

#include "orsBase/orsIPlatform.h"
#include "orsPointCloud/orsIPointCloudService.h"

orsIPlatform *g_platform;

orsIPlatform* getPlatform()
{
	return g_platform;
}

ORS_GET_POINT_CLOUD_SERVICE_IMPL();


//PCL
#define PCL_NO_PRECOMPILE
#include <pcl/point_cloud.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/kdtree/impl/kdtree_flann.hpp>
#include <boost/make_shared.hpp>


using namespace std;
using namespace pcl;

//只对平面划分
struct MyPointType
{
	double x, y, z;                  // preferred way of adding a XYZ+padding
	uint32_t id;
}; 


pcl::PointCloud<MyPointType>::Ptr m_cloud;
pcl::KdTreeFLANN<MyPointType>	m_kdtree;


inline double sqr(double value)
{
	return value * value;
};

//判断xy坐标是否相等
static bool IsEqualXY(D_3DOT p1, D_3DOT p2)
{
	if(fabs(p1.x-p2.x)>1e-6)
		return	false;
	if(fabs(p1.y-p2.y)>1e-6)
		return	false;
	
	return	true;
}

static bool IsPtInPolygon(double x, double y, orsPOINT3D *pPolygon, int ptNum, D_RECT *pRect)
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


CKDTPtIndex::CKDTPtIndex(orsIPlatform *platform)
{
	g_platform = platform;

//	m_pBlockLut=NULL;
//	m_pLidBlock=NULL;

	m_pTin=0;
	m_pTin = new CTINClass( "pczAenVQ" );

// 	ref_ptr<orsIPointCloudService>  m_pointCloudService = 
// 		ORS_PTR_CAST(orsIPointCloudService,platform->getService(ORS_SERVICE_POINTCLOUD) );

	m_reader = NULL;

	m_cloud.reset(new pcl::PointCloud<MyPointType>());
//	m_reader = ORS_CREATE_OBJECT(orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_ATN);
}

CKDTPtIndex::~CKDTPtIndex()
{
	Release();
	if(m_pTin)
	{
		delete m_pTin;
		m_pTin=0;
	}
}

void CKDTPtIndex::Release()
{
// 	if(m_pBlockLut)
// 	{
// 		delete m_pBlockLut;
// 		m_pBlockLut=NULL;
// 	}
// 	if(m_pLidBlock)
// 	{
// 		delete m_pLidBlock;
// 		m_pLidBlock=NULL;
// 	}

	if(m_reader)
		m_reader->close();

//	m_strBlkName.Empty();
// 	m_gridRow=0;
// 	m_gridCol=0;
// 	m_gridNum=0;
// 	m_blkNum=0;
}

// static orsMemoryPool<ATNPt_Block>	block_buffer;
// static orsMemoryPool<ATNPoint>	point_buffer;
// static __int64  block_offset = 227; //跳过文件头


ors_uint32 CKDTPtIndex::get_point_contentMask()
{
	if(m_reader)
		return m_reader->get_point_contentMask();
	else
		return 0;
}


class MyPointTypeRepresenationXY : public pcl::PointRepresentation<MyPointType>
{
public:
	MyPointTypeRepresenationXY () 
	{ 
		this->nr_dimensions_ = 2; 
	}

	void copyToFloatArray (const MyPointType &p, float *out) const
	{
		out[0] = p.x;
		out[1] = p.y;
	}
};


bool CKDTPtIndex::Open(const char *pLasFileName)
{
//	orsString strAtnName, strIdxName;
	int i;

	Release();

	m_reader=getPointCloudlService()->openPointFileForRead(pLasFileName);

	if(m_reader==NULL)
		return false;

	int numofpts = m_reader->get_number_of_points();

	

//	m_kdtree = new pcl::KdTreeFLANN<MyPointType>;

	m_cloud->width = numofpts;
	m_cloud->height = 1;
	m_cloud->points.resize (m_cloud->width * m_cloud->height);

	double coordinates[3];
	i=0;
	while(m_reader->read_point(coordinates))
	{
		m_cloud->points[i].x = coordinates[0];
		m_cloud->points[i].y = coordinates[1];
		m_cloud->points[i].id = i;
		i++;
	}	
	
	m_kdtree.setInputCloud (m_cloud);
	m_kdtree.setPointRepresentation (boost::make_shared <MyPointTypeRepresenationXY>());

	return true;
}

void CKDTPtIndex::GetPoints(double x0, double y0, double radius, std::vector<LidPt_SurvInfo> *ptDataVec)
{
	ors_uint32  ptContenMask = m_reader->get_point_contentMask();

	std::vector<int> k_indices;
	std::vector<float> k_distances;
	MyPointType searchPoint;

	searchPoint.x = x0;
	searchPoint.y = y0;
	
	ptDataVec->clear();
	LidPt_SurvInfo  point;
	orsPointObservedInfo Obs_info;

	//2016.4.22
	//flann在进行radiusSearch()时如果找不到点会出现断言错误，先调用一次NN搜索，避免断言错误
	orsPOINT3D pt;
	pt.X = x0; pt.Y = y0;
	double nn_dis;
	GetNNPoint(&pt, radius, ptDataVec, &nn_dis);

	if(ptDataVec->size()==0)
		return;

	m_kdtree.radiusSearch (searchPoint, radius, k_indices, k_distances);

	m_reader->reopen();
	for(size_t i=0; i<k_indices.size(); i++)
	{
		memset(&point, 0, sizeof(LidPt_SurvInfo));

		int ID = m_cloud->points[k_indices[i]].id;
		double coord[3];
		m_reader->set_readpos(ID);
		m_reader->read_point(coord);
		point.x = coord[0];
		point.y = coord[1];
		point.z = coord[2];

		if(ptContenMask & ORS_PCM_GPSTIME)
		{
			point.time = m_reader->get_gpstime();
		}
		if(ptContenMask & ORS_PCM_OBSINFO)
		{
			m_reader->get_point_observed_info(&Obs_info);
			point.POS_Info.coord = Obs_info.pos;
			point.POS_Info.r = Obs_info.roll;
			point.POS_Info.p = Obs_info.pitch;
			point.POS_Info.h = Obs_info.heading;
		}

		ptDataVec->push_back(point);
	}
}

//取多边形内的点
void CKDTPtIndex::GetPoints(orsPOINT3D *pPolygon, int ptNum, std::vector<LidPt_SurvInfo> *ptDataVec)
{
	int vertexNum;
	int i;
	D_RECT bb_box;

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

	double x0, y0, r;  //外接圆
	x0 = (bb_box.XMax+bb_box.XMin)*0.5;
	y0 = (bb_box.YMax+bb_box.YMin)*0.5;
	r = 0.5*sqrt((bb_box.XMax-bb_box.XMin)*(bb_box.XMax-bb_box.XMin)+(bb_box.YMax-bb_box.YMin)*(bb_box.YMax-bb_box.YMin));


	ors_uint32  ptContenMask = m_reader->get_point_contentMask();

	std::vector<int> k_indices;
	std::vector<float> k_distances;
	MyPointType searchPoint;

	searchPoint.x = x0;
	searchPoint.y = y0;

	ptDataVec->clear();
	LidPt_SurvInfo  point;
	orsPointObservedInfo Obs_info;

	m_kdtree.radiusSearch (searchPoint, r, k_indices, k_distances);

	m_reader->reopen();
	for(size_t i=0; i<k_indices.size(); i++)
	{
		if(!IsPtInPolygon(m_cloud->points[k_indices[i]].x, m_cloud->points[k_indices[i]].y, pPolygon, vertexNum, &bb_box))
		{
			continue;
		}

		memset(&point, 0, sizeof(LidPt_SurvInfo));

		int ID = m_cloud->points[k_indices[i]].id;
		double coord[3];
		m_reader->set_readpos(ID);
		m_reader->read_point(coord);
		point.x = coord[0];
		point.y = coord[1];
		point.z = coord[2];

		if(ptContenMask & ORS_PCM_GPSTIME)
		{
			point.time = m_reader->get_gpstime();
		}
		if(ptContenMask & ORS_PCM_OBSINFO)
		{
			m_reader->get_point_observed_info(&Obs_info);
			point.POS_Info.coord = Obs_info.pos;
			point.POS_Info.r = Obs_info.roll;
			point.POS_Info.p = Obs_info.pitch;
			point.POS_Info.h = Obs_info.heading;

			double  Rl[9], vec_range[3];
			Rl[0]=1.0e0;	Rl[1]=0.0e0;		Rl[2]=0.0e0;
			Rl[3]=0.0e0;	Rl[4]=cos(Obs_info.scanAngle);	Rl[5]=-sin(Obs_info.scanAngle);
			Rl[6]=0.0e0;	Rl[7]=sin(Obs_info.scanAngle);	Rl[8]=cos(Obs_info.scanAngle);

			vec_range[0]=vec_range[1]=0.0e0;
			vec_range[2]=Obs_info.range;

			point.vecRange[0]=0;
			point.vecRange[1]=Rl[5]*vec_range[2];
			point.vecRange[2]=Rl[8]*vec_range[2];

		}

		ptDataVec->push_back(point);
	}

}

void CKDTPtIndex::GetNNPoint(orsPOINT3D *pt, double radius, std::vector<LidPt_SurvInfo> *ptDataVec, double *NN_dis)
{
	ors_uint32  ptContenMask = m_reader->get_point_contentMask();

	int k=10;
	std::vector<int> k_indices(k);
	std::vector<float> k_distances(k);
	MyPointType searchPoint;

	searchPoint.x = pt->X;
	searchPoint.y = pt->Y;
	
	ptDataVec->clear();
	LidPt_SurvInfo  point;
	orsPointObservedInfo Obs_info;

	m_kdtree.nearestKSearch (searchPoint, k, k_indices, k_distances);

	double minDis2=radius*radius;
	int id = -1;
	for(size_t i=0; i<10; i++)
	{
		if(k_distances[i]<minDis2)
		{
			minDis2 = k_distances[i];
			id = k_indices[i];
		}
	}

	if(id<0)  //没有找到满足距离条件的点
		return;
	
	*NN_dis = sqrt(minDis2);

	m_reader->reopen();
	
	memset(&point, 0, sizeof(LidPt_SurvInfo));

	double coord[3];
	m_reader->set_readpos(id);
	m_reader->read_point(coord);
	point.x = coord[0];
	point.y = coord[1];
	point.z = coord[2];

	if(ptContenMask & ORS_PCM_GPSTIME)
	{
		point.time = m_reader->get_gpstime();
	}
	if(ptContenMask & ORS_PCM_OBSINFO)
	{
		m_reader->get_point_observed_info(&Obs_info);
		point.POS_Info.coord = Obs_info.pos;
		point.POS_Info.r = Obs_info.roll;
		point.POS_Info.p = Obs_info.pitch;
		point.POS_Info.h = Obs_info.heading;
	}

	ptDataVec->push_back(point);
}

locateresult CKDTPtIndex::GetTriangleVertex(orsPOINT3D *pt, double radius, std::vector<LidPt_SurvInfo> *ptDataVec, double *rmse)
{
	locateresult result;

	std::vector<LidPt_SurvInfo> candidate;
	int i, j;
	LidPt_SurvInfo point;

	ptDataVec->clear();

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
	LidPt_SurvInfo p1, p2, p3;

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
	
	*rmse = sqrt(var)/candidate.size();
	return result;
}	



// double CKDTPtIndex::interpolateByTIN(double x, double y, double radius)
// {
// 
// }


bool CKDTPtIndex::QueryNNPoint(double x, double y, LidPt_SurvInfo *pt)
{
	//	bool bReturn=true;
	std::vector<LidPt_SurvInfo> ptDataVec;

	GetPoints(x, y, 5.0, &ptDataVec);

	int num=ptDataVec.size();


	if(num==0)
		return false;

	LidPt_SurvInfo nnpt;
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