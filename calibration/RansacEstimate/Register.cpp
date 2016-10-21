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

/* Register.cpp */
/* Compute relationships between images */

#include "StdAfx.h"
#include <assert.h>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//#include "keys.h"
#include "RansacEstimate/Register.h"



#include "bundler/homography.h"
#include "bundler/horn.h"
//#include "matrix.h"
#include "bundler/tps.h"
//#include "bundler/bundler_vector.h"

static int CountInliers(myPtPair *matchPair, int pairNum,
			double *M, double thresh, std::vector<int> &inliers);

static int LeastSquaresFit(myPtPair *matchPair, int pairNum, MotionModel mm,
			   const std::vector<int> &inliers, double *M);

/* Estimate a transform between two sets of keypoints */
std::vector<int> EstimateTransform(myPtPair *matchPair, int pairNum, int &refineNum, 
				   MotionModel mm,
				   int nRANSAC, double RANSACthresh, 
				   double *Mout) 
{
    int min_matches = -1;
    switch (mm) {
	case MotionRigid:
	    min_matches = 3;
	    break;
	case MotionHomography:
	    min_matches = 4;
	    break;
    }

    int *match_idxs = new int[min_matches];

    int num_matches = pairNum/*(int) matches.size()*/;
    int max_inliers = 0;
    double Mbest[9];
    
    if (num_matches < min_matches) {
	std::vector<int> empty;
	printf("Cannot estimate rigid transform\n");
	return empty;
    }

    v3_t *r_pts = new v3_t[min_matches];
    v3_t *l_pts = new v3_t[min_matches];
    double *weight = new double[min_matches];

    for (int round = 0; round < nRANSAC; round++) {
	for (int i = 0; i < min_matches; i++) {
	    bool found;
	    int idx;
	    
	    do {
		found = true;
		idx = rand() % num_matches;
		
		for (int j = 0; j < i; j++) {
		    if (match_idxs[j] == idx) {
			found = false;
			break;
		    }
		}
	    } while (!found);

	    match_idxs[i] = idx;
	}

	/* Solve for the motion */
		
	for (int i = 0; i < min_matches; i++) {
// 	    int idx1 = matches[match_idxs[i]].m_idx1;
// 	    int idx2 = matches[match_idxs[i]].m_idx2;
	    
	    Vx(l_pts[i]) = matchPair[match_idxs[i]].xi0/*k1[idx1].m_x*/;
	    Vy(l_pts[i]) = matchPair[match_idxs[i]].yi0;
	    Vz(l_pts[i]) = 1.0;
		    
	    Vx(r_pts[i]) = matchPair[match_idxs[i]].xi1;
	    Vy(r_pts[i]) = matchPair[match_idxs[i]].yi1;
	    Vz(r_pts[i]) = 1.0;

	    weight[i] = 1.0;
	}

	double Mcurr[9];

	switch (mm) {
	    case MotionRigid: {
		double R[9], T[9], Tout[9], scale;
		align_horn(min_matches, r_pts, l_pts, R, T, Tout, &scale, weight);
		memcpy(Mcurr, Tout, 9 * sizeof(double));
		break;
	    }
		
	    case MotionHomography: {
		align_homography(min_matches, r_pts, l_pts, Mcurr, 0);
		break;
	    }
	}
		

	std::vector<int> inliers;
	int num_inliers = CountInliers(matchPair, pairNum, Mcurr, 
				       RANSACthresh, inliers);

	if (num_inliers > max_inliers) {
	    max_inliers = num_inliers;
	    memcpy(Mbest, Mcurr, 9 * sizeof(double));
	}
    }

    std::vector<int> inliers;
    CountInliers(matchPair, pairNum, Mbest, RANSACthresh, inliers);
    memcpy(Mout, Mbest, 9 * sizeof(double));
    LeastSquaresFit(matchPair, pairNum, mm, inliers, Mout);

    // memcpy(Mout, Mbest, 9 * sizeof(double));

    delete [] match_idxs;
    delete [] r_pts;
    delete [] l_pts;
    delete [] weight;

    return inliers;
}

static int CountInliers(myPtPair *matchPair, int pairNum,
			double *M, double thresh, std::vector<int> &inliers)
{
    inliers.clear();
    int count = 0;

    for (unsigned int i = 0; i < pairNum; i++) {
	/* Determine if the ith feature in f1, when transformed by M,
	 * is within RANSACthresh of its match in f2 (if one exists)
	 *
	 * if so, increment count and append i to inliers */

	double p[3];

	p[0] = matchPair[i].xi0;
	p[1] = matchPair[i].yi0;
	p[2] = 1.0;

	double q[3];
	matrix_product(3, 3, 3, 1, M, p, q);

	double qx = q[0] / q[2];
	double qy = q[1] / q[2];

	double dx = qx - matchPair[i].xi1;
	double dy = qy - matchPair[i].yi1;
	
	double dist = sqrt(dx * dx + dy * dy);
	
	if (dist <= thresh) {
	    count++;
	    inliers.push_back(i);
	}
    }

    return count;
}

static int LeastSquaresFit(myPtPair *matchPair, int pairNum, MotionModel mm,
			   const std::vector<int> &inliers, double *M)
{
    v3_t *r_pts = new v3_t[inliers.size()];
    v3_t *l_pts = new v3_t[inliers.size()];
    double *weight = new double[inliers.size()];

    /* Compute residual */
    double error = 0.0;
    for (int i = 0; i < (int) inliers.size(); i++) {
// 	int idx1 = matches[inliers[i]].m_idx1;
// 	int idx2 = matches[inliers[i]].m_idx2;
	
	double r[3], l[3];
	l[0] = matchPair[inliers[i]].xi0;
	l[1] = matchPair[inliers[i]].yi0;
	l[2] = 1.0;
		    
	r[0] = matchPair[inliers[i]].xi1;
	r[1] = matchPair[inliers[i]].yi1;
	r[2] = 1.0;	

	double rp[3];
	matrix_product(3, 3, 3, 1, M, l, rp);
	
	rp[0] /= rp[2];
	rp[1] /= rp[2];
	
	double dx = rp[0] - r[0];
	double dy = rp[1] - r[1];
	
	error += dx * dx + dy * dy;
    }

    printf("[LeastSquaresFit] Residual error (before) is %0.3e\n", error);    


    for (int i=0; i < (int) inliers.size(); i++) {
// 	int idx1 = matches[inliers[i]].m_idx1;
// 	int idx2 = matches[inliers[i]].m_idx2;
	
	Vx(l_pts[i]) = matchPair[inliers[i]].xi0;
	Vy(l_pts[i]) = matchPair[inliers[i]].yi0;
	Vz(l_pts[i]) = 1.0;
		    
	Vx(r_pts[i]) = matchPair[inliers[i]].xi1;
	Vy(r_pts[i]) = matchPair[inliers[i]].yi1;
	Vz(r_pts[i]) = 1.0;

	weight[i] = 1.0;
    }
    
    switch (mm) {
	case MotionRigid: {
	    double R[9], T[9], Tout[9], scale;
	    align_horn((int) inliers.size(), r_pts, l_pts, R, T, Tout, &scale, weight);
	    memcpy(M, Tout, 9 * sizeof(double));
	    break;
	}
	
	case MotionHomography: {
	    align_homography((int) inliers.size(), r_pts, l_pts, M, 1);
	    break;
	}
    }

    /* Compute residual */
    error = 0.0;
    for (int i = 0; i < (int) inliers.size(); i++) {
// 	int idx1 = matches[inliers[i]].m_idx1;
// 	int idx2 = matches[inliers[i]].m_idx2;
	
	double r[3], l[3];
	l[0] = matchPair[inliers[i]].xi0;
	l[1] = matchPair[inliers[i]].yi0;
	l[2] = 1.0;
		    
	r[0] = matchPair[inliers[i]].xi1;
	r[1] = matchPair[inliers[i]].yi1;
	r[2] = 1.0;	

	double rp[3];
	matrix_product(3, 3, 3, 1, M, l, rp);
	
	rp[0] /= rp[2];
	rp[1] /= rp[2];
	
	double dx = rp[0] - r[0];
	double dy = rp[1] - r[1];
	
	error += dx * dx + dy * dy;
    }
	
    printf("[LeastSquaresFit] Residual error (after) is %0.3e\n", error);    

    delete [] r_pts;
    delete [] l_pts;
    delete [] weight;

    return 0;
}
