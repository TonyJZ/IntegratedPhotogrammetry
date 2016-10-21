// ImageMatchViewView.h : interface of the CImageMatchViewView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEMATCHVIEWVIEW_H__A4D3D5B5_D927_48C6_99F4_84C67419367B__INCLUDED_)
#define AFX_IMAGEMATCHVIEWVIEW_H__A4D3D5B5_D927_48C6_99F4_84C67419367B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CImageMatchViewView : public CView
{
protected: // create from serialization only
	CImageMatchViewView();
	DECLARE_DYNCREATE(CImageMatchViewView)

// Attributes
public:
	CImageMatchViewDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageMatchViewView)
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
	virtual ~CImageMatchViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CImageMatchViewView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ImageMatchViewView.cpp
inline CImageMatchViewDoc* CImageMatchViewView::GetDocument()
   { return (CImageMatchViewDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEMATCHVIEWVIEW_H__A4D3D5B5_D927_48C6_99F4_84C67419367B__INCLUDED_)
