//////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "orsMath/orsIMatrixService.h"
#include "orsSRS/orsISpatialReferenceService.h"
#include "orsBase\orsIUtilityService.h"

ORS_GET_UTILITY_SERVICE_IMPL();

ORS_GET_SRS_SERVICE_IMPL();

ORS_GET_MATRIX_SERVICE_IMPL();



//////////////////////////////////////////////////////////////////////////

#include "orsXPos_WGS84.h"
orsIObject *createPOS( bool bForRegister )
{
	return new orsXPOS_WGS84( bForRegister );
}

#include "trajectory/orsXTrajectory.h"
orsIObject *createTrajectory( bool bForRegister )
{
	return new orsXTrajectory( bForRegister );
}

#include "trajectory/orsXTrajectory_trj.h"
orsIObject *createTrajectory_trj( bool bForRegister )
{
	return new orsXTrajectory_trj( bForRegister );
}

#include "orsXLidarGeoModel_Rigorous.h"
orsIObject *createLidarGeoModel_Rigorous( bool bForRegister )
{
	return new orsXLidarGeoModel_Rigorous( bForRegister );
}

#include "orsXLidarGeoModel_Ressl.h"
orsIObject *createLidarGeoModel_Ressl( bool bForRegister )
{
	return new orsXLidarGeoModel_Ressl( bForRegister );
}

#include "orsXLidarGeoModel_Yuan.h"
orsIObject *createLidarGeoModel_Yuan( bool bForRegister )
{
	return new orsXLidarGeoModel_Yuan( bForRegister );
}

#include "orsXLidarGeoModel_Jing.h"
orsIObject *createLidarGeoModel_Jing( bool bForRegister )
{
	return new orsXLidarGeoModel_Jing( bForRegister );
}

#include "orsXLidarGeoModel_Traj.h"
orsIObject *createLidarGeoModel_Traj( bool bForRegister )
{
	return new orsXLidarGeoModel_Traj( bForRegister );
}

class orsXPlugin: public orsIPlugin
{
public:
	virtual const orsChar *getID()
	{
		return "org.openRS.LidarProcess";
	};

	virtual	const orsChar *getName()
	{
		return "orsXLidarGeometryService";
	};

	virtual const orsChar *getProvider()
	{
		return "edu.whu.liesmars";
	};

	virtual const orsChar *getVersion()
	{
		return "0.1";
	}

	virtual  bool initialize(orsIPlatform*  platform)
	{
//		printf("orsImageGeometry initialize\n");
		orsIRegisterService *pReg = platform->getRegisterService();

		pReg->registerObject( createPOS );
		pReg->registerObject( createTrajectory );
		pReg->registerObject( createTrajectory_trj );
		pReg->registerObject( createLidarGeoModel_Rigorous );
		pReg->registerObject( createLidarGeoModel_Ressl );
		pReg->registerObject( createLidarGeoModel_Yuan );
		pReg->registerObject( createLidarGeoModel_Jing );
		pReg->registerObject( createLidarGeoModel_Traj );

		return true;
	}

	virtual void finalize()
	{

	}
};

ORS_REGISTER_PLUGIN( orsXPlugin )


