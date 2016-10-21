#if !defined(AFX_COORDSYSPAGE_H__8A2A2820_83D1_41E4_A5B4_46C3AB676E85__INCLUDED_)
#define AFX_COORDSYSPAGE_H__8A2A2820_83D1_41E4_A5B4_46C3AB676E85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CoordSysPage.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CCoordSysPage dialog

class CCoordSysPage : public CDialog
{
// Construction
public:
	CCoordSysPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCoordSysPage)
	enum { IDD = IDD_CoordinateSysDlg };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoordSysPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCoordSysPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COORDSYSPAGE_H__8A2A2820_83D1_41E4_A5B4_46C3AB676E85__INCLUDED_)
