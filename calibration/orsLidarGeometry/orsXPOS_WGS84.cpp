#include "StdAfx.h"
#include "orsXPOS_WGS84.h"

#include "orsSRS\orsISpatialReferenceService.h"
#include "dpsMatrix.h"
#include "orsMath/orsQuaternion.h"
#include "math.h"

double Normalize(double v[])
{
	double result;

	result = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	return result;
}

void Cal_RotateMat(double vecOrg[3], double vecDst[3], double R[9])
{
	double rot_axis[3];

	//叉积得到旋转轴
	rot_axis[0] = vecOrg[1] * vecDst[2] - vecOrg[2] * vecDst[1];
	rot_axis[1] = vecOrg[2] * vecDst[0] - vecOrg[0] * vecDst[2];
	rot_axis[2] = vecOrg[0] * vecDst[1] - vecOrg[1] * vecDst[0];

	double rot_angle;
	double dotProd;
	//点积计算旋转角
	dotProd = vecOrg[0] * vecDst[0] + vecOrg[1] * vecDst[1] + vecOrg[2] * vecDst[2];

	rot_angle = acos(dotProd/Normalize(vecOrg)/Normalize(vecDst));

	orsQuaternion quat;
	quat.setRotate(rot_angle, rot_axis[0], rot_axis[1], rot_axis[2]);
	quat.getRotateMatrix(R);
}

orsXPOS_WGS84::orsXPOS_WGS84( bool bForRegister )
{
	m_rawCoordType = ORS_igctGeographic;
	m_userCoordType = ORS_igctGeographic;

	m_orgCoordSys = getSRService()->CreateSpatialReference();
	m_dstCoordSys = getSRService()->CreateSpatialReference();

	m_pTangentPlane = getSRService()->CreateTangentPlane();
	m_wgs84=getSRService()->CreateEllipsoid();

	m_matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
}

orsXPOS_WGS84::~orsXPOS_WGS84()
{

}

const double RAD2DEG = 57.295779513082320876798154814114;
const double DEG2RAD = 0.01745329251994329576923690768489;

bool orsXPOS_WGS84::SetRayCoordType( orsImageGeometryCoordSysTYPE coordType ) 
{
	m_rawCoordType = coordType;
	return true;
}

bool orsXPOS_WGS84::SetUserCoordType( orsImageGeometryCoordSysTYPE coordType )
{
	m_userCoordType = coordType;

	return true;
}

void orsXPOS_WGS84::SetTangentPlane_AnchorPoint(double lat0, double long0, double h0)
{
	m_pTangentPlane->SetAnchorPoint(lat0, long0, h0);

	m_Anchor_lat0=lat0;	m_Anchor_lon0=long0;	m_Anchor_h0=h0;

	//NED北东地 -->  地心地固坐标
	//	RotateMat_NED2ECEF(lat0, long0, m_Rw);
	//	RotateMat_TPlane2WGS84_NED(lat0, long0, m_Rw);

	//	RotateMat_WGS842TPlane_ENU1(lat0, long0, m_Rt);  //矩阵是否正确？2010.9.9 

	//地心地固坐标 -->  东北天
//	orsIMatrixService *matrixService;

	if( NULL == m_matrixService.get() )	{
		//		msg->logPrint( ORS_LOG_ERROR, "Can not get matrixService" );
		return;
	}

	m_matrixService->RotateMatrix_ECEF2ENU(lat0, long0, m_RRaw2User);
}

//设置中央经线（默认：meridian == long0）
void orsXPOS_WGS84::SetCentralMeridian(double meridian)
{
	m_CentralMeridian = meridian;
}

bool orsXPOS_WGS84::SetSRS( const char *hcsWkt, const char *vcsWkt )
{
	m_orgCoordSys->importFromWkt( hcsWkt );

	m_dstCoordSys->importFromWkt( vcsWkt );

	if(m_coordTransform.get()==NULL)
		m_coordTransform = getSRService()->CreateCoordinateTransform();

	m_coordTransform->Initialize( m_orgCoordSys.get(), m_dstCoordSys.get());

	return true;
}

void orsXPOS_WGS84::WGS84_To_TangentPlane(orsPOSParam *pOrgData, orsPOSParam *pDstData, ors_int32 num)
{
	for(int i=0; i<num; i++)
	{
		//线元素转换
		if(m_userCoordType == ORS_igctTangentPlane)
			m_pTangentPlane->Geographic2TangentPlane(pOrgData[i].coord.X, pOrgData[i].coord.Y, pOrgData[i].coord.Z, 
			&(pDstData[i].coord.X), &(pDstData[i].coord.Y), &(pDstData[i].coord.Z));
		

		//角元素转换
		double Rn[9], Rw[9], R1[9]/*, R2[9]*/;
		double R[9];
		double lon = pOrgData[i].coord.X;
		double lat = pOrgData[i].coord.Y;

		//导航系
		m_matrixService->RotateMatrix_rph(pOrgData[i].r, pOrgData[i].p, pOrgData[i].h, Rn);

		//	RotateMat_TPlane2WGS84_NED(lat, lon, Rw);	//当前导航系到地心地固系的旋转矩阵
		m_matrixService->RotateMatrix_NED2ECEF(lat, lon, Rw);
		//R=Rt*Rw*Rn
		matrix_product(3, 3, 3, 3, Rw, Rn, R1);

		//转到局部切平面坐标系
		matrix_product(3, 3, 3, 3, m_RRaw2User, R1, R);

		memcpy(pDstData[i].R, R, sizeof(double)*9);

	}
}

void orsXPOS_WGS84::WGS84_To_UTM(orsPOSParam *pOrgData, orsPOSParam *pDstData, ors_int32 num)
{
// 	orsIMatrixService *matrixService;
// 
// 	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
// 	if( NULL == matrixService )	{
// 		//		msg->logPrint( ORS_LOG_ERROR, "Can not get matrixService" );
// 		return;
// 	}
	double vOrg[3], vDst[3];
	double ROrg[9], RDst[9];
	for(int i=0; i<num; i++)
	{
		vOrg[0] = pOrgData[i].coord.X;
		vOrg[1] = pOrgData[i].coord.Y;
		vOrg[2] = pOrgData[i].coord.Z;

		//旋转矩阵
		m_matrixService->RotateMatrix_rph(pOrgData[i].r, pOrgData[i].p, pOrgData[i].h, ROrg);
		
		WGS84_To_UTM(vOrg, ROrg, vDst, RDst);

		pDstData[i].coord.X = vDst[0];
		pDstData[i].coord.Y = vDst[1];
		pDstData[i].coord.Z = vDst[2];

		m_matrixService->R2rph(RDst, &(pDstData[i].r), &(pDstData[i].p), &(pDstData[i].h));
	}	
}

void orsXPOS_WGS84::WGS84_To_UTM(double vOrg[3], double ROrg[9], double vDst[3], double RDst[9])
{
	//线元素转换
	if(m_userCoordType == ORS_igctTangentPlane)
		m_pTangentPlane->Geographic2TangentPlane(vOrg[0], vOrg[1], vOrg[2], 
		vDst, vDst+1, vDst+2);
	else if(m_userCoordType == ORS_igctProjected)
	{//切平面转投影
		orsPOINT3D  pt1, pt2;

		if(m_rawCoordType == ORS_igctGeographic)
		{
			pt1.X=vOrg[0]*RAD2DEG;	
			pt1.Y=vOrg[1]*RAD2DEG;	
			pt1.Z=vOrg[2];	
		}
		else
		{
			pt1.X=vOrg[0];	
			pt1.Y=vOrg[1];
			pt1.Z=vOrg[2];	
			//				pt1 = pOrgData[i].coord;
		}

		m_coordTransform->Transform( pt1, &pt2 );

		vDst[0]=pt2.X;	vDst[1]=pt2.Y;	vDst[2]=pt2.Z;
	}


	//角元素转换
	double Rn[9], Rw[9], R1[9]/*, R2[9]*/;
	double R[9];
	double lat = vOrg[0];
	double lon = vOrg[1];

	//导航系
	//		matrixService->RotateMat_rph(pOrgData[i].r, pOrgData[i].p, pOrgData[i].h, Rn);
	memcpy(Rn, ROrg, sizeof(double)*9);

	//RotateMat_TPlane2WGS84_NED(lat, lon, Rw);	//当前导航系到地心地固系的旋转矩阵
	m_matrixService->RotateMatrix_NED2ECEF(lat, lon, Rw);
	//R=Rt*Rw*Rn
	matrix_product(3, 3, 3, 3, Rw, Rn, R1);

	//转到局部切平面坐标系
	matrix_product(3, 3, 3, 3, m_RRaw2User, R1, R);

	memcpy(RDst, R, sizeof(double)*9);

	if(m_userCoordType == ORS_igctProjected)
	{//切平面转投影的补偿矩阵
		//	"高斯-克吕格投影坐标系下POS角元素的转换方法", 袁修孝
		double Rcomp[9];

		double sinB = sin(lat);
		double cosB = cos(lat);
		double l1 = m_Anchor_lon0 - m_CentralMeridian;	//L0-L0gk
		double l2 = m_Anchor_lon0 - lon;	//L0 - L
		double B2 = lat - m_Anchor_lat0;	//B - B0

		// 			Rcomp[0]=1;			Rcomp[3]=-l*sinB;		Rcomp[6]=0;
		// 			Rcomp[1]=l*sinB;	Rcomp[4]=1;				Rcomp[7]=0;
		// 			Rcomp[2]=0;			Rcomp[5]=0;				Rcomp[8]=1;

		Rcomp[0]=1;			Rcomp[3]=l1*sinB;		Rcomp[6]=-l2*cosB;
		Rcomp[1]=-l1*sinB;	Rcomp[4]=1;				Rcomp[7]=B2;
		Rcomp[2]=l2*cosB;	Rcomp[5]=-B2;			Rcomp[8]=1;

		matrix_product(3, 3, 3, 3, Rcomp, R, RDst);
	}	
	
}

// void orsXPOS_WGS84::test_atn(ATNPoint *atnpt, double coord[3], orsPOINT3D *gps, double *Rimu)
// {
// 	//角元素转换
// 	double Rn[9], Rw[9], R1[9]/*, R2[9]*/;
// 	double R[9];
// 	double lat = atnpt->latitude;
// 	double lon = atnpt->longitude;
// 
// 	double Rl[9];
// 	///////扫描角旋转矩阵//////////////////////
// 	Rl[0]=1.0e0;	Rl[1]=0.0e0;	Rl[2]=0.0e0;
// 	Rl[3]=0.0e0;	Rl[4]=cos(atnpt->scanAngle);	Rl[5]=-sin(atnpt->scanAngle);
// 	Rl[6]=0.0e0;	Rl[7]=sin(atnpt->scanAngle);	Rl[8]=cos(atnpt->scanAngle);
// 
// 	double vec_range[3];
// 	vec_range[0]=vec_range[1]=0.0e0;
// 	vec_range[2]=atnpt->range;
// 	double vec_LFrame[3];
// 	matrix_product(3, 3, 3, 1, Rl, vec_range, vec_LFrame);
// 
// // 	matrix_product(3, 3, 3, 1, Rm, vec_LFrame, vec_IFrame);
// // 
// // 	vec_IFrame[0]+=leverarm[0];
// // 	vec_IFrame[1]+=leverarm[1];
// // 	vec_IFrame[2]+=leverarm[2];
// 
// 	//导航系
// //	if(Rimu==NULL)
// 		m_matrixService->RotateMat_rph(atnpt->roll, atnpt->pitch, atnpt->heading, Rn);
// // 	else
// // 		memcpy(Rn, Rimu, sizeof(double)*9);
// 
// 	double vec_TFrame1[3], vec_TFrame2[3];
// 	matrix_product(3, 3, 3, 1, Rn, vec_LFrame, vec_TFrame1);
// 	matrix_product(3, 3, 3, 1, Rimu, vec_LFrame, vec_TFrame2);
// 
// 	//	RotateMat_TPlane2WGS84_NED(lat, lon, Rw);	//当前导航系到地心地固系的旋转矩阵
// 	m_matrixService->RotateMat_NED2ECEF(lat, lon, Rw);
// 	double pos[3];
// 	matrix_product(3, 3, 3, 1, Rw, vec_TFrame1, pos);
// 
// 	ref_ptr<orsIEllipsoid>	pEllipsoid=getSRService()->CreateEllipsoid();
// 	double Xgps1, Ygps1, Zgps1;
// 	double Xgps2, Ygps2, Zgps2;
// 	pEllipsoid->Geodetic_To_Geocentric(atnpt->latitude, atnpt->longitude, atnpt->altitude, &Xgps1, &Ygps1, &Zgps1);
// 	pEllipsoid->Geodetic_To_Geocentric(gps->lat, gps->lon, gps->h, &Xgps2, &Ygps2, &Zgps2);
// 
// 	pos[0]+=Xgps1;
// 	pos[1]+=Ygps1;
// 	pos[2]+=Zgps1;	
// 
// 	pEllipsoid->Geocentric_To_Geodetic(pos[0], pos[1], pos[2], coord+1, coord, coord+2);
// }