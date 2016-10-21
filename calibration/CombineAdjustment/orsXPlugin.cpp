//////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "orsMath/orsIMatrixService.h"
#include "orsSRS/orsISpatialReferenceService.h"
#include "orsBase\orsIUtilityService.h"

ORS_GET_UTILITY_SERVICE_IMPL();

ORS_GET_SRS_SERVICE_IMPL();

ORS_GET_MATRIX_SERVICE_IMPL();



//////////////////////////////////////////////////////////////////////////

#include "CombineAdjustment/IntegratedAT.h"

orsIObject *createAlgIntegratedAT( bool bForRegister )
{
	return new orsAlgIntegratedAT( bForRegister );
}


class orsXPlugin: public orsIPlugin
{
public:
	virtual const orsChar *getID()
	{
		return "org.openRS.Algorithm.CombineAdjustment";
	};

	virtual	const orsChar *getName()
	{
		return "orsAlgCombineAdjustment";
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

		pReg->registerObject( createAlgIntegratedAT );
		
		return true;
	}

	virtual void finalize()
	{

	}
};

ORS_REGISTER_PLUGIN( orsXPlugin )


