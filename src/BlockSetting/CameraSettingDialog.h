#if !defined(AFX_CAMERASETTINGDIALOG_H__17777C52_3EDF_440E_A800_EE2ACC9FCBC1__INCLUDED_)
#define AFX_CAMERASETTINGDIALOG_H__17777C52_3EDF_440E_A800_EE2ACC9FCBC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CameraSettingDialog.h : header file
//	2010.4.13	zj	对话框方式设置像机参数只对当前选中的像机进行修改
//

/////////////////////////////////////////////////////////////////////////////
// CCameraSettingDialog dialog
#include "resource.h"
class CCameraSettingDialog : public CDialog
{
// Construction
public:
	CCameraSettingDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCameraSettingDialog)
	enum { IDD = IDD_DIALOG_CAMERA };
	double	m_f;
	int		m_ImgHei;
	int		m_ImgWid;
	double	m_k1;
	double	m_k2;
	double	m_k3;
	double	m_p1;
	double	m_p2;
	double	m_PixelHei;
	double	m_PixelWid;
	double	m_x0;
	double	m_y0;
	int		m_Option;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCameraSettingDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCameraSettingDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAMERASETTINGDIALOG_H__17777C52_3EDF_440E_A800_EE2ACC9FCBC1__INCLUDED_)
