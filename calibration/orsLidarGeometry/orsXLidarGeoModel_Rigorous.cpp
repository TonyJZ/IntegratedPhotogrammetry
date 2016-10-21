#include "StdAfx.h"
#include "orsXLidarGeoModel_Rigorous.h"

#include "orsBase\orsContants.h"
#include "orsSRS\orsISpatialReferenceService.h"
#include "orsSRS/orsIEllipsoid.h"
#include "dpsMatrix.h"

orsXLidarGeoModel_Rigorous::orsXLidarGeoModel_Rigorous(bool bForRegister)
{
	m_matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	m_pTangentPlane = getSRService()->CreateTangentPlane();
	m_wgs84=getSRService()->CreateEllipsoid();

	m_leverarm[0] = m_leverarm[1] = m_leverarm[2] = 0;	//偏心分量
	m_sa0 = m_sac = 0;	//扫描角误差，扫描角线性改正
	m_semiXYZ[0] = m_semiXYZ[1] = m_semiXYZ[2];
	m_drange = 0;		//测距误差
	matrix_ident(3, m_Rboresight);
}

orsXLidarGeoModel_Rigorous::~orsXLidarGeoModel_Rigorous()
{

}

void orsXLidarGeoModel_Rigorous::setSysParam(/*orsLidSysExtraParam_TYPE type, */void *extParam)
{
//	m_type = type;
	m_extParam = (orsLidSysParam*)extParam;
	m_matrixService->RotateMatrix_rph(m_extParam->boresight_angle[0], m_extParam->boresight_angle[1], m_extParam->boresight_angle[2], 
		m_Rboresight);	//偏心角

	memcpy(m_leverarm, m_extParam->lever_arm, sizeof(double)*3);
	memcpy(m_semiXYZ, m_extParam->semiXYZ, sizeof(double)*3);
	m_sa0 = m_extParam->sa0;
	m_sac = m_extParam->sac;
	m_drange = m_extParam->drange;

}

bool orsXLidarGeoModel_Rigorous::setSRS(const char *hcsWkt, const char *vcsWkt)
{
// 	m_pSRS = getSRService()->CreateSpatialReference();
// 
// 	bool bRet = m_pSRS->importFromWkt( hcsWkt );
// 
// 	if( vcsWkt )	{
// 		bRet = m_pSRS->importFromVcsWkt( vcsWkt );
// 	}

	if(hcsWkt){
		m_orgCoordSys = getSRService()->CreateSpatialReference();
		m_orgCoordSys->importFromWkt( hcsWkt );
	}
	if(vcsWkt){
		m_dstCoordSys = getSRService()->CreateSpatialReference();
		m_dstCoordSys->importFromWkt( vcsWkt );
	}
	if(hcsWkt && vcsWkt){

		m_coordTransform = getSRService()->CreateCoordinateTransform();
		m_coordTransform->Initialize( m_orgCoordSys.get(), m_dstCoordSys.get());
	}
	return true;
}

void orsXLidarGeoModel_Rigorous::cal_Laserfootprint(LidGeo_RawPoint *pdata, ors_int32 ptNum)
{
	double  Rn[9];		//imu旋转矩阵

	for(int i=0; i<ptNum; i++)
	{
		m_matrixService->RotateMatrix_rph(pdata[i].eoParam.r, pdata[i].eoParam.p, pdata[i].eoParam.h, Rn);	//pos姿态

		cal_Laserfootprint(&(pdata[i].eoParam.coord), Rn, pdata[i].range, pdata[i].sAngle,
			&(pdata[i].X), &(pdata[i].Y), &(pdata[i].Z));
	}
}

void orsXLidarGeoModel_Rigorous::cal_Laserfootprint(orsPOINT3D *gps_pos, double Rn[9], double range, double sAngle, double *X, double *Y, double *Z)
{
	double  Rl[9];		//扫描镜旋转矩阵
	//	double  Rm[9];		//偏心角旋转矩阵
	//	double  Rn[9];		//imu旋转矩阵
	double	Rw[9];		//局部切平面转ECEF

	//	double  Rl[9];		//imu坐标系到局部参考坐标系旋转矩阵
	double  vec_range[3];
	double  vec_LFrame[3];	//扫描坐标系
	double  vec_IFrame[3];	//imu坐标系
	double  vec_TFrame[3];	//局部切平面坐标系
	double	vec_ECEF[3];

	double lon, lat, H;
	
	///////扫描角旋转矩阵//////////////////////
	Rl[0]=1.0e0;	Rl[1]=0.0e0;		Rl[2]=0.0e0;
	Rl[3]=0.0e0;	Rl[4]=cos(sAngle+sAngle*m_sac+m_sa0);	Rl[5]=-sin(sAngle+sAngle*m_sac+m_sa0);
	Rl[6]=0.0e0;	Rl[7]=sin(sAngle+sAngle*m_sac+m_sa0);	Rl[8]=cos(sAngle+sAngle*m_sac+m_sa0);

	vec_range[0]=vec_range[1]=0.0e0;
	vec_range[2]=range + m_drange;

	matrix_product(3, 3, 3, 1, Rl, vec_range, vec_LFrame);

	matrix_product(3, 3, 3, 1, m_Rboresight, vec_LFrame, vec_IFrame);

	vec_IFrame[0]+=m_leverarm[0];
	vec_IFrame[1]+=m_leverarm[1];
	vec_IFrame[2]+=m_leverarm[2];

	//		m_matrixService->RotateMat_rph(pdata[i].eoParam.r, pdata[i].eoParam.p, pdata[i].eoParam.h, Rn);	//pos姿态
	matrix_product(3, 3, 3, 1, Rn, vec_IFrame, vec_TFrame);

	//当前坐标系为地理坐标系
	if(m_orgCoordSys.get() && m_orgCoordSys->IsGeographic())
	{
		lat = gps_pos->lat;
		lon = gps_pos->lon;
		H = gps_pos->h;
		m_matrixService->RotateMatrix_NED2ECEF(lat, lon, Rw);

		matrix_product(3, 3, 3, 1, Rw, vec_TFrame, vec_ECEF);

		//地心坐标系下
		double Xgps, Ygps, Zgps;
		m_wgs84->Geodetic_To_Geocentric(lat, lon, H, &Xgps, &Ygps, &Zgps);

		vec_ECEF[0] += Xgps;
		vec_ECEF[1] += Ygps;
		vec_ECEF[2] += Zgps;

		m_wgs84->Geocentric_To_Geodetic(vec_ECEF[0], vec_ECEF[1], vec_ECEF[2], Y, X, Z);
	}
	else
	{
		*X = vec_TFrame[0] + gps_pos->X;
		*Y = vec_TFrame[1] + gps_pos->Y;
		*Z = vec_TFrame[2] + gps_pos->Z;
	}
}

void orsXLidarGeoModel_Rigorous::cal_Laserfootprint(LidPt_SurvInfo *pts, int Num)
{
	//double  Rl[9];		//扫描镜旋转矩阵
	//	double  Rm[9];		//偏心角旋转矩阵
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

		//当前坐标系为地理坐标系
		if(m_orgCoordSys.get() && m_orgCoordSys->IsGeographic())
		{
			double Rw[9], R1[9]/*, R2[9]*/;
			double R[9];
			double lon = pts[i].POS_Info.coord.lon;
			double lat = pts[i].POS_Info.coord.lat;

			//	RotateMat_TPlane2WGS84_NED(lat, lon, Rw);	//当前导航系到地心地固系的旋转矩阵
			m_matrixService->RotateMatrix_NED2ECEF(lat, lon, Rw);
			//R=Rt*Rw*Rn
			matrix_product33(Rw, Rn, R1);
			matrix_product(3, 3, 3, 1, R1, vec_IFrame, vec_TFrame);

			//地心坐标系下
			double x, y, z;
			m_wgs84->Geodetic_To_Geocentric(pts[i].POS_Info.coord.lat, 
				pts[i].POS_Info.coord.lon, 
				pts[i].POS_Info.coord.h, &Xgps, &Ygps, &Zgps);

			x = vec_TFrame[0]+Xgps;
			y = vec_TFrame[1]+Ygps;
			z = vec_TFrame[2]+Zgps;

			m_wgs84->Geocentric_To_Geodetic(x, y, z, 
				&pts[i].y, &pts[i].x, &pts[i].z);

			if(m_dstCoordSys.get() && m_dstCoordSys->IsProjected())
			{
				orsPOINT3D  pt_org, pt_dst;
				pt_org.X = pts[i].x*DEG_PER_RAD;	
				pt_org.Y = pts[i].y*DEG_PER_RAD;	
				pt_org.Z = pts[i].z;	

				m_coordTransform->Transform( pt_org, &pt_dst );

				pts[i].x = pt_dst.X;
				pts[i].y = pt_dst.Y;
				pts[i].z = pt_dst.Z;
			}
		}
		else
		{
			matrix_product(3, 3, 3, 1, Rn, vec_IFrame, vec_TFrame);

			pts[i].x = vec_TFrame[0]+Xgps;
			pts[i].y = vec_TFrame[1]+Ygps;
			pts[i].z = vec_TFrame[2]+Zgps;
		}
	}
}

//根据当前激光点的坐标和POS计算斜距
//将安置参数当做0计算
void orsXLidarGeoModel_Rigorous::cal_RangeVec(LidPt_SurvInfo *pts, int Num)
{
	int i;
	double  Rn[9], Rn_T[9];		//imu旋转矩阵
	double  vec_LFrame[3];	//扫描坐标系
	//	double  vec_IFrame[3];	//imu坐标系
	double  vec_TFrame[3];	//局部切平面坐标系

	double Xs, Ys, Zs;
	double x, y, z;
	double dis_off[3];  //姿态角与lever arm的积

	for(i=0; i<Num; i++)
	{
		if(m_orgCoordSys.get() && m_orgCoordSys->IsGeographic())
		{
			m_wgs84->Geodetic_To_Geocentric(pts[i].y, pts[i].x, pts[i].z, &x, &y, &z);
			m_wgs84->Geodetic_To_Geocentric(pts[i].POS_Info.coord.lat, 
											pts[i].POS_Info.coord.lon, 
											pts[i].POS_Info.coord.h,
											&Xs, &Ys, &Zs);

			//姿态矩阵：导航 -> 地心
			double lon0 = pts[i].POS_Info.coord.lon;
			double lat0 = pts[i].POS_Info.coord.lat;
			double Rw[9], R1[9];
			m_matrixService->RotateMatrix_NED2ECEF(lat0, lon0, Rw);
			//R=Rt*Rw*Rn
			m_matrixService->RotateMatrix_rph(pts[i].POS_Info.r, pts[i].POS_Info.p, pts[i].POS_Info.h, Rn);	
			matrix_product(3, 3, 3, 3, Rw, Rn, R1);
			matrix_transpose(3, 3, R1, Rn_T);
		}
		else
		{
			Xs = pts[i].POS_Info.coord.X;
			Ys = pts[i].POS_Info.coord.Y;
			Zs = pts[i].POS_Info.coord.Z;

			x = pts[i].x;
			y = pts[i].y;
			z = pts[i].z;

			//pos姿态
			m_matrixService->RotateMatrix_rph(pts[i].POS_Info.r, pts[i].POS_Info.p, pts[i].POS_Info.h, Rn);	
			matrix_transpose(3, 3, Rn, Rn_T);
		}
		
		vec_TFrame[0] = x - Xs;
		vec_TFrame[1] = y - Ys;
		vec_TFrame[2] = z - Zs;

		//默认安置误差都为0
// 		matrix_product(3,3,3,1, Rn_T, vec_TFrame, vec_LFrame);
// 		pts[i].vecRange[0] = vec_LFrame[0];
// 		pts[i].vecRange[1] = vec_LFrame[1];
// 		pts[i].vecRange[2] = vec_LFrame[2];

		//利用当前安置参数来计算斜距
		matrix_product(3, 3, 3, 1, Rn, m_leverarm, dis_off);
		
		vec_TFrame[0] -= dis_off[0];
		vec_TFrame[1] -= dis_off[1];
		vec_TFrame[2] -= dis_off[2];
		
		matrix_product(3,3,3,1, Rn_T, vec_TFrame, vec_LFrame);
		matrix_transpose_product(3,3,3,1, m_Rboresight, vec_LFrame, pts[i].vecRange);
	}
}

void orsXLidarGeoModel_Rigorous::estimate_range_scanAngle(double *rangeVec, double *range, double *sAngle)
{
	double X, Y, Z;
	double Xgps, Ygps, Zgps; 
	double vec_ECEF[3], vec_TFrame[3], vec_IFrame[3], vec_LFrame[3];
	double Rw[9];

	vec_LFrame[0] = rangeVec[0];
	vec_LFrame[1] = rangeVec[1];
	vec_LFrame[2] = rangeVec[2];

	//vec_LFrame[0] = 0
	*sAngle = (atan(-vec_LFrame[1]/vec_LFrame[2])-m_sa0)/(1+m_sac);
	*range = sqrt(vec_LFrame[1]*vec_LFrame[1]+vec_LFrame[2]*vec_LFrame[2])-m_drange;
}

//计算偏导数
void orsXLidarGeoModel_Rigorous::calPDC_misalign(void *data, ors_int32 ptNum, 
	/*orsLidSysExtraParam_TYPE type,*/ void *extParam, double *pPDC)
{
	LidGeo_RawPoint *pdata = (LidGeo_RawPoint*)data;
	orsLidSysParam *pextParam = (orsLidSysParam*)extParam;

}