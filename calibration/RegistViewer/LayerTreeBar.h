//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: LayerTreeBar.h 2009/02/20 17:34 JWS $
//
// Project: OpenRS 
//
// Purpose:  
//
// Author: JIANG Wanshou, YAO Huang, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: LayerTreeBar.h,v $
//
// Revision 1.0 date: 2009/02/20 by JIANG Wanshou
// new
//

#if !defined(AFX_LayerTreeBar_H__E0E700B4_D7E0_472E_803E_CA2D6A042A30__INCLUDED_)
#define AFX_LayerTreeBar_H__E0E700B4_D7E0_472E_803E_CA2D6A042A30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "layerTreeControl.h"
class CLayerTreeBar : public CBCGPDockingControlBar
{
public:
	CLayerTreeBar();

	CLayerTreeCtrl &GetLayerTree()	{ return m_layerTree;	}

	void AttachLayerCollection( orsILayerCollection *pLayerCollection )
	{
		m_layerTree.AttachLayerCollection( pLayerCollection );		
	}

// Attributes
protected:
	CLayerTreeCtrl m_layerTree;
//CTreeCtrl	m_wndTree;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerTreeBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLayerTreeBar();

// Generated message map functions
protected:
	//{{AFX_MSG(CLayerTreeBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LayerTreeBar_H__E0E700B4_D7E0_472E_803E_CA2D6A042A30__INCLUDED_)
