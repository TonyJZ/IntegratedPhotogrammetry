//ATN文件的分块索引，提高查找定位的效率
#ifndef _ATN_Point_Index_h_zj_2010_09_27_
#define _ATN_Point_Index_h_zj_2010_09_27_

#include <vector>
#include "tin\tinClass.h"
#include "orsBase\orsString.h"
#include "ATN_def.h"
#include "orsPointCloud\orsIPointCloudReader.h"
#include "CalibBasedef.h"


#ifdef POINTINDEX_EXPORTS
#define  _PointIdx_Lib_  __declspec(dllexport)
#else
#define  _PointIdx_Lib_  __declspec(dllimport)	
#endif

// #ifndef _LIDCALIB_
// #ifdef _DEBUG
// #pragma comment(lib,"LidCalib.lib")
// #else
// #pragma comment(lib,"LidCalib.lib")
// #endif
// #endif

//using namespace std;

class _PointIdx_Lib_ CATNPtIndex
{
public:
	CATNPtIndex(orsIPlatform *platform);
	~CATNPtIndex();
	
	//对点云文件建立分块索引，加速查找
	bool CreateQTIdx(const char *pszOrgFileName, const char *pszIdxFileName, double gridSize=5.0);
	
	bool OpenIdx(const char *pszIdxFileName);
	bool Open(const char *pATNFileName);	//根据ATN文件查找索引文件

	//根据圆心坐标和半径查找数据
	void GetPoints(double x0, double y0, double radius, std::vector<ATNPoint> *ptDataVec); 
	//提取多边形内的点
	void GetPoints(POINT3D *pPolygon, int ptNum, std::vector<ATNPoint> *ptDataVec);

	void GetPoints(POINT3D *pPolygon, int ptNum, std::vector<LidPt_SurvInfo> *ptDataVec);

	//提取最近点
	void GetNNPoint(POINT3D *pt, double radius, std::vector<ATNPoint> *ptDataVec, double *NN_dis);

	//提取点所在的三角形顶点,同时内插pt的z值，并计算均方差
	locateresult GetTriangleVertex(POINT3D *pt, double radius, std::vector<ATNPoint> *ptDataVec, double *rmse);

//	double interpolateByTIN(double x, double y, double radius);

	bool QueryNNPoint(double x, double y, ATNPoint *pt);
	
protected:
	void Release();
	//根据索引号取一块
	ATNPoint* GetOneBlock(double x, double y, int &num);

	ref_ptr<orsIPointCloudReader>  m_reader;
		   
private:
	CTINClass *m_pTin;


private:
	orsString m_strBlkName;	//分块文件名
	D_RECT	m_boundary;		//边界
	double  m_gridSize;		//网格大小
	int     m_gridRow;		//网格行数
	int		m_gridCol;		//网格列数
	int		m_gridNum;		//网格数，对应查找表长度(网格中可能存在空网格)
	long   *m_pBlockLut;	//分块查找表
	int		m_blkNum;		//块数(存在数据的网格数)

	LidBlock *m_pLidBlock;	//分块数组
};


#endif