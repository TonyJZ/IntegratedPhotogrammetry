#if !defined(AFX_LIDARSETTINGPAGE_H__4A94AD80_F9B9_49AE_A793_BB12670D3058__INCLUDED_)
#define AFX_LIDARSETTINGPAGE_H__4A94AD80_F9B9_49AE_A793_BB12670D3058__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LidarSettingPage.h : header file
//
#include "resource.h"
#include "TabCtrl/PropertiesTabCtrl.h"
#include "GridCtrl/GridCtrl_src/GridCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CLidarSettingPage dialog
class CALMPrjManager; 

class CLidarSettingPage : public CPropertiesTabPage
{
// Construction
public:
	CLidarSettingPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLidarSettingPage)
	enum { IDD = IDD_LidarSettingDlg };
	BOOL	m_bCopyLIDAR;
	//}}AFX_DATA

	virtual void OnActivate();
	
	CGridCtrl	m_Grid;

	void AddLidarFile();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLidarSettingPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLidarSettingPage)
	afx_msg void OnBUTTONCoordTrans();
	afx_msg void OnBUTTONAddLIDAR();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CALMPrjManager	*m_pBlockInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIDARSETTINGPAGE_H__4A94AD80_F9B9_49AE_A793_BB12670D3058__INCLUDED_)
