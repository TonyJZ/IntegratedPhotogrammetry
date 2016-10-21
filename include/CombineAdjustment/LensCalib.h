#ifndef _LENS_CALIBRATION_INCLUDE_H_
#define _LENS_CALIBRATION_INCLUDE_H_


//镜头畸变的验后补偿法
//x0, y0, k0, k1, k2, p1, p2
bool Cal_LensDistortion(void *pdata, double *distParam);



#endif