#include "StdAfx.h"
#include "orsXLidarGeoModel_Traj.h"
#include "dpsMatrix.h"


orsXLidarGeoModel_Traj::orsXLidarGeoModel_Traj(bool bForRegister)
{
	m_pTraj = NULL;

	memset(m_leverarm, 0, sizeof(double)*3);
	memset(m_boreAngle, 0, sizeof(double)*3);
	matrix_ident(3, m_Rboresight);
	m_matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
}

orsXLidarGeoModel_Traj::~orsXLidarGeoModel_Traj()
{
	m_pTraj = NULL;
}

void orsXLidarGeoModel_Traj::setSysParam(void *extParam)
{
	m_pTraj = ((orsLidTrajAidParam*)extParam)->pTraj;
	
	memcpy(m_leverarm, ((orsLidTrajAidParam*)extParam)->lever_arm, sizeof(double)*3);
	memcpy(m_boreAngle, ((orsLidTrajAidParam*)extParam)->boresight_angle, sizeof(double)*3);

	m_matrixService->RotateMatrix_rph(m_boreAngle[0], m_boreAngle[1], m_boreAngle[2], 
		m_Rboresight);	//偏心角
}


bool orsXLidarGeoModel_Traj::setSRS(const char *hcsWkt, const char *vcsWkt)
{

	return false;
}

void orsXLidarGeoModel_Traj::cal_Laserfootprint(LidPt_SurvInfo *pts, int Num)
{
	double  Rn[9];		//imu旋转矩阵
	//double	Rn[9];		//局部切平面转ECEF

	//	double  Rl[9];		//imu坐标系到局部参考坐标系旋转矩阵
	double  vec_range[3];
	double  vec_LFrame[3];	//扫描坐标系
	double  vec_IFrame[3];	//imu坐标系
	double  vec_TFrame[3];	//局部切平面坐标系
	double	vec_ECEF[3];
	double Xgps, Ygps, Zgps, r, p, h;

	for(int i=0; i<Num; i++)
	{
		r=pts[i].POS_Info.r;
		p=pts[i].POS_Info.p;
		h=pts[i].POS_Info.h;

		Xgps=pts[i].POS_Info.coord.X;
		Ygps=pts[i].POS_Info.coord.Y;
		Zgps=pts[i].POS_Info.coord.Z;

		m_matrixService->RotateMatrix_rph(r, p, h, Rn);	//pos姿态

		vec_LFrame[0] = pts[i].vecRange[0];
		vec_LFrame[1] = pts[i].vecRange[1];
		vec_LFrame[2] = pts[i].vecRange[2];

		matrix_product(3, 3, 3, 1, m_Rboresight, vec_LFrame, vec_IFrame);

		vec_IFrame[0]+=m_leverarm[0];
		vec_IFrame[1]+=m_leverarm[1];
		vec_IFrame[2]+=m_leverarm[2];

		matrix_product(3, 3, 3, 1, Rn, vec_IFrame, vec_TFrame);

		pts[i].x = vec_TFrame[0]+Xgps;
		pts[i].y = vec_TFrame[1]+Ygps;
		pts[i].z = vec_TFrame[2]+Zgps;
	
	}
}

void orsXLidarGeoModel_Traj::cal_Laserfootprint(orsPOINT3D *oriPts, orsPOINT3D *dstPts, int num)
{
	assert(false);
}


void orsXLidarGeoModel_Traj::cal_RangeVec(LidPt_SurvInfo *pts, int Num)
{
	int i;
	double  Rn[9], Rn_T[9];		//imu旋转矩阵
	double  vec_LFrame[3];	//扫描坐标系
	//	double  vec_IFrame[3];	//imu坐标系
	double  vec_TFrame[3];	//局部切平面坐标系
	double vec_1[3];

	double Xs, Ys, Zs;
	double x, y, z;

	for(i=0; i<Num; i++)
	{
		Xs = pts[i].POS_Info.coord.X;
		Ys = pts[i].POS_Info.coord.Y;
		Zs = pts[i].POS_Info.coord.Z;

		x = pts[i].x;
		y = pts[i].y;
		z = pts[i].z;

		//pos姿态
		m_matrixService->RotateMatrix_rph(pts[i].POS_Info.r, pts[i].POS_Info.p, pts[i].POS_Info.h, Rn);	

		matrix_product33(Rn, m_leverarm, vec_1);

		vec_TFrame[0] = x - Xs - vec_1[0];
		vec_TFrame[1] = y - Ys - vec_1[1];
		vec_TFrame[2] = z - Zs - vec_1[2];

		//matrix_transpose(3, 3, Rn, Rn_T);

		matrix_transpose_product(3,3,3,1, Rn, vec_TFrame, vec_LFrame);

		matrix_transpose_product(3,3,3,1, m_Rboresight, vec_LFrame, pts[i].vecRange);

		//默认安置误差都为0
// 		pts[i].vecRange[0] = vec_LFrame[0];
// 		pts[i].vecRange[1] = vec_LFrame[1];
// 		pts[i].vecRange[2] = vec_LFrame[2];
	}
}

void orsXLidarGeoModel_Traj::calPDC_misalign(void *data, ors_int32 ptNum, 
	/*orsLidSysExtraParam_TYPE type, */void *extParam, double *pPDC)
{
	LidPt_SurvInfo *pdata = (LidPt_SurvInfo *)data;

	double *leverarm = ((orsLidTrajAidParam*)extParam)->lever_arm;
	double *boreAngle = ((orsLidTrajAidParam*)extParam)->boresight_angle;

//	double Rimu[9];
	double gps[3];

	double *pXdiff, *pYdiff, *pZdiff;
	double sinR,cosR, sinP, cosP, sinH, cosH;  //boresight angle, R, P, H
	double sin_phi, cos_phi, sin_omega, cos_omega, sin_kappa, cos_kappa;	//imu angle
	double p1, p2, p3;     //range vector

	for(int i=0; i<ptNum; i++)
	{
		pXdiff = pPDC + i*3*6;	//6个未知数的安置模型
		pYdiff = pXdiff + 6;
		pZdiff = pYdiff + 6;

		sin_phi = sin(pdata[i].POS_Info.r);
		cos_phi = cos(pdata[i].POS_Info.r);
		sin_omega = sin(pdata[i].POS_Info.p);
		cos_omega = cos(pdata[i].POS_Info.p);
		sin_kappa = sin(pdata[i].POS_Info.h);
		cos_kappa = cos(pdata[i].POS_Info.h);
		
		gps[0] = pdata[i].POS_Info.coord.X;
		gps[1] = pdata[i].POS_Info.coord.Y;
		gps[2] = pdata[i].POS_Info.coord.Z;

		p1 = pdata[i].vecRange[0];
		p2 = pdata[i].vecRange[1];
		p3 = pdata[i].vecRange[2];

		sinR = sin(boreAngle[0]);
		cosR = cos(boreAngle[0]);
		sinP = sin(boreAngle[1]);
		cosP = cos(boreAngle[1]);
		sinH = sin(boreAngle[2]);
		cosH = cos(boreAngle[2]);

		pXdiff[0] = cos_phi*cos_omega*((sinH*sinR+cosH*sinP*cosR)*p2+(sinH*cosR-cosH*sinP*sinR)*p3)+
			(-sin_kappa*cos_phi+cos_kappa*sin_omega*sin_phi)*((sinH*sinP*cosR-cosH*sinR)*p2+(-sinH*sinP*sinR
			-cosH*cosR)*p3)+(sin_kappa*sin_phi+cos_kappa*sin_omega*cos_phi)*(cosP*cosR*p2
			-cosP*sinR*p3);
		pXdiff[1] = cos_phi*cos_omega*(-cosH*sinP*p1+cosH*cosP*sinR*p2+cosH*cosP*cosR*p3)
			+(-sin_kappa*cos_phi+cos_kappa*sin_omega*sin_phi)*(-sinH*sinP*p1+sinH*cosP*sinR*p2+sinH*cosP*cosR*p3)+(sin_kappa*sin_phi+cos_kappa*sin_omega*cos_phi)*(-cosP*p1-sinP*sinR*p2-sinP*cosR*p3);
		pXdiff[2] = cos_phi*cos_omega*(-sinH*cosP*p1+(-sinH*sinP*sinR-cosH*cosR)*p2+(cosH*sinR-sinH*sinP*cosR)*p3)+(-sin_kappa*cos_phi+cos_kappa*sin_omega*sin_phi)*(cosH*cosP*p1+(-sinH*cosR+cosH*sinP*sinR)*p2+(sinH*sinR+cosH*sinP*cosR)*p3);
		pXdiff[3] = cos_phi*cos_omega;
		pXdiff[4] = -sin_kappa*cos_phi+cos_kappa*sin_omega*sin_phi;
		pXdiff[5] = sin_kappa*sin_phi+cos_kappa*sin_omega*cos_phi;

		pYdiff[0] = sin_kappa*cos_omega*((sinH*sinR+cosH*sinP*cosR)*p2+(sinH*cosR-cosH*sinP*sinR)*p3)+(cos_kappa*cos_phi+sin_kappa*sin_omega*sin_phi)*((sinH*sinP*cosR-cosH*sinR)*p2+(-sinH*sinP*sinR-cosH*cosR)*p3)+(sin_kappa*sin_omega*cos_phi-cos_kappa*sin_phi)*(cosP*cosR*p2-cosP*sinR*p3);
		pYdiff[1] = sin_kappa*cos_omega*(-cosH*sinP*p1+cosH*cosP*sinR*p2+cosH*cosP*cosR*p3)+(cos_kappa*cos_phi+sin_kappa*sin_omega*sin_phi)*(-sinH*sinP*p1+sinH*cosP*sinR*p2+sinH*cosP*cosR*p3)+(sin_kappa*sin_omega*cos_phi-cos_kappa*sin_phi)*(-cosP*p1-sinP*sinR*p2-sinP*cosR*p3);
		pYdiff[2] = sin_kappa*cos_omega*(-sinH*cosP*p1+(-sinH*sinP*sinR-cosH*cosR)*p2+(cosH*sinR-sinH*sinP*cosR)*p3)+(cos_kappa*cos_phi+sin_kappa*sin_omega*sin_phi)*(cosH*cosP*p1+(-sinH*cosR+cosH*sinP*sinR)*p2+(sinH*sinR+cosH*sinP*cosR)*p3);
		pYdiff[3] = sin_kappa*cos_omega;
		pYdiff[4] = cos_kappa*cos_phi+sin_kappa*sin_omega*sin_phi;
		pYdiff[5] = sin_kappa*sin_omega*cos_phi-cos_kappa*sin_phi;

		pZdiff[0] = -sin_omega*((sinH*sinR+cosH*sinP*cosR)*p2+(sinH*cosR-cosH*sinP*sinR)*p3)+cos_omega*sin_phi*((sinH*sinP*cosR-cosH*sinR)*p2+(-sinH*sinP*sinR-cosH*cosR)*p3)+cos_phi*cos_omega*(cosP*cosR*p2-cosP*sinR*p3);
		pZdiff[1] = -sin_omega*(-cosH*sinP*p1+cosH*cosP*sinR*p2+cosH*cosP*cosR*p3)+cos_omega*sin_phi*(-sinH*sinP*p1+sinH*cosP*sinR*p2+sinH*cosP*cosR*p3)+cos_phi*cos_omega*(-cosP*p1-sinP*sinR*p2-sinP*cosR*p3);
		pZdiff[2] = -sin_omega*(-sinH*cosP*p1+(-sinH*sinP*sinR-cosH*cosR)*p2+(cosH*sinR-sinH*sinP*cosR)*p3)+cos_omega*sin_phi*(cosH*cosP*p1+(-sinH*cosR+cosH*sinP*sinR)*p2+(sinH*sinR+cosH*sinP*cosR)*p3);
		pZdiff[3] = -sin_omega;
		pZdiff[4] = cos_omega*sin_phi;
		pZdiff[5] = cos_phi*cos_omega;
	}


}