#ifndef _My_LINE_STRUCT_H_
#define	_My_LINE_STRUCT_H_


#include "LidBaseDef.h"

//opengl中的坐标系，X轴正向向右，Y轴正向向上，Z轴正向向外
//笛卡尔坐标系，X轴正向向外，Y轴正向向右，Z轴正向向上
//当前坐标系，X轴正向向右，Y轴正向向外，Z轴正向向上

#define	Square(x)	((x)*(x))

class CMyGLLine
{
public:
	CMyGLLine(D_3DOT dStart, D_3DOT dEnd);
	CMyGLLine();
	~CMyGLLine();

	void	SetLine(D_3DOT Pnt1, D_3DOT Pnt2);
	D_3DOT	GetPntByX(double x);
	D_3DOT	GetPntByY(double y);
	D_3DOT	GetPntByZ(double z);

	bool	IsOnLine(D_3DOT pt);	//判断点是否位于线段上
	bool	IsIntersectWith(CMyGLLine &LineSeg1, D_3DOT &IntersectPnt);
	
	void	GetLineSegLength(double	&dis);
	void	GetLineDirectionParameter(double &ax,double &ay,double &az);
	void	GetStartPoint(D_3DOT* pnt);
	void	GetEndPoint(D_3DOT* pnt);

protected:
	void	CalculateVectorParameter();


private:
	D_3DOT		m_StartPnt;
	D_3DOT		m_EndPnt;		//两点式表示

	double		m_dDistance;	//向量的模与方向余弦表示
	double		m_cosx;
	double		m_cosy;
	double		m_cosz;
};


#endif