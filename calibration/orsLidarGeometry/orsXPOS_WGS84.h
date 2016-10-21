#ifndef _ORS_LIDARGEOMETRY_POS_WGS84_INCLUDE_H_
#define _ORS_LIDARGEOMETRY_POS_WGS84_INCLUDE_H_

#include "orsLidarGeometry/orsIPOS.h"
#include "orsSRS/orsIEllipsoid.h"
#include "orsMath\orsIMatrixService.h"

//#include "orsImageGeometry/orsIImageGeometry.h"

class orsXPOS_WGS84:  public orsIPOS, public orsObjectBase 
{
private:
	orsImageGeometryCoordSysTYPE  m_rawCoordType;
	orsImageGeometryCoordSysTYPE  m_userCoordType;

	ref_ptr<orsISpatialReference>  m_orgCoordSys;
	ref_ptr<orsISpatialReference>  m_dstCoordSys;
	ref_ptr<orsITangentPlane>	m_pTangentPlane; 
	ref_ptr<orsIEllipsoid>	m_wgs84;

	ref_ptr<orsICoordinateTransform> m_coordTransform;	//for投影转换

	double m_Anchor_lat0, m_Anchor_lon0, m_Anchor_h0;
	double m_CentralMeridian;	//unit: radian
	double m_RRaw2User[9];

	ref_ptr<orsIMatrixService> m_matrixService;

public:
	orsXPOS_WGS84( bool bForRegister );
	virtual ~orsXPOS_WGS84();

	virtual bool SetRayCoordType( orsImageGeometryCoordSysTYPE coordType );

	virtual bool SetUserCoordType( orsImageGeometryCoordSysTYPE coordType );

	virtual void SetTangentPlane_AnchorPoint(double lat0, double long0, double h0);
	//设置中央经线（默认：meridian == long0）
	virtual void SetCentralMeridian(double meridian);

	virtual bool SetSRS( const char *hcsWkt, const char *vcsWkt );

	virtual void WGS84_To_TangentPlane(orsPOSParam *pOrgData, orsPOSParam *pDstData, ors_int32 num);

	virtual void WGS84_To_UTM(orsPOSParam *pOrgData, orsPOSParam *pDstData, ors_int32 num);

	virtual void WGS84_To_UTM(double vOrg[3], double ROrg[9], double vDst[3], double RDst[9]);

//	virtual void test_atn(ATNPoint *atnpt, double coord[3],orsPOINT3D *gps, double *Rn);

// 	virtual void position_transform(orsPOINT3D orgPt, orsPOINT3D &dstPt);
// 
// 	virtual void attitude_transform();

public:
	ORS_OBJECT_IMP1( orsXPOS_WGS84, orsIPOS, "WGS84", "WGS84" );
};

#endif