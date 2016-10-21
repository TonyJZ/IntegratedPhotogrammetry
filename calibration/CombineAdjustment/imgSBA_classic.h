#ifndef _IMAGE_SBA_CLASSIC_INCLUDE_
#define _IMAGE_SBA_CLASSIC_INCLUDE_

void _img_projs(double *pX, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, 
	double *projs, void *adata);

void _img_projsRTS_jac(double *pX, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, 
	double *jac, void *adata);

double output_image_pts_error(void *adata, int j, int mnp, double *x, double *hx, 
	struct sba_crsm *idxij, int *rcidxs, int *rcsubs);

int sba_motstr_levmar_classic(
    const int n,   /* number of points */
    const int ncon,/* number of points (starting from the 1st) whose parameters should not be modified.
                   * All B_ij (see below) with i<ncon are assumed to be zero
                   */
    const int m,   /* number of images */
    const int mcon,/* number of images (starting from the 1st) whose parameters should not be modified.
					          * All A_ij (see below) with j<mcon are assumed to be zero
					          */
    char *vmask,  /* visibility mask: vmask[i, j]=1 if point i visible in image j, 0 otherwise. nxm */
    double *p,    /* initial parameter vector p0: (a1, ..., am, b1, ..., bn).
                   * aj are the image j parameters, bi are the i-th point parameters,
                   * size m*cnp + n*pnp
                   */
    const int cnp,/* number of parameters for ONE camera; e.g. 6 for Euclidean cameras */
    const int pnp,/* number of parameters for ONE point; e.g. 3 for Euclidean points */
    double *x,    /* measurements vector: (x_11^T, .. x_1m^T, ..., x_n1^T, .. x_nm^T)^T where
                   * x_ij is the projection of the i-th point on the j-th image.
                   * NOTE: some of the x_ij might be missing, if point i is not visible in image j;
                   * see vmask[i, j], max. size n*m*mnp
                   */
    double *covx, /* measurements covariance matrices: (Sigma_x_11, .. Sigma_x_1m, ..., Sigma_x_n1, .. Sigma_x_nm),
                   * where Sigma_x_ij is the mnp x mnp covariance of x_ij stored row-by-row. Set to NULL if no
                   * covariance estimates are available (identity matrices are implicitly used in this case).
                   * NOTE: a certain Sigma_x_ij is missing if the corresponding x_ij is also missing;
                   * see vmask[i, j], max. size n*m*mnp*mnp
                   */
    const int mnp,/* number of parameters for EACH measurement; usually 2 */
    void (*func)(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata),
                                              /* functional relation describing measurements. Given a parameter vector p,
                                               * computes a prediction of the measurements \hat{x}. p is (m*cnp + n*pnp)x1,
                                               * \hat{x} is (n*m*mnp)x1, maximum
                                               * rcidxs, rcsubs are max(m, n) x 1, allocated by the caller and can be used
                                               * as working memory
                                               */
    void (*fjac)(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata),
                                              /* function to evaluate the sparse jacobian dX/dp.
                                               * The Jacobian is returned in jac as
                                               * (dx_11/da_1, ..., dx_1m/da_m, ..., dx_n1/da_1, ..., dx_nm/da_m,
                                               *  dx_11/db_1, ..., dx_1m/db_1, ..., dx_n1/db_n, ..., dx_nm/db_n), or (using HZ's notation),
                                               * jac=(A_11, B_11, ..., A_1m, B_1m, ..., A_n1, B_n1, ..., A_nm, B_nm)
                                               * Notice that depending on idxij, some of the A_ij and B_ij might be missing.
                                               * Note also that A_ij and B_ij are mnp x cnp and mnp x pnp matrices resp. and they
                                               * should be stored in jac in row-major order.
                                               * rcidxs, rcsubs are max(m, n) x 1, allocated by the caller and can be used
                                               * as working memory
                                               *
                                               * If NULL, the jacobian is approximated by repetitive func calls and finite
                                               * differences. This is computationally inefficient and thus NOT recommended.
                                               */
    void *adata,       /* pointer to possibly additional data, passed uninterpreted to func, fjac */ 

    const int itmax,   /* I: maximum number of iterations. itmax==0 signals jacobian verification followed by immediate return */
    const int verbose, /* I: verbosity */
    const double opts[],
	                     /* I: minim. options [\mu, \epsilon1, \epsilon2, \epsilon3, \epsilon4]. Respectively the scale factor for initial \mu,
                        * stopping thresholds for ||J^T e||_inf, ||dp||_2, ||e||_2 and (||e||_2-||e_new||_2)/||e||_2
                        */
    double info[]
	                     /* O: information regarding the minimization. Set to NULL if don't care
                        * info[0]=||e||_2 at initial p.
                        * info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
                        * info[5]= # iterations,
                        * info[6]=reason for terminating: 1 - stopped by small gradient J^T e
                        *                                 2 - stopped by small dp
                        *                                 3 - stopped by itmax
                        *                                 4 - stopped by small relative reduction in ||e||_2
                        *                                 5 - stopped by small ||e||_2
                        *                                 6 - too many attempts to increase damping. Restart with increased mu
                        *                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
                        * info[7]= # function evaluations
                        * info[8]= # jacobian evaluations
			                  * info[9]= # number of linear systems solved, i.e. number of attempts	for reducing error
                        */
);


#endif