#ifndef __LIDQC_API_H_
#define __LIDQC_API_H_

#include "orsBase/orsTypedef.h"


#ifdef _LIDQC_
#define  _lidQC_Dll_  __declspec(dllexport)
#else
#define  _lidQC_Dll_  __declspec(dllimport)	
#endif

#endif

struct orsIPlatform;

void _lidQC_Dll_ FastDspSetPlatform(orsIPlatform *pPlatform);



#endif
