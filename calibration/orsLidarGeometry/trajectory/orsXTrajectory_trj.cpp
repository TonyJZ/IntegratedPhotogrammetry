//#include "..\StdAfx.h"

#include <math.h>

#include "orsImageGeometry/orsIImageGeometry.h"
#include "orsSRS\orsISpatialReferenceService.h"
#include "orsMath\orsIMatrixService.h"
#include "dpsMatrix.h"
#include "orsMath\orsQuaternion.h"

#include "orsXTrajectory_trj.h"

const double RAD2DEG = 57.295779513082320876798154814114;
const double DEG2RAD = 0.01745329251994329576923690768489;

orsXTrajectory_trj::orsXTrajectory_trj( bool bForRegister )
{
// 	m_rawCoordType = ORS_igctGeographic;
// 	m_userCoordType = ORS_igctGeographic;

	m_bTransform = false;
}


orsXTrajectory_trj::~orsXTrajectory_trj()
{

}

bool orsXTrajectory_trj::read_trjfile(const char *filename)
{
	FILE  *fp=NULL;
	bool bReturn=true;
	int numread=0;

	fp=fopen(filename, "rb");
	if(fp==0)
		return false;

	fread(m_TrjHeader.Recog, sizeof(char), 8, fp);
	fread(&(m_TrjHeader.Version), sizeof(ors_int32), 1, fp);
	fread(&(m_TrjHeader.HdrSize), sizeof(ors_int32), 1, fp);
	fread(&(m_TrjHeader.PosCnt), sizeof(ors_int32), 1, fp);
	fread(&(m_TrjHeader.PosSize), sizeof(ors_int32), 1, fp);
	fread(m_TrjHeader.Desc, sizeof(char), 79, fp);
	fread(&(m_TrjHeader.Quality), sizeof(ors_byte), 1, fp);
	fread(&(m_TrjHeader.BegTime), sizeof(double), 1, fp);
	fread(&(m_TrjHeader.EndTime), sizeof(double), 1, fp);
	fread(&(m_TrjHeader.OrigNbr), sizeof(ors_int32), 1, fp);
	fread(&(m_TrjHeader.Number), sizeof(ors_int32), 1, fp);
	fread(m_TrjHeader.VrtVideo, sizeof(char), 400, fp);
	fread(&(m_TrjHeader.VrtBeg), sizeof(double), 1, fp);
	fread(&(m_TrjHeader.VrtEnd), sizeof(double), 1, fp);
	fread(m_TrjHeader.FwdVideo, sizeof(char), 400, fp);
	fread(&(m_TrjHeader.FwdBeg), sizeof(double), 1, fp);
	fread(&(m_TrjHeader.FwdEnd), sizeof(double), 1, fp);

	if(m_TrjHeader.PosCnt<=0)
	{
		fclose(fp);
		return false;
	}

	TrajPos   trjData;
	
	while (numread < m_TrjHeader.PosCnt)
	{
		fread(&trjData, sizeof(TrajPos), 1, fp);
		//是否要进行角度转弧度？2014.2.11

		m_rawTraj.push_back(trjData);		
		numread++;
	}

	if(fp)	fclose(fp); fp=NULL;
	return bReturn;
}

bool orsXTrajectory_trj::Initialize( const orsChar *imageFileName )
{
// 	m_orgCoordSys = getSRService()->CreateSpatialReference();
// 	m_dstCoordSys = getSRService()->CreateSpatialReference();
// 
// 	m_pTangentPlane = getSRService()->CreateTangentPlane();

	m_posTrans = ORS_CREATE_OBJECT(orsIPOS, ORS_LIDARGEOMETRY_POS_WGS84);
	if( NULL == m_posTrans.get() )	{
		return false;
	}

	return read_trjfile( imageFileName );
}

orsISpatialReference* orsXTrajectory_trj::GetSpatialReference()
{
// 	switch( m_userCoordType )	{
// 	case ORS_igctGeographic:
// 		if( NULL == m_dstCoordSys.get() )	{
// 			m_dstCoordSys = getSRService()->CreateSpatialReference();
// 
// 			m_dstCoordSys->importFromWkt( 
// 				"GEOGCS[\"WGS 84\", \
// 				DATUM[\"WGS_1984\", \
// 				SPHEROID[\"WGS 84\", 6378137,298.257223563, \
// 				AUTHORITY[\"EPSG\",\"7030\"]], \
// 				TOWGS84[0,0,0,0,0,0,0], \
// 				AUTHORITY[\"EPSG\",\"6326\"]], \
// 				PRIMEM[\"Greenwich\", 0, \
// 				AUTHORITY[\"EPSG\",\"8901\"]], \
// 				UNIT[\"degree\",0.0174532925199433, \
// 				AUTHORITY[\"EPSG\",\"9108\"]], \
// 				AUTHORITY[\"EPSG\",\"4326\"]]");
// 
// 		}
// 
// 		return m_dstCoordSys.get(); 
// 		break;
// 	}
	return NULL;
}

bool orsXTrajectory_trj::RayCoord2UserCoord( double X, double Y, double Z,  double *xUser, double *yUser, double *zUser ) const
{
	*xUser = X;
	*yUser = Y;
	*zUser = Z;

	return true;
}


bool orsXTrajectory_trj::UserCoord2RayCoord( double xUser, double yUser, double zUser,  double *X, double *Y, double *Z ) const
{
	*X = xUser;
	*Y = yUser;
	*Z = zUser;

	return true;
}

bool orsXTrajectory_trj::SetSRS( const char *hcsWkt, const char *vcsWkt )
{
// 	m_orgCoordSys->importFromWkt( hcsWkt );
// 
// 	m_dstCoordSys->importFromWkt( vcsWkt );
// 
// 	if(m_coordTransform.get()==NULL)
// 		m_coordTransform = getSRService()->CreateCoordinateTransform();
// 
// 	m_coordTransform->Initialize( m_orgCoordSys.get(), m_dstCoordSys.get());

	m_posTrans->SetSRS(hcsWkt, vcsWkt);
	m_bTransform = true;

	return true;
}

void orsXTrajectory_trj::SetTangentPlane_AnchorPoint(double lat0, double long0, double h0)
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

void orsXTrajectory_trj::findTimeSlice(double t, int &sIdx, int &eIdx)
{
	int lLow=0, lHigh=m_TrjHeader.PosCnt-1;
	int lMid=0;


	if(t<m_rawTraj[lLow].Time)
	{
		sIdx=-1;
		eIdx=lLow;
		return;
	}
	if(t>m_rawTraj[lHigh].Time)
	{
		sIdx=lHigh;
		eIdx=lHigh+1;
		return;
	}

	//折半查找
	while(lLow<lHigh)	
	{
		if(lLow+1==lHigh)
		{
			sIdx=lLow;
			eIdx=lHigh;
			break;
		}

		lMid=(lLow+lHigh)/2;

		if(m_rawTraj[lMid].Time<=t)
		{
			lLow=lMid;

		}
		else
		{
			lHigh=lMid;
		}

	}
}

void orsXTrajectory_trj::interpolationXYZ_Lagrange(double t, double t0, double t1, double v0[3], double v1[3], double v[3])
{
	ors_float64 dt1   = t - t0;
	ors_float64 dt0   = t1 - t;
	ors_float64 dt    = t1 - t0;

	v[0] = ( dt0*v0[0] + dt1*v1[0] ) / dt;
	v[1] = ( dt0*v0[1] + dt1*v1[1] ) / dt;
	v[2] = ( dt0*v0[2] + dt1*v1[2] ) / dt;
}

void orsXTrajectory_trj::interpolationRotate_Lagrange(double t, double t0, double t1, double R0[9], double R1[9], double R[9])
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

// void orsXTrajectory_trj::interpolationRotate_Lagrange_dpsMatrix(double t, double t0, double t1, 
// 	double v0[3], double v1[3], double v[3])
// {
// 	ors_float64 dt    = t1 - t0;
// 
// 	ors_float64 dt1   = (t - t0)/dt;
// 	ors_float64 dt0   = (t1 - t)/dt;
// 
// 	slerp(v0, v1, dt1, v);
// 	//qt = orsQuaternion::slerp( dt1, q0, q1 );
// }

//直接内插不做坐标转换
void orsXTrajectory_trj::Interpolate_linear(double t, orsPOINT3D *gps_pos, double *R)
{
	int sIdx, eIdx;

	findTimeSlice(t, sIdx, eIdx);

	if(sIdx<0 || eIdx>m_rawTraj.size()-1)
	{//不进行外推
		assert(false);
		return;
	}

//	orsPOSParam	orgp[2], dstp[2];
	double v0[3], v1[3], v[3];
	double t0 = m_rawTraj[sIdx].Time;
	double t1 = m_rawTraj[eIdx].Time;
//	double t2 = m_rawTraj[eIdx].Time;
//	double t3 = m_rawTraj[eIdx+1].Time;

	v0[0]=m_rawTraj[sIdx].x;
	v0[1]=m_rawTraj[sIdx].y;
	v0[2]=m_rawTraj[sIdx].z;

	v1[0]=m_rawTraj[eIdx].x;
	v1[1]=m_rawTraj[eIdx].y;
	v1[2]=m_rawTraj[eIdx].z;

	//线元素插值,直接对经纬度内插，忽略内插的误差，在极地和子午线附近会失败
	interpolationXYZ_Lagrange( t, t0, t1, v0, v1, v );

	orsIMatrixService *matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	if( NULL == matrixService )	{
		//		msg->logPrint( ORS_LOG_ERROR, "Can not get matrixService" );
		return;
	}

	double R0[9], R1[9], Rt[9];

	matrixService->RotateMatrix_rph(m_rawTraj[sIdx].roll*DEG2RAD, m_rawTraj[sIdx].pitch*DEG2RAD, m_rawTraj[sIdx].heading*DEG2RAD, R0);
	matrixService->RotateMatrix_rph(m_rawTraj[eIdx].roll*DEG2RAD, m_rawTraj[eIdx].pitch*DEG2RAD, m_rawTraj[eIdx].heading*DEG2RAD, R1);
	//角元素插值,直接在导航系下插值，小范围内地球曲率的影响可以忽略
	interpolationRotate_Lagrange( t, t0, t1, R0, R1, Rt);

	gps_pos->X = v[0];	gps_pos->Y = v[1];	gps_pos->Z = v[2];
	memcpy(R, Rt, sizeof(double)*9);

// 	double eularAngle0[3], eularAngle1[3], eularAngle[3];
// 	eularAngle0[0] = m_rawTraj[sIdx].roll*DEG2RAD;
// 	eularAngle0[1] = m_rawTraj[sIdx].pitch*DEG2RAD;
// 	eularAngle0[2] = m_rawTraj[sIdx].heading*DEG2RAD;
// 
// 	eularAngle1[0] = m_rawTraj[eIdx].roll*DEG2RAD;
// 	eularAngle1[1] = m_rawTraj[eIdx].pitch*DEG2RAD;
// 	eularAngle1[2] = m_rawTraj[eIdx].heading*DEG2RAD;
// 
// 	interpolationRotate_Lagrange_dpsMatrix(t, t0, t1, eularAngle0, eularAngle1, eularAngle);
// 	matrixService->RotateMat_rph(eularAngle[0], eularAngle[1], eularAngle[2], R);

	////////////////////////////////////////////
	//导航数据转换
	
// 	double vDst[3];
// 	m_posTrans->POS_Transform(v, Rt, vDst, R);
// 
// 	*Xs = vDst[0];	*Ys = vDst[1];	*Zs = vDst[2];


// 	double v_trans[3];
// 	if(m_bTransform)
// 		coordinate_transform(v, v_trans);
// 	else
// 		memcpy(v_trans, v, sizeof(double)*3);
// 
// 	*Xs = v_trans[0];
// 	*Ys = v_trans[1];
// 	*Zs = v_trans[2];
// 
// 	if(m_bTransform)
// 	{
// 		attitude_transform(Rt, v[0], v[1], R);
// 	}
// 	else
// 		memcpy(R, Rt, sizeof(double)*9);
}

// void orsXTrajectory_trj::GetXsYsZs( double t, double *Xs, double *Ys, double *Zs )
// {
// 	int sIdx, eIdx;
// 
// 	findTimeSlice(t, sIdx, eIdx);
// 	
// 	if(sIdx<0 || eIdx>m_rawTraj.size()-1)
// 	{//不进行外推
// 		assert(false);
// 		return;
// 	}
// 	
// 	double v0[3], v1[3], v[3];
// 	double t0 = m_rawTraj[sIdx].Time;
// 	double t1 = m_rawTraj[eIdx].Time;
// 	
// 	v0[0]=m_rawTraj[sIdx].x;
// 	v0[1]=m_rawTraj[sIdx].y;
// 	v0[2]=m_rawTraj[sIdx].z;
// 
// 	v1[0]=m_rawTraj[eIdx].x;
// 	v1[1]=m_rawTraj[eIdx].y;
// 	v1[2]=m_rawTraj[eIdx].z;
// 
// 	interpolationXYZ_Lagrange( t, t0, t1, v0, v1, v );
// 
// 	double v_trans[3];
// 	if(m_bTransform)
// 		coordinate_transform(v, v_trans);
// 	else
// 		memcpy(v_trans, v, sizeof(double)*3);
// 
// 	*Xs = v_trans[0];
// 	*Ys = v_trans[1];
// 	*Zs = v_trans[2];
// }
// 
// void orsXTrajectory_trj::GetRotateMatrix( double t, double *R )
// {
// 	orsIMatrixService *matrixService;
// 
// 	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
// 	if( NULL == matrixService )	{
// 		//		msg->logPrint( ORS_LOG_ERROR, "Can not get matrixService" );
// 		return;
// 	}
// 
// 	int sIdx, eIdx;
// 	findTimeSlice(t, sIdx, eIdx);
// 
// 	if(sIdx<0 || eIdx>m_rawTraj.size()-1)
// 	{//不进行外推
// 		assert(false);
// 		return;
// 	}
// 
// 	double t0  = m_rawTraj[sIdx].Time;
// 	double t1  = m_rawTraj[eIdx].Time;
// 	double R0[9], R1[9], Rt[9];
// 
// 	matrixService->RotateMat_rph(m_rawTraj[sIdx].roll, m_rawTraj[sIdx].pitch, m_rawTraj[sIdx].heading, R0);
// 	matrixService->RotateMat_rph(m_rawTraj[eIdx].roll, m_rawTraj[eIdx].pitch, m_rawTraj[eIdx].heading, R1);
// 
// 	interpolationRotate_Lagrange( t, t0, t1, R0, R1, Rt);
// 
// 	if(m_bTransform)
// 	{
// 		attitude_transform(Rt, );
// 	}
// 	else
// 		memcpy(R, Rt, sizeof(double)*9);
// }

// void orsXTrajectory_trj::coordinate_transform(double pos_from[3], double pos_to[3])
// {
// 	orsIMatrixService *matrixService;
// 
// 	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
// 	if( NULL == matrixService )	{
// 		//		msg->logPrint( ORS_LOG_ERROR, "Can not get matrixService" );
// 		return;
// 	}
// 
// 	//线元素转换
// 	if(m_userCoordType == ORS_igctTangentPlane)
// 		m_pTangentPlane->Geographic2TangentPlane(pos_from[0], pos_from[1], pos_from[2], pos_to, pos_to+1, pos_to+2);
// 	else if(m_userCoordType == ORS_igctProjected)
// 	{//切平面转投影
// 		orsPOINT3D  pt1, pt2;
// 
// 		if(m_rawCoordType == ORS_igctGeographic)
// 		{
// 			pt1.X=pos_from[0]*RAD2DEG;	pt1.Y=pos_from[1]*RAD2DEG;	pt1.Z=pos_from[2]*RAD2DEG;	
// 		}
// 		else
// 		{
// 			pt1.X=pos_from[0];	pt1.Y=pos_from[1];	pt1.Z=pos_from[2];	
// 		}
// 		
// 		m_coordTransform->Transform( pt1, &pt2 );
// 
// 		pos_to[0]=pt2.X;	pos_to[1]=pt2.Y;	pos_to[2]=pt2.Z;
// 	}
// }

// void orsXTrajectory_trj::attitude_transform(double R_from[9], double lat, double lon, double R_to[9])
// {
// 	orsIMatrixService *matrixService;
// 
// 	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
// 	if( NULL == matrixService )	{
// 		//		msg->logPrint( ORS_LOG_ERROR, "Can not get matrixService" );
// 		return;
// 	}
// 
// 	//角元素转换
// 	double Rn[9], Rw[9], R1[9]/*, R2[9]*/;
// 	double R[9];
// 
// //	matrixService->RotateMat_rph(raw.roll, raw.pitch, raw.heading, Rn);
// 	memcpy(Rn, R_from, sizeof(double)*9);
// 	//	RotateMat_TPlane2WGS84_NED(lat, lon, Rw);	//当前导航系到地心地固系的旋转矩阵
// 	matrixService->RotateMat_NED2ECEF(lat, lon, Rw);
// 	//R=Rt*Rw*Rn
// 	matrix_product(3, 3, 3, 3, Rw, Rn, R1);
// 
// 	//转到局部切平面坐标系
// 	matrix_product(3, 3, 3, 3, m_RRaw2User, R1, R);
// 
// 	memcpy(R_to, R, sizeof(double)*9);
// 
// 	if(m_userCoordType == ORS_igctProjected)
// 	{//切平面转投影的补偿矩阵
// 	 //	"高斯-克吕格投影坐标系下POS角元素的转换方法", 袁修孝
// 		double Rcomp[9];
// 
// 		double sinB = sin(lat);
// 		double l = m_CentralMeridian - lon;	//L0-L
// 		
// 		Rcomp[0]=1;			Rcomp[1]=-l*sinB;		Rcomp[2]=0;
// 		Rcomp[3]=l*sinB;	Rcomp[4]=1;				Rcomp[5]=0;
// 		Rcomp[6]=0;			Rcomp[7]=0;				Rcomp[8]=1;
// 
// 		matrix_product(3, 3, 3, 3, Rcomp, R, R_to);
// 	}
// }
