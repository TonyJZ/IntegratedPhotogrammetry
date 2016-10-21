// ImageMatch.h : main header file for the IMAGEMATCH DLL
//

#if !defined(AFX_IMAGEMATCH_H__73B4F057_3A3C_4431_A4CD_C9079D715756__INCLUDED_)
#define AFX_IMAGEMATCH_H__73B4F057_3A3C_4431_A4CD_C9079D715756__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CImageMatchApp
// See ImageMatch.cpp for the implementation of this class
//

class CImageMatchApp : public CWinApp
{
public:
	CImageMatchApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageMatchApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CImageMatchApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEMATCH_H__73B4F057_3A3C_4431_A4CD_C9079D715756__INCLUDED_)
