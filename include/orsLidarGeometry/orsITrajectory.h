#ifndef _OGR_PLATFORM_TRAJECTORY_INTERFACE_H_
#define _OGR_PLATFORM_TRAJECTORY_INTERFACE_H_


#include "orsBase/orsIPlatform.h"

#include "orsBase/orsMatrix.h"

#include "orsLidarGeometry\LidGeo_def.h"

#include <vector>

//#include "orsImageGeometry/orsIImageGeometry.h"

struct orsITrajectory : public orsIObject
{
//	virtual int	GetOrder() const {	return 1;	};
public:
	virtual bool Initialize( const orsChar *imageFileName ) = 0;

	virtual bool Initialize(std::vector<orsPOSParam> *posVec) = 0;

	//内插外方位元素
	virtual void Interpolate_linear(double t, orsPOINT3D *gps_pos, double *R) = 0;

	//直接对所需要的POS进行内插
	virtual void Interpolate_linear(orsPOSParam *pos_t, orsPOSParam *pos0, orsPOSParam *pos1) = 0;

// 	virtual void GetXsYsZs( double t, double *Xs, double *Ys, double *Zs ) = 0;
// 
// 	virtual void GetRotateMatrix( double t, double *R ) = 0;

//	virtual void GetPhotoSpaceCoord( double xi, double yi, double w, double *u, double *v ) { assert(false); }
	//设置切平面的原点(单位：弧度)
	virtual void SetTangentPlane_AnchorPoint(double lat0, double long0, double h0) = 0;
	//设置中央经线（默认：meridian == long0）
	virtual void SetCentralMeridian(double meridian) = 0;

public:
	ORS_INTERFACE_DEF( orsIObject, _T("trajectory" ) );

};

#define ORS_LIDARGEOMETRY_TRAJECTORY			"ors.trajectory"
#define ORS_LIDARGEOMETRY_TRAJECTORY_DEFAULT	"ors.trajectory.default" 
#define ORS_LIDARGEOMETRY_TRAJECTORY_TRJ	"ors.trajectory.trj" 

#endif