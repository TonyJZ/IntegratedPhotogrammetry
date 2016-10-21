#ifndef _ORS_LIDARGEOMETRY_POS_INTERFACE_H_
#define _ORS_LIDARGEOMETRY_POS_INTERFACE_H_

#include "orsBase/orsTypedef.h"
#include "orsBase/orsIObject.h"
#include "orsImageGeometry/orsIImageGeometry.h"
#include "orsLidarGeometry/LidGeo_def.h"

//#include "lastools/ATN_def.h"

interface orsIPOS : public orsIObject
{
public:
	virtual bool SetRayCoordType( orsImageGeometryCoordSysTYPE coordType ) = 0;

	virtual bool SetUserCoordType( orsImageGeometryCoordSysTYPE coordType ) = 0;

	//unit: radian
	virtual void SetTangentPlane_AnchorPoint(double lat0, double long0, double h0) = 0;
	//设置中央经线（默认：meridian == long0）unit:radian
	virtual void SetCentralMeridian(double meridian) = 0;

	virtual bool SetSRS( const char *hcsWkt, const char *vcsWkt ) = 0;

	//wgs84转切平面
	virtual void WGS84_To_TangentPlane(orsPOSParam *pOrgData, orsPOSParam *pDstData, ors_int32 num) = 0;

	virtual void WGS84_To_UTM(orsPOSParam *pOrgData, orsPOSParam *pDstData, ors_int32 num) = 0;

	virtual void WGS84_To_UTM(double vOrg[3], double ROrg[9], double vDst[3], double RDst[9]) = 0;

//	virtual void test_atn(ATNPoint *atnpt, double coord[3], orsPOINT3D *gps, double *Rn) = 0;

// 	virtual void position_transform(orsPOINT3D orgPt, orsPOINT3D &dstPt) = 0;
// 
// 	virtual void attitude_transform() = 0;

public:
	ORS_INTERFACE_DEF( orsIObject, "POS" );
};

#define ORS_LIDARGEOMETRY_POS			"ors.POS"
#define ORS_LIDARGEOMETRY_POS_WGS84	"ors.POS.WGS84" 

#endif