// ImageMatchView.h : main header file for the IMAGEMATCHVIEW application
//

#if !defined(AFX_IMAGEMATCHVIEW_H__18FE87DA_CCA7_4212_908B_A7B2192DA82C__INCLUDED_)
#define AFX_IMAGEMATCHVIEW_H__18FE87DA_CCA7_4212_908B_A7B2192DA82C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "ImageMatchViewDoc.h"
#include "ALMPrjManager.h"

/////////////////////////////////////////////////////////////////////////////
// CImageMatchViewApp:
// See ImageMatchView.cpp for the implementation of this class
//

class CImageMatchViewApp : public CWinApp
{
public:
	CImageMatchViewApp();
	~CImageMatchViewApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageMatchViewApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CImageMatchViewApp)
	afx_msg void OnAppAbout();
	afx_msg void OnPrjOPEN();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SetCurDocTitle();
	CImageMatchViewDoc *GetTailDocument();
	CImageMatchViewDoc *GetCurDocument();	

public:
	CALMPrjManager	*m_pBlockManager;

protected:
	CSingleDocTemplate	*m_pDocTemplate;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEMATCHVIEW_H__18FE87DA_CCA7_4212_908B_A7B2192DA82C__INCLUDED_)
