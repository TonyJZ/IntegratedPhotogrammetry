/* 
 *  Copyright (c) 2008  Noah Snavely (snavely (at) cs.washington.edu)
 *    and the University of Washington
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

/* Register.h */
/* Compute relationships between images */

#ifndef __register_h__
#define __register_h__

#include <vector>
#include "PointPair_Struct.h"

#ifdef RANSACESTIMATE_EXPORTS
#define  _estimate_Dll_  __declspec(dllexport)
#else
#define  _estimate_Dll_  __declspec(dllimport)	
#endif
 
//#include "keys.h"

enum MotionModel {
    MotionRigid,
    MotionHomography,
};

/* Estimate a transform between two sets of keypoints */
std::vector<int> _estimate_Dll_ EstimateTransform(myPtPair *matchPair, int pairNum, int &refineNum,
				   MotionModel mm,
				   int nRANSAC, double RANSACthresh, 
				   double *Mout);



#endif /* __register_h__ */
