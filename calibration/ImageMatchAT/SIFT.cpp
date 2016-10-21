#include "StdAfx.h"
#include "ImageMatchAT/SIFT.H"
#include "orsBase/orsTypedef.h"
#include "orsBase/orsArray.h"
#include "orsBase/orsString.h"

#include <time.h>
#include "orsBase/orsIExe.h"
#include "orsBase/orsUtil.h"
#include "orsBase/orsIUtilityService.h"
#include "orsImage/orsIImageService.h"
#include "orsGuiBase/orsIProcessMsgBar.h"

ORS_GET_LOG_SERVICE_IMPL();
ORS_GET_UTILITY_SERVICE_IMPL();

#include "orsFeature2d/orsISE_Image2DFeatureDetector.h"
#include "orsFeature2d/orsIAlg2DFeatureRegistering.h"


bool SIFTDetector(orsArray<orsString> vStripFiles, int zoom)
{
	ors_int32	zoomOut = zoom;

	ors_int32 blkSizeX = 512, blkSizeY = 512;
	ors_int32 gridNumX = 0, gridNumY = 0;
	ors_int32 gridSizeX = 0, gridSizeY = 0;

	ref_ptr<orsIProcessMsg> pMsg = 
		ORS_PTR_CAST( orsIProcessMsg,  getPlatform()->createObject( ORS_PROCESSMSG_CMD ) );

	// 1，提取所有影像缩略图上和1:1的特征点
	if( true ) {
		ref_ptr<orsISimpleExe> sift = ORS_CREATE_OBJECT( orsISimpleExe,  ORS_IMAGE_2DFEATURE_DETECTOR_DEFAULT );

		if( NULL != sift.get() ) {
			// 提取SIFT特征点

			for( int iStrip=0; iStrip<vStripFiles.size(); iStrip++ )
			{	
				orsString outputFile;
				if(zoomOut > 1)
				{
					char pyrLevel[4];
					_itoa(zoomOut, pyrLevel, 10);
					
					outputFile = vStripFiles[iStrip] + _T("_zoomOut") + pyrLevel + _T(".sift.bin");
					
//					if( !getUtilityService()->CheckFileExist( outputFile ) ) 
					{
						ref_ptr<orsIProperty> prop = getPlatform()->createProperty();

						prop->addAttr( "inputImageFile",  vStripFiles[iStrip] );

						/*ors_int32	zoomOut = 16;*/
						prop->addAttr( "zoomOut", zoomOut  );

						prop->addAttr( "outputSIFTFile", outputFile );

						sift->initFromProperty( prop.get() );

						sift->execute( pMsg.get() );				
					}
				}
				if( true )
				{
					outputFile = vStripFiles[iStrip] + _T(".sift.bin");

//					if( !getUtilityService()->CheckFileExist( outputFile ) )	
					{
						ref_ptr<orsIProperty> prop = getPlatform()->createProperty();

						prop->addAttr( "inputImageFile",  vStripFiles[iStrip] );

						ors_int32	zoomOut = 1;
						prop->addAttr( "zoomOut", zoomOut  );

						prop->addAttr( "outputSIFTFile", outputFile );

						sift->initFromProperty( prop.get() );

						sift->execute( pMsg.get() );

					}
				}

				printf( "iRank %d: sift finished\n");
			}

		}
		else	{
			getPlatform()->logPrint( ORS_LOG_ERROR, "Can not create %s", ORS_IMAGE_2DFEATURE_DETECTOR_DEFAULT );
			assert(false);
		}	
	}


	return true;
}