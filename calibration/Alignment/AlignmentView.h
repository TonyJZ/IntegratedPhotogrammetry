// AlignmentView.h : interface of the CAlignmentView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALIGNMENTVIEW_H__CDA26609_F7D1_44E2_8559_3491A3C6225A__INCLUDED_)
#define AFX_ALIGNMENTVIEW_H__CDA26609_F7D1_44E2_8559_3491A3C6225A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAlignmentView : public CView
{
protected: // create from serialization only
	CAlignmentView();
	DECLARE_DYNCREATE(CAlignmentView)

// Attributes
public:
	CAlignmentDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlignmentView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAlignmentView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAlignmentView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AlignmentView.cpp
inline CAlignmentDoc* CAlignmentView::GetDocument()
   { return (CAlignmentDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIGNMENTVIEW_H__CDA26609_F7D1_44E2_8559_3491A3C6225A__INCLUDED_)
