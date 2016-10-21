// CALMPrjManager.cpp: implementation of the CALMPrjManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ALMProjectManager.h"
#include "..\..\include\ALMPrjManager.h"
#include "Markup.h"
#include "..\..\include\lidBase.h"

//#include "lidBase.h"
//#include "LASFileIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



HINSTANCE oldRcHandle;

CALMPrjManager::CALMPrjManager()
{
	m_xml = new CMarkup;
	InitALMPrjManager();
}

CALMPrjManager::~CALMPrjManager()
{
	m_strAlmFile="";
	m_ALMFileFlag="ALMPSProjectFile";
	m_strAlmFile="";	                 //工程文件全路径
	m_strAlmFileDir="NULL";	             //工程文件路径 .alm文件所在的路径
	m_strAlmName="";		             //工程文件路径 不包括后缀 ".alm"
	
	m_nImgNum=0;                           //影像总数
	m_strRawImgDir="NULL";		           //原始影像目录
	m_nImgStripNum=0;                      //影像航带总数
	m_aryImgList.RemoveAll();              //影像列表
	m_nLidarStripNum=0;                      //Lidar航带总数
	m_strRawLidarDir="NULL";	               //原始Lidar文件目录
	m_aryStrMetaInf.RemoveAll();           //元数据信息
	m_aryLidarList.RemoveAll();              //Lidar列表
	m_aryPOSData.RemoveAll();              //POS数据
	m_aryEOEData.RemoveAll();              //外方位元素数据
	m_aryGCPData.RemoveAll();              //地面控制点数据
	
	m_nATGridNum=50;                       //空三匹配格网数
	m_nDSMGridSize=20;                     //DSM匹配格网大小

	m_dMinGrdElev=-100.0;
	m_dAverGrdElev=0.0;                      //地面平均高程 
	m_dMaxGrdElev=100.0;

	m_strPOSFile="NULL";                   //POS数据文件
	m_strEOEFile="NULL";                   //配准后的外方位元素文件
	m_strCPtFile="NULL";                   //地面控制点数据文件
	m_strColorLutFile="NULL";              //Lidar数据色表文件
	m_strImgQVDir="NULL";                  //缩略图路径
	m_strDgImageDir="NULL";                //匀光匀色后影像路径
	m_strMosaicDir="NULL";                 //拼图路径
	m_strRegistDir="NULL";                 //Lidar与影像配准数据路径
	m_strFltLidarDir="NULL";                 //滤波后Lidar数据路径
	m_strMatchLidarDir="NULL";               //匹配后Lidar数据路径
	m_strDSMDir="NULL";                    //数字表面模型数据路径
	m_strDEMDir="NULL";                    //数字高程模型数据路径
	m_strDOMDir="NULL";                    //正射影像数据路径
	m_strCFilesDir="NULL";                 //控制点等文件路径
	m_strLOGDir="NULL";                    //日志文件路径
	m_strProductDir="NULL";                //产品路径
	m_aryDOMProduct.RemoveAll();           //DOM产品数据
	m_aryDEMProduct.RemoveAll();           //DEM产品数据
	m_aryTMProduct.RemoveAll();            //专题产品数据

	if (m_xml) 
	{
		delete m_xml;
		m_xml=NULL;
	}
}

void CALMPrjManager::InitALMPrjManager()
{
	m_strAlmFile="";
	m_ALMFileFlag="ALMPSProjectFile";
	m_strAlmFile="";	                 //工程文件全路径
	m_strAlmFileDir="NULL";	                 //工程文件路径 .alm文件所在的路径
	m_strAlmName=_T("空工程");		             //工程文件路径 不包括后缀 ".alm"
        	
	m_nImgNum=0;                           //影像总数
	m_strRawImgDir="NULL";		           //原始影像目录
	m_nImgStripNum=0;                      //影像航带总数
	m_aryImgList.RemoveAll();              //影像列表
	m_nLidarStripNum=0;                      //Lidar航带总数
	m_strRawLidarDir="NULL";	               //原始Lidar文件目录
	m_aryStrMetaInf.RemoveAll();           //元数据信息
	m_aryLidarList.RemoveAll();              //Lidar列表
	m_aryPOSData.RemoveAll();              //POS数据
	m_aryEOEData.RemoveAll();              //外方位元素数据
	m_aryGCPData.RemoveAll();              //地面控制点数据

	m_nATGridNum=50;                       //空三匹配格网数
	m_nDSMGridSize=20;                     //DSM匹配格网大小
	
	m_dMinGrdElev=-100.0;
	m_dAverGrdElev=0.0;                      //地面平均高程 
	m_dMaxGrdElev=100.0;

	m_flyOverlap=65;
	m_stripOverlap=30;

	m_strPOSFile="NULL";                   //POS数据文件
	m_strEOEFile="NULL";                   //配准后的外方位元素文件
	m_strCPtFile="NULL";                   //地面控制点数据文件
	m_strColorLutFile="NULL";              //Lidar数据色表文件
	m_strImgQVDir="NULL";                  //缩略图路径
	m_strDgImageDir="NULL";                //匀光匀色后影像路径
	m_strMosaicDir="NULL";                 //拼图路径
	m_strRegistDir="NULL";                 //Lidar与影像配准数据路径
	m_strFltLidarDir="NULL";                 //滤波后Lidar数据路径
	m_strMatchLidarDir="NULL";               //匹配后Lidar数据路径
	m_strDSMDir="NULL";                    //数字表面模型数据路径
	m_strDEMDir="NULL";                    //数字高程模型数据路径
	m_strDOMDir="NULL";                    //正射影像数据路径
	m_strCFilesDir="NULL";                 //控制点等文件路径
	m_strLOGDir="NULL";                    //日志文件路径
	m_strProductDir="NULL";                //产品路径
	m_aryDOMProduct.RemoveAll();           //DOM产品数据
	m_aryDEMProduct.RemoveAll();           //DEM产品数据
	m_aryTMProduct.RemoveAll();            //专题产品数据

	m_bIsPrjModified=false;
}
//读取工程文件头信息，,m_strAlmFile赋值全路径
bool	CALMPrjManager::bReadAlmFileHeader()
{
	while ( m_xml->FindChildElem("MetaInformation") )
	{    
		m_xml->IntoElem();
		while(m_xml->FindChildElem( "Info" ))
		{
			CString strInfo;
			strInfo = m_xml->GetChildData();
			m_aryStrMetaInf.Add(strInfo);
		}		
		m_xml->OutOfElem();	
	}	
	return true;
}
//读取工程文件影像信息
bool	CALMPrjManager::bReadAlmFileImgInfo()
{
	HINSTANCE oldRcHandle = AfxGetResourceHandle();
#ifdef _DEBUG
	AfxSetResourceHandle ( GetModuleHandle("ALMPrjManagerD.dll") );
#else
	AfxSetResourceHandle ( GetModuleHandle("ALMPrjManager.dll") );
#endif

	CString	strImagePath;
	CString	strErr;
	while ( m_xml->FindChildElem( "ImageInformation" ))
	{		
		m_xml->IntoElem();
		//影像总数
		m_xml->FindChildElem( "ImageCount" );
		m_nImgNum = atoi(m_xml->GetChildData());		
		//影像路径
		m_xml->FindChildElem( "ImageDirectory" );
		m_strRawImgDir = m_xml->GetChildData(); 
		if(bIsDirectoryExist(m_strRawImgDir.GetBuffer(128))==false)
		{//重新设置原始影像目录
			
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_RawImg_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strRawImgDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strRawImgDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置原始影像目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strRawImgDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置原始影像目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strRawImgDir=strNewDir;
						break;
					}
				}
			}
		}

		//影像总线数
		m_xml->FindChildElem( "ImageStripCount" );
		m_nImgStripNum = atoi(m_xml->GetChildData());		
		//影像列表
		while ( m_xml->FindChildElem( "ImageList" ))
		{			
			m_xml->IntoElem();	
			for (int i=0;i<m_nImgNum;i++)
			{
				iphImgINFO imglist;
				m_xml->FindChildElem( "ImageID");
				imglist.nImgID = atoi(m_xml->GetChildData());
				m_xml->FindChildElem( "ImageName");
				imglist.strImgName = m_xml->GetChildData();
				m_xml->FindChildElem( "StripID" );
				imglist.nStripID = atoi(m_xml->GetChildData());				
				m_xml->FindChildElem( "PhotoID" );
				imglist.nPhoID = atoi(m_xml->GetChildData());
				m_xml->FindChildElem( "RotateFlag" );
				imglist.nIsReversal = atoi(m_xml->GetChildData());
// 				m_xml->FindChildElem( "UseFlag" );
// 				imglist.useful = atoi(m_xml->GetChildData());

				//m_xml->FindChildElem("CameraFileName");
				CString str=imglist.strImgName;
				int pos;
				pos=str.ReverseFind('.');
				str=str.Left(pos);
				str+=".cam";
				//sprintf(imglist.camera.m_pCameraFileName, str.GetBuffer(256));
				imglist.camera.ReadCameraFile(str.GetBuffer(256));
				
				m_aryImgList.Add(imglist);

				strImagePath=/*m_strRawImgDir+"\\"+*/imglist.strImgName;
				if(bIsFileExist(strImagePath.GetBuffer(128))==false)
				{
					strErr.Format("无法定位%s", strImagePath);
					AfxMessageBox(strErr.GetBuffer(256), MB_OK|MB_ICONEXCLAMATION);
					CString		strNewDir;
					CString		strTmp;
					strTmp.LoadString(IDS_ALM_RawImg_Dir);
					strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
					if(m_strRawImgDir.Compare(strNewDir)!=0)
					{
						strImagePath=strNewDir+"\\"+imglist.strImgName;
						if(bIsFileExist(strImagePath.GetBuffer(128)))
						{
							m_bIsPrjModified=true;
							m_strRawImgDir=strNewDir;
						}
						else
						{
							while(1)
							{
								if(bSelectFolderDialog("请设置原始影像目录...",&strNewDir))
								{
									m_bIsPrjModified=true;
									m_strRawImgDir=strNewDir;
									break;
								}
							}
						}
					}
					else
					{
						while(1)
						{
							if(bSelectFolderDialog("请设置原始影像目录...",&strNewDir))
							{
								m_bIsPrjModified=true;
								m_strRawImgDir=strNewDir;
								break;
							}
						}
					}
				}
			}
			m_xml->OutOfElem();
		}
		
		m_xml->OutOfElem();
	}

	AfxSetResourceHandle ( oldRcHandle );

	return true;
}
//读取工程文件Lidar信息
bool	CALMPrjManager::bReadAlmFileLidarInfo()
{
	HINSTANCE oldRcHandle = AfxGetResourceHandle();
#ifdef _DEBUG
	AfxSetResourceHandle ( GetModuleHandle("ALMPrjManagerD.dll") );
#else
	AfxSetResourceHandle ( GetModuleHandle("ALMPrjManager.dll") );
#endif

	CString	strLidarPath;
	CString strErr;
	while ( m_xml->FindChildElem( "LidarInformation" ))
	{
		
		m_xml->IntoElem();
		//Lidar路径
		m_xml->FindChildElem( "LidarDirectory" );
		m_strRawLidarDir = m_xml->GetChildData();
		if(bIsDirectoryExist(m_strRawLidarDir.GetBuffer(128))==false)
		{
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_RawLidar_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strRawLidarDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strRawLidarDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置原始Lidar目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strRawLidarDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置原始Lidar目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strRawLidarDir=strNewDir;
						break;
					}
				}
			}
		}
		//Lidar总线数
		m_xml->FindChildElem( "LidarStripCount" );
		m_nLidarStripNum = atoi(m_xml->GetChildData());
		
		//Lidar列表
		while ( m_xml->FindChildElem( "LidarList" ))
		{
			m_xml->IntoElem();	
			for (int i=0;i<m_nLidarStripNum;i++)
			{
				LIDARINFO Lidarlist;
				m_xml->FindChildElem( "LidarName");
				Lidarlist.strLidarName=m_xml->GetChildData();			
				m_xml->FindChildElem( "StripID" );
				Lidarlist.nStripID=atoi(m_xml->GetChildData());
				m_xml->FindChildElem( "MinX" );
				Lidarlist.dfBox3D.XMin=atof(m_xml->GetChildData());
				m_xml->FindChildElem( "MinY" );
				Lidarlist.dfBox3D.YMin=atof(m_xml->GetChildData());
				m_xml->FindChildElem( "MinZ" );
				Lidarlist.dfBox3D.ZMin=atof(m_xml->GetChildData());
				m_xml->FindChildElem( "MaxX" );
				Lidarlist.dfBox3D.XMax=atof(m_xml->GetChildData());
				m_xml->FindChildElem( "MaxY" );
				Lidarlist.dfBox3D.YMax=atof(m_xml->GetChildData());
				m_xml->FindChildElem( "MaxZ" );
				Lidarlist.dfBox3D.ZMax=atof(m_xml->GetChildData());
				m_aryLidarList.Add(Lidarlist);

				strLidarPath=/*m_strRawLidarDir+"\\"+*/Lidarlist.strLidarName;
				if(bIsFileExist(strLidarPath.GetBuffer(128))==false)
				{
					strErr.Format("无法定位%s", strLidarPath);
					AfxMessageBox(strErr.GetBuffer(256), MB_OK|MB_ICONEXCLAMATION);

					CString		strNewDir;
					CString		strTmp;
					strTmp.LoadString(IDS_ALM_RawLidar_Dir);
					strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
					if(m_strRawLidarDir.Compare(strNewDir)!=0)
					{
						strLidarPath=strNewDir+"\\"+Lidarlist.strLidarName;
						if(bIsFileExist(strLidarPath.GetBuffer(128)))
						{
							m_bIsPrjModified=true;
							m_strRawLidarDir=strNewDir;
						}
						else
						{
							while(1)
							{
								if(bSelectFolderDialog("请设置原始Lidar目录...",&strNewDir))
								{
									m_bIsPrjModified=true;
									m_strRawLidarDir=strNewDir;
									break;
								}
							}
						}
					}
					else
					{
						while(1)
						{
							if(bSelectFolderDialog("请设置原始Lidar目录...",&strNewDir))
							{
								m_bIsPrjModified=true;
								m_strRawLidarDir=strNewDir;
								break;
							}
						}
					}
				}

			}
			m_xml->OutOfElem();
		}
		
		m_xml->OutOfElem();
	}

	AfxSetResourceHandle ( oldRcHandle );

	return true;
}
//读取工程文件相机参数
bool	CALMPrjManager::bReadAlmFileCameraParas()
{
	while ( m_xml->FindChildElem("CameraInformation"))
	{		
		m_xml->IntoElem();
		
		//相机参数
		m_xml->FindChildElem( "X0" );
		m_Camera.dfU0=atof(m_xml->GetChildData());
		m_xml->FindChildElem( "Y0" );
		m_Camera.dfV0=atof(m_xml->GetChildData());
		m_xml->FindChildElem( "F" );
		m_Camera.dfF=atof(m_xml->GetChildData());
		m_xml->FindChildElem( "PixelSize" );
		m_Camera.dfPixSize=atof(m_xml->GetChildData());
		m_xml->FindChildElem( "FrameWidth" );
		m_Camera.nWidth=atof(m_xml->GetChildData());
		m_xml->FindChildElem( "FrameHeight" );
		m_Camera.nHeight=atof(m_xml->GetChildData());
		
		//畸变类型
		m_xml->FindChildElem( "DistortionType" );
		m_Camera.nDistortionNum=atoi(m_xml->GetChildData());
		//畸变参数	
		if (m_Camera.nDistortionNum==4)
		{
			m_xml->FindChildElem( "K1" );
			m_Camera.dfK1=atof(m_xml->GetChildData());
			m_xml->FindChildElem( "K2" );
			m_Camera.dfK2=atof(m_xml->GetChildData());
			m_xml->FindChildElem( "P1" );
			m_Camera.dfP1=atof(m_xml->GetChildData());
			m_xml->FindChildElem( "P2" );
			m_Camera.dfP2=atof(m_xml->GetChildData());
			
		}
		else  if (m_Camera.nDistortionNum==3)         //[Count_A0_A1_R]
		{
			m_xml->FindChildElem( "A0" );
			m_Camera.dfA0=atof(m_xml->GetChildData());
			m_xml->FindChildElem( "A1" );
			m_Camera.dfA1=atof(m_xml->GetChildData());
			m_xml->FindChildElem( "R" );
			m_Camera.dfR=atof(m_xml->GetChildData());		
		}
		else
		{
			return false;
		}
		m_xml->OutOfElem();
	}
	return true;
}
//读取工程文件影像匹配参数
bool	CALMPrjManager::bReadAlmFileImgMachParas()
{
	while ( m_xml->FindChildElem("ImgMachInformation"))
	{
		m_xml->IntoElem();
		
		//空三格网数
		m_xml->FindChildElem( "ATGridNum" );
		m_nATGridNum=atoi(m_xml->GetChildData());
		//DSM格网
		m_xml->FindChildElem( "DSMGridSize" );
		m_nDSMGridSize=atoi(m_xml->GetChildData());
		
		//地面平均高程
		m_xml->FindChildElem( "AverGroundElev" );
		m_dAverGrdElev=atof(m_xml->GetChildData());
		//最大高程
		m_xml->FindChildElem( "MaxGroundElev" );
		m_dMaxGrdElev=atof(m_xml->GetChildData());
		//最小高程
		m_xml->FindChildElem( "MinGroundElev" );
		m_dMinGrdElev=atof(m_xml->GetChildData());

		
		m_xml->OutOfElem();
	}
	return true;
}
//读取工程文件相关文件路径信息
bool	CALMPrjManager::bReadAlmFileDirInfo()
{
	HINSTANCE oldRcHandle = AfxGetResourceHandle();
#ifdef _DEBUG
	AfxSetResourceHandle ( GetModuleHandle("ALMPrjManagerD.dll") );
#else
	AfxSetResourceHandle ( GetModuleHandle("ALMPrjManager.dll") );
#endif

	while ( m_xml->FindChildElem("FileFolderInformation"))
	{
		m_xml->IntoElem();		
		//POS数据全路径
		m_xml->FindChildElem( "POSDataFile"); m_strPOSFile = m_xml->GetChildData();
		//外方位元素数据全路径
		m_xml->FindChildElem( "ExteriorOrientationElementsFile"); m_strEOEFile = m_xml->GetChildData();
		//控制点数据全路径
		m_xml->FindChildElem( "ControlPointFile"); m_strCPtFile = m_xml->GetChildData();
		//Lidar色表文件全路径
		m_xml->FindChildElem( "ColorLutFile"); m_strColorLutFile = m_xml->GetChildData();
		//缩略图路径
		m_xml->FindChildElem( "ImageQuickViewDirectory"); m_strImgQVDir = m_xml->GetChildData();
		if(bIsDirectoryExist(m_strImgQVDir.GetBuffer(128))==false)
		{//重新设置原始影像目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_QV_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strImgQVDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strImgQVDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置缩略图目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strImgQVDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置缩略图目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strImgQVDir=strNewDir;
						break;
					}
				}
			}
		}
		//匀色后影像路径
		m_xml->FindChildElem( "DodgingImageDirectory"); m_strDgImageDir = m_xml->GetChildData();
		if(bIsDirectoryExist(m_strDgImageDir.GetBuffer(128))==false)
		{//重新设置匀色影像目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_DodgImg_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strDgImageDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strDgImageDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置匀色后影像目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strDgImageDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置匀色后影像目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strDgImageDir=strNewDir;
						break;
					}
				}
			}
			
		}
		//拼图影像路径
		m_xml->FindChildElem( "MosaicDirectory"); m_strMosaicDir = m_xml->GetChildData();
		if(bIsDirectoryExist(m_strMosaicDir.GetBuffer(128))==false)
		{//重新设置拼图目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_Mosaic_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strMosaicDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strMosaicDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置拼图影像目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strMosaicDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置拼图影像目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strMosaicDir=strNewDir;
						break;
					}
				}
			}
			
		}
		//Lidar与影像配准数据路径
		m_xml->FindChildElem( "RegistDirectory"); m_strRegistDir = m_xml->GetChildData();
		if(bIsDirectoryExist(m_strRegistDir.GetBuffer(128))==false)
		{//重新设置配准目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_Regist_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strRegistDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strRegistDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置配准数据目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strRegistDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置配准数据目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strRegistDir=strNewDir;
						break;
					}
				}
			}
		}
		//滤波后Lidar数据路径
		m_xml->FindChildElem( "FilterLidarDirectory"); m_strFltLidarDir = m_xml->GetChildData();
		if(bIsDirectoryExist(m_strFltLidarDir.GetBuffer(128))==false)
		{//重新设置滤波后Lidar数据目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_FilterLidar_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strFltLidarDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strFltLidarDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置滤波后Lidar数据目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strFltLidarDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置滤波后Lidar数据目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strFltLidarDir=strNewDir;
						break;
					}
				}
			}
		}
		//匹配后Lidar数据路径
		m_xml->FindChildElem( "MatchLidarDirectory"); m_strMatchLidarDir = m_xml->GetChildData();
		if(bIsDirectoryExist(m_strMatchLidarDir.GetBuffer(128))==false)
		{//重新设置匹配后Lidar数据目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_MatchLidar_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strMatchLidarDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strMatchLidarDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置匹配后Lidar数据目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strMatchLidarDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置匹配后Lidar数据目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strMatchLidarDir=strNewDir;
						break;
					}
				}
			}
		}
		//DSM数据路径
		m_xml->FindChildElem( "DSMDirectory"); m_strDSMDir = m_xml->GetChildData();	
		if(bIsDirectoryExist(m_strDSMDir.GetBuffer(128))==false)
		{//重新设置DSM数据目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_DSM_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strDSMDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strDSMDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置DSM数据目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strDSMDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置DSM数据目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strDSMDir=strNewDir;
						break;
					}
				}
			}
		}
		//DEM数据路径
		m_xml->FindChildElem( "DEMDirectory"); m_strDEMDir = m_xml->GetChildData();	
		if(bIsDirectoryExist(m_strDEMDir.GetBuffer(128))==false)
		{//重新设置DEM数据目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_DEM_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strDEMDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strDEMDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置DEM数据目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strDEMDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置DEM数据目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strDEMDir=strNewDir;
						break;
					}
				}
			}
		}
		//DOM数据路径
		m_xml->FindChildElem( "DOMDirectory"); m_strDOMDir = m_xml->GetChildData();	
		if(bIsDirectoryExist(m_strDOMDir.GetBuffer(128))==false)
		{//重新设置DOM数据目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_DOM_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strDOMDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strDOMDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置DOM数据目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strDOMDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置DOM数据目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strDOMDir=strNewDir;
						break;
					}
				}
			}
		}
		//控制数据路径
		m_xml->FindChildElem( "ControlDirectory"); m_strCFilesDir = m_xml->GetChildData();
		//日志文件路径
		m_xml->FindChildElem( "LogDirectory"); m_strLOGDir = m_xml->GetChildData();	
		if(bIsDirectoryExist(m_strLOGDir.GetBuffer(128))==false)
		{//重新设置日志文件目录
			CString		strNewDir;
			CString		strTmp;
			strTmp.LoadString(IDS_ALM_LOG_Dir);
			strNewDir=m_strAlmFileDir+"\\"+strTmp;	//工程默认路径
			if(m_strLOGDir.Compare(strNewDir)!=0)
			{
				if(bIsDirectoryExist(strNewDir.GetBuffer(128)))
				{
					m_bIsPrjModified=true;
					m_strLOGDir=strNewDir;
				}
				else
				{
					while(1)
					{
						if(bSelectFolderDialog("请设置日志文件目录...",&strNewDir))
						{
							m_bIsPrjModified=true;
							m_strLOGDir=strNewDir;
							break;
						}
					}
				}
			}
			else
			{
				while(1)
				{
					if(bSelectFolderDialog("请设置日志文件目录...",&strNewDir))
					{
						m_bIsPrjModified=true;
						m_strLOGDir=strNewDir;
						break;
					}
				}
			}
			
		}
		m_xml->OutOfElem();
	}
	AfxSetResourceHandle ( oldRcHandle );
	return true;
}
//读取工程文件相关产品路径信息
bool	CALMPrjManager::bReadAlmFileProductInfo()
{
	while ( m_xml->FindChildElem("ProductInformation"))
	{	
		m_xml->IntoElem();
		
		while ( m_xml->FindChildElem("DOMProduct"))
		{			
			m_xml->IntoElem();
			while (m_xml->FindChildElem("DOM"))
			{
				CString strDOM;
				strDOM=m_xml->GetChildData();
				m_aryDOMProduct.Add(strDOM);
			}			
			m_xml->OutOfElem();
		}
		
		while ( m_xml->FindChildElem("DEMProduct"))
		{
			m_xml->IntoElem();
			while (m_xml->FindChildElem("DEM"))
			{
				CString strDEM;
				strDEM=m_xml->GetChildData();
				m_aryDEMProduct.Add(strDEM);
			}			
			m_xml->OutOfElem();
		}
		while ( m_xml->FindChildElem("ThematicProduct"))
		{
			m_xml->IntoElem();
			while (m_xml->FindChildElem("Thematic"))
			{
				CString strThematic;
				strThematic=m_xml->GetChildData();
				m_aryTMProduct.Add(strThematic);
			}			
			m_xml->OutOfElem();
		}	
		
		m_xml->OutOfElem();
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bReadAlmFile()
//	  Function     :	读取工程文件信息
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	先为m_strAlmFile赋值全路径,再读取该工程文件
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.18
////////////////////////////////////////////////////////////////////////////////
bool	CALMPrjManager::bReadAlmFile(CString strAlmFile)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//检查工程路径是否存在
	if(GetFileAttributes(strAlmFile) == -1) 
	{
		//指定文件不存在
		CString strMsg;
		strMsg.Format("%s文件不存在,请确认工程路径正确性!",(const char*)strAlmFile);
		AfxMessageBox(strMsg);
		return false;
	}
	
	if (!m_xml->Load(strAlmFile)) 
	{
		AfxMessageBox("打开工程文件失败!");
		return false;
	}
	m_strAlmFile=strAlmFile;	
	m_strAlmFileDir=m_strAlmFile.Left(m_strAlmFile.ReverseFind('\\'));
	m_strAlmName=m_strAlmFile.Right(m_strAlmFile.GetLength()-m_strAlmFileDir.GetLength()-1);
	m_strAlmName=m_strAlmName.Left(m_strAlmName.GetLength()-4);
	//读入文件头信息
	if (!bReadAlmFileHeader()) 
	{
		AfxMessageBox("没有文件头信息!");
	}
	//读入影像信息
	if (!bReadAlmFileImgInfo()) 
	{
		AfxMessageBox("没有影像信息!");
	}
	//读入有Lidar信息
	if (!bReadAlmFileLidarInfo()) 
	{
		AfxMessageBox("没有Lidar信息!");
	}
	//读入相机参数
	if (!bReadAlmFileCameraParas()) 
	{
		AfxMessageBox("没有相机参数信息!");
	}	
	//读入影像匹配参数
	if (!bReadAlmFileImgMachParas()) 
	{
		AfxMessageBox("没有影像匹配参数!");
	}
	//读入文件路径信息
	if (!bReadAlmFileDirInfo()) 
	{
		AfxMessageBox("没有文件路径信息!");
	}
	//读入相关数据
	//POS
	CFileFind fFind;
	if (fFind.FindFile(m_strPOSFile))
	{
		if (!bReadPOSData())
		{
			AfxMessageBox("读入POS数据失败!");
		}
	}
// 	else
// 	{
// 		AfxMessageBox("读入POS数据失败!");
// 	}

	//读入产品信息
	if (!bReadAlmFileProductInfo()) 
	{
		AfxMessageBox("没有文件路径信息!");
	}

	return TRUE;
}
//写入工程文件头信息，,m_strAlmFile赋值全路径
bool	CALMPrjManager::bWriteAlmFileHeader()
{
	if (m_aryStrMetaInf.GetSize()<1)
	{
		return false;
	}
	
	//XML文件头
	m_xml->SetDoc("<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n");
	m_xml->AddElem( "ALMPSProjectFile" );
	m_xml->AddChildElem( "MetaInformation" );
	m_xml->IntoElem();
	for (int i=0;i<m_aryStrMetaInf.GetSize();i++)
	{
		CString strInfo;
		strInfo=m_aryStrMetaInf[i];
		m_xml->AddChildElem( "Info", strInfo);
	}
	m_xml->OutOfElem();
	return true;
}
//写入工程文件影像信息
bool	CALMPrjManager::bWriteAlmFileImgInfo()
{

	if (GetImageCount()<1)
	{
		return false;
	}

	m_xml->AddChildElem( "ImageInformation" );
	m_xml->IntoElem();
	CString strInfo;
	//影像总数
	strInfo.Format("%d",m_nImgNum);
	m_xml->AddChildElem( "ImageCount", strInfo);
	//影像路径
	m_xml->AddChildElem( "ImageDirectory", m_strRawImgDir);

	//影像总线数
	strInfo.Format("%d",m_nImgStripNum);
	m_xml->AddChildElem( "ImageStripCount", strInfo);

	//影像列表
	m_xml->AddChildElem( "ImageList" );
	m_xml->IntoElem();	
	for (int i=0;i<m_nImgNum;i++)
	{
		iphImgINFO imglist;
		imglist=m_aryImgList[i];
		strInfo.Format("%d",imglist.nImgID);
		m_xml->AddChildElem( "ImageID", strInfo);
		m_xml->AddChildElem( "ImageName", imglist.strImgName);
		strInfo.Format("%d",imglist.nStripID);
		m_xml->AddChildElem( "StripID", strInfo);
		strInfo.Format("%d",imglist.nPhoID);
		m_xml->AddChildElem( "PhotoID", strInfo);
		strInfo.Format("%d",imglist.nIsReversal);
		m_xml->AddChildElem( "RotateFlag", strInfo);
// 		strInfo.Format("%c", imglist.useful);
// 		m_xml->AddChildElem( "UseFlag", strInfo);

//		strInfo=imglist.camera.m_pCameraFileName;
//		m_xml->AddChildElem( "CameraFileName", strInfo);
		
		CString str=imglist.strImgName;
		int pos;
		pos=str.ReverseFind('.');
		str=str.Left(pos);
		str+=".cam";
		if(imglist.camera.m_bInor || imglist.camera.m_bExor)
			imglist.camera.WriteCameraFile(str.GetBuffer(256));
	}
	m_xml->OutOfElem();

	m_xml->OutOfElem();
	return true;
}
//写入工程文件Lidar信息
bool	CALMPrjManager::bWriteAlmFileLidarInfo()
{
	if (GetLidarStripNum()<1)
	{
		return false;
	}

	m_xml->AddChildElem( "LidarInformation" );
	m_xml->IntoElem();
	//Lidar路径
	m_xml->AddChildElem( "LidarDirectory", m_strRawLidarDir);

	CString strInfo;
	//Lidar总线数
	strInfo.Format("%d",m_nLidarStripNum);
	m_xml->AddChildElem( "LidarStripCount", strInfo);

	//Lidar列表
	m_xml->AddChildElem( "LidarList" );
	m_xml->IntoElem();	
	for (int i=0;i<m_nLidarStripNum;i++)
	{
		LIDARINFO Lidarlist;
		Lidarlist=m_aryLidarList[i];
		m_xml->AddChildElem( "LidarName", Lidarlist.strLidarName);
		strInfo.Format("%d",Lidarlist.nStripID);
		m_xml->AddChildElem( "StripID", strInfo);
		strInfo.Format("%.6lf",Lidarlist.dfBox3D.XMin);
		m_xml->AddChildElem( "MinX", strInfo);
		strInfo.Format("%.6lf",Lidarlist.dfBox3D.YMin);
		m_xml->AddChildElem( "MinY", strInfo);
		strInfo.Format("%.6lf",Lidarlist.dfBox3D.ZMin);
		m_xml->AddChildElem( "MinZ", strInfo);
		strInfo.Format("%.6lf",Lidarlist.dfBox3D.XMax);
		m_xml->AddChildElem( "MaxX", strInfo);
		strInfo.Format("%.6lf",Lidarlist.dfBox3D.YMax);
		m_xml->AddChildElem( "MaxY", strInfo);
		strInfo.Format("%.6lf",Lidarlist.dfBox3D.ZMax);
		m_xml->AddChildElem( "MaxZ", strInfo);
	}
	m_xml->OutOfElem();

	m_xml->OutOfElem();
	return true;
}
//写入工程文件相机参数
bool	CALMPrjManager::bWriteAlmFileCameraParas()
{
	m_xml->AddChildElem("CameraInformation");
	m_xml->IntoElem();

	//相机参数
	CString strInfo;
	strInfo.Format("%.6lf",m_Camera.dfU0);
	m_xml->AddChildElem( "X0", strInfo);
	strInfo.Format("%.6lf",m_Camera.dfV0);
	m_xml->AddChildElem( "Y0", strInfo);
	strInfo.Format("%.6lf",m_Camera.dfF);
	m_xml->AddChildElem( "F", strInfo);
	strInfo.Format("%.6lf",m_Camera.dfPixSize);
	m_xml->AddChildElem( "PixelSize", strInfo);
	strInfo.Format("%d",m_Camera.nWidth);
	m_xml->AddChildElem( "FrameWidth", strInfo);
	strInfo.Format("%d",m_Camera.nHeight);
	m_xml->AddChildElem( "FrameHeight", strInfo);

	//畸变类型
	strInfo.Format("%d",m_Camera.nDistortionNum);
	m_xml->AddChildElem( "DistortionType", strInfo);
	//畸变参数	
	if (m_Camera.nDistortionNum==4)
	{
		strInfo.Format("%e",m_Camera.dfK1);
		m_xml->AddChildElem( "K1", strInfo);
		strInfo.Format("%e",m_Camera.dfK2);
		m_xml->AddChildElem( "K2", strInfo);
		strInfo.Format("%e",m_Camera.dfP1);
		m_xml->AddChildElem( "P1", strInfo);
		strInfo.Format("%e",m_Camera.dfP2);
		m_xml->AddChildElem( "P2", strInfo);
		
	}
	else  if (m_Camera.nDistortionNum==3)         //[Count_A0_A1_R]
	{
		strInfo.Format("%e",m_Camera.dfA0);
		m_xml->AddChildElem( "A0", strInfo);
		strInfo.Format("%e",m_Camera.dfA1);
		m_xml->AddChildElem( "A1", strInfo);
		strInfo.Format("%e",m_Camera.dfR);
		m_xml->AddChildElem( "R", strInfo);
	}
	else
	{
		return false;
	}
	m_xml->OutOfElem();
	return true;
}
//写入工程文件影像匹配参数
bool	CALMPrjManager::bWriteAlmFileImgMachParas()
{
	m_xml->AddChildElem("ImgMachInformation");
	m_xml->IntoElem();

	//空三格网数
	CString strInfo;
	strInfo.Format("%d",m_nATGridNum);
	m_xml->AddChildElem( "ATGridNum", strInfo);

	//DSM格网
	strInfo.Format("%d",m_nDSMGridSize);
	m_xml->AddChildElem( "DSMGridSize", strInfo);

	//地面平均高程
	strInfo.Format("%.6lf",m_dAverGrdElev);
	m_xml->AddChildElem( "AverGroundElev", strInfo);
	//最大高程
	strInfo.Format("%.6lf",m_dMaxGrdElev);
	m_xml->AddChildElem( "MaxGroundElev", strInfo);
	//最小高程
	strInfo.Format("%.6lf",m_dMinGrdElev);
	m_xml->AddChildElem( "MinGroundElev", strInfo);

	strInfo.Format("%d",m_flyOverlap);
	m_xml->AddChildElem( "FlyOverlap", strInfo);

	strInfo.Format("%d",m_stripOverlap);
	m_xml->AddChildElem( "StripOverlap", strInfo);

	m_xml->OutOfElem();
	return true;
}
//写入工程文件相关文件路径信息
bool	CALMPrjManager::bWriteAlmFileDirInfo()
{
	m_xml->AddChildElem("FileFolderInformation");
	m_xml->IntoElem();
	 
	//POS数据全路径
	m_xml->AddChildElem( "POSDataFile", m_strPOSFile);
	//外方位元素数据全路径
	m_xml->AddChildElem( "ExteriorOrientationElementsFile", m_strEOEFile);
	//控制点数据全路径
	m_xml->AddChildElem( "ControlPointFile", m_strCPtFile);
	//Lidar色表文件全路径
	m_xml->AddChildElem( "ColorLutFile", m_strColorLutFile);
	//缩略图路径
	m_xml->AddChildElem( "ImageQuickViewDirectory", m_strImgQVDir);
	//匀色后影像路径
	m_xml->AddChildElem( "DodgingImageDirectory", m_strDgImageDir);
	//拼图影像路径
	m_xml->AddChildElem( "MosaicDirectory", m_strMosaicDir);
	//Lidar与影像配准数据路径
	m_xml->AddChildElem( "RegistDirectory", m_strRegistDir);
	//滤波后Lidar数据路径
	m_xml->AddChildElem( "FilterLidarDirectory", m_strFltLidarDir);
	//匹配后Lidar数据路径
	m_xml->AddChildElem( "MatchLidarDirectory", m_strMatchLidarDir);
	//DSM数据路径
	m_xml->AddChildElem( "DSMDirectory", m_strDSMDir);	
	//DEM数据路径
	m_xml->AddChildElem( "DEMDirectory", m_strDEMDir);	
	//DOM数据路径
	m_xml->AddChildElem( "DOMDirectory", m_strDOMDir);	
	//控制数据路径
	m_xml->AddChildElem( "ControlDirectory", m_strCFilesDir);
	//日志文件路径
	m_xml->AddChildElem( "LogDirectory", m_strLOGDir);

	m_xml->OutOfElem();
	return true;
}
//写入工程文件相关产品路径信息
bool	CALMPrjManager::bWriteAlmFileProductInfo()
{
	m_xml->AddChildElem("ProductInformation");
	m_xml->IntoElem();
	 
	m_xml->AddChildElem("DOMProduct");
	m_xml->IntoElem();
	CString strFilePath;
	int i;
	for (i=0;i<m_aryDOMProduct.GetSize();i++)
	{
		strFilePath=m_aryDOMProduct[i];
		m_xml->AddChildElem( "DOM", strFilePath);
	}
	m_xml->OutOfElem();

	m_xml->AddChildElem("DEMProduct");
	m_xml->IntoElem();
	for (i=0;i<m_aryDEMProduct.GetSize();i++)
	{
		strFilePath=m_aryDEMProduct[i];
		m_xml->AddChildElem( "DEM", strFilePath);
	}
	m_xml->OutOfElem();

	m_xml->AddChildElem("ThematicProduct");
	m_xml->IntoElem();
	for (i=0;i<m_aryTMProduct.GetSize();i++)
	{
		strFilePath=m_aryTMProduct[i];
		m_xml->AddChildElem( "Thematic", strFilePath);
	}
	m_xml->OutOfElem();

	m_xml->OutOfElem();
	return true;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bWriteAlmFile()
//	  Function     :	写工程信息到m_strAlmFile文件中
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.19
////////////////////////////////////////////////////////////////////////////////
bool	CALMPrjManager::bWriteAlmFile(CString strAlmFile)
{
	//写入文件头信息
	if (!bWriteAlmFileHeader()) 
	{
		AfxMessageBox("没有文件头信息!");
	}
	//写入影像信息
	if (!bWriteAlmFileImgInfo()) 
	{
		AfxMessageBox("没有影像信息!");
	}
	//写入有Lidar信息
	if (!bWriteAlmFileLidarInfo()) 
	{
		AfxMessageBox("没有Lidar信息!");
	}
	//写入相机参数
	if (!bWriteAlmFileCameraParas()) 
	{
		AfxMessageBox("没有相机参数信息!");
	}	
	//写入影像匹配参数
	if (!bWriteAlmFileImgMachParas()) 
	{
		AfxMessageBox("没有影像匹配参数!");
	}
	//写入文件路径信息
	if (!bWriteAlmFileDirInfo()) 
	{
		AfxMessageBox("没有文件路径信息!");
	}
	//写入产品信息
	if (!bWriteAlmFileProductInfo()) 
	{
		AfxMessageBox("没有文件路径信息!");
	}
	m_xml->Save(strAlmFile);

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::SetImgList()
//	  Function     :	设置影像列表
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL CALMPrjManager::SetImgList(const iphImgList &aryImgList)
{
	if (aryImgList.GetSize()==0)
	{
		AfxMessageBox("没有影像!");
		return FALSE;
	}
	m_aryImgList.RemoveAll();
	m_aryImgList.Copy(aryImgList);
	m_bIsPrjModified=true;
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::SetLidarList()
//	  Function     :	设置Lidar列表
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL CALMPrjManager::SetLidarList(const CArray<LIDARINFO,LIDARINFO&> &aryLidarList)
{
	if (aryLidarList.GetSize()==0)
	{
		AfxMessageBox("没有影像!");
		return FALSE;
	}
	m_aryLidarList.RemoveAll();
	m_aryLidarList.Copy(aryLidarList);
	m_bIsPrjModified=true;
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::SetEoeData()
//	  Function     :	设置EOE data
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL CALMPrjManager::SetEoeData(const CArray<OUTPARA,OUTPARA&> &aryEoeData)
{
	if (aryEoeData.GetSize()==0)
	{
		AfxMessageBox("没有影像!");
		return FALSE;
	}
	m_aryEOEData.RemoveAll();
	m_aryEOEData.Copy(aryEoeData);
	m_bIsPrjModified=true;
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::SetAlmFile(CString strAlmFile)
//	  Function     :	设置工程文件
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
void CALMPrjManager::SetAlmFile(CString strAlmFile)
{
	m_strAlmFile=strAlmFile;	
	m_strAlmFileDir=m_strAlmFile.Left(m_strAlmFile.ReverseFind('\\'));
	m_strAlmName=m_strAlmFile.Right(m_strAlmFile.GetLength()-m_strAlmFileDir.GetLength()-1);
	m_strAlmName=m_strAlmName.Left(m_strAlmName.GetLength()-4);
}  
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::SetCameraPara(CAMPARA campara)
//	  Function     :	设置相机参数
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
void CALMPrjManager::SetCameraPara(CAMPARA campara)
{
	m_Camera.dfA0=campara.dfA0;
	m_Camera.dfA1=campara.dfA1;
	m_Camera.dfF=campara.dfF;
	m_Camera.dfHeight=campara.dfHeight;
	m_Camera.dfK1=campara.dfK1;
	m_Camera.dfK2=campara.dfK2;
	m_Camera.dfP1=campara.dfP1;
	m_Camera.dfP2=campara.dfP2;
	m_Camera.dfPixSize=campara.dfPixSize;
	m_Camera.dfR=campara.dfR;
	m_Camera.dfU0=campara.dfU0;
	m_Camera.dfV0=campara.dfV0;
	m_Camera.dfWidth=campara.dfWidth;
	m_Camera.nDistortionNum=campara.nDistortionNum;
	m_Camera.nHeight=campara.nHeight;
	m_Camera.nWidth=campara.nWidth;
	m_Camera.nAttrib=campara.nAttrib;

	m_bIsPrjModified=true;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bReadPOSData()
//	  Function     :	读POS数据
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL	CALMPrjManager::bReadPOSData()
{
	if (m_strPOSFile=="NULL")
	{
		AfxMessageBox("POS文件为NULL!");
//		return FALSE;
	}

	//Open file to read
	FILE *pFile=fopen(m_strPOSFile,"r");
	CString strInfo;
	if (pFile==NULL)
	{
		strInfo.Format("不能打开"+m_strPOSFile+"!");
		AfxMessageBox(strInfo);
//		return FALSE;
	}
	
	//operate
	char chInfo[PATHLEN];
	fgets(chInfo,PATHLEN,pFile);
	strInfo=CString(chInfo);
	strInfo.TrimLeft();
	strInfo.TrimRight();
	if (strInfo!="##GPSIMU data")
	{
		AfxMessageBox("不是POS文件!");
		return FALSE;
	}
	fgets(chInfo,PATHLEN,pFile);
	fgets(chInfo,PATHLEN,pFile);
	int nNum;
	fscanf(pFile,"%d\n",&nNum);
	m_aryPOSData.RemoveAll();
	for (int i = 0; i < nNum ; i++)
	{
		OUTPARA outpara;
		fscanf(pFile,"%s %lf %lf %lf %lf %lf %lf %lf\n",outpara.chImgName,
			&outpara.dfXs,&outpara.dfYs,&outpara.dfZs,&outpara.dfPhi,
			&outpara.dfOmg,&outpara.dfKapa,&outpara.dfGPSTime);
		m_aryPOSData.Add(outpara);
	}
	
	//close files
	fclose(pFile);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bWritePOSData()
//	  Function     :	写POS数据
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL	CALMPrjManager::bWritePOSData()
{
	if (m_strPOSFile=="NULL")
	{
		AfxMessageBox("POS文件路径为NULL!");
		return FALSE;
	}

	//Open file to read
	FILE *pFile=fopen(m_strPOSFile,"w");
	CString strInfo;
	if (pFile==NULL)
	{
		strInfo.Format("不能打开"+m_strPOSFile+"!");
		AfxMessageBox(strInfo);
		return FALSE;
	}
	
	//operate
	fprintf(pFile,"##GPSIMU data\n");
	fprintf(pFile,"##PhotoCount\n");
	fprintf(pFile,"##PhotoName Easting(metres) Northing(metres) H(metres) Phi(radian) Omega(radian) Kap(radian) GPSTime(s)\n");
	int nNum;
	nNum=m_aryPOSData.GetSize();
	fprintf(pFile,"%d\n",nNum);
	for (int i = 0; i < nNum ; i++)
	{
		OUTPARA outpara;
		outpara=m_aryPOSData[i];
		fprintf(pFile,"%20s %15.6lf %15.6lf %15.6lf %15.8lf %15.8lf %15.8lf %15.8lf\n",
			outpara.chImgName,outpara.dfXs,outpara.dfYs,outpara.dfZs,
			outpara.dfPhi,outpara.dfOmg,outpara.dfKapa,outpara.dfGPSTime);
	}
	
	//close files
	fclose(pFile);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bReadEOEData()
//	  Function     :	读外方位元素数据
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL	CALMPrjManager::bReadEOEData()
{
	if (m_strEOEFile=="NULL")
	{
		AfxMessageBox("EOE文件为NULL!");
		return FALSE;
	}

	//Open file to read
	FILE *pFile=fopen(m_strEOEFile,"r");
	CString strInfo;
	if (pFile==NULL)
	{
		strInfo.Format("不能打开"+m_strEOEFile+"!");
		AfxMessageBox(strInfo);
		return FALSE;
	}
	
	//operate
	char chInfo[PATHLEN];
	fgets(chInfo,PATHLEN,pFile);
	strInfo=CString(chInfo);
	strInfo.TrimLeft();
	strInfo.TrimRight();
	if (strInfo!="##Exterior Orientation Elements")
	{
		AfxMessageBox("不是空三结果文件!");
		return FALSE;
	}
	fgets(chInfo,PATHLEN,pFile);
	fgets(chInfo,PATHLEN,pFile);
	int nNum;
	fscanf(pFile,"%d\n",&nNum);
	m_aryEOEData.RemoveAll();
	for (int i = 0; i < nNum ; i++)
	{
		OUTPARA outpara;
		fscanf(pFile,"%s %lf %lf %lf %lf %lf %lf %d\n",outpara.chImgName,
			&outpara.dfXs,&outpara.dfYs,&outpara.dfZs,&outpara.dfPhi,
			&outpara.dfOmg,&outpara.dfKapa,&outpara.nFlag);
		m_aryEOEData.Add(outpara);
	}
	
	//close files
	fclose(pFile);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bWriteEOEData()
//	  Function     :	写外方位元素数据
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL	CALMPrjManager::bWriteEOEData()
{
	if (m_strEOEFile=="NULL")
	{
		AfxMessageBox("空三结果文件路径为NULL!");
		return FALSE;
	}

	//Open file to read
	FILE *pFile=fopen(m_strEOEFile,"w");
	CString strInfo;
	if (pFile==NULL)
	{
		strInfo.Format("不能打开"+m_strEOEFile+"!");
		AfxMessageBox(strInfo);
		return FALSE;
	}
	
	//operate
	fprintf(pFile,"##Exterior Orientation Elements\n");
	fprintf(pFile,"##PhotoCount\n");
	fprintf(pFile,"##PhotoName Easting(metres) Northing(metres) H(metres) Phi(radian) Omega(radian) Kap(radian) Flag\n");
	int nNum;
	nNum=m_aryEOEData.GetSize();
	fprintf(pFile,"%d\n",nNum);
	for (int i = 0; i < nNum ; i++)
	{
		OUTPARA outpara;
		outpara=m_aryEOEData[i];
		fprintf(pFile,"%20s %15.6lf %15.6lf %15.6lf %15.8lf %15.8lf %15.8lf %4d\n",
			outpara.chImgName,outpara.dfXs,outpara.dfYs,outpara.dfZs,
			outpara.dfPhi,outpara.dfOmg,outpara.dfKapa,outpara.nFlag);
	}
	
	//close files
	fclose(pFile);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bReadCPTData()
//	  Function     :	读控制点数据
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL	CALMPrjManager::bReadCPTData()
{
	if (m_strCPtFile=="NULL")
	{
		AfxMessageBox("控制点文件路径为NULL!");
		return FALSE;
	}

	//Open file to read
	FILE *pFile=fopen(m_strCPtFile,"r");
	CString strInfo;
	if (pFile==NULL)
	{
		strInfo.Format("不能打开"+m_strCPtFile+"!");
		AfxMessageBox(strInfo);
		return FALSE;
	}
	
	//operate
	char chInfo[PATHLEN];
	fgets(chInfo,PATHLEN,pFile);
	strInfo=CString(chInfo);
	strInfo.TrimLeft();
	strInfo.TrimRight();
	if (strInfo!="##Coordinates of Ground Control Points")
	{
		AfxMessageBox("不是控制点文件!");
		return FALSE;
	}
	fgets(chInfo,PATHLEN,pFile);
	fgets(chInfo,PATHLEN,pFile);
	int nNum;
	fscanf(pFile,"%d\n",&nNum);
	m_aryGCPData.RemoveAll();
	for (int i = 0; i < nNum ; i++)
	{
		CONTROLPOINT cpt;
		fscanf(pFile,"%d %lf %lf %lf %d %d\n",&cpt.nName,
			&cpt.dfX,&cpt.dfY,&cpt.dfZ,&cpt.nAttrib,&cpt.nGroup);
		m_aryGCPData.Add(cpt);
	}
	
	//close files
	fclose(pFile);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bWriteCPTData()
//	  Function     :	写控制点数据
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.20
////////////////////////////////////////////////////////////////////////////////
BOOL	CALMPrjManager::bWriteCPTData()
{
	if (m_strCPtFile=="NULL")
	{
		AfxMessageBox("控制点文件路径为NULL!");
		return FALSE;
	}

	//Open file to read
	FILE *pFile=fopen(m_strCPtFile,"w");
	CString strInfo;
	if (pFile==NULL)
	{
		strInfo.Format("不能打开"+m_strCPtFile+"!");
		AfxMessageBox(strInfo);
		return FALSE;
	}
	
	//operate
	fprintf(pFile,"##Coordinates of Ground Control Points\n");
	fprintf(pFile,"##PointCount\n");
	fprintf(pFile,"##PointName        X            Y           Z     Attrib  GroupID\n");
	int nNum;
	nNum=m_aryGCPData.GetSize();
	fprintf(pFile,"%d\n",nNum);
	for (int i = 0; i < nNum ; i++)
	{
		CONTROLPOINT cpt;
		cpt=m_aryGCPData[i];
		fprintf(pFile,"%10d %15.6lf %15.6lf %15.6lf %4d %4d\n",
			cpt.nName,cpt.dfX,cpt.dfY,cpt.dfZ,cpt.nAttrib,cpt.nGroup);
	}
	
	//close files
	fclose(pFile);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bReadCameraPara()
//	  Function     :	从文件读取相机参数
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.22
////////////////////////////////////////////////////////////////////////////////
BOOL CALMPrjManager::bReadCameraPara(CString CameraFile)
{
	//Open file to read
	FILE *pFile=fopen(CameraFile,"r");
	if (pFile==NULL)
	{
		AfxMessageBox("不能打开"+CameraFile+"!");
		return FALSE;
	}
	
	//operate
	char chInfo[PATHLEN];
	CString strInfo;
	fgets(chInfo,PATHLEN,pFile);
	strInfo=CString(chInfo);
	strInfo.TrimLeft();
	strInfo.TrimRight();
	if (strInfo!="##Interior Orientation Parameters of Cameras")
	{
		AfxMessageBox("不是相机参数文件!");
		return FALSE;
	}
	fgets(chInfo,PATHLEN,pFile);
	fgets(chInfo,PATHLEN,pFile);
	int nNum;
	fscanf(pFile,"%d\n",&nNum);
	
	for (int i = 0; i < nNum ; i++)
	{
		fscanf(pFile,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d\n",&m_Camera.dfU0,
			&m_Camera.dfV0,&m_Camera.dfF,&m_Camera.dfWidth,&m_Camera.dfHeight,
			&m_Camera.dfPixSize,&m_Camera.dfK1,&m_Camera.dfK2,
			&m_Camera.dfP1,&m_Camera.dfP2,&m_Camera.nAttrib);
		m_Camera.nDistortionNum=4;
		m_Camera.nWidth=int(m_Camera.dfWidth/m_Camera.dfPixSize);
		m_Camera.nHeight=int(m_Camera.dfHeight/m_Camera.dfPixSize);
	}
	
	//close files
	fclose(pFile);
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void CALMPrjManager::bWriteCameraPara()
//	  Function     :	写相机参数到相机参数文件
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	目前为止仅支持单个相机
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2008.12.22
////////////////////////////////////////////////////////////////////////////////
BOOL CALMPrjManager::bWriteCameraPara(CString CameraFile)
{
	//Open file to read
	FILE *pFile=fopen(CameraFile,"w");
	if (pFile==NULL)
	{
		AfxMessageBox("不能打开"+CameraFile+"!");
		return FALSE;
	}
	if (m_Camera.nDistortionNum!=4)
	{
		AfxMessageBox("畸变不是4参数模型,暂不支持!");
		return FALSE;
	}
	
	//operate
	fprintf(pFile,"##Interior Orientation Parameters of Cameras\n");
	fprintf(pFile,"##CamaraCount\n");
	fprintf(pFile,"##    x0          y0           f          format_X     format_Y      pixelSize      k1            k2            p1            p2     Attrib\n");
	int nNum;
	nNum=1;
	fprintf(pFile,"%d\n",nNum);
	for (int i = 0; i < nNum ; i++)
	{
		if (m_Camera.nAttrib!=1111)
		{
			m_Camera.nAttrib=1111;
		}
		double dfWidth;
		dfWidth=m_Camera.nWidth*m_Camera.dfPixSize;
		if (fabs(dfWidth-m_Camera.dfWidth)>0.000001)
		{
			AfxMessageBox("像幅宽度(像素)乘以象元大小与像幅宽度(毫米)不一致!");
			return FALSE;
		}
		double dfHeight;
		dfHeight=m_Camera.nHeight*m_Camera.dfPixSize;
		if (fabs(dfHeight-m_Camera.dfHeight)>0.000001)
		{
			AfxMessageBox("像幅高度(像素)乘以象元大小与像幅高度(毫米)不一致!");
			return FALSE;
		}
		int nWidth;
		nWidth=int(m_Camera.dfWidth/m_Camera.dfPixSize);
		if (nWidth!=m_Camera.nWidth)
		{
			AfxMessageBox("像幅宽度(毫米)除以象元大小与像幅宽度(像素)不一致!");
			return FALSE;
		}
		int nHeight;
		nHeight=int(m_Camera.dfHeight/m_Camera.dfPixSize);
		if (nHeight!=m_Camera.nHeight)
		{
			AfxMessageBox("像幅高度(毫米)除以象元大小与像幅高度(像素)不一致!");
			return FALSE;
		}
		fprintf(pFile,"%12.6lf %12.6lf %12.6lf %12.6lf %12.6lf %12.6lf %e %e %e %e %5d\n",
			m_Camera.dfU0,m_Camera.dfV0,m_Camera.dfF,m_Camera.dfWidth,
			m_Camera.dfHeight,m_Camera.dfPixSize,m_Camera.dfK1,
			m_Camera.dfK2,m_Camera.dfP1,m_Camera.dfP2,m_Camera.nAttrib);
	}
	
	//close files
	fclose(pFile);
	return TRUE;
}

bool CALMPrjManager::GetSwathBoundingBox(char *pLidarFile, BoundingBox3D *pBox)
{
 	FILE* fp;
	fp=fopen(pLidarFile,"rb");
	if(fp==0)
		return false;
	fseek(fp,179,SEEK_SET);
	if(fread(&pBox->XMax,sizeof(double),1,fp)!=1)
		return false;
	if(fread(&pBox->XMin,sizeof(double),1,fp)!=1)
		return false;
	if(fread(&pBox->YMax,sizeof(double),1,fp)!=1)
		return false;
	if(fread(&pBox->YMin,sizeof(double),1,fp)!=1)
		return false;
	if(fread(&pBox->ZMax,sizeof(double),1,fp)!=1)
		return false;
	if(fread(&pBox->ZMin,sizeof(double),1,fp)!=1)
		return false;
 	fclose(fp);
	return	true;
}

bool CALMPrjManager::IsPrjModified()
{
	return	m_bIsPrjModified;
}

bool CALMPrjManager::CreateALMPrj(char *pAlmFilePath)
{
	HINSTANCE oldRcHandle = AfxGetResourceHandle();
#ifdef _DEBUG
	AfxSetResourceHandle ( GetModuleHandle("ALMPrjManagerD.dll") );
#else
	AfxSetResourceHandle ( GetModuleHandle("ALMPrjManager.dll") );
#endif

	bool bReturn=true;
	CString	str;

	CloseALMPrjFile();
	
	SetAlmFile(pAlmFilePath);
	//创建目录结构
	str.LoadString(IDS_ALM_RawImg_Dir);
	m_strRawImgDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_RawLidar_Dir);
	m_strRawLidarDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_QV_Dir);
	m_strImgQVDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_DodgImg_Dir);
	m_strDgImageDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_Mosaic_Dir);
	m_strMosaicDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_Regist_Dir);
	m_strRegistDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_FilterLidar_Dir);
	m_strFltLidarDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_MatchLidar_Dir);
	m_strMatchLidarDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_DSM_Dir);
	m_strDSMDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_DEM_Dir);
	m_strDEMDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_DOM_Dir);
	m_strDOMDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_ControlFiles_Dir);
	m_strCFilesDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_LOG_Dir);
	m_strLOGDir=m_strAlmFileDir+_T("\\")+str;

	str.LoadString(IDS_ALM_Product_Dir);
	m_strProductDir=m_strAlmFileDir+_T("\\")+str;

	//RawImages
	if(_IsDirectoryExist(m_strRawImgDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strRawImgDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strRawImgDir);
			AfxMessageBox(strInfo);
		}
	}

	//RawLIDAR
	if(_IsDirectoryExist(m_strRawLidarDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strRawLidarDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strRawLidarDir);
			AfxMessageBox(strInfo);
		}
	}

	//QV
	if(_IsDirectoryExist(m_strImgQVDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strImgQVDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strImgQVDir);
			AfxMessageBox(strInfo);
		}
	}

	//DodgingImage
	if(_IsDirectoryExist(m_strDgImageDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strDgImageDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strDgImageDir);
			AfxMessageBox(strInfo);
		}
	}

	//Mosaic
	if(_IsDirectoryExist(m_strMosaicDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strMosaicDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strMosaicDir);
			AfxMessageBox(strInfo);
		}
	}

	//Regist
	if(_IsDirectoryExist(m_strRegistDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strRegistDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strRegistDir);
			AfxMessageBox(strInfo);
		}
	}

	//FilterLas
	if(_IsDirectoryExist(m_strFltLidarDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strFltLidarDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strFltLidarDir);
			AfxMessageBox(strInfo);
		}
	}

	//MatchLas
	if(_IsDirectoryExist(m_strMatchLidarDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strMatchLidarDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strMatchLidarDir);
			AfxMessageBox(strInfo);
		}
	}

	//DSM
	if(_IsDirectoryExist(m_strDSMDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strDSMDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strDSMDir);
			AfxMessageBox(strInfo);
		}
	}

	//DEM
	if(_IsDirectoryExist(m_strDEMDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strDEMDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strDEMDir);
			AfxMessageBox(strInfo);
		}
	}

	//DOM
	if(_IsDirectoryExist(m_strDOMDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strDOMDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strDOMDir);
			AfxMessageBox(strInfo);
		}
	}

	//ControlFiles
	if(_IsDirectoryExist(m_strCFilesDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strCFilesDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strCFilesDir);
			AfxMessageBox(strInfo);
		}
	}

	//LOG
	if(_IsDirectoryExist(m_strProductDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strProductDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strProductDir);
			AfxMessageBox(strInfo);
		}
	}

	//Product
	if(_IsDirectoryExist(m_strLOGDir.GetBuffer(128))==false)
	{
		if(!CreateDirectory(m_strLOGDir,NULL))
		{
			CString strInfo;
			strInfo.Format("不能创建工程文件夹%s",m_strLOGDir);
			AfxMessageBox(strInfo);
		}
	}

	CArray<CString,CString&> aryStrMetInfo;
//	CString	str;
	str=_T("##Integrated Photogrammetry and LiDAR Register");
	aryStrMetInfo.Add(str);
	str=_T("##V1.0");
	aryStrMetInfo.Add(str);
	str=_T("......");
	aryStrMetInfo.Add(str);

	SetMetaInfo(aryStrMetInfo);

	m_bIsPrjModified=true;
	AfxSetResourceHandle ( oldRcHandle );
	return	bReturn;
}

bool CALMPrjManager::OpenALMPrjFile(CString strAlmFile)
{
	bool	bReturn=true;

	if(strAlmFile.IsEmpty())
	{
		AfxMessageBox("文件路径错误!", MB_OK|MB_ICONSTOP);
		bReturn=false;
		goto	FUNC_END;
	}

	if(m_strAlmFile.Compare(strAlmFile)==0)
	{//打开同一个文件
		AfxMessageBox("工程文件已打开!", MB_OK|MB_ICONQUESTION);
		goto	FUNC_END;
	}

	bReturn=CloseALMPrjFile();
	if(bReturn==false)
		goto	FUNC_END;

	m_strAlmFile=strAlmFile;
	bReturn=bReadAlmFile(strAlmFile);

FUNC_END:
	return	bReturn;
}

bool CALMPrjManager::CloseALMPrjFile()
{
	bool	bReturn=true;
	
	if(m_bIsPrjModified)
	{
		if(AfxMessageBox("工程已改变，是否保存?",MB_YESNO|MB_ICONQUESTION)==IDYES)
		{
			bReturn=SaveALMPrjFile();
			if(bReturn==false)
			{
				AfxMessageBox("保存工程文件失败!",MB_OK);
				goto	FUNC_END;
			}
		}
	}
	
	InitALMPrjManager();

FUNC_END:
	return	bReturn;
}

bool CALMPrjManager::SaveALMPrjFile()
{
	bool	bReturn=true;
	
	bReturn=bWriteAlmFile(m_strAlmFile);
	m_bIsPrjModified=false;

	return	bReturn;
}

bool CALMPrjManager::SaveALMPrjAs(CString strAlmFile)
{
	bool	bReturn=true;
	
	bReturn=bWriteAlmFile(strAlmFile);
	if(bReturn==false)
	{
		AfxMessageBox("保存工程文件失败!", MB_OK);
		goto	FUNC_END;
	}

	m_strAlmFile=strAlmFile;
	m_bIsPrjModified=false;

FUNC_END:
	return	bReturn;
}

BOOL CALMPrjManager::SetMetaInfo(const CArray<CString,CString&> &aryStrMetInfo)
{
	if (aryStrMetInfo.GetSize()==0)
	{
		AfxMessageBox("没有文件头信息!");
		return FALSE;
	}
	m_aryStrMetaInf.RemoveAll();
	for (int i=0;i<aryStrMetInfo.GetSize();i++)
	{
		CString strInfo;
		strInfo=aryStrMetInfo[i];
		m_aryStrMetaInf.Add(strInfo);
	}
	m_bIsPrjModified=true;
	return TRUE;
}

void  CALMPrjManager::SetImageCount(int nImgNum)
{
	if(m_nImgNum!=nImgNum)
	{
		m_nImgNum=nImgNum;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetRawImageDir(CString strRImgDir)
{
	if(m_strRawImgDir.Compare(strRImgDir)!=0)
	{
		m_strRawImgDir=strRImgDir;
		m_bIsPrjModified=true;
	}
}
void CALMPrjManager::SetRawLidarDir(CString strRLidarDir)
{
	if(m_strRawLidarDir.Compare(strRLidarDir)!=0)
	{
		m_strRawLidarDir=strRLidarDir;
		m_bIsPrjModified=true;
	}
}
void CALMPrjManager::SetImgStripNum(int nImgStripNum)
{
	if(m_nImgStripNum!=nImgStripNum)
	{
		m_nImgStripNum=nImgStripNum;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetLidarStripNum(int nLidarStripNum)
{
	if(m_nLidarStripNum!=nLidarStripNum)
	{
		m_nLidarStripNum=nLidarStripNum;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetATGridNum(int nATGridNum)
{
	if(m_nATGridNum!=nATGridNum)
	{
		m_nATGridNum=nATGridNum;
		m_bIsPrjModified=true;
	}
} 

void CALMPrjManager::SetDSMGridSize(int nDSMGridSize)
{
	if(m_nDSMGridSize!=nDSMGridSize)
	{
		m_nDSMGridSize=nDSMGridSize;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetAverGrdElev(double dfGrdElev)
{
	if(fabs(m_dAverGrdElev-dfGrdElev)>1e-6)
	{
		m_dAverGrdElev=dfGrdElev;
		m_bIsPrjModified=true;
	}
} 

void CALMPrjManager::SetMaxGrdElev(double dfGrdElev)
{
	if(fabs(m_dMaxGrdElev-dfGrdElev)>1e-6)
	{
		m_dMaxGrdElev=dfGrdElev;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetMinGrdElev(double dfGrdElev)
{
	if(fabs(m_dMinGrdElev-dfGrdElev)>1e-6)
	{
		m_dMinGrdElev=dfGrdElev;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetFlyOverlap(short flyoverlap)
{
	if(fabs(double(m_flyOverlap-flyoverlap))>1e-6)
	{
		m_flyOverlap = flyoverlap;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetStripOverlap(short stripoverlap)
{
	if(fabs(double(m_stripOverlap-stripoverlap))>1e-6)
	{
		m_stripOverlap = stripoverlap;
		m_bIsPrjModified=true;
	}

}

void CALMPrjManager::SetPOSFile(CString strPOSFile)
{
	if(m_strPOSFile.Compare(strPOSFile)!=0)
	{
		m_strPOSFile=strPOSFile;
		m_bIsPrjModified=true;
	}
	
} 

void CALMPrjManager::SetEOEFile(CString strEOEFile)
{
	if(m_strEOEFile.Compare(strEOEFile)!=0)
	{
		m_strEOEFile=strEOEFile;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetCPtFile(CString strCPtFile)
{
	if(m_strCPtFile.Compare(strCPtFile)!=0)
	{
		m_strCPtFile=strCPtFile;
		m_bIsPrjModified=true;
	}
	
}

void CALMPrjManager::SetColorLutFile(CString strColorLutFile)
{
	if(m_strColorLutFile.Compare(strColorLutFile)!=0)
	{
		m_strColorLutFile=strColorLutFile;
		m_bIsPrjModified=true;
	}
	
}

void CALMPrjManager::SetImgQVDir(CString strImgQVDir)
{
	if(m_strImgQVDir.Compare(strImgQVDir)!=0)
	{
		m_strImgQVDir=strImgQVDir;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetDgImageDir(CString strDgImageDir)
{
	if(m_strDgImageDir.Compare(strDgImageDir)!=0)
	{
		m_strDgImageDir=strDgImageDir;
		m_bIsPrjModified=true;
	}
	
}

void CALMPrjManager::SetMosaicDir(CString strMosaicDir)
{
	if(m_strMosaicDir.Compare(strMosaicDir)!=0)
	{
		m_strMosaicDir=strMosaicDir;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetRegistDir(CString strRegistDir)
{
	if(m_strRegistDir.Compare(strRegistDir)!=0)
	{
		m_strRegistDir=strRegistDir;
		m_bIsPrjModified=true;
	}
	
}

void CALMPrjManager::SetFltLidarDir(CString strFltLidarDir)
{
	if(m_strFltLidarDir.Compare(strFltLidarDir)!=0)
	{
		m_strFltLidarDir=strFltLidarDir;
		m_bIsPrjModified=true;
	}

}

void CALMPrjManager::SetMatchLidarDir(CString strMatchLidarDir)
{
	if(m_strMatchLidarDir.Compare(strMatchLidarDir)!=0)
	{
		m_strMatchLidarDir=strMatchLidarDir;
		m_bIsPrjModified=true;
	}
}

void CALMPrjManager::SetDSMDir(CString strDSMDir)
{
	if(m_strDSMDir.Compare(strDSMDir)!=0)
	{
		m_strDSMDir=strDSMDir;
		m_bIsPrjModified=true;
	}
} 
 
void CALMPrjManager::SetDEMDir(CString strDEMDir)
{
	if(m_strDEMDir.Compare(strDEMDir)!=0)
	{
		m_strDEMDir=strDEMDir;
		m_bIsPrjModified=true;
	}
	
} 

void CALMPrjManager::SetDOMDir(CString strDOMDir)
{
	if(m_strDOMDir.Compare(strDOMDir)!=0)
	{
		m_strDOMDir=strDOMDir;
		m_bIsPrjModified=true;
	}
	
} 

void CALMPrjManager::SetCFilesDir(CString strCFilesDir)
{
	if(m_strCFilesDir.Compare(strCFilesDir)!=0)
	{
		m_strCFilesDir=strCFilesDir;
		m_bIsPrjModified=true;
	}
	
}

void CALMPrjManager::SetLOGDir(CString strLOGDir)
{
	if(m_strLOGDir.Compare(strLOGDir)!=0)
	{
		m_strLOGDir=strLOGDir;
		m_bIsPrjModified=true;
	}
	
}

void CALMPrjManager::SetProductDir(CString strProductDir)
{
	if(m_strProductDir.Compare(strProductDir)!=0)
	{
		m_strProductDir=strProductDir;
		m_bIsPrjModified=true;
	}
	
}
////////////////////////////////////////////////////////////////////////////////
//	  FunctionName :	void ALMPrjManager::
//	  Function     :	判断路径是否存在
//	  Principles   :   
//	  Input        :	()
//	  Output       :	None
//	  return       :	None	
//	  assignation  :	None
//	  Note         :	
//				        #include ".h"
//    example      :    
//    Information  :    DQY 2009.07.28
////////////////////////////////////////////////////////////////////////////////
bool  CALMPrjManager::bIsDirectoryExist(const char *sCurDir)
{
	char	sOldDir[_MAX_PATH];
	bool	bExist=true;
	
	ZeroMemory(sOldDir,_MAX_PATH);	
	GetCurrentDirectory(_MAX_PATH,sOldDir);
	
	if (strcmp(sCurDir,sOldDir)==0)	goto	EndPro;
	
	if (SetCurrentDirectory(sCurDir))
		SetCurrentDirectory(sOldDir);
	else
		bExist=false;
	
EndPro:
	return bExist;
} 

bool CALMPrjManager::bSelectFolderDialog(char* pszDlgTitle, CString *strFolder, HWND hParent)
{
	strFolder->Empty();

	LPMALLOC lpMalloc;
	if (::SHGetMalloc(&lpMalloc) != NOERROR) 
		return false;

	char szDisplayName[_MAX_PATH];
	char szBuffer[_MAX_PATH];
	BROWSEINFO browseInfo;
	browseInfo.hwndOwner = hParent;
	browseInfo.pidlRoot = NULL; // set root at Desktop
	browseInfo.pszDisplayName = szDisplayName;
	browseInfo.lpszTitle = pszDlgTitle;
	browseInfo.ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;
	browseInfo.lpfn = NULL;
	browseInfo.lParam = 0;
	LPITEMIDLIST lpItemIDList;
	if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
	{
	// Get the path of the selected folder from the item ID list.
		if (::SHGetPathFromIDList(lpItemIDList, szBuffer))
		{
		// At this point, szBuffer contains the path the user chose.
			if (szBuffer[0] == '\0') 
				return false;
			// We have a path in szBuffer! Return it.
			*strFolder=szBuffer;
			return true;
		}
		else 
			return true; // strResult is empty
		lpMalloc->Free(lpItemIDList);
		lpMalloc->Release();
	}
	return	false;
} 


bool CALMPrjManager::bIsFileExist(const char *filename)
{
	return (_access(filename, 0) == 0); 
}

void CALMPrjManager::Copy(CALMPrjManager& __x)
{
	m_strAlmFile=__x.GetAlmFile();
	m_strAlmFileDir=__x.GetAlmFileDir();
	m_strAlmName=__x.GetAlmName();
	m_aryStrMetaInf.Copy(*(__x.GetMetaInfo()));
	m_nImgNum=__x.GetImageCount();
	m_strRawImgDir=__x.GetRawImageDir();
	m_nImgStripNum=__x.GetImgStripNum();
	m_aryImgList.Copy(*(__x.GetImgList()));
	m_nLidarStripNum=__x.GetLidarStripNum();
	m_strRawLidarDir=__x.GetRawLidarDir();
	m_aryLidarList.Copy(*(__x.GetLidarList()));
	m_aryPOSData.Copy(*(__x.GetPOSData()));
	m_aryEOEData.Copy(*(__x.GetEOEData()));
//	m_aryGCPData.Copy(*(__x.Get))
	m_Camera=*(__x.GetCameraPara());
	m_nATGridNum=__x.GetATGridNum();
	m_nDSMGridSize=__x.GetDSMGridSize();
	m_dMaxGrdElev=__x.GetMaxGrdElev();
	m_dAverGrdElev=__x.GetAverGrdElev();
	m_dMinGrdElev=__x.GetMinGrdElev();

	m_flyOverlap=__x.GetFlyOverlap();
	m_stripOverlap=__x.GetStripOverlap();

	m_strPOSFile=__x.GetPOSFile();
	m_strEOEFile=__x.GetEOEFile();
	m_strCPtFile=__x.GetCPtFile();
	m_strColorLutFile=__x.GetColorLutFile();
	m_strImgQVDir=__x.GetImgQVDir();
	m_strDgImageDir=__x.GetDgImageDir();
	m_strMosaicDir=__x.GetMosaicDir();
	m_strRegistDir=__x.GetRegistDir();
	m_strFltLidarDir=__x.GetFltLidarDir();
	m_strMatchLidarDir=__x.GetMatchLidarDir();
	m_strDSMDir=__x.GetDSMDir();
	m_strDEMDir=__x.GetDEMDir();
	m_strDOMDir=__x.GetDOMDir();
	m_strCFilesDir=__x.GetCFilesDir();
	m_strLOGDir=__x.GetLOGDir();
	m_strProductDir=__x.GetProductDir();
	m_aryDOMProduct.Copy(*(__x.GetDOMProduct()));
	m_aryDEMProduct.Copy(*(__x.GetDEMProduct()));
	m_aryTMProduct.Copy(*(__x.GetThemeProduct()));
}

