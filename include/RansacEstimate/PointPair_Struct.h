#ifndef _POINT_PAIR_STRUCTURE_H_ZJ_2014_04_30_
#define _POINT_PAIR_STRUCTURE_H_ZJ_2014_04_30_


typedef struct
{
	int ptId0;
	float xi0, yi0, zi0;		
	float scale0, orient0;
	int ptId1;
	float xi1, yi1, zi1;
	float scale1, orient1;
	bool  bAccepted;

	float vx, vy;

} myPtPair;

typedef struct
{
	float xi, yi;	//像素坐标
	float X, Y, Z;	//物方坐标
	bool  bAccepted;

} my3DProjPair;


#endif