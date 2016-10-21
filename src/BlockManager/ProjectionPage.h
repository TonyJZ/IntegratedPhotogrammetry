#if !defined(AFX_PROJECTIONPAGE_H__2154DBA6_115C_465A_BFB7_6B674C5DC37E__INCLUDED_)
#define AFX_PROJECTIONPAGE_H__2154DBA6_115C_465A_BFB7_6B674C5DC37E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProjectionPage.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CProjectionPage dialog

class CProjectionPage : public CDialog
{
// Construction
public:
	CProjectionPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProjectionPage)
	enum { IDD = IDD_ProjectionDlg };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectionPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectionPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTIONPAGE_H__2154DBA6_115C_465A_BFB7_6B674C5DC37E__INCLUDED_)
