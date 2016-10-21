// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__BF5F39C0_8223_4C00_AA8E_EA7EB87E6F20__INCLUDED_)
#define AFX_MAINFRM_H__BF5F39C0_8223_4C00_AA8E_EA7EB87E6F20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Align_def.h"
//#include "LayerTreeBar.h"
#include "orsGuiBase/orsIFrameWndHelper.h"

#define CFrameWnd CBCGPFrameWnd

class CMainFrame : public orsIFrameWndHelper<CFrameWnd>
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	void DisplayCoord( double x, double y, double z, float coef, double Xt, double Yt, double Zt );

	void AttachLayerCollection( orsILayerCollection *pLayerCollection );

// 	CLayerTreeBar &GetLayerTreeBar()	{
// 		return m_layerTreeBar;
// 	}

	UINT m_linkOn;
//	TieObj_Type	m_tieType;
	int    m_tieID;
//	HWND  m_AlgWindow;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
//	CStatusBar  m_wndStatusBar;
//	CToolBar    m_wndToolBar;

//	CLayerTreeBar	m_layerTreeBar;

	bool   m_LayerAttached;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnTiePatch();
	afx_msg void OnTieLine();
	afx_msg void OnTiePOINT();
	afx_msg void OnUpdateTiePOINT(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTieLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTiePatch(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CMainFrame* GetMainFrame();
#define  RegistWINDOW_TEXT "RegistViewer"


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BF5F39C0_8223_4C00_AA8E_EA7EB87E6F20__INCLUDED_)
