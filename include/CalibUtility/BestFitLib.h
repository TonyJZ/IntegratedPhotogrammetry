#ifndef _BEST_FIT_LIB_ZJ_2016_04_12_H_
#define _BEST_FIT_LIB_ZJ_2016_04_12_H_

#ifdef _CALIB_UTILITY_
#define  _CalibUtility_Dll_  __declspec(dllexport)
#else
#define  _CalibUtility_Dll_  __declspec(dllimport)	
#endif

//notice:  using scatter points fitting, unconsidered direction of the line
void _CalibUtility_Dll_ line_fitting(int ptNum, double *point, double *line_gradient, double *line_intercept);






#endif