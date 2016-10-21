#ifndef _Image_Match_AT_SIFT_H__
#define _Image_Match_AT_SIFT_H__

#ifdef _ImageMatchAT_
#define  _Image_Match_  __declspec(dllexport)
#else
#define  _Image_Match_  __declspec(dllimport)	
#endif

#include "orsBase/orsTypedef.h"
#include "orsBase/orsArray.h"
#include "orsBase/orsString.h"

bool _Image_Match_ SIFTDetector(orsArray<orsString> vFiles, int zoom=1);


#endif