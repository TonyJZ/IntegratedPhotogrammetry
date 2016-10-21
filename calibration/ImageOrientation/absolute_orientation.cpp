#include "StdAfx.h"
#include "absolute_orientation.h"

#include "math.h"
#include "bundler\bundler_vector.h"
#include "bundler\matrix.h"
#include <stdio.h>

/* Solve for a 3x4 projection matrix, given a set of 3D points and 2D
 * projections */
int find_projection_3x4(int num_pts, v3_t *points, v2_t *projs, double *P) {
    if (num_pts < 6) {
	printf("[find_projection_3x4] Need at least 6 points!\n");
	return -1;
    } else {

	// #define _CONDITION_
#ifdef _CONDITION_
	double Tpoints[16];
	v3_t *points_new = condition_points_3D(num_pts, points, Tpoints);
	
	double Tprojs[9];
	v2_t *projs_new = condition_points_2D(num_pts, projs, Tprojs);

	double Tprojs_inv[9];
	double Ptmp[12];
#else
	v3_t *points_new = points;
	v2_t *projs_new = projs;
#endif

	int num_eqns = 2 * num_pts;
	int num_vars = 11;

	double *A = (double *)malloc(sizeof(double) * num_eqns * num_vars);
	double *b = (double *)malloc(sizeof(double) * num_eqns);
	double X[11];

	double error = 0.0;
    
	int i;

	for (i = 0; i < num_pts; i++) {
	    double *row1 = A + 2 * i * num_vars;
	    double *row2 = A + (2 * i + 1) * num_vars;
	    
	    row1[0]  = Vx(points_new[i]);
	    row1[1]  = Vy(points_new[i]);
	    row1[2]  = Vz(points_new[i]);
	    row1[3]  = 1.0;
	
	    row1[4]  = 0.0;
	    row1[5]  = 0.0;
	    row1[6]  = 0.0;
	    row1[7]  = 0.0;
	
            // EDIT!!!
	    row1[8]  = Vx(projs_new[i]) * Vx(points_new[i]);
	    row1[9]  = Vx(projs_new[i]) * Vy(points_new[i]);
	    row1[10] = Vx(projs_new[i]) * Vz(points_new[i]);
	
	    b[2 * i] = -Vx(projs_new[i]);


	    row2[0]  = 0.0;
	    row2[1]  = 0.0;
	    row2[2]  = 0.0;
	    row2[3]  = 0.0;

	    row2[4]  = Vx(points_new[i]);
	    row2[5]  = Vy(points_new[i]);
	    row2[6]  = Vz(points_new[i]);
	    row2[7]  = 1.0;	

            // EDIT!!!
	    row2[8]  = Vy(projs_new[i]) * Vx(points_new[i]);
	    row2[9]  = Vy(projs_new[i]) * Vy(points_new[i]);
	    row2[10] = Vy(projs_new[i]) * Vz(points_new[i]);
	
	    b[2 * i + 1] = -Vy(projs_new[i]);
	}

	dgelsy_driver(A, b, X, num_eqns, num_vars, 1);

	memcpy(P, X, sizeof(double) * 11);
	P[11] = 1.0;

#ifdef _CONDITION_
	matrix_invert(3, Tprojs, Tprojs_inv);
	matrix_product(3, 3, 3, 4, Tprojs_inv, P, Ptmp);
	matrix_product(3, 4, 4, 4, Ptmp, Tpoints, P);
	
	matrix_scale(3, 4, P, 1.0 / P[11], P);
#endif

	for (i = 0; i < num_pts; i++) {
	    double pt[4] = { Vx(points[i]), 
			     Vy(points[i]), 
			     Vz(points[i]), 1.0 };
	    double pr[3];
	    double dx, dy, dist;

	    matrix_product341(P, pt, pr);
            // EDIT!!!
	    pr[0] /= -pr[2];
	    pr[1] /= -pr[2];
	    
	    dx = pr[0] - Vx(projs[i]);
	    dy = pr[1] - Vy(projs[i]);

	    dist = dx * dx + dy * dy;

	    error += dist;
	}

	// printf("[find_projection_3x4] Average error is %0.3f\n", 
	//       error / num_pts);

	free(A);
	free(b);

#ifdef _CONDITION_
	free(points_new);
	free(projs_new);
#endif

	return 0;
    }
}

static int global_num_pts;
static v3_t *global_points;
static v2_t *global_projs;

static void projection_residual(const int *m, const int *n, double *x, 
				double *fvec, double *iflag) 
{
    int i;
    
    double P[12];
    memcpy(P, x, sizeof(double) * 11);
    P[11] = 1.0;

    for (i = 0; i < global_num_pts; i++) {
	double pt[4] = { Vx(global_points[i]), 
			 Vy(global_points[i]), 
			 Vz(global_points[i]), 1.0 };

	double pr[3];
	double dx, dy;
	
	matrix_product341(P, pt, pr);
        // EDIT!!
	pr[0] /= -pr[2];
	pr[1] /= -pr[2];
	    
	dx = pr[0] - Vx(global_projs[i]);
	dy = pr[1] - Vy(global_projs[i]);

	fvec[2 * i + 0] = dx;
	fvec[2 * i + 1] = dy;
    }
}

/* Solve for a 3x4 projection matrix, given a set of 3D points and 2D
 * projections using non-linear optimization */
int find_projection_3x4_nonlinear(int num_pts, v3_t *points, v2_t *projs, 
				  double *Pin, double *Pout) 
{
    if (num_pts < 6) {
	printf("[find_projection_3x4_nonlinear] Need at least 6 points!\n");
	return -1;
    } else {
	int num_eqns = 2 * num_pts;
	int num_vars = 11;
	double x[11];

	global_num_pts = num_pts;
	global_points = points;
	global_projs = projs;

	memcpy(x, Pin, sizeof(double) * 11);
	lmdif_driver(projection_residual, num_eqns, num_vars, x, 1.0e-5);

	memcpy(Pout, x, sizeof(double) * 11);
	Pout[11] = 1.0;

	return 0;
    }
}


/* Solve for a 3x4 projection matrix using RANSAC, given a set of 3D
 * points and 2D projections */
int find_projection_3x4_ransac(int num_pts, v3_t *points, v2_t *projs, 
			       double *P, int ransac_rounds, double ransac_threshold, int *inliers, int &num_inliers) 
{
    if (num_pts < 6) 
	{
		printf("[find_projection_3x4_ransac] Error: need at least 6 points!\n");
		return -1;
    } 

#define MIN_PTS 6
	// const int min_pts = 6;
//	int *inliers = (int *) malloc(sizeof(int) * num_pts);
	int indices[MIN_PTS];
	int round, i, j;
	int max_inliers = 0;
	double max_error = 0.0;
	double Pbest[12];
	int num_inliers_new = 0;
	v3_t *pts_final = NULL;
	v2_t *projs_final = NULL;
	double Plinear[12];

	double Rinit[9];
	double triangular[9], orthogonal[9];
	int neg, sign;

	double thresh_sq = ransac_threshold * ransac_threshold;
	double error = 0.0;

	int num_inliers_polished = 0;
	num_inliers = 0;
	for (round = 0; round < ransac_rounds; round++) {
	    v3_t pts_inner[MIN_PTS];
	    v2_t projs_inner[MIN_PTS];
	    double Ptmp[12];

	    num_inliers = 0;
	    for (i = 0; i < MIN_PTS; i++) 
		{
			int redo = 0;
			int idx;
			int redo_count = 0;

			do {
				if (redo_count > 10000) 
				{
					//free(inliers);
					return -1;
				}

				idx = rand() % num_pts;

				redo = 0;
				for (j = 0; j < i; j++) 
				{
					if (idx == indices[j]) 
					{
						redo = 1;
						break;
					} //删除坐标相同的点
					else if (Vx(projs[idx]) == Vx(projs[indices[j]]) && 
						Vy(projs[idx]) == Vy(projs[indices[j]])) 
					{
							redo = 1;
							break;
					}
					else if(Vx(points[idx]) == Vx(points[indices[j]]) && 
						Vy(points[idx]) == Vy(points[indices[j]]))
					{
						redo = 1;
						break;
					}
				}

				redo_count++;
			} while(redo);

			indices[i] = idx;
			pts_inner[i] = points[idx];
			projs_inner[i] = projs[idx];
		}

	    /* Solve for the parameters */
	    find_projection_3x4(MIN_PTS, pts_inner, projs_inner, Ptmp);

#if 1
	    /* Fix the sign on the P matrix */
            memcpy(Rinit + 0, Ptmp + 0, 3 * sizeof(double));
            memcpy(Rinit + 3, Ptmp + 4, 3 * sizeof(double));
            memcpy(Rinit + 6, Ptmp + 8, 3 * sizeof(double));

            dgerqf_driver(3, 3, Rinit, triangular, orthogonal);	    

	    /* Check the parity along the diagonal */
	    neg = 
		(triangular[0] < 0.0) + 
		(triangular[4] < 0.0) + 
		(triangular[8] < 0.0);

	    if ((neg % 2) == 1) {
		sign = -1;
	    } else {
		sign = 1;
	    }
#endif
	    
	    /* Count the number of inliers */
	    error = 0.0;
	    for (i = 0; i < num_pts; i++) {
		double pt[4] = { Vx(points[i]), 
				 Vy(points[i]), 
				 Vz(points[i]), 1.0 };
		double pr[3];
		double dx, dy, dist;

		matrix_product341(Ptmp, pt, pr);

		/* Check cheirality */
                // EDIT!!!
		if (sign * pr[2] > 0.0) 
		    continue;

                // EDIT!!!
		pr[0] /= -pr[2];
		pr[1] /= -pr[2];
	    
		dx = pr[0] - Vx(projs[i]);
		dy = pr[1] - Vy(projs[i]);

		dist = dx * dx + dy * dy;

		if (dist < thresh_sq) {
		    inliers[num_inliers] = i;
		    num_inliers++;
		    error += dist;
		}
	    }
	    
	    if (num_inliers > max_inliers) {
		memcpy(Pbest, Ptmp, sizeof(double) * 12);
		max_error = error;
		max_inliers = num_inliers;
	    }
	}
	
	memcpy(P, Pbest, sizeof(double) * 12);

	printf("[find_projection_3x4_ransac] num_inliers = %d (out of %d)\n",
	       max_inliers, num_pts);
	printf("[find_projection_3x4_ransac] error = %0.3f\n", 
	       sqrt(max_error / max_inliers));

        if (max_inliers < 6) {
            printf("[find_projection_3x4_ransac] "
                   "Too few inliers to continue.\n");
            
            //free(inliers);

            return -1;
        }
	
	/* Do the final least squares minimization */

#if 1
	/* Fix the sign on the P matrix */
	memcpy(Rinit + 0, Pbest + 0, 3 * sizeof(double));
	memcpy(Rinit + 3, Pbest + 4, 3 * sizeof(double));
	memcpy(Rinit + 6, Pbest + 8, 3 * sizeof(double));

	dgerqf_driver(3, 3, Rinit, triangular, orthogonal);	    

	/* Check the parity along the diagonal */
	neg = 
	    (triangular[0] < 0.0) + 
	    (triangular[4] < 0.0) + 
	    (triangular[8] < 0.0);

	if ((neg % 2) == 1) {
	    sign = -1;
	} else {
	    sign = 1;
	}
#endif

	num_inliers = 0;
	pts_final = (v3_t *) malloc(sizeof(v3_t) * max_inliers);
	projs_final = (v2_t *) malloc(sizeof(v2_t) * max_inliers);
	
	for (i = 0; i < num_pts; i++) {
	    double pt[4] = { Vx(points[i]), 
			     Vy(points[i]), 
			     Vz(points[i]), 1.0 };

	    double pr[3];
	    double dx, dy, dist;
	    
	    matrix_product341(Pbest, pt, pr);

	    /* Check cheirality */
            // EDIT!!!
	    if (sign * pr[2] > 0.0) 
		continue;

            // EDIT!!!
	    pr[0] /= -pr[2];
	    pr[1] /= -pr[2];
	    
	    dx = pr[0] - Vx(projs[i]);
	    dy = pr[1] - Vy(projs[i]);

	    dist = dx * dx + dy * dy;

	    if (dist < thresh_sq) {
		pts_final[num_inliers] = points[i];
		projs_final[num_inliers] = projs[i];
		num_inliers++;
	    }
	}

	if (num_inliers != max_inliers) {
	    printf("[find_projection_3x4_ransac] Error! There was a miscount "
		   "somewhere: (%d != %d)\n", num_inliers, max_inliers);
	}

	find_projection_3x4(max_inliers, pts_final, projs_final, Plinear);

#if 1
	/* Fix the sign on the P matrix */
	memcpy(Rinit + 0, Plinear + 0, 3 * sizeof(double));
	memcpy(Rinit + 3, Plinear + 4, 3 * sizeof(double));
	memcpy(Rinit + 6, Plinear + 8, 3 * sizeof(double));

	dgerqf_driver(3, 3, Rinit, triangular, orthogonal);	    
	
	/* Check the parity along the diagonal */
	neg = 
	    (triangular[0] < 0.0) + 
	    (triangular[4] < 0.0) + 
	    (triangular[8] < 0.0);

	if ((neg % 2) == 1) {
	    sign = -1;
	} else {
	    sign = 1;
	}
#endif

	for (i = 0; i < num_pts; i++) {
	    double pt[4] = 
		{ Vx(points[i]), Vy(points[i]), Vz(points[i]), 1.0 };
	    double pr[3];
	    double dx, dy, dist;
	    
	    matrix_product341(Plinear, pt, pr);

            // EDIT!!!
	    if (sign * pr[2] > 0.0)
		continue;

            // EDIT!!!
	    pr[0] /= -pr[2];
	    pr[1] /= -pr[2];
	    
	    dx = pr[0] - Vx(projs[i]);
	    dy = pr[1] - Vy(projs[i]);

	    dist = dx * dx + dy * dy;

	    if (dist < thresh_sq) {
		num_inliers_new++;
	    }
	}

	if (num_inliers_new < max_inliers) {
	    printf("[find_projection_3x4_ransac] Reverting to old solution\n");
	    memcpy(Plinear, Pbest, 12 * sizeof(double));
	}
	
	printf("Best matrix (pre-opt):\n");
	matrix_print(3, 4, Plinear);

	error = 0.0;
	for (i = 0; i < max_inliers; i++) {
	    double pt[4] = 
		{ Vx(pts_final[i]), Vy(pts_final[i]), Vz(pts_final[i]), 1.0 };
	    double pr[3];
	    double dx, dy, dist;
	    
	    matrix_product341(Plinear, pt, pr);
	    pr[0] /= pr[2];
	    pr[1] /= pr[2];
	    
	    dx = pr[0] - Vx(projs_final[i]);
	    dy = pr[1] - Vy(projs_final[i]);

	    dist = dx * dx + dy * dy;

	    error += dist;
	}
	
	printf("Old error: %0.3e\n", sqrt(error / max_inliers));

	/* Polish the result */
	if (max_inliers >= 6) {
	    int num_inliers_polished = 0;
	    find_projection_3x4_nonlinear(max_inliers, pts_final, projs_final,
					  Plinear, P);

#if 1
            /* Fix the sign on the P matrix */
            memcpy(Rinit + 0, P + 0, 3 * sizeof(double));
            memcpy(Rinit + 3, P + 4, 3 * sizeof(double));
            memcpy(Rinit + 6, P + 8, 3 * sizeof(double));

            dgerqf_driver(3, 3, Rinit, triangular, orthogonal);	    
            
            /* Check the parity along the diagonal */
            neg = 
                (triangular[0] < 0.0) + 
                (triangular[4] < 0.0) + 
                (triangular[8] < 0.0);

            if ((neg % 2) == 1) {
                sign = -1;
            } else {
                sign = 1;
            }
#endif

            /* Check that the number of inliers hasn't gone down */
	    num_inliers_polished = 0;
	    for (i = 0; i < num_pts; i++) {
		double pt[4] = 
		    { Vx(points[i]), Vy(points[i]), Vz(points[i]), 1.0 };
		double pr[3];
		double dx, dy, dist;
	    
		matrix_product341(P, pt, pr);

                // EDIT!!!
		if (sign * pr[2] > 0.0)
		    continue;

                // EDIT!!!
		pr[0] /= -pr[2];
		pr[1] /= -pr[2];
	    
		dx = pr[0] - Vx(projs[i]);
		dy = pr[1] - Vy(projs[i]);

		dist = dx * dx + dy * dy;

		if (dist < thresh_sq) {
		    num_inliers_polished++;
		}
	    }

	    if (num_inliers_polished < max_inliers) {
		printf("Decreased number of inliers (%d < %d), reverting\n",
		       num_inliers_polished, max_inliers);

		memcpy(P, Plinear, sizeof(double) * 12);		
	    }
	} else {
	    memcpy(P, Plinear, sizeof(double) * 12);
	}

	printf("Best matrix (post-opt):\n");
	matrix_print(3, 4, P);

	error = 0.0;
	for (i = 0; i < max_inliers; i++) {
	    double pt[4] = 
		{ Vx(pts_final[i]), Vy(pts_final[i]), Vz(pts_final[i]), 1.0 };
	    double pr[3];
	    double dx, dy, dist;
	    
	    matrix_product341(P, pt, pr);

            // EDIT!!!
	    pr[0] /= -pr[2];
	    pr[1] /= -pr[2];
	    
	    dx = pr[0] - Vx(projs_final[i]);
	    dy = pr[1] - Vy(projs_final[i]);

	    dist = dx * dx + dy * dy;

	    error += dist;
	}
	
	printf("New error: %0.3e\n", sqrt(error / max_inliers));

//	free(inliers);
	free(pts_final);
	free(projs_final);

	return max_inliers;
    
#undef MIN_PTS
}

//m=P*Xc 物方到像方的投影矩阵
bool EstimateTransform_Projection(orsPOINT3D *p3DBuf, orsPOINT2Df *p2DBuf, int ptNum, int &refineNum, double P[12],
								  _iphCamera *camera, double thresh)
{


	int num_pts = ptNum;
	int *inliers = (int *) malloc(sizeof(int) * num_pts);
	int num_inliers=0;

	/* num_pts should be greater than a threshold */
	if (num_pts < 6) 
	{
		refineNum=0;
		free(inliers);
		return false;
	}

	v3_t *k1_pts = new v3_t[num_pts];
	v2_t *k2_pts = new v2_t[num_pts];

	v3_t *k1_pts_in = new v3_t[num_pts];
	v2_t *k2_pts_in = new v2_t[num_pts];

	for (int i = 0; i < num_pts; i++) 
	{
		// 		int idx1 = matches[i].m_idx1;
		// 		int idx2 = matches[i].m_idx2;
		double xp, yp;

		k1_pts[i] = v3_new(p3DBuf[i].X, p3DBuf[i].Y, p3DBuf[i].Z);

//		camera->Image2Photo(pBuf2[i].pt2D.x, pBuf2[i].pt2D.y, xp, yp);
		xp=p2DBuf[i].x;
		yp=p2DBuf[i].y;
		k2_pts[i] = v2_new(xp, yp);
	}

//	double P[12];
	bool essential=false;
	double threshold=thresh*camera->m_pixelX;	

	int r = -1;
	r=find_projection_3x4_ransac(num_pts, k1_pts, k2_pts, P,
		1024, threshold, inliers, num_inliers);

	if (r == -1) {
		printf("[FindAndVerifyCamera] Couldn't find projection matrix\n");
		refineNum=0;
		return false;
	}

	for (int i = 0, j=0; i < num_inliers; i++, j++) 
	{
		if(j!=inliers[i])
			p3DBuf[j] = p3DBuf[inliers[i]];
//		p3DBuf[inliers[i]].flag=keypoint_inlier;
	
	}
	refineNum=num_inliers;

	free(inliers);
	delete [] k1_pts;
	delete [] k2_pts;
	delete [] k1_pts_in;
	delete [] k2_pts_in;

	return true;
}