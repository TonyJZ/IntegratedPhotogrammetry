#ifndef _LIDAR_STRIP_ADJUSTMENT_INCLUDE_H__
#define _LIDAR_STRIP_ADJUSTMENT_INCLUDE_H__
//联合平差中的lidar平差部分


#include "AlignPrj.h"

#include "orsBase/orsIPlatform.h"
#include "orsLidarGeometry\orsITrajectory.h"
#include "LidStripAdj\LidAdj_def.h"
#include "CalibBasedef.h"
#include "Align_def.h"

using namespace std;

#ifdef LIDADJLIB_EXPORTS
#define  _LIDADJ_DLL_  __declspec(dllexport)
#else
#define  _LIDADJ_DLL_  __declspec(dllimport)	
#endif

class  _LIDADJ_DLL_ CLidAdjustment
{
public:
	CLidAdjustment(orsIPlatform *platform);
	~CLidAdjustment();

	//设置测区
	//2014.12.4 测区信息与连接点要分开存储  todo
	void setAlignPrj(CAlignPrj *pAlg);

	//
	bool loadTiePoints(const char* pszFileName, int minOverlap=2);

	//条带平差
	int Lidar_StripAdjustment(char *pszSAResultFile, orsLidSA_TYPE SA_type);


private:



private:
	CAlignPrj *m_pAlg;
	ref_ptr<orsITrajectory>	  m_traj;	//导航轨迹

	CArray<TieObjChain*, TieObjChain*>   m_TieList;
	int		m_nprojs;   //各条带上的同名观测值总数
public:
	double	*m_covar;	//未知数的协方差阵
	int		m_nUnknown;		//未知数个数
	double  m_rms;		//单位权中误差（观测值）
	double  *m_corcoef;	//未知数之间的相关系数
};


#endif