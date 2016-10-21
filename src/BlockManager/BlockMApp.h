// BlockManager.h : main header file for the BLOCKMANAGER DLL
//

#if !defined(AFX_BLOCKMANAGER_H__11378D54_1615_4415_ADB4_B6EC9FAA95CE__INCLUDED_)
#define AFX_BLOCKMANAGER_H__11378D54_1615_4415_ADB4_B6EC9FAA95CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBlockManagerApp
// See BlockManager.cpp for the implementation of this class
//

class CBlockManagerApp : public CWinApp
{
public:
	CBlockManagerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlockManagerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CBlockManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOCKMANAGER_H__11378D54_1615_4415_ADB4_B6EC9FAA95CE__INCLUDED_)
