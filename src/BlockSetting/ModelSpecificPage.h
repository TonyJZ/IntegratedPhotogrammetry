#if !defined(AFX_MODELSPECIFICPAGE_H__56BEDC83_4728_4C5D_B773_7A4D6C9AEA3B__INCLUDED_)
#define AFX_MODELSPECIFICPAGE_H__56BEDC83_4728_4C5D_B773_7A4D6C9AEA3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelSpecificPage.h : header file
//
#include "resource.h"
#include "TabCtrl/PropertiesTabCtrl.h"
#include "..\..\include\ALMPrjManager.h"
/////////////////////////////////////////////////////////////////////////////
// CModelSpecificPage dialog

class CModelSpecificPage : public CPropertiesTabPage
{
// Construction
public:
	CModelSpecificPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelSpecificPage)
	enum { IDD = IDD_ModelSpecificDlg };
	double	m_dMinHeight;
	double	m_dMaxHeight;
	short	m_flyoverlap;
	short	m_stripoverlap;
	//}}AFX_DATA
	
	virtual void OnActivate();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelSpecificPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelSpecificPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:
	CALMPrjManager	*m_pBlockInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELSPECIFICPAGE_H__56BEDC83_4728_4C5D_B773_7A4D6C9AEA3B__INCLUDED_)
