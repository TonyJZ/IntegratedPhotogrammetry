#if !defined(AFX_COORDSYSTRANSDLG_H__3CE4A166_447E_413C_A316_2AAD808A84A6__INCLUDED_)
#define AFX_COORDSYSTRANSDLG_H__3CE4A166_447E_413C_A316_2AAD808A84A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CoordSysTransDlg.h : header file
//
#include "resource.h"
#include "..\..\include\ALMPrjManager.h"
/////////////////////////////////////////////////////////////////////////////
// CCoordSysTransDlg dialog

class CCoordSysTransDlg : public CDialog
{
// Construction
public:
	CCoordSysTransDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCoordSysTransDlg)
	enum { IDD = ID_CoordSysTransDlg };
	CComboBox	m_wndVerticalUnit;
	CComboBox	m_wndHorizontalUnit;
	CString	m_strHorizontalDatum;
	CString	m_strVerticalDatum;
	//}}AFX_DATA

	iphUnit			m_PosUnitH;
	iphUnit			m_PosUnitV;
	iphUnit			m_AngleUnit;
	iphRotateSys	m_RotateSys;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoordSysTransDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCoordSysTransDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COORDSYSTRANSDLG_H__3CE4A166_447E_413C_A316_2AAD808A84A6__INCLUDED_)
