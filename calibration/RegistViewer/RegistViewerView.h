// RegistViewerView.h : interface of the CRegistViewerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTVIEWERVIEW_H__5B75F238_4718_42E6_80B2_C612FCE9667A__INCLUDED_)
#define AFX_REGISTVIEWERVIEW_H__5B75F238_4718_42E6_80B2_C612FCE9667A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gui\SingleImageView.h"
#include "Align_def.h"

#include "MemDC.h"

class CRegistViewerDoc;

class CRegistViewerView : public CSingleImageView 
{
protected: // create from serialization only
	CRegistViewerView();
	DECLARE_DYNCREATE(CRegistViewerView)

// Attributes
public:
	CRegistViewerDoc* GetDocument();
	
	virtual void DisplayCoordinates(double X, double Y, double Z, float coef, double Xt, double Yt, double Zt );
	
	void DrawTieObjects(CDC *pDC);
	void DrawTiePoint(CDC *pDC, TiePoint *pObj);
	void DrawTieLine(CDC *pDC, TieLine *pObj);
	void DrawTiePatch(CDC *pDC, TiePatch *pObj);

	CArray<TieObject*, TieObject*> m_TObjList;
	TieObject* m_pCurObj;

	POINT2D	*m_pt2DBuf;
	int      m_ptNum;

	bool  m_bMeasureFlag;
	bool  m_bQueryPoint;

	void	EndMeasure();

private:
	enum opMySTATE	{
		opSWIPE_X = opPICK + 1,
		opSWIPE_Y = opPICK + 2,
		opRuler_LEN = opPICK + 3
	};

	CMemDC m_memLayers[3];

// Operations
public:
	orsILayerCollection *m_pLayerCollection;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistViewerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRegistViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
		void PrepareSwipeMemDC();

protected:
//	virtual void DrawYourContents();

// Generated message map functions
protected:
	//{{AFX_MSG(CRegistViewerView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMeasureFlag();
	afx_msg void OnUpdateMeasureFlag(CCmdUI* pCmdUI);
	afx_msg void OnQueryPoint();
	afx_msg void OnUpdateQueryPoint(CCmdUI* pCmdUI);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLayerSwipeHorizontal();
	afx_msg void OnUpdateLayerSwipeHorizontal(CCmdUI* pCmdUI);
	afx_msg void OnLayerSwipeVertical();
	afx_msg void OnUpdateLayerSwipeVertical(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in RegistViewerView.cpp
inline CRegistViewerDoc* CRegistViewerView::GetDocument()
   { return (CRegistViewerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGISTVIEWERVIEW_H__5B75F238_4718_42E6_80B2_C612FCE9667A__INCLUDED_)
