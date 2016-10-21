#include "StdAfx.h"
#include "CombineAdjustment/orsAlgImageBA.h"
#include "imgSBA_classic.h"
#include "imgSBA_classic_weight.h"
#include "imgSBA_GPS_supported.h"
#include "\OpenRS\external\source\sba-1.6\sba.h"
#include "CombineAdjustment\BA_def.h"
#include "CombineAdjustment\adjustFunc.h"


#define MAXITER         256
#define MAXITER2        512


orsAlgImageBA::orsAlgImageBA()
{

}

orsAlgImageBA::~orsAlgImageBA()
{

}

int orsAlgImageBA::bundle(void *adata)
{
	int retval;
	double opts[SBA_OPTSSZ], info[SBA_INFOSZ];
	int howto, expert, analyticjac, fixedcal, havedist, n, prnt, verbose=1;
	int nframes, numpts3D, numprojs, nvars;
	const int nconstframes=0;
	register int i;
	FILE *fp;

	clock_t start_time, end_time;

	imgBA_globs *globs;
	globs = (imgBA_globs*)adata;


	/* call sparse LM routine */
	opts[0]=SBA_INIT_MU;		//初始mu 
	opts[1]=SBA_STOP_THRESH;	//||J^T e||_inf    JX = e
	opts[2]=SBA_STOP_THRESH;    //||dp||_2	未知数增量
//	opts[3]=SBA_STOP_THRESH;	//||e||_2 重投影误差
	opts[3]=0.01*globs->camInfo->m_pixelX; 
	opts[4]=0.0;				//(||e||_2-||e_new||_2)/||e||_2
	//opts[4]=1E-05; // uncomment to force termination if the relative reduction in the RMS reprojection error drops below 1E-05

	start_time=clock();

	nframes = globs->nimg;
	numpts3D = globs->ntie;
	numprojs = globs->nproj;
	//未知数个数
//	nvars=nframes*globs->cnp+numpts3D*globs->pnp;

//	double *covimgpts=(double *)malloc(globs->nproj*globs->mnp*globs->mnp*sizeof(double));

	switch(globs->BA_type)
	{
	case ImageBA_classic:
		
		retval=sba_motstr_levmar_classic(numpts3D, globs->gcp.size(), nframes, nconstframes, globs->mask, globs->pUnknowns, globs->cnp, globs->pnp,
			globs->pObs, NULL, globs->mnp,
			_img_projs,
			_img_projsRTS_jac,
			adata, MAXITER2, verbose, opts, info);
		break;

	case ImageBA_GPS_supported:

		retval=sba_motstr_levmar_gps_supported(numpts3D, globs->gcp.size(), nframes, nconstframes, globs->mask, globs->pUnknowns, globs->cnp, globs->pnp,
			globs->pObs, NULL, globs->mnp,
			_cal_Obs,
			_cal_Jac,
			adata, MAXITER2, verbose, opts, info);
		break;

	case ImageBA_POS_supported:
		
		break;

	case ImageBA_classic_W:
		retval=sba_motstr_levmar_classic_W(numpts3D, globs->gcp.size(), nframes, nconstframes, globs->mask, globs->pUnknowns, globs->cnp, globs->pnp,
			globs->pObs, NULL, globs->mnp,
			_img_projs,
			_img_projsRTS_jac,
			adata, MAXITER2, verbose, opts, info);
		break;

	default:
		break;
	}
	
	end_time=clock();

	fprintf(stdout, "Elapsed time: %.2lf seconds\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC);

cleanup:
//	if(covimgpts)	free(covimgpts);
	return retval;
}