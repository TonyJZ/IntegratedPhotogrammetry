#ifndef _LIDAR_CALIBRATION_PARAMETER_FILE_IO_H__
#define _LIDAR_CALIBRATION_PARAMETER_FILE_IO_H__

#ifdef _CALIB_UTILITY_
#define  _CalibUtility_Dll_  __declspec(dllexport)
#else
#define  _CalibUtility_Dll_  __declspec(dllimport)	
#endif

const char CPFLAG_RIGOROUS[64] = "CalibParam_File_RigorousModel_V1.0";
const char CPFLAG_YUAN[64] = "CalibParam_File_YuanModel_V1.0";
const char CPFLAG_JING[64] = "CalibParam_File_JingModel_V1.0";
const char CPFLAG_RESSL[64] = "CalibParam_File_ResslModel_V1.0";
const char CPFLAG_TRAJAIDED[64] = "CalibParam_File_TrajAidedModel_V1.0";


enum LidCalib_ModelType
{
	LCMT_Rigorous = 0,
	LCMT_Yuan,
	LCMT_Ressl,
	LCMT_Jing,
	LCMT_TrajAided
};



bool _CalibUtility_Dll_ write_CPfile(const char *pFileName, LidCalib_ModelType type, void *CalibParam, int num);

bool _CalibUtility_Dll_ read_CPfile(const char *pFileName, LidCalib_ModelType &type, void **CalibParam, int &num);




#endif