// PhotoLocationView.h : interface of the CPhotoLocationView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHOTOLOCATIONVIEW_H__982C7338_A8AC_4D6F_BFAD_A4DAD9954051__INCLUDED_)
#define AFX_PHOTOLOCATIONVIEW_H__982C7338_A8AC_4D6F_BFAD_A4DAD9954051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "3DView.h"

class CPhotoLocationView : public C3DView
{
protected: // create from serialization only
	CPhotoLocationView();
	DECLARE_DYNCREATE(CPhotoLocationView)

// Attributes
public:
	CPhotoLocationDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhotoLocationView)
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
	virtual ~CPhotoLocationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPhotoLocationView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PhotoLocationView.cpp
inline CPhotoLocationDoc* CPhotoLocationView::GetDocument()
   { return (CPhotoLocationDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHOTOLOCATIONVIEW_H__982C7338_A8AC_4D6F_BFAD_A4DAD9954051__INCLUDED_)
