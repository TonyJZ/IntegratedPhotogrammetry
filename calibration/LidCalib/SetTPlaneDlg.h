#if !defined(AFX_SETTPLANEDLG_H__A7A0035E_079F_4BE1_AFCD_0A6074A04E19__INCLUDED_)
#define AFX_SETTPLANEDLG_H__A7A0035E_079F_4BE1_AFCD_0A6074A04E19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetTPlaneDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetTPlaneDlg dialog
#include "resource.h"

class CSetTPlaneDlg : public CDialog
{
// Construction
public:
	CSetTPlaneDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetTPlaneDlg)
	enum { IDD = IDD_SetTPlaneDLG };
	double	m_alt0;
	double	m_lat0;
	double	m_lon0;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetTPlaneDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetTPlaneDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTPLANEDLG_H__A7A0035E_079F_4BE1_AFCD_0A6074A04E19__INCLUDED_)
