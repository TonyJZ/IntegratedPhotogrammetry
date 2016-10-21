#include "StdAfx.h"
#include "orsXLidarGeoModel_Yuan.h"

#include "dpsMatrix.h"

orsXLidarGeoModel_Yuan::orsXLidarGeoModel_Yuan(bool bForRegister)
{
// 	m_XVec = NULL;
// 	m_YVec = NULL;
// 	m_vecLen = 0;
}


orsXLidarGeoModel_Yuan::~orsXLidarGeoModel_Yuan()
{
//	if(m_XVec)	delete[] m_XVec;	m_XVec=NULL;
//	if(m_YVec)	delete[] m_YVec;	m_YVec=NULL;
}

void orsXLidarGeoModel_Yuan::setSysParam(void *extParam)
{
	m_extParam = (orsLidYuanParam*)extParam;

	memcpy(m_rotMatZ, m_extParam->rotMatZ, sizeof(double)*9);
	m_stripCenter = m_extParam->stripCenter;

//	matrix_transpose(3,3,m_rotMatZ, m_rotMatZ_Trans);
}

bool orsXLidarGeoModel_Yuan::setSRS(const char *hcsWkt, const char *vcsWkt)
{
	return false;
}

void orsXLidarGeoModel_Yuan::cal_Laserfootprint(orsPOINT3D *oriPts, orsPOINT3D *dstPts, int num)
{
	double matRA[9], matTemp[9];
	double mA[9];	//最终的转换矩阵
	int i;

// 	if(m_vecLen < num*3)
// 	{
// 		m_vecLen = num*3;
// 		if(m_XVec)	delete[] m_XVec;
// 		if(m_YVec)	delete[] m_YVec;
// 
// 		m_XVec = new double[m_vecLen];
// 		m_YVec = new double[m_vecLen];
// 	}

	matRA[0]=1;	matRA[1]=-m_extParam->kappa;	matRA[2]=0;
	matRA[3]=0;	matRA[4]=1;						matRA[5]=-m_extParam->omega;
	matRA[6]=0;	matRA[7]=m_extParam->omega;		matRA[8]=1;

	matrix_transpose_product(3,3,3,3, m_rotMatZ, matRA, matTemp);
	matrix_product33(matTemp, m_rotMatZ, mA);

	//Y = AX+S+a
	for(i=0; i<num; i++)
	{
		m_XVec[0] =oriPts[i].X - m_stripCenter.X;
		m_XVec[1] =oriPts[i].Y - m_stripCenter.Y;
		m_XVec[2] =oriPts[i].Z - m_stripCenter.Z;

		matrix_product331(mA, m_XVec, m_YVec);

		dstPts[i].X = m_YVec[0]+m_stripCenter.X+m_extParam->ax;
		dstPts[i].Y = m_YVec[1]+m_stripCenter.Y+m_extParam->ay;
		dstPts[i].Z = m_YVec[2]+m_stripCenter.Z+m_extParam->az;
	}
}

void orsXLidarGeoModel_Yuan::cal_Laserfootprint(LidPt_SurvInfo *pts, int num)
{
	double matRA[9], matTemp[9];
	double mA[9];	//最终的转换矩阵
	int i;


	matRA[0]=1;	matRA[1]=-m_extParam->kappa;	matRA[2]=0;
	matRA[3]=0;	matRA[4]=1;						matRA[5]=-m_extParam->omega;
	matRA[6]=0;	matRA[7]=m_extParam->omega;		matRA[8]=1;

	matrix_transpose_product(3,3,3,3, m_rotMatZ, matRA, matTemp);
	matrix_product33(matTemp, m_rotMatZ, mA);

	//Y = AX+S+a
	for(i=0; i<num; i++)
	{
		m_XVec[0] =pts[i].x - m_stripCenter.X;
		m_XVec[1] =pts[i].y - m_stripCenter.Y;
		m_XVec[2] =pts[i].z - m_stripCenter.Z;

		matrix_product331(mA, m_XVec, m_YVec);

		pts[i].x = m_YVec[0]+m_stripCenter.X+m_extParam->ax;
		pts[i].y = m_YVec[1]+m_stripCenter.Y+m_extParam->ay;
		pts[i].z = m_YVec[2]+m_stripCenter.Z+m_extParam->az;
	}
}

void orsXLidarGeoModel_Yuan::calPDC_misalign(void *data, ors_int32 ptNum, 
	/*orsLidSysExtraParam_TYPE type, */void *extParam, double *pPDC)
{
	orsPOINT3D *pdata = (orsPOINT3D *)data;

	int i;

	double xs, ys, zs; 
	double a1,a2,a3,b1,b2,b3,c1,c2,c3;
// 	double sin_r,cos_r;
// 	double yaw;

	xs = ((orsLidYuanParam*)extParam)->stripCenter.X;
	ys = ((orsLidYuanParam*)extParam)->stripCenter.Y;
	zs = ((orsLidYuanParam*)extParam)->stripCenter.Z;

// 	sin_r = sin(((orsLidYuanParam*)extParam)->aroll);
// 	cos_r = cos(((orsLidYuanParam*)extParam)->aroll);
// 	yaw = ((orsLidYuanParam*)extParam)->ayaw;

	a1=((orsLidYuanParam*)extParam)->rotMatZ[0];	a2=((orsLidYuanParam*)extParam)->rotMatZ[1];	a3=((orsLidYuanParam*)extParam)->rotMatZ[2];
	b1=((orsLidYuanParam*)extParam)->rotMatZ[3];	b2=((orsLidYuanParam*)extParam)->rotMatZ[4];	b3=((orsLidYuanParam*)extParam)->rotMatZ[5];
	c1=((orsLidYuanParam*)extParam)->rotMatZ[6];	c2=((orsLidYuanParam*)extParam)->rotMatZ[7];	c3=((orsLidYuanParam*)extParam)->rotMatZ[8];

	double *pXdiff, *pYdiff, *pZdiff;
	double x, y, z;
	for(i=0; i<ptNum; i++)
	{
		x = pdata[i].X;
		y = pdata[i].Y;
		z = pdata[i].Z;

		pXdiff = pPDC + i*3*5;   //5 unknowns
		pYdiff = pXdiff + 5;
		pZdiff = pYdiff + 5;

		pXdiff[0] = 1.0;
		pXdiff[1] = 0.0;
		pXdiff[2] = 0.0;
		pXdiff[3] = -a1*b1*(x-xs)-a1*b2*(y-ys)-a1*b3*(z-zs);
		pXdiff[4] = (c1*b2-b1*c2)*(y-ys)+(c1*b3-b1*c3)*(z-zs);

		pYdiff[0] = 0.0;
		pYdiff[1] = 1.0;
		pYdiff[2] = 0.0;
		pYdiff[3] = -a2*b1*(x-xs)-a2*b2*(y-ys)-a2*b3*(z-zs);
		pYdiff[4] = (b1*c2-c1*b2)*(x-xs)+(c2*b3-b2*c3)*(z-zs);

		pZdiff[0] = 0.0;
		pZdiff[1] = 0.0;
		pZdiff[2] = 1.0;
		pZdiff[3] =-a3*b1*(x-xs)-a3*b2*(y-ys)-a3*b3*(z-zs);
		pZdiff[4] =(b1*c3-c1*b3)*(x-xs)+(b2*c3-c2*b3)*(y-ys);
	}

}