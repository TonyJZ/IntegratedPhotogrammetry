#ifndef _INTEGRATED_SBA_INCLUDE_H__
#define _INTEGRATED_SBA_INCLUDE_H__

//#include "orsImageGeometry\orsIAlgBundleAdjustment.h"
#include "orsIBundler.h"


class orsAlgIntegratedAT : public orsIBundler/* : public orsIAlgBundleAdjustment, orsObjectBase*/
{
public:
	virtual int bundle( void *adata );

	orsAlgIntegratedAT();
	virtual ~orsAlgIntegratedAT();


// public:	
// 	ORS_OBJECT_IMP2( orsAlgIntegratedAT, orsIAlgBundleAdjustment, orsIAlgorithm, 
// 		_T("IntSBA"), _T("Integrated SBA" ) )
};



#endif