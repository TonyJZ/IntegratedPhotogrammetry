#ifndef _LID_BASE_FUNCTIONS_
#define _LID_BASE_FUNCTIONS_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/********************************************************************************************************************
//			create in 2008.9.18		by Tony	
//	通用基础函数库：
//			1.文件创建、删除、查找
//			2.文件夹创建、删除
//			3.环境变量设置
//			4.磁盘空间查询
********************************************************************************************************************/

#include "PointBaseDefinitons.h"

#ifdef _lidBASELIB_
#define  _BaseLib_  __declspec(dllexport)
#else
#define  _BaseLib_  __declspec(dllimport)	
#endif



typedef struct
{
//	char clib[128];	//汉字库目录
	char sys[256];	//系统库目录			program 目录 （作用不大）
	char cur[256];	//当前工作目录		当前数据处理的工作目录
	char temp[256];	//临时工作目录（工作区使用）
//	char slib[128];	//系统库目录
}LIDENV;

const	char	SYS_FILE_NAME[] = "sysEnv.txt";		//系统环境变量记录

//判断指定文件是否存在
bool	_BaseLib_	_IsFileExist(const char *filename);
//指定目录是否存在
bool	_BaseLib_	_IsDirectoryExist(const char *sCurDir);
//判断文件夹是否存在
//bool    _BaseLib_	_IsFolderExist(const char *sCurDir);
//删除指定目录以及目录下的全部文件
bool	_BaseLib_	_DeleteDirectory(char *pszDir);
//取当前程序执行目录
bool	_BaseLib_	_GetProgramDirectory(CString *pszDir);	
//取执行程序环境信息
bool	_BaseLib_	_GetEnv(LIDENV	*pEnv);
//设置执行程序环境		
//return		true/false		设置成功/失败或取消设置
bool	_BaseLib_	_SetEnv();
//获取指定分区剩余空间
//	lpDirectoryName:  "C:" or "C:\\temp" or "C:\\temp\\"
//	dwFreeDiskSpace:  **MB
bool	_BaseLib_	_GetDiskFreeSpace(LPCTSTR lpDirectoryName, DWORD *dwFreeDiskSpace);
//取当前系统显示分辨率
void	_BaseLib_	_GetDisplayResolution(int &cx, int &cy);

//从全路径文件名中提取文件名(不包含文件后缀)
CString	_BaseLib_   _ExtractFileName(CString strFilePathName);
//提取文件后缀
CString	_BaseLib_	_ExtractFileSuffix(CString strFileName);
//从全路径中提取完整文件名，包括后缀。
CString _BaseLib_   _ExtractFileFullName(CString strFilePathName);
//从文件名中提取文件目录
CString _BaseLib_   _ExtractFileDir(CString strFilePathName);

//选择文件夹对话框	true/false: 选中/未选中	对话框名称，选中路径，父窗口句柄
bool	_BaseLib_	_SelectFolderDialog(char* pszDlgTitle, CString *strFolder, HWND hParent=NULL);

//################################  基本数学处理函数  ################################################
//判断点是否在矩形范围内		true/false
bool _BaseLib_		_PointInRect(double x, double y, D_RECT rect);





#ifndef _lidBASELIB_
#ifdef _DEBUG
#pragma comment(lib,"lidBasLibD.lib")
#else
#pragma comment(lib,"lidBasLib.lib")
#endif
#endif

#endif