// Alignment.h : main header file for the ALIGNMENT application
//

#if !defined(AFX_ALIGNMENT_H__1A8B6BC6_B356_4CAF_A887_173839F11E6E__INCLUDED_)
#define AFX_ALIGNMENT_H__1A8B6BC6_B356_4CAF_A887_173839F11E6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "orsBase\orsUtil.h"

/////////////////////////////////////////////////////////////////////////////
// CAlignmentApp:
// See Alignment.cpp for the implementation of this class
//

class CAlignmentApp : public CWinApp,
					  public CBCGPWorkspace
{
public:
	CAlignmentApp();
	~CAlignmentApp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState ();

	CString m_appDir;
	CString GetAppDir();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlignmentApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CAlignmentApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline CAlignmentApp *GetApp()	{ return (CAlignmentApp *)AfxGetApp();	};

extern CAlignmentApp theApp;

orsIPlatform *getPlatform();

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIGNMENT_H__1A8B6BC6_B356_4CAF_A887_173839F11E6E__INCLUDED_)
