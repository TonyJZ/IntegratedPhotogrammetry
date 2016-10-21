#include "StdAfx.h"
#include "ImageMatchAT/ImageMatchAPI.h"


#include "orsImage/orsIImageService.h"
#include "orsGeometry/orsIGeometryService.h"
#include "orsImageGeometry/orsIImageGeometryService.h"
#include "orsSRS/orsISpatialReferenceService.h"
#include "orsMap/orsIMapService.h"

orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}


ORS_GET_GEOMETRY_SERVICE_IMPL();
ORS_GET_SRS_SERVICE_IMPL();
ORS_GET_MAP_SERVICE_IMPL();
ORS_GET_IMAGE_SERVICE_IMPL();
ORS_GET_IMAGEGEOMETRY_SERVICE_IMPL();

void ImageMatchSetPlatform(orsIPlatform *pPlatform)
{
	g_pPlatform = pPlatform;
}