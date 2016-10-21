
#if !defined(_Ors_Trajectory_H_INCLUDED_)
#define _Ors_Trajectory_H_INCLUDED_


#include "orsSRS/orsIEllipsoid.h"
#include "orsLidarGeometry/orsITrajectory.h"
#include "orsLidarGeometry/orsIPOS.h"

class orsXTrajectory:  public orsITrajectory, public orsObjectBase 
{
private:
	std::vector<orsPOSParam> *m_posVec;

	ref_ptr<orsIPOS>	m_posTrans;

	bool	m_bTransform;

private:
// 	bool read_trjfile(const char *filename);
// 	void findTimeSlice(double t, int &sIdx, int &eIdx);

	void interpolationXYZ_Lagrange(double t, double t0, double t1, double v0[3], double v1[3], double v[3]);
	void interpolationRotate_Lagrange(double t, double t0, double t1, double R0[9], double R1[9], double R[9]);
//	void interpolationRotate_Lagrange_dpsMatrix(double t, double t0, double t1, double v0[3], double v1[3], double v[3]);

// 	void coordinate_transform(double pos_from[3], double pos_to[3]);
// 	void attitude_transform(double R_from[9], double lat, double lon, double R_to[9]);

public:
	orsXTrajectory( bool bForRegister );
	virtual ~orsXTrajectory();

	virtual bool Initialize( const orsChar *imageFileName ) {return false;};
	virtual bool Initialize(std::vector<orsPOSParam> *posVec);

	virtual int	GetOrder() const { return 1; };

// 	virtual void GetXsYsZs( double t, double *Xs, double *Ys, double *Zs );
// 	virtual void GetRotateMatrix( double t, double *R );

	virtual void Interpolate_linear(double t, orsPOINT3D *gps_pos, double *R) {assert(false);};

	virtual void Interpolate_linear(orsPOSParam *pos_t, orsPOSParam *pos0, orsPOSParam *pos1);


	//设置坐标系原点（切平面或投影坐标系）
	virtual void SetTangentPlane_AnchorPoint(double lat0, double long0, double h0);
	virtual void SetCentralMeridian(double meridian) { m_posTrans->SetCentralMeridian(meridian); };


public:
	ORS_OBJECT_IMP1( orsXTrajectory, orsITrajectory, "default", "default" );
};


//gDPS_DLL_API orsXStereoModel_refImg *LoadStereoModel( const char *modelName );

#endif
