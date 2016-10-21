// ALMProjectManager.h : main header file for the ALMPROJECTMANAGER DLL
//

#if !defined(AFX_ALMPROJECTMANAGER_H__AE009B77_0DDE_4211_8DED_5B8B071ED61F__INCLUDED_)
#define AFX_ALMPROJECTMANAGER_H__AE009B77_0DDE_4211_8DED_5B8B071ED61F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CALMProjectManagerApp
// See ALMProjectManager.cpp for the implementation of this class
//

class CALMProjectManagerApp : public CWinApp
{
public:
	CALMProjectManagerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CALMProjectManagerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CALMProjectManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALMPROJECTMANAGER_H__AE009B77_0DDE_4211_8DED_5B8B071ED61F__INCLUDED_)
