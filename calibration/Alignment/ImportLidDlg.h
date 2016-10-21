#if !defined(AFX_IMPORTLIDDLG_H__119197C3_FE87_46C4_85BD_0F52CCBDD219__INCLUDED_)
#define AFX_IMPORTLIDDLG_H__119197C3_FE87_46C4_85BD_0F52CCBDD219__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportLidDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportLidDlg dialog
#include "resource.h"
#include "afxcmn.h"

class CImportLidDlg : public CDialog
{
// Construction
public:
	CImportLidDlg(int FileSetID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportLidDlg)
	enum { IDD = IDD_ImportDataDlg };
	CString	m_strFileSetName;
	CString	m_strImgName;
	CString	m_strAtnName;
	CArray<CString, CString&>		m_imgNameVec;
	CString m_strTrajName;
	double m_dAFH;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportLidDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImportLidDlg)
	afx_msg void OnBUTTONAtnName();
	afx_msg void OnBUTTONImgName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_wndImgList;
	afx_msg void OnBnClickedButtonAddimg();
	virtual BOOL OnInitDialog();

	void AddImageTable(int startPos);

	afx_msg void OnBnClickedButtonTrajectory();
	virtual void OnOK();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTLIDDLG_H__119197C3_FE87_46C4_85BD_0F52CCBDD219__INCLUDED_)
