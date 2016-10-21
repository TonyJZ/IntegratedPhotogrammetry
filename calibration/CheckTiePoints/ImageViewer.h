#if !defined(AFX_IMAGEVIEWER_H__A98FB52E_57EC_4A54_BEC2_25758704FA27__INCLUDED_)
#define AFX_IMAGEVIEWER_H__A98FB52E_57EC_4A54_BEC2_25758704FA27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageViewer.h : header file
//
#include "gui\SingleImageView.h"
/////////////////////////////////////////////////////////////////////////////
// CImageViewer view

class CImageViewer : public CSingleImageView
{
protected:
	DECLARE_DYNCREATE(CImageViewer)

// Attributes
public:
	CImageViewer();           // protected constructor used by dynamic creation
	virtual ~CImageViewer();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageViewer)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
//	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CImageViewer)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEVIEWER_H__A98FB52E_57EC_4A54_BEC2_25758704FA27__INCLUDED_)
