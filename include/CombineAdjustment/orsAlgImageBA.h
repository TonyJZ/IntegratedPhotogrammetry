#ifndef _ORS_ALGORITHM_IMAGE_BUNDLE_ADJUSTMENT_INCLUDE_
#define _ORS_ALGORITHM_IMAGE_BUNDLE_ADJUSTMENT_INCLUDE_

#include "orsIBundler.h"
#include "Calib_Camera.h"
#include "CombineAdjustment/BA_def.h"


class orsAlgImageBA : public orsIBundler
{
public:

	orsAlgImageBA();
	virtual ~orsAlgImageBA();

	virtual int bundle(void *adata);
};

#endif