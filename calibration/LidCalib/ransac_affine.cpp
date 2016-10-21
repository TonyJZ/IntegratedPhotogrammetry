#include "StdAfx.h"
#include "ransac_affine.h"
#include <math.h>
#include "matrix.h"
#include "float.h"


bool EstimateTransform_Affine(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, int &refineNum, double *AffineMat, double Outlierth) 
{
	int min_matches = 3;
	int num_matches=ptNum;
	int max_inliers = 0;
	int match_idxs[4];
	double Mbest[9];
	double RANSACthresh=6.0*0.012;

	if (num_matches < min_matches)
	{
		printf("Cannot estimate transform, point num is not enough\n");
		return false;
	}

	//v3_t *r_pts = new v3_t[min_matches];
	//v3_t *l_pts = new v3_t[min_matches];
	v3_t r_pts[4];
	v3_t  l_pts[4];
	double weight[4];
	//	int ngcp;

	int i;
	// 	ngcp=0;
	// 	for(i=0; i<ptNum; i++)
	// 	{
	// 		if(pObvs[i])
	// 	}

	int nRANSAC=256;
	double min_rms=1.7e+308;
	for (int round = 0; round < nRANSAC; round++)
	{
		for (i = 0; i < min_matches; i++) 
		{
			bool found;
			int idx;

			do 
			{
				found = true;
				idx = rand() % num_matches;

				for (int j = 0; j < i; j++) 
				{
					if (match_idxs[j] == idx) 
					{
						found = false;
						break;
					}
				}
			} while (!found);

			match_idxs[i] = idx;
		}

		/* Solve for the motion */

		for (int i = 0; i < min_matches; i++) 
		{
			int idx=match_idxs[i];

			Vx(l_pts[i]) = pBuf1[idx].pt2D.x;
			Vy(l_pts[i]) = pBuf1[idx].pt2D.y;
			Vz(l_pts[i]) = 1.0;

			Vx(r_pts[i]) = pBuf2[idx].pt2D.x;
			Vy(r_pts[i]) = pBuf2[idx].pt2D.y;
			Vz(r_pts[i]) = 1.0;

			weight[i] = 1.0;
		}

		double Mcurr[9];

		align_affine(min_matches, r_pts, l_pts, Mcurr, 0);


		std::vector<int> inliers;
		double rms;
		int num_inliers = CountInliers(pBuf1, pBuf2, ptNum,  Mcurr, Outlierth, inliers, rms);

// 		if(rms<min_rms)
// 		{
// 			max_inliers = num_inliers;
// 			memcpy(Mbest, Mcurr, 9 * sizeof(double));
// 			min_rms=rms;
// 		}

		if (num_inliers > max_inliers) 
		{
			max_inliers = num_inliers;
			memcpy(Mbest, Mcurr, 9 * sizeof(double));
		}
	}
	
	double rms;
	std::vector<int> inliers;
	CountInliers(pBuf1, pBuf2, ptNum,  Mbest, Outlierth, inliers, rms);


// 	for(i=0; i<ptNum; i++)
// 	{
// 		if(pObvs[i].bGCP)
// 		{
// 			pOutlier[pObvs[i].ObjPtIdx]=false;
// 			continue;
// 		}
// 		pOutlier[pObvs[i].ObjPtIdx]=true;
// 	}

	for(i=0; i<inliers.size(); i++)
	{
		int j=inliers[i];
		pBuf1[j].flag=keypoint_inlier;
	}
	printf("inliers %d in %d\n", inliers.size(), ptNum);
	refineNum=inliers.size();
	// memcpy(Mout, Mbest, 9 * sizeof(double));


	//	delete [] r_pts;
	//	delete [] l_pts;
	//	delete [] weight;

	return true;
}

//X=[a,b,c,d,e,f]^T
void align_affine(int num_pts, v3_t *r_pts, v3_t *l_pts, double *Tout, int refine) 
{
	int m = num_pts * 2;/* Rows of A */
	int n = 6;				 /* Columns of A */
	int nrhs = 1;			/* Columns of X */
	int i, base;

	double *A = (double *)malloc(sizeof(double) * m * n);    /* Left-hand matrix */
	double *B = (double *)malloc(sizeof(double) * m * nrhs); /* Right-hand matrix */
	double xVec[6];

	double Ttmp[9];
	double T1[9], T2[9];

//#define _CONDITION_
#ifdef _CONDITION_
	/* Normalize the points */
	v3_t *r_pts_norm = condition_points(num_pts, r_pts, T1);
	v3_t *l_pts_norm = condition_points(num_pts, l_pts, T2);
	double T1inv[9];
#else
	v3_t *r_pts_norm = r_pts;
	v3_t *l_pts_norm = l_pts;
#endif

	for (i = 0; i < num_pts; i++) 
	{
		base = 2 * i * n;
		A[base + 0] = Vx(l_pts[i]);
		A[base + 1] = Vy(l_pts[i]);
		A[base + 2] = 1.0;
		A[base + 3] = A[base + 4] = A[base + 5] = 0.0;


		base = (2 * i + 1) * n;
		A[base + 0] = A[base + 1] = A[base + 2] = 0.0;
		A[base + 3] = Vx(l_pts[i]);
		A[base + 4] = Vy(l_pts[i]);
		A[base + 5] = 1.0;

		B[2 * i + 0] = Vx(r_pts[i]);
		B[2 * i + 1] = Vy(r_pts[i]);
		// 
		// 		A[i*6]=Vx(l_pts[i]);
		// 		A[i*6+1]=1;
		// 		A[i*6+2]=0;
		// 		A[i*6+3]=Vy(l_pts[i]);
		// 		A[i*6+4]=0;
		// 		A[i*6+5]=1;
	}

	/* Make the call to dgelsy */
	dgelsy_driver(A, B, xVec, m, n, nrhs);

	Tout[0] = xVec[0];  Tout[1] = xVec[1];  Tout[2] = xVec[2];
	Tout[3] = xVec[3];  Tout[4] = xVec[4];  Tout[5] = xVec[5];
	Tout[6] = 0.0;  Tout[7] = 0.0;  Tout[8] = 1.0;

	//	Tout[8] = 1.0;

	// #ifdef _CONDITION_
	// 	/* Undo normalization */
	// 	matrix_invert(3, T1, T1inv);
	// 
	// 	matrix_product(3, 3, 3, 3, T1inv, Tout, Ttmp);
	// 	matrix_product(3, 3, 3, 3, Ttmp, T2, Tout);
	// 
	// //	matrix_scale(3, 3, Tout, 1.0 / Tout[8], Tout);
	// #endif

	free(A);
	free(B);

#ifdef _CONDITION_
	free(r_pts_norm);
	free(l_pts_norm);
#endif
}


//#include "matrix/vector.h"
static v3_t *condition_points(int num_points, v3_t *pts, double *T) 
{
	v3_t *pts_new = (v3_t *) malloc(sizeof(v3_t) * num_points);

	v3_t mean = v3_mean(num_points, pts);
	double total_dist = 0.0;;
	double avg_dist;
	double factor;
	int i;

	for (i = 0; i < num_points; i++) {
		double dx = Vx(pts[i]) - Vx(mean);
		double dy = Vy(pts[i]) - Vy(mean);
		total_dist += sqrt(dx * dx + dy * dy);
	}

	avg_dist = total_dist / num_points;
	factor = sqrt(2.0) / avg_dist;

	for (i = 0; i < num_points; i++) {
		double x = factor * (Vx(pts[i]) - Vx(mean));
		double y = factor * (Vy(pts[i]) - Vy(mean));
		pts_new[i] = v3_new(x, y, 1.0);
	}

	//相似变换
	T[0] = factor;  T[1] = 0.0;     T[2] = -factor * Vx(mean);
	T[3] = 0.0;     T[4] = factor;  T[5] = -factor * Vy(mean);
	T[6] = 0.0;      T[7] = 0.0;    T[8] = 1.0;

	return pts_new;
}


//指定阈值挑点
static int CountInliers(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, double *M, double thresh, 
						std::vector<int> &inliers, double &rms)
//						const std::vector<Keypoint> &k1, 
// 						const std::vector<Keypoint> &k2, 
// 						std::vector<KeypointMatch> matches,
// 						double *M, double thresh, std::vector<int> &inliers)
{
	inliers.clear();
	int count = 0;
	
	rms=0;
	for (unsigned int i = 0; i < ptNum; i++)
	{
		/* Determine if the ith feature in f1, when transformed by M,
		* is within RANSACthresh of its match in f2 (if one exists)
		*
		* if so, increment count and append i to inliers */

		double p[3];

		p[0] = pBuf1[i].pt2D.x;
		p[1] = pBuf1[i].pt2D.y;
		p[2] = 1.0;

		double q[3];
		matrix_product(3, 3, 3, 1, M, p, q);

		double qx = q[0] ;
		double qy = q[1] ;

		double dx = qx - pBuf2[i].pt2D.x;
		double dy = qy - pBuf2[i].pt2D.y;

		double dist2 = dx * dx + dy * dy;
//		rms+=dist2;

		if(sqrt(dist2)<thresh)
		{
			count++;
			inliers.push_back(i);
		}
	}

	return count;
}