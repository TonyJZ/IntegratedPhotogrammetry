// LidQC.h : main header file for the LIDQC DLL
//

#if !defined(AFX_LIDQC_H__FD634DCB_9C0B_46E9_8036_1BD7256AFEDA__INCLUDED_)
#define AFX_LIDQC_H__FD634DCB_9C0B_46E9_8036_1BD7256AFEDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLidQCApp
// See LidQC.cpp for the implementation of this class
//

class CLidQCApp : public CWinApp
{
public:
	CLidQCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLidQCApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLidQCApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIDQC_H__FD634DCB_9C0B_46E9_8036_1BD7256AFEDA__INCLUDED_)
