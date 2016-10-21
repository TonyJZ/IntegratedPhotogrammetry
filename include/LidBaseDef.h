#ifndef _Lid_Base_define_h_ZJ_2010_08_25_
#define _Lid_Base_define_h_ZJ_2010_08_25_

//////////////////////////////////////////////////////////////////////////
//公用数据结构定义
//////////////////////////////////////////////////////////////////////////
#include "orsBase/orstypedef.h"

#ifndef _D_3DOT
#define _D_3DOT
struct  D_3DOT
{
	double	x,y,z;
};

#endif

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

#ifndef _POINT2D_Def_
#define _POINT2D_Def_
typedef orsPOINT2D		POINT2D;
// struct POINT2D	{
// 	double x, y;
// };

#endif

#ifndef _POINT3D_Def_
#define _POINT3D_Def_
typedef	orsPOINT3D		POINT3D;
// struct POINT3D	{
// 	double X, Y, Z;
// };

#endif

#ifndef _LAS_Format_Def
#define _LAS_Format_Def
//公共头
#pragma pack(1) // 按照1字节方式进行对齐

struct PublicHeadBlock 
{
	char				FileSignature[4];
	unsigned short		File_Source_ID ;                     
	unsigned short		Reserved;                        
	unsigned long 		Project_ID_GUID_data1 ;            
	unsigned short 		Project_ID_GUID_data2;            
	unsigned short  	Project_ID_GUID_data3;          
	unsigned char		Project_ID_GUID_data4[8];          
	unsigned char		Version_Major;                            
	unsigned char		Version_Minor;                              
	char				System_Identifier[32] ;                        
	char				Generating_Software[32];                            
	unsigned short		File_Creation_Day_of_Year;             
	unsigned short		File_Creation_Year ;                  
	unsigned short		Header_Size  ;                              
	unsigned long		Offset_to_point_data;                         
	unsigned long		Number_of_variable_length_records;           
	unsigned char		Point_Data_Format_ID ;       
	unsigned short		Point_Data_Record_Length;                   
	unsigned long		Number_of_point_records;                    
	unsigned long 		Number_points_by_return[5];              
	double 				X_scale_factor;                                   
	double				Y_scale_factor;                                    
	double				Z_scale_factor;                               
	double				X_offset;                                           
	double				Y_offset;                                     
	double				Z_offset;                                
	double				Max_X;                                             
	double				Min_X;                                            
	double				Max_Y;                                     
	double				Min_Y;                                        
	double				Max_Z;                                            
	double				Min_Z; 	
};

//变长记录
struct VariableLengthRecordHead  
{
	unsigned short		RecordSignature; 
	char				userID[16];
	unsigned short		RecordID;
	unsigned short		RecordLengthAfterHead;
	char				Description[32];	
};


//format1
struct POINTSTRUOFLAS1
{
	long				x;
	long				y;
	long				z;
	unsigned short 		Intensity ;      
	
	unsigned char		Reserved;	//ReturnNumber;3 bits (bits 0, 1, 2)  , NumberOFReturns;3 bits (bits 3, 4, 5) ,  ScanDirectionFlag;1 bit (bit 6) , Edge of FlightLine;1 bit (bit 7)总共为1byte        
	unsigned char		Classification;                         
	char				ScanAngleRankLeftside ;                     
	unsigned char		FileMaker;                                        
	unsigned short 		UserBitField; 
	double				GPSTime;
};

struct POINTSTRUOFLAS0
{
	long				x;
	long				y;
	long				z;
	unsigned short 		Intensity ;      
	
	unsigned char		Reserved;	//include (ReturnNumber;3 bits (bits 0, 1, 2)  , NumberOFReturns;3 bits (bits 3, 4, 5) ,  ScanDirectionFlag;1 bit (bit 6) , Edge of FlightLine;1 bit (bit 7)总共为1byte)        
	
	unsigned char		Classification;                         
	char				ScanAngleRankLeftside ;                     
	unsigned char		FileMaker;                                        
	unsigned short 		UserBitField; 
	
};



//format1
struct POINTSTRULAS1
{
	long				x;
	long				y;
	long				z;
	unsigned short 		Intensity ;      
	
	unsigned char		Reserved;	//ReturnNumber;3 bits (bits 0, 1, 2)  , NumberOFReturns;3 bits (bits 3, 4, 5) ,  ScanDirectionFlag;1 bit (bit 6) , Edge of FlightLine;1 bit (bit 7)总共为1byte        
	unsigned char  ReturnNumber;     //一个脉冲的返回值个数
	unsigned char  NumberofReturns;  //脉冲的返回数量
	unsigned char  ScanDirectionFlag;//扫描方向标志
	unsigned char  EdgeofFlightLine; //飞行线边界标志(结束为1)


	unsigned char		Classification;                         
	char				ScanAngleRankLeftside ;                     
	unsigned char		FileMaker;                                        
	unsigned short 		UserBitField; 
	double				GPSTime;
};



//format0
struct POINTSTRULAS0
{
	long				x;
	long				y;
	long				z;
	unsigned short 		Intensity ;      
	
	unsigned char		Reserved;	//include (ReturnNumber;3 bits (bits 0, 1, 2)  , NumberOFReturns;3 bits (bits 3, 4, 5) ,  ScanDirectionFlag;1 bit (bit 6) , Edge of FlightLine;1 bit (bit 7)总共为1byte)        
	unsigned char  ReturnNumber;     //一个脉冲的返回值个数
	unsigned char  NumberofReturns;  //脉冲的返回序数
	unsigned char  ScanDirectionFlag;//扫描方向标志
	unsigned char  EdgeofFlightLine; //飞行线边界标志(结束为1)

	unsigned char		Classification;                         
	char				ScanAngleRankLeftside ;                     
	unsigned char		FileMaker;                                        
	unsigned short 		UserBitField; 
	
};




struct PointRecordHead 
{
	char				FileSignature[4];
	unsigned long		Number_of_point_records;
   	double				Max_X;                                             
	double				Min_X;                                            
	double				Max_Y;                                     
	double				Min_Y;                                        
	double				Max_Z;                                            
	double				Min_Z; 
	
	
};
#pragma pack() // 取消1字节对齐方式 
#endif



#endif