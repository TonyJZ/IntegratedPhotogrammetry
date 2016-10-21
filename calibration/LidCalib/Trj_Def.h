/*
//	航迹文件定义：《TerraScan User’s Guide》 pp162
*/

#ifndef _Trajectory_Binary_File_Define_h__
#define _Trajectory_Binary_File_Define_h__



//The structure of trj file header
typedef struct {
	char Recog[8] ; // TSCANTRJ
	int Version ; // File version 20010715
	int HdrSize ; // sizeof(TrajHdr)
	int PosCnt ; // Number of position records
	int PosSize ; // Size of position records
	char Desc[79] ; // Description
	BYTE Quality ; // Quality for whole trajectory (1-5)
	double BegTime ; // First time stamp
	double EndTime ; // Last time stamp
	int OrigNbr ; // Original number (before any splitting)
	int Number ; // Flightline number (in laser points)
	char VrtVideo[400] ; // Vertical facing video
	double VrtBeg ; // Start time of VrtVideo[]
	double VrtEnd ; // End time of VrtVideo[]
	char FwdVideo[400] ; // Forward facing video
	double FwdBeg ; // Start time of FwdVideo[]
	double FwdEnd ; // End time of FwdVideo[]
} TrajHdr ;


// Dp3d structure is:
#ifndef _Dp3d_Define_
#define _Dp3d_Define_
typedef struct {
	double x ;
	double y ;
	double z ;
} Dp3d ;
#endif

//The structure of the trajectory position records is:
typedef struct {
	double Time ; // Time stamp (seconds in some system)

//	Dp3d Xyz ; // Position
	
 	double x;	//Position	与导出时选择的投影有关
 	double y;
 	double z;
	double heading ; // Heading (degrees)
	double roll ; // Roll (degrees)
	double pitch ; // Pitch (degrees)
	int Quality ; // Quality tag value (0-5)
	int Mark ; // Run time flag
} TrajPos ;	


typedef struct
{
	int		imgID;		//片号
	int		stripID;	//线号
	CString imgName;

	double  x;			//meter
	double	y;
	double	z;
	double  phi;		//radian
	double  omega;
	double  kappa;
	
	double	R[9];	

	BYTE	rotatesys;	//转角系统：1  YXZ(phi-omega-kappa); 2  XYZ(omega-phi-kappa); 3  ZYZ

} imagePos;



#endif