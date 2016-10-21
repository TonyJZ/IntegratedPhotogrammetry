// #include "BaseDataType.h"
// 
// #include "GMatrix.h"
// #include "BaseDataType.h"
//#include "datatype.h"
#include "LidBaseDef.h"


double Average(double *x, int n);

int Gauss(double *A,double *b,int n);


//三维刚体变换   zj  2010.12.7
class CRigidTransform_3D
{
private:
	double m_a;	//罗德里格矩阵元素
	double m_b;
	double m_c;
	double m_R[9];

	double m_mx;
	double m_my;
	double m_mz;

	double m_xcSrc;
	double m_ycSrc;
	double m_zcSrc;
	double m_xcDst;
	double m_ycDst;
	double m_zcDst;

public:
	CRigidTransform_3D();

	void Initialize(double *xSrc, double *ySrc, double *zSrc, int n, double *xDst, double *yDst, double *zDst);
	

//	void GetResidual(double *xSrc, double *ySrc, int n, double *xDst, double *yDst, double *pVx, double *pVy, double *zScr=NULL, double *zDst=NULL, double *pVz=NULL );

//	void GetMeanError(double *mx, double *my, double *mz = NULL);

//	void GetParameter( double *a, double *b, double *c = NULL);

//	void GetCenters( double *xcSrc, double *ycSrc, double *xcDst, double *ycDst, double *zcScr=NULL, double *zcDst=NULL);

	void DoTransform(double *xSrc, double *ySrc, double *zSrc, int n, double *xDst, double *yDst, double *zDst);
};

//////////////////////////////////////////////////////////////////////////
// xDst - xcDst = a0*( xSrc-xcSrc) + a1*(ySrc-ycSrc)
// yDst - ycDst = b0*( xSrc-xcSrc) + b1*(ySrc-ycSrc)
//
//////////////////////////////////////////////////////////////////////////
class CAffineTransform
{
private:
	double m_a[2];
	double m_b[2];
	double m_c[2];

	double m_mx;
	double m_my;
	double m_mz;

	double m_xcSrc;
	double m_ycSrc;
	double m_zcSrc;
	double m_xcDst;
	double m_ycDst;
	double m_zcDst;

public:
	CAffineTransform();

	void Initialize(double *xSrc, double *ySrc, int n, double *xDst, double *yDst, double *zScr=NULL, double *zDst=NULL);

	void GetResidual(double *xSrc, double *ySrc, int n, double *xDst, double *yDst, double *pVx, double *pVy, double *zScr=NULL, double *zDst=NULL, double *pVz=NULL );

	void GetMeanError(double *mx, double *my, double *mz = NULL);

	void GetParameter( double *a, double *b, double *c = NULL);

	void GetCenters( double *xcSrc, double *ycSrc, double *xcDst, double *ycDst, double *zcScr=NULL, double *zcDst=NULL);

	void DoTransform(double *xSrc, double *ySrc, int n, double *xDst, double *yDst, double *zSrc=NULL, double *zDst=NULL);
};


//////////////////////////////////////////////////////////////////////////
// xDst - xcDst =  a0*( xSrc-xcSrc) + a1*(ySrc-ycSrc)
// yDst - ycDst = -a1*( xSrc-xcSrc) + a0*(ySrc-ycSrc)
//
//////////////////////////////////////////////////////////////////////////
class CSimilarityTransform
{
private:
	double m_a[2];

	double m_xcSrc;
	double m_ycSrc;
	double m_xcDst;
	double m_ycDst;

	double m_mx, m_my;

public:
	CSimilarityTransform();
	void Initialize(double *xSrc, double *ySrc, int n, double *xDst, double *yDst);
	void GetMeanError( double *mx, double *my);
	void GetParameter( double *a);

	void GetCenters( double *xcSrc, double *ycSrc, double *xcDst, double *ycDst);

}	;



//////////////////////////////////////////////////////////////////////////
// xDst - xcDst = a0 + a1*( xSrc-xcSrc) + a2*(ySrc-ycSrc) + a3*( xSrc-xcSrc)*(ySrc-ycSrc)
// yDst - ycDst = b0 + b1*( xSrc-xcSrc) + b2*(ySrc-ycSrc) + b3*( xSrc-xcSrc)*(ySrc-ycSrc)
//
//////////////////////////////////////////////////////////////////////////
class CBilinearTransform
{
private:
	double m_a[4];
	double m_b[4];

	double m_xcSrc;
	double m_ycSrc;
	double m_xcDst;
	double m_ycDst;

	double m_mx, m_my;

public:
	CBilinearTransform();

	void Initialize(POINT2D *ptSrc, int n, POINT2D *ptDst );
	void Initialize(double *xSrc, double *ySrc, int n, double *xDst, double *yDst);

	void GetMeanError( double *mx, double *my);

	// 取变换参数
	void GetParameter( double *xcSrc, double *ycSrc, double *xcDst, double *ycDst, double *a, double *b );

	// 取变换参数，中心点计入参数
	void GetParameter( double *a, double *b );
}	;


//////////////////////////////////////////////////////////////////////////
//				   l0*( xSrc-xcSrc) + l1*(ySrc-ycSrc) + l2
// xDst - xcDst = ------------------------------------------
//				   l6*( xSrc-xcSrc) + l7*(ySrc-ycSrc) + 1
//
//				   l3*( xSrc-xcSrc) + l4*(ySrc-ycSrc) + l5
// yDst - ycDst = ------------------------------------------
//				   l6*( xSrc-xcSrc) + l7*(ySrc-ycSrc) + 1
//////////////////////////////////////////////////////////////////////////
class CDLTTransform_2D
{
private:
	double m_l[9];

	double m_xcSrc;
	double m_ycSrc;
	double m_xcDst;
	double m_ycDst;

	double m_mx, m_my;

public:
	CDLTTransform_2D();

	void Initialize(POINT2D *ptSrc, int n, POINT2D *ptDst );
	void Initialize(double *xSrc, double *ySrc, int n, double *xDst, double *yDst);

	void GetMeanError( double *mx, double *my);
	void GetParameter( double *l);

	// 取变换参数
	void GetParameter( double *xcSrc, double *ycSrc, double *xcDst, double *ycDst, double *l );

	// 取变换参数，中心点计入参数
	void GetParameter( double *a, double *b );

}	;




//////////////////////////////////////////////////////////////////////////
//				   l0*( xSrc-xcSrc) + l1*(ySrc-ycSrc) + l2*(zSrc-zcSrc) + l3
// xDst - xcDst = -----------------------------------------------------------
//				   l8*( xSrc-xcSrc) + l9*(ySrc-ycSrc) + l10*(zSrc-zcSrc) + 1
//
//				   l4*( xSrc-xcSrc) + l5*(ySrc-ycSrc) + l6*(zSrc-zcSrc) + l7
// yDst - ycDst = -------------------------------------------------------------
//				   l8*( xSrc-xcSrc) + l9*(ySrc-ycSrc) + l10*(zSrc-zcSrc) + 1
//////////////////////////////////////////////////////////////////////////
class CDLTTransform_3D
{
private:
	double m_l[12];
	double m_mx, m_my;

	double m_xcSrc;
	double m_ycSrc;
	double m_zcSrc;
	double m_xcDst;
	double m_ycDst;
	double m_zcDst;

public:
	CDLTTransform_3D();

	void Initialize(double *xSrc, double *ySrc, int n, double *xDst, double *yDst);

	void GetMeanError( double *mx, double *my);
	void GetParameter( double *a);
}	;


float PointSetScale( int numOfPoint, POINT2D *src, POINT2D *dst );