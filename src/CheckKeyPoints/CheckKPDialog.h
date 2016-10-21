#if !defined(AFX_CHECKKPDIALOG_H__B5E5E5A1_3B94_45A2_8BAD_D870759B3FA8__INCLUDED_)
#define AFX_CHECKKPDIALOG_H__B5E5E5A1_3B94_45A2_8BAD_D870759B3FA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckKPDialog.h : header file
//

#include "resource.h"
#include "orsBase/orsIPlatform.h"
//#include "orsBase\orsUtil.h"
#include "gui\SingleImageView.h"
#include "orsImage\orsIImageSourceReader.h"
#include "ResizableDialog.h"
#include "imageviewerView1.h"

#include "orsImage\orsIImageSourceReader.h"
/////////////////////////////////////////////////////////////////////////////
// CCheckKPDialog dialog

class CCheckKPDialog : public CResizableDialog
{
// Construction
public:
	CCheckKPDialog(orsIPlatform* platform,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCheckKPDialog)
	enum { IDD = IDD_CheckKPDIALOG };
	CListCtrl	m_ListCtrl;
	//}}AFX_DATA


	orsIPlatform*  m_pPlatform;
	ref_ptr<orsIImageSource> m_pImg;
	orsIImageData* m_pImgData;
	ref_ptr<orsIImageSourceReader> m_ImageReader;
	ref_ptr<orsIImageChain> m_imageChain;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckKPDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


public:
	bool m_bCPtFileOpen;
	int		m_KPnum;
	KeyPoint	*m_pKPbuf;

	HICON       m_hIcon;

// Implementation
protected:
	CImageViewerView   *m_pView;
	void	SetKeyPointsList();

	// Generated message map functions
	//{{AFX_MSG(CCheckKPDialog)
	afx_msg void OnOpenKP();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnOpenImage();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClickListCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnImageViewer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifdef _CHECK_KEY_POINT_DLG_
#define  _CHECK_KPDLG_  __declspec(dllexport)
#else
#define  _CHECK_KPDLG_  __declspec(dllimport)	
#endif

void _CHECK_KPDLG_ CheckKeyPoints(orsIPlatform*  pPlatform);

#ifndef _CHECK_KEY_POINT_DLG_
#ifdef _DEBUG
#pragma comment(lib,"CheckPointDlgD.lib")
#else
#pragma comment(lib,"CheckPointDlg.lib")
#endif
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKKPDIALOG_H__B5E5E5A1_3B94_45A2_8BAD_D870759B3FA8__INCLUDED_)
