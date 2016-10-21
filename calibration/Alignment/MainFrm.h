// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__AC1B99F4_9B66_4F8C_AAF6_05EAE76D82DB__INCLUDED_)
#define AFX_MAINFRM_H__AC1B99F4_9B66_4F8C_AAF6_05EAE76D82DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "WorkspaceBar.h"
#include "WorkspaceBar2.h"
#include "OutputBar.h"
#include "AlignPrj.h"

#define CFrameWnd CBCGPFrameWnd

#define  Max_RegistViewers	32

interface LidMC_Plane;
interface LidMC_VP;

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	TieObj_Type			m_tieType;
	int					m_TOMeasureWindows;			//打开的测量进程数
	int					m_RegistViewWindows;	//打开的视图窗口数

// Operations
public:
	bool Extract_CorrespondingPlanes(LidMC_Plane **pLidPatch, int &nPlanes);
	bool Extract_ConjugatePoints(LidMC_VP **pLidConPts, int &nPts);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPToolBar			m_wndToolBar;
//	CBCGPToolBar			m_wndToolBar1;

	CBCGPStatusBar			m_wndStatusBar;
	CWorkspaceBar		m_wndWorkSpace;
	CWorkspaceBar2		m_wndWorkSpace2;
	COutputBar			m_wndOutput;

	CAlignPrj			m_AlgPrj;
//	CString				m_strAlgFileName;

	
	bool				m_bIsPrjOpen;
	int					m_SIFTDetectZoom;
	
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnImportData();
	afx_msg void OnPrjNEW();
	afx_msg void OnPrjOPEN();
	afx_msg void OnPrjSAVE();
	afx_msg void OnPrjSAVEAS();
	afx_msg void OnUpdateImportLidData(CCmdUI* pCmdUI);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnUpdateTiePOINT(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTiePatch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTieLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOpenTieMeasure(CCmdUI* pCmdUI);
	afx_msg void OnOpenTieMeasure();
	//}}AFX_MSG
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	afx_msg void OnAppLook(UINT id);
	afx_msg void OnUpdateAppLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()


	UINT	m_nAppLook;
public:
	afx_msg void OnToolsExportimglist();
	afx_msg void OnToolsExportkeylist();
	//afx_msg void OnToolsImportmatch();
	afx_msg void OnToolsLoadtiept();
	afx_msg void OnToolsTracktiept();
	afx_msg void OnToolsCleartielist();
	afx_msg void OnRansacEstimateaffine();
	afx_msg void OnMatchlid2image();
	afx_msg void OnToolsDeleteduplicates();
	afx_msg void OnImagepeCreateiop();
	afx_msg void OnImagepeCreateaop();
	afx_msg void OnRansacEpipolarestimate();
	afx_msg void OnToolsTracks();
	afx_msg void OnExporttieobject();
	afx_msg void OnImageDetectSIFT();
	afx_msg void OnImageMatchANN();
	afx_msg void OnTestLidgeo();
	afx_msg void OnTestTrajectory();
	afx_msg void OnTestInterpolate();
	afx_msg void OnCombinedatPointcloudrasterize();
	afx_msg void OnPreproccessATN2TPlane();
//	afx_msg void OnCalibrationCameraboresight();
//	afx_msg void OnCameraboresightExtracttiepoints();
	afx_msg void OnCameraboresightExportimagelist();
	afx_msg void OnCameraboresightRuncalib();
	afx_msg void OnImageorientationAbor();
	afx_msg void OnMatchlidimg();
//	afx_msg void OnImagematchExportlidimglist();
	afx_msg void OnImagebaClassic();
	afx_msg void OnPreproccessImagedistortioncorrecting();
	afx_msg void OnImagebaGps();
	afx_msg void OnPatbClassic();
	afx_msg void OnPATBGPSBA();
	afx_msg void OnLidcalibVcm();
	afx_msg void OnLidcalibSkaloud();
	afx_msg void OnLidcalibNnp();
	afx_msg void OnPreproccessAtn2las();
	afx_msg void OnLidadjPossup();
	afx_msg void OnPreproccessAtn2utm();
	afx_msg void OnLidarmatchAnn();
	afx_msg void OnLidcalibTimeinterpolation();
	afx_msg void OnToolsExporttiepoints();
	afx_msg void OnLidarDetectSift();
	afx_msg void OnVisualsfmdataClassicba();
	afx_msg void OnAccuracyanalystZdis();
	afx_msg void OnCoplanarRigorousmodel();
	afx_msg void OnLidarcalibrationExtractobsvs();
	afx_msg void OnCoplanarTraj();
	afx_msg void OnCoplanarNotrajRessl();
	afx_msg void OnLidarmatchExtractcorresppolylines();
	afx_msg void OnLidarcorrectionRigorousmodel();
	afx_msg void OnLidarcorrectionYuanmodel();
//	afx_msg void OnCoplanarNotraj32980();
	afx_msg void OnCoplanarNotrajYuan();
//	afx_msg void OnLidarcalibrationVcp();
	afx_msg void OnLidarcalibrationVcp();
	afx_msg void OnConjugatepointsNotrajYuan();
	afx_msg void OnCoplanarNotrajJing();
	afx_msg void OnConjugatepointsNotrajJing();
	afx_msg void OnConjugatepointsNotrajRessl();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__AC1B99F4_9B66_4F8C_AAF6_05EAE76D82DB__INCLUDED_)
