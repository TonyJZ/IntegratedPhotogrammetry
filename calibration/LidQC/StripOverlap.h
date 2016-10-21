#ifndef  _Strip_Overlap_H_ZJ_2010_08_24_
#define _Strip_Overlap_H_ZJ_2010_08_24_

//////////////////////////////////////////
//计算LiDAR条带重叠区  zj
//////////////////////////////////////////////////////////////////////////

#include "lastools\lasreader.h"
#include "Geometry.h"
#include "AFXTEMPL.H"
#include "..\LidCalib\Trj_Def.h"
#include "LidBaseDef.h"
#include "MemoryPool.h"


#ifdef _LIDQC_
#define  _lidQC_Dll_  __declspec(dllexport)
#else
#define  _lidQC_Dll_  __declspec(dllimport)	
#endif

#ifndef _LIDQC_
#ifdef _DEBUG
#pragma comment(lib,"LidQCD.lib")
#else
#pragma comment(lib,"LidQC.lib")
#endif
#endif

//二维Hull的边界块 (多维也可采用类似的处理方法)
struct Hull2D_Block
{
	int row;
	int col;
	bool *pBoundary;

	Hull2D_Block()
	{
		row=0;
		col=0;
		pBoundary=0;
	};

	~Hull2D_Block()
	{
		row=0;
		col=0;
		if(pBoundary)	delete pBoundary;
	}
};

struct Overlap_Block
{
	int row;
	int col;
	double x0;
	double y0;
	double dx;
	double dy;
	BYTE *pOverlap;

	Overlap_Block()
	{
		pOverlap=0;
	}
	~Overlap_Block()
	{
		if(pOverlap)
			delete pOverlap;

		pOverlap=0;
	}
};

#define NULL_Flag		 0x00	//00000000b
#define ProcData_Flag    0x01	//00000001b
#define RefData_Flag	 0x02	//00000010b
#define OverlapData_Flag 0x03	//00000011b

//计算点云凸包, 由于需要将全部数据导入内存，在处理大数据时会失败
bool _lidQC_Dll_ CalLasBoundary(LASreader* lasreader, float concavity, CArray<POINT3D, POINT3D> *pBoundary);	

//计算待处理的点云在参考点云中的重叠区
bool _lidQC_Dll_ GetOverlapData(LASreader* pProcDataset, LASreader* pRefDataset, CArray<LASpointXYZI, LASpointXYZI> *pOverlapData);


//沿中心线裁剪,需要飞行轨迹数据,裁剪结果直接修改分类号
void _lidQC_Dll_ OverlapClip_Centreline(char *pszLasFile1, char *pszLasFile2, char *pszTrjFile1, char *pszTrjFile2);

//裁剪轨迹数据,按激光点云的时间记录裁剪
bool _lidQC_Dll_ CutTrajectory(char *pszLasFile, char *pszTrjFile, TrajPos **pTrjBuf, int &num);

//判断点是否在多边形内
bool IsPtInPolygon(double Pnt[3], CArray<POINT3D, POINT3D> *pBoundary);


//计算重叠区误差
//pOverlapErr:  x, y:平面坐标, z:高程误差, i:强度误差
//type:  0 高程差 / 1 强度差
void _lidQC_Dll_ QC_CalOverlapError(char* pProcFile, char* pRefFile, CMemoryPool<POINT3D, POINT3D&> *pOverlapErr, int type=0);

void _lidQC_Dll_ QC_CalOverlapError(char* pProcFile, char* pRefFile, char* pDiffFile, int type=0);


//计算位于点云边界的数据块
void CalPointsBoundaryBlock(LASreader *pProcDataset, /*D_RECT bkRect, */double bkSize, Hull2D_Block *pHullBlock);

//计算两个条带点云的重叠块
void _lidQC_Dll_ CalOverlapBlock(LASreader *pProcDataset, LASreader* pRefDataset, double bkSize, Overlap_Block *pOBlock);

//扫描线
//void scanLine(int *pMatrix, );

#endif