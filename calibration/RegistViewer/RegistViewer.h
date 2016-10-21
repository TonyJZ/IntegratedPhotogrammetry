// RegistViewer.h : main header file for the REGISTVIEWER application
//

#if !defined(AFX_REGISTVIEWER_H__FF29E270_9327_4857_9544_ACF48740AF7D__INCLUDED_)
#define AFX_REGISTVIEWER_H__FF29E270_9327_4857_9544_ACF48740AF7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Align_def.h"

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerApp:
// See RegistViewer.cpp for the implementation of this class
//

interface orsIPlatform;

class CRegistViewerApp : public CWinApp
{
public:
	CRegistViewerApp();
	~CRegistViewerApp();
	CString GetImageFilterExt();

	int m_sourceID;	//数据条带ID (影像/LiDAR)
	Source_Type m_sourceType;	//lidar, image
	HWND  m_AlgWindow;
	TieObj_Type	m_tieType;
	CString m_strAlgName;
	bool	m_bReproj;	//是否重投影
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistViewerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRegistViewerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTVIEWER_H__FF29E270_9327_4857_9544_ACF48740AF7D__INCLUDED_)
