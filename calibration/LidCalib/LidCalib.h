// LidCalib.h : main header file for the LIDCALIB DLL
//

#if !defined(AFX_LIDCALIB_H__A548EC4B_231C_481C_BE82_931F72A4A65A__INCLUDED_)
#define AFX_LIDCALIB_H__A548EC4B_231C_481C_BE82_931F72A4A65A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLidCalibApp
// See LidCalib.cpp for the implementation of this class
//

class CLidCalibApp : public CWinApp
{
public:
	CLidCalibApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLidCalibApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLidCalibApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIDCALIB_H__A548EC4B_231C_481C_BE82_931F72A4A65A__INCLUDED_)
