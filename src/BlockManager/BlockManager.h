// BlockManager.h: interface for the CBlockManager class.
// 2010.2.5 created by zhangjing	²âÇø¹ÜÀí
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOCKMANAGER_H__9F2E06C8_DB5C_4CC2_BC33_78BA24AD0139__INCLUDED_)
#define AFX_BLOCKMANAGER_H__9F2E06C8_DB5C_4CC2_BC33_78BA24AD0139__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _BLOCKMANAGER_
#define  _Blk_Manager_  __declspec(dllexport)
#else
#define  _Blk_Manager_  __declspec(dllimport)	
#endif

#include "IPhBaseDef.h"

class _Blk_Manager_ CBlockManager  
{
public:
	CBlockManager();
	virtual ~CBlockManager();

	bool CreateBlock();
	CString	GetBlockName();

private:
	iphBlockInfo	*m_pBlockInfo;	

};

#ifndef _BLOCKMANAGER_
#ifdef _DEBUG
#pragma comment(lib,"BlockManagerD.lib")
#else
#pragma comment(lib,"BlockManager.lib")
#endif
#endif

#endif // !defined(AFX_BLOCKMANAGER_H__9F2E06C8_DB5C_4CC2_BC33_78BA24AD0139__INCLUDED_)
