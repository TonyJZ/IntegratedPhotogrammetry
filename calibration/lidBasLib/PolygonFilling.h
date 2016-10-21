// PolygonFilling.h: interface for the CPolygonFilling class.
//	Create in 2008.11.29	多边形填充类
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYGONFILLING_H__C8ECBF7D_6164_4A11_83FF_37A89598508F__INCLUDED_)
#define AFX_POLYGONFILLING_H__C8ECBF7D_6164_4A11_83FF_37A89598508F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "lidBase.h"
//#include "BaseDataType.h "
#include "PointBaseDefinitons.h"

//扫描行X或Y坐标
struct	ScanLine
{
	double  start;	//起点
	double	end;	//终点
};

// struct POINT2D	{
// 	double x, y;
// };

class _BaseLib_ CPolygonFilling  
{
public:
	CPolygonFilling(POINT2D *polygon=NULL, int VertexNum=0);
	virtual ~CPolygonFilling();

	//设置需要填充的多边形，保证顶点顺序排列
	void	SetPolygon(POINT2D *polygon, int VertexNum);
	//沿X轴进行填充		2008.12.5  debug unsolved, (快速排序)
	bool	XAxisFilling(double	y0, ScanLine **pLine, int &num);
	//沿X轴填充矩形，处理像片，简化计算  必须保证设置的多边形为矩形
	bool	XAxisFillingRect(double	y0, ScanLine *pLine);
	
	void	GetPolyBoundary(double &XMax, double &XMin, double &YMax, double &YMin);
//	void	GetPolyBoundary(D_RECT &rect);
	
	bool	PointInPolygen(double x, double y);

private:
	void	CalBoundingBox();
	bool	Insert(double x1,double y1,double x2,double y2,double x3,double y3);
	bool	Online(double x1,double y1,double x2,double y2,double x3,double y3);

private:
	POINT2D		*m_polygon;
	int			m_VertexNum;
	//多边形包围盒
	double		m_Xmax;
	double		m_Xmin;
	double		m_Ymax;
	double		m_Ymin;
};

#endif // !defined(AFX_POLYGONFILLING_H__C8ECBF7D_6164_4A11_83FF_37A89598508F__INCLUDED_)
