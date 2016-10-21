// CalibProc.h : main header file for the CALIBPROC application
//

#if !defined(AFX_CALIBPROC_H__6AD9CB67_5D4C_4EFA_AD1D_C5DD00F8B7C0__INCLUDED_)
#define AFX_CALIBPROC_H__6AD9CB67_5D4C_4EFA_AD1D_C5DD00F8B7C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCalibProcApp:
// See CalibProc.cpp for the implementation of this class
//

#include "orsBase\orsUtil.h"

orsIPlatform *getPlatform();

class CCalibProcApp : public CWinApp
{
public:
	CCalibProcApp();
	~CCalibProcApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalibProcApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCalibProcApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBPROC_H__6AD9CB67_5D4C_4EFA_AD1D_C5DD00F8B7C0__INCLUDED_)
