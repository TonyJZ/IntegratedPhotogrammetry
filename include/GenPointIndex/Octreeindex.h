#ifndef _Octav_Tree_Index_H__ZJ_2016_3_21_
#define _Octav_Tree_Index_H__ZJ_2016_3_21_

#include <vector>
#include "orsBase\orsTypedef.h"
#include "orsBase\orsString.h"
//#include "ATN_def.h"
#include "orsPointCloud\orsIPointCloudService.h"
#include "orsPointCloud\orsIPointCloudReader.h"
#include "CalibBasedef.h"




class COctreePtIndex
{
public:
	COctreePtIndex(orsIPlatform *platform);
	~COctreePtIndex();



protected:


	ref_ptr<orsIPointCloudService>  m_pointCloudService;
	orsIPointCloudReader*  m_reader;


private:
//	CTINClass *m_pTin;

};

#endif