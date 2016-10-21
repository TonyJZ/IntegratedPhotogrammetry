#ifndef _Point_Interpolation_h_ZJ_2010_08_31_
#define	_Point_Interpolation_h_ZJ_2010_08_31_


#include "\openrs\desktop\include\orsBase\orsUtil.h"
#include "..\lastool\lasdefinitions.h"
#include "Geometry.h"
#include "MemoryPool.h"


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


//转换成影像保存
bool _lidCalib_Dll_ ConvertToImage(char *pszName, LASpointXYZI *pData, int ptNum, double gridsize, int style=0);

bool _lidCalib_Dll_ ConvertToImage(char *pszName, POINT3D *pData, int ptNum, double gridsize);

bool _lidCalib_Dll_ ConvertToImage(char *pszName, CMemoryPool<POINT3D, POINT3D&> *pData, double gridsize);

#endif