// BlockSetting.cpp: implementation of the CBlockSetting class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "BlockSetting.h"
#include "BlockSettingDlg.h"
#include "..\..\include\lidBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
CBlockSetting::CBlockSetting()
{
	m_pBlockInfo=NULL;
	m_strBlockName=_T("");
	m_strBlockPathName=_T("");
}

CBlockSetting::~CBlockSetting()
{
	if(m_pBlockInfo)
	{
		delete m_pBlockInfo;
		m_pBlockInfo=NULL;
	}
}


bool CBlockSetting::CreateBlock()
{
	HINSTANCE oldRcHandle = AfxGetResourceHandle();
	
#ifdef _DEBUG
	AfxSetResourceHandle ( GetModuleHandle("BlockSettingD.dll") );
#else
	AfxSetResourceHandle ( GetModuleHandle("BlockSetting.dll") );
#endif

	CFileDialog	dlg( FALSE, "blk", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("Block File (*.blk)|*.blk|All Files(*.*)|*.*||"));
	
	if(dlg.DoModal()==IDOK)
	{
		if(m_pBlockInfo)
			m_pBlockInfo->reinitialize();
		else
			m_pBlockInfo=new iphBlockInfo;

		CBlockSettingDlg	blockdlg(m_pBlockInfo, 0);
		blockdlg.DoModal();

		m_strBlockPathName=dlg.GetPathName();
		m_strBlockName=_ExtractFileName(m_strBlockPathName);

		SaveBlock((LPSTR)(LPCTSTR)m_strBlockPathName);
	}
	else
		return	false;

	AfxSetResourceHandle ( oldRcHandle );
}

CString	CBlockSetting::GetBlockName()
{
	return	m_strBlockName;
}

bool CBlockSetting::OpenBlock(char *pBlockPathName)
{
	return	true;
}

bool CBlockSetting::SaveBlock(char *pBlockPathName)
{
	
	return	true;
}*/


/*
void BlockSetting(CALMPrjManager *pAlm)
{
	CFileDialog	dlg( FALSE, "blk", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("Block File (*.blk)|*.blk|All Files(*.*)|*.*||"));
	CString	strPathName;

	if(dlg.DoModal()==IDOK)
	{
		HINSTANCE oldRcHandle = AfxGetResourceHandle();
		
#ifdef _DEBUG
		AfxSetResourceHandle ( GetModuleHandle("BlockSettingD.dll") );
#else
		AfxSetResourceHandle ( GetModuleHandle("BlockSetting.dll") );
#endif
		if(pAlm==NULL)
		{
			pAlm=new CALMPrjManager;
		}
		
		strPathName=dlg.GetPathName();
		pAlm->CreateALMPrj(strPathName.GetBuffer(0));

		CBlockSettingDlg	blockdlg(pAlm, 0);
		blockdlg.DoModal();
		
		AfxSetResourceHandle ( oldRcHandle );
	}

}*/
