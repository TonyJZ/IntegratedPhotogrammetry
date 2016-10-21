//ALM基本数据结构定义

#ifndef _ALM_PROJECT_BASE_DEFINE_H_
#define _ALM_PROJECT_BASE_DEFINE_H_


#include "Afxtempl.h"

//////////////////////////////////////////////////////////////////////////
#include <math.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#pragma   warning(disable:4786)
#include <vector>
#pragma   warning(disable:4251)
#include <algorithm> 
#include <functional>  
using namespace::std;

#ifndef PATHLEN
#define PATHLEN  512
#endif

#ifndef NAMELEN
#define NAMELEN  128
#endif

#ifndef IDLEN
#define IDLEN    16
#endif

#ifndef PI
#define PI       3.1415926535897932384626433832795
#endif

#ifndef EI
#define EI 2.718281828459045f
#endif

#ifndef NONEIDX
#define NONEIDX  -999
#endif

#ifndef DEMNoDataValue
#define DEMNoDataValue -99999
#endif

#ifndef _StripInfo
#define _StripInfo
typedef struct tagStripInfo  
{
	CArray<CString, CString&>	ImageList;
	CArray<CString, CString&>	LasList;

	tagStripInfo();
	~tagStripInfo()
	{
		ImageList.RemoveAll();
		LasList.RemoveAll();
	}
}StripInfo,*PStripInfo;
#else
#pragma message("StripInfo已经定义!" )
#endif


#ifndef _BoundingBox2D
#define _BoundingBox2D
typedef struct tagBoundingBox2D 
{
	double	XMax;
	double	XMin;
	double	YMax;
	double	YMin;
	tagBoundingBox2D()
	{
		XMax=0.0;
		XMin=0.0;
		YMax=0.0;
		YMin=0.0;
	}
}BoundingBox2D,*PBoundingBox2D;
#else
#pragma message("BoundingBox2D已经定义!" )
#endif

#ifndef _BoundingBox3D
#define _BoundingBox3D
typedef struct tagBoundingBox3D    //三维包围盒
{
	double	XMax;
	double	XMin;
	double	YMax;
	double	YMin;
	double	ZMax;
	double	ZMin;
	tagBoundingBox3D()
	{
		XMax=0.0;
		XMin=0.0;
		YMax=0.0;
		YMin=0.0;
		ZMax=0.0;
		ZMin=0.0;
	}
}BoundingBox3D,*PBoundingBox3D;
#else
#pragma message("BoundingBox3D已经定义!" )
#endif

#ifndef _PT2D
#define _PT2D
typedef struct tagPT2D        //二维点结构
{
	double   dfX;
	double   dfY;
}PT2D,*PPT2D;
#else
#pragma message("PT2D已经定义!" )
#endif

#ifndef _PT3D
#define _PT3D
typedef struct tagPT3D       //三维点结构
{
	double   dfX;
	double   dfY;
	double   dfZ;
}PT3D,*PPT3D;
#else
#pragma message("PT3D已经定义!" )
#endif

#ifndef _PT4D
#define _PT4D
typedef struct tagPT4D        //四维点结构
{
	double   dfX;
	double   dfY;
	double   dfZ;
	double   dfAlph;
}PT4D,*PPT4D;
#else
#pragma message("PT4D已经定义!" )
#endif

#ifndef _MARKCAM
#define _MARKCAM
typedef struct tagMARKCAM      //标志的结构
{
	double dfX;
	double dfY;
	int    nMarkNO;	
}MARKCAM,*PMAKCAM;
#else
#pragma message("MARKCAM已经定义!" )
#endif

#ifndef _IMGPARA
#define _IMGPARA
typedef struct tagIMGPARA       //影像路径
{	
	int  nIndex;	            //影像索引
	char chImgPath[PATHLEN];    //影像全路径
}IMGPARA,*PIMGPARA;
#else
#pragma message("IMGPARA已经定义!" )
#endif

#ifndef _STRIPPARA
#define _STRIPPARA
typedef struct tagSTRIPPARA            //航带参数
{
	int             nStripImgNum;      //航带影像数
	int             nIsReversal;       //是否旋转180度
	vector<IMGPARA> vImgPara;          //航带内影像列表
}STRIPPARA,*PSTRIPPARA;
#else
#pragma message("STRIPPARA已经定义!" )
#endif

#ifndef _IMGLIST
#define _IMGLIST
typedef struct tagIMGLIST              //单影像航带信息
{
	int nImgID;                        //影像索引(总) (从1开始) 
//    char    chImgPath[PATHLEN];      //影像全路径
//	char    chImgName[PATHLEN];        //影像名
	CString strImgName;                //影像名
	int     nStripID;                  //线号(航带号,从1开始)
	int 	nPhoID;                    //片号(影像在航带内的ID,从1开始)  
	int    nIsReversal;                //是否旋转180度

}IMGLIST,*PIMGLIST; 
#endif

#ifndef _LIDARINFO
#define _LIDARINFO
typedef struct tagLIDARINFO              //LAS航带信息
{
//	char    chLasName[PATHLEN];        //影像名
	CString strLidarName;                //影像名
	int     nStripID;                  //线号(航带号,从1开始)
	tagBoundingBox3D  dfBox3D;         //航带范围

	int    nFlag;                      //标识
}LIDARINFO,*PLIDARINFO; 
#endif

// #ifndef _STRIPLIST
// #define _STRIPLIST
// typedef struct tagSTRIPLIST
// {   	
// 	int      camRev;
// 	int      imgNum;
// 	PIMGLIST pImgLst;	
// }STRIPLIST,*PSTRIPLIST; 
// #endif
// 
// #ifndef _OBVPARA
// #define _OBVPARA
// typedef struct tagOBVPARA
// {   	
// 	unsigned int  nID;	
// 	int           phSN;
// 	int           flag;
// 	double        mx;
// 	double        my;
// 	double        px;
// 	double        py;	
// }OBVPARA,*POBVPARA; 
// #endif
// 
// #ifndef _TIEPARA
// #define _TIEPARA
// typedef struct tagTIEPARA
// {
// 	int starIdx;
// 	int endIdx;
// }TIEPARA,*PTIEPARA; 
// #endif


// struct PHOTOINFO
// {
// 	char name[512];	//影像名
// 	int row;		//影像行号
// 	int col;		//影像列号
// 	BOOL sel;		//是否选中
// 	BOOL rotate;	//是否需要旋转
// 
// 	PHOTOINFO()
// 	{
// 		memset(&name,0,512*sizeof(char));
// 		row=col=-1;
// 		sel=false;
// 		rotate=false;
// 	}
// 
// 	PHOTOINFO(char Name[512],int Row,int Col,BOOL Sel=false,BOOL Rotate=false)
// 	{
// 		strcpy(name,Name);
// 		row=Row;
// 		col=Col;
// 		sel=Sel;
// 		rotate=Rotate;
// 	}
// 
// 	PHOTOINFO(CString Name,int Row,int Col,BOOL Sel=false,BOOL Rotate=false)
// 	{
// 		strcpy(name,Name);
// 		row=Row;
// 		col=Col;
// 		sel=Sel;
// 		rotate=Rotate;
// 	}
// };
#ifndef _CAMPARA
#define _CAMPARA
typedef struct tagCAMPARA         //相机参数的数据结构
{
	double dfU0,dfV0;             //主点(毫米)
	double dfF;			          //焦距(毫米)

	int nWidth,nHeight;           //像幅宽，高(像素)
	double dfPixSize;			  //像元大小
	double dfWidth,dfHeight;      //像幅宽，高(毫米)

//	int unit,origin;		      //坐标系参数		unit 单位: 0,象素; 1,毫米		
	                              //origin 原点:0,左下角; 1,中心点

	int nDistortionNum;		      //畸变模型 3,3参数畸变模型; 4,4参数畸变模型 
	double dfA0,dfA1,dfR;		  //3参数畸变差
	double dfK1,dfK2,dfP1,dfP2;   //4参数畸变差
	double dfK3,dfB1,dfB2;        //Lecia畸变参数

	int nAttrib;                  //相机属性

	tagCAMPARA()
	{
		dfU0=dfV0=dfF=0.0;
		nWidth=nHeight=0;
		dfPixSize=0.0;
		dfWidth=dfHeight=0.0;
		nDistortionNum=4;
		dfK1=dfK2=dfP1=dfP2=0.0;
// 		unit=1;
// 		origin=1;
		dfA0=dfA1=dfR=0.0;
		dfK3=dfB1=dfB2=0.0;

		nAttrib=1111;
	}

	tagCAMPARA(double F,double U0,double V0, double dfW=0.0,double dfH=0.0, double PS=0.0,
		int W=0,int H=0, double K1=0.0,double K2=0.0,double K3=0.0,double P1=0.0,
		double P2=0.0,double B1=0.0,double B2=0.0)
	{
		dfF=F;		
		dfU0=U0;
		dfV0=V0;
		dfWidth=dfW;
		dfHeight=dfH;
		
		dfPixSize=PS;
		nWidth=W;
		nHeight=H;
		dfK1=K1;
		dfK2=K2;
		dfP1=P1;
		dfP2=P2;
		
		dfK3=K3;
		dfB1=B1;
		dfB2=B2;
	}
	void operator = (const tagCAMPARA &camera)
	{
		nDistortionNum=camera.nDistortionNum;
		dfF=camera.dfF;
		dfU0=camera.dfU0;
		dfV0=camera.dfV0;
		dfK1=camera.dfK1;
		dfK2=camera.dfK2;
		dfK3=camera.dfK3;
		dfP1=camera.dfP1;
		dfP2=camera.dfP2;
		dfB1=camera.dfB1;
		dfB2=camera.dfB2;
		dfA0=camera.dfA0;
		dfA1=camera.dfA1;
		dfR=camera.dfR;
		dfPixSize=camera.dfPixSize;
		dfWidth=camera.dfWidth;
		dfHeight=camera.dfHeight;
	}
}CAMPARA,*PCAMPARA;
#else
#pragma message("CAMPARA已经定义!" )
#endif

#ifndef _OUTPARA
#define _OUTPARA
typedef struct tagOUTPARA		//影像外方位元素的数据结构
{
	char chImgName[PATHLEN];    //影像名
	double dfXs,dfYs,dfZs;		//摄影中心坐标或GPS坐标
	double dfPhi,dfOmg,dfKapa;  //三个角元素 (phi-omg-kapa转角系统)
	double dfGPSTime;           //GPS时间(秒)
	int nFlag;                  //标志

	tagOUTPARA()
	{
		dfXs=dfYs=dfZs=0.0;
		dfPhi=dfOmg=dfKapa=0.0;
		dfGPSTime=0;
		nFlag=0;
		chImgName[0]='\0';
	}
	tagOUTPARA(double XS,double YS,double ZS,
		double FAI,double OMG,double KAP,double GPST=0,int Flag=0)
	{
		dfXs=XS;
		dfYs=YS;
		dfZs=ZS;
		dfPhi=FAI;
		dfOmg=OMG;
		dfKapa=KAP;
		dfGPSTime=GPST;
		nFlag=Flag;
	}
}OUTPARA,*POUTPARA;
#else
#pragma message("OUTPARA已经定义!" )
#endif

#ifndef _IMGPOINT
#define _IMGPOINT
typedef struct tagIMGPOINT		        //像点数据结构
{
	long lID;				//影像点的点号
	int nImgID;				//影像点位于的影像号
	double dfX,dfY;  		//横纵坐标

	tagIMGPOINT()
	{
		lID=-1;
		nImgID=-1;
		dfX=dfY=0.0;
	}
	tagIMGPOINT(long Id,int Imgid,double X,double Y)
	{
		lID=Id;
		nImgID=Imgid;
		dfX=X;
		dfY=Y;
	}
}IMGPOINT,*PIMGPOINT;
#else
#pragma message("IMGPOINT已经定义!" )
#endif

#ifndef _CONTROLPOINT
#define _CONTROLPOINT
typedef struct tagCONTROLPOINT		 //空间控制点数据结构
{
	unsigned int nName;              //点名
	double dfX,dfY,dfZ;			     //空间坐标
	char chName[IDLEN];			     //控制点名
	int nAttrib;			//0 一般点；1 平高控制点；2 检查点；
	                        //3 平面控制点；4 高程控制点
	int nGroup;	            //分组信息
	
	tagCONTROLPOINT()
	{
		nName=999000;
		chName[0]='\0';
		dfX=dfY=dfZ=0.0;
		nAttrib=0;
		nGroup=0;
	}
}CONTROLPOINT,*PCONTROLPOINT;
#else
#pragma message("CONTROLPOINT已经定义!" )
#endif

#ifndef _PCONTROLPOINT
#define _PCONTROLPOINT
typedef struct tagPHOTOCONTROLPT		//影像控制点数据结构
{
	char chName[IDLEN];		//控制点名
	int nPhotoID;			//所在影像号
	double dfx,dfy;			//平面坐标

	tagPHOTOCONTROLPT()
	{
		chName[0]='\0';
		nPhotoID=-1;
		dfx=dfy=0.0;
	}
}PHOTOCONTROLPT,*PPHOTOCONTROLPT;
#else
#pragma message("PCONTROLPOINT已经定义!" )
#endif

#ifndef _SPACEPOINT
#define _SPACEPOINT
typedef struct tagSPACEPOINT		//空间点数据结构
{
	long lID;				        //空间点的点号
	double dfX,dfY,dfZ;			    //空间点坐标

	tagSPACEPOINT()
	{
		lID=-1;
		dfX=dfY=dfZ=0.0;
	}
	tagSPACEPOINT(long Id,double x,double y,double z)
	{
		lID=Id;
		dfX=x;
		dfY=y;
		dfZ=z;
	} 
}SPACEPOINT,*PSPACEPOINT;
#else
#pragma message("SPACEPOINT已经定义!" )
#endif

// #ifndef _POINT2D_Def_
// #define _POINT2D_Def_
// typedef struct tagPOINT2D		//二维点数据结构
// {
// 	double x,y;
// }PPOINT2D;
// #else
// #pragma message("POINT_2D已经定义!" )
// #endif

// #ifndef _LINE2D
// #define _LINE2D
// typedef struct tagLINE2D		//二维直线数据结构
// {
// 	POINT2D PTbegin;        	//起点
// 	POINT2D PTend;	            //终点
// 
// 	tagLINE2D()
// 	{
// 		memset(&PTbegin, 0, sizeof(POINT2D));
// //		memset(&PTend, 0, sizeof(POINT2D));
// 	};
// 	tagLINE2D(POINT2D b,POINT2D e)
// 	{
// 		PTbegin.x=b.x;	PTbegin.y=b.y;
// 		PTend.x=e.x;	PTend.y=e.y;
// 	};
// 	tagLINE2D(double x1,double y1,double x2,double y2)
// 	{
// 		PTbegin.x=x1, PTbegin.y=y1;
// 		PTend.x=x2,	PTend.y=y2;
// 	};
// }LINE2D,*PLINE2D;
// #else
// #pragma message("LINE2D已经定义!" )
// #endif

// #ifndef _POLYGON2D
// #define _POLYGON2D
// typedef struct tagPOLYGON2D
// {
// 	int nVNum;			       //顶点数
// 	int nFlag;			       //多边形属性标记
// 	tagPOINT2D *pVertex;	   //顶点数据
// 	tagPOLYGON2D *pNext;	   //下一个多边形
// 
// 	tagPOLYGON2D()
// 	{
// 		nVNum=0;
// 		nFlag=0;
// 		pVertex=NULL;
// 		pNext=NULL;
// 	};
// 
// 	~tagPOLYGON2D()
// 	{
// 		if (pVertex)
// 		{
// 			delete pVertex;
// 			pVertex=NULL;
// 		}
// 		if (pNext)
// 		{
// 			delete pNext;
// 			pNext=NULL;
// 		}
// 	};
// }POLYGON2D,*PPOLYGON2D;
// #else
// #pragma message("POLYGON2D已经定义!" )
// #endif

// #ifndef _POINT3D
// #define _POINT3D
// typedef struct tagPOINT3D		//三维点数据结构
// {
// 	double X,Y,Z;
// 
// 	tagPOINT3D()
// 	{
// 		X=Y=Z=0.0;
// 	}
// 	tagPOINT3D(double x,double y,double z)
// 	{
// 		X=x;
// 		Y=y;
// 		Z=z;
// 	}
// 	tagPOINT3D(SPACEPOINT sp)
// 	{
// 		X=sp.dfX;
// 		Y=sp.dfY;
// 		Z=sp.dfZ;
// 	}
// }POINT3D,*PPOINT3D;
// #else
// #pragma message("POINT_3D已经定义!" )
// #endif

#ifndef _RECTBOX
#define _RECTBOX
typedef struct tagRECTBOX       //矩形结构
{
	float fl,fr,ft,fb;          //左边界 右边界 上边界 下边界
	tagRECTBOX()
	{
		fl=fr=ft=fb=0.0;
	}
	tagRECTBOX(float L,float R,float T,float B)
	{
		fl=L;
		fr=R;
		ft=T;
		fb=B;
	}
	float Width()
	{
		return fr-fl;
	}
	float Height()
	{
		return ft-fb;
	}
}RECTBOX,*PRECTBOX;
#else
#pragma message("RECTBOX已经定义!" )
#endif

#ifndef _ATIMAGE
#define _ATIMAGE
typedef struct tagATIMAGE		//空三影像点
{
	int nImgID;		            //影像号
	double dfx,dfy;		        //x,y坐标
	tagATIMAGE *pNext;	        //下一点地址

	tagATIMAGE()
	{
		nImgID=0;
		dfx=dfy=0.0;
		pNext=NULL;
	}

	tagATIMAGE(int Id,double X,double Y,tagATIMAGE *Next=NULL)
	{
		nImgID=Id;
		dfx=X;
		dfy=Y;
		pNext=Next;
	}

	~tagATIMAGE()
	{
		if (pNext!=NULL)
		{
			delete pNext;
			pNext=NULL;
 		}
	}
}ATIMAGE,*PATIMAGE;
#else
#pragma message("ATIMAGE已经定义!" )
#endif

// #ifndef _ATPOINT
// #define _ATPOINT
// typedef struct tagATPOINT		//空三、自由网数据结构
// {
// 	POINT3D spPoint;	       //空间点坐标
// 	int nImgPtNum;			   //影像点数量
// 	ATIMAGE *pImgPt; 		   //影像点坐标与其对应的影像片号,并连接到下一影像点
// 
// 	tagATPOINT()
// 	{
// 		spPoint = SPACEPOINT();
// 		nImgPtNum=0;
// 		pImgPt=NULL;
// 	}
// 	tagATPOINT(POINT3D sp,int ipnum,ATIMAGE *imgp)
// 	{
// 		spPoint=sp;
// 		nImgPtNum=ipnum;
// 
// 		ATIMAGE *curpt=pImgPt=new ATIMAGE;
// 		ATIMAGE *curp=imgp;
// 
// 		for (int i=0;i<nImgPtNum;i++)
// 		{
// 			memcpy(curpt,curp,sizeof(ATIMAGE));
// 			curp=curp->pNext;
// 			curpt->pNext=new ATIMAGE;
// 			curpt=curpt->pNext;
// 		}
// 		delete curpt;
// 	}
// 
// 	~tagATPOINT()
// 	{
// 		if (pImgPt!=NULL) 
// 		{
// 			delete pImgPt;
// 			pImgPt=NULL;
// 		}
// 	}
// }ATPOINT,*PATPOINT;
// #else
// #pragma message("ATPOINT已经定义!" )
// #endif

// #ifndef _ATLINK
// #define _ATLINK
// typedef struct tagATLINK		//空三点连接码
// {
// 	long lID;			//点号
// 	ATPOINT *pPoint;		//数据地址
// 
// 	tagATLINK()
// 	{
// 		lID=0;
// 		pPoint=NULL;
// 	}
// 
// 	~tagATLINK()
// 	{
// 		if (pPoint!=NULL)
// 		{
// 			delete pPoint;
// 			pPoint=NULL;
// 		}
// 	}
// }ATLINK,*PATLINK;
// #else
// #pragma message("ATLINK已经定义!" )
// #endif

#ifndef _MODTRANSLATE
#define _MODTRANSLATE
typedef struct tagMODTRANSLATE		//模型变换参数的数据结构
{
	double dfScale;				    //比例Scale
	double dfXm,dfYm,dfZm;			//平移Offset
	double dfPhi,dfOmg,dfKapa;	    //旋转Rotation

	tagMODTRANSLATE()
	{
		dfScale=1.0;			
		dfXm=dfYm=dfZm=0.0;
		dfPhi=dfOmg=dfKapa=0.0;		
	}
	tagMODTRANSLATE(double s,double x,double y,double z,double f,double w,double k)
	{
		dfScale=s;
		dfXm=x;
		dfYm=y;
		dfZm=z;
		dfPhi=f;
		dfOmg=w;
		dfKapa=k;		
	}
}MODTRANSLATE,*PMODTRANSLATE;
#else
#pragma message("MODTRANSLATE已经定义!" )
#endif

// #ifndef _GRAYDATA
// #define _GRAYDATA
// typedef struct tagGRAYDATA  //灰度图像块的数据结构
// {
// 	BYTE *data;			//灰度值矩阵，以左下点为原点
// 	int w,h;			//灰度矩阵宽高
// 	double xset,yset;	//灰度矩阵的左下角在原影像中的坐标(在核线相关中恢复灰度的坐标有用)
// 
// 	tagGRAYDATA()
// 	{
// 		data=NULL;
// 		w=h=0;
// 		xset=yset=0;
// 	}
// 	tagGRAYDATA(int W,int H,double X=0,double Y=0)
// 	{
// 		w=W;
// 		h=H;
// 		xset=X;
// 		yset=Y;
// 		data=new BYTE[w*h];
// 
// 		memset(data, 0, w*h*sizeof(BYTE));
// 	}
// 	tagGRAYDATA(int W,int H,BYTE *Data,double X=0,double Y=0)
// 	{
// 		w=W;
// 		h=H;
// 		xset=X;
// 		yset=Y;
// 		data=new BYTE[w*h];
// 
// 		memcpy(data, Data, w*h*sizeof(BYTE));
// 	}
// 
// 	~tagGRAYDATA()
// 	{
// 		if (data!=NULL)
// 		{
// 			delete data;
// 			data=NULL;
// 		}
// 	}
// }GRAYDATA,*PGRAYDATA;
// #else
// #pragma message("GRAYDATA已经定义!" )
// #endif

#ifndef _MATCHPOINT
#define _MATCHPOINT
typedef struct tagMATCHPOINT		//一对匹配点的结构
{
	long	lID;				    //点号
	int		nLinkMark;		        //链接码
	int		nLPID,nRPID;		    //左右影像的影像号
	double	dflx,dfly,dfrx,dfry;	//左右影像上的点的坐标
	BOOL	bIsOld;			        //目标点来源 false 特征点 true 老匹配点
	tagMATCHPOINT()
	{
		lID=-1;
		nLinkMark=-1;
		nLPID=nRPID=-1;
		dflx=dfly=dfrx=dfry=0.0;
		bIsOld=false;
	}
	
	tagMATCHPOINT(long ID,int Link,int Lpno,int Rpno,double Lx,double Ly,double Rx,double Ry,bool Oral)
	{
		lID=ID;
		nLinkMark=Link;
		nLPID=Lpno;
		nRPID=Rpno;
		dflx=Lx;
		dfly=Ly;
		dfrx=Rx;
		dfry=Ry;
		bIsOld=Oral;
	}
}MATCHPOINT,*PMATCHPOINT;
#else
#pragma message("MATCHPOINT已经定义!" )
#endif

// #ifndef _SEEDPOINT
// #define _SEEDPOINT
// typedef struct tagSEEDPOINT
// {
// 	int row1,col1;	//左影像的航带号，航片号
// 	double x1,y1;	//左影像上的x，y坐标
// 
// 	int row2,col2;	//右影像的航带号，航片号
// 	double x2,y2;	//右影像上的x，y坐标
// 
// 	tagSEEDPOINT()
// 	{
// 		row1=col1=-1;
// 		x1=y1=0;
// 
// 		row2=col2=-1;
// 		x2=y2=0;	
// 	}
// 	tagSEEDPOINT(int R1,int C1,double X1,double Y1,int R2,int C2,double X2,double Y2)
// 	{
// 		row1=R1;
// 		col1=C1;
// 		x1=X1;
// 		y1=Y1;
// 
// 		row2=R2;
// 		col2=C2;
// 		x2=X2;
// 		y2=Y2;	
// 	}
// }SEEDPOINT,*PSEEDPOINT;
// #else
// #pragma message("SEEDPOINT已经定义!" )
// #endif
// 
// #ifndef _STRIPOFFSET
// #define _STRIPOFFSET
// typedef struct tagSTRIPOFFSET
// {
// 	int row;
// 	int offset;
// 
// 	tagSTRIPOFFSET()
// 	{
// 		row=0;
// 		offset=0;
// 	}
// }STRIPOFFSET,*PSTRIPOFFSET;
// #else
// #pragma message("STRIPOFFSET已经定义!" )
// #endif
// 
// #ifndef _IPADDRESS
// #define _IPADDRESS
// typedef struct tagIPADDRESS
// {
// 	int id1,id2,id3,id4;
// 
// 	tagIPADDRESS()
// 	{
// 		id1=id2=id3=id4=0;
// 	}
// 
// 	tagIPADDRESS(int Id1,int Id2,int Id3,int Id4)
// 	{
// 		id1=Id1;
// 		id2=Id2;
// 		id3=Id3;
// 		id4=Id4;
// 	}
// }IPADDRESS,*PIPADDRESS;
// #else
// #pragma message("IPADDRESS已经定义!" )
// #endif
// 
// //
// //为多边形裁切定义的结构
// #ifndef _POLYVERTEX
// #define _POLYVERTEX
// typedef struct tagPOLYVERTEX  //双向链表的多边形顶点结构
// {
// 	int idno; //顶点的id，id号为0，则为新增顶点
// 	double x,y;//顶点的坐标
// 
// 	tagPOLYVERTEX *front; //前向节点指针
// 	tagPOLYVERTEX *after; //后向节点指针
// 
// 	//在该节点为新节点时，以下结构起作用
// 	tagPOLYVERTEX *goalfr; //指向被裁多边形的前向节点指针
// 	tagPOLYVERTEX *goalaf; //指向被裁多边形的后向节点指针
// 
// 	tagPOLYVERTEX *cutfr; //指向裁切多边形的前向节点指针
// 	tagPOLYVERTEX *cutaf; //指向裁切多边形的后向节点指针
// 
// 	tagPOLYVERTEX()
// 	{
// 		int idno=0;
// 		x=y=0;
// 		front=after=goalfr=goalaf=cutfr=cutaf=NULL;
// 	}
// 
// 	~tagPOLYVERTEX()
// 	{
// 		if (after&&after->idno) 
// 		{
// 			delete after;
// 			after=NULL;
// 		}
// 		front=after=goalfr=goalaf=cutfr=cutaf=NULL;
// 	}
// }POLYVERTEX,*PPOLYVERTEX;
// #else
// #pragma message("POLYVERTEX已经定义!" )
// #endif
// 
// #ifndef _SUPPOLYGON
// #define _SUPPOLYGON
// typedef struct tagSUPPOLYGON   //双向链表多边形的结构
// {
// 	int polyid;				//多边形编号
// 	int pnum;				//结点数
// 	POLYVERTEX *pvertex;	//首结点指针
// 	POLYVERTEX *endvert;	//末结点指针
// 
// 	tagSUPPOLYGON()
// 	{
// 		polyid=-1;
// 		pnum=0;
// 		pvertex=NULL;
// 		endvert=NULL;
// 	}
// 
// 	~tagSUPPOLYGON()
// 	{
// 		if (pvertex)
// 		{
// 			delete pvertex;
// 			pvertex=NULL;
// 		}
// 	}
// }SUPPOLYGON,*PSUPPOLYGON;
// #else
// #pragma message("SUPPOLYGON已经定义!" )
// #endif


#endif