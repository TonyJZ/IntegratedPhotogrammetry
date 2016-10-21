//*************************************************************************
//
// Copyright (C) 2008, LIESMARS, Wuhan University
//
// License:  LGPL
//
//*************************************************************************
//
// $Id: LayerTreeBar.cpp 2009/02/20 17:33 JWS $
//
// Project: OpenRS 
//
// Purpose:  
//
// Author: JIANG Wanshou, YAO Huang, jws@lmars.whu.edu.cn
//
//*************************************************************************
//
// $Log: LayerTreeBar.cpp,v $
//
// Revision 1.0 date: 2009/02/20 by JIANG Wanshou
// new
//
// LayerTreeBar.cpp : implementation of the CLayerTreeBar class
//

#include "stdafx.h"
//#include "orsViewer.h"
#include "LayerTreeBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;

/////////////////////////////////////////////////////////////////////////////
// CLayerTreeBar

BEGIN_MESSAGE_MAP(CLayerTreeBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CLayerTreeBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerTreeBar construction/destruction

CLayerTreeBar::CLayerTreeBar()
{
	// TODO: add one-time construction code here
}

CLayerTreeBar::~CLayerTreeBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLayerTreeBar message handlers

int CLayerTreeBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect oRect(50,50,200,200 );
	
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
								TVS_LINESATROOT | TVS_HASBUTTONS;
	
// 	WS_CHILD | WS_VISIBLE | TVS_EDITLABELS |
// 		TVS_HASLINES|TVS_HASBUTTONS | TVS_LINESATROOT | TVS_CHECKBOXES |
// 		TVS_SHOWSELALWAYS 

	m_layerTree.Create( dwViewStyle, CRect(0, 0, 0, 0), this, 5000);

	//////////////////////////////////////////////////////////////////////////
	// first make the root node for the entire tree
	//


	return 0;
}



void CLayerTreeBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tab control should cover a whole client area:
	m_layerTree.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CLayerTreeBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_layerTree.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
								::GetSysColor (COLOR_3DSHADOW));
}




void CLayerTreeBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
}
