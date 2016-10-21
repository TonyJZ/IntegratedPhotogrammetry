#include "stdafx.h"

#include "orsBase/orsIPlatform.h"
//#include "orsXPointCloudService.h"
//#include "orsBase\orsIUtilityService.h"
#include "orsBase/orsIPlugin.h"
#include "orsBase/orsIRegisterService.h"

/*orsIPlatform *g_platform = NULL;*/

#include "atnReader.h"
orsIObject* createAtnReader( bool bForRegister )
{
	CATNReader *reader = new CATNReader(bForRegister);
	return reader;
}

#include "ATNwriter.h"
orsIObject* createAtnWriter( bool bForRegister )
{
	CATNWriter *writer = new CATNWriter(bForRegister);
	return writer;
}

// orsIPointCloudService *getPointCloudService()
// {
// 	orsIPointCloudService *pointService = 
// 		ORS_PTR_CAST(orsIPointCloudService, getPlatform()->getService( ORS_SERVICE_POINTCLOUD) );
// 	
// 	assert( pointService );
// 	
// 	return pointService;
// }

class orsXPlugin: public orsIPlugin
{
public:
	virtual const orsChar *getID()	
	{
		return _T("ors.service.pointcloud"); 
	};

	virtual	const orsChar *getName()	
	{	
		return _T("PointCloud Service"); 
	};
	
	virtual const orsChar *getProvider() 
	{	
		return _T("edu.whu.liesmars"); 
	};
	
	virtual const orsChar *getVersion()
	{
		return _T("0.1");
	}
	
	virtual  bool initialize(orsIPlatform*  platform)
	{
		orsIRegisterService *pReg = platform->getRegisterService();

		pReg->registerObject( createAtnReader );
		pReg->registerObject( createAtnWriter );

	
		return true;
	}
	
	virtual void finalize()
	{
		
	}
};

ORS_REGISTER_PLUGIN( orsXPlugin )


