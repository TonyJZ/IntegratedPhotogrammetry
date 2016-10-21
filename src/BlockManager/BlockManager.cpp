// BlockManager.cpp: implementation of the CBlockManager class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "BlockManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockManager::CBlockManager()
{
	m_pBlockInfo=NULL;
}

CBlockManager::~CBlockManager()
{
	if(m_pBlockInfo)
	{
		delete m_pBlockInfo;
		m_pBlockInfo=NULL;
	}
}


bool CBlockManager::CreateBlock()
{
	CFileDialog	dlg( FALSE, "blk", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("Block File (*.blk)|*.blk|All Files(*.*)|*.*||"));

	if(dlg.DoModal()==IDOK)
	{

	}
	else
		return	false;
}

CString	CBlockManager::GetBlockName()
{
	CString	BlockName="111";

	return	BlockName;
}