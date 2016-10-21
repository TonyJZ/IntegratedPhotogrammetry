#if !defined(AFX_IMGVIEWERDLG_H__0178C9E1_03CA_4998_9DA9_875D7891440E__INCLUDED_)
#define AFX_IMGVIEWERDLG_H__0178C9E1_03CA_4998_9DA9_875D7891440E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// imgviewerdlg.h : header file
//
#include "Resource.h"
#include "imageviewer.h"
#include "ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CImgViewerDlg dialog

class CImgViewerDlg : public CResizableDialog
{
// Construction
public:
	CImgViewerDlg( const orsChar *titleStr, ref_ptr<orsIImageSource> pImg, CWnd* pParent = NULL);   // standard constructor
	~CImgViewerDlg();

	void DisplayImage(const orsChar *titleStr, ref_ptr<orsIImageSource> pImg);

	void drawPoints(orsPOINT2D &pts2d);
	void drawPoints(orsPOINT3D &pts3d);

	bool	m_bQuit;

// Dialog Data
	//{{AFX_DATA(CImgViewerDlg)
	enum { IDD = IDD_ImageViewerDlg };
//	CStatic	m_wndImgViewer;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgViewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
private:
	HICON       m_hIcon;
	ref_ptr<orsIImageSource> m_pImg;
	ref_ptr<orsIImageChain>  m_imageChain;

	const orsChar *m_titleStr;

private:
	
	void SetImageSource( ref_ptr<orsIImageSource> pImg );

// Implementation
protected:

	CImageViewer   *m_pView;
	
	// Generated message map functions
	//{{AFX_MSG(CImgViewerDlg)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnFileOpen();
	afx_msg void OnOk();
	afx_msg void OnClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGVIEWERDLG_H__0178C9E1_03CA_4998_9DA9_875D7891440E__INCLUDED_)
