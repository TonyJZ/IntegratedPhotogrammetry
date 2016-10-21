// ImageViewerVIEW.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ImageViewerVIEW_H__F3387FE6_6D02_4C97_B018_561AFAF28CB0__INCLUDED_)
#define AFX_ImageViewerVIEW_H__F3387FE6_6D02_4C97_B018_561AFAF28CB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gui\SingleImageView.h"
//#include "PointTree\PointLayer.h"
//#include "PointTree\PointIndex.h"

/////////////////////////////////////////////////////////////////////////////
// CImageViewerView view
typedef struct
{
	float	x;
	float	y;
	float	scale;
	float	orientation;
} KeyPoint;

class CImageViewerView : public CSingleImageView 
{
private:	// test data
	bool	m_bEnd;
	int		m_pSum;
	double m_fRotateAngle;
	orsIImageGeometry *m_lRay;
	int m_iCPtNum;

//	CDialogBrightnessContrast *m_pDlgBrightContrast;

private:
//	CArray<CONTROLPOINT,CONTROLPOINT> *m_aryGCPData;
public:
//	void SetGCPData(CArray<CONTROLPOINT,CONTROLPOINT> *aryGCPData){m_aryGCPData = aryGCPData;}
public:
	CImageViewerView();           // protected constructor used by dynamic creation
	virtual ~CImageViewerView();
	DECLARE_DYNCREATE(CImageViewerView)

// Attributes
public:
	BOOL m_bCanUndoRedo;

// Operations
public:	

	// ×ø±êÏµ
/*
	virtual BOOL OnDPtoLP(CPoint ptDP, CPoint& ptLP);
	virtual BOOL OnLPtoRP(CPoint ptLP, CPoint3D& ptRP, int nFlag = 0);
	virtual BOOL OnDPtoRP(CPoint ptDP, CPoint3D& ptRP);
	virtual BOOL OnDPtoRP(long lFuzzyDP, long& lFuzzyRP);
*/

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageViewerView)
	public:

	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

public:
	int		m_SelectPointNo;
	bool	m_bSelPoint;
protected:
	int		m_KPnum;
	KeyPoint	*m_pKPbuf;

// Implementation
protected:

	virtual void DrawYourContents();
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CImageViewerView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ImageViewerVIEW_H__F3387FE6_6D02_4C97_B018_561AFAF28CB0__INCLUDED_)
