#ifndef _ORS_LIDARGEOMETRY_MODEL_RIGOROUS_INCLUDE_H_
#define _ORS_LIDARGEOMETRY_MODEL_RIGOROUS_INCLUDE_H_

#include "orsLidarGeometry/orsILidarGeoModel.h"
#include "orsSRS/orsIEllipsoid.h"
#include "orsSRS\orsISpatialReference.h"
#include "orsMath\orsIMatrixService.h"

class orsXLidarGeoModel_Rigorous:  public orsILidarGeoModel, public orsObjectBase
{
private:
	ref_ptr<orsITangentPlane>	m_pTangentPlane; 

	ref_ptr<orsISpatialReference>  m_orgCoordSys;
	ref_ptr<orsISpatialReference>  m_dstCoordSys;
	ref_ptr<orsICoordinateTransform> m_coordTransform;	//for投影转换

//	ref_ptr<orsISpatialReference> m_pSRS;

	ref_ptr<orsIEllipsoid>	m_wgs84;
	ref_ptr<orsIMatrixService> m_matrixService;

	orsLidSysParam	*m_extParam;
//	orsLidSysExtraParam_TYPE	m_type;
	double m_leverarm[3];	//偏心分量
	double m_sa0, m_sac;	//扫描角误差，扫描角线性改正
	double m_semiXYZ[3];
	double m_drange;		//测距误差
	double m_Rboresight[9];

public:
	orsXLidarGeoModel_Rigorous(bool bForRegister);
	virtual ~orsXLidarGeoModel_Rigorous();

	virtual void setSysParam(void *extParam);

	//计算激光脚点坐标
	inline virtual void cal_Laserfootprint(LidGeo_RawPoint *pdata, ors_int32 ptNum);

	inline virtual void cal_Laserfootprint(orsPOINT3D *gps_pos, double Rn[9], double range, double sAngle, 
		double *X, double *Y, double *Z);

	inline virtual void cal_Laserfootprint(LidPt_SurvInfo *pts, int Num);

	inline virtual void cal_Laserfootprint(orsPOINT3D *oriPts, orsPOINT3D *dstPts, int num)
	{
		assert(0);
	};

// 	inline virtual void cal_Laserfootprint(LidPt_XYZT *oriPts, LidPt_XYZT *dstPts, int num)
// 	{
// 		assert(0);
// 	};
	//计算扫描斜距
	inline virtual void cal_RangeVec(LidPt_SurvInfo *pts, int Num);


	inline virtual void estimate_range_scanAngle(double *rangeVec, double *range, double *sAngle);

	//计算偏导数
	inline virtual void calPDC_misalign(void *data, ors_int32 ptNum, 
		/*orsLidSysExtraParam_TYPE type, */void *extParam, double *pPDC);

	virtual bool setSRS(const char *hcsWkt, const char *vcsWkt);

public:
	ORS_OBJECT_IMP1( orsXLidarGeoModel_Rigorous, orsILidarGeoModel, "Rigorous", "Rigorous" );
};


#endif