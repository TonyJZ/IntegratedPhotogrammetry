#ifndef _CIRCLE_DETECTION_H_ZJ_2010_08_11_
#define _CIRCLE_DETECTION_H_ZJ_2010_08_11_

/*************************************************************************
//从给定数据点中检测出圆心位置  zj
//  1.水平圆的检测			_2d
//  2.斜面上圆的检测	平差模型还没有搞清楚
**************************************************************************/

#ifdef _LIDCALIB_
#define  _lidCalib_Dll_  __declspec(dllexport)
#else
#define  _lidCalib_Dll_  __declspec(dllimport)	
#endif

// #ifndef _LIDCALIB_
// #ifdef _DEBUG
// #pragma comment(lib,"LidCalib.lib")
// #else
// #pragma comment(lib,"LidCalib.lib")
// #endif
// #endif

#include "\OpenRS\desktop\src\orsPointCloud\lastools\lasdefinitions.h"
#include "Geometry.h"

//#include "lastools/lasreader.h"

//检测水平面上的圆心
void _lidCalib_Dll_ DetectCircleCenter_2d(POINT2D *pData, int ptNum, double radius, POINT2D &center);

bool _lidCalib_Dll_ simulate_onecircle(int &ptNum, double radius, POINT2D *pData);

//检测两个同心圆的圆心
void _lidCalib_Dll_ DetectConcentric_2d(POINT2D *pData1, int ptNum1, double radius1, 
						 POINT2D *pData2, int ptNum2, double radius2, POINT2D &center);


//转换成矩阵
bool ConvertToMatrix(LASpointXYZI *pData, int ptNum);

//转换成影像保存
//bool _lidCalib_Dll_ ConvertToImage(char *pszName, LASpointXYZI *pData, int ptNum, double radius, double gridsize, int style=0);

#endif