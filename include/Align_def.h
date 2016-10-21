#ifndef _Alignment_Define_h_zj_2010_10_06_
#define _Alignment_Define_h_zj_2010_10_06_

//#include "vld.h"

#include "Afxtempl.h"
#include "LidBaseDef.h"
//#include "orsBase/orsString.h"
#include "orsBase/orsTypedef.h"


//#pragma pack(8) 

#define sFactor 1000L		//条带因子
//#define sFlag	10000000L	//最多标记1000个条带，每条带最多1000张影像

inline int getStripID(int ID)
{
	return (ID /*- sFlag*/) / sFactor;
};

inline int getPhotoID(int ID)
{
	return (ID /*- sFlag*/) % sFactor;
};

inline int getUniqueID(int stripID, int photoID)
{
	return /*sFlag+*/stripID*sFactor+photoID;
};

#ifndef _Lid_Line_Def_
#define _Lid_Line_Def_
typedef struct Align_LidLine		//LiDAR扫描条带
{	
	int LineID;				//(从1开始)
	CString GridName;		//点云对应的格网文件名
	CString LasName;		//标准的las文件名
//	CString AtnName;        //Atn文件名
	CString TrjName;		//trj文件名
	double  averFH;			//平均航高
} Align_LidLine;
#endif

#ifndef _Align_Image_Def_
#define _Align_Image_Def_
typedef struct Align_Image				//单影像航带信息
{
	int nImgID;                        //影像索引(总) (从0001开始) sFlag+nStripID*1000+imgID 
	CString ImgName;					//影像名
// 	int     nStripID;                  //线号(航带号,从1开始)
// 	int 	nPhoID;                    //片号(影像在航带内的ID,从1开始)  
	int    nIsReversal;                //是否旋转180度
	double timestamp;				   //相片的时标 2014.12.23 add
}Align_Image;
#endif

//#ifndef _Tie_Object_Def_
//#define _Tie_Object_Def_
typedef enum 
{
    ST_LiDAR=0,	//3D坐标
	ST_Image=1	//2D坐标
} Source_Type;

typedef enum
{
    TO_POINT=0, TO_LINE=1, TO_PATCH=2
} TieObj_Type; 

//tie point type
#ifndef Tiepoint_Type
#define Tiepoint_Type
#define  TP_type_undefine		0
#define  TP_type_vertical		1		//高程点
#define  TP_type_horizon		2		//平面点
#define  TP_type_hor_ver		3		//平高点
#endif

class TieObject
{
public:
	TieObject(){};
	virtual ~TieObject(){};


public:
	int sourceID;	//数据条带ID (影像/LiDAR)   从1开始
	Source_Type sourceType;	//lidar, image
	int	tieID;		//add 2010.10.13 by zj 支持反向查找。在多视图窗口的单点追踪机制   从1开始
	TieObj_Type objType;		//类型：POINT/LINE/PATCH
};

class TiePoint : public TieObject
{
public:
	TiePoint() 
	{ 
		tpType=TP_type_undefine; 
	};

	char tpType;
	orsPOINT2D pt2D;
	orsPOINT3D pt3D;
};

class TieLine : public TieObject
{
public:
	TieLine() { pt2D=0; pt3D=0; };
	virtual ~TieLine() 
	{ 
		if(pt2D)	delete pt2D; pt2D=0; 
		if(pt3D)	delete pt3D; pt3D=0;
	};

	int ptNum;
	orsPOINT2D *pt2D;
	orsPOINT3D *pt3D;
};

class TiePatch : public TieObject
{
public:

	TiePatch() { pt2D=0; pt3D=0; };
	virtual ~TiePatch() 
	{ 
		if(pt2D) delete pt2D; pt2D=0; 
		if(pt3D) delete pt3D; pt3D=0;
	};

	int ptNum;
	orsPOINT2D *pt2D;
	orsPOINT3D *pt3D;
};

// typedef struct TieObject
// {
// 	int sourceID;			//数据条带ID
// 	char type;				//类型：POINT/LINE/PATCH
// 	union 	
// 	{
// 		POINT2D tiePoint;	//像方坐标	
// 		POINT2D *tieLine;
// 		POINT2D *tiePatch;
// 	}obj;
// }TieObject;
//#endif

//多片连接点(线、面)链表
class TieObjChain
{
public:
	int TieID;				//连接点ID 从1开始
	TieObj_Type type;
	bool bDel;
	int  objNum;
	CArray<TieObject*, TieObject*> *pChain;
	
	bool bGCP;
	orsPOINT3D gcp;

	TieObjChain() 
	{ 
		TieID=-1;
		type=TO_POINT;
		objNum=0;
		pChain=NULL; 
		bDel=false;
		bGCP=false;
	}

	virtual ~TieObjChain()
	{
		if(pChain)
		{
			TieObject *pObj;
			for(int i=0; i<pChain->GetSize(); i++)
			{
				pObj=pChain->GetAt(i);
				delete pObj;
			}
			pChain->RemoveAll();

			delete pChain;
		}
		pChain=NULL; 
		bDel=false;
		bGCP=false;
	}

};

//地面控制点
// typedef struct TieObj_GCP
// {
// 	POINT3D gcp;
// 	int tieObjID;
// }TieObj_GCP;

enum ORS_LINKMSG	{
	ORS_LM_LINK,
		ORS_LM_MOUSEMOVE,
		ORS_LM_ObjType,				//标记连接物类型
		ORS_LM_TOMeasure_Exit,		//进程终止
		ORS_LM_RegistViewer_Exit,
		ORS_LM_Measure_Info			//坐标量测信息
};

struct linkMSG 
{
	UINT id;   //消息类型
	
	int sourceID;	//数据条带ID (影像/LiDAR)
	Source_Type sourceType;	//lidar, image
	int	tieID;		//add 2010.10.13 by zj 支持反向查找。在多视图窗口的单点追踪机制 
	TieObj_Type objType;		//类型：POINT/LINE/PATCH

	
	int ptNum;	//对象总点数
	int ptID;	//当前点号(从0开始)
	orsPOINT2D pt2D;	//像点坐标
	orsPOINT3D pt3D;	//物方坐标

	double zoom;
	UINT nWindows;
	HWND hLinkWindows[32];
	UINT bWindowLinkOn[32];
};

// static int linkMSGLen(int ptNum)
// {
// 	int len;
// 	len=sizeof(UINT)+sizeof(TieObj_Type)+sizeof(int)+sizeof(POINT2D)*ptNum+sizeof(int)+sizeof(double)
// 		+sizeof(UINT)+sizeof(HWND)*32+sizeof(UINT)*32;
// 
// 	return len;
// }

//标记outlier
#define keypoint_outlier	0x02
#define keypoint_inlier	0x03

struct Align_Keypoint
{
	UINT ID;
	orsPOINT2D pt2D;		//像素坐标
	orsPOINT3D pt3D;		//物方坐标
	char flag;
};

// struct Keypoint3D
// {
// 	ors_uint32 ID;
// 	double x, y, z;		//物方坐标
// 	ors_byte flag;
// };


//#pragma pack()
#endif