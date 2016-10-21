#if !defined(AFX_OUTPUTVIEW_H__3CBB380E_C45E_4DDA_AEF1_93DE7D35A018__INCLUDED_)
#define AFX_OUTPUTVIEW_H__3CBB380E_C45E_4DDA_AEF1_93DE7D35A018__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputView.h : header file
//
#include "resource.h"
#include "OutputEdit.h"
//#include "ALMGuiEvent/ALMIOutputMsgEvent.h"
/////////////////////////////////////////////////////////////////////////////
// COutputView dialog

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define		IDD_PSEUDO_DIALOG     100007

class COutputView : public CBCGPFormView
{
// Construction
public:
	COutputView(CWnd* pParent = NULL);   // standard constructor
	DECLARE_DYNCREATE(COutputView)
// Dialog Data
	//{{AFX_DATA(COutputView)
	enum { IDD = IDD_OUTPUTVIEW_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
//	ALMOutputMsgEventHandler	m_outputMsg;		
//	ors_int32					m_outputMsgCookie;

//	void OnOutputMsg( alm_OutputStyle style, char* msg, alm_guiScrollPos pos );

// Implementation
protected:

	COutputEdit	m_wndOutput;

	// Generated message map functions
	//{{AFX_MSG(COutputView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTVIEW_H__3CBB380E_C45E_4DDA_AEF1_93DE7D35A018__INCLUDED_)
