//  *************************************************************************
//  *                                                                       *
//  *  CThumbNailControl    of ThumbNail                ed. 1.0  23/10/2002 *
//  *                                                                       *
//  *************************************************************************
//
//  DESCRIPTION     : Thumbnail Control
//
//  -------------------------------------------------------------------------
//
//  INHERITANCE     : CWnd
//
//  -------------------------------------------------------------------------
//
//  COMMENTS        :
//
//    You can modify, revise & distribute any part of this piece of code 
//    provided that you will retain this header.
//
//  -------------------------------------------------------------------------
//
//  REVISION        : Edition 1.0  23/10/2002/RFO    Layout
//
//  -------------------------------------------------------------------------
//
//  RFO  Rex Fong (rexfong@bac98.net)
//
#if !defined(AFX_THUMBNAILCONTROL_H__EA2BDCEA_556B_46D0_9C14_B68E4BC7F4A1__INCLUDED_)
#define AFX_THUMBNAILCONTROL_H__EA2BDCEA_556B_46D0_9C14_B68E4BC7F4A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThumbNailControl.h : header file
//

#include "ThumbnailButton.h"

/////////////////////////////////////////////////////////////////////////////
// CThumbNailControl window


class CThumbNailControl : public CWnd
  {
	//*****************
	//*  Enumeration	*
	//*****************
  enum { DEFAULT_THUMBWIDTH  = 100, 
         DEFAULT_THUMBHEIGHT = 100,
         DEFAULT_SEPERATOR   = 4 };

	//****************
	//*  Attributes  *
	//****************
  private:
    static CBrush     m_bkBrush;

    CPtrArray         m_arPtrData;

    long              m_nStartX;
    long              m_nStartY;

    long              m_nCol;
    long              m_nRow;

    long              m_nThumbWidth;
    long              m_nThumbHeight;

	//*********************************
	//*  Construction / Destruction 	*
	//*********************************
  public:
	  CThumbNailControl();
	  virtual ~CThumbNailControl();

	  void clear();

	//****************
	//*  Operations  *
	//****************
  private:
	  void RecalScrollBars( void );
    BOOL RegisterWindowClass();
	  void RecalButtonPos( void );

  protected:
	  int GetScrollPos32(int nBar, BOOL bGetTrackPos = FALSE );
	  BOOL SetScrollPos32( int nBar, int nPos, BOOL bRedraw = TRUE );

  public:
    void Add( const CString& sPath, HBITMAP hBmp );
	  void InitializeVariables( int cX = DEFAULT_THUMBWIDTH, int cY = DEFAULT_THUMBHEIGHT);

	//***************
	//*  Overrides	*
	//***************
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CThumbNailControl)
    //}}AFX_VIRTUAL

	//*************************************
	//*  Generated message map functions	*
	//*************************************
  protected:
    //{{AFX_MSG(CThumbNailControl)
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THUMBNAILCONTROL_H__EA2BDCEA_556B_46D0_9C14_B68E4BC7F4A1__INCLUDED_)
