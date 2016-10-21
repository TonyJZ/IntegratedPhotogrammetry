#if !defined(AFX_INORSETTINGDLG_H__9F691988_7387_48FD_857C_7C3829E489D4__INCLUDED_)
#define AFX_INORSETTINGDLG_H__9F691988_7387_48FD_857C_7C3829E489D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InorSettingDlg.h : header file
//
#include "resource.h"

#include "TabCtrl/PropertiesTabCtrl.h"
#include "GridCtrl/GridCtrl_src/GridCtrl.h"

#include "orsBase/orsTypedef.h"
#include "orsBase/orsString.h"
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// CInorSettingDlg dialog

class _iphCamera;

class CInorSettingDlg : public CDialog
{
// Construction
public:
	CInorSettingDlg(std::vector<orsString>	*srcImgVec, _iphCamera *camera, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInorSettingDlg)
	enum { IDD = IDD_InorSettingDLG };
	CString	m_strOutputDir;
	//}}AFX_DATA

	CGridCtrl	m_Grid;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInorSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


private:
		void AddImageTable(int startPos);


private:
	std::vector<orsString>	*m_srcImgVec;
	_iphCamera				*m_camera;

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInorSettingDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddImage();
	afx_msg void OnLoadCamera();
	afx_msg void OnSetCamera();
	afx_msg void OnSetOutputDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INORSETTINGDLG_H__9F691988_7387_48FD_857C_7C3829E489D4__INCLUDED_)
