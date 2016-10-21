#if !defined(AFX_IMGSETTINGPAGE_H__B38CC871_66DA_4F21_B88C_4C4095859956__INCLUDED_)
#define AFX_IMGSETTINGPAGE_H__B38CC871_66DA_4F21_B88C_4C4095859956__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImgSettingPage.h : header file
//
#include "resource.h"
#include "TabCtrl/PropertiesTabCtrl.h"
#include "GridCtrl/GridCtrl_src/GridCtrl.h"
#include "..\..\include\AlmPrjManager.h"
/////////////////////////////////////////////////////////////////////////////
// CImgSettingPage dialog

class CImgSettingPage : public CPropertiesTabPage
{
// Construction
public:
	CImgSettingPage(CWnd* pParent = NULL);   // standard constructor
	~CImgSettingPage();
// Dialog Data
	//{{AFX_DATA(CImgSettingPage)
	enum { IDD = IDD_ImageSettingDlg };
	BOOL	m_bCopyImg;
	//}}AFX_DATA

	virtual void OnActivate();

//	bool		m_bImage;
	CGridCtrl	m_Grid;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgSettingPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void	UpdataImageState();

// Implementation
protected:

	void	AddImageTable(int startPos);
	int  	GetSelectedRow();		//取当前选中的行,将行号存入m_pSelRowBuf,返回行数

	int *m_pSelRowBuf;
	int m_SelBufLen;
	// Generated message map functions
	//{{AFX_MSG(CImgSettingPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddImage();
	afx_msg void OnAddCamera();
	afx_msg void OnAddPOS();
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnCHECKCopyImg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CALMPrjManager	*m_pBlockInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGSETTINGPAGE_H__B38CC871_66DA_4F21_B88C_4C4095859956__INCLUDED_)
