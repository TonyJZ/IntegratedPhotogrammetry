// lidBasLib.h : main header file for the LIDBASLIB DLL
//

#if !defined(AFX_LIDBASLIB_H__3EF7032B_65D5_4EF1_B342_3795093C6D96__INCLUDED_)
#define AFX_LIDBASLIB_H__3EF7032B_65D5_4EF1_B342_3795093C6D96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLidBasLibApp
// See lidBasLib.cpp for the implementation of this class
//

class CLidBasLibApp : public CWinApp
{
public:
	CLidBasLibApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLidBasLibApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLidBasLibApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIDBASLIB_H__3EF7032B_65D5_4EF1_B342_3795093C6D96__INCLUDED_)
