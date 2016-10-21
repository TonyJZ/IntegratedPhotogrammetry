#if !defined(AFX_ENVDIALOG_H__444251E6_6ADA_4687_8F39_4123DA1B1AE3__INCLUDED_)
#define AFX_ENVDIALOG_H__444251E6_6ADA_4687_8F39_4123DA1B1AE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnvDialog.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CEnvDialog dialog

class CEnvDialog : public CDialog
{
// Construction
public:
	CEnvDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnvDialog)
	enum { IDD = IDD_ENV_DIALOG };
	CString	m_strCurDir;
	CString	m_strSysDir;
	CString	m_strTempDir;
	//}}AFX_DATA

	void	SetSysDir(char *pSysDir);
	void	SetCurDir(char *pCurDir);
	void	SetTempDir(char *pTempDir);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnvDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnvDialog)
	afx_msg void OnCurButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENVDIALOG_H__444251E6_6ADA_4687_8F39_4123DA1B1AE3__INCLUDED_)
