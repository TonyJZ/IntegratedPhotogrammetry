// BlockSetting.h: interface for the CBlockSetting class.
// 2010.2.5 created by zhangjing	测区管理
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BlockSetting_H__9F2E06C8_DB5C_4CC2_BC33_78BA24AD0139__INCLUDED_)
#define AFX_BlockSetting_H__9F2E06C8_DB5C_4CC2_BC33_78BA24AD0139__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _BLOCKSETTING_
#define  _Blk_Setting_  __declspec(dllexport)
#else
#define  _Blk_Setting_  __declspec(dllimport)	
#endif

#include "..\..\include\iphBaseDef.h"
#include "..\..\include\ALMPrjManager.h"

/*
class _Blk_Manager_ CBlockSetting
{
public:
	CBlockSetting();
	virtual ~CBlockSetting();

	bool CreateBlock();
	bool OpenBlock(char *pBlockPathName);
	bool SaveBlock(char *pBlockPathName);

	CString	GetBlockName();

	iphBlockInfo* GetBlockInfo() { return m_pBlockInfo; };

private:
	iphBlockInfo	*m_pBlockInfo;	
	CString			m_strBlockName;		//工程名(去路径和后缀)
	CString			m_strBlockPathName;	//全路径工程名
};*/


//void _Blk_Setting_ BlockSetting(CALMPrjManager *pAlm);

#ifndef _BLOCKSETTING_
#ifdef _DEBUG
#pragma comment(lib,"BlockSettingD.lib")
#else
#pragma comment(lib,"BlockSetting.lib")
#endif
#endif

#endif // !defined(AFX_BlockSetting_H__9F2E06C8_DB5C_4CC2_BC33_78BA24AD0139__INCLUDED_)
