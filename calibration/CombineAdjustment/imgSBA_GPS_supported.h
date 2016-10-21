#ifndef _IMAGE_SBA_GPS_SUPPORTED_INCLUDE_
#define _IMAGE_SBA_GPS_SUPPORTED_INCLUDE_

//计算观测值
void _cal_Obs(double *pX, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *projs, void *adata);

//计算jac
void _cal_Jac(double *pX, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);


int sba_motstr_levmar_gps_supported(
    const int n, const int ncon, const int m, const int mcon, char *vmask, double *p, const int cnp, const int pnp, double *x, double *covx, const int mnp,
    void (*func)(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata),
    void (*fjac)(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata),
    void *adata, const int itmax, const int verbose, const double opts[], double info[]);


#endif