#ifndef _ORS_ALGORITHM_CAMERA_CALIBRATION_INCLUDE_
#define _ORS_ALGORITHM_CAMERA_CALIBRATION_INCLUDE_

#include "orsIBundler.h"
#include "Calib_Camera.h"
#include "CombineAdjustment/BA_def.h"

class orsAlgCameraCalib : public orsIBundler
{

public:
	//光束法平差标定
	virtual int bundle( void *adata );

	//直接利用sparseLM求解
	virtual int bundle_SparseLM( void *adata );

	orsAlgCameraCalib();
	virtual ~orsAlgCameraCalib();

//前交-后交标定法
public:
	int calib_IP( void *adata );
	//基于LM的求解
	int calib_IP_LM( void *adata );
};


#endif