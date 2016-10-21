#if !defined(AFX_BLOCKSETTINGDLG_H__4E5063B3_820C_4125_BF86_758A7A42BC38__INCLUDED_)
#define AFX_BLOCKSETTINGDLG_H__4E5063B3_820C_4125_BF86_758A7A42BC38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlockSettingDlg.h : header file
//
//#include "resource.h"
#include "..\..\include\ALMPrjManager.h"
#include "ImgSettingPage.h"
#include "lidarsettingpage.h"
//#include "CoordSysPage.h"
//#include "ProjectionPage.h"
#include "ModelSpecificPage.h"
#include "TabCtrl/PropertiesTabCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CBlockSettingDlg dialog

#ifdef _BLOCKSETTING_
#define  _Blk_Setting_  __declspec(dllexport)
#else
#define  _Blk_Setting_  __declspec(dllimport)	
#endif

#ifndef _BLOCKSETTING_
#ifdef _DEBUG
#pragma comment(lib,"BlockSettingD.lib")
#else
#pragma comment(lib,"BlockSetting.lib")
#endif
#endif

enum Current_Tab {ImagePage=0, LidarPage, /*ProjectionPage,*/ ModelSpecificPage};

class _Blk_Setting_ CBlockSettingDlg : public CDialog
{
// Construction
public:
	CBlockSettingDlg(CALMPrjManager *pBlockInfo, int nTab = ImagePage, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBlockSettingDlg)
	enum { IDD = IDD_BlockSetting };
	CPropertiesTabCtrl	m_wndTabCtrl;	
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CImgSettingPage		m_wndPageImg;
	CLidarSettingPage	m_wndPageLidar;
//	CCoordSysPage		m_wndPageCoordSys;
//	CProjectionPage		m_wndPageProjection;
	CModelSpecificPage	m_wndPageModelSpecific;
	
	int				m_nTab;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlockSettingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CALMPrjManager	*m_pBlockInfo;
	CALMPrjManager	m_newBlockInfo;
	// Generated message map functions
	//{{AFX_MSG(CBlockSettingDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOCKSETTINGDLG_H__4E5063B3_820C_4125_BF86_758A7A42BC38__INCLUDED_)
