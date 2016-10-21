// IPLR.h : main header file for the IPLR application
//

#if !defined(AFX_IPLR_H__6995B742_E061_43EB_B787_13B25EC05353__INCLUDED_)
#define AFX_IPLR_H__6995B742_E061_43EB_B787_13B25EC05353__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "IPLRDoc.h"
#include "ALMPrjManager.h"
/////////////////////////////////////////////////////////////////////////////
// CIPLRApp:
// See IPLR.cpp for the implementation of this class
//

class CIPLRApp : public CWinApp
{
public:
	CIPLRApp();
	~CIPLRApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPLRApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CIPLRApp)
	afx_msg void OnAppAbout();
	afx_msg void OnPrjNEW();
	afx_msg void OnPrjOPEN();
	afx_msg void OnPrjClose();
	afx_msg void OnPrjSAVE();
	afx_msg void OnPrjSAVEAS();
	afx_msg void OnUpdateSIFTKeyPoint(CCmdUI* pCmdUI);
	afx_msg void OnModifyProject();
	afx_msg void OnUpdateModifyProject(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExporImgaop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportImgiop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExportblv(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetCurDocTitle();
	CIPLRDoc *GetTailDocument();
	CIPLRDoc *GetCurDocument();

public:
	CALMPrjManager	*m_pBlockManager;

protected:
	CSingleDocTemplate	*m_pDocTemplate;
};

extern CIPLRApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPLR_H__6995B742_E061_43EB_B787_13B25EC05353__INCLUDED_)
