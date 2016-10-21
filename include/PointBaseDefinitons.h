/*
===============================================================================

  FILE:  PointBaseDefinitions.h
  
  CONTENTS:
		Point Processing中用到的基本数据结构定义

  PROGRAMMERS:
			Tony Zhang
    
  
  COPYRIGHT:
  
    copyright (C) 2007  zhangjing_whu@126.com
    

  
  CHANGE HISTORY:
				Created  in  2008.4.16	By  T.Zhang
   
  
===============================================================================
*/

#ifndef __POINT_BASE_DEFINITONS__
#define __POINT_BASE_DEFINITONS__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ALMDefine.h"

#pragma pack(1) // 按照1字节方式进行对齐

//LiDAR激光点结构		28Bytes
//激光点的实际地理坐标
struct	LIDAR_POINT	
{
	double	X;//X地理坐标(X坐标等于：X要乘以XScaleFactor加上XOffset)
	double	Y;//Y地理坐标(Y坐标等于：Y要乘以YScaleFactor加上YOffset)
	double	Z;//Z地理坐标(Z坐标等于：Z要乘以ZScaleFactor加上ZOffset)
	unsigned short	 Intensity;//脉冲强度值
	unsigned char    Pulse;	//脉冲信息，脉冲回波号3bits(0,1,2)，总返回数3bits(3,4,5)，扫描方向1bits(6)，航线边缘1bits(7)
	unsigned char	 Classification;//点分类
};

//扩展的LIDAR_POINT结构，添加GPS时间
struct	LIDAR_POINT1
{
	double	X;//X地理坐标(X坐标等于：X要乘以XScaleFactor加上XOffset)
	double	Y;//Y地理坐标(Y坐标等于：Y要乘以YScaleFactor加上YOffset)
	double	Z;//Z地理坐标(Z坐标等于：Z要乘以ZScaleFactor加上ZOffset)
	unsigned short	 Intensity;//脉冲强度值
	unsigned char    Pulse;	//脉冲信息，脉冲回波号3bits(0,1,2)，总返回数3bits(3,4,5)，扫描方向1bits(6)，航线边缘1bits(7)
	unsigned char	 Classification;//点分类
	double		GPSTime;
};

//激光点的模型坐标		16Bytes
typedef struct  
{
	long	X;//X模型坐标
	long	Y;//Y模型坐标
	long	Z;//Z模型坐标
	unsigned short	 Intensity;//脉冲强度值
	unsigned char    Pulse;	//脉冲信息，脉冲回波号3bits(0,1,2)，总返回数3bits(3,4,5)，扫描方向1bits(6)，航线边缘1bits(7)
	unsigned char	 Classification;//点分类
} RAW_POINT;

//利用点号可以获取LIDAR_POINT结构中未包含的原始信息
typedef struct
{
	DWORD			PntNO;			//点号		
	LIDAR_POINT		PntInfo;		//点信息

}POINT_RECORD;

struct	L_DOT
{
	long	x;
	long	y;
};

struct	D_DOT 
{
	double	x;
	double	y;
};

struct	L_3DOT
{
	long	x;
	long	y;
	long	z;
};


#ifndef	_D_RECT
#define _D_RECT
struct D_RECT
{
	double	XMax;
	double	XMin;
	double	YMax;
	double	YMin;
}; 
#endif

#ifndef _POINT2D_Def_
#define _POINT2D_Def_

struct POINT2D	{
	double x, y;
};

#endif

#ifndef _POINT3D_Def_
#define _POINT3D_Def_

struct POINT3D	{
	double X, Y, Z;
};

#endif

/*
typedef struct 
{
	double	XMax;
	double	XMin;
	double	YMax;
	double	YMin;
}D_RECT;*/

//typedef	BoundingBox2D	D_RECT;

typedef BoundingBox3D	D_CUBE;
/*
#ifndef _D_CUBE
#define _D_CUBE
typedef struct 
{
	double	XMax;
	double	XMin;
	double	YMax;
	double	YMin;
	double  ZMax;
	double	ZMin;
}D_CUBE;
#endif*/


typedef float F_POINT[3];
typedef long L_POINT[3];


#ifndef _LID_COLOR
#define _LID_COLOR
typedef unsigned char color[3];
#else
#pragma message("color已经定义!" )
#endif

#ifndef _LID_VEC
#define _LID_VEC
typedef float vec[3];
#else
#pragma message("vec已经定义!" )
#endif

typedef float point[3];
typedef float quat[4];



/*********************************************************
		y
		|
		|____x			OpenGL coordinate
	    /
	   /
	  z
**********************************************************/
struct F_NORMALVEC 
{
	float	x;
	float	y;
	float	z;
};

//赋值操作
#define MAX_DOUBLE(x)		( x=1.7E+308 )
#define MIN_DOUBLE(x)		( x=-1.7E+308 )	
#define MAX_FLOAT(x)		( x=3.3e33f )
#define MIN_FLOAT(x)		( x=-3.3e33f )
#define MAX_LONG(x)			( x=2147483647 )	
#define MIN_LONG(x)			( x=-2147483648 )
#define MAX_USHORT(x)		( x=65535 )
#define MIN_USHORT(x)		( x=0 )


//输出文件类型
enum  expFileTYPE 
{
	typeTif,			//tif格式
	typeDem				//VirtuoZo dem格式
};

//插值方式
enum Resample_Mode
{
	GridMaxValue	=0,		//网格内最大值
	GridMinValue	=1,		//网格内最小值
	GridMeanValue	=2,		//网格内平均值
	GridIDWValue	=3,		//网格内距离导数加权值
	TINLinear		=4		//TIN线性插值
};

#define		VZ_DEM_NodataValue		-99999
#define		Tif_DEM_NodataValue		-9999999



#pragma pack() // 取消1字节对齐方式
#endif