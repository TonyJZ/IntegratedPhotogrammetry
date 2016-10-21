#ifndef _ATN_Define_header_H__
#define _ATN_Define_header_H__

//#include "LidBaseDef.h"
#include "orsPointCloud\orsIPointCloud.h"

#pragma pack(1) // 按照1字节方式进行对齐
#ifndef _ATN_RawPoint_Def_
#define _ATN_RawPoint_Def_

class ATN_RawPoint
{
public:
	int x;
	int y;
	int z;
	unsigned short intensity;
	unsigned char return_number : 3;
	unsigned char number_of_returns_of_given_pulse : 3;
	unsigned char scan_direction_flag : 1;
	unsigned char edge_of_flight_line : 1;
	unsigned char classification;
	char scan_angle_rank;
	unsigned char user_data;
	unsigned short point_source_ID;

	double gpstime;
	int latitude;	// (WGS84/radians - ALS50 OPTICAL CENTER)
	int longitude;	// (WGS84/radians - ALS50 OPTICAL CENTER)
	int altitude;	// (WGS84/meters - ALS50 OPTICAL CENTER)
	int roll;		// (radians)
	int pitch;		// (radians)
	int heading;	// (radians)
	int scanAngle;	// (radians)
	int range;		// (meters)

	ATN_RawPoint()
	{
		x=0;
		y=0;
		z=0;
		intensity=0;
		edge_of_flight_line=0;
		scan_direction_flag=0;

		number_of_returns_of_given_pulse = 1;
		return_number = 1;

		classification = 0;
		scan_angle_rank = 0;
		user_data = 0;
		point_source_ID = 0;
		
		gpstime=0;
		latitude=0;
		longitude=0;
		altitude=0;
		roll=0;
		pitch=0;
		heading=0;
		range=0;
		scanAngle=0;
	};

};
#endif

// #ifndef _ATN_TP_RawPoint_Def_
// #define _ATN_TP_RawPoint_Def_
// typedef struct ATN_TP_RawPoint
// {
// 	int x;
// 	int y;
// 	int z;
// 	unsigned short intensity;
// 	unsigned char return_number : 3;
// 	unsigned char number_of_returns_of_given_pulse : 3;
// 	unsigned char scan_direction_flag : 1;
// 	unsigned char edge_of_flight_line : 1;
// 	unsigned char classification;
// 	char scan_angle_rank;
// 	unsigned char user_data;
// 	unsigned short point_source_ID;
// 
// 	double gpstime;
// 	int Xs;	// (WGS84/radians - ALS50 OPTICAL CENTER)
// 	int Ys;	// (WGS84/radians - ALS50 OPTICAL CENTER)
// 	int Zs;	// (WGS84/meters - ALS50 OPTICAL CENTER)
// 	float phi;		// (radians)
// 	float omega;		// (radians)
// 	float kappa;	// (radians)
// 	float scanAngle;	// (radians)
// 	int range;		// (meters)
// 
// } ATN_TP_RawPoint;
// #endif
#pragma pack() // 取消1字节对齐方式

const int ATN_RawPoint_Length=sizeof(ATN_RawPoint);
//const int ATN_TP_RawPoint_Length=sizeof(ATN_TP_RawPoint);

#ifndef _ATNPoint_Def_
#define _ATNPoint_Def_
typedef struct ATNPoint
{
	double x;		//(WGS84/radians)
	double y;		//(WGS84/radians)
	double z;
	unsigned short intensity;
	unsigned char return_number : 3;
	unsigned char number_of_returns_of_given_pulse : 3;
	unsigned char scan_direction_flag : 1;
	unsigned char edge_of_flight_line : 1;
	unsigned char classification;
	char scan_angle_rank;
	unsigned char user_data;
	unsigned short point_source_ID;

	double gpstime;
// 	double latitude;	// (WGS84/radians - ALS50 OPTICAL CENTER)	Xs	
// 	double longitude;	// (WGS84/radians - ALS50 OPTICAL CENTER)	Ys
// 	double altitude;	// (WGS84/meters - ALS50 OPTICAL CENTER)	Zs
// 	double roll;		// (radians)								phi
// 	double pitch;		// (radians)								omega
// 	double heading;		// (radians)								kappa
// 	double scanAngle;	// (radians)
// 	double range;		// (meters)
	orsPointObservedInfo obsInfo;

} ATNPoint;
#endif

// #ifndef _ATN_TP_PointDef_
// #define _ATN_TP_PointDef_
// typedef struct ATNTP_Point
// {
// 	double x;		//(TPlane/meter)
// 	double y;		//(TPlane/meter)
// 	double z;		//(TPlane/meter)
// 	unsigned short intensity;
// 	unsigned char return_number : 3;
// 	unsigned char number_of_returns_of_given_pulse : 3;
// 	unsigned char scan_direction_flag : 1;
// 	unsigned char edge_of_flight_line : 1;
// 	unsigned char classification;
// 	char scan_angle_rank;
// 	unsigned char user_data;
// 	unsigned short point_source_ID;
// 
// 	double gpstime;
// 	double Xs;			//(TPlane/meter)	
// 	double Ys;			//(TPlane/meter)
// 	double Zs;			//(TPlane/meter)
// 	double phi;			// (radians)
// 	double omega;		// (radians)
// 	double kappa;		// (radians)
// 	double scanAngle;	// (radians)
// 	double range;		// (meters)
// 
// } ATNTP_Point;
// #endif


typedef struct ATNPt_Block
{
	ATNPt_Block* buffer_next;     // used for efficient memory management and as a list pointer
	ATNPoint *pItem;				// stores the actual point coordinates
} ATNPt_Block;

#ifndef	_D_RECT
#define _D_RECT
struct D_RECT
{
	double	XMax;
	double	XMin;
	double	YMax;
	double	YMin;
}; 
#endif

#ifndef _LidBlock_Def_
#define _LidBlock_Def_
struct LidBlock
{
	int idx;			//块号
	D_RECT boundbox;	//包围盒
	int    ptNum;		//点数
	__int64  fileOffset;	//文件偏移值

	ATNPt_Block *pData;
};
#endif


#endif