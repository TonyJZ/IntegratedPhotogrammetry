#if !defined(AFX_IMGSETTINGPAGE_H__B38CC871_66DA_4F21_B88C_4C4095859956__INCLUDED_)
#define AFX_IMGSETTINGPAGE_H__B38CC871_66DA_4F21_B88C_4C4095859956__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImgSettingPage.h : header file
//
#include "resource.h"
#include "PropertiesTabCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CImgSettingPage dialog

class CImgSettingPage : public CPropertiesTabPage
{
// Construction
public:
	CImgSettingPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImgSettingPage)
	enum { IDD = IDD_ImageSettingDlg };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	virtual void OnActivate();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgSettingPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CImgSettingPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGSETTINGPAGE_H__B38CC871_66DA_4F21_B88C_4C4095859956__INCLUDED_)
