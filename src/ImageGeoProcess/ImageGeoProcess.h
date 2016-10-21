// ImageGeoProcess.h : main header file for the IMAGEGEOPROCESS DLL
//

#if !defined(AFX_IMAGEGEOPROCESS_H__1857464A_681D_4F70_ACB6_15D1CFA1318E__INCLUDED_)
#define AFX_IMAGEGEOPROCESS_H__1857464A_681D_4F70_ACB6_15D1CFA1318E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CImageGeoProcessApp
// See ImageGeoProcess.cpp for the implementation of this class
//

class CImageGeoProcessApp : public CWinApp
{
public:
	CImageGeoProcessApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageGeoProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CImageGeoProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEGEOPROCESS_H__1857464A_681D_4F70_ACB6_15D1CFA1318E__INCLUDED_)
