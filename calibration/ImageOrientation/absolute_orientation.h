#ifndef _IMAGE_ORIENTATION_ABOR_H_
#define _IMAGE_ORIENTATION_ABOR_H_

#include "orsBase\orsTypedef.h"
#include "Calib_Camera.h"

#ifdef IMAGEORIENTATION_EXPORTS
#define  _ImageOr_Dll_  __declspec(dllexport)
#else
#define  _ImageOr_Dll_  __declspec(dllimport)	
#endif



bool _ImageOr_Dll_ EstimateTransform_Projection(orsPOINT3D *p3DBuf, orsPOINT2Df *p2DBuf, int ptNum, int &refineNum, double P[12],
	_iphCamera *camera, double thresh=1.0) ;	//thresh ÏñËØ


#endif