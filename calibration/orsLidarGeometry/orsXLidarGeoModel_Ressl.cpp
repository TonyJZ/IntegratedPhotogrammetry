#include "StdAfx.h"
#include "orsXLidarGeoModel_Ressl.h"

#include "dpsMatrix.h"

orsXLidarGeoModel_Ressl::orsXLidarGeoModel_Ressl(bool bForRegister)
{
// 	m_XVec = NULL;
// 	m_YVec = NULL;
// 	m_vecLen = 0;
}


orsXLidarGeoModel_Ressl::~orsXLidarGeoModel_Ressl()
{
//	if(m_XVec)	delete[] m_XVec;	m_XVec=NULL;
//	if(m_YVec)	delete[] m_YVec;	m_YVec=NULL;
}

void orsXLidarGeoModel_Ressl::setSysParam(void *extParam)
{
	m_extParam = (orsLidResslParam*)extParam;

	memcpy(m_rotMatZ, m_extParam->rotMatZ, sizeof(double)*9);
	m_stripCenter = m_extParam->stripCenter;

//	matrix_transpose(3,3,m_rotMatZ, m_rotMatZ_Trans);
}

bool orsXLidarGeoModel_Ressl::setSRS(const char *hcsWkt, const char *vcsWkt)
{
	return false;
}

void orsXLidarGeoModel_Ressl::cal_Laserfootprint(orsPOINT3D *oriPts, orsPOINT3D *dstPts, int num)
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

	//计算Rroll*A
	double sinar,cosar;
	sinar = sin(m_extParam->aroll);
	cosar = cos(m_extParam->aroll);

	matRA[0]=1;	matRA[1]=m_extParam->ayaw;	matRA[2]=0;
	matRA[3]=0;	matRA[4]=cosar;				matRA[5]=-sinar;
	matRA[6]=0;	matRA[7]=sinar;				matRA[8]=cosar;

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

void orsXLidarGeoModel_Ressl::cal_Laserfootprint(LidPt_SurvInfo *pts, int num)
{
	double matRA[9], matTemp[9];
	double mA[9];	//最终的转换矩阵
	int i;

	//计算Rroll*A
	double sinar,cosar;
	sinar = sin(m_extParam->aroll);
	cosar = cos(m_extParam->aroll);

	matRA[0]=1;	matRA[1]=m_extParam->ayaw;	matRA[2]=0;
	matRA[3]=0;	matRA[4]=cosar;				matRA[5]=-sinar;
	matRA[6]=0;	matRA[7]=sinar;				matRA[8]=cosar;

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

void orsXLidarGeoModel_Ressl::calPDC_misalign(void *data, ors_int32 ptNum, 
	/*orsLidSysExtraParam_TYPE type, */void *extParam, double *pPDC)
{
	orsPOINT3D *pdata = (orsPOINT3D*)data;

	int i;

	double xs, ys, zs; 
	double a1,a2,a3,b1,b2,b3,c1,c2,c3;
	double sin_r,cos_r;
	double yaw;

	xs = ((orsLidResslParam*)extParam)->stripCenter.X;
	ys = ((orsLidResslParam*)extParam)->stripCenter.Y;
	zs = ((orsLidResslParam*)extParam)->stripCenter.Z;

	sin_r = sin(((orsLidResslParam*)extParam)->aroll);
	cos_r = cos(((orsLidResslParam*)extParam)->aroll);
	yaw = ((orsLidResslParam*)extParam)->ayaw;

	a1=((orsLidResslParam*)extParam)->rotMatZ[0];	a2=((orsLidResslParam*)extParam)->rotMatZ[1];	a3=((orsLidResslParam*)extParam)->rotMatZ[2];
	b1=((orsLidResslParam*)extParam)->rotMatZ[3];	b2=((orsLidResslParam*)extParam)->rotMatZ[4];	b3=((orsLidResslParam*)extParam)->rotMatZ[5];
	c1=((orsLidResslParam*)extParam)->rotMatZ[6];	c2=((orsLidResslParam*)extParam)->rotMatZ[7];	c3=((orsLidResslParam*)extParam)->rotMatZ[8];

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
		pXdiff[3] = ((-b1*sin_r+c1*cos_r)*b1+(-b1*cos_r-c1*sin_r)*c1)*(x-xs)+((-b1*sin_r+c1*cos_r)*b2+(-b1*cos_r-c1*sin_r)*c2)*(y-ys)+((-b1*sin_r+c1*cos_r)*b3+(-b1*cos_r-c1*sin_r)*c3)*(z-zs);
		pXdiff[4] = a1*b1*(x-xs)+a1*b2*(y-ys)+a1*b3*(z-zs);

		pYdiff[0] = 0.0;
		pYdiff[1] = 1.0;
		pYdiff[2] = 0.0;
		pYdiff[3] = ((-b2*sin_r+c2*cos_r)*b1+(-b2*cos_r-c2*sin_r)*c1)*(x-xs)+((-b2*sin_r+c2*cos_r)*b2+(-b2*cos_r-c2*sin_r)*c2)*(y-ys)+((-b2*sin_r+c2*cos_r)*b3+(-b2*cos_r-c2*sin_r)*c3)*(z-zs);
		pYdiff[4] = a2*b1*(x-xs)+a2*b2*(y-ys)+a2*b3*(z-zs);

		pZdiff[0] = 0.0;
		pZdiff[1] = 0.0;
		pZdiff[2] = 1.0;
		pZdiff[3] = ((-b3*sin_r+c3*cos_r)*b1+(-b3*cos_r-c3*sin_r)*c1)*(x-xs)+((-b3*sin_r+c3*cos_r)*b2+(-b3*cos_r-c3*sin_r)*c2)*(y-ys)+((-b3*sin_r+c3*cos_r)*b3+(-b3*cos_r-c3*sin_r)*c3)*(z-zs);
		pZdiff[4] = a3*b1*(x-xs)+a3*b2*(y-ys)+a3*b3*(z-zs);
	}

}