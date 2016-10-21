// POSProc.h : main header file for the POSPROC application
//

#if !defined(AFX_POSPROC_H__8136DD4A_0C79_4A10_A1E1_D82FA820AEA9__INCLUDED_)
#define AFX_POSPROC_H__8136DD4A_0C79_4A10_A1E1_D82FA820AEA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPOSProcApp:
// See POSProc.cpp for the implementation of this class
//

class CPOSProcApp : public CWinApp
{
public:
	CPOSProcApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOSProcApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPOSProcApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSPROC_H__8136DD4A_0C79_4A10_A1E1_D82FA820AEA9__INCLUDED_)
