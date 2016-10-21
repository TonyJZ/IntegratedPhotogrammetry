#include "stdafx.h"
#include "lidBase.h"
#include "IO.h"
#include "EnvDialog.h"

#include <iostream>
#include <fstream>
using namespace std;


bool  _IsFileExist(const char *filename)
{
	return (_access(filename, 0) == 0); 
}

bool  _IsDirectoryExist(const char *sCurDir)
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

/*
bool	_IsFolderExist(const char *sCurDir)
{
	fstream _file;
	_file.open(sCurDir,ios::in);
	if(!_file)
	{
		return	false;
	}
	else
	{
		return	true;
	}
}*/


/********************************************************************************************************************
	取系统临时目录		
	char	pTmpDir[_MAX_PATH];
	GetTempPath(_MAX_PATH, pTmpDir);
********************************************************************************************************************/

//删除指定目录以及目录下的全部文件
bool	_DeleteDirectory(char *pszDir)
{
	CFileFind	fileFind;
	CString	strTmp, strFileName;
	BOOL	bIsEnd;
	
	strTmp.Format("%s\\*.*",pszDir);
	bIsEnd=fileFind.FindFile(strTmp.GetBuffer(0));
	while(bIsEnd)
	{
		bIsEnd=fileFind.FindNextFile();
		if(!fileFind.IsDots())
		{
			strFileName=fileFind.GetFilePath();
			if(fileFind.IsDirectory())
			{//递归删除文件夹
				_DeleteDirectory(strFileName.GetBuffer(0));
			}
			else
			{
				DeleteFile(strFileName.GetBuffer(0));
			}
		}
	}
	
	fileFind.Close();
	if(RemoveDirectory(pszDir)==FALSE)
		return	false;
	else
		return	true;
}

bool _GetProgramDirectory(CString *pszDir)
{
	bool   bResult;   
	bResult   =   false; 
	char   szFilePath[1024];
	int		count;

	if(GetModuleFileName(NULL, szFilePath, 1024))   
	{   
		char   *ptr = strrchr(szFilePath, _T('\\'));   
		if(ptr != NULL)     
		{   
			count = ptr - szFilePath;
			strncpy(pszDir->GetBuffer(count), szFilePath, count);
			bResult = true;   
		}
	}
	pszDir->ReleaseBuffer();
	return   bResult;   
}

bool _GetEnv(LIDENV *pEnv)
{
	bool	bReturn=true;
	CString	strSysPath;	
	FILE	*fp=NULL;

	bReturn=_GetProgramDirectory(&strSysPath);
	if(!bReturn)
		return	bReturn;

	strSysPath=strSysPath+"\\"+SYS_FILE_NAME;

	bReturn=_IsFileExist(strSysPath.GetBuffer(_MAX_PATH));
	if(!bReturn)
	{//不存在系统环境变量文件则创建
		_SetEnv();
	}

	memset(pEnv, 0, sizeof(LIDENV));
	fp=fopen(strSysPath.GetBuffer(_MAX_PATH), "r");
	fread(pEnv->sys,sizeof(char),256,fp);
	fread(pEnv->cur,sizeof(char),256,fp);
	fread(pEnv->temp,sizeof(char),256,fp);

	fclose(fp);
	return	bReturn;
}

bool _SetEnv()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	bool	bReturn=true;
	CString	strSysFileName;
	CEnvDialog	dlg;
	FILE	*fp;
	char	pSysDir[256];
	char	pCurDir[256];
	char	pTempDir[256];

	ZeroMemory(pSysDir,sizeof(char)*256);
	ZeroMemory(pCurDir,sizeof(char)*256);
	ZeroMemory(pTempDir,sizeof(char)*256);
	bReturn=_GetProgramDirectory(&strSysFileName);
	if(!bReturn)
		goto	FUNC_END;
	
	strSysFileName=strSysFileName+"\\"+SYS_FILE_NAME;
	bReturn=_IsFileExist(strSysFileName.GetBuffer(_MAX_PATH));
	if(bReturn)
	{
		fp=fopen(strSysFileName.GetBuffer(128), "r+");
		if(fp==NULL)
		{
			bReturn=false;
			goto	FUNC_END;
		}
		fread(pSysDir, sizeof(char), 256, fp);
		fread(pCurDir, sizeof(char), 256, fp);
		fread(pTempDir, sizeof(char), 256, fp);
		
		dlg.SetSysDir(pSysDir);
		dlg.SetCurDir(pCurDir);
		dlg.SetTempDir(pTempDir);
		if(dlg.DoModal()==IDOK)
		{
			fseek(fp, 0, SEEK_SET);
			fwrite(dlg.m_strSysDir.GetBuffer(256), sizeof(char), 256, fp);
			fwrite(dlg.m_strCurDir.GetBuffer(256), sizeof(char), 256, fp);
			fwrite(dlg.m_strTempDir.GetBuffer(256), sizeof(char), 256, fp);
		}
	}
	else
	{//不存在系统环境参数文件
		fp=fopen(strSysFileName.GetBuffer(128),"w");
		fwrite(pSysDir, sizeof(char), 256, fp);
		fwrite(pCurDir, sizeof(char), 256, fp);
		fwrite(pTempDir, sizeof(char), 256, fp);
	}

FUNC_END:
	if(fp)
	{
		fclose(fp);
		fp=NULL;
	}
	return	bReturn;
}

bool _GetDiskFreeSpace(LPCTSTR lpDirectoryName, DWORD *dwFreeDiskSpace)
{
//	bool	bReturn=true;

//	DWORD   dwTotalDiskSpace,/*dwFreeDiskSpace,*/dwUsedDiskSpace;   
    
	ULARGE_INTEGER   uiFreeBytesAvailableToCaller;   
	ULARGE_INTEGER   uiTotalNumberOfBytes;   
	ULARGE_INTEGER   uiTotalNumberOfFreeBytes;   
    
	if(GetDiskFreeSpaceEx(lpDirectoryName, &uiFreeBytesAvailableToCaller,
		&uiTotalNumberOfBytes, &uiTotalNumberOfFreeBytes))   
	{   
//		dwTotalDiskSpace  = (DWORD)(uiTotalNumberOfBytes.QuadPart / 1024 / 1024);   
		*dwFreeDiskSpace  = (DWORD)(uiFreeBytesAvailableToCaller.QuadPart >> 20);   
//		dwUsedDiskSpace   = dwTotalDiskSpace - *dwFreeDiskSpace;   
/*
		TRACE("硬盘%s::总空间%dMB, 已用%dMB, 可用%dMB\n", lpDirectoryName,   
			  dwTotalDiskSpace, dwUsedDiskSpace, *dwFreeDiskSpace);   
*/

		return	true;
	}
	else
		return	false;

}

bool _PointInRect(double x, double y, D_RECT rect)
{
	if((x-rect.XMin)*(rect.XMax-x)>0 && (y-rect.YMin)*(rect.YMax-y)>0)
		return	true;
	else
		return	false;
}
//从全路径文件名中提取文件名(不包含文件后缀)
CString _ExtractFileName(CString strFilePathName)
{
	CString	strFileName;
	int	pos, length;

	length=strFilePathName.GetLength();
	pos=strFilePathName.ReverseFind('\\');
	if(pos==-1)
		pos=strFilePathName.ReverseFind('/');

// 	if(pos==-1)
// 		return strFileName;

	strFileName=strFilePathName.Right(length-pos-1);
	pos=strFileName.ReverseFind('.');
	strFileName=strFileName.Left(pos);

	return	strFileName;
}

CString _ExtractFileFullName(CString strFilePathName)
{
	CString	strFileName;
	int	pos, length;

	length=strFilePathName.GetLength();
	pos=strFilePathName.ReverseFind('\\');
	strFileName=strFilePathName.Right(length-pos-1);
	// 	pos=strFileName.ReverseFind('.');
	// 	strFileName=strFileName.Left(pos);

	return	strFileName;
}

CString _ExtractFileSuffix(CString strFileName)
{
	CString	strSuffix;
	int	pos, length;
	
	strSuffix.Empty();

	length=strFileName.GetLength();
	pos=strFileName.ReverseFind('.');
	if(pos!=-1)
		strSuffix=strFileName.Right(length-pos-1);
	
	return	strSuffix;
}

void _GetDisplayResolution(int &cx, int &cy)
{
	//水平分辨率
	cx=::GetSystemMetrics(SM_CXSCREEN); 
	//垂直分辨率
	cy=::GetSystemMetrics(SM_CYSCREEN); 
}

bool _SelectFolderDialog(char* pszDlgTitle, CString *strFolder, HWND hParent)
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