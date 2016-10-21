#ifndef _LIDAR_CALIB_CORRESPONDING_FILE_IO_H__ZJ_2016_03_25_
#define _LIDAR_CALIB_CORRESPONDING_FILE_IO_H__ZJ_2016_03_25_

#include "CalibBasedef.h"

#ifdef _CALIB_UTILITY_
#define  _CalibUtility_Dll_  __declspec(dllexport)
#else
#define  _CalibUtility_Dll_  __declspec(dllimport)	
#endif

const char CorrespFileFlag[32] = "Corresp_File_V_1.0";

bool _CalibUtility_Dll_ read_CorrespFile(const char *pszFile, int *objType, int *nObj, LidMC_Obj **pCorObj);

bool _CalibUtility_Dll_ write_CorrespFile(const char *pszFile, const int nObj, LidMC_Obj *pCorObj);



#endif