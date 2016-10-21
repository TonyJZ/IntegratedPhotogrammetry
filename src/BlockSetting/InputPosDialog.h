#if !defined(AFX_INPUTPOSDIALOG_H__DF099614_1C2B_484B_B177_5AD717BF1036__INCLUDED_)
#define AFX_INPUTPOSDIALOG_H__DF099614_1C2B_484B_B177_5AD717BF1036__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputPosDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputPosDialog dialog
#include "resource.h"
#include "..\..\include\ALMPrjManager.h"

//角度单位：0-360	1-400	2-弧度
//转角系统：0-YXZ	1-XYZ
//平面单位：0-meters	1-degrees	2-radians
//高程单位：0-meters
class CInputPosDialog : public CDialog
{
// Construction
public:
	CInputPosDialog(iphCamera *camera, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputPosDialog)
	enum { IDD = IDD_DIALOG_POS };
	double	m_Kappa;
	double	m_Omega;
	double	m_Phi;
	double	m_Xs;
	double	m_Ys;
	double	m_Zs;
	int		m_RotateUnit;
	int		m_RotateSystem;
//	int		m_HorizontalUnit;
//	int		m_VerticalUnit;
	//}}AFX_DATA
	
	CString			m_strHorizontalDatum;
	CString			m_strVerticalDatum;
	iphUnit			m_PosUnitH;
	iphUnit			m_PosUnitV;
	iphUnit			m_AngleUnit;
	iphRotateSys	m_RotateSys;

	iphCamera	*m_pCamera;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputPosDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputPosDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTPOSDIALOG_H__DF099614_1C2B_484B_B177_5AD717BF1036__INCLUDED_)
