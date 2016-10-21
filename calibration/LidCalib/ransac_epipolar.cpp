#include "StdAfx.h"
#include "ransac_epipolar.h"
#include "math.h"
#include "float.h"
#include "bundler\matrix.h"
#include "bundler\svd.h"
#include "bundler\qsort.h"

#ifndef DBL_MAX
#define DBL_MAX         1.7976931348623158e+308 /* max value */
#endif




int svd3_driver(double *A, double *U, double *S, double *VT) 
{
	double V[9], Utmp[9], VTtmp[9], UT[9];
	int retval = svd(3, 3, 1, 1, 1.0e-4, 1.0e-4, A, S, Utmp, V, VTtmp);
	int perm[3];

	if (retval != 0)
		return retval;

	qsort_descending();
	qsort_perm(3, S, perm);

	matrix_transpose(3, 3, Utmp, UT);
	memcpy(Utmp + 0, UT + perm[0] * 3, 3 * sizeof(double));
	memcpy(Utmp + 3, UT + perm[1] * 3, 3 * sizeof(double));
	memcpy(Utmp + 6, UT + perm[2] * 3, 3 * sizeof(double));
	matrix_transpose(3, 3, Utmp, U);

	memcpy(VT + 0, VTtmp + perm[0] * 3, 3 * sizeof(double));
	memcpy(VT + 3, VTtmp + perm[1] * 3, 3 * sizeof(double));
	memcpy(VT + 6, VTtmp + perm[2] * 3, 3 * sizeof(double));

	return retval;
}

/* Find the closest rank 2 matrix to the given 3x3 matrix */
int closest_rank2_matrix(double *Fin, double *Fout, double *U, double *VT) {
    double S[3], sigma[9], F_rank2[9], tmp[9];

    int success = dgesvd_driver(3, 3, Fin, U, S, VT);
    // int retval = svd3_driver(Fin, U, S, VT);

    sigma[0] = S[0];  sigma[1] =  0.0;  sigma[2] =  0.0;
    sigma[3] =  0.0;  sigma[4] = S[1];  sigma[5] =  0.0;
    sigma[6] =  0.0;  sigma[7] =  0.0;  sigma[8] =  0.0;

    matrix_product(3, 3, 3, 3, U, sigma, tmp);
    matrix_product(3, 3, 3, 3, tmp, VT, F_rank2);

    memcpy(Fout, F_rank2, sizeof(double) * 9);

    return success;
    // return (retval == 0);
}

/* Find the closest rank 2 matrix (with the same singular values) *
 * to the given 3x3 matrix */
int closest_rank2_matrix_ssv(double *Fin, double *Fout, 
                             double *U, double *VT) {
    double S[3], sigma[9], F_rank2[9], tmp[9];

    // int success = dgesvd_driver(3, 3, Fin, U, S, VT);
    int retval = svd3_driver(Fin, U, S, VT);

    sigma[0] =  1.0;  sigma[1] =  0.0;  sigma[2] =  0.0;
    sigma[3] =  0.0;  sigma[4] =  1.0;  sigma[5] =  0.0;
    sigma[6] =  0.0;  sigma[7] =  0.0;  sigma[8] =  0.0;

    matrix_product(3, 3, 3, 3, U, sigma, tmp);
    matrix_product(3, 3, 3, 3, tmp, VT, F_rank2);

    memcpy(Fout, F_rank2, sizeof(double) * 9);

    // return success;
    return (retval == 0);
}

/* Use linear least-squares to estimate the fundamantal matrix */
int estimate_fmatrix_linear(int num_pts, v3_t *r_pts, v3_t *l_pts, 
                            int essential,
                            double *Fout, double *e1, double *e2) 
{
    int i;
    v3_t r_c, l_c;
    double r_dist, l_dist, r_scale, l_scale;

    v3_t *r_pts_new, *l_pts_new;

    double *A, *b, X[8], F[9], H[9], H_p[9], tmp[9], F_new[9];
    double U[9], VT[9];

    v3_t r_pts_8pt[8], l_pts_8pt[8];
    double A_8pt[64], b_8pt[8];

    int success;

    /* Check that there are enough point correspondences */
    if (num_pts < 8) {
	printf("[estimate_fmatrix_linear] Insufficient correspondences "
               "(need at least 8, given only %d)\n", num_pts);
	return 0;
    }


    /* First, compute the centroid of both sets of points */
    
    r_c = v3_new(0.0, 0.0, 0.0);
    l_c = v3_new(0.0, 0.0, 0.0);

    for (i = 0; i < num_pts; i++) {
	r_c = v3_add(r_c, r_pts[i]);
	l_c = v3_add(l_c, l_pts[i]);
    }

    r_c = v3_scale(1.0 / num_pts, r_c);
    l_c = v3_scale(1.0 / num_pts, l_c);


    /* Compute the average distance from each point to the centroid */
    r_dist = l_dist = 0;
    
    for (i = 0; i < num_pts; i++) {
	r_dist += v3_mag(v3_sub(r_c, r_pts[i]));
	l_dist += v3_mag(v3_sub(l_c, l_pts[i]));
    }

	r_dist /= num_pts;
    l_dist /= num_pts;

    r_dist /= sqrt(2.0);
    l_dist /= sqrt(2.0);

    r_scale = 1.0 / r_dist;
    l_scale = 1.0 / l_dist;


    /* Normalize the points with an affine transform */
	//平移到以形心为原点的坐标系下，并且使点到形心的距离的均方差为sqrt(2)
    if (num_pts > 8) {
        r_pts_new = (v3_t *)malloc(sizeof(v3_t) * num_pts);
        l_pts_new = (v3_t *)malloc(sizeof(v3_t) * num_pts);
    } else {
        r_pts_new = r_pts_8pt;
        l_pts_new = l_pts_8pt;
    }
    

    for (i = 0; i < num_pts; i++) {
	r_pts_new[i] = v3_scale(r_scale, v3_sub(r_pts[i], r_c));
	l_pts_new[i] = v3_scale(l_scale, v3_sub(l_pts[i], l_c));

	Vz(r_pts_new[i]) = 1.0;
	Vz(l_pts_new[i]) = 1.0;
     }


    /* Fill in the rows of the matrix A */
    if (num_pts > 8)
        A = (double *)malloc(sizeof(double) * 8 * num_pts);
    else
        A = A_8pt;

    for (i = 0; i < num_pts; i++) {
	double u = Vx(l_pts_new[i]);
	double v = Vy(l_pts_new[i]);
	double u_p = Vx(r_pts_new[i]);
	double v_p = Vy(r_pts_new[i]);

	A[i * 8 + 0] = u * u_p;
	A[i * 8 + 1] = v * u_p;
	A[i * 8 + 2] = u_p;
	A[i * 8 + 3] = u * v_p;
	A[i * 8 + 4] = v * v_p;
	A[i * 8 + 5] = v_p;
	A[i * 8 + 6] = u;
	A[i * 8 + 7] = v;
    }


    /* Fill in the vector b */
    if (num_pts > 8)
        b = (double *)malloc(sizeof(double) * num_pts);
    else
        b = b_8pt;

    for (i = 0; i < num_pts; i++) {
	b[i] = -1.0;
    }


    /* Solve for the least-squares solution to the F-matrix */
    if (num_pts > 8)
        dgelsy_driver(A, b, X, num_pts, 8, 1);
    else
        dgesv_driver(num_pts, A, b, X);

    /* Un-normalize */
    H[0] = l_scale;  H[1] =     0.0;  H[2] = -l_scale * Vx(l_c);
    H[3] =     0.0;  H[4] = l_scale;  H[5] = -l_scale * Vy(l_c);
    H[6] =     0.0;  H[7] =     0.0;  H[8] =                1.0;

    H_p[0] = r_scale;  H_p[3] =     0.0;  H_p[6] = -r_scale * Vx(r_c);
    H_p[1] =     0.0;  H_p[4] = r_scale;  H_p[7] = -r_scale * Vy(r_c);
    H_p[2] =     0.0;  H_p[5] =     0.0;  H_p[8] =                1.0;

    memcpy(F, X, sizeof(double) * 8);
    F[8] = 1.0;

    matrix_product(3, 3, 3, 3, H_p, F, tmp);
    matrix_product(3, 3, 3, 3, tmp, H, F_new);

    /* Use SVD to compute the nearest rank 2 matrix */
    if (essential == 0)
        success = closest_rank2_matrix(F_new, Fout, U, VT);
    else
        success = closest_rank2_matrix_ssv(F_new, Fout, U, VT);

    /* The last column of U spans the nullspace of F, so it is the
     * epipole in image A.  The last column of V spans the nullspace
     * of F^T, so is the epipole in image B */

    e1[0] = U[2];
    e1[1] = U[5];
    e1[2] = U[8];
    
    e2[0] = VT[6];
    e2[1] = VT[7];
    e2[2] = VT[8];


    /* Cleanup */
    if (num_pts > 8) {
        free(A);
        free(b);
        free(r_pts_new);
        free(l_pts_new);
    }

    return success;
}

double fmatrix_compute_residual(double *F, v3_t r, v3_t l) {
	double Fl[3], Fr[3], pt;    

#if 1
	Fl[0] = F[0] * Vx(l) + F[1] * Vy(l) + F[2] * Vz(l);
	Fl[1] = F[3] * Vx(l) + F[4] * Vy(l) + F[5] * Vz(l);
	Fl[2] = F[6] * Vx(l) + F[7] * Vy(l) + F[8] * Vz(l);

	Fr[0] = F[0] * Vx(r) + F[3] * Vy(r) + F[6] * Vz(r);
	Fr[1] = F[1] * Vx(r) + F[4] * Vy(r) + F[7] * Vz(r);
	Fr[2] = F[2] * Vx(r) + F[5] * Vy(r) + F[8] * Vz(r);

	pt = Vx(r) * Fl[0] + Vy(r) * Fl[1] + Vz(r) * Fl[2];
#else
	matrix_product(3, 3, 3, 1, F, l.p, Fl);
	matrix_transpose_product(3, 3, 3, 1, F, r.p, Fr);
	matrix_product(1, 3, 3, 1, r.p, Fl, &pt);
#endif

	return
		(1.0 / (Fl[0] * Fl[0] + Fl[1] * Fl[1]) +
		1.0 / (Fr[0] * Fr[0] + Fr[1] * Fr[1])) *
		(pt * pt);
}

int estimate_fmatrix_ransac_matches(int num_pts, v3_t *a_pts, v3_t *b_pts, 
									int num_trials, double threshold, 
									double success_ratio,
									int essential, double *F) 
{
	int i, j, k, idx;

	v3_t l_pts_best[8], r_pts_best[8];

	double Fbest[9];
	double *resid;
	double error_min;
	int inliers_max;

	double *a_matrix, *b_matrix;

	// double threshold = 1.0e-10;

	// srand(time(0));

	/* Make an array of all good correspondences */
	if (num_pts < 8) {
		printf("[estimate_fmatrix_ransac] Could not find 8 good correspondences,"
			"F-matrix estimation failed\n");
		return 0;
	}

	a_matrix = (double*)malloc(sizeof(double) * 3 * num_pts);
	b_matrix = (double*)malloc(sizeof(double) * 3 * num_pts);

	for (i = 0; i < num_pts; i++) 
	{
		a_matrix[i] = Vx(a_pts[i]);
		a_matrix[i+num_pts] = Vy(a_pts[i]);
		a_matrix[i+2*num_pts] = Vz(a_pts[i]);

		b_matrix[i] = Vx(b_pts[i]);
		b_matrix[i+num_pts] = Vy(b_pts[i]);
		b_matrix[i+2*num_pts] = Vz(b_pts[i]);        
	}

	error_min = DBL_MAX;
	inliers_max = 0;
	resid = (double *) malloc(sizeof(double) * num_pts);

	/* Estimate the F-matrix using RANSAC */
	for (i = 0; i < num_trials; i++) 
	{
		int idxs[8];
		v3_t l_pts[8], r_pts[8];
		double Ftmp[9], e1_tmp[3], e2_tmp[3];
		// double error;
		int num_inliers = 0;
		int success, nan = 0;
		int round = 0;

		/* Sample 8 random correspondences */
		for (j = 0; j < 8; j++) 
		{
			int reselect = 0;

			if (round == 1000)	//随机数反复重复时退出
				return 0;

			idx = rand() % num_pts;

			/* Make sure we didn't sample this index yet */
			for (k = 0; k < j; k++) 
			{
				if (idx == idxs[k] ||
					(Vx(a_pts[idx]) == Vx(a_pts[idxs[k]]) &&
					Vy(a_pts[idx]) == Vy(a_pts[idxs[k]]) &&
					Vz(a_pts[idx]) == Vz(a_pts[idxs[k]])) ||
					(Vx(b_pts[idx]) == Vx(b_pts[idxs[k]]) &&
					Vy(b_pts[idx]) == Vy(b_pts[idxs[k]]) &&
					Vz(b_pts[idx]) == Vz(b_pts[idxs[k]]))) 
				{
					reselect = 1;
					break;
				}
			}

			if (reselect) 
			{
				round++;
				j--;
				continue;
			}

			idxs[j] = idx;
		}

		/* Fill in the left and right points */
		for (j = 0; j < 8; j++) 
		{
			l_pts[j] = b_pts[idxs[j]];
			r_pts[j] = a_pts[idxs[j]];
		}

		/* Estimate the F-matrix */
		success = estimate_fmatrix_linear(8, r_pts, l_pts, essential, 
			Ftmp, e1_tmp, e2_tmp);

		if (success == 0)
			nan = 1;

		for (j = 0; j < 9; j++) 
		{
			if (Ftmp[j] != Ftmp[j] /* isnan(Ftmp[j]) */) 
			{
				printf("[estimate_fmatrix_ransac_matches] nan encountered\n");
				nan = 1;
				break;
			}
		}

		/* Check for nan entries */
		if (_isnan(Ftmp[0]) || _isnan(Ftmp[1]) || _isnan(Ftmp[2]) ||
			_isnan(Ftmp[3]) || _isnan(Ftmp[4]) || _isnan(Ftmp[5]) ||
			_isnan(Ftmp[6]) || _isnan(Ftmp[7]) || _isnan(Ftmp[8])) 
		{
			printf("[estimate_fmatrix_ransac_matches] "
				"nan matrix encountered\n");
			nan = 1;
		}

		if (nan) 
		{
			// error = DBL_MAX;
			num_inliers = 0;
		} 
		else 
		{
			// printf("%0.3f\n", Ftmp[0]);

			/* Compute residuals */
#if 1
			for (j = 0; j < num_pts; j++) 
			{
				resid[j] = fmatrix_compute_residual(Ftmp, a_pts[j], b_pts[j]);
				if (resid[j] < threshold)
					num_inliers++;
			}
#else
			fmatrix_compute_residuals(num_pts, Ftmp, a_matrix, b_matrix,
				resid);

			for (j = 0; j < num_pts; j++) 
			{
				if (resid[j] < threshold)
					num_inliers++;                
			}
#endif

#if 0
			/* Find the median */
			error = median(num_pts, resid);

			if (error < error_min) {
				error_min = error;
				memcpy(Fbest, Ftmp, sizeof(double) * 9);
				memcpy(l_pts_best, l_pts, sizeof(v3_t) * 8);
				memcpy(r_pts_best, r_pts, sizeof(v3_t) * 8);
			}
#else
			if (num_inliers > inliers_max) 
			{
				inliers_max = num_inliers;		//记录每次迭代最大inliers
				memcpy(Fbest, Ftmp, sizeof(double) * 9);	//最佳基本矩阵
				memcpy(l_pts_best, l_pts, sizeof(v3_t) * 8);	//计算F所用的两片上的对应点
				memcpy(r_pts_best, r_pts, sizeof(v3_t) * 8);
			}
#endif
		}

#if 0
		if (error < threshold)
			break;
#endif

		if ((double) num_inliers / num_pts > success_ratio)
			break;
	}

	// printf("Minimum error: %0.5e\n", error_min);
	// printf("Maximum inliers: %d\n", inliers_max);

	// matrix_print(3, 3, Fbest);

	free(resid);

	/* Copy out the F-matrix */
	memcpy(F, Fbest, sizeof(double) * 9);

	free(a_matrix);
	free(b_matrix);

	return inliers_max;
}

static v3_t *global_ins = NULL;
static v3_t *global_outs = NULL;
static int global_num_matches = 0;
static double global_scale;

void fmatrix_residuals(int *m, int *n, double *x, double *fvec, int *iflag) {
	int i;
	double sum = 0.0;

	double F[9], F2[9], U[9], VT[9];
	memcpy(F, x, sizeof(double) * 8);
	F[8] = global_scale;

	closest_rank2_matrix(F, F2, U, VT);

	if (global_num_matches != (*m)) {
		printf("Error: number of matches don't match!\n");
	}

	for (i = 0; i < global_num_matches; i++) {
		fvec[i] = sqrt(fmatrix_compute_residual(F2, global_outs[i], global_ins[i]));
		if (*iflag == 0) {
			sum += fvec[i];
		}
	}

#if 0
	if (*iflag == 0) {
		matrix_print(3, 3, F);
		matrix_print(3, 3, F2);
		printf("Residuals: %0.5f\n", sum);
	}
#endif
}
/* Refine an F-matrix estimate using LM */
void refine_fmatrix_nonlinear_matches(int num_pts, v3_t *r_pts, v3_t *l_pts, 
									  double *F0, double *Fout)
{
	double Ftmp[9];
	double U[9], VT[9];

	global_ins = l_pts;
	global_outs = r_pts;
	global_num_matches = num_pts;
	global_scale = F0[8];

	memcpy(Ftmp, F0, sizeof(double) * 9);

	lmdif_driver2(fmatrix_residuals, num_pts, 8, Ftmp, 1.0e-12);

	Ftmp[8] = global_scale;
	matrix_print(3, 3, Ftmp);
	closest_rank2_matrix(Ftmp, Fout, U, VT);
	matrix_print(3, 3, Fout);

	global_ins = global_outs = NULL;
	global_num_matches = 0;    
}


bool EstimateTransform_Epipolar(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, int &refineNum,
											   _iphCamera *camera, double thresh)
{
	

	int num_pts = ptNum;

	/* num_pts should be greater than a threshold */
	if (num_pts < 20) 
	{
		refineNum=0;
		return false;
	}

	v3_t *k1_pts = new v3_t[num_pts];
	v3_t *k2_pts = new v3_t[num_pts];

	v3_t *k1_pts_in = new v3_t[num_pts];
	v3_t *k2_pts_in = new v3_t[num_pts];

	for (int i = 0; i < num_pts; i++) 
	{
// 		int idx1 = matches[i].m_idx1;
// 		int idx2 = matches[i].m_idx2;
		double xp, yp;

		camera->Image2Photo(pBuf1[i].pt2D.x, pBuf1[i].pt2D.y, xp, yp);
		k1_pts[i] = v3_new(xp, yp, 1.0);

		camera->Image2Photo(pBuf2[i].pt2D.x, pBuf2[i].pt2D.y, xp, yp);
		k2_pts[i] = v3_new(xp, yp, 1.0);
	}

	double F[9];
	bool essential=false;
	double threshold=thresh*camera->m_pixelX;	

	estimate_fmatrix_ransac_matches(num_pts, k2_pts, k1_pts, 
		1024, threshold, 0.95, (essential ? 1 : 0), F);

	/* Find the inliers */
	std::vector<int> inliers;

	for (int i = 0; i < num_pts; i++) {
		double dist = fmatrix_compute_residual(F, k2_pts[i], k1_pts[i]);
		if (dist < threshold) {
			inliers.push_back(i);
		}
	}

	/* Re-estimate using inliers */
	int num_inliers = (int) inliers.size();

	for (int i = 0; i < num_inliers; i++) 
	{
		k1_pts_in[i] = k1_pts[inliers[i]]; // v3_new(k1[idx1]->m_x, k1[idx1]->m_y, 1.0);
		k2_pts_in[i] = k2_pts[inliers[i]]; // v3_new(k2[idx2]->m_x, k2[idx2]->m_y, 1.0);
	}

	// printf("[1] num_inliers = %d\n", num_inliers);

#if 0
	double F0[9];
	double e1[3], e2[3];
	estimate_fmatrix_linear(num_inliers, k2_pts_in, k1_pts_in, F0, e1, e2);

	inliers.clear();
	for (int i = 0; i < num_pts; i++) {
		double dist = fmatrix_compute_residual(F0, k2_pts[i], k1_pts[i]);
		if (dist < threshold) {
			inliers.push_back(i);
		}
	}
	num_inliers = inliers.size();
	// printf("[2] num_inliers = %d\n", num_inliers);

	// matrix_print(3, 3, F0);
#else
	double F0[9];
	memcpy(F0, F, sizeof(double) * 9);
#endif

	if (!essential) {
		/* Refine using NLLS */
		for (int i = 0; i < num_inliers; i++) {
			k1_pts_in[i] = k1_pts[inliers[i]];
			k2_pts_in[i] = k2_pts[inliers[i]];
		}

		refine_fmatrix_nonlinear_matches(num_inliers, k2_pts_in, k1_pts_in, 
			F0, F);
	} else 
	{
		memcpy(F, F0, sizeof(double) * 9);
	}

#if 0
	if (essential) {
		/* Compute the SVD of F */
		double U[9], S[3], VT[9];
		dgesvd_driver(3, 3, F, U, S, VT);
		double E0[9] = { 1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 0.0 };

		double tmp[9];
		matrix_product(3, 3, 3, 3, U, E0, tmp);
		matrix_product(3, 3, 3, 3, tmp, VT, F);
	}
#endif

	inliers.clear();
	for (int i = 0; i < num_pts; i++) 
	{
		double dist = fmatrix_compute_residual(F, k2_pts[i], k1_pts[i]);
		if (dist < threshold) 
		{
			inliers.push_back(i);
			pBuf1[i].flag=keypoint_inlier;
		}
	}
	refineNum = (int) inliers.size();

	delete [] k1_pts;
	delete [] k2_pts;
	delete [] k1_pts_in;
	delete [] k2_pts_in;

	return true;
}


bool estimate_fmatrix_nonlinear(Align_Keypoint *pBuf1, Align_Keypoint *pBuf2, int ptNum, double *F0, double *Fout)
{
	

	return false;
}