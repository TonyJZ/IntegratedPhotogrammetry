#ifndef __LIDCALIB_SET_PLATFORM_H__
#define __LIDCALIB_SET_PLATFORM_H__

#include "orsBase/orsTypedef.h"

#ifdef _LIDCALIB_
#define  _lidCalib_Dll_  __declspec(dllexport)
#else
#define  _lidCalib_Dll_  __declspec(dllimport)	
#endif

struct orsIPlatform;

void _lidCalib_Dll_ LidCalib_SetPlatform(orsIPlatform *pPlatform);

#endif