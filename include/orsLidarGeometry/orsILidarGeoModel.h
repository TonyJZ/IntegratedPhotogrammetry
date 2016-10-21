#ifndef _OGR_LIDAR_GEOMETRY_MODEL_INTERFACE_H_
#define _OGR_LIDAR_GEOMETRY_MODEL_INTERFACE_H_

#include "orsBase/orsTypedef.h"
#include "orsBase/orsIObject.h"
#include "orsLidarGeometry/LidGeo_def.h"
//#include "calibbaseddef.h"


//计算激光脚点坐标值不对  2014.12.2
class orsILidarGeoModel:  public orsIObject
{
public:

	//设置系统参数
	virtual void setSysParam(void *extParam) = 0;

	//计算激光脚点坐标
	inline virtual void cal_Laserfootprint(LidGeo_RawPoint *pdata, ors_int32 ptNum) = 0;

	inline virtual void cal_Laserfootprint(orsPOINT3D *gps_pos, double Rn[9], double range, double sAngle, 
		double *X, double *Y, double *Z) = 0;

	inline virtual void cal_Laserfootprint(LidPt_SurvInfo *pts, int Num) = 0;

	inline virtual void cal_Laserfootprint(orsPOINT3D *oriPts, orsPOINT3D *dstPts, int num) = 0;

//	inline virtual void cal_Laserfootprint(LidPt_XYZT *oriPts, LidPt_XYZT *dstPts, int num) = 0;

	//计算扫描斜距
	inline virtual void cal_RangeVec(LidPt_SurvInfo *pts, int Num) = 0;

	inline virtual void estimate_range_scanAngle(double *rangeVec, double *range, double *sAngle)=0;

	//计算偏导数
	inline virtual void calPDC_misalign(void *data, ors_int32 ptNum, 
		/*orsLidSysExtraParam_TYPE type,*/ void *extParam, double *pPDC) = 0;

	virtual bool setSRS(const char *hcsWkt, const char *vcsWkt) = 0;

public:
	ORS_INTERFACE_DEF( orsIObject, "LidarGeoModel" );
};

#define ORS_LIDAR_GEOMETRYMODEL				"ors.LidarGeoModel"
#define ORS_LIDAR_GEOMETRYMODEL_RIGOROUS	"ors.LidarGeoModel.Rigorous" 
#define ORS_LIDAR_GEOMETRYMODEL_RESSL		"ors.LidarGeoModel.Ressl"
#define ORS_LIDAR_GEOMETRYMODEL_YUAN		"ors.LidarGeoModel.Yuan"
#define ORS_LIDAR_GEOMETRYMODEL_JING		"ors.LidarGeoModel.Jing"
#define ORS_LIDAR_GEOMETRYMODEL_TRAJ		"ors.LidarGeoModel.Traj"

#endif