//#include "..\StdAfx.h"

#include <math.h>

#include "orsImageGeometry/orsIImageGeometry.h"
#include "orsSRS\orsISpatialReferenceService.h"
#include "orsMath\orsIMatrixService.h"
#include "dpsMatrix.h"
#include "orsMath\orsQuaternion.h"

#include "orsXTrajectory.h"

const double RAD2DEG = 57.295779513082320876798154814114;
const double DEG2RAD = 0.01745329251994329576923690768489;

orsXTrajectory::orsXTrajectory( bool bForRegister )
{
// 	m_rawCoordType = ORS_igctGeographic;
// 	m_userCoordType = ORS_igctGeographic;

	m_bTransform = false;
}


orsXTrajectory::~orsXTrajectory()
{

}

void orsXTrajectory::SetTangentPlane_AnchorPoint(double lat0, double long0, double h0)
{
// 	m_pTangentPlane->SetAnchorPoint(lat0, long0, h0);
// 
// 	m_Anchor_lat0=lat0;	m_Anchor_lon0=long0;	m_Anchor_h0=h0;
// 
// 	//NED北东地 -->  地心地固坐标
// 	//	RotateMat_NED2ECEF(lat0, long0, m_Rw);
// 	//	RotateMat_TPlane2WGS84_NED(lat0, long0, m_Rw);
// 
// 	//	RotateMat_WGS842TPlane_ENU1(lat0, long0, m_Rt);  //矩阵是否正确？2010.9.9 
// 
// 	//地心地固坐标 -->  东北天
// 	orsIMatrixService *matrixService;
// 
// 	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
// 	if( NULL == matrixService )	{
// //		msg->logPrint( ORS_LOG_ERROR, "Can not get matrixService" );
// 		return;
// 	}
// 
// 	matrixService->RotateMat_ECEF2ENU(lat0, long0, m_RRaw2User);
	m_posTrans->SetTangentPlane_AnchorPoint(lat0, long0, h0);
}

bool orsXTrajectory::Initialize(std::vector<orsPOSParam> *posVec)
{
	m_posVec = posVec;
	return true;
}

void orsXTrajectory::interpolationXYZ_Lagrange(double t, double t0, double t1, double v0[3], double v1[3], double v[3])
{
	ors_float64 dt1   = t - t0;
	ors_float64 dt0   = t1 - t;
	ors_float64 dt    = t1 - t0;

	v[0] = ( dt0*v0[0] + dt1*v1[0] ) / dt;
	v[1] = ( dt0*v0[1] + dt1*v1[1] ) / dt;
	v[2] = ( dt0*v0[2] + dt1*v1[2] ) / dt;
}

void orsXTrajectory::interpolationRotate_Lagrange(double t, double t0, double t1, double R0[9], double R1[9], double R[9])
{
	ors_float64 dt    = t1 - t0;

	ors_float64 dt1   = (t - t0)/dt;
	ors_float64 dt0   = (t1 - t)/dt;

	orsQuaternion q0, q1, qt;

	q0.setRotateMatrix(R0);
	q1.setRotateMatrix(R1);

	qt = orsQuaternion::slerp( dt1, q0, q1 );

	qt.getRotateMatrix(R);
}

//直接内插不做坐标转换
void orsXTrajectory::Interpolate_linear(orsPOSParam *pos_t, orsPOSParam *pos0, orsPOSParam *pos1)
{
//	orsPOSParam	orgp[2], dstp[2];
	double v0[3], v1[3], v[3];
	double t = pos_t->time;
	double t0 = pos0->time;
	double t1 = pos1->time;
//	double t2 = m_rawTraj[eIdx].Time;
//	double t3 = m_rawTraj[eIdx+1].Time;

	v0[0]=pos0->coord.X;
	v0[1]=pos0->coord.Y;
	v0[2]=pos0->coord.Z;

	v1[0]=pos1->coord.X;
	v1[1]=pos1->coord.Y;
	v1[2]=pos1->coord.Z;

	//线元素插值,直接对经纬度内插，忽略内插的误差，在极地和子午线附近会失败
	interpolationXYZ_Lagrange( t, t0, t1, v0, v1, v );
	pos_t->coord.X = v[0];
	pos_t->coord.Y = v[1];
	pos_t->coord.Z = v[2];

	double R0[9], R1[9], Rt[9];	//默认已经获得了旋转矩阵

	//角元素插值,直接在导航系下插值，小范围内地球曲率的影响可以忽略
	interpolationRotate_Lagrange( t, t0, t1, pos0->R, pos1->R, pos_t->R);
}

