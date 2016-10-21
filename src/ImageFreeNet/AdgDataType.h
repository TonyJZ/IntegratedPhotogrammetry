/******************************************************************************
 * $Id: AdgDataType.h, v 1.0 date: 10/23/2006 江万寿 Exp$
 *
 * Project:  
 * Purpose:  declarations of AdgDataType
 * Author:   江万寿, wsjws@163.com
 *
 ******************************************************************************
 * Copyright (c) 2006, 江万寿
 *
 *		未得到授权请勿拷贝
 *
 ******************************************************************************
 *
 * $Log: AdgDataType.h,v $
 *
 * Revision 0.9  10/5/2007  江万寿
 *	安装变化与不变的部分对文件结构，内存结构进行重整
 *
 * Revision 0.6  10/23/2006  江万寿
 * 整理
 * Revision 0.5  8/23/2006  谢俊峰
 * New
 */

#ifndef _ADG_DATA_TYPE_H
#define  _ADG_DATA_TYPE_H

#include "clip.h"
//#include "DPSDataObject.h"
#include "ALMdefine.h"

//#define PI 3.1415926

#ifndef _POINT2D_Def_
#define _POINT2D_Def_

struct POINT2D	{
	double x, y;
};

#endif

struct	extorPARA	{	// *.eop, exterior orientation parameter
	double Xs, Ys, Zs;			//单位：m
	double phi, omega, kappa;	//单位：弧度
	
	double R[9];		//只有旋转矩阵是形式是一致的
	double	GPSTime;
};

struct	inorPARA	{	// *.iop, interior orientation parameter
	double inorImage_a[3];	// pixel to image
	double inorImage_b[3];
	
	double inorPixel_a[3];	// image to pixel
	double inorPixel_b[3];

	double x0, y0, f;
	double	k1, k2, k3;
	double	p1, p2;

};

struct  overlapINFO {
	short imgIndex;		// -1 == NONE
	short overlap;
};

// 用于保存pos文件数据
struct imagePOS {
	int ImageID;
	double timeMark;
	double Xs,Ys,Zs;
	double phi, omega, kappa;
};


struct CEnvelope
{
public:
	double XMin,YMin,XMax,YMax;
	
	POINT2D MidPoint()
	{ 
		POINT2D pt2d;
		
		pt2d.x =(XMax+XMin)/2;
		pt2d.y = (YMax+YMin)/2;
		
		return pt2d;
	}
	
	double Width(){ return (XMax-XMin);}
	double Height(){ return(YMax-YMin);}
	bool   IsInRect(double x, double y)	{	return (x<XMax && x>XMin && y<YMax && y>YMin );	};
}; 



struct triPOINT;

enum	topoINDEX	{
	tiRIGHT = 0,
	tiTopRIGHT = 1,
	tiTop = 2,
	tiTopLEFT = 3,
	tiLEFT = 4,
	tiBottomLEFT = 5,
	tiBOTTOM = 6,
	tiBottomRIGHT = 7
};


// 用于保存影像数据
struct photoINFO	{
	CString		fileName;
    overlapINFO nbImages[8];
	extorPARA	extOr;
	inorPARA	intOr;

	triPOINT *tinPt;
	short	marker;		// 操作标记
	BYTE	useless;	//不能进行定向，不能加入区域网的影像
};


// 依赖于顺序 ?
struct  stripINFO
{
	short	fisrtImgIndex;	// 第一个影像的索引，后续影像根据拓扑关系确定 ？
	short	numOfImages;	// 影像数
	short	flyDirAngle;	// 飞行方向角[0,360]
};


//区域网信息
struct blockINFO {
	// 
	char tag[40];			// "dps survey block data file: V1.0"

	CString prjName;		//path & file
	CString prjDir;

	short  flyOverlap;			// 航向重叠度
	short  stripOverlap;		// 航带重叠度

	float	imgGrdSize;			// 影像地面大小

	vector< stripINFO >	strips;		// 不保存，实时重建 
	vector< photoINFO > images;		// 只保存影像名
};


#endif