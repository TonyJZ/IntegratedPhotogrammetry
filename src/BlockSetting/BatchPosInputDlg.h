#if !defined(AFX_BATCHPOSINPUTDLG_H__240CB62A_33AF_4B2B_A77C_25F62881D9CE__INCLUDED_)
#define AFX_BATCHPOSINPUTDLG_H__240CB62A_33AF_4B2B_A77C_25F62881D9CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BatchPosInputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBatchPosInputDlg dialog
#include "resource.h"
#include "GridCtrl/GridCtrl_src/GridCtrl.h"
//#include "IPhBaseDef.h"
#include "..\..\include\ALMPrjManager.h"

class CBatchPosInputDlg : public CDialog
{
// Construction
public:
	CBatchPosInputDlg(CALMPrjManager *pBlockInfo, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBatchPosInputDlg)
	enum { IDD = IDD_DIALOG_BatchPOS };
	int		m_RotateUnit;
	int		m_RotateSystem;
	CString	m_strPosPath;
	//}}AFX_DATA
	
	CALMPrjManager *m_pBlockInfo;
	CGridCtrl	m_Grid;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchPosInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchPosInputDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBUTTONCoordSysTrans();
	afx_msg void OnBUTTONOpenPOS();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHPOSINPUTDLG_H__240CB62A_33AF_4B2B_A77C_25F62881D9CE__INCLUDED_)
