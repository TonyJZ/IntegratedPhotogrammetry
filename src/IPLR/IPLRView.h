// IPLRView.h : interface of the CIPLRView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPLRVIEW_H__E9771F4A_BB8D_4FB9_A20F_95004B0CFB27__INCLUDED_)
#define AFX_IPLRVIEW_H__E9771F4A_BB8D_4FB9_A20F_95004B0CFB27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CIPLRView : public CView
{
protected: // create from serialization only
	CIPLRView();
	DECLARE_DYNCREATE(CIPLRView)

// Attributes
public:
	CIPLRDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPLRView)
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
	virtual ~CIPLRView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CIPLRView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in IPLRView.cpp
inline CIPLRDoc* CIPLRView::GetDocument()
   { return (CIPLRDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPLRVIEW_H__E9771F4A_BB8D_4FB9_A20F_95004B0CFB27__INCLUDED_)
