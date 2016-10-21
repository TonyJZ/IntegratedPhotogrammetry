#ifndef _LIDAR_ADJUSTMENT_DEFINE_INCLUDE_H__
#define _LIDAR_ADJUSTMENT_DEFINE_INCLUDE_H__

#include "orsLidarGeometry/LidGeo_def.h"
#include <vector>

//Lidar条带平差模型
enum orsLidSA_TYPE
{
	LidSA_3DSimilarityTrans = 0,		//3D相似变换
	LidSA_POSDrift,						//POS漂移	
	LidSA_OriAnchor,					//定向点模型
};


//条带平差中采用的连接点结构
struct LidAdj_keyPt
{
	int tieID;				//连接点ID，从0开始, 顺序递增，同名连接点的tieID相同
//	int stripID;			//条带ID, 从0开始编号
	double  vX, vY, vZ;		//观测值
	double  eX, eY, eZ;		//期望值 (真值)

	std::vector<LidPt_SurvInfo> *pLidPoints;	//实际激光点数组	
	char	VP_type;		//0: undefined;  1: horizontal and vertical control point;   2: horizontal control point;   3: vertical control point
	char	reliability;		//0: 不可靠；1: 可靠

	LidAdj_keyPt()
	{
		pLidPoints=NULL;
		reliability=1;
	}

	~LidAdj_keyPt()
	{
		if(pLidPoints)	delete pLidPoints;	pLidPoints=NULL;
	}
};

#endif