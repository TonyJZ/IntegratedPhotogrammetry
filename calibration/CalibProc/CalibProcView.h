// CalibProcView.h : interface of the CCalibProcView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALIBPROCVIEW_H__47330007_7995_47E5_89CA_6C2574C38002__INCLUDED_)
#define AFX_CALIBPROCVIEW_H__47330007_7995_47E5_89CA_6C2574C38002__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CCalibProcView : public CView
{
protected: // create from serialization only
	CCalibProcView();
	DECLARE_DYNCREATE(CCalibProcView)

// Attributes
public:
	CCalibProcDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalibProcView)
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
	virtual ~CCalibProcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCalibProcView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CalibProcView.cpp
inline CCalibProcDoc* CCalibProcView::GetDocument()
   { return (CCalibProcDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALIBPROCVIEW_H__47330007_7995_47E5_89CA_6C2574C38002__INCLUDED_)
