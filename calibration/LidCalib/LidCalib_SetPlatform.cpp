#include "LidCalib/LidCalib_SetPlatform.h"
#include "orsBase/orsIPlatform.h"

orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

void LidCalib_SetPlatform(orsIPlatform *pPlatform)
{
	g_pPlatform = pPlatform;
}