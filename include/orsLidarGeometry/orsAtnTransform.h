#ifndef __ORS_ATN_COORDINATE_TRANSFORM_INCLUDE__
#define __ORS_ATN_COORDINATE_TRANSFORM_INCLUDE__

#include "orsSRS/orsISpatialReferenceService.h"
#include "orsMath/orsIMatrixService.h"
#include "orsPointCloud/orsIPointCloud.h"
#include "dpsMatrix.h"
#include "orsMath/orsQuaternion.h"

ORS_GET_SRS_SERVICE_IMPL();

ORS_GET_MATRIX_SERVICE_IMPL();

// void CreateRotateMatrix(double angle, double axis[3], R[9])
// {
//     double norm = sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
//     double u[3];
//     
//     u[0] = axis[0] / norm;
//     u[1] = axis[1] / norm;
//     u[2] = axis[2] / norm;
// 
//     R[0] = cos(angle) + u[0] * u[0] * (1 - cos(angle));
//     R[1] = u[0] * u[1] * (1 - cos(angle) - u[2] * sin(angle));
//     R[2] = u[1] * sin(angle) + u[0] * u[2] * (1 - cos(angle));
// 
//     R[3] = u[2] * Math.Sin(angle) + u[0] * u[1] * (1 - Math.Cos(angle));
//     R[4] = Math.Cos(angle) + u[1] * u[1] * (1 - Math.Cos(angle));
//     R[5] = -u[0] * Math.Sin(angle) + u[1] * u[2] * (1 - Math.Cos(angle));
//       
//     R[6] = -u[1] * Math.Sin(angle) + u[0] * u[2] * (1 - Math.Cos(angle));
//     R[7] = u[0] * Math.Sin(angle) + u[1] * u[2] * (1 - Math.Cos(angle));
//     R[8] = Math.Cos(angle) + u[2] * u[2] * (1 - Math.Cos(angle));
// 
//     return rotatinMatrix;
// }

class orsAtnTransform
{
public:
	orsAtnTransform()
	{
		m_pTangentPlane = getSRService()->CreateTangentPlane();
		m_matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	}

	~orsAtnTransform()
	{

	}

	bool SetSRS( const char *hcsWkt, const char *vcsWkt )
	{
		if(m_orgCoordSys.get()==NULL)
			m_orgCoordSys = getSRService()->CreateSpatialReference();
		if(m_dstCoordSys.get()==NULL)
			m_dstCoordSys = getSRService()->CreateSpatialReference();

		m_orgCoordSys->importFromWkt( hcsWkt );
		m_dstCoordSys->importFromWkt( vcsWkt );

		if(m_wgs84.get()==NULL)
			m_wgs84=getSRService()->CreateEllipsoid();

		if(m_coordTransform.get()==NULL)
			m_coordTransform = getSRService()->CreateCoordinateTransform();

		m_coordTransform->Initialize( m_orgCoordSys.get(), m_dstCoordSys.get());

		return true;
	}

	bool SetAnchor( double lat0, double lon0, double h0 )
	{
		m_pTangentPlane->SetAnchorPoint(lat0, lon0, h0);

		m_Anchor_lat0=lat0;	m_Anchor_lon0=lon0;	m_Anchor_h0=h0;

		m_matrixService->RotateMatrix_ECEF2ENU(lat0, lon0, m_RRaw2User);
		return true;
	}

		
	//wgs84转切平面
	inline void WGS84_To_TangentPlane(double xyz1[3], orsPointObservedInfo *obsInfo1, 
		double xyz2[3], orsPointObservedInfo *obsInfo2)
	{
		//线元素转换 lat. lon. h
		m_pTangentPlane->Geographic2TangentPlane(xyz1[1], xyz1[0], xyz1[2], 
			xyz2, xyz2+1, xyz2+2);

		m_pTangentPlane->Geographic2TangentPlane(obsInfo1->pos.lat, obsInfo1->pos.lon, obsInfo1->pos.h, 
			&(obsInfo2->pos.X), &(obsInfo2->pos.Y), &(obsInfo2->pos.Z));

		//角元素转换
		double Rn[9], Rw[9], R1[9]/*, R2[9]*/;
		double R[9];
		double lon = obsInfo1->pos.lon;
		double lat = obsInfo1->pos.lat;

		//导航系
		m_matrixService->RotateMatrix_rph(obsInfo1->roll, obsInfo1->pitch, obsInfo1->heading, Rn);
//		m_matrixService->R2rph(Rn, &(obsInfo2->roll), &(obsInfo2->pitch), &(obsInfo2->heading));

		//	RotateMat_TPlane2WGS84_NED(lat, lon, Rw);	//当前导航系到地心地固系的旋转矩阵
		m_matrixService->RotateMatrix_NED2ECEF(lat, lon, Rw);
		//R=Rt*Rw*Rn
		matrix_product(3, 3, 3, 3, Rw, Rn, R1);

		//转到局部切平面坐标系
		matrix_product(3, 3, 3, 3, m_RRaw2User, R1, R);

		m_matrixService->R2rph(R, &(obsInfo2->roll), &(obsInfo2->pitch), &(obsInfo2->heading));

//		memcpy(pDstData[i].R, R, sizeof(double)*9);
		obsInfo2->scanAngle = obsInfo1->scanAngle;
		obsInfo2->range = obsInfo1->range;

		//切平面下的斜距方向矢量
// 		double vec_TFrame[3];
// 		vec_TFrame[0] = xyz2[0] - obsInfo2->pos.X;
// 		vec_TFrame[1] = xyz2[1] - obsInfo2->pos.Y;
// 		vec_TFrame[2] = xyz2[2] - obsInfo2->pos.Z;
// 
// 		double R_T[9];
// 		matrix_transpose(3, 3, R, R_T);
// 		double RangVec_TP[3];
// 		matrix_product(3,3,3,1, R_T, vec_TFrame, RangVec_TP);

		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		//测试
// 		double RangVec[3]/*, vec_TFrame[3]*/;
// 		double x, y, z;
// 		double Xs, Ys, Zs;
// 		m_wgs84->Geodetic_To_Geocentric(xyz1[1], xyz1[0], xyz1[2], &x, &y, &z);
// 		m_wgs84->Geodetic_To_Geocentric(obsInfo1->pos.lat, obsInfo1->pos.lon, 
// 			obsInfo1->pos.h, &Xs, &Ys, &Zs);
// 
// 		//姿态矩阵：导航 -> 地心
// 		double lon0 = obsInfo1->pos.lon;
// 		double lat0 = obsInfo1->pos.lat;
// 		double /*Rw[9], R1[9],*/ Rnav[9], Rn_T[9];
// 		m_matrixService->RotateMatrix_NED2ECEF(lat0, lon0, Rw);
// 		//Rn=Rw*Rnav
// 		m_matrixService->RotateMatrix_rph(obsInfo1->roll, obsInfo1->pitch, obsInfo1->heading, Rnav);	
// 		matrix_product(3, 3, 3, 3, Rw, Rnav, R1);
// 		matrix_transpose(3, 3, R1, Rn_T);
// 
// 		vec_TFrame[0] = x - Xs;
// 		vec_TFrame[1] = y - Ys;
// 		vec_TFrame[2] = z - Zs;
// 		matrix_product(3,3,3,1, Rn_T, vec_TFrame, RangVec);
// 
// 		
// 		/*计算两个向量之间的旋转矩阵*/
// 		//Cal_RotateMat(v1, v_t, RDst);
// 		double rot_axis[3];
// 
// 		//叉积得到旋转轴
// 		rot_axis[0] = RangVec[1] * vec_TFrame[2] - RangVec[2] * vec_TFrame[1];
// 		rot_axis[1] = RangVec[2] * vec_TFrame[0] - RangVec[0] * vec_TFrame[2];
// 		rot_axis[2] = RangVec[0] * vec_TFrame[1] - RangVec[1] * vec_TFrame[0];
// 
// 		double rot_angle;
// 		double dotProd, norm1, norm2;
// 		//点积计算旋转角
// 		dotProd = RangVec[0] * vec_TFrame[0] + RangVec[1] * vec_TFrame[1] + RangVec[2] * vec_TFrame[2];
// 
// 		norm1 = sqrt(RangVec[0] * RangVec[0] + RangVec[1] * RangVec[1] + RangVec[2] * RangVec[2]);
// 		norm2 = sqrt(vec_TFrame[0] * vec_TFrame[0] + vec_TFrame[1] * vec_TFrame[1] + vec_TFrame[2] * vec_TFrame[2]);
// 		rot_angle = acos(dotProd/norm1/norm2);
// 
// 		orsQuaternion quat;
// //		double Rn[9];
// 		quat.setRotate(rot_angle*DEG_PER_RAD, rot_axis[0], rot_axis[1], rot_axis[2]);
// 		quat.getRotateMatrix(Rn);
// 
// 		double r, p, h;
// 		m_matrixService->R2rph(Rn, &r, &p, &h);
// 
// 		obsInfo2->roll = r;
// 		obsInfo2->pitch = p;
// 		obsInfo2->heading = h;

		//测试斜距向量是否相同

// 		vec_TFrame[0] = xyz2[0] - obsInfo2->pos.X;
// 		vec_TFrame[1] = xyz2[1] - obsInfo2->pos.Y;
// 		vec_TFrame[2] = xyz2[2] - obsInfo2->pos.Z;
//  		double vec_LFrame[3];
// 		matrix_product(3,3,3,1, Rn, RangVec_TP, vec_LFrame);
// 		matrix_product(3,3,3,1, R, RangVec_TP, vec_LFrame);

	}

	//转UTM投影坐标系
	//姿态转换有问题，会导致平差无法收敛 2014.12.18
	inline void WGS84_To_UTM(double xyz1[3], orsPointObservedInfo *obsInfo1, 
		double xyz2[3], orsPointObservedInfo *obsInfo2)
	{
		assert(m_coordTransform.get());

		orsPOINT3D  pt1, pt2;

		pt1.X=xyz1[0]*DEG_PER_RAD;	
		pt1.Y=xyz1[1]*DEG_PER_RAD;	
		pt1.Z=xyz1[2];	

		m_coordTransform->Transform( pt1, &pt2 );
		xyz2[0]=pt2.X;	xyz2[1]=pt2.Y;	xyz2[2]=pt2.Z;

		//计算UTM下的Xs,Ys,Zs
		pt1.X=obsInfo1->pos.X*DEG_PER_RAD;	
		pt1.Y=obsInfo1->pos.Y*DEG_PER_RAD;	
		pt1.Z=obsInfo1->pos.Z;	

		m_coordTransform->Transform( pt1, &pt2 );
		obsInfo2->pos = pt2;

		//计算地心坐标系下的方向矢量
		double RangVec[3], vec_TFrame[3];
		double x, y, z;
		double Xs, Ys, Zs;
		m_wgs84->Geodetic_To_Geocentric(xyz1[1], xyz1[0], xyz1[2], &x, &y, &z);
		m_wgs84->Geodetic_To_Geocentric(obsInfo1->pos.lat, obsInfo1->pos.lon, 
			obsInfo1->pos.h, &Xs, &Ys, &Zs);

		//姿态矩阵：导航 -> 地心
		double lon0 = obsInfo1->pos.lon;
		double lat0 = obsInfo1->pos.lat;
		double Rw[9], R1[9], Rnav[9], Rn_T[9];
		m_matrixService->RotateMatrix_NED2ECEF(lat0, lon0, Rw);
		//Rn=Rw*Rnav
		m_matrixService->RotateMatrix_rph(obsInfo1->roll, obsInfo1->pitch, obsInfo1->heading, Rnav);	
		matrix_product(3, 3, 3, 3, Rw, Rnav, R1);
		matrix_transpose(3, 3, R1, Rn_T);
		
		vec_TFrame[0] = x - Xs;
		vec_TFrame[1] = y - Ys;
		vec_TFrame[2] = z - Zs;
		matrix_product(3,3,3,1, Rn_T, vec_TFrame, RangVec);

		//计算UTM下的斜距方向矢量
		vec_TFrame[0] = xyz2[0] - obsInfo2->pos.X;
		vec_TFrame[1] = xyz2[1] - obsInfo2->pos.Y;
		vec_TFrame[2] = xyz2[2] - obsInfo2->pos.Z;


		/*计算两个向量之间的旋转矩阵*/
		//Cal_RotateMat(v1, v_t, RDst);
		double rot_axis[3];

		//叉积得到旋转轴
		rot_axis[0] = RangVec[1] * vec_TFrame[2] - RangVec[2] * vec_TFrame[1];
		rot_axis[1] = RangVec[2] * vec_TFrame[0] - RangVec[0] * vec_TFrame[2];
		rot_axis[2] = RangVec[0] * vec_TFrame[1] - RangVec[1] * vec_TFrame[0];

		double rot_angle;
		double dotProd, norm1, norm2;
		//点积计算旋转角
		dotProd = RangVec[0] * vec_TFrame[0] + RangVec[1] * vec_TFrame[1] + RangVec[2] * vec_TFrame[2];

		norm1 = sqrt(RangVec[0] * RangVec[0] + RangVec[1] * RangVec[1] + RangVec[2] * RangVec[2]);
		norm2 = sqrt(vec_TFrame[0] * vec_TFrame[0] + vec_TFrame[1] * vec_TFrame[1] + vec_TFrame[2] * vec_TFrame[2]);
		rot_angle = acos(dotProd/norm1/norm2);

		orsQuaternion quat;
		double Rn[9];
		quat.setRotate(rot_angle*DEG_PER_RAD, rot_axis[0], rot_axis[1], rot_axis[2]);
// 		orsPOINT3D vOrg, vDst;
// 		vOrg.X = RangVec[0];	vOrg.Y = RangVec[1];	vOrg.Z = RangVec[2];
// 		vDst = quat.rotate(vOrg);
// 
// 		vOrg.X = vec_TFrame[0];	vOrg.Y = vec_TFrame[1];	vOrg.Z = vec_TFrame[2];
// 		vDst = quat.rotate(vOrg);
		

		quat.getRotateMatrix(Rn);

//		matrix_transpose(3,3,Rn, Rn_T);
// 
// 		double v1[3], v2[3];
// 
// 		matrix_product(3,3,3,1, Rn, RangVec, v1);
// 		matrix_product(3,3,3,1, Rn_T, RangVec, v2);
// 
// 		matrix_product(3,3,3,1, Rn, vec_TFrame, v1);
// 		matrix_product(3,3,3,1, Rn_T, vec_TFrame, v2);


		m_matrixService->R2rph(Rn, &(obsInfo2->roll), &(obsInfo2->pitch), &(obsInfo2->heading));

		obsInfo2->scanAngle = obsInfo1->scanAngle;
		obsInfo2->range = obsInfo1->range;
	};

private:
	ref_ptr<orsITangentPlane>	m_pTangentPlane; 
	double m_Anchor_lat0, m_Anchor_lon0, m_Anchor_h0;
	
	double m_RRaw2User[9];
	ref_ptr<orsIMatrixService> m_matrixService;

	//for投影转换
	ref_ptr<orsISpatialReference>  m_orgCoordSys;
	ref_ptr<orsISpatialReference>  m_dstCoordSys;
	ref_ptr<orsICoordinateTransform> m_coordTransform;	
	ref_ptr<orsIEllipsoid>	m_wgs84;
};


#endif