#if !defined(AFX_SETCAMERADLG_H__A1E6740D_596B_4D50_A537_DFF13C6BA42E__INCLUDED_)
#define AFX_SETCAMERADLG_H__A1E6740D_596B_4D50_A537_DFF13C6BA42E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetCameraDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetCameraDlg dialog

class CSetCameraDlg : public CDialog
{
// Construction
public:
	CSetCameraDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetCameraDlg)
	enum { IDD = IDD_SetCameraDlg };
	double	m_f;
	long	m_imgHei;
	long	m_imgWid;
	double	m_k1;
	double	m_k2;
	double	m_k3;
	double	m_p1;
	double	m_p2;
	double	m_pixelY;
	double	m_pixelX;
	double	m_x0;
	double	m_y0;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetCameraDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetCameraDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETCAMERADLG_H__A1E6740D_596B_4D50_A537_DFF13C6BA42E__INCLUDED_)
