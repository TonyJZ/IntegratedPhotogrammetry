#include "StdAfx.h"
#include "orsXLidarGeoModel_Jing.h"

#include "dpsMatrix.h"

orsXLidarGeoModel_Jing::orsXLidarGeoModel_Jing(bool bForRegister)
{
// 	m_XVec = NULL;
// 	m_YVec = NULL;
// 	m_vecLen = 0;
}


orsXLidarGeoModel_Jing::~orsXLidarGeoModel_Jing()
{
//	if(m_XVec)	delete[] m_XVec;	m_XVec=NULL;
//	if(m_YVec)	delete[] m_YVec;	m_YVec=NULL;
}

void orsXLidarGeoModel_Jing::setSysParam(void *extParam)
{
	m_extParam = (orsLidJingParam*)extParam;

	memcpy(m_rotMatZ, m_extParam->rotMatZ, sizeof(double)*9);
	m_stripCenter = m_extParam->stripCenter;

	matrix_transpose(3,3,m_rotMatZ, m_rotMatZ_T);
}

bool orsXLidarGeoModel_Jing::setSRS(const char *hcsWkt, const char *vcsWkt)
{
	return false;
}

void orsXLidarGeoModel_Jing::cal_Laserfootprint(orsPOINT3D *oriPts, orsPOINT3D *dstPts, int num)
{
	int i;
	double dX, dY, dZ;
	double phi, omega, kappa;

	phi = m_extParam->phi;
	omega = m_extParam->omega;
	kappa = m_extParam->kappa;

	for(i=0; i<num; i++)
	{
		//转换到条带坐标系下
		m_XCentered[0] =oriPts[i].X - m_stripCenter.X;
		m_XCentered[1] =oriPts[i].Y - m_stripCenter.Y;
		m_XCentered[2] =oriPts[i].Z - m_stripCenter.Z;

		matrix_product331(m_rotMatZ, m_XCentered, m_XStriped);

		//改正量
		dX = -kappa*m_XStriped[1] - phi*m_XStriped[2];
		dY = -omega*m_XStriped[2];
		dZ = omega*m_XStriped[1];

		m_XStriped[0] += dX;
		m_XStriped[1] += dY;
		m_XStriped[2] += dZ;

		//转回原坐标系
		matrix_product331(m_rotMatZ_T, m_XStriped, m_XCentered);
		dstPts[i].X = m_XCentered[0] + m_stripCenter.X;
		dstPts[i].Y = m_XCentered[1] + m_stripCenter.Y;
		dstPts[i].Z = m_XCentered[2] + m_stripCenter.Z;
	}
}

void orsXLidarGeoModel_Jing::cal_Laserfootprint(LidPt_SurvInfo *pts, int num)
{
	int i;
	double dX, dY, dZ;
	double phi, omega, kappa;

	phi = m_extParam->phi;
	omega = m_extParam->omega;
	kappa = m_extParam->kappa;
	
	for(i=0; i<num; i++)
	{
		//转换到条带坐标系下
		m_XCentered[0] =pts[i].x - m_stripCenter.X;
		m_XCentered[1] =pts[i].y - m_stripCenter.Y;
		m_XCentered[2] =pts[i].z - m_stripCenter.Z;

		matrix_product331(m_rotMatZ, m_XCentered, m_XStriped);

		//改正量
		dX = -kappa*m_XStriped[1] - phi*m_XStriped[2];
		dY = -omega*m_XStriped[2];
		dZ = omega*m_XStriped[1];

		m_XStriped[0] += dX;
		m_XStriped[1] += dY;
		m_XStriped[2] += dZ;

		//转回原坐标系
		matrix_product331(m_rotMatZ_T, m_XStriped, m_XCentered);
		pts[i].x = m_XCentered[0] + m_stripCenter.X;
		pts[i].y = m_XCentered[1] + m_stripCenter.Y;
		pts[i].z = m_XCentered[2] + m_stripCenter.Z;
	}
}

void orsXLidarGeoModel_Jing::calPDC_misalign(void *data, ors_int32 ptNum, 
	/*orsLidSysExtraParam_TYPE type, */void *extParam, double *pPDC)
{
	orsPOINT3D *pdata = (orsPOINT3D*)data;

	int i;

	double Xc, Yc, Zc;  //条带中心坐标
	double a1,a2,a3,b1,b2,b3,c1,c2,c3;
// 	double sin_r,cos_r;
// 	double yaw;

	Xc = ((orsLidJingParam*)extParam)->stripCenter.X;
	Yc = ((orsLidJingParam*)extParam)->stripCenter.Y;
	Zc = ((orsLidJingParam*)extParam)->stripCenter.Z;

// 	sin_r = sin(((orsLidJingParam*)extParam)->aroll);
// 	cos_r = cos(((orsLidJingParam*)extParam)->aroll);
// 	yaw = ((orsLidJingParam*)extParam)->ayaw;

	a1=((orsLidJingParam*)extParam)->rotMatZ[0];	a2=((orsLidJingParam*)extParam)->rotMatZ[1];	a3=((orsLidJingParam*)extParam)->rotMatZ[2];
	b1=((orsLidJingParam*)extParam)->rotMatZ[3];	b2=((orsLidJingParam*)extParam)->rotMatZ[4];	b3=((orsLidJingParam*)extParam)->rotMatZ[5];
	c1=((orsLidJingParam*)extParam)->rotMatZ[6];	c2=((orsLidJingParam*)extParam)->rotMatZ[7];	c3=((orsLidJingParam*)extParam)->rotMatZ[8];

	double *pXdiff, *pYdiff, *pZdiff;
	double Xl, Yl, Zl;
	for(i=0; i<ptNum; i++)
	{
		Xl = pdata[i].X;
		Yl = pdata[i].Y;
		Zl = pdata[i].Z;

		pXdiff = pPDC + i*3*3;   //3 unknowns
		pYdiff = pXdiff + 3;
		pZdiff = pYdiff + 3;

		//phi, omega, kappa
		pXdiff[0] = a1*(-c1*(Xl-Xc)-c2*(Yl-Yc)-c3*(Zl-Zc));
		pXdiff[1] = b1*(-c1*(Xl-Xc)-c2*(Yl-Yc)-c3*(Zl-Zc))+c1*(b1*(Xl-Xc)+b2*(Yl-Yc)+b3*(Zl-Zc));
		pXdiff[2] = a1*(-b1*(Xl-Xc)-b2*(Yl-Yc)-b3*(Zl-Zc));

		pYdiff[0] = a2*(-c1*(Xl-Xc)-c2*(Yl-Yc)-c3*(Zl-Zc));
		pYdiff[1] = b2*(-c1*(Xl-Xc)-c2*(Yl-Yc)-c3*(Zl-Zc))+c2*(b1*(Xl-Xc)+b2*(Yl-Yc)+b3*(Zl-Zc));
		pYdiff[2] = a2*(-b1*(Xl-Xc)-b2*(Yl-Yc)-b3*(Zl-Zc));
		
		pZdiff[0] = a3*(-c1*(Xl-Xc)-c2*(Yl-Yc)-c3*(Zl-Zc));
		pZdiff[1] = b3*(-c1*(Xl-Xc)-c2*(Yl-Yc)-c3*(Zl-Zc))+c3*(b1*(Xl-Xc)+b2*(Yl-Yc)+b3*(Zl-Zc));
		pZdiff[2] = a3*(-b1*(Xl-Xc)-b2*(Yl-Yc)-b3*(Zl-Zc));
	}

}