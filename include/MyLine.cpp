#include "stdafx.h"
#include "Myline.h"
#include "math.h"


CMyGLLine::CMyGLLine(D_3DOT dStart, D_3DOT dEnd)
{
	memcpy(&m_StartPnt,&dStart,sizeof(D_3DOT));
	memcpy(&m_EndPnt,&dEnd,sizeof(D_3DOT));

	CalculateVectorParameter();
}

CMyGLLine::CMyGLLine()
{

}
CMyGLLine::~CMyGLLine()
{

}

void CMyGLLine::CalculateVectorParameter()
{
	m_dDistance = sqrt(Square(m_StartPnt.x-m_EndPnt.x)+Square(m_StartPnt.y-m_EndPnt.y)+Square(m_StartPnt.z-m_EndPnt.z));
	m_cosx = (m_EndPnt.x-m_StartPnt.x)/m_dDistance;
	m_cosy = (m_EndPnt.y-m_StartPnt.y)/m_dDistance;
	m_cosz = (m_EndPnt.z-m_StartPnt.z)/m_dDistance;
}

void CMyGLLine::SetLine(D_3DOT Pnt1, D_3DOT Pnt2)
{
	memcpy(&m_StartPnt,&Pnt1,sizeof(D_3DOT));
	memcpy(&m_EndPnt,&Pnt2,sizeof(D_3DOT));

	CalculateVectorParameter();
}

//x=x1+cosx*t	y=y1+cosy*t		z=z1+cosz*t
D_3DOT	CMyGLLine::GetPntByX(double x)
{
	D_3DOT Pos;
	double	t;

	t=(x-m_StartPnt.x)/m_cosx;
	Pos.x=x;
	Pos.y=m_StartPnt.y+m_cosy*t;
	Pos.z=m_StartPnt.z+m_cosz*t;

	return	Pos;
}

D_3DOT CMyGLLine::GetPntByY(double y)
{
	D_3DOT Pos;
	double	t;

	t=(y-m_StartPnt.y)/m_cosy;
	Pos.x=m_StartPnt.x+m_cosx*t;
	Pos.y=y;
	Pos.z=m_StartPnt.z+m_cosz*t;

	return	Pos;
}

D_3DOT CMyGLLine::GetPntByZ(double z)
{
	D_3DOT Pos;
	double	t;

	t=(z-m_StartPnt.z)/m_cosz;
	Pos.x=m_StartPnt.x+m_cosx*t;;
	Pos.y=m_StartPnt.y+m_cosy*t;
	Pos.z=z;

	return	Pos;
}

bool CMyGLLine::IsOnLine(D_3DOT pt)
{
	double	d1,d2;
	d1=sqrt(Square(m_StartPnt.x-pt.x)+Square(m_StartPnt.y-pt.y)+Square(m_StartPnt.z-pt.z));
	d2=sqrt(Square(pt.x-m_EndPnt.x)+Square(pt.y-m_EndPnt.y)+Square(pt.z-m_EndPnt.z));

	if(fabs(d1+d2-m_dDistance)<1e-6)
		return	true;
	else
		return	false;
}

//两条线段是否有交点
bool CMyGLLine::IsIntersectWith(CMyGLLine &LineSeg1, D_3DOT &IntersectPnt)
{
	bool	bReturn=true;
	bool	bOverlap=false;
	double	bx, by, bz;
	double	dMomentX, dMomentY, dMomentZ;	//叉积
	double	t;	//直线的参数方程中的参数
	D_3DOT	Line1PortS,Line1PortE;

	LineSeg1.GetLineDirectionParameter(bx,by,bz);
	dMomentX = m_cosy*bz-m_cosz*by;
	dMomentY = m_cosz*bx-m_cosx*bz;
	dMomentZ = m_cosx*by-m_cosy*bx;

	LineSeg1.GetStartPoint(&Line1PortS);
	LineSeg1.GetEndPoint(&Line1PortE);

	if(fabs(dMomentX)<1e-6&&
		fabs(dMomentY)<1e-6&&
		fabs(dMomentZ)<1e-6)
	{//平行
		if(IsOnLine(Line1PortS))
		{//重合
			bReturn=true;
			IntersectPnt.x=Line1PortS.x;
			IntersectPnt.y=Line1PortS.y;
			IntersectPnt.z=Line1PortS.z;
			goto	FUNC_END;
		}
		else if(IsOnLine(Line1PortE))
		{//重合
			bReturn=true;
			IntersectPnt.x=Line1PortE.x;
			IntersectPnt.y=Line1PortE.y;
			IntersectPnt.z=Line1PortE.z;
			goto	FUNC_END;
		}
		else
		{
			bReturn=false;
			goto	FUNC_END;
		}
	}

	
//	t=(m_StartPnt.x-Line1PortS.x)/(bx-m_cosx);
//	t=(m_StartPnt.y-Line1PortS.y)/(by-m_cosy);
//	t=(m_StartPnt.z-Line1PortS.z)/(bz-m_cosz);

	

	if(fabs(m_cosx)>1e-6)
	{
		IntersectPnt.x = (bx*m_cosy*m_StartPnt.x-m_cosx*by*Line1PortS.x+m_cosx*bx*(Line1PortS.y-m_StartPnt.y))/
					 (bx*m_cosy-m_cosx*by);
		t = (IntersectPnt.x-m_StartPnt.x)/m_cosx;
		IntersectPnt.y = m_StartPnt.y + m_cosy*t;
		IntersectPnt.z = m_StartPnt.z+m_cosz*t;
	}
	else if(fabs(m_cosy)>1e-6)
	{
		IntersectPnt.y=(m_cosx*by*m_StartPnt.y-m_cosy*bx*Line1PortS.y+m_cosy*by*(Line1PortS.x-m_StartPnt.x))
						/(m_cosx*by-m_cosy*bx);
		t = (IntersectPnt.y-m_StartPnt.y)/m_cosy; 
		IntersectPnt.x = m_StartPnt.x + m_cosx*t;
		IntersectPnt.z = m_StartPnt.z+m_cosz*t;
	}
//	else if (fabs(m_cosz)>1e-6)
//	{
		
//	}
	else
	{
		bReturn=false;
		goto	FUNC_END;
	}
	

	if((IntersectPnt.x-m_StartPnt.x)*(IntersectPnt.x-m_EndPnt.x)>0)
	{
		bReturn=false;
		goto	FUNC_END;
	}
	if((IntersectPnt.x-Line1PortS.x)*(IntersectPnt.x-Line1PortE.x)>0)
	{
		bReturn=false;
		goto	FUNC_END;
	}
	

FUNC_END:
	return	bReturn;
}

void CMyGLLine::GetLineSegLength(double &dis)
{
	dis = m_dDistance;
}

void CMyGLLine::GetLineDirectionParameter(double &ax,double &ay,double &az)
{
	ax = m_cosx;
	ay = m_cosy;
	az = m_cosz;
}

void CMyGLLine::GetStartPoint(D_3DOT* pnt)
{
	memcpy(pnt,&m_StartPnt,sizeof(D_3DOT));
}

void CMyGLLine::GetEndPoint(D_3DOT* pnt)
{
	memcpy(pnt,&m_EndPnt,sizeof(D_3DOT));
}