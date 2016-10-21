// WorkspaceBar.h : interface of the CWorkspaceBar2 class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKSPACEBAR2_H__42FB45AE_F0D4_4D25_BA32_D4943D4D7623__INCLUDED_)
#define AFX_WORKSPACEBAR2_H__42FB45AE_F0D4_4D25_BA32_D4943D4D7623__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWorkspaceBar2 : public CBCGPDockingControlBar
{
public:
	CWorkspaceBar2();

// Attributes
protected:
	CTreeCtrl	m_wndTree;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkspaceBar2)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkspaceBar2();

// Generated message map functions
protected:
	//{{AFX_MSG(CWorkspaceBar2)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACEBAR2_H__42FB45AE_F0D4_4D25_BA32_D4943D4D7623__INCLUDED_)
