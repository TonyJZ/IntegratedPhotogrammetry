// TieObjMeasure.h : main header file for the TIEOBJMEASURE application
//

#if !defined(AFX_TIEOBJMEASURE_H__B4510BEF_FB38_4369_A7FB_3A749638BBF0__INCLUDED_)
#define AFX_TIEOBJMEASURE_H__B4510BEF_FB38_4369_A7FB_3A749638BBF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTieObjMeasureApp:
// See TieObjMeasure.cpp for the implementation of this class
//

class CTieObjMeasureApp : public CWinApp
{
public:
	CTieObjMeasureApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTieObjMeasureApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTieObjMeasureApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIEOBJMEASURE_H__B4510BEF_FB38_4369_A7FB_3A749638BBF0__INCLUDED_)
