// ImageRectify.h : main header file for the IMAGERECTIFY DLL
//

#if !defined(AFX_IMAGERECTIFY_H__96D598E9_8865_4EFC_9451_D8292A32DE82__INCLUDED_)
#define AFX_IMAGERECTIFY_H__96D598E9_8865_4EFC_9451_D8292A32DE82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CImageRectifyApp
// See ImageRectify.cpp for the implementation of this class
//

class CImageRectifyApp : public CWinApp
{
public:
	CImageRectifyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageRectifyApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CImageRectifyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGERECTIFY_H__96D598E9_8865_4EFC_9451_D8292A32DE82__INCLUDED_)
