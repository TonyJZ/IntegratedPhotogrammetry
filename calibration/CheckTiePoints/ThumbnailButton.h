//  *************************************************************************
//  *                                                                       *
//  *  CThumbnailButton     of FabricEye                 ed. 1.0  17/9/2002 *
//  *                                                                       *
//  *************************************************************************
//
//  DESCRIPTION     :
//
//  -------------------------------------------------------------------------
//
//  INHERITANCE     :
//
//  -------------------------------------------------------------------------
//
//  IMPLEMENTATION  : ThumbnailButton.cpp
//
//  -------------------------------------------------------------------------
//
//  COMMENTS        :
//
//  -------------------------------------------------------------------------
//
//  REVISION        : Edition 1.0  17/9/2002/RFO    Layout
//
//  -------------------------------------------------------------------------
//
//  RFO  Rex Fong

#if !defined(AFX_THUMBNAILBUTTON_H__65393EA8_3BAD_42E1_9B27_E275568D5063__INCLUDED_)
#define AFX_THUMBNAILBUTTON_H__65393EA8_3BAD_42E1_9B27_E275568D5063__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThumbnailButton.h : header file
//

#define ON_TNB_LCLICKED _T( "WM_ON_TNB_LCLICKED_{D190C85B-217C-4a91-8684-0C523559C67D}" )
#define ON_TNB_RCLICKED _T( "WM_ON_TNB_RCLICKED_{0BBF61D0-8379-4470-B30A-C11854B9938C}" )

static const UINT UWM_ON_TNB_LCLICKED = ::RegisterWindowMessage( ON_TNB_LCLICKED );
static const UINT UWM_ON_TNB_RCLICKED = ::RegisterWindowMessage( ON_TNB_RCLICKED );

/////////////////////////////////////////////////////////////////////////////
// CThumbnailButton window

class CThumbnailButton : public CWnd
  {
    static CBrush  m_bkBrush;

	//****************
	//*  Attributes  *
	//****************
  private:
    BOOL    m_bTracking;
    BOOL    m_bMouseClicked;
    CString m_sFilename;

    CBitmap m_bmp;

    int     m_cX;     // Image Width
    int     m_cY;     // Image Height

    CString m_sFullpath;

//	double m_x, m_y;	//连接点像素坐标

	//********************************
	//*  Construction / Destruction  *
	//********************************
  public:
	  CThumbnailButton( const CString& sPath, const int cx, const int cy, HBITMAP hBmp );
	  virtual ~CThumbnailButton();

	//****************
	//*  Operations  *
	//****************
  private:
	  void ExtractFilename( const CString& sPath );
    BOOL RegisterWindowClass(void);

  public:
    void  ResetTrackFlag( void );
	  const CString& GetFullpath( void ) const;
	  BOOL IsValid( void ) const;
    

	//***************
	//*  Overrides	*
	//***************
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CThumbnailButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//*************************************
	//*  Generated message map functions	*
	//*************************************
  protected:
	//{{AFX_MSG(CThumbnailButton)
  afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	  DECLARE_MESSAGE_MAP()

  };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THUMBNAILBUTTON_H__65393EA8_3BAD_42E1_9B27_E275568D5063__INCLUDED_)
