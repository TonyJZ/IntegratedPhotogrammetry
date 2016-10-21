// CALMPrjManager.h: interface for the CALMPrjManager class.	
//////////////////////////////////////////////////////////////////////
/*#########################################################################################################
功能描述：
		ALM工程管理器  管理维护工程相关信息。


修改列表：
		创建于2008.12.17
			创建人:	Jing ZHANG   zhangjing_whu@foxmail.com
					Quanye DU	 duquanye@163.com
		2008.12.18 基本结构的定义
		2009.3.27  工程文件改为XML格式
		2009.5.28 释放出#include "Markup.h" 不再绕过编译器
		2009.6.5  补充获取las范围函数
		2009.7.22 修改影像列表信息增加ImageID 影像总体索引 从1开始
		
		2010.3.3  针对整体摄影测量的需求进行修改	zj	
		2010.3.17 默认影像在同一个目录下，只记录影像名和目录名，不对每个影像记录全路径

#########################################################################################################*/

#if !defined(AFX_ALMPRJMANAGER_H__A014D0BD_69B7_49CD_824C_F1E2836D83E3__INCLUDED_)
#define AFX_ALMPRJMANAGER_H__A014D0BD_69B7_49CD_824C_F1E2836D83E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ALMDefine.h"
#include "IPhBaseDef.h"



#ifdef _ALMPRJManager_
#define _ALMPrj_ __declspec(dllexport)
//#include "Markup.h"
#else
#define _ALMPrj_ __declspec(dllimport)
//#define CMarkup void
#endif

#ifndef _ALMPRJManager_
#ifdef _DEBUG
#pragma comment(lib, "ALMPrjManagerD.lib")
#pragma message("Automatically Linking With ALMPrjManagerD.dll")
#else
#pragma comment(lib, "ALMPrjManager.lib")
#pragma message("Automatically Linking With ALMPrjManager.dll")
#endif
#endif


class CMarkup;

class _ALMPrj_ CALMPrjManager  
{
public:
	CALMPrjManager();
	virtual ~CALMPrjManager();

public:
	
	bool	CreateALMPrj(char *pAlmFilePath);		//新建工程
	bool	OpenALMPrjFile(CString strAlmFile);		//打开工程文件
	bool	CloseALMPrjFile();						//关闭工程文件
	bool	SaveALMPrjFile();						//保存工程文件
	bool	SaveALMPrjAs(CString strAlmFile);		//另存为

	//工程是否被修改
	bool	IsPrjModified();

			
	//与其他文件的交互(pos,eoe,cpt,camera)
	BOOL	bReadPOSData();								//读POS数据
	BOOL    bWritePOSData();							//写POS数据
	BOOL	bReadEOEData();								//读外方位元素数据
	BOOL    bWriteEOEData();							//写外方位元素数据
	BOOL	bReadCPTData();								//读控制点数据
	BOOL    bWriteCPTData();							//写控制点数据
	BOOL    bReadCameraPara(CString CameraFile);		//读相机参数
	BOOL    bWriteCameraPara(CString CameraFile);		//写相机参数


	void    SetAlmFile(CString strAlmFile);           //设置工程文件全路径
	void    SetImageCount(int nImgNum);	              //设置影像总数
	void    SetRawImageDir(CString strRImgDir);		  //设置原始影像目录
	void    SetImgStripNum(int nImgStripNum);		  //设置影像航带总数
	void	SetLidarStripNum(int nLidarStripNum);     //设置Lidar航带总数
	void    SetRawLidarDir(CString strRLidarDir);	  //设置原始Lidar目录
	void	SetATGridNum(int nATGridNum);	          //设置空三匹配格网数 
	void	SetDSMGridSize(int nDSMGridSize);         //设置DSM匹配格网大小

	void	SetMaxGrdElev(double dfGrdElev);
	void	SetAverGrdElev(double dfGrdElev);         //设置地面平均高程   
	void	SetMinGrdElev(double dfGrdElev);

	void	SetFlyOverlap(short flyoverlap);		  //设置影像重叠度	
	void	SetStripOverlap(short stripoverlap);

	void	SetPOSFile(CString strPOSFile);           //设置POS数据文件            
	void	SetEOEFile(CString strEOEFile);           //设置配准后的外方位元素文件 
	void	SetCPtFile(CString strCPtFile);           //设置地面控制点数据文件     
	void	SetColorLutFile(CString strColorLutFile); //设置Lidar数据色表文件      
	void	SetImgQVDir(CString strImgQVDir);         //设置缩略图路径             
	void	SetDgImageDir(CString strDgImageDir);      //设置匀光匀色后影像路径     
	void	SetMosaicDir(CString strMosaicDir);       //设置拼图路径               
	void	SetRegistDir(CString strRegistDir);       //设置Lidar与影像配准数据路径
	void	SetFltLidarDir(CString strFltLidarDir);       //设置滤波后Lidar数据路径      
	void	SetMatchLidarDir(CString strMatchLidarDir);   //设置匹配后Lidar数据路径      
	void	SetDSMDir(CString strDSMDir);             //设置数字表面模型数据路径   
	void	SetDEMDir(CString strDEMDir);             //设置数字高程模型数据路径   
	void	SetDOMDir(CString strDOMDir);             //设置正射影像数据路径       
	void	SetCFilesDir(CString strCFilesDir);       //设置控制点等文件路径       
	void	SetLOGDir(CString strLOGDir);             //设置日志文件路径           
	void	SetProductDir(CString strProductDir);     //设置产品路径        
	void	SetCameraPara(CAMPARA campara);			  //设置相机参数

	BOOL	SetMetaInfo(const CArray<CString,CString&> &aryStrMetInfo);	      //设置元数据
	BOOL    SetImgList(const iphImgList &aryImgList); //设置影像列表
	BOOL    SetLidarList(const CArray<LIDARINFO,LIDARINFO&> &aryLidarList); //设置LAS列表
    BOOL	SetEoeData(const CArray<OUTPARA,OUTPARA&> &aryEoeData); //设置EOE data


	//工程信息查询接口	(直接取内存信息)
	CString GetAlmFile(){return m_strAlmFile;};    //获取工程文件全路径
	CString GetAlmFileDir(){return m_strAlmFileDir;};    //获取工程文件路径
	CString GetAlmName(){return m_strAlmName;};    //获取工程文件名		
	int 	GetImageCount()  //获取影像总数
	{
		m_nImgNum=m_aryImgList.GetSize();
		return m_nImgNum;
	};    
	CString	GetRawImageDir(){return m_strRawImgDir;};   //获取原始影像目录
	CString	GetRawLidarDir(){return m_strRawLidarDir;};		//获取原始Lidar目录
	int 	GetImgStripNum(){return m_nImgStripNum;};   //获取影像航带总数			
	int		GetLidarStripNum()  //获取Lidar航带总数
	{
		m_nLidarStripNum=m_aryLidarList.GetSize();
		return m_nLidarStripNum;
	};               
	int		GetATGridNum(){return m_nATGridNum;};                   //获取空三匹配格网数 
	int		GetDSMGridSize(){return m_nDSMGridSize;};               //获取DSM匹配格网大小

	double	GetMinGrdElev(){return m_dMinGrdElev;};
	double	GetAverGrdElev(){return m_dAverGrdElev;};                   //获取地面平均高程   
	double	GetMaxGrdElev(){return m_dMaxGrdElev;};

	short   GetFlyOverlap() {return m_flyOverlap;};
	short	GetStripOverlap() {return m_stripOverlap;};

	CString GetPOSFile(){return m_strPOSFile;};                 //获取POS数据文件            
	CString GetEOEFile(){return m_strEOEFile;};                 //获取配准后的外方位元素文件 
	CString GetCPtFile(){return m_strCPtFile;};                 //获取地面控制点数据文件     
	CString GetColorLutFile(){return m_strColorLutFile;};       //获取Lidar数据色表文件      
	CString GetImgQVDir(){return m_strImgQVDir;};               //获取缩略图路径             
	CString GetDgImageDir(){return m_strDgImageDir;};            //获取匀光匀色后影像路径     
	CString GetMosaicDir(){return m_strMosaicDir;};             //获取拼图路径               
	CString GetRegistDir(){return m_strRegistDir;};             //获取Lidar与影像配准数据路径
	CString GetFltLidarDir(){return m_strFltLidarDir;};             //获取滤波后Lidar数据路径      
	CString GetMatchLidarDir(){return m_strMatchLidarDir;};         //获取匹配后Lidar数据路径      
	CString GetDSMDir(){return m_strDSMDir;};                   //获取数字表面模型数据路径   
	CString GetDEMDir(){return m_strDEMDir;};                   //获取数字高程模型数据路径   
	CString GetDOMDir(){return m_strDOMDir;};                   //获取正射影像数据路径       
	CString GetCFilesDir(){return m_strCFilesDir;};             //获取控制点等文件路径       
	CString GetLOGDir(){return m_strLOGDir;};                   //获取日志文件路径           
	CString GetProductDir(){return m_strProductDir;};           //获取产品路径               
	CAMPARA* GetCameraPara(){return &m_Camera;};                //获取相机参数

	CArray<CString,CString&>* GetMetaInfo(){return &m_aryStrMetaInf;};	 //获取元数据
	iphImgList* GetImgList(){return &m_aryImgList;};     //获取影像列表
	CArray<LIDARINFO,LIDARINFO&>* GetLidarList(){return &m_aryLidarList;};     //获取Lidar列表
	CArray<OUTPARA,OUTPARA&>* GetPOSData(){return &m_aryPOSData;};     //获取POS数据
	CArray<OUTPARA,OUTPARA&>* GetEOEData(){return &m_aryEOEData;};     //获取外方位元素数据
	CArray<CString,CString&>* GetDEMProduct(){return &m_aryDEMProduct;};
	CArray<CString,CString&>* GetDOMProduct(){return &m_aryDOMProduct;};
	CArray<CString,CString&>* GetThemeProduct(){return &m_aryTMProduct;};

	bool	GetSwathBoundingBox(char *pLidarFile, BoundingBox3D *pBox);	   //取一条Lidar的包围盒
	//virtual	bool	GetPrjBoundingBox(BoundingBox3D *pBox)=0;					   //取整个工程包围盒
	//判断路径是否存在
	bool bIsDirectoryExist(const char *sCurDir);
	//选择文件夹路径
	bool bSelectFolderDialog(char* pszDlgTitle, CString *strFolder, HWND hParent=NULL);
	//文件是否存在
	bool bIsFileExist(const char *filename);

public:
	
	void Copy(CALMPrjManager& __x);

protected:	
	//工程文件IO
	bool	bReadAlmFile(CString strAlmFile);		//读取该工程文件先,m_strAlmFile赋值全路径
	bool	bWriteAlmFile(CString strAlmFile);		//写入工程文件 m_strAlmFile为全路径
	void	InitALMPrjManager();					//初始化工程管理
	
	//读取工程文件头信息，,m_strAlmFile赋值全路径
	bool	bReadAlmFileHeader();
	//读取工程文件影像信息
	bool	bReadAlmFileImgInfo();
	//读取工程文件Lidar信息
	bool	bReadAlmFileLidarInfo();
	//读取工程文件相机参数
	bool	bReadAlmFileCameraParas();
	//读取工程文件影像匹配参数
	bool	bReadAlmFileImgMachParas();
	//读取工程文件相关文件路径信息
	bool	bReadAlmFileDirInfo();
    //读取工程文件相关产品路径信息
	bool	bReadAlmFileProductInfo();
	
	//写入工程文件头信息，,m_strAlmFile赋值全路径
	bool	bWriteAlmFileHeader();
	//写入工程文件影像信息
	bool	bWriteAlmFileImgInfo();
	//写入工程文件Lidar信息
	bool	bWriteAlmFileLidarInfo();
	//写入工程文件相机参数
	bool	bWriteAlmFileCameraParas();
	//写入工程文件影像匹配参数
	bool	bWriteAlmFileImgMachParas();
	//写入工程文件相关文件路径信息
	bool	bWriteAlmFileDirInfo();
    //写入工程文件相关产品路径信息
	bool	bWriteAlmFileProductInfo();
	

protected:
	bool		m_bIsPrjModified;		//工程文件中的信息是否已被修改

private:
	//XML读写
	CMarkup* m_xml;

	CString m_ALMFileFlag;      //文件标识 "ALMPSProjectFile"
	CString	m_strAlmFile;	    //工程文件全路径
	CString	m_strAlmFileDir;	//工程文件路径 .alm文件所在的路径
	CString	m_strAlmName;		//工程文件路径 不包括后缀 ".alm"

	CArray<CString,CString&>	m_aryStrMetaInf;	            //元数据信息

	int m_nImgNum;                          //影像总数
	CString	m_strRawImgDir;		            //原始影像目录
	int m_nImgStripNum;                     //影像航带总数
//	CArray<IMGLIST,IMGLIST> m_aryImgList;   //影像列表
	iphImgList		m_aryImgList;			//影像列表

	CArray<OUTPARA,OUTPARA&> m_aryPOSData;   //POS数据
	CArray<OUTPARA,OUTPARA&> m_aryEOEData;   //外方位元素数据
	CArray<CONTROLPOINT,CONTROLPOINT&> m_aryGCPData;   //地面控制点数据
	CAMPARA m_Camera;                       //相机参数

	double	m_dMaxGrdElev;					//地面最大高程
	double	m_dAverGrdElev;                    //地面平均高程
	double	m_dMinGrdElev;					//地面最小高程

	short	m_flyOverlap;					//航向重叠
	short	m_stripOverlap;					//旁向重叠


	int m_nLidarStripNum;                     //Lidar航带总数
	CString m_strRawLidarDir;	            	//原始Lidar文件目录
	CArray<LIDARINFO,LIDARINFO&> m_aryLidarList;   //Lidar列表

	
	int m_nATGridNum;                       //空三匹配格网数
	int m_nDSMGridSize;                     //DSM匹配格网大小
 
	CString m_strPOSFile;                   //POS数据文件
	CString m_strEOEFile;                   //配准后的外方位元素文件
	CString m_strCPtFile;                   //地面控制点数据文件
	CString m_strColorLutFile;              //Lidar数据色表文件
	CString m_strImgQVDir;                  //缩略图路径
	CString m_strDgImageDir;                //匀光匀色后影像路径
	CString m_strMosaicDir;                 //拼图路径
	CString m_strRegistDir;                 //Lidar与影像配准数据路径
	CString m_strFltLidarDir;                 //滤波后Lidar数据路径
	CString m_strMatchLidarDir;               //匹配后Lidar数据路径
	CString m_strDSMDir;                    //数字表面模型数据路径
	CString m_strDEMDir;                    //数字高程模型数据路径
	CString m_strDOMDir;                    //正射影像数据路径
	CString m_strCFilesDir;                 //控制点等文件路径
	CString m_strLOGDir;                    //日志文件路径
	CString m_strProductDir;                //产品路径
	CArray<CString,CString&> m_aryDOMProduct;   //DOM产品数据
	CArray<CString,CString&> m_aryDEMProduct;   //DEM产品数据
	CArray<CString,CString&> m_aryTMProduct;    //专题产品数据

};

typedef CArray<LIDARINFO,LIDARINFO&> iphLIDARList;

#endif // !defined(AFX_ALMPRJMANAGER_H__A014D0BD_69B7_49CD_824C_F1E2836D83E3__INCLUDED_)
