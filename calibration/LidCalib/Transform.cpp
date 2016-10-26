
#include "stdafx.h"
#include <math.h>

#define USING_MFC

/*#include "BaseDataType.h"*/
#include "Transform.h"
#include "GMatrix.h"
#include "matrix.h "



double Average(double *x, int n)
{
	double sum;

	sum = 0;
	for(int i=0; i<n; i++)
		sum += *x++;

	return sum / n;

}


POINT2D Average(POINT2D *pts, int n)
{
	POINT2D sum;

	sum.x = sum.y = 0;
	for(int i=0; i<n; i++)
	{
		sum.x += pts->x;
		sum.y += pts->y;
		pts++;
	}

	sum.x /= n;
	sum.y /= n;

	return sum;
}


CRigidTransform_3D::CRigidTransform_3D()
{
}

//直接解法，罗德里格矩阵参考李海鸿论文“绝对定向的一种线性解法”，与“摄影测量原理”略有不同
void CRigidTransform_3D::Initialize(double *xSrc, double *ySrc, double *zSrc, int n, double *xDst, double *yDst, double *zDst)
{
	m_xcSrc = Average( xSrc, n);
	m_ycSrc = Average( ySrc, n);

	m_xcDst = Average( xDst, n);
	m_ycDst = Average( yDst, n);

	if( zSrc )
		m_zcSrc = Average( zSrc, n);

	if( zDst )
		m_zcDst = Average( zDst, n);

	ASSERT(n>2);
	
	////////////////////////////////////
	int i, j, k;
	double a[2];
	double *A=NULL, *L=NULL;
	double *p;
	double xd, yd, zd, xs, ys, zs; 

	L=new double[3*n];
	A=new double[3*3*n];

	for(i=0; i<n; i++)
	{
		xd=xDst[i]-m_xcDst;
		yd=yDst[i]-m_ycDst;
		zd=zDst[i]-m_zcDst;
		xs=xSrc[i]-m_xcSrc;
		ys=ySrc[i]-m_ycSrc;
		zs=zSrc[i]-m_zcSrc;

		p=L+i*3;
		p[0]=xd - xs;
		p[1]=yd - ys;
		p[2]=zd - zs;

		p=A+i*3*3;
		p[0]=0;
		p[1]=0.5*(zd+zs);
		p[2]=0.5*(yd+ys);
		p[3]=p[1];
		p[4]=0;
		p[5]=-0.5*(xd+xs);
		p[6]=-p[2];
		p[7]=p[5];
		p[8]=0;
	}
	double X[3];

	dgelsy_driver(A, L, X, n*3, 3, 1);
	m_a=X[0];	m_b=X[1];	m_c=X[2];

	double S1[9], S1_inv[9], S2[9];

	//S1:  I+S,   S2:  I-S   R=(I+S)^-1(I-S)
	S1[0]=1;			S1[1]=-0.5*m_c;		S1[2]=-0.5*m_b;
	S1[3]=0.5*m_c;	S1[4]=1;				S1[5]=-0.5*m_a;
	S1[6]=0.5*m_b;	S1[7]=0.5*m_a;		S1[8]=1;

	S2[0]=1;			S2[1]=0.5*m_c;		S2[2]=0.5*m_b;
	S2[3]=-0.5*m_c; S2[4]=1;				S2[5]=0.5*m_a;
	S2[6]=-0.5*m_b; S2[7]=-0.5*m_a;		S2[8]=1;

	matrix_invert(3, S1, S1_inv);
	matrix_product(3,3,3,3, S1_inv, S2, m_R);

	//测试转换是否正确
	for(i=0; i<n; i++)
	{
		double x, y, z, dx, dy, dz;
		DoTransform(xSrc+i, ySrc+i, zSrc+i, 1, &x, &y, &z);
		dx=x-xDst[i];
		dy=y-yDst[i];
		dz=z-zDst[i];
	}

	if(L)	delete L; L=NULL;
	if(A)	delete A; A=NULL;
}

void CRigidTransform_3D::DoTransform(double *xSrc, double *ySrc, double *zSrc, int n, double *xDst, double *yDst, double *zDst)
{
	int k;
	double dx, dy, dz;

	for( k=0; k<n ; k++)
	{
		dx = (xSrc[k] - m_xcSrc);	dy = (ySrc[k] - m_ycSrc);  dz=(zSrc[k] - m_zcSrc);

		xDst[k] = m_xcDst + ( m_R[0]*dx + m_R[1]*dy + m_R[2]*dz );
		yDst[k] = m_ycDst + ( m_R[3]*dx + m_R[4]*dy + m_R[5]*dz );
		zDst[k] = m_zcDst + ( m_R[6]*dx + m_R[7]*dy + m_R[8]*dz );
			
	}
}

//////////////////////////////////////////////////////////////////////////
// xDst - xcDst = a0*( xSrc-xcSrc) + a1*(ySrc-ycSrc)
// yDst - ycDst = b0*( xSrc-xcSrc) + b1*(ySrc-ycSrc)
//
//////////////////////////////////////////////////////////////////////////
CAffineTransform::CAffineTransform()
{
}


void CAffineTransform::GetParameter( double *a, double *b, double *c)
{
	for( int i=0; i<2; i++)	
		a[i] = m_a[i], b[i] = m_b[i];

	if( c ) {
		for( int i=0; i<2; i++)	
			c[i] = m_c[i];
	}
};


void CAffineTransform::GetCenters( double *xcSrc, double *ycSrc, double *xcDst, double *ycDst, double *zcSrc, double *zcDst ) 
{
	*xcSrc = m_xcSrc;	*ycSrc = m_ycSrc;
	*xcDst = m_xcDst;	*ycDst = m_ycDst;

	if( zcSrc ) {
		*zcSrc = m_zcSrc;
		*zcDst = m_zcDst;
	}
}




void CAffineTransform::Initialize(double *xSrc, double *ySrc,int n, double *xDst, double *yDst, double *zSrc, double *zDst)
{
	/////////////////////////////
	m_xcSrc = Average( xSrc, n);
	m_ycSrc = Average( ySrc, n);
	
	m_xcDst = Average( xDst, n);
	m_ycDst = Average( yDst, n);
	
	if( zSrc )
		m_zcSrc = Average( zSrc, n);
	
	if( zDst )
		m_zcDst = Average( zDst, n);

	////////////////////////////////////
	int i,j, k;
	double a[2];

	GMatrix	AA(2,2), AL(2,1), BL(2,1), CL(2,1);

	AA.Zero();	AL.Zero();	BL.Zero();	CL.Zero();

	for( k=0; k<n ; k++)
	{
		a[0] = xSrc[k]-m_xcSrc;	a[1] = ySrc[k]-m_ycSrc;

		for( i=0; i<2; i++)
		{
			for( j=0; j<2; j++)
				AA[i][j] +=a[i]*a[j];

			AL[i][0] += a[i]* (xDst[k] - m_xcDst);
			BL[i][0] += a[i]* (yDst[k] - m_ycDst);

			if( zSrc )
				CL[i][0] += a[i]* ( (zDst[k] - m_zcDst ) - ( zSrc[k] - m_zcSrc ) );
		}
	}

	GMatrix AI = AA.Inv();
	
	GMatrix x(2,1);

	x = AI * AL;
	m_a[0] = x[0][0];	m_a[1] = x[1][0];	//	m_a[2] = x[2][0];

	x = AI * BL;
	m_b[0] = x[0][0];	m_b[1] = x[1][0];	//	m_b[2] = x[2][0];

	if( zSrc ) {
		x = AI * CL;
		m_c[0] = x[0][0];	m_c[1] = x[1][0];	//	m_c[2] = x[2][0];
	}

	m_mx = m_my = m_mz = 0;
	double vx, vy, vz, dx, dy;
	for( k=0; k<n ; k++)
	{
		dx = (xSrc[k] - m_xcSrc);	dy = (ySrc[k] - m_ycSrc);

		vx = (xDst[k]-m_xcDst) - ( m_a[0]*dx + m_a[1]*dy );
		vy = (yDst[k]-m_ycDst) - ( m_b[0]*dx + m_b[1]*dy );

		m_mx += vx*vx;
		m_my += vy*vy;

		if( zSrc ) {
			vz = zDst[k] - (  m_zcDst  + zSrc[k] - m_zcSrc + m_c[0]* dx + m_c[1]* dy );
			m_mz += vz*vz;
		}
	}

	if( n > 3 )	{
		m_mx = sqrt( m_mx / (n-3) );
		m_my = sqrt( m_my / (n-3) );
		m_mz = sqrt( m_mz / (n-3) );
	}
	else
		m_mx = m_my = m_mz = 0;

}

void CAffineTransform::GetResidual(double *xSrc, double *ySrc, int n, double *xDst, double *yDst, double *pVx, double *pVy, double *zSrc, double *zDst, double *pVz )
{
	int k;
	double dx, dy;

	for( k=0; k<n ; k++)
	{
		dx = (xSrc[k] - m_xcSrc);	dy = (ySrc[k] - m_ycSrc);

		*pVx++ = (xDst[k]-m_xcDst) - ( m_a[0]*dx + m_a[1]*dy );
		*pVy++ = (yDst[k]-m_ycDst) - ( m_b[0]*dx + m_b[1]*dy );

		if( pVz && zSrc && zDst ) 
			*pVz++ = zDst[k] - (  m_zcDst  + zSrc[k] - m_zcSrc + m_c[0]* dx + m_c[1]* dy );
	}
}



void CAffineTransform::GetMeanError(double *mx,double *my, double *mz)
{
	*mx = m_mx;
	*my = m_my;

	if( mz ) 
		*mz = m_mz;
}

void CAffineTransform::DoTransform(double *xSrc, double *ySrc, int n, double *xDst, double *yDst, double *zSrc, double *zDst)
{
	int k;
	double dx, dy;
	
	for( k=0; k<n ; k++)
	{
		dx = (xSrc[k] - m_xcSrc);	dy = (ySrc[k] - m_ycSrc);
		
		xDst[k] = m_xcDst + ( m_a[0]*dx + m_a[1]*dy );
		yDst[k] = m_ycDst + ( m_b[0]*dx + m_b[1]*dy );
		
		if( zSrc && zDst ) 
			zDst[k] =  m_zcDst  + zSrc[k] - m_zcSrc + m_c[0]* dx + m_c[1]* dy ;
	}
}



//////////////////////////////////////////////////////////////////////////
// xDst - xcDst =  a0*( xSrc-xcSrc) + a1*(ySrc-ycSrc)
// yDst - ycDst = -a1*( xSrc-xcSrc) + a0*(ySrc-ycSrc)
//
//////////////////////////////////////////////////////////////////////////
CSimilarityTransform::CSimilarityTransform()
{
}


void CSimilarityTransform::Initialize(double *xSrc, double *ySrc, int n, double *xDst, double *yDst)
{
	m_xcSrc = Average( xSrc, n);
	m_ycSrc = Average( ySrc, n);
	
	m_xcDst = Average( xDst, n);
	m_ycDst = Average( yDst, n);

	//////////////////////////////////////////////////////////////////////////
	
	int i,j, k;
	double a[2], b[2], dx, dy, lx, ly;

	GMatrix	AA(2,2), AL(2);

	AA.Zero();	AL.Zero();
	for( k=0; k<n ; k++)
	{
		dx = (xSrc[k] - m_xcSrc);	dy = (ySrc[k] - m_ycSrc);

		a[0] = dx;	a[1] = dy;	lx = xDst[k] - m_xcDst;
		b[1] =-dx;	b[0] = dy;	ly = yDst[k] - m_ycDst;

		for( i=0; i<2; i++)
		{
			for( j=0; j<2; j++)
				AA[i][j] += a[i]*a[j] + b[i]*b[j];

			AL[i][0] += a[i]*lx + b[i]*ly;
		}
	}

	GMatrix AI = AA.Inv();

	GMatrix x(4,1);
		
	x = AI * AL;
	m_a[0] = x[0][0];	m_a[1] = x[1][0];

	m_mx = m_my = 0;
	double vx, vy;
	for( k=0; k<n ; k++)
	{
		dx = (xSrc[k] - m_xcSrc);	dy = (ySrc[k] - m_ycSrc);

		vx = (xDst[k] - m_xcDst) - ( m_a[0]*dx + m_a[1]*dy );
		vy = (yDst[k] - m_ycDst) - (-m_a[1]*dx + m_a[0]*dy );

		m_mx += vx*vx;
		m_my += vy*vy;
	}
	
	if( n>2 )	{
		m_mx = sqrt( m_mx / (n-2) );
		m_my = sqrt( m_my / (n-2) );
	}
	else	{
		m_mx = m_my = 0;
	}

}



void CSimilarityTransform::GetParameter( double *a)
{
	for( int i=0; i<4; i++)
		a[i] = m_a[i];
}

void CSimilarityTransform::GetMeanError( double *mx,double *my )
{
	*mx = m_mx;
	*my = m_my;
}


void CSimilarityTransform::GetCenters( double *xcSrc, double *ycSrc, double *xcDst, double *ycDst ) 
{
	*xcSrc = m_xcSrc;	*ycSrc = m_ycSrc;
	*xcDst = m_xcDst;	*ycDst = m_ycDst;
}

//////////////////////////////////////////////////////////////////////////
// xDst - xcDst = a0 + a1*( xSrc-xcSrc) + a2*(ySrc-ycSrc) + a3*( xSrc-xcSrc)*(ySrc-ycSrc)
// yDst - ycDst = b0 + b1*( xSrc-xcSrc) + b2*(ySrc-ycSrc) + b3*( xSrc-xcSrc)*(ySrc-ycSrc)
//
//////////////////////////////////////////////////////////////////////////
CBilinearTransform::CBilinearTransform()
{
}


void CBilinearTransform::Initialize( POINT2D *ptSrc, int n, POINT2D *ptDst )
{
	POINT2D centerSrc, centerDst;

	/////////////////////////////
	centerSrc = Average( ptSrc, n );
	centerDst = Average( ptDst, n );

	m_xcSrc = centerSrc.x;	m_ycSrc = centerSrc.y;
	m_xcDst = centerDst.x;	m_ycDst = centerDst.y;
	
	////////////////////////////////////
	int i,j, k;
	double a[4];//, b[4];都使用a[]来计算By YCY 2008.5.13

	GMatrix	AA(4,4), AL(4,1), BL(4,1);

	AA.Zero();	AL.Zero();	BL.Zero();

	for( k=0; k<n ; k++)
	{
		a[0] = 1;	a[1] = ptSrc[k].x - m_xcSrc;	a[2] = ptSrc[k].y-m_ycSrc;	a[3] = a[1] * a[2];

		for( i=0; i<4; i++)
		{
			for( j=0; j<4; j++)
				AA[i][j] += a[i]*a[j];

			AL[i][0] += a[i]* ( ptDst[k].x - m_xcDst );
//			BL[i][0] += b[i]* ( ptDst[k].y - m_ycDst );把b[i]替换为a[i] By YCY 2008.5.13
            BL[i][0] += a[i]* ( ptDst[k].y - m_ycDst );
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	GMatrix AI = AA.Inv();
	
	GMatrix x(4,1);

	x = AI * AL;
	for( i=0; i<4; i++)
		m_a[i] = x[i][0];

	x = AI * BL;
	for( i=0; i<4; i++)
		m_b[i] = x[i][0];


	//////////////////////////////////////////////////////////////////////////

	m_mx = m_my = 0;
	double vx, vy,  dx, dy;
	for( k=0; k<n ; k++)
	{
		dx = (ptSrc[k].x - m_xcSrc);	dy = (ptSrc[k].y - m_ycSrc);

		vx = ( ptDst[k].x - m_xcDst) - ( m_a[0] + m_a[1]*dx + m_a[2]*dy + m_a[3]*dx*dy );
		vy = ( ptDst[k].y - m_ycDst) - ( m_b[0] + m_b[1]*dx + m_b[2]*dy + m_b[3]*dx*dy );

		m_mx += vx*vx;
		m_my += vy*vy;
	}

	if( n >4 )	{
		m_mx = sqrt( m_mx / (n-4) );
		m_my = sqrt( m_my / (n-4) );
	}
	else
		m_mx = m_my = 0;

}

void CBilinearTransform::Initialize(double *xSrc, double *ySrc, int n, double *xDst, double *yDst)
{
	/////////////////////////////
	m_xcSrc = Average( xSrc, n);
	m_ycSrc = Average( ySrc, n);
	
	m_xcDst = Average( xDst, n);
	m_ycDst = Average( yDst, n);
	
	////////////////////////////////////
	int i,j, k;
	double a[4], b[4];

	GMatrix	AA(4,4), AL(4,1), BL(4,1);

	AA.Zero();	AL.Zero();	BL.Zero();

	for( k=0; k<n ; k++)
	{
		a[0] = 1;	a[1] = xSrc[k]-m_xcSrc;	a[2] = ySrc[k]-m_ycSrc;	a[3] = a[1] * a[2];

		for( i=0; i<4; i++)
		{
			for( j=0; j<4; j++)
				AA[i][j] += a[i]*a[j];

			AL[i][0] += a[i]* ( xDst[k] - m_xcDst );
			BL[i][0] += b[i]* ( yDst[k] - m_ycDst );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	GMatrix AI = AA.Inv();
	
	GMatrix x(4,1);

	x = AI * AL;
	for( i=0; i<4; i++)
		m_a[i] = x[i][0];

	x = AI * BL;
	for( i=0; i<4; i++)
		m_b[i] = x[i][0];


	//////////////////////////////////////////////////////////////////////////

	m_mx = m_my = 0;
	double vx, vy, dx, dy;
	for( k=0; k<n ; k++)
	{
		dx = (xSrc[k] - m_xcSrc);	dy = (ySrc[k] - m_ycSrc);

		vx = (xDst[k] - m_xcDst) - ( m_a[0] + m_a[1]*dx + m_a[2]*dy + m_a[3]*dx*dy );
		vy = (yDst[k] - m_ycDst) - ( m_b[0] + m_b[1]*dx + m_b[2]*dy + m_b[3]*dx*dy );

		m_mx += vx*vx;
		m_my += vy*vy;
	}
	m_mx = sqrt( m_mx / (n-4) );
	m_my = sqrt( m_my / (n-4) );
}



void CBilinearTransform::GetMeanError( double *mx,double *my )
{
	*mx = m_mx;
	*my = m_my;
}



// 取变换参数
void CBilinearTransform::GetParameter( double *xcSrc, double *ycSrc, double *xcDst, double *ycDst, double *a, double *b )
{
	int i;

	for( i=0; i<4; i++)
		a[i] = m_a[i];

	for( i=0; i<4; i++)
		b[i] = m_b[i];

	*xcSrc = m_xcSrc;	*ycSrc = m_ycSrc;
	*xcDst = m_xcDst;	*ycDst = m_ycDst;
}


// 取变换参数，中心点计入参数
void CBilinearTransform::GetParameter( double *a, double *b )
{
	int i;

	for( i=0; i<4; i++)
		a[i] = m_a[i];	
	
	for( i=0; i<4; i++)
		b[i] = m_b[i];

	// xDst - xcDst = a0 + a1*( xSrc-xcSrc) + a2*(ySrc-ycSrc) + a3*( xSrc-xcSrc)*(ySrc-ycSrc)
	//              = a0 + a1*xSrc - a1*xcSrc + a2*ySrc - a2*ycSrc + a3*xSrc*ySrc 
	//					 - a3*xSrc*ycSrc - a3*xcSrc*ySrc + a3*xcSrc*ycSrc;
	a[0] += m_xcDst - a[1]*m_xcSrc - a[2]*m_ycSrc + a[3]*m_xcSrc*m_ycSrc;
	a[1] -= a[3]*m_ycSrc;
	a[2] -= a[3]*m_xcSrc;

	b[0] += m_ycDst - b[1]*m_xcSrc - b[2]*m_ycSrc + b[3]*m_xcSrc*m_ycSrc;
	b[1] -= b[3]*m_ycSrc;
	b[2] -= b[3]*m_xcSrc;
}





//////////////////////////////////////////////////////////////////////////
//				   l0*( xSrc-xcSrc) + l1*(ySrc-ycSrc) + l2
// xDst - xcDst = ------------------------------------------
//				   l6*( xSrc-xcSrc) + l7*(ySrc-ycSrc) + 1
//
//				   l3*( xSrc-xcSrc) + l4*(ySrc-ycSrc) + l5
// yDst - ycDst = ------------------------------------------
//				   l6*( xSrc-xcSrc) + l7*(ySrc-ycSrc) + 1
//
//				   
// dxDst*(l6*dxSrc + l7*dySrc + 1) = l0*dxSrc + l1*dySrc + l2
//
// dyDst*(l6*dxSrc + l7*dySrc + 1) = l3*dxSrc + l4*dySrc + l5
//
//////////////////////////////////////////////////////////////////////////
CDLTTransform_2D::CDLTTransform_2D()
{
	m_l[8] = 1;
}



void CDLTTransform_2D::Initialize(POINT2D *ptSrc, int n, POINT2D *ptDst )
{
	POINT2D centerSrc, centerDst;

	/////////////////////////////
	centerSrc = Average( ptSrc, n );
	centerDst = Average( ptDst, n );

	m_xcSrc = centerSrc.x;	m_ycSrc = centerSrc.y;
	m_xcDst = centerDst.x;	m_ycDst = centerDst.y;
	
	////////////////////////////////////
	int i,j, k;
	double a[8], b[8], dxSrc, dySrc, dxDst, dyDst, lx, ly;

	GMatrix	AA(8,8), AL(8,1);

	AA.Zero();	AL.Zero();

	memset(a, 0, 8*sizeof(double) );
	memset(b, 0, 8*sizeof(double) );

	for( k=0; k<n ; k++)
	{ 
		dxSrc = ptSrc[k].x - m_xcSrc;		dySrc = ptSrc[k].y - m_ycSrc;
		dxDst = ptDst[k].x - m_xcDst;		dyDst = ptDst[k].y - m_ycDst;

		a[0] = dxSrc;	a[1] = dySrc;	a[2] = 1;
		a[6] = -dxDst*dxSrc;	a[7] = -dxDst*dySrc;		lx = dxDst;

		b[3] = dxSrc;	b[4] = dySrc;	b[5] = 1;
		b[6] = -dyDst*dxSrc;	b[7] = -dyDst*dySrc;		ly = dyDst;

		for( i=0; i<8; i++)
		{
			for( j=0; j<8; j++)
				AA[i][j] += a[i]*a[j] + b[i]*b[j];

			AL[i][0] += a[i]* lx + b[i]* ly;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	GMatrix AI = AA.Inv();
	
	GMatrix x(8,1);

	x = AI * AL;
	for( i=0; i<8; i++)
		m_l[i] = x[i][0];

	//////////////////////////////////////////////////////////////////////////

	m_mx = m_my = 0;
	double vx, vy, w;
	for( k=0; k<n ; k++)
	{
		dxSrc = ptSrc[k].x - m_xcSrc;		dySrc = ptSrc[k].y - m_ycSrc;
		dxDst = ptDst[k].x - m_xcDst;		dyDst = ptDst[k].y - m_ycDst;
		
		w = m_l[6]*dxSrc + m_l[7]*dySrc + 1;

		vx = dxDst - ( m_l[0]*dxSrc + m_l[1]*dySrc + m_l[2] ) / w;
		vy = dyDst - ( m_l[3]*dxSrc + m_l[4]*dySrc + m_l[5] ) / w;

		m_mx += vx*vx;
		m_my += vy*vy;
	}
	m_mx = sqrt( m_mx / (n-2) );
	m_my = sqrt( m_my / (n-2) );

	//////////////////////////////////////////////////////////////////////////
	double l[9];
	GetParameter( l );

	m_mx = m_my = 0;
	vx, vy, w;
	for( k=0; k<n ; k++)
	{
		dxSrc = ptSrc[k].x;		dySrc = ptSrc[k].y;
		dxDst = ptDst[k].x;		dyDst = ptDst[k].y;
		
		w = l[6]*dxSrc + l[7]*dySrc + l[8];

		vx = dxDst - ( l[0]*dxSrc + m_l[1]*dySrc + l[2] ) / w;
		vy = dyDst - ( l[3]*dxSrc + m_l[4]*dySrc + l[5] ) / w;

		m_mx += vx*vx;
		m_my += vy*vy;
	}
	m_mx = sqrt( m_mx / (n-2) );
	m_my = sqrt( m_my / (n-2) );

}



void CDLTTransform_2D::Initialize(double *xSrc, double *ySrc, int n, double *xDst, double *yDst)
{
	/////////////////////////////
	m_xcSrc = Average( xSrc, n);
	m_ycSrc = Average( ySrc, n);
	
	m_xcDst = Average( xDst, n);
	m_ycDst = Average( yDst, n);
	
	////////////////////////////////////
	int i,j, k;
	double a[8], b[8], dxSrc, dySrc, dxDst, dyDst, lx, ly;

	GMatrix	AA(8,8), AL(8,1);

	AA.Zero();	AL.Zero();

	for( k=0; k<n ; k++)
	{ 
		dxSrc = xSrc[k]-m_xcSrc;		dySrc = ySrc[k]-m_ycSrc;
		dxDst = xDst[k]-m_xcDst;		dyDst = yDst[k]-m_ycDst;

		a[0] = dxSrc;	a[1] = dySrc;	a[2] = 1;
		a[2] = dxSrc;	a[4] = dySrc;	a[5] = 1;
		a[6] = -dxDst*dxSrc;	a[7] = -dxDst*dySrc;		lx = dxDst;

		memcpy( b, a, 5*sizeof(double) );
		b[6] = -dyDst*dxSrc;	b[7] = -dyDst*dySrc;		ly = dyDst;

		for( i=0; i<8; i++)
		{
			for( j=0; j<8; j++)
				AA[i][j] += a[i]*a[j] + b[i]*b[j];

			AL[i][0] += a[i]* lx + b[i]* ly;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	GMatrix AI = AA.Inv();
	
	GMatrix x(8,1);

	x = AI * AL;
	for( i=0; i<8; i++)
		m_l[i] = x[i][0];

	//////////////////////////////////////////////////////////////////////////

	m_mx = m_my = 0;
	double vx, vy, w;
	for( k=0; k<n ; k++)
	{
		dxSrc = xSrc[k]-m_xcSrc;		dySrc = ySrc[k]-m_ycSrc;
		dxDst = xDst[k]-m_xcDst;		dyDst = yDst[k]-m_ycDst;
		
		w = m_l[6]*dxSrc + m_l[7]*dySrc + 1;

		vx = dxDst - ( m_l[0]*dxSrc + m_l[1]*dySrc + m_l[2] ) / w;
		vy = dyDst - ( m_l[3]*dxSrc + m_l[4]*dySrc + m_l[6] ) / w;

		m_mx += vx*vx;
		m_my += vy*vy;
	}
	m_mx = sqrt( m_mx / (n-2) );
	m_my = sqrt( m_my / (n-2) );

}



void CDLTTransform_2D::GetMeanError( double *mx,double *my )
{
	*mx = m_mx;
	*my = m_my;
}



void CDLTTransform_2D::GetParameter(  double *xcSrc, double *ycSrc, double *xcDst, double *ycDst, double *l )
{
	for( int i=0; i<9; i++)
		l[i] = m_l[i];

	*xcSrc = m_xcSrc;	*ycSrc = m_ycSrc;
	*xcDst = m_xcDst;	*ycDst = m_ycDst;
}



// 取变换参数，中心点计入参数
void CDLTTransform_2D::GetParameter( double *l )
{
	int i;

	for( i=0; i<9; i++)
		l[i] = m_l[i];


//////////////////////////////////////////////////////////////////////////
//				   l0*( xSrc-xcSrc) + l1*(ySrc-ycSrc) + l2
// xDst - xcDst = ------------------------------------------
//				   l6*( xSrc-xcSrc) + l7*(ySrc-ycSrc) + 1
// ==>
//		           l0*xSrc + l1*ySrc + (l2 - l0*xcSrc - l1*ycSrc)
// xDst = xcDst + -------------------------------------------------
//		           l6*xSrc + l7*ySrc + (1  - l6*xcSrc - l7*ycSrc)=l8
//
// ==>
//
//		   ( l0 + l6*xcDst) * *xSrc + ( l1 + l7*xcDst)* ySrc + (l2 - l0*xcSrc - l1*ycSrc)+ xcDst*l8 
// xDst = -----------------------------------------------------------------------------------------------------------------
//		   l6*xSrc + l7*ySrc + (1  - l6*xcSrc - l7*ycSrc)
	
	l[8] = 1 - l[6]*m_xcSrc - l[7]*m_ycSrc;
	
	// 不能置后， 否则l[0],l[1]已被修改
	l[2] += l[8]*m_xcDst - l[0]*m_xcSrc - l[1]*m_ycSrc;
	l[0] += l[6]*m_xcDst;
	l[1] += l[7]*m_xcDst;

	l[5] += l[8]*m_ycDst - l[3]*m_xcSrc - l[4]*m_ycSrc;
	l[3] += l[6]*m_ycDst;
	l[4] += l[7]*m_ycDst;
}


float PointSetScale( int numOfPoint, POINT2D *src, POINT2D *dst )
{

	if( numOfPoint == 0 )
		return 0;
	
	//////////////////////////////////////////////////////////////////////////
	// 计算影像比例
	int i;
	
	double xcSrc, ycSrc, xcDst, ycDst;
	double xrSrc, yrSrc, xrDst, yrDst, rSrc, rDst;
	
	xcSrc = ycSrc = xcDst = ycDst = 0;
	xrSrc = yrSrc = xrDst = yrDst = 0;
	for( i=0; i<numOfPoint; i++ )
	{
		xcSrc += src[i].x;	ycSrc += src[i].y;
		xcDst += dst[i].x;	ycDst += dst[i].y;
		
		xrSrc += src[i].x*src[i].x;		yrSrc += src[i].y*src[i].y;
		xrDst += dst[i].x*dst[i].x;		yrDst += dst[i].y*dst[i].y;
	}
	
	rSrc = (xrSrc + yrSrc) - ( xcSrc*xcSrc + ycSrc*ycSrc) / numOfPoint;
	rDst = (xrDst + yrDst) - ( xcDst*xcDst + ycDst*ycDst) / numOfPoint;
	
	return  sqrt( rSrc  / rDst );
}