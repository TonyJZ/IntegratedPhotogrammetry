// POSProcDlg.h : header file
//

#if !defined(AFX_POSPROCDLG_H__7848BE08_8289_4E93_A6AC_A088A7879E12__INCLUDED_)
#define AFX_POSPROCDLG_H__7848BE08_8289_4E93_A6AC_A088A7879E12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPOSProcDlg dialog

class CPOSProcDlg : public CDialog
{
// Construction
public:
	CPOSProcDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPOSProcDlg)
	enum { IDD = IDD_POSPROC_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPOSProcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPOSProcDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSPROCDLG_H__7848BE08_8289_4E93_A6AC_A088A7879E12__INCLUDED_)
