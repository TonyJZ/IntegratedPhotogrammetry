#ifndef _Image_Boresight_Calibration_zj_2011_11_17_
#define _Image_Boresight_Calibration_zj_2011_11_17_

#include "orsBase/orsTypedef.h"
#include <vector>


#include "Calib_Camera.h"


#ifdef _LIDCALIB_
#define  _lidCalib_Dll_  __declspec(dllexport)
#else
#define  _lidCalib_Dll_  __declspec(dllimport)	
#endif

// #ifndef _LIDCALIB_
// #ifdef _DEBUG
// #pragma comment(lib,"LidCalib.lib")
// #else
// #pragma comment(lib,"LidCalib.lib")
// #endif
// #endif


struct correspond_point3D
{
	int ID;	
	double X_cor, Y_cor, Z_cor;		//匹配点
	double X_ref, Y_ref, Z_ref;		//参考点，lidar是
};

struct imgExOrientParam
{
	double gps_time;
	double Xs, Ys, Zs;
	double phi, omega, kappa;
};

struct imgPoint
{
	ors_int32 imgID;
	double xi, yi;
};

struct AT_tiePoint
{
	ors_int32 ptID;
	double X, Y, Z;
	std::vector<imgPoint> projs;
};

struct Misalign_Param
{
	double u, v, w;					//偏心分量
	//double phi, pitch, heading;	//偏心角
	double phi, omega, kappa;
	//double f;						//主距
};

//投影点
struct Proj_Point
{
	long iXyz;			// 
	//double X, Y, Z;	//物方
	double xi, yi;		//像方
	float  vx, vy;
};


//时间延迟，Z约束
int _lidCalib_Dll_ Img_misalign_calib_delay_ZConstraint(const char* pImgList, const char* tracks, const char *posSequence, const char *pLasIdxName);

int _lidCalib_Dll_ Img_misalign_calib_ZConstraint(const char* pImgList, const char* tracks, const char *pLasIdxName);

void _lidCalib_Dll_ ExportReprojectError(const char *pDir, std::vector<imgExOrientParam> *exorParam, 
										 _iphCamera *camera, std::vector<AT_tiePoint> &pData, Proj_Point *projs);

//格式转换
//int _lidCalib_Dll_ tracks2tiepoints(const char* pTrackFile, const char* pImgList);

#endif