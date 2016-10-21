//  旋转角和旋转矩阵计算
#ifndef _Calib_GEOMETRY_H_zj_20110813
#define _Calib_GEOMETRY_H_zj_20110813

#include "CalibBasedef.h"

#ifdef _CALIB_UTILITY_
#define  _CalibUtility_Dll_  __declspec(dllexport)
#else
#define  _CalibUtility_Dll_  __declspec(dllimport)	
#endif

// #ifndef _CALIB_UTILITY_
// #ifdef _DEBUG
// #pragma comment(lib,"CalibUtilityD.lib")
// #else
// #pragma comment(lib,"CalibUtility.lib")
// #endif
// #endif

#ifndef _dVector_ANGLE_Def_
#define _dVector_ANGLE_Def_
//
// 向量对方向角的偏导数
//
struct dVector_ANGLE 	{
	double da_alpha, da_beta;
	double db_alpha, db_beta;
	double dc_beta;
};
#endif


/************************************ RotateMat.c ****************************
	function :	set up the rotation matrix
	input 	 :	a,b,c--------- angle array
	output	 :	*r--------- rotation matrix

				                 | 0 -c -b |      | 0 -k -f |
	R=(I+S)*transpose(I-S)   S== | c  0 -a |/2 == | k  0 -w |/2
				                 | b  a  0 |      | f  w  0 |

****************************************************************************/
void _CalibUtility_Dll_ RotateMat_abc(double a,double b,double c, double *r);
_CalibUtility_Dll_ double R2abc(double *R, double *ao, double *bo, double *co);

//
// 采用中式的phi, omega, kappa表达法, 中式的旋转矩阵
//
_CalibUtility_Dll_ void RotateMat_fwk(double phi, double omega, double kapa, double *r);
_CalibUtility_Dll_ void R2fwk(double *R, double *phi, double *omega, double *kapa);

//
// 采用欧美的omega, phi, kappa表达法, 中式的旋转矩阵
//
_CalibUtility_Dll_ void RotateMat_wfk(double omega, double phi, double kapa, double *r );
_CalibUtility_Dll_ void R2wfk(double *R, double *phi, double *omega,  double *kapa);


/********************************************************************************
L1-----P-----L2
       |
       P1
********************************************************************************/
_CalibUtility_Dll_ double Perpendicular( double x1, double y1, double x2, double y2, double px, double py, double *x, double *y );

_CalibUtility_Dll_ double P3ArcPara(POINT3D pts[3], double *xc,double *yc,double ang[3]);
_CalibUtility_Dll_ double P3ArcPara(POINT2D pts[3], double *xc,double *yc,double ang[3]);

_CalibUtility_Dll_ double TanP2ArcPara(double a,double b, POINT3D pts[2], double *xc,double *yc,double ang[2]);


//////////////////////////////////////////////////////////////////////////
//载体系 ---> 导航系   (radian)
_CalibUtility_Dll_ void RotateMat_roll(double roll, double *r);
_CalibUtility_Dll_ void RotateMat_pitch(double pitch, double *r);
_CalibUtility_Dll_ void RotateMat_heading(double heading, double *r);
_CalibUtility_Dll_ void RotateMat_rph(double roll, double pitch, double heading, double *r);

//////////////////////////////////////////////////////////////////////////
//切平面与地心直角坐标系之间的旋转矩阵   (radian)
//TPlane采用北东地形式，WGS84地心直角坐标
_CalibUtility_Dll_ void RotateMat_NED2ECEF(double lat0, double lon0, double *R);
_CalibUtility_Dll_ void RotateMat_ENU2ECEF(double lat0, double lon0, double *R);
//void RotateMat_TPlane2WGS84_NED(double lat0, double lon0, double *R);

//地固坐标系到切平面坐标系的旋转矩阵，TPlane采用东北天形式
_CalibUtility_Dll_ void RotateMat_ECEF2NED(double lat0, double lon0, double *R);
_CalibUtility_Dll_ void RotateMat_ECEF2ENU(double lat0, double lon0, double *R);

//void RotateMat_WGS842TPlane_ENU1(double lat0, double lon0, double *R);

//void RotateMat_WGS842TPlane_ENU2(double lat0, double lon0, double *R);


#endif