#ifndef __Image_Match_API_H__
#define __Image_Match_API_H__

#include "orsBase/orsTypedef.h"

#ifdef _ImageMatchAT_
#define  _Image_Match_  __declspec(dllexport)
#else
#define  _Image_Match_  __declspec(dllimport)	
#endif

struct orsIPlatform;

void _Image_Match_ ImageMatchSetPlatform(orsIPlatform *pPlatform);

#endif