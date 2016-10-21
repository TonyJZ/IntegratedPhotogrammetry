// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Alignment.h"
//#include "vld.h"

//#include "..\..\src\ImageMatch\sift.h"
#include "MainFrm.h"
#include "orsImage\orsIImageService.h"
#include "orsImage\orsIImageSourceReader.h"
#include "orsGuiBase\orsIProcessMsgBar.h"
#include "orsFeature2D\orsIAlg2DFeatureRegistering.h"
#include "lidBase.h"
#include "orsBase\orsContants.h"
//#include "DPSDataType.h"
#include "Calib_Camera.h"
#include "DlgATN2UTM.h"

#include <vector>
#include "math.h"

///////////match//////
#include "ImageMatchAT/ANNMatch.h"
#include "ImageMatchAT/featureMatch.h"
#include "orsFeature2D\orsMatchLinks.h"
// #include "RansacEstimate\ransac_affine.h"
// #include "RansacEstimate\ransac_epipolar.h"
// #include "RansacEstimate\ransac_projection.h"

//////////point cloud///////////
#include "orsPointCloud/orsIPointCloudService.h"

ORS_GET_POINT_CLOUD_SERVICE_IMPL();
ORS_GET_IMAGE_SERVICE_IMPL();

/////////ATN index/////////
#include "ATNPointIndex.h"
#include "GenPointIndex/KDTindex.h"

#include "CorrespFile_IO.h"
//#include "GeometryFitting\lineFitting.h"


/////////Lidar calib////////////
#include "LidCalib/LidCalib_SetPlatform.h"
#include "LidCalib/CalibModel_Ressl.h"

using namespace std;


#include "ImportLidDlg.h"
#define __BUNDLER__
#define __DEMO__
//#include "bundler\Bundler_zj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_STRING 1024

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_ImportLidData, OnImportData)
	ON_COMMAND(ID_Prj_NEW, OnPrjNEW)
	ON_COMMAND(ID_Prj_OPEN, OnPrjOPEN)
	ON_COMMAND(ID_Prj_SAVE, OnPrjSAVE)
	ON_COMMAND(ID_Prj_SAVE_AS, OnPrjSAVEAS)
	ON_UPDATE_COMMAND_UI(ID_ImportLidData, OnUpdateImportLidData)
	ON_WM_COPYDATA()
	ON_UPDATE_COMMAND_UI(ID_TiePOINT, OnUpdateTiePOINT)
	ON_UPDATE_COMMAND_UI(ID_TiePatch, OnUpdateTiePatch)
	ON_UPDATE_COMMAND_UI(ID_TieLine, OnUpdateTieLine)
	ON_UPDATE_COMMAND_UI(ID_OpenTieMeasure, OnUpdateOpenTieMeasure)
	ON_COMMAND(ID_OpenTieMeasure, OnOpenTieMeasure)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2008, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2008, OnUpdateAppLook)
	ON_COMMAND(ID_TOOLS_ExportImgList, &CMainFrame::OnToolsExportimglist)
	ON_COMMAND(ID_TOOLS_ExportKeyList, &CMainFrame::OnToolsExportkeylist)
	//ON_COMMAND(ID_TOOLS_IMPORTMATCH, &CMainFrame::OnToolsImportmatch)
	ON_COMMAND(ID_TOOLS_LoadTiePt, &CMainFrame::OnToolsLoadtiept)
	ON_COMMAND(ID_TOOLS_TRACKTiePt, &CMainFrame::OnToolsTracktiept)
	ON_COMMAND(ID_TOOLS_CLEARTIELIST, &CMainFrame::OnToolsCleartielist)
	ON_COMMAND(ID_RANSAC_ESTIMATEAFFINE, &CMainFrame::OnRansacEstimateaffine)
	ON_COMMAND(ID_MatchLid2Image, &CMainFrame::OnMatchlid2image)
	ON_COMMAND(ID_TOOLS_DELETEDUPLICATES, &CMainFrame::OnToolsDeleteduplicates)
	ON_COMMAND(ID_IMAGEPE_CREATEIOP, &CMainFrame::OnImagepeCreateiop)
	ON_COMMAND(ID_IMAGEPE_CREATEAOP, &CMainFrame::OnImagepeCreateaop)
	ON_COMMAND(ID_RANSAC_EPIPOLARESTIMATE, &CMainFrame::OnRansacEpipolarestimate)
	ON_COMMAND(ID_TOOLS_TRACKS, &CMainFrame::OnToolsTracks)
	ON_COMMAND(ID_ExportTieObject, &CMainFrame::OnExporttieobject)
	ON_COMMAND(ID_TOOLS_DETECTSIFTKEYPOINTS, &CMainFrame::OnImageDetectSIFT)
	ON_COMMAND(ID_TOOLS_ANNMATCHES, &CMainFrame::OnImageMatchANN)
	ON_COMMAND(ID_TEST_LidGeo, &CMainFrame::OnTestLidgeo)
	ON_COMMAND(ID_TEST_TRAJECTORY, &CMainFrame::OnTestTrajectory)
	ON_COMMAND(ID_TEST_INTERPOLATE, &CMainFrame::OnTestInterpolate)
	ON_COMMAND(ID_COMBINEDAT_POINTCLOUDRASTERIZE, &CMainFrame::OnCombinedatPointcloudrasterize)
	ON_COMMAND(ID_PREPROCCESS_COORDINATETRANSFORM, &CMainFrame::OnPreproccessATN2TPlane)
//	ON_COMMAND(ID_CALIBRATION_CAMERABORESIGHT, &CMainFrame::OnCalibrationCameraboresight)
//	ON_COMMAND(ID_CAMERABORESIGHT_EXTRACTTIEPOINTS, &CMainFrame::OnCameraboresightExtracttiepoints)
	ON_COMMAND(ID_CAMERABORESIGHT_EXPORTIMAGELIST, &CMainFrame::OnCameraboresightExportimagelist)
	ON_COMMAND(ID_CAMERABORESIGHT_RUNCALIB, &CMainFrame::OnCameraboresightRuncalib)
	ON_COMMAND(ID_IMAGEORIENTATION_ABOR, &CMainFrame::OnImageorientationAbor)
	ON_COMMAND(ID_MATCHLidImg, &CMainFrame::OnMatchlidimg)
//	ON_COMMAND(ID_IMAGEMATCH_EXPORTLIDIMGLIST, &CMainFrame::OnImagematchExportlidimglist)
ON_COMMAND(ID_IMAGEBA_CLASSIC, &CMainFrame::OnImagebaClassic)
ON_COMMAND(ID_PREPROCCESS_IMAGEDISTORTIONCORRECTING, &CMainFrame::OnPreproccessImagedistortioncorrecting)
ON_COMMAND(ID_IMAGEBA_GPS, &CMainFrame::OnImagebaGps)
ON_COMMAND(ID_PATB_Classic, &CMainFrame::OnPatbClassic)
ON_COMMAND(ID_PATB_GPSBA, &CMainFrame::OnPATBGPSBA)
ON_COMMAND(ID_LidCalib_VCM, &CMainFrame::OnLidcalibVcm)
ON_COMMAND(ID_LidCalib_SKALOUD, &CMainFrame::OnLidcalibSkaloud)
ON_COMMAND(ID_LidCalib_NNP, &CMainFrame::OnLidcalibNnp)
ON_COMMAND(ID_PREPROCCESS_ATN2Las, &CMainFrame::OnPreproccessAtn2las)
ON_COMMAND(ID_LIDAdj_POSSup, &CMainFrame::OnLidadjPossup)
ON_COMMAND(ID_PREPROCCESS_ATN2UTM, &CMainFrame::OnPreproccessAtn2utm)
ON_COMMAND(ID_LIDARMATCH_ANN, &CMainFrame::OnLidarmatchAnn)
ON_COMMAND(ID_LidCalib_TimeInterpolation, &CMainFrame::OnLidcalibTimeinterpolation)
ON_COMMAND(ID_TOOLS_EXPORTTIEPOINTS, &CMainFrame::OnToolsExporttiepoints)
ON_COMMAND(ID_LIDARMATCH_DETECT_SIFT, &CMainFrame::OnLidarDetectSift)
ON_COMMAND(ID_VISUALSFMDATA_CLASSICBA, &CMainFrame::OnVisualsfmdataClassicba)
ON_COMMAND(ID_ACCURACYANALYST_Zdis, &CMainFrame::OnAccuracyanalystZdis)
ON_COMMAND(ID_COPLANAR_RIGOROUSMODEL, &CMainFrame::OnCoplanarRigorousmodel)
ON_COMMAND(ID_LIDARCALIBRATION_EXTRACTOBSVS, &CMainFrame::OnLidarcalibrationExtractobsvs)
ON_COMMAND(ID_COPLANAR_TRAJ, &CMainFrame::OnCoplanarTraj)
ON_COMMAND(ID_COPLANAR_NOTRAJ_Ressl, &CMainFrame::OnCoplanarNotrajRessl)
ON_COMMAND(ID_LIDARMATCH_EXTRACTCORRESPPOLYLINES, &CMainFrame::OnLidarmatchExtractcorresppolylines)
ON_COMMAND(ID_LIDARCORRECTION_RIGOROUSMODEL, &CMainFrame::OnLidarcorrectionRigorousmodel)
ON_COMMAND(ID_LIDARCORRECTION_YUANMODEL, &CMainFrame::OnLidarcorrectionYuanmodel)
//ON_COMMAND(ID_COPLANAR_NOTRAJ32980, &CMainFrame::OnCoplanarNotraj32980)
ON_COMMAND(ID_COPLANAR_NOTRAJ_YUAN, &CMainFrame::OnCoplanarNotrajYuan)
//ON_COMMAND(ID_LIDARCALIBRATION_VCP, &CMainFrame::OnLidarcalibrationVcp)
ON_COMMAND(ID_LIDARCALIBRATION_VCP, &CMainFrame::OnLidarcalibrationVcp)
ON_COMMAND(ID_CONJUGATEPOINTS_NOTRAJ_Yuan, &CMainFrame::OnConjugatepointsNotrajYuan)
ON_COMMAND(ID_COPLANAR_NOTRAJ_Jing, &CMainFrame::OnCoplanarNotrajJing)
ON_COMMAND(ID_CONJUGATEPOINTS_NOTRAJ_Jing, &CMainFrame::OnConjugatepointsNotrajJing)
ON_COMMAND(ID_CONJUGATEPOINTS_NOTRAJ_Ressl, &CMainFrame::OnConjugatepointsNotrajRessl)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static orsString wktWGS84 = "GEOGCS[\"WGS 84\", \
						  DATUM[\"WGS_1984\", \
						  SPHEROID[\"WGS 84\", 6378137,298.257223563, \
						  AUTHORITY[\"EPSG\",\"7030\"]], \
						  TOWGS84[0,0,0,0,0,0,0], \
						  AUTHORITY[\"EPSG\",\"6326\"]], \
						  PRIMEM[\"Greenwich\", 0, \
						  AUTHORITY[\"EPSG\",\"8901\"]], \
						  UNIT[\"degree\",0.0174532925199433, \
						  AUTHORITY[\"EPSG\",\"9108\"]], \
						  AUTHORITY[\"EPSG\",\"4326\"]]";

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_WIN_XP);

	// TODO: add member initialization code here
	m_tieType=TO_POINT;
	m_bIsPrjOpen=false;
	m_TOMeasureWindows=0;
	m_RegistViewWindows=0;

	m_SIFTDetectZoom = 8;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	OnAppLook (m_nAppLook);

	CBCGPToolBar::EnableQuickCustomization ();


	// TODO: Define your own basic commands. Be sure, that each pulldown 
	// menu have at least one basic command.

	CList<UINT, UINT>	lstBasicCommands;

	lstBasicCommands.AddTail (ID_VIEW_TOOLBARS);
	lstBasicCommands.AddTail (ID_FILE_NEW);
	lstBasicCommands.AddTail (ID_FILE_OPEN);
	lstBasicCommands.AddTail (ID_FILE_SAVE);
	lstBasicCommands.AddTail (ID_FILE_PRINT);
	lstBasicCommands.AddTail (ID_APP_EXIT);
	lstBasicCommands.AddTail (ID_EDIT_CUT);
	lstBasicCommands.AddTail (ID_EDIT_PASTE);
	lstBasicCommands.AddTail (ID_EDIT_UNDO);
	lstBasicCommands.AddTail (ID_APP_ABOUT);
	lstBasicCommands.AddTail (ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail (ID_VIEW_CUSTOMIZE);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2000);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_XP);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2003);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2005);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2008);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_1);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_2);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_3);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_WIN_XP);

	CBCGPToolBar::SetBasicCommands (lstBasicCommands);

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	// Detect color depth. 256 color toolbars can be used in the
	// high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

//	UINT uiToolbarHotID = bIsHighColor ? IDB_TOOLBAR256 : 0;
	UINT uiToolbarHotID = 0;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

//	if (!m_wndToolBar1.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
// 		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
// 		!m_wndToolBar1.LoadToolBar(IDR_TOOLBAR1, 0, 0, FALSE, 0, 0, uiToolbarHotID))
// 	{
// 		TRACE0("Failed to create toolbar\n");
// 		return -1;      // fail to create
// 	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Load control bar icons:
	CBCGPToolBarImages imagesWorkspace;
	imagesWorkspace.SetImageSize (CSize (16, 16));
	imagesWorkspace.SetTransparentColor (RGB (255, 0, 255));
	imagesWorkspace.Load (IDB_WORKSPACE);

	if (!m_wndWorkSpace.Create (_T("View  1"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_WORKSPACE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar\n");
		return -1;      // fail to create
	}

	m_wndWorkSpace.SetIcon (imagesWorkspace.ExtractIcon (0), FALSE);

	if (!m_wndWorkSpace2.Create (_T("View 2"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_WORKSPACE2,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar 2\n");
		return -1;      // fail to create
	}

	m_wndWorkSpace2.SetIcon (imagesWorkspace.ExtractIcon (1), FALSE);

	if (!m_wndOutput.Create (_T("Output"), this, CRect (0, 0, 150, 150),
		TRUE /* Has gripper */, ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create output bar\n");
		return -1;      // fail to create
	}

	m_wndOutput.SetIcon (imagesWorkspace.ExtractIcon (2), FALSE);

	m_wndToolBar.SetWindowText (_T("Standard"));
//	m_wndToolBar1.SetWindowText (_T("Customerized"));
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	m_wndToolBar1.EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBarLeftOf (&m_wndToolBar, &m_wndToolBar1);
	m_wndWorkSpace.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWorkSpace2.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndToolBar);
//	DockControlBar(&m_wndToolBar1);
	DockControlBar (&m_wndWorkSpace);
	m_wndWorkSpace2.AttachToTabWnd (&m_wndWorkSpace, BCGP_DM_STANDARD, FALSE, NULL);
	DockControlBar(&m_wndOutput);


	m_wndToolBar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));
//	m_wndToolBar1.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));

	// Enable conttol bar context menu (list of bars + customize command):
	EnableControlBarMenu (	
		TRUE,				// Enable
		ID_VIEW_CUSTOMIZE, 	// Customize command ID
		_T("Customize..."),	// Customize command text
		ID_VIEW_TOOLBARS);	// Menu items with this ID will be replaced by
							// toolbars menu

	m_wndWorkSpace.SetOutputBar(&m_wndOutput);
	m_wndWorkSpace.SetAlgPrj(&m_AlgPrj);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnViewCustomize()
{
	//------------------------------------
	// Create a customize toolbars dialog:
	//------------------------------------
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this,
		TRUE /* Automatic menus scaning */
		);

	pDlgCust->Create ();
}

afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM /*wp*/,LPARAM)
{
	// TODO: reset toolbar with id = (UINT) wp to its initial state:
	//
	// UINT uiToolBarId = (UINT) wp;
	// if (uiToolBarId == IDR_MAINFRAME)
	// {
	//		do something with m_wndToolBar
	// }

	return 0;
}

void CMainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);

	m_nAppLook = id;

	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D;

	switch (m_nAppLook)
	{
	case ID_VIEW_APPLOOK_2000:
		// enable Office 2000 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager));
		break;

	case ID_VIEW_APPLOOK_XP:
		// enable Office XP look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		// enable Windows XP look (in other OS Office XP look will be used):
		CBCGPWinXPVisualManager::m_b3DTabsXPTheme = TRUE;
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
		break;

	case ID_VIEW_APPLOOK_2003:
		// enable Office 2003 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2005:
		// enable VS 2005 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2008:
		// enable VS 2007 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2008));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007:
	case ID_VIEW_APPLOOK_2007_1:
	case ID_VIEW_APPLOOK_2007_2:
	case ID_VIEW_APPLOOK_2007_3:
		// enable Office 2007 look:

		switch (m_nAppLook)
		{
		case ID_VIEW_APPLOOK_2007:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_2007_1:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_2007_2:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
			break;

		case ID_VIEW_APPLOOK_2007_3:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);
			break;
		}

		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	default:
		ASSERT (FALSE);
	}

	CBCGPDockManager* pDockManager = GetDockManager ();
	if (pDockManager != NULL)
	{
		ASSERT_VALID (pDockManager);
		pDockManager->AdjustBarFrames ();
	}

	CBCGPTabbedControlBar::ResetTabs ();

	RecalcLayout ();
	RedrawWindow (NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

//	theApp.WriteInt (_T("ApplicationLook"), m_nAppLook);
}

void CMainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio (m_nAppLook == pCmdUI->m_nID);
}


void CMainFrame::OnImportData() 
{
	int lineNum=m_AlgPrj.GetLidList()->GetSize();
	CImportLidDlg dlg(lineNum+1);

	if(dlg.DoModal()!=IDOK)
		return;
	
	Align_LidLine lidLine;

	lidLine.LineID=lineNum+1;
	lidLine.GridName=dlg.m_strImgName;
	lidLine.LasName=dlg.m_strAtnName;
	lidLine.TrjName=dlg.m_strTrajName;
	lidLine.averFH=dlg.m_dAFH;

	m_AlgPrj.GetLidList()->Add(lidLine);

	Align_Image	 imgItem;
	CArray<CString, CString&>	*pImgVec=&(dlg.m_imgNameVec);
	int imgNum=pImgVec->GetSize();

	for(int i=0; i<imgNum; i++)
	{
		imgItem.nImgID = getUniqueID(lidLine.LineID, i+1);//sFlag + lidLine.LineID*sFactor + i;
		imgItem.ImgName=pImgVec->GetAt(i);
//		imgItem.nPhoID=i+1;

		m_AlgPrj.GetImgList()->Add(imgItem);
	}

	m_wndWorkSpace.Relayout();
	m_bIsPrjOpen=true;
}


void CMainFrame::OnPrjNEW() 
{
	m_AlgPrj.Close();
//	m_strAlgFileName.Empty();
	m_bIsPrjOpen=false;

	m_wndWorkSpace.SetAlgPrj(&m_AlgPrj);
}


void CMainFrame::OnPrjOPEN() 
{
	CFileDialog  dlg(TRUE,"apj",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"Align工程文件(*.apj)|*.apj|(*.*)|*.*||",NULL);
	
	if(dlg.DoModal()!=IDOK)
		return;
	
	CString strPath=dlg.GetPathName();

	m_AlgPrj.Open(strPath.GetBuffer(0));
//	m_strAlgFileName=strPath;

	m_wndWorkSpace.SetAlgPrj(&m_AlgPrj);

	m_bIsPrjOpen=true;
}

void CMainFrame::OnPrjSAVE() 
{
	m_AlgPrj.Save();
	
}

void CMainFrame::OnPrjSAVEAS() 
{
	m_AlgPrj.SaveAs();
	
}

void CMainFrame::OnUpdateImportLidData(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	linkMSG *msg = (linkMSG *)pCopyDataStruct->lpData;
	
	m_tieType=msg->objType;

	if (msg->id==ORS_LM_TOMeasure_Exit)
	{
		m_TOMeasureWindows--;
	}
	else if(msg->id==ORS_LM_RegistViewer_Exit)
	{
		m_RegistViewWindows--;
	}

	return S_OK;
}

void CMainFrame::OnUpdateTiePOINT(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);

	if(m_tieType==TO_POINT)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateTiePatch(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	
	if(m_tieType==TO_PATCH)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CMainFrame::OnUpdateTieLine(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	
	if(m_tieType==TO_LINE)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateOpenTieMeasure(CCmdUI* pCmdUI) 
{
	if(m_bIsPrjOpen)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnOpenTieMeasure() 
{
	if(m_TOMeasureWindows==0)
	{//每次最多打开一个实例
		CString cmdLine;
		cmdLine = GetApp()->GetAppDir();
		
		CString	strAlgName;
		char buf[20];
		
		strAlgName=m_AlgPrj.GetFilePath();
		
		HWND hWnd=GetSafeHwnd();
		_ultoa( (DWORD)hWnd, buf, 16 );
		
		
		cmdLine += "TieObjMeasure.exe "; 
		cmdLine += strAlgName;
		cmdLine += " -HWND ";
		cmdLine += buf;
		cmdLine += " -TO_Type ";

		char type[8];
		_itoa(m_tieType, type, 10);

		cmdLine += type;

		int err = WinExec( cmdLine,  SW_SHOW);
		
		m_TOMeasureWindows++;
	}
}

void CMainFrame::OnToolsCleartielist()
{
	m_AlgPrj.ReleaseTieList();
}

void CMainFrame::OnToolsExportimglist()
{
	CFileDialog  dlg(FALSE,"image list file",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"image list file(*.txt)|*.txt||",NULL);	 

	if(dlg.DoModal()!=IDOK)
		return;

	CString	strPathName=dlg.GetPathName();

	FILE *fp=NULL;
	fp=fopen(strPathName.GetBuffer(0), "w");

	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i;
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine line=pLidList->GetAt(i);

		fprintf(fp, "%s\n", line.GridName.GetBuffer(0));
	}

	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image line=pImgList->GetAt(i);

		fprintf(fp, "%s\n", line.ImgName.GetBuffer(0));
	}

	fclose(fp);
}

void CMainFrame::OnToolsExportkeylist()
{
	CFileDialog  dlg(FALSE,"image list file",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"image list file(*.txt)|*.txt||",NULL);	 

	if(dlg.DoModal()!=IDOK)
		return;

	CString	strPathName=dlg.GetPathName();

	FILE *fp=NULL;
	fp=fopen(strPathName.GetBuffer(0), "w");

	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i;
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine line=pLidList->GetAt(i);
		int pos;
		CString strKeyName;

		pos=line.GridName.ReverseFind('.');
		strKeyName=line.GridName.Left(pos);
		strKeyName+=".sift.txt";

		fprintf(fp, "%s\n", strKeyName.GetBuffer(0));
	}

	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image line=pImgList->GetAt(i);
		int pos;
		CString strKeyName;

		pos=line.ImgName.ReverseFind('.');
		strKeyName=line.ImgName.Left(pos);
		strKeyName+=".sift.txt";


		fprintf(fp, "%s\n", strKeyName.GetBuffer(0));
	}


	fclose(fp);
}

#include "DlgTrackPt.h"
void CMainFrame::OnToolsTracktiept()
{
// 	CBundler		bundler;
// 	camera_param camInfo;
// 
	CDlgTrackPt dlg;
	if(dlg.DoModal()!=IDOK)
		return;

// 
// 
// 	CFileDialog  indlg(TRUE,"匹配点",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);
// 	
// 	if(indlg.DoModal()!=IDOK)
// 		return;
// 
// 	CFileDialog  outdlg(FALSE,"连接点",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.txt)|*.txt||",NULL);
// 
// 	if(outdlg.DoModal()!=IDOK)
// 		return;

	CString strInput=dlg.m_strMatchFile;
	CString strOutput=dlg.m_strTrackFile;
	CString strImgList=dlg.m_strImgList;
	CString strKeyList=dlg.m_strKeyList;

// 	char imglist[]="G:/Data/HYSJ/LIDAR-raw-data/anyang/Calibrate/ATN/TP/imgList.txt";
// 	char keylist[]="G:/Data/HYSJ/LIDAR-raw-data/anyang/Calibrate/ATN/TP/keyList.txt";

// 	bundler.SetImageNameList(strImgList.GetBuffer(128));
// 	bundler.SetKeyNameList(strKeyList.GetBuffer(128));

// 	memset(&camInfo, 0, sizeof(camInfo));
// 	bundler.TrackTiePoints(strInput.GetBuffer(128), strOutput.GetBuffer(128)/*, camInfo*/);
}

//加载连接点(tracks)  注意：需要先打开工程
#include "DlgSetTiePtOverlap.h"
void CMainFrame::OnToolsLoadtiept()
{
	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	std::vector< ref_ptr<orsIImageSourceReader> >	vec_imgReader;


	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	int i, j;
// 	if(pLidList->GetSize()==0)
// 	{
// 		AfxMessageBox("LiDAR条带为空，是否未打开工程!");
// 		return;
// 	}

	bool	bEmptyLidar=false;
	int emptyNum=0;	//空条带数
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine line=pLidList->GetAt(i);
		ref_ptr<orsIImageSourceReader> ImgReader;

		ImgReader=imageService->openImageFile(line.GridName.GetBuffer(0));
		if (!ImgReader.get())
		{//could not open the image
			AfxMessageBox("can't open image\n") ;
			bEmptyLidar=true;
			emptyNum++;
			continue;
		}
		
		vec_imgReader.push_back(ImgReader);
	}

	CFileDialog  dlg(true, "连接点", NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);	 

	if(dlg.DoModal()!=IDOK)
		return;

	CString	strPathName=dlg.GetPathName();

	FILE *fp=NULL;
	fp=fopen(strPathName.GetBuffer(0), "r");
	if(fp==NULL)	return;

	int num, connects;
	int lidNum=pLidList->GetSize();

	char pLine[MAX_STRING];
	fgets(pLine, MAX_STRING, fp);
	if(!strstr(pLine, "[tracks]"))
	{
		AfxMessageBox("It is not tracks file!");

	}

//	m_AlgPrj.ReleaseTieList();

	CArray<TieObjChain*, TieObjChain*> *pTieList=m_AlgPrj.GetTieObjList();
	int tieID=pTieList->GetSize();
	if(tieID==0)	tieID++;

	int imgID;
	int step;
	if(bEmptyLidar)
	{
		step=emptyNum+1;
	}
	else
	{
		step=1;
	}

	int minProj = 2;	//最小重叠度
	CDlgSetTiePtOverlap overlapDlg;
	if(overlapDlg.DoModal()==IDOK)
		minProj = overlapDlg.m_minTiePtOverlap;

	int imgNum;
	ors_int64 ftID;
	double xi, yi, zi, xs, ys, zs;
	while(!feof(fp)) 
	{
		fscanf(fp, "%d\n", &num);

		fgets(pLine, MAX_STRING, fp);
		if(strstr(pLine, "[imageNum]"))
		{
			fscanf( fp, "%d\n", &imgNum );
		}

		for(i=0; i<num; i++)
		{
			fscanf(fp, "%I64d%d ", &ftID, &connects);
//			fscanf(fp, "%d ", &connects);
			TieObjChain *pCurChain=NULL;
			
			if(connects >= minProj)
			{
				pCurChain=new TieObjChain;
				pCurChain->TieID=tieID;
				pCurChain->type=TO_POINT;
				pCurChain->bDel=false;
				pCurChain->pChain=new CArray<TieObject*, TieObject*>; //无连接点
				pCurChain->objNum=connects;
			}	

			for(j=0; j<connects; j++)
			{
				fscanf(fp, "%d %lf %lf %lf ", &imgID, &xi, &yi, &zi);

				if(connects < minProj)
					continue;

				orsIImageGeometry *pImgGeo=NULL;

				TiePoint	*pTmpObj=new TiePoint;

				pTmpObj->tieID=pCurChain->TieID;
				pTmpObj->objType=pCurChain->type;
				pTmpObj->sourceID=imgID+step;
				pTmpObj->pt2D.x=xi;
				pTmpObj->pt2D.y=yi;
				
				if(pTmpObj->sourceID<=lidNum)
				{
					pImgGeo=vec_imgReader[imgID]->GetImageGeometry();
				
					pTmpObj->sourceType=ST_LiDAR;
					pImgGeo->IntersectWithZ( xi, yi, 0, &xs, &ys ) ;

// 					orsIImageData *pImgData=NULL;
// 					orsBandSet bandSet;
// 					ors_uint32 numBand = vec_imgReader[imgID]->getNumberOfInputBands();
// 					orsRect_i rect=vec_imgReader[imgID]->getBoundingRect();
// 					for(int iband = 0; iband < numBand; iband++)
// 					{
// 						bandSet.push_back(iband);
// 					}
// 
// 					pImgData=vec_imgReader[imgID]->getImageData(rect, 1, bandSet);
// 
// 					orsPOINT2Di position;
// 					
// 					position.x=floor(xi+0.5);
// 					position.y=floor(yi+0.5);
 					
					pTmpObj->pt3D.X=xs;
					pTmpObj->pt3D.Y=ys;
					pTmpObj->pt3D.Z=/*pImgData->getSampleValue( position, 1) ;*/0;	//从距离影像上提取
				}
				else
				{
					pTmpObj->sourceType=ST_Image;

					pTmpObj->sourceID-=lidNum;
					pTmpObj->pt3D.X = 0;
					pTmpObj->pt3D.Y = 0;
					pTmpObj->pt3D.Z = 0;
				}
				
				pCurChain->pChain->Add(pTmpObj);
			}
			fscanf(fp, "\n");

			if(pCurChain)
			{
				pTieList->Add(pCurChain);
				tieID++;
			}
		}
	};
	
	fclose(fp);
}

void CMainFrame::OnToolsExporttiepoints()
{
	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	std::vector< ref_ptr<orsIImageSourceReader> >	vec_imgReader;


	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList = m_AlgPrj.GetImgList();

// 	if(pLidList==NULL)
// 		return;

	int i, j;
	

	CFileDialog  outdlg(FALSE,"连接点",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.txt)|*.txt||",NULL);
	outdlg.m_ofn.lpstrTitle="导出连接点";

	if(outdlg.DoModal()!=IDOK)
		return;

	CString	strPathName=outdlg.GetPathName();

	FILE *fp=NULL;
	fp=fopen(strPathName.GetBuffer(0), "wt");
	if(fp==NULL)	return;

	CArray<TieObjChain*, TieObjChain*> *pTieList=m_AlgPrj.GetTieObjList();
	int tieNum = pTieList->GetSize();
	int imgNum = pLidList->GetSize() + pImgList->GetSize();
	ors_int64 ftID;
	double xi, yi, zi, xs, ys, zs;

	fprintf( fp, "[tracks]\n");
	fprintf(fp, "%10d\n", tieNum); //tie points num

	fprintf(fp, "[imageNum]\n");
	fprintf(fp, "%3d\n", imgNum); //image num

	int n=0;
	for( int i=0; i<tieNum; i++ )
	{//遍历连接点
		TieObjChain *pCurChain=pTieList->GetAt(i);
		long overlap = pCurChain->pChain->GetSize();
		long tieID = pCurChain->TieID;

		if(pCurChain->type == TO_POINT)
		{
			fprintf( fp, "%10ld ", tieID);
			fprintf( fp, "%2ld ", overlap);

			for(int j=0; j<overlap; j++)
			{
				TiePoint *pObject = (TiePoint*)pCurChain->pChain->GetAt(j); 
				fprintf( fp, "%3ld %7.6f %7.6f %7.6f ",pObject->sourceID-1, pObject->pt2D.x, pObject->pt2D.y, 0.0 );
			}
			fprintf( fp, "\n");
		}
	}
	
	fclose(fp);
}

// #include "..\LidCalib\ransac_affine.h"
// #include "..\LidCalib\ransac_projection.h"
// #include "..\LidCalib\ransac_epipolar.h"

//对LiDAR条带间的匹配点进行仿射变换估计
void CMainFrame::OnRansacEstimateaffine()
{
/*	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	std::vector< ref_ptr<orsIImageSourceReader> >	vec_imgReader;


	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i, j;
	int lidstripNum=pLidList->GetSize();
	if(lidstripNum==0)
	{
		AfxMessageBox("LiDAR条带为空，是否未打开工程!");
		return;
	}

	for(i=0; i<lidstripNum; i++)
	{
		Align_LidLine line=pLidList->GetAt(i);
		ref_ptr<orsIImageSourceReader> ImgReader;

		ImgReader=imageService->openImageFile(line.GridName.GetBuffer(0));
		if (!ImgReader.get())
		{//could not open the image
			return ;
		}
		
		vec_imgReader.push_back(ImgReader);
	}

	ors_int32 imgWid, imgHei;
	
	CFileDialog  indlg(TRUE,"匹配点",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);
	indlg.m_ofn.lpstrTitle="打开匹配点文件";

	if(indlg.DoModal()!=IDOK)
		return;

	CString strInput=indlg.GetPathName();
	
	FILE *fin=NULL;
	fin=fopen(strInput.GetBuffer(0), "r");
	if(fin==NULL)	return;

	CFileDialog  outdlg(FALSE,"ransac",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.txt)|*.txt||",NULL);
	outdlg.m_ofn.lpstrTitle="保存精化结果";

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput=outdlg.GetPathName();

	FILE *fout=NULL;
	fout=fopen(strOutput.GetBuffer(0), "w");
	if(fout==NULL)	return;

	_iphCamera  camera;
	if(pImgList->GetSize()>0)
	{
		Align_Image imgInfo=pImgList->GetAt(0);
		ref_ptr<orsIImageSourceReader> ImgReader=imageService->openImageFile(imgInfo.ImgName.GetBuffer(0));

		if (!ImgReader.get())
		{//could not open the image
			AfxMessageBox("can't open image!");		
			return ;
		}
		imgWid=ImgReader->getWidth();
		imgHei=ImgReader->getHeight();

		CFileDialog  camdlg(TRUE,"像机文件",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.cam)|*.cam|(*.*)|*.*||",NULL);
		camdlg.m_ofn.lpstrTitle="打开像机文件";

		if(camdlg.DoModal()!=IDOK)
			return;

		CString strCam=camdlg.GetPathName();
		camera.ReadCameraFile(strCam.GetBuffer(0));
		camera.m_imgWid=imgWid;
		camera.m_imgHei=imgHei;
		camera.InteriorOrientation();
	}
	
	int i1, i2;	//条带号
	int num, refineNum;
	Align_Keypoint  *pBuf1=NULL, *pBuf2=NULL;
	orsIImageGeometry *pImgGeo=NULL;
	orsIImageSourceReader	*pImgReader=NULL;
	orsRect_i rect;
	orsIImageData *pImgData=NULL;
	orsBandSet bandSet;
	double Mat[9];

	pImgReader=vec_imgReader[0].get();
	ors_uint32 numBand = pImgReader->getNumberOfInputBands();
	for(j = 0; j < numBand; j++)
	{
		bandSet.push_back(j);
	}

	float averHei = 1040;
	while(!feof(fin))
	{
		fscanf(fin, "%d %d\n", &i1, &i2);
		fscanf(fin, "%d\n", &num);
		
		if(pBuf1)		delete pBuf1;
		pBuf1=new Align_Keypoint[num];
		
		if(pBuf2)		delete pBuf2;
		pBuf2=new Align_Keypoint[num];
		
		int id1, id2;
		double x1, y1, x2, y2;
		double xs, ys, zs;
		for(i=0; i<num; i++)
		{
			fscanf(fin, "%d %lf %lf %d %lf %lf\n", &id1, &x1, &y1, &id2, &x2, &y2);

			pBuf1[i].ID=id1;	pBuf1[i].pt2D.x=x1;	pBuf1[i].pt2D.y=y1;	pBuf1[i].flag=0;
			pBuf2[i].ID=id2;	pBuf2[i].pt2D.x=x2;	pBuf2[i].pt2D.y=y2;	pBuf2[i].flag=0;
		}
		
		//LiDAR点提取三维坐标
		if(i1<lidstripNum)
		{
			pImgReader=vec_imgReader[i1].get();
			pImgGeo=vec_imgReader[i1]->GetImageGeometry();
			rect=pImgReader->getBoundingRect();
//			pImgData=pImgReader->getImageData(rect, 1, bandSet);

			orsPOINT2Di position;
			for(i=0; i<num; i++)
			{
				pImgGeo->IntersectWithZ( pBuf1[i].pt2D.x, pBuf1[i].pt2D.y, averHei, &xs, &ys ) ;

				pBuf1[i].pt3D.X=xs;
				pBuf1[i].pt3D.Y=ys;
				pBuf1[i].pt3D.Z=averHei;	//从点云中提取

				position.x=floor(pBuf1[i].pt2D.x+0.5);
				position.y=floor(pBuf1[i].pt2D.y+0.5);

//				pBuf1[i].pt3D.Z=pImgData->getSampleValue( position, 1) ;
			}
		}

		if(i2<lidstripNum)
		{
			pImgReader=vec_imgReader[i2].get();
			pImgGeo=vec_imgReader[i2]->GetImageGeometry();
			rect=pImgReader->getBoundingRect();
//			pImgData=pImgReader->getImageData(rect, 1, bandSet);

			orsPOINT2Di position;
			for(i=0; i<num; i++)
			{
				pImgGeo->IntersectWithZ( pBuf2[i].pt2D.x, pBuf2[i].pt2D.y, averHei, &xs, &ys ) ;

				pBuf2[i].pt3D.X=xs;
				pBuf2[i].pt3D.Y=ys;
				pBuf2[i].pt3D.Z=averHei;	//从点云中提取

				position.x=floor(pBuf2[i].pt2D.x+0.5);
				position.y=floor(pBuf2[i].pt2D.y+0.5);

//				pBuf2[i].pt3D.Z=pImgData->getSampleValue( position, 1) ;
			}
		}
	
		if(i1<lidstripNum && i2<lidstripNum)
		{//LiDAR条带间挑点
			EstimateTransform_Affine(pBuf1, pBuf2, num, refineNum, Mat, 1.0);
		}
		else if(i1<lidstripNum)
		{//LiDAR与影像间  须保证排列时LiDAR条带在前
			EstimateTransform_Projection(pBuf1, pBuf2, num, refineNum, 
				&camera, 1.0) ;
		}
		else
		{//影像间
			EstimateTransform_Epipolar(pBuf1, pBuf2, num, refineNum,
				&camera, 1.0) ;
		}
		
		fprintf(fout, "%d %d\n", i1, i2);
		fprintf(fout, "%d\n", refineNum);

		for(i=0; i<num; i++)
		{
			if(pBuf1[i].flag != keypoint_inlier)
				continue;

			fprintf(fout, "%d %lf %lf %d %lf %lf\n", pBuf1[i].ID, pBuf1[i].pt2D.x, pBuf1[i].pt2D.y, pBuf2[i].ID, pBuf2[i].pt2D.x, pBuf2[i].pt2D.y);
		}
	}


	if(fin)		fclose(fin);	
	if(fout)	fclose(fout);
	if(pBuf1)		delete pBuf1;		pBuf1=NULL;
	if(pBuf2)		delete pBuf2;		pBuf2=NULL;*/
}

void CMainFrame::OnMatchlid2image()
{
	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();

	CFileDialog  dlg(FALSE,"match file",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"match file(*.txt)|*.txt||",NULL);	 

	if(dlg.DoModal()!=IDOK)
		return;

	CString	strMatches=dlg.GetPathName();
	FILE	*fp=NULL;

	fp=fopen(strMatches, "w");
	if(fp==NULL)
	{
		AfxMessageBox("can't open file!");
		return;
	}

	ref_ptr<orsIProcessMsgBar2> pMsgBar;

	pMsgBar = ORS_CREATE_OBJECT( orsIProcessMsgBar2, ORS_PROCESSMSG_BAR2_DEFAULT );

	pMsgBar->InitProgressBar( "特征匹配", "整体进度", pLidList->GetSize() );
	

// 	CKeyMatch	KeyMatch;
// 
// 	int i, j, k;
// 	int nStrip=pLidList->GetSize();
// 	for(i=0; i<pLidList->GetSize(); i++)
// 	{
// 		char msg[40];
// 		sprintf( msg, "Strip: %d", i+1 );
// 		pMsgBar->logPrint( ORS_LOG_INFO, msg );
// 
// 
// 		Align_LidLine line=pLidList->GetAt(i);
// 		int pos;
// 		CString strLidKey, strImgKey;
// 
// 		pos=line.GridName.ReverseFind('.');
// 		strLidKey=line.GridName.Left(pos);
// 		strLidKey+=".sift.txt";
// 
// 		for(j=0; j<pImgList->GetSize(); j++)
// 		{
// 			pMsgBar->process( (float)(j+1)/pImgList->GetSize() );
// 
// 			Align_Image imgInfo=pImgList->GetAt(j);
// 			if(imgInfo.nStripID!=line.LineID)
// 				continue;
// 
// 			pos=imgInfo.ImgName.ReverseFind('.');
// 			strImgKey=imgInfo.ImgName.Left(pos);
// 			strImgKey+=".sift.txt";
// 
// 			std::vector<KeypointMatch> matches = KeyMatch.Match_pairwise_NCC(strLidKey.GetBuffer(128), strImgKey.GetBuffer(128), 
// 				0.8, 2);
// 
// // 			std::vector<KeypointMatch> matches = KeyMatch.Match_pairwise_ratio(strLidKey.GetBuffer(128), strImgKey.GetBuffer(128),
// // 				0.8);
// 
// 			int num_matches = (int) matches.size();
// 
// 			if (num_matches > 0) 
// 			{
// 				/* Write the pair */
// 				fprintf(fp, "%d %d\n", i, j+nStrip);
// 
// 				/* Write the number of matches */
// 				fprintf(fp, "%d\n", (int) matches.size());
// 
// 				for (k = 0; k < num_matches; k++) 
// 				{
// 					fprintf(fp, "%d %.6f %.6f %d %.6f %.6f\n", 
// 						matches[k].m_idx1, matches[k].m_x1, matches[k].m_y1,
// 						matches[k].m_idx2, matches[k].m_x2, matches[k].m_y2);
// 				}
// 			}
// 		}
// 
// 		pMsgBar->SetPos(i+1);
// 	}
// 
// 	fclose(fp);
}

void CMainFrame::OnToolsDeleteduplicates()
{
	CFileDialog  indlg(TRUE," ",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);
	indlg.m_ofn.lpstrTitle="包含重复点的匹配文件";

	if(indlg.DoModal()!=IDOK)
		return;

	CString strInput=indlg.GetPathName();

	CFileDialog  outdlg(FALSE," ",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.txt)|*.txt||",NULL);
	outdlg.m_ofn.lpstrTitle="去除重复点的匹配文件";

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput=outdlg.GetPathName();

//	DeleteDuplicates(strInput.GetBuffer(128), strOutput.GetBuffer(128));
}


void CMainFrame::OnImagepeCreateiop()
{
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();

	if(pImgList->GetSize()==0)
	{
		AfxMessageBox("没有影像!");
		return;
	}

	CFileDialog  indlg(TRUE," ",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.cam)|*.cam|(*.*)|*.*||",NULL);
	indlg.m_ofn.lpstrTitle="打开像机文件";

	if(indlg.DoModal()!=IDOK)
		return;
	CString strCamFile=indlg.GetPathName();

// 	CAMERA	cam;
// 	ReadCamera( strCamFile.GetBuffer(128), &cam );
	_iphCamera   cam;
	cam.ReadCameraFile(strCamFile);

	CString strDir;
	if(!_SelectFolderDialog("设置iop输出目录...", &strDir, NULL))
		return;

	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	double inorImage_a[3], inorImage_b[3], inorPixel_a[3], inorPixel_b[3];
	double imgWid, imgHei;
	for(int i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image img=pImgList->GetAt(i);
		ref_ptr<orsIImageSourceReader> ImgReader;

		ImgReader=imageService->openImageFile(img.ImgName.GetBuffer(0));
		if (!ImgReader.get())
		{//could not open the image
			continue;
		}

		imgWid=ImgReader->getWidth();
		imgHei=ImgReader->getHeight();
	
		//x方向
		inorImage_a[0] = -imgWid*cam.m_pixelX/2;
		inorImage_a[1] = cam.m_pixelX;
		inorImage_a[2] = 0;	
		//y方向
		inorImage_b[0] = imgHei*cam.m_pixelY/2;
		inorImage_b[1] = 0;
		inorImage_b[2] = -cam.m_pixelY;

		inorPixel_a[0] = imgWid*1.0/2;		//保证不丢失精度
		inorPixel_a[1] = 1/cam.m_pixelX;
		inorPixel_a[2] = 0;

		inorPixel_b[0] = imgHei*1.0/2;
		inorPixel_b[1] = 0;
		inorPixel_b[2] = -1/cam.m_pixelY;

		CString iopName;
		FILE *fp;

		iopName=_ExtractFileName(img.ImgName);
		iopName=strDir+"\\"+iopName+".iop.txt";

		fp=0;
		fp=fopen(iopName, "wt");
		if(fp==0)
			continue;

		fprintf(fp, "LMARSDPS  INOR file: V1.0\n\n");

		fprintf( fp, "[Image to Pixel Parameters]\n" );
		fprintf( fp, "%e\t%e\t%e\n", inorPixel_a[0], inorPixel_a[1], inorPixel_a[2] );
		fprintf( fp, "%e\t%e\t%e\n", inorPixel_b[0], inorPixel_b[1], inorPixel_b[2] );

		fprintf( fp, "\n[Pixel to Image Parameters]\n" );
		fprintf( fp, "%e\t%e\t%e\n", inorImage_a[0], inorImage_a[1], inorImage_a[2] );
		fprintf( fp, "%e\t%e\t%e\n", inorImage_b[0], inorImage_b[1], inorImage_b[2] );

		fprintf( fp, "\n[x0, y0]\n");
		fprintf(fp, "%.6lf %.6lf\n\n", cam.m_x0, cam.m_y0);
		fprintf(fp, "[focal length]\n");
		fprintf(fp, "%.6lf\n\n", cam.m_f);
		fprintf(fp, "[radial distortion: k0,k1,k2,k3]\n");
		fprintf(fp, "%e\t%e\t%e\t%e\n\n", cam.m_k0, cam.m_k1, cam.m_k2, cam.m_k3);
		fprintf(fp, "[tangential distortion: p1,p2]\n");
		fprintf(fp, "%e\t%e\n\n", cam.m_p1, cam.m_p2);

		fclose(fp);
	}

	
}

#include "Geometry.h"
//#include "../LidCalib\LidarGeometry.h"
void CMainFrame::OnImagepeCreateaop()
{
	CFileDialog dlg(TRUE,"",NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.txt)|*.txt|(*.*)|*.*||",NULL);
	dlg.m_ofn.lpstrTitle="打开POS文件";

	if(dlg.DoModal()!=IDOK)
		return;

	CString strPOSFile=dlg.GetPathName();

	CString strDir;
	if(!_SelectFolderDialog("设置aop输出目录...", &strDir, NULL))
		return;

	int ID;
//	CString  imgName;
	double time;
	double Xs,Ys,Zs;
	double phi, omega, kappa;	

	double	s=PI/180;
	CString	strLine;
	CStdioFile	file;
//	imagePOS	imgPos;
	int i, j;
	double lat, lon;
	double	R[9];
	char imgName[128];
	bool bTPlane;

// 	orsLidarGeometry  lidGeo;
// 	lidGeo.SetORSPlatform(getPlatform());
// 
// 	bTPlane=lidGeo.SetTPlaneCoordinate();

	if(file.Open(strPOSFile.GetBuffer(0), CFile::modeRead)==0)
	{
		return;
	}

	//[imageName] [ID]	[GPStime]	[Xs]	[Ys]	[Zs]	[omega]	[phi]	[kappa]	[lat]	[lon]
//	file.ReadString(strLine);	

	bool bDeg = true;	//度

	if(bDeg)
	{
		s = PI/180;
	}
	else
	{
		s = 1.0;
	}

	while (file.ReadString(strLine))
	{

		sscanf(strLine,"%s%d%lf%lf%lf%lf%lf%lf%lf%lf%lf\n", imgName,&ID,&time,
			&Xs,&Ys,&Zs,&omega,&phi,&kappa,&lat,&lon);

//		RotateMat_wfk(omega*s, phi*s, (kappa+180)*s, R); //原始外方位元素需要进行轴改正

		//zj 2014.5.4 对原始外方位元素的处理。
		//用于定向锚点内插的相片方位元素，由于内插时已经做过相机安装改正（相机安装时主方向与飞行方向的夹角，绕z轴旋转）
		RotateMat_wfk(omega*s, phi*s, kappa*s, R);  
		R2fwk(R, &(phi), &(omega), &(kappa));

// 		if(bTPlane)
// 		{
// 			lidGeo.Geographic2TangentPlane(lat*s, lon*s, Zs, &(Xs), &(Ys), &(Zs));
// 		}

		FILE *fp=NULL;
		CString aopName=_ExtractFileName(imgName);

		aopName=strDir+"\\"+aopName+".aop.txt";	

		fp=fopen(aopName.GetBuffer(0), "wt");
		if(fp==0)
			continue;

		fprintf(fp,  "[Xs,Ys,Zs]\n");
		fprintf(fp,  "%13.3lf %13.3lf %10.3lf\n\n", Xs, Ys, Zs );

		fprintf(fp,  "[Rotation Angle]\n");
		fprintf(fp,  "%13.9lf %13.9lf %13.9lf\n\n", phi, omega, kappa );
		fprintf(fp,  "[Rotation Matrix]\n"  );
		for( i=0; i<3; i++) 
		{
			for( j=0; j<3; j++)
			{
				fprintf(fp,  "%10.7lf ", R[i*3+j] );
			}
			fprintf(fp, "\n");
		}

		fprintf(fp, "\n[GPS Time]\n");
		fprintf(fp, "%.6f\n", time);

		fprintf(fp,  "\n[Elevation Range]\n"  );
		fprintf(fp,  "%d %d %d\n", 1050, 1060, 1070);

		fclose(fp);
	}

	file.Close(); 
}

//只处理影像之间
void CMainFrame::OnRansacEpipolarestimate()
{
	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	std::vector< ref_ptr<orsIImageSourceReader> >	vec_imgReader;

	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i, j;
	int ImgNum=pImgList->GetSize();
	if(ImgNum==0)
	{
		AfxMessageBox("LiDAR条带为空，是否未打开工程!");
		return;
	}

	for(i=0; i<ImgNum; i++)
	{
		Align_Image img=pImgList->GetAt(i);
		ref_ptr<orsIImageSourceReader> ImgReader;

		ImgReader=imageService->openImageFile(img.ImgName.GetBuffer(0));
		if (!ImgReader.get())
		{//could not open the image
			return ;
		}

		vec_imgReader.push_back(ImgReader);
	}

	ors_int32 imgWid, imgHei;

	CFileDialog  indlg(TRUE,"匹配点",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);
	indlg.m_ofn.lpstrTitle="打开匹配点文件";

	if(indlg.DoModal()!=IDOK)
		return;

	CString strInput=indlg.GetPathName();

	FILE *fin=NULL;
	fin=fopen(strInput.GetBuffer(0), "r");
	if(fin==NULL)	return;

	CFileDialog  outdlg(FALSE,"ransac",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.txt)|*.txt||",NULL);
	outdlg.m_ofn.lpstrTitle="保存精化结果";

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput=outdlg.GetPathName();

	FILE *fout=NULL;
	fout=fopen(strOutput.GetBuffer(0), "w");
	if(fout==NULL)	return;

	_iphCamera  camera;
	if(pImgList->GetSize()>0)
	{
		Align_Image imgInfo=pImgList->GetAt(0);
		ref_ptr<orsIImageSourceReader> ImgReader=imageService->openImageFile(imgInfo.ImgName.GetBuffer(0));

		if (!ImgReader.get())
		{//could not open the image
			AfxMessageBox("can't open image!");		
			return ;
		}
		imgWid=ImgReader->getWidth();
		imgHei=ImgReader->getHeight();

		CFileDialog  camdlg(TRUE,"像机文件",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.cam)|*.cam|(*.*)|*.*||",NULL);
		camdlg.m_ofn.lpstrTitle="打开像机文件";

		if(camdlg.DoModal()!=IDOK)
			return;

		CString strCam=camdlg.GetPathName();
		camera.ReadCameraFile(strCam.GetBuffer(0));
		camera.m_imgWid=imgWid;
		camera.m_imgHei=imgHei;
		camera.InteriorOrientation();
	}

	int i1, i2;	//条带号
	int num, refineNum;
	Align_Keypoint  *pBuf1=NULL, *pBuf2=NULL;
	orsIImageGeometry *pImgGeo=NULL;
	orsIImageSourceReader	*pImgReader=NULL;
	orsRect_i rect;
	orsIImageData *pImgData=NULL;
	orsBandSet bandSet;
	double Mat[9];

	pImgReader=vec_imgReader[0].get();
	ors_uint32 numBand = pImgReader->getNumberOfInputBands();
	for(j = 0; j < numBand; j++)
	{
		bandSet.push_back(j);
	}

	while(!feof(fin))
	{
		fscanf(fin, "%d %d\n", &i1, &i2);
		fscanf(fin, "%d\n", &num);

		if(pBuf1)		delete pBuf1;
		pBuf1=new Align_Keypoint[num];

		if(pBuf2)		delete pBuf2;
		pBuf2=new Align_Keypoint[num];

		int id1, id2;
		double x1, y1, x2, y2;
		double xs, ys, zs;
		for(i=0; i<num; i++)
		{
			fscanf(fin, "%d %lf %lf %d %lf %lf\n", &id1, &x1, &y1, &id2, &x2, &y2);

			pBuf1[i].ID=id1;	pBuf1[i].pt2D.x=x1;	pBuf1[i].pt2D.y=y1;	pBuf1[i].flag=0;
			pBuf2[i].ID=id2;	pBuf2[i].pt2D.x=x2;	pBuf2[i].pt2D.y=y2;	pBuf2[i].flag=0;
		}

		
// 		EstimateTransform_Epipolar(pBuf1, pBuf2, num, refineNum,
// 			&camera, 1.0) ;
		

		fprintf(fout, "%d %d\n", i1, i2);
		fprintf(fout, "%d\n", refineNum);

		for(i=0; i<num; i++)
		{
			if(pBuf1[i].flag != keypoint_inlier)
				continue;

			fprintf(fout, "%d %lf %lf %d %lf %lf\n", pBuf1[i].ID, pBuf1[i].pt2D.x, pBuf1[i].pt2D.y, pBuf2[i].ID, pBuf2[i].pt2D.x, pBuf2[i].pt2D.y);
		}
	}


	if(fin)		fclose(fin);	
	if(fout)	fclose(fout);
	if(pBuf1)		delete pBuf1;		pBuf1=NULL;
	if(pBuf2)		delete pBuf2;		pBuf2=NULL;
}

void CMainFrame::OnToolsTracks()
{
	AfxMessageBox("未实现", IDOK);
	
	CFileDialog  indlg(TRUE,"匹配点",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"(*.txt)|*.txt|(*.*)|*.*||",NULL);
	indlg.m_ofn.lpstrTitle="打开tracks文件";

	if(indlg.DoModal()!=IDOK)
		return;

	CString strTracks=indlg.GetPathName();

	
}




void CMainFrame::OnExporttieobject()
{
	CFileDialog  dlg(FALSE,"txt",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return ;

	CString strFileName = dlg.GetPathName();

	FILE *fp = fopen(strFileName, "wt");

	fprintf(fp, "num: %d\n", m_AlgPrj.m_header.tieObjNum);
	
	int i, j, k;
	for(i=0; i<m_AlgPrj.m_TieList.GetSize(); i++)
	{
		TieObjChain* pChain = m_AlgPrj.m_TieList.GetAt(i);

		if(pChain->bDel)  //跳过删除节点
			continue;

// 		pChain->objNum = pChain->pChain->GetSize();
// 
// 		pChain->TieID = tieID;
// 		fwrite(&(pChain->TieID), sizeof(int), 1, fp);
// 		fwrite(&(pChain->type), sizeof(TieObj_Type), 1, fp);
// 		fwrite(&(pChain->objNum), sizeof(int), 1, fp);
// 
// 		tieID++;

		if(pChain->type==TO_POINT)
		{
			fprintf(fp, "TO_POINT %d\n", pChain->objNum);

			TieObject *pObj=pChain->pChain->GetAt(0);

			POINT3D *point3d=&(((TiePoint*)pObj)->pt3D);

			fprintf(fp, "%.3f %.3f %.3f ", point3d->X, point3d->Y, point3d->Z);
			
			for(j=0; j<pChain->objNum; j++)
			{
				pObj=pChain->pChain->GetAt(j);

				POINT2D *point2d=&(((TiePoint*)pObj)->pt2D);

				fprintf(fp, "%d %.3f %.3f ", pObj->sourceID, point2d->x, point2d->y);
				
			}
			fprintf(fp, "\n");
		}
		else if(pChain->type==TO_LINE)
		{
			fprintf(fp, "TO_LINE %d\n", pChain->objNum);

			TieObject *pObj=pChain->pChain->GetAt(0);
			POINT3D *point3d=((TieLine*)pObj)->pt3D;

			fprintf(fp, "%.3f %.3f %.3f %.3f %.3f %.3f ", point3d[0].X, point3d[0].Y, point3d[0].Z, point3d[1].X, point3d[1].Y, point3d[1].Z);
			
			for(j=0; j<pChain->objNum; j++)
			{
				pObj=pChain->pChain->GetAt(j);

				fwrite(&(pObj->sourceID), sizeof(int), 1, fp);
				

				int ptNum=((TieLine*)pObj)->ptNum;
				POINT2D *pt2d=((TieLine*)pObj)->pt2D;

				fprintf(fp, "%d %.3f %.3f %.3f %.3f ", pObj->sourceID, pt2d[0].x, pt2d[0].y, pt2d[1].x, pt2d[1].y);
			}
			fprintf(fp, "\n");
		}
		else if(pChain->type==TO_PATCH)
		{
			fprintf(fp, "TO_PATCH %d\n", pChain->objNum);

			TieObject *pObj=pChain->pChain->GetAt(0);
			POINT3D *point3d=((TiePatch*)pObj)->pt3D;

			int ptNum=((TiePatch*)pObj)->ptNum;

			fprintf(fp, "%d ", ptNum);
			for(k=0; k<ptNum; k++)
			{
				fprintf(fp, "%.3f %.3f %.3f ", point3d[k].X, point3d[k].Y, point3d[k].Z);
			}
			

			for(j=0; j<pChain->objNum; j++)
			{
				pObj=pChain->pChain->GetAt(j);

				fprintf(fp, "%d ", pObj->sourceID);

				ptNum=((TiePatch*)pObj)->ptNum;
				POINT2D *pt2d=((TiePatch*)pObj)->pt2D;
				
				fprintf(fp, "%d ", ptNum);
				for(k=0; k<ptNum; k++)
				{
					fprintf(fp, "%.3f %.3f ", pt2d[k].x, pt2d[k].y);
				}
			}
			fprintf(fp, "\n");
		}

	}

	fclose(fp);	fp=NULL;

}


#include "ImageMatchAT/SIFT.h"
#include "ImageMatchAT/ImageMatchAPI.h"

void CMainFrame::OnImageDetectSIFT()
{
//	int i, imgNum;
//	iphImgList	*ImgList;


	orsIPlatform*  pPlatform = getPlatform();
	ImageMatchSetPlatform(pPlatform);


// 	CFileDialog	dlg(TRUE, "txt", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("image list File (*.txt)|*.txt|All Files(*.*)|*.*||"));
// 
// 	if(dlg.DoModal()!=IDOK)
// 		return;
// 
// 	CString strImgListName=dlg.GetPathName();
// 
// 	FILE *fp=NULL;
// 	orsArray<orsString> vStripFiles;
// 
// 	fp=fopen(strImgListName.GetBuffer(0), "r");
// 	if(fp==NULL)
// 		return;
// 
// 	char pLine[1024];
// 	while(feof(fp)==0)
// 	{
// 		fscanf(fp, "%s", pLine);
// 
// 		vStripFiles.push_back(orsString(pLine));
// 	}
// 
// 	fclose(fp);
	orsArray<orsString> vStripFiles;

//	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i;
// 	for(i=0; i<pLidList->GetSize(); i++)
// 	{
// 		Align_LidLine line=pLidList->GetAt(i);
// 
// 		if(line.GridName.GetLength() != 0)
// 			vStripFiles.push_back(orsString(line.GridName.GetBuffer(0)));
// 	}

	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image line=pImgList->GetAt(i);

		if(line.ImgName.GetLength() != 0)
			vStripFiles.push_back(orsString(line.ImgName.GetBuffer(0)));
	}

	SIFTDetector(vStripFiles, 8);

}


void CMainFrame::OnImageMatchANN()
{
	//导出连接点文件
	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("match init (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="影像初始匹配文件";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strTrackName=outputdlg.GetPathName();

/////////////////////////////////////////////////////////////////
	int minOverlap = 2; //最小的连接点数

//	myPtPair *ptPairVec=NULL;
	int maxLen=0;
	int matchNum, refineNum;
	orsMatchLinks	mtLinkMap;
//	CATNPtIndex	index1(getPlatform()), index2(getPlatform());

	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	ref_ptr<orsIImageSourceReader> imgReader1, imgReader2;
	orsIImageGeometry *pImgGeo1=NULL, *pImgGeo2=NULL;

	ref_ptr<orsIAlg2DPointPairsCheck> pairsCheck;
	pairsCheck = ORS_CREATE_OBJECT( orsIAlg2DPointPairsCheck, ORS_ALG_2DPOINTPAIR_CHECK_DEFAULT );
//	pairsCheck = ORS_CREATE_OBJECT( orsIAlg2DPointPairsCheck, ORS_ALG_2DPOINTPAIR_CHECK_RANSAC );
		
	ref_ptr<orsI2DFeatureSet> featureSet0 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
	ref_ptr<orsI2DFeatureSet> featureSet1 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

//	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i, j, k;
	int i_ID, j_ID;
	int stripID;
//	int lidNum = pLidList->GetSize();
	int imgNum = pImgList->GetSize();

	std::vector<orsAdjPtPAIR> ptPairVec;
	orsArray<orsString> vStripFiles;

	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image imgItem=pImgList->GetAt(i);
		vStripFiles.push_back(orsString(imgItem.ImgName.GetBuffer(0)));
	}

	ImageMatchSetPlatform(getPlatform());

	
//	vStripFiles.clear();
	//沿航向匹配,k,k+1,k+2
	for(i=0; i<pImgList->GetSize()-1; i++)
	{
		Align_Image imgItem1=pImgList->GetAt(i);
		i_ID = imgItem1.nImgID;
		stripID = getStripID(i_ID);

		////////// 匹配i+1 ///////////////////////
		j=i+1;
		Align_Image imgItem2=pImgList->GetAt(j);
		j_ID = imgItem2.nImgID;

		if(getStripID(j_ID) > stripID)
		{//进入下一条带
			//i = j;
			continue;
		}

		KeyMatch_images(imgItem1.ImgName.GetBuffer(0), imgItem2.ImgName.GetBuffer(0),
			&ptPairVec, matchNum, 8);

		refineNum = matchNum;
		int numF, numH;
		numF = refineNum;
		const orsMatchedFtPAIR *pPairs;
		ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();
		pPairs = pairsCheck->check_Transform( &ptPairVec[0], &numF, ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_FMATRIX, pProp.get(), 0 );

		if(numF > 0)
			numH = numF;
		else
			assert( numF > 0 );
		pPairs = pairsCheck->check_Transform(&ptPairVec[0], &numH, ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_HMATRIX, pProp.get(), 0);

		if(numH > 0) 
			refineNum = numH;
		else
			assert(numH > 0);

		orsArray<double> arrF, arrH;
		printf( "%d %d\n", i_ID,j_ID);
		if( pProp->getAttr("F", arrF) ) {
			printf("F:");
			for (k = 0; k < 9; k++)
				printf("%lf ", arrF[k]);
			printf("\n");
		}
		if( pProp->getAttr("H", arrH) ) {
			printf( "H:");
			for (k = 0; k < 9; k++)
				printf("%lf ", arrH[k]);
			printf("\n");
		}
		printf("\n");

		if( refineNum > 6 )	
		{
			orsString siftName0 = imgItem1.ImgName.GetBuffer(0);
			siftName0 += _T(".sift.bin");
			featureSet0->load( siftName0 );

			orsString siftName1 = imgItem2.ImgName.GetBuffer(0);
			siftName1 += _T(".sift.bin");
			featureSet1->load( siftName1 );

			orsKeyPOINT dstPt, srcPt;
			for( k=0; k<refineNum; k++ )
			{//提取像方坐标
				// iStrip
				featureSet0->getFetureVector(pPairs[k].dstFtId, &dstPt);

				// iStrip1
				featureSet1->getFetureVector(pPairs[k].srcFtId, &srcPt);

				//if(ptPairVec[k].bAccepted)
				//内部会对相片号+1
				mtLinkMap.AddPtPair( j, pPairs[k].srcFtId, &srcPt, i, pPairs[k].dstFtId, &dstPt );
			}
		}

		////////////// 匹配i+2  /////////////////////////////////////////
		j=i+2;
		if(j>=pImgList->GetSize())
			continue;

		imgItem2=pImgList->GetAt(j);
		j_ID = imgItem2.nImgID;

		if(getStripID(j_ID) > stripID)
		{//进入下一条带
			//i = j;
			continue;
		}

		KeyMatch_images(imgItem1.ImgName.GetBuffer(0), imgItem2.ImgName.GetBuffer(0),
			&ptPairVec, matchNum, 8);

		refineNum = matchNum;
//		int numF, numH;
		numF = refineNum;
// 		const orsMatchedFtPAIR *pPairs;
// 		ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();
		pPairs = pairsCheck->check_Transform( &ptPairVec[0], &numF, ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_FMATRIX, pProp.get(), 0 );

		if(numF > 0)
			numH = numF;
		else
			assert( numF > 0 );
		pPairs = pairsCheck->check_Transform(&ptPairVec[0], &numH, ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_HMATRIX, pProp.get(), 0);

		if(numH > 0) 
			refineNum = numH;
		else
			assert(numH > 0);

//		orsArray<double> arrF, arrH;
		printf( "%d %d\n", i_ID,j_ID);
		if( pProp->getAttr("F", arrF) ) {
			printf("F:");
			for (k = 0; k < 9; k++)
				printf("%lf ", arrF[k]);
			printf("\n");
		}
		if( pProp->getAttr("H", arrH) ) {
			printf( "H:");
			for (k = 0; k < 9; k++)
				printf("%lf ", arrH[k]);
			printf("\n");
		}
		printf("\n");

		if( refineNum > 6 )	
		{
			orsString siftName0 = imgItem1.ImgName.GetBuffer(0);
			siftName0 += _T(".sift.bin");
			featureSet0->load( siftName0 );

			orsString siftName1 = imgItem2.ImgName.GetBuffer(0);
			siftName1 += _T(".sift.bin");
			featureSet1->load( siftName1 );

			orsKeyPOINT dstPt, srcPt;
			for( k=0; k<refineNum; k++ )
			{//提取像方坐标
				// iStrip
				featureSet0->getFetureVector(pPairs[k].dstFtId, &dstPt);

				// iStrip1
				featureSet1->getFetureVector(pPairs[k].srcFtId, &srcPt);

				//if(ptPairVec[k].bAccepted)
				//内部会对相片号+1
				mtLinkMap.AddPtPair( j, pPairs[k].srcFtId, &srcPt, i, pPairs[k].dstFtId, &dstPt );
			}
		}
	}

	int stripNum = m_AlgPrj.GetLidList()->GetSize();
	//旁向匹配
	int next_stripID;
	std::vector<orsAdjPtPAIR> bestPairVec;
	for(i=0; i<pImgList->GetSize()-1; i++)
	{
		Align_Image imgItem1=pImgList->GetAt(i);
		i_ID = imgItem1.nImgID;
		stripID = getStripID(i_ID);  //stripID从1开始编号

		if(stripID==stripNum)  //最后一个条带
			break;

		next_stripID = stripID+1;
		int sPos=-1, ePos=-1; //条带的起点和终点索引号
		for(j=i+1; j<pImgList->GetSize(); j++)
		{
			Align_Image imgItem2=pImgList->GetAt(j);
			j_ID = imgItem2.nImgID;
			if(getStripID(j_ID)==next_stripID)
			{
				if(sPos == -1)
					sPos = j;

				ePos = j;
			}
		}

		int max_matchNum=0;
		int bestIdx;
		Align_Image imgItem2;
		for(j=sPos; j<=ePos; j++)
		{
			imgItem2 = pImgList->GetAt(j);
			j_ID = imgItem2.nImgID;

			KeyMatch_images(imgItem1.ImgName.GetBuffer(0), imgItem2.ImgName.GetBuffer(0),
				&ptPairVec, matchNum, 8);

			if(max_matchNum < matchNum)
			{
				max_matchNum = matchNum;
				bestIdx = j;
				bestPairVec = ptPairVec; 
			}
		}
		//旁向取最大匹配点的像对
		refineNum = max_matchNum;
		imgItem2 = pImgList->GetAt(bestIdx); 

		int numF, numH;
		numF = refineNum;
		const orsMatchedFtPAIR *pPairs;
		ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();
		pPairs = pairsCheck->check_Transform( &bestPairVec[0], &numF, ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_FMATRIX, pProp.get(), 0 );

		if(numF > 0)
			numH = numF;
		else
			assert( numF > 0 );
		pPairs = pairsCheck->check_Transform(&bestPairVec[0], &numH, ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_HMATRIX, pProp.get(), 0);

		if(numH > 0) 
			refineNum = numH;
		else
			assert(numH > 0);

		orsArray<double> arrF, arrH;
		printf( "%d %d\n", i_ID,j_ID);
		if( pProp->getAttr("F", arrF) ) {
			printf("F:");
			for (k = 0; k < 9; k++)
				printf("%lf ", arrF[k]);
			printf("\n");
		}
		if( pProp->getAttr("H", arrH) ) {
			printf( "H:");
			for (k = 0; k < 9; k++)
				printf("%lf ", arrH[k]);
			printf("\n");
		}
		printf("\n");

		if( refineNum > 6 )	{

			orsString siftName0 = imgItem1.ImgName.GetBuffer(0);
			siftName0 += _T(".sift.bin");
			featureSet0->load( siftName0 );

			orsString siftName1 = imgItem2.ImgName.GetBuffer(0);
			siftName1 += _T(".sift.bin");
			featureSet1->load( siftName1 );

			orsKeyPOINT dstPt, srcPt;
			for( k=0; k<refineNum; k++ )
			{//提取像方坐标
				// iStrip
				featureSet0->getFetureVector(pPairs[k].dstFtId, &dstPt);

				// iStrip1
				featureSet1->getFetureVector(pPairs[k].srcFtId, &srcPt);

				//if(ptPairVec[k].bAccepted)
				//内部会对相片号+1
				mtLinkMap.AddPtPair( bestIdx, pPairs[k].srcFtId, &srcPt, i, pPairs[k].dstFtId, &dstPt );
			}
		}
	}


	int count = mtLinkMap.RemoveDuplicatePoints();
	printf("%d duplicate points are removed\n", count );

	mtLinkMap.CountOverlap( vStripFiles.size() );

	mtLinkMAP &ptLinks = mtLinkMap.GetMatchLinks();

	int tieNum = ptLinks.size();

	std::vector<int> imgIDVec;
	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image imgItem=pImgList->GetAt(i);
		imgIDVec.push_back(imgItem.nImgID);
	}
	OutputMatchLinks(&mtLinkMap, &vStripFiles, imgIDVec, minOverlap, strTrackName.GetBuffer(0));
	tieNum = ptLinks.size();

	//////////////////////////////////////////////////////////////////////////
	// 生成各影像的连接点文件
	for( i=0; i<vStripFiles.size(); i++ )
	{
		orsString outputFile = vStripFiles[i] + _T(".tiePts.txt");
		Align_Image imgItem1=pImgList->GetAt(i);
//		i_ID = imgItem1.nImgID;

		FILE *fpOut;

		fpOut = fopen( outputFile, 	"wt");

		if(i < imgNum)
		{
			imgReader1 = imageService->openImageFile(vStripFiles[i]);
//			pImgGeo1 = imgReader1->GetImageGeometry();
		}

		if( NULL != fpOut ) {
			mtLinkMapITER iter, next;	
			int k=0;
			for( iter = ptLinks.begin(); iter != ptLinks.end(); iter++ )
			{
				if( !mtLinkMap.isVaild(iter) )
					continue;

				long tieNum = iter->second.count;
				if( tieNum < 100 && tieNum >= minOverlap ) 
				{
					k++;
					ors_int32 iImg = mtLinkMap.getImgNum( iter->first );//内部会对imgID-1						
					ors_int64 uniqueId =	mtLinkMap.getRootId( iter->first );

					if( i == iImg )	{
						
						double xi, yi;
						xi = iter->second.x;
						yi = iter->second.y;

// 						if(i<lidNum)
// 						{
// 							pImgGeo1->Project(iter->second.x, iter->second.y, 
// 								iter->second.z, &xi, &yi);
// 						}
						fprintf( fpOut, "%I64d %.6f %.6f %.2f %f\n", uniqueId, xi, yi, 
							/*iter->second.scale*/0, /*iter->second.orient*/0 );										
					}

					////////////////////////////////////////////////////
					
					next = iter;
					for( j=1; j<tieNum; j++)
					{
						next = ptLinks.find( next->second.next );

						iImg = mtLinkMap.getImgNum( next->first );
						if( i == iImg )	{

							double xi, yi;
							xi = next->second.x;
							yi = next->second.y;

// 							if(i<lidNum)
// 							{
// 								pImgGeo1->Project(next->second.x, next->second.y, 
// 									next->second.z, &xi, &yi);
// 							}
							fprintf( fpOut, "%I64d %.6f %.6f %.2f %f\n", uniqueId, xi, yi, 
								/*iter->second.scale*/0, /*iter->second.orient*/0 );										
						}
					}
				}
			}

			fclose( fpOut );
		}
	}

}



//#include "ATNLib/ATNIO.h"
//#include "lastools/lasreader.h"
//#include "lastools/laswriter.h"
#include "orsLidarGeometry/orsILidarGeoModel.h"
#include "orsLidarGeometry/orsIPOS.h"
#include "orsGuiBase/orsIPropDlg.h"
#include "orsPointCloud/orsIPointCloudReader.h"
#include "orsPointCloud/orsIPointCloudWriter.h"
//利用ATN文件测试几何定位是否正确
void CMainFrame::OnTestLidgeo()
{
	CFileDialog	atndlg(TRUE, "atn.las", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("ATN File (*.las)|*.las|All Files(*.*)|*.*||"));

	if(atndlg.DoModal()!=IDOK)
		return;
	CString atnFile = atndlg.GetPathName();
	CString difFile;

	difFile = atnFile;
	int k = difFile.ReverseFind('.');
	difFile = difFile.Left(k);
	difFile += "_dif.las";

	ref_ptr<orsIPointCloudReader> atnreader = ORS_CREATE_OBJECT( orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_ATN );
	if(atnreader.get() == NULL)
		return;

	//	CATNReader	atnreader;
	if(!atnreader->open(atnFile))
		return;

	CFileDialog	lasdlg(TRUE, "project.las", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("project File (*.las)|*.las|All Files(*.*)|*.*||"));
	if(lasdlg.DoModal()!=IDOK)
		return;
	CString lasFile = lasdlg.GetPathName();

	ref_ptr<orsIPointCloudReader> lasreader = ORS_CREATE_OBJECT( orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_LAS );
	if(!lasreader->open(lasFile))
		return;

	ref_ptr<orsIPointCloudWriter> difwriter = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	if(!difwriter->open(difFile, lasPOINT))
		return;

	//默认是WGS84地理坐标
	orsString wktHcs = "GEOGCS[\"WGS 84\", \
					   DATUM[\"WGS_1984\", \
					   SPHEROID[\"WGS 84\", 6378137,298.257223563, \
					   AUTHORITY[\"EPSG\",\"7030\"]], \
					   TOWGS84[0,0,0,0,0,0,0], \
					   AUTHORITY[\"EPSG\",\"6326\"]], \
					   PRIMEM[\"Greenwich\", 0, \
					   AUTHORITY[\"EPSG\",\"8901\"]], \
					   UNIT[\"degree\",0.0174532925199433, \
					   AUTHORITY[\"EPSG\",\"9108\"]], \
					   AUTHORITY[\"EPSG\",\"4326\"]]";

	orsString wktVCS; //目标投影
	ref_ptr<orsIPropDlg> pDlg = ORS_CREATE_OBJECT( orsIPropDlg, "ors.extension.propDlg.hcs" );

	ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();

	pProp->addAttr("hcs", wktVCS );

	if( pDlg->runDlg( pProp.get(), "hcs", NULL ) )
	{
		pProp->getAttr("hcs", wktVCS );
	}

	ref_ptr<orsIPOS> posTrans = ORS_CREATE_OBJECT(orsIPOS, ORS_LIDARGEOMETRY_POS_WGS84);
	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	orsLidSysParam_TYPE type;
	orsLidSysParam extParam;

	type = LidGeo_rph;
	memset(&extParam, 0, sizeof(orsLidSysParam));
	lidGeo->setSysParam(/*type,*/ &extParam);
	lidGeo->setSRS(wktVCS, NULL);

// 	if( NULL == posTrans.get() )	{
// 		return;
// 	}

	posTrans->SetSRS(wktHcs, wktVCS);
	posTrans->SetTangentPlane_AnchorPoint(34.44117222*PI/180, 113.10243611*PI/180, 0);
	posTrans->SetCentralMeridian(111.0*PI/180);
	posTrans->SetUserCoordType(ORS_igctProjected);

	//ATNPoint atnpt;
	double xyz[3];
	double maxX=-100000.0, maxY=-100000.0, maxZ=-100000.0;
	double minX=100000.0, minY=100000.0, minZ=100000.0;
	while (atnreader->read_point(xyz))
	{
// 		double p[3];
// 		posTrans->test_atn(&atnpt, p);
//		continue;

		LidGeo_RawPoint rawpt;
		orsPOSParam rawPos, dstPos;

// 		rawPos.position.X = p[0];/*atnpt.x;*/
// 		rawPos.position.Y = p[1];/*atnpt.y;*/
// 		rawPos.position.Z = p[2];/*atnpt.z;*/

//		posTrans->WGS84_To_UTM(&rawPos, &dstPos, 1);
		
		double coord[3], diff_coord[3];
		lasreader->read_point(coord);
		double gpstime;
		gpstime = lasreader->get_gpstime();


// 		diff_coord[0] = coord[0]/* - dstPos.position.X*/;
// 		diff_coord[1] = coord[1]/* - dstPos.position.Y*/;
// 		diff_coord[2] = coord[2] - dstPos.position.Z;

// 		difwriter.write_point(diff_coord);
// 
// 		continue;
		orsPointObservedInfo obsInfo;
		atnreader->get_point_observed_info(&obsInfo);

		rawPos.coord.X = obsInfo.pos.lon;
		rawPos.coord.Y = obsInfo.pos.lat;
		rawPos.coord.Z = obsInfo.pos.h;
		rawPos.r = obsInfo.roll;
		rawPos.p = obsInfo.pitch;
		rawPos.h = obsInfo.heading;

		posTrans->WGS84_To_UTM(&rawPos, &dstPos, 1);

		rawpt.eoParam = /*rawPos*/dstPos;
		rawpt.range = obsInfo.range;
		rawpt.sAngle = obsInfo.scanAngle;

		lidGeo->cal_Laserfootprint(&rawpt, 1);

		double coord_WGS84[3], coord_UTM[3], atn_UTM[3];
		double Rn[9], RDst[9];

		coord_UTM[0]=rawpt.X;
		coord_UTM[1]=rawpt.Y;
		coord_UTM[2]=rawpt.Z;
		posTrans->WGS84_To_UTM(xyz, Rn, atn_UTM, RDst);

// 		coord_WGS84[0]=rawpt.X;
// 		coord_WGS84[1]=rawpt.Y;
// 		coord_WGS84[2]=rawpt.Z;
// 		posTrans->WGS84_To_UTM(coord_WGS84, Rn, coord_UTM, RDst);
// 
// 		coord_WGS84[0] = xyz[0];
// 		coord_WGS84[1] = xyz[1];
// 		coord_WGS84[2] = xyz[2];
// 		posTrans->WGS84_To_UTM(coord_WGS84, Rn, atn_UTM, RDst);

		diff_coord[0] = coord_UTM[0] - atn_UTM[0];
		diff_coord[1] = coord_UTM[1] - atn_UTM[1];
		diff_coord[2] = coord_UTM[2] - atn_UTM[2];
		
		if(diff_coord[0]>maxX)	maxX=diff_coord[0];
		if(diff_coord[0]<minX)	minX=diff_coord[0];
		if(diff_coord[1]>maxY)	maxY=diff_coord[1];
		if(diff_coord[1]<minY)	minY=diff_coord[1];
		if(diff_coord[2]>maxZ)	maxZ=diff_coord[2];
		if(diff_coord[2]<minZ)	minZ=diff_coord[2];


 		difwriter->write_point(diff_coord);
	}

	atnreader->close();
	lasreader->close();
	difwriter->close();
}

#include "orsLidarGeometry/orsITrajectory.h"
void CMainFrame::OnTestTrajectory()
{
	CFileDialog	atndlg(TRUE, "atn.las", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("ATN File (*.las)|*.las|All Files(*.*)|*.*||"));

	if(atndlg.DoModal()!=IDOK)
		return;
	CString atnFile = atndlg.GetPathName();
	CString difFile;

	difFile = atnFile;
	int k = difFile.ReverseFind('.');
	difFile = difFile.Left(k);
	difFile += "_trj_dif.las";

	ref_ptr<orsIPointCloudReader> atnreader = ORS_CREATE_OBJECT( orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_ATN );
	if(!atnreader->open(atnFile))
		return;

	CFileDialog	trjdlg(TRUE, ".trj", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("trajectory file (*.trj)|*.trj|All Files(*.*)|*.*||"));
	if(trjdlg.DoModal()!=IDOK)
		return;
	CString trjFile = trjdlg.GetPathName();

	ref_ptr<orsITrajectory> trajectory = ORS_CREATE_OBJECT(orsITrajectory, ORS_LIDARGEOMETRY_TRAJECTORY_TRJ);

	if(!trajectory->Initialize(trjFile))
		return ;

	ref_ptr<orsIPointCloudWriter> difwriter = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	if(!difwriter->open(difFile, lasPOINT))
		return;

	//默认是WGS84地理坐标
	orsString wktHcs = "GEOGCS[\"WGS 84\", \
					   DATUM[\"WGS_1984\", \
					   SPHEROID[\"WGS 84\", 6378137,298.257223563, \
					   AUTHORITY[\"EPSG\",\"7030\"]], \
					   TOWGS84[0,0,0,0,0,0,0], \
					   AUTHORITY[\"EPSG\",\"6326\"]], \
					   PRIMEM[\"Greenwich\", 0, \
					   AUTHORITY[\"EPSG\",\"8901\"]], \
					   UNIT[\"degree\",0.0174532925199433, \
					   AUTHORITY[\"EPSG\",\"9108\"]], \
					   AUTHORITY[\"EPSG\",\"4326\"]]";

	orsString wktVCS; //目标投影
	ref_ptr<orsIPropDlg> pDlg = ORS_CREATE_OBJECT( orsIPropDlg, "ors.extension.propDlg.hcs" );

	ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();

	pProp->addAttr("hcs", wktVCS );

	if( pDlg->runDlg( pProp.get(), "hcs", NULL ) )
	{
		pProp->getAttr("hcs", wktVCS );
	}

	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	orsLidSysParam_TYPE type;
	orsLidSysParam extParam;
	type = LidGeo_rph;
	memset(&extParam, 0, sizeof(orsLidSysParam));
	lidGeo->setSysParam(/*type,*/ &extParam);


	ref_ptr<orsIPOS> posTrans = ORS_CREATE_OBJECT(orsIPOS, ORS_LIDARGEOMETRY_POS_WGS84);
	posTrans->SetSRS(wktHcs, wktVCS);
	posTrans->SetTangentPlane_AnchorPoint(40.0*PI/180, 111.0*PI/180, 0);
	posTrans->SetCentralMeridian(111.0*PI/180);
	posTrans->SetUserCoordType(ORS_igctProjected);

//	ATNPoint atnpt;
	double xyz[3];
	while (atnreader->read_point(xyz))
	{
		orsPOINT3D gps_pos;
		double Rn[9], RDst[9];
		double gpstime = atnreader->get_gpstime();
		trajectory->Interpolate_linear(gpstime, &gps_pos, Rn);

		double coord_WGS84[3], coord_UTM[3];
		double atn_UTM[3];

		orsPointObservedInfo info;
		atnreader->get_point_observed_info(&info);

		lidGeo->cal_Laserfootprint(&gps_pos, Rn, info.range, info.scanAngle, coord_WGS84, coord_WGS84+1, coord_WGS84+2);

		double p[3];
// 		atnpt.latitude = gps_pos.Y;
// 		atnpt.longitude = gps_pos.X;
// 		atnpt.altitude = gps_pos.Z;

//		posTrans->test_atn(&atnpt, p, &gps_pos, Rn);
		//continue;
		
		posTrans->WGS84_To_UTM(coord_WGS84, Rn, coord_UTM, RDst);
		coord_WGS84[0] = xyz[0];
		coord_WGS84[1] = xyz[1];
		coord_WGS84[2] = xyz[2];
		posTrans->WGS84_To_UTM(coord_WGS84, Rn, atn_UTM, RDst);
		
		double diff_coord[3];
		diff_coord[0] = coord_UTM[0]/* - dstPos.position.X*/;
		diff_coord[1] = coord_UTM[1]/* - dstPos.position.Y*/;
		diff_coord[2] = coord_UTM[1] - atn_UTM[1];

		difwriter->write_point(diff_coord);
	}

	atnreader->close();

	difwriter->close();
}

#include "orsMath\orsIMatrixService.h"
void CMainFrame::OnTestInterpolate()
{
	CFileDialog	atndlg(TRUE, "atn.las", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("ATN File (*.las)|*.las|All Files(*.*)|*.*||"));

	if(atndlg.DoModal()!=IDOK)
		return;

	CString atnFile = atndlg.GetPathName();

	ref_ptr<orsIPointCloudReader> atnreader=ORS_CREATE_OBJECT(orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_ATN);
	if(!atnreader->open(atnFile))
		return;

	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	orsLidSysParam_TYPE type;
	orsLidSysParam extParam;
	type = LidGeo_rph;
	memset(&extParam, 0, sizeof(orsLidSysParam));
	lidGeo->setSysParam(/*type,*/ &extParam);

	orsIMatrixService *matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );
	if( NULL == matrixService )	{
		//		msg->logPrint( ORS_LOG_ERROR, "Can not get matrixService" );
		return;
	}

//	ATNPoint atnpt;
	double xyz[3];
	while (atnreader->read_point(xyz))
	{
		orsPOINT3D gps_pos;
		double Rn[9], RDst[9];
		orsPointObservedInfo info;
		atnreader->get_point_observed_info(&info);

		gps_pos.lat = info.pos.lat;
		gps_pos.lon = info.pos.lon;
		gps_pos.h = info.pos.h;

		matrixService->RotateMatrix_rph(info.roll, info.pitch, info.heading, Rn);

		double X, Y, Z;
		lidGeo->cal_Laserfootprint(&gps_pos, Rn, info.range, info.scanAngle, &X, &Y, &Z);
		
		double range, sAngle;
//		lidGeo->estimate_range_scanAngle(&gps_pos, Rn, Y, X, Z, &range, &sAngle);

		double dif[3];
		dif[0] = range - info.range;
		dif[1] = sAngle - info.scanAngle;

	}
}

#include "ATN2LASDlg.h"
#include "orsLidarGeometry\LidGeo_def.h"
#include "orsLidarGeometry\orsAtnTransform.h"
//格式转换：ATN -> LAS
//实现的功能：
// 1.系统误差改正
// 2.投影转换
const double RAD2DEG = 57.295779513082320876798154814114;
const double DEG2RAD = 0.01745329251994329576923690768489;
void CMainFrame::OnPreproccessAtn2las()
{
	CATN2LASDlg Dlg;

	if(Dlg.DoModal()!=IDOK)
		return;

	orsLidSysParam_TYPE param_type;
	param_type = LidGeo_rph;
	orsLidSysParam oriParam;
	memset(&oriParam, 0, sizeof(orsLidSysParam));

	ref_ptr<orsILidarGeoModel> orglidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	ref_ptr<orsILidarGeoModel> dstlidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);

	orglidGeo->setSysParam(/*param_type,*/ &oriParam);

	ref_ptr<orsIPointCloudReader> atnreader = ORS_CREATE_OBJECT( orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_ATN );
	ref_ptr<orsIPointCloudWriter> laswriter = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );

	ref_ptr<orsIProcessMsgBar> process;
	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	process->InitProgressBar("数据转换...", "wait...", Dlg.m_atnNameVec.GetSize());

// 	ref_ptr<orsISpatialReference> orgCoordSys = getSRService()->CreateSpatialReference();
// 	orgCoordSys->importFromWkt( Dlg.m_srcWkt );
// 
// 	ref_ptr<orsISpatialReference> dstCoordSys = getSRService()->CreateSpatialReference();
// 	dstCoordSys->importFromWkt( Dlg.m_targetWkt );
// 
// 	ref_ptr<orsICoordinateTransform> coordTransform = getSRService()->CreateCoordinateTransform();
// 	coordTransform->Initialize( orgCoordSys.get(), dstCoordSys.get());
	
// 	orsAtnTransform  *atnTrans = new orsAtnTransform;
// 	atnTrans->SetSRS(Dlg.m_srcWkt, Dlg.m_targetWkt);

// 	if(Dlg.m_bTP_ATN)
// 	{
// 		orsString wkt = "TangentPLANE";
// 		lidGeo->setSRS(wkt, NULL);
// 	}
// 	else
// 	{
//		lidGeo->setSRS(NULL, NULL);
//	}

	oriParam.boresight_angle[0] = Dlg.m_misalign_roll;
	oriParam.boresight_angle[1] = Dlg.m_misalign_pitch;
	oriParam.boresight_angle[2] = Dlg.m_misalign_heading;

	if(Dlg.m_bMisalign_6Param)
	{
		oriParam.lever_arm[0] = Dlg.m_misalign_tx;
		oriParam.lever_arm[1] = Dlg.m_misalign_ty;
		oriParam.lever_arm[2] = Dlg.m_misalign_tz;
	}
	
	if(Dlg.m_bCheck_sa0)
	{
		oriParam.sa0 = Dlg.m_sa0;
		oriParam.sac = Dlg.m_sac;
	}

	if(Dlg.m_bCheck_SemiXYZ)
	{
		oriParam.semiXYZ[0] = Dlg.m_semi_dx;
		oriParam.semiXYZ[1] = Dlg.m_semi_dy;
		oriParam.semiXYZ[2] = Dlg.m_semi_dz;
	}

	if(Dlg.m_bCheck_dp)
		oriParam.drange = Dlg.m_dp;

	orglidGeo->setSRS(Dlg.m_srcWkt, NULL);
	dstlidGeo->setSysParam(/*param_type,*/ &oriParam);
	dstlidGeo->setSRS(Dlg.m_srcWkt, Dlg.m_targetWkt);

	orsLidSysParam calib_param;
	memset(&calib_param, 0, sizeof(orsLidSysParam));
	calib_param.boresight_angle[0] = oriParam.boresight_angle[0];
	calib_param.boresight_angle[1] = oriParam.boresight_angle[1];
	calib_param.boresight_angle[2] = oriParam.boresight_angle[2];

	CString atnName;
	CString lasName;
	CString pureName;
	int pos;
	for(int i=0; i<Dlg.m_atnNameVec.GetSize(); i++)
	{
		atnName = Dlg.m_atnNameVec[i];
		if(!atnreader->open(atnName))
			continue;

		pos = atnName.ReverseFind('_');
		atnName = atnName.Left(pos);
		pos = atnName.ReverseFind('\\');
		pureName = atnName.Right(atnName.GetLength()-pos-1);
		lasName = Dlg.m_strLasDir + "\\" + pureName + ".LAS";

		if(!laswriter->open(lasName, lasPOINT_GPS))
			continue;

		double xyz1[3], xyz[3];

		while (atnreader->read_point(xyz1))
		{
// 			LidGeo_RawPoint rawpt;
// 			orsPOSParam rawPos;

//			double coord[3], diff_coord[3];
			
			unsigned short intensity = atnreader->get_intensity();
			unsigned char return_number = atnreader->get_return_number();
			unsigned char number_of_returns_of_given_pulse = atnreader->get_number_of_returns();
			unsigned char classification = atnreader->get_classification();
			double gpstime = atnreader->get_gpstime();

			unsigned char scan_direction_flag = atnreader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = atnreader->get_edge_of_flight_line();
			char scan_angle_rank = atnreader->get_scan_angle_rank();
			unsigned char user_data = atnreader->get_user_data();
			unsigned short point_source_ID = atnreader->get_point_source_ID();

			orsPointObservedInfo obsInfo1/*, obsInfo*/;
 			atnreader->get_point_observed_info(&obsInfo1);

			LidPt_SurvInfo pts;
			pts.x = xyz1[0];
			pts.y = xyz1[1];
			pts.z = xyz1[2];

			pts.POS_Info.coord = obsInfo1.pos;
			pts.POS_Info.r = obsInfo1.roll;
			pts.POS_Info.p = obsInfo1.pitch;
			pts.POS_Info.h = obsInfo1.heading;

			//为了避免ALS PP在导出数据过程中做的未知的改正，用斜距进行计算
			orglidGeo->cal_RangeVec(&pts, 1);
			dstlidGeo->cal_Laserfootprint(&pts, 1);

			xyz[0] = pts.x; xyz[1] = pts.y; xyz[2] = pts.z;

//			_calLaserPosition(&pts, 1, &calib_param, NULL, LidAdj_NoneSysError);

			ATNPoint  atnPt;
			atnPt.obsInfo = obsInfo1;
	
// 			_calLaserPosition(&atnPt, 1, calib_param, NULL, Calib_None);
// 
// 			if(fabs(pts.z-xyz[2])>0.01)
// 			{
// 				printf("");
// 			}
// 
// 			if(fabs(atnPt.z - xyz[2])>0.01)
// 			{
// 				printf("");
// 			}
// 
// 			xyz[0] = atnPt.x; xyz[1] = atnPt.y; xyz[2] = atnPt.z;
			laswriter->write_point(xyz, intensity, return_number, 
				number_of_returns_of_given_pulse, scan_direction_flag, edge_of_flight_line, classification, scan_angle_rank, user_data, gpstime);
		}

		atnreader->close();
		laswriter->close();
		process->SetPos(i+1);
	}

//	if(atnTrans) delete atnTrans;
}

#include "PointCloudRasterize/PointRasterize.h"
#include "DlgAtnRasterize.h"
void CMainFrame::OnCombinedatPointcloudrasterize()
{
	CDlgAtnRasterize dlg;
	if(dlg.DoModal()!=IDOK)
		return	;

	orsPointAttributionFlag  flag;
	double gsize;

	CPointRasterize rast(getPlatform());

	ref_ptr<orsIProcessMsg> pMsg;
	pMsg = ORS_CREATE_OBJECT( orsIProcessMsg, ORS_PROCESSMSG_BAR_DEFAULT );

	if(dlg.m_selAttType == FALSE)
		flag = PA_ELEVATION;
	else
		flag = PA_INTENSITY;

	gsize = dlg.m_gridSize;
//	gsize = dlg.m_gridSize/6378140/*/PI*180*/; //转弧度

	int num=dlg.m_atnNameVec.GetSize();
	for(int i=0; i<num; i++)
	{
		CString atnName = dlg.m_atnNameVec[i];
		CString gridName;
		int pos = atnName.ReverseFind('.');
		gridName = atnName.Left(pos);

		gridName += ".tif";
		
		orsString proj;//投影信息
		orsString prjName = atnName;
		pos = prjName.reverseFind('.');
		prjName = prjName.left(pos);
		prjName += ".prj";

		FILE *fp = fopen( prjName, "rt");
		if( fp )
		{
			fseek( fp, 0L, SEEK_END );
			long len = ftell( fp );
			char *buf = new char[len+2];
			fseek( fp, 0L, SEEK_SET );
			fread( buf, 1, len, fp );
			fclose( fp );

			buf[len] = 0;
			proj = buf;
			delete buf;
		}
	

		rast.PointToGrid(dlg.m_atnNameVec[i].GetBuffer(0), gridName.GetBuffer(0), 
			gsize, gsize, pMsg.get(), flag, 0, 50, proj.c_str());
	}
}

#include "DlgSetTangentPlane.h"
#include "orsLidarGeometry/orsAtnTransform.h"
//WGS84地理坐标转切平面坐标系
void CMainFrame::OnPreproccessATN2TPlane()
{
	CDlgSetTangentPlane dlg;

	dlg.m_latitude = 0;
	dlg.m_longitude = 0;
	if(dlg.DoModal()!=IDOK)
		return;

	double lat, lon;
	lat = dlg.m_latitude*PI/180;
	lon = dlg.m_longitude*PI/180;

	orsAtnTransform atnTrans;
	atnTrans.SetAnchor(lat, lon, 0);
//	atnTrans.SetSRS(NULL, NULL);

	CString strOutputDir = dlg.m_strOutputDir;

	ref_ptr<orsIProcessMsgBar> process;
	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	process->InitProgressBar("数据转换...", "wait...", dlg.m_atnNameVec.GetSize());
	CString strFileName;
	CString strInput, strOutput;

	ref_ptr<orsIPointCloudReader> atnReader = ORS_CREATE_OBJECT(orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_ATN);
	ref_ptr<orsIPointCloudWriter> atnWriter = ORS_CREATE_OBJECT(orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_ATN);

	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	ref_ptr<orsILidarGeoModel> lidGeoWGS84 = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	lidGeoWGS84->setSRS(wktWGS84.c_str(), NULL);

	for(int i=0; i<dlg.m_atnNameVec.GetSize(); i++)
	{
		strInput=dlg.m_atnNameVec[i];
		strFileName=_ExtractFileName(strInput);
		strOutput=strOutputDir+"\\"+strFileName+"_TP.LAS";

		//逐条带转换   
		atnReader->open(strInput.GetBuffer(0));
		double scale[3];
		double offset[3];

		atnReader->get_scale_factor(scale);
		atnReader->get_offset(offset);
		atnWriter->open(strOutput.GetBuffer(0), lasPOINT_ATN/*, 1e-6, 1e-6, 1e-6*/);

		double xyz1[3], xyz2[3];
		while(atnReader->read_point(xyz1))
		{
			orsPointObservedInfo obs1, obs2;
			atnReader->get_point_observed_info(&obs1);

			double gpstime = atnReader->get_gpstime();
			unsigned short intensity = atnReader->get_intensity();

			unsigned char classificaiton = atnReader->get_classification();
			int return_number = atnReader->get_return_number();
			int number_of_returns = atnReader->get_number_of_returns();
			unsigned char scan_direction_flag = atnReader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = atnReader->get_edge_of_flight_line();
			char scan_angle_rank = atnReader->get_scan_angle_rank();
			unsigned char user_data = atnReader->get_user_data();
			unsigned short point_source_ID = atnReader->get_point_source_ID();

			obs2 = obs1;
			//关键的转换在这里，下面都是测试用的
			atnTrans.WGS84_To_TangentPlane(xyz1, &obs1, xyz2, &obs2); 

			double posT[3];
			double Rn[9];
			RotateMat_rph(obs2.roll, obs2.pitch, obs2.heading, Rn);

			//测试LidGeo计算出的坐标与原始坐标的差别
			lidGeo->cal_Laserfootprint(&obs2.pos, Rn, obs2.range, obs2.scanAngle, 
				posT, posT+1, posT+2);

			LidPt_SurvInfo pts1, pts2;
			pts1.x = xyz1[0];
			pts1.y = xyz1[1];
			pts1.z = xyz1[2];

			pts1.POS_Info.coord = obs1.pos;
			pts1.POS_Info.r = obs1.roll;
			pts1.POS_Info.p = obs1.pitch;
			pts1.POS_Info.h = obs1.heading;

			//为了避免ALS PP在导出数据过程中做的未知的改正，用斜距进行计算
			lidGeoWGS84->cal_RangeVec(&pts1, 1);
			lidGeoWGS84->cal_Laserfootprint(&pts1, 1);

			pts2.x = xyz2[0];
			pts2.y = xyz2[1];
			pts2.z = xyz2[2];

			pts2.POS_Info.coord = obs2.pos;
			pts2.POS_Info.r = obs2.roll;
			pts2.POS_Info.p = obs2.pitch;
			pts2.POS_Info.h = obs2.heading;

			//为了避免ALS PP在导出数据过程中做的未知的改正，用斜距进行计算
			lidGeo->cal_RangeVec(&pts2, 1);
			lidGeo->cal_Laserfootprint(&pts2, 1);


			atnWriter->write_point(xyz2, intensity, (unsigned char)return_number, (unsigned char)number_of_returns,
				scan_direction_flag, edge_of_flight_line, classificaiton, scan_angle_rank, user_data, point_source_ID, gpstime, &obs2);
		}

		atnReader->close();
		atnWriter->close();

		process->SetPos(i+1);
	}
}

//WGS84转UTM投影坐标系
void CMainFrame::OnPreproccessAtn2utm()
{
	CDlgATN2UTM dlg;

	if(dlg.DoModal()!=IDOK)
		return;

	CString strOutputDir = dlg.m_strOutDir;
	orsString wktVCS; //目标投影
	wktVCS = dlg.m_targetWkt;
//	double cenMeridian = dlg.m_centralMeridian;

	ref_ptr<orsIProcessMsgBar> process;
	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	process->InitProgressBar("数据转换...", "wait...", dlg.m_atnNameVec.GetSize());
	CString strFileName;
	CString strInput, strOutput;

	ref_ptr<orsIPointCloudReader> atnReader = ORS_CREATE_OBJECT(orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_ATN);
	ref_ptr<orsIPointCloudWriter> atnWriter = ORS_CREATE_OBJECT(orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_ATN);

	//源投影默认是WGS84地理坐标
	orsString wktHcs = "GEOGCS[\"WGS 84\", \
					   DATUM[\"WGS_1984\", \
					   SPHEROID[\"WGS 84\", 6378137,298.257223563, \
					   AUTHORITY[\"EPSG\",\"7030\"]], \
					   TOWGS84[0,0,0,0,0,0,0], \
					   AUTHORITY[\"EPSG\",\"6326\"]], \
					   PRIMEM[\"Greenwich\", 0, \
					   AUTHORITY[\"EPSG\",\"8901\"]], \
					   UNIT[\"degree\",0.0174532925199433, \
					   AUTHORITY[\"EPSG\",\"9108\"]], \
					   AUTHORITY[\"EPSG\",\"4326\"]]";

	
	ref_ptr<orsIPOS> posTrans = ORS_CREATE_OBJECT(orsIPOS, ORS_LIDARGEOMETRY_POS_WGS84);
	ref_ptr<orsILidarGeoModel> lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	ref_ptr<orsILidarGeoModel> lidGeoUTM = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);
	orsLidSysParam_TYPE type;
	orsLidSysParam extParam;

	orsAtnTransform  *atnTrans = new orsAtnTransform;
	atnTrans->SetSRS(wktHcs, wktVCS);

// 	type = LidGeo_rph;
// 	memset(&extParam, 0, sizeof(orsLidSysExtraParam));
// 	lidGeo->setSysParam(type, &extParam);
	lidGeo->setSRS(wktHcs, NULL);
	lidGeoUTM->setSRS(wktVCS, NULL);

	posTrans->SetSRS(wktHcs, wktVCS);
//	posTrans->SetTangentPlane_AnchorPoint(34.44117222*PI/180, 113.10243611*PI/180, 0);
//	posTrans->SetCentralMeridian(cenMeridian*PI/180);
	posTrans->SetUserCoordType(ORS_igctProjected);

	CString prjName;
	int pos;
	FILE *fProj=NULL;
	for(int i=0; i<dlg.m_atnNameVec.GetSize(); i++)
	{
		strInput=dlg.m_atnNameVec[i];
		strFileName=_ExtractFileName(strInput);
		strOutput=strOutputDir+"\\"+strFileName+"_UTM.LAS";

		pos = strOutput.ReverseFind('.');

		prjName = strOutput.Left(pos);
		prjName += ".prj";	//投影文件名


		//逐条带转换   
		atnReader->open(strInput.GetBuffer(0));
		double scale[3];
		double offset[3];

		atnReader->get_scale_factor(scale);
		atnReader->get_offset(offset);
		atnWriter->open(strOutput.GetBuffer(0), lasPOINT_ATN/*, 1e-6, 1e-6, 1e-6*/);

		double xyz1[3], xyz2[3], xyz3[3], xyz4[3];
		while(atnReader->read_point(xyz1))
		{
			orsPointObservedInfo obs1, obs2;
			atnReader->get_point_observed_info(&obs1);

			double gpstime = atnReader->get_gpstime();
			unsigned short intensity = atnReader->get_intensity();

			unsigned char classificaiton = atnReader->get_classification();
			int return_number = atnReader->get_return_number();
			int number_of_returns = atnReader->get_number_of_returns();

			unsigned char scan_direction_flag = atnReader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = atnReader->get_edge_of_flight_line();
			char scan_angle_rank = atnReader->get_scan_angle_rank();
			unsigned char user_data = atnReader->get_user_data();
			unsigned short point_source_ID = atnReader->get_point_source_ID();

			obs2 = obs1;
			//坐标转换核心
			atnTrans->WGS84_To_UTM(xyz1, &obs1, xyz2, &obs2);

			//下面都是用来测试的
/*			LidPt_SurvInfo pts1, pts2;
			pts1.x = xyz1[0];
			pts1.y = xyz1[1];
			pts1.z = xyz1[2];

			pts1.POS_Info.coord = obs1.pos;
			pts1.POS_Info.r = obs1.roll;
			pts1.POS_Info.p = obs1.pitch;
			pts1.POS_Info.h = obs1.heading;

			//为了避免ALS PP在导出数据过程中做的未知的改正，用斜距进行计算
			lidGeo->cal_RangeVec(&pts1, 1);
			lidGeo->cal_Laserfootprint(&pts1, 1);

			pts2.x = xyz2[0];
			pts2.y = xyz2[1];
			pts2.z = xyz2[2];

			pts2.POS_Info.coord = obs2.pos;
			pts2.POS_Info.r = obs2.roll;
			pts2.POS_Info.p = obs2.pitch;
			pts2.POS_Info.h = obs2.heading;

			//为了避免ALS PP在导出数据过程中做的未知的改正，用斜距进行计算
			lidGeoUTM->cal_RangeVec(&pts2, 1);
			lidGeoUTM->cal_Laserfootprint(&pts2, 1);*/

			atnWriter->write_point(xyz2, intensity, (unsigned char)return_number, (unsigned char)number_of_returns,
				scan_direction_flag, edge_of_flight_line, classificaiton, scan_angle_rank, user_data, point_source_ID, gpstime, &obs2);
		}

		atnReader->close();
		atnWriter->close();

		fProj = fopen(prjName.GetBuffer(0), "wt");
		if(fProj)
		{
			fprintf(fProj, "%s\n", wktVCS.c_str() );
			fclose(fProj);	

			fProj=NULL;
		}

		process->SetPos(i+1);
	}

	if(atnTrans) delete atnTrans;
}


//相片畸变纠正
#include "ImageRectify/orsImageRectify_Inor.h"
void CMainFrame::OnPreproccessImagedistortioncorrecting()
{
	ImageRectifyInor_SetPlatform(getPlatform());

	CImageRectify_Inor   rectify;

	rectify.Run();
}

void CMainFrame::OnCameraboresightExportimagelist()
{
	CFileDialog  dlg(FALSE,"image list file",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"image list file(*.txt)|*.txt||",NULL);	 

	if(dlg.DoModal()!=IDOK)
		return;

	CString	strPathName=dlg.GetPathName();

	FILE *fp=NULL;
	fp=fopen(strPathName.GetBuffer(0), "w");

	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i;
	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image line=pImgList->GetAt(i);

		fprintf(fp, "%s\n", line.ImgName.GetBuffer(0));
	}

	fclose(fp);
}

#include "CombineAdjustment/IntATManager.h"
void CMainFrame::OnCameraboresightRuncalib()
{
	CFileDialog  dlg(TRUE,"txt",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"连接点文件(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString  strTirPtFile=dlg.GetPathName();

	orsCamSysExtraParam_TYPE calibType = CamGeo_rph;
	orsCamSysExtraParam      calibParam;

	calibParam.lever_arm[0]=calibParam.lever_arm[1]=calibParam.lever_arm[2]=0;
	calibParam.boresight_angle[0]=calibParam.boresight_angle[1]=calibParam.boresight_angle[2]=0;

	if(m_AlgPrj.GetLidList()->GetSize()>0)
	{//存在点云条带，则利用点云更新相片外方位元素
		CIntATManager  atManager(getPlatform());

		lensDistortion lesDist;
		memset(&lesDist, 0, sizeof(lensDistortion));
		lesDist.k0 = 8.560790e-003;
		lesDist.k1 = -2.050420e-005;
		lesDist.k2 = 5.819490e-009;

		atManager.setOutputDir("H:\\AT_experiment\\baotou20110930\\CombinedAT\\CamCalib\\result");

		double moutAngle[3];
		moutAngle[0] = 180.0*RAD_PER_DEG;
		moutAngle[1] = 0;
		moutAngle[2] = 90.0*RAD_PER_DEG;
		atManager.setCameraParam(-0.0275,-0.4783,59.839,7162,5389,0.0068,0.0068, lesDist, moutAngle);

		atManager.setAlignPrj(&m_AlgPrj);
//		atManager.setTiePoints(strTirPtFile.GetBuffer(0));

		atManager.get_OrientedArchorsFromLIDAR(0.1);

		atManager.camera_calibration_BA(strTirPtFile.GetBuffer(0), &calibParam, calibType, POS_IntepolatedFromLIDAR);

//		atManager.camera_calibration_IP(strTirPtFile.GetBuffer(0), &calibParam, calibType, POS_IntepolatedFromLIDAR);
	}


}


void CMainFrame::OnImageorientationAbor()
{
	CFileDialog  dlg(TRUE,"txt",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"绝对定向点文件(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString  strTiePtFile=dlg.GetPathName();

	FILE *fp=NULL;

	fp=fopen(strTiePtFile.GetBuffer(0), "rt");



	if(fp)	fclose(fp);	fp=0;
}


//文件编码方式：先lidar， 后image
void CMainFrame::OnMatchlidimg()
{
	//导出连接点文件
	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("match init (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="初始匹配文件";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strTrackName=outputdlg.GetPathName();

/////////////////////////////////////////////////////////////////
	int minOverlap = 2; //最小的连接点数

//	myPtPair *ptPairVec=NULL;
	int maxLen=0;
	int matchNum, refineNum;
	orsMatchLinks	mtLinkMap;
	CATNPtIndex	index1(getPlatform()), index2(getPlatform());

	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );

	ref_ptr<orsIImageSourceReader> imgReader1, imgReader2;
	orsIImageGeometry *pImgGeo1=NULL, *pImgGeo2=NULL;

	ref_ptr<orsIAlg2DPointPairsCheck> pairsCheck;
	pairsCheck = ORS_CREATE_OBJECT( orsIAlg2DPointPairsCheck, ORS_ALG_2DPOINTPAIR_CHECK_DEFAULT );
//	pairsCheck = ORS_CREATE_OBJECT( orsIAlg2DPointPairsCheck, ORS_ALG_2DPOINTPAIR_CHECK_RANSAC );
		
	ref_ptr<orsI2DFeatureSet> featureSet0 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
	ref_ptr<orsI2DFeatureSet> featureSet1 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i, j, k;
	int lidNum = pLidList->GetSize();

	std::vector<orsAdjPtPAIR> ptPairVec;

	orsArray<orsString> vStripFiles;
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine LidItem=pLidList->GetAt(i);
		vStripFiles.push_back(orsString(LidItem.GridName.GetBuffer(0)));
	}
	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image imgItem=pImgList->GetAt(i);
		vStripFiles.push_back(orsString(imgItem.ImgName.GetBuffer(0)));
	}

	ImageMatchSetPlatform(getPlatform());

	//对Lidar进行两两匹配
	for(i=0; i<pLidList->GetSize()-1; i++)
	{
		Align_LidLine LidItem1=pLidList->GetAt(i);

		imgReader1 = imageService->openImageFile(LidItem1.GridName.GetBuffer(0));
		pImgGeo1 = imgReader1->GetImageGeometry();
		index1.Open(LidItem1.LasName);

		for(j=i+1; j<pLidList->GetSize(); j++)
		{
			Align_LidLine LidItem2=pLidList->GetAt(j);

			imgReader2 = imageService->openImageFile(LidItem2.GridName.GetBuffer(0));
			pImgGeo2 = imgReader2->GetImageGeometry();
			index2.Open(LidItem2.LasName);

			KeyMatch_images(LidItem1.GridName.GetBuffer(0), LidItem2.GridName.GetBuffer(0),
				&ptPairVec, matchNum, 8);

			//挑粗差
// 			double Mout[9];
// 			EstimateTransform_Affine(ptPairVec, matchNum, refineNum,3, Mout);

			refineNum = matchNum;
			if(matchNum==0)
				continue;

			const orsMatchedFtPAIR *pPairs;
			pPairs = pairsCheck->check_2DTransform( &ptPairVec[0], &refineNum, ORS_GEOMETRY_TRANSFORM2D_DLT, 2.0 );

			if( refineNum > 3 )	{
				orsString siftName0 = LidItem1.GridName.GetBuffer(0);
				siftName0 += _T(".sift.bin");
				featureSet0->load( siftName0 );

				orsString siftName1 = LidItem2.GridName.GetBuffer(0);
				siftName1 += _T(".sift.bin");
				featureSet1->load( siftName1 );

				orsKeyPOINT dstPt, srcPt;
				for( k=0; k<refineNum; k++ )
				{//提取物方坐标
					// iStrip
					featureSet0->getFetureVector(pPairs[k].dstFtId, &dstPt);

					// iStrip1
					featureSet1->getFetureVector(pPairs[k].srcFtId, &srcPt);

					//if(ptPairVec[k].bAccepted)
					mtLinkMap.AddPtPair( j, pPairs[k].srcFtId, &srcPt, i, pPairs[k].dstFtId, &dstPt );

// 					mtLinkMap.AddPtPair(iImg, vNumOfImgPts[iImg] + pPairs[k].srcFtId, &srcPt,
// 						iRefImg, vNumOfImgPts[iRefImg] + pPairs[k].dstFtId, &dstPt);
// 					orsPOINT3D dstPt, srcPt;
// 					//double xs, ys;
// 					//orsPOINT3D pt3D;
// 					pImgGeo1->IntersectWithZ(ptPairVec[k].x0, ptPairVec[k].y0, 0, &(dstPt.X), &(dstPt.Y));
// 					std::vector<ATNPoint> ptVec;
// 					double rms;
// 					POINT3D pt;
// 					pt.X=dstPt.X; pt.Y=dstPt.Y; pt.Z=dstPt.Z;
// 					index1.GetTriangleVertex(/*&dstPt*/&pt, 5, &ptVec, &rms);
// 					
// 					pImgGeo2->IntersectWithZ(ptPairVec[k].x1, ptPairVec[k].y1, 0, &(srcPt.X), &(srcPt.Y));
// 					pt.X=srcPt.X; pt.Y=srcPt.Y; pt.Z=srcPt.Z;
// 					index2.GetTriangleVertex(/*&srcPt*/&pt, 5, &ptVec, &rms);
				}
			}
		}
	}

	//对影像进行两两匹配
//	vStripFiles.clear();
	for(i=0; i<pImgList->GetSize()-1; i++)
	{
		Align_Image imgItem1=pImgList->GetAt(i);
		
		for(j=i+1; j<pImgList->GetSize(); j++)
		{
			Align_Image imgItem2=pImgList->GetAt(j);

			KeyMatch_images(imgItem1.ImgName.GetBuffer(0), imgItem2.ImgName.GetBuffer(0),
				&ptPairVec, matchNum, 8);

			//挑粗差
// 			double Mout[9];
// 			EstimateTransform_Epipolar(ptPairVec, matchNum, refineNum, &camera, 1);
			refineNum = matchNum;
			int numF, numH;
			numF = refineNum;
			const orsMatchedFtPAIR *pPairs;
			ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();
			pPairs = pairsCheck->check_Transform( &ptPairVec[0], &numF, ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_FMATRIX, pProp.get(), 0 );
			
			if(numF > 0)
				numH = numF;
			else
				assert( numF > 0 );
			pPairs = pairsCheck->check_Transform(&ptPairVec[0], &numH, ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_HMATRIX, pProp.get(), 0);
			
			if(numH > 0) 
				refineNum = numH;
			else
				assert(numH > 0);

			orsArray<double> arrF, arrH;
			printf( "%d %d\n", i,j);
			if( pProp->getAttr("F", arrF) ) {
				printf("F:");
				for (k = 0; k < 9; k++)
					printf("%lf ", arrF[k]);
				printf("\n");
			}
			if( pProp->getAttr("H", arrH) ) {
				printf( "H:");
				for (k = 0; k < 9; k++)
					printf("%lf ", arrH[k]);
				printf("\n");
			}
			printf("\n");

			if( refineNum > 6 )	{
				
				orsString siftName0 = imgItem1.ImgName.GetBuffer(0);
				siftName0 += _T(".sift.bin");
				featureSet0->load( siftName0 );

				orsString siftName1 = imgItem1.ImgName.GetBuffer(0);
				siftName1 += _T(".sift.bin");
				featureSet1->load( siftName1 );

				orsKeyPOINT dstPt, srcPt;
				for( k=0; k<refineNum; k++ )
				{//提取物方坐标
					// iStrip
					featureSet0->getFetureVector(pPairs[k].dstFtId, &dstPt);

					// iStrip1
					featureSet1->getFetureVector(pPairs[k].srcFtId, &srcPt);

					//if(ptPairVec[k].bAccepted)
					mtLinkMap.AddPtPair( j, pPairs[k].srcFtId, &srcPt, i, pPairs[k].dstFtId, &dstPt );
				}
			}
		}
	}

/*	if(pImgList->GetSize()>0){
		//导入相机文件
		CFileDialog	camdlg(TRUE, "cam", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("camera File (*.cam)|*.cam|All Files(*.*)|*.*||"));

		if(camdlg.DoModal()!=IDOK)
			return;

		CString strCamFile=camdlg.GetPathName();

		_iphCamera camera;
		camera.ReadCameraFile(strCamFile.GetBuffer(0));
	//lidar和影像之间按条带匹配
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine lidItem=pLidList->GetAt(i);

//		vStripFiles.push_back(orsString(line.GridName.GetBuffer(0)));
		for(j=0; j<pImgList->GetSize(); j++)
		{
			Align_Image imgItem=pImgList->GetAt(j);
			int stripID = (imgItem.nImgID - sFlag) / sFactor;
			if(stripID != i)
				continue;

			KeyMatch_Lidarimage_SIFT(lidItem.LasName.GetBuffer(0), lidItem.GridName.GetBuffer(0),
				imgItem.ImgName.GetBuffer(0), &ptPairVec, maxLen, matchNum);


			//挑粗差
			double Mout[9];
//   			EstimateTransform_Projection(ptPairVec, matchNum, refineNum, &camera, 1);
//   			if( refineNum > 6 )	
			{
				//导出控制点格式

				int k;
				for( k=0; k<matchNum; k++ )
				{
					orsPOINT3D dstPt, srcPt;
					dstPt.X = ptPairVec[k].xi0;
					dstPt.Y = ptPairVec[k].yi0;
					dstPt.Z = ptPairVec[k].zi0;

					srcPt.X = ptPairVec[k].xi1;
					srcPt.Y = ptPairVec[k].yi1;
					srcPt.Z = 0;
	
					if(ptPairVec[k].bAccepted)
						mtLinkMap.AddPtPair( j+lidNum, ptPairVec[k].ptId1, &srcPt, i, ptPairVec[k].ptId0, &dstPt );
				}
			}

			break;
		}
		break;
	}

	}*/


	int count = mtLinkMap.RemoveDuplicatePoints();
	printf("%d duplicate points are removed\n", count );

	mtLinkMap.CountOverlap( vStripFiles.size() );

	mtLinkMAP &ptLinks = mtLinkMap.GetMatchLinks();

	int tieNum = ptLinks.size();
	std::vector<int> IDVec;
	for(i=0; i<pLidList->GetSize(); i++)
	{//先lidar后影像
		Align_LidLine lidItem=pLidList->GetAt(i);
		IDVec.push_back(lidItem.LineID);
	}

	for(i=0; i<pImgList->GetSize(); i++)
	{
		Align_Image imgItem=pImgList->GetAt(i);
		IDVec.push_back(imgItem.nImgID);
	}
	OutputMatchLinks(&mtLinkMap, &vStripFiles, IDVec, minOverlap, strTrackName.GetBuffer(0));
	tieNum = ptLinks.size();

	//////////////////////////////////////////////////////////////////////////
	// 生成各影像的连接点文件
	for( i=0; i<vStripFiles.size(); i++ )
	{
		orsString outputFile = vStripFiles[i] + _T(".tiePts.txt");

		FILE *fpOut;

		fpOut = fopen( outputFile, 	"wt");

		if(i < lidNum)
		{
			imgReader1 = imageService->openImageFile(vStripFiles[i]);
			pImgGeo1 = imgReader1->GetImageGeometry();
		}

		if( NULL != fpOut ) {
			mtLinkMapITER iter, next;	
			int k=0;
			for( iter = ptLinks.begin(); iter != ptLinks.end(); iter++ )
			{
				if( !mtLinkMap.isVaild(iter) )
					continue;

				long tieNum = iter->second.count;
				if( tieNum < 100 && tieNum >= minOverlap ) 
				{
					k++;
					long iImg = mtLinkMap.getImgNum( iter->first );						
					long uniqueId =	mtLinkMap.getRootId( iter->first );

					if( i == iImg )	{
						
						double xi, yi;
						xi = iter->second.x;
						yi = iter->second.y;

						if(i<lidNum)
						{
							pImgGeo1->Project(iter->second.x, iter->second.y, 
								iter->second.z, &xi, &yi);
						}
						fprintf( fpOut, "%ld %.6f %.6f %.2f %f\n", uniqueId, xi, yi, 
							/*iter->second.scale*/0, /*iter->second.orient*/0 );										
					}

					////////////////////////////////////////////////////
					
					next = iter;
					for( j=1; j<tieNum; j++)
					{
						next = ptLinks.find( next->second.next );

						iImg = mtLinkMap.getImgNum( next->first );
						if( i == iImg )	{

							double xi, yi;
							xi = next->second.x;
							yi = next->second.y;

							if(i<lidNum)
							{
								pImgGeo1->Project(next->second.x, next->second.y, 
									next->second.z, &xi, &yi);
							}
							fprintf( fpOut, "%ld %.6f %.6f %.2f %f\n", uniqueId, xi, yi, 
								/*iter->second.scale*/0, /*iter->second.orient*/0 );										
						}
					}
				}
			}

			fclose( fpOut );
		}
	}

//	if(ptPairVec)	delete ptPairVec; ptPairVec=NULL;
}

void CMainFrame::OnLidarDetectSift()
{
	orsIPlatform*  pPlatform = getPlatform();
	ImageMatchSetPlatform(pPlatform);
	orsArray<orsString> vStripFiles;

	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
//	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i;
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine line=pLidList->GetAt(i);

		if(line.GridName.GetLength() != 0)
			vStripFiles.push_back(orsString(line.GridName.GetBuffer(0)));
	}

// 	for(i=0; i<pImgList->GetSize(); i++)
// 	{
// 		Align_Image line=pImgList->GetAt(i);
// 
// 		if(line.ImgName.GetLength() != 0)
// 			vStripFiles.push_back(orsString(line.ImgName.GetBuffer(0)));
// 	}

	SIFTDetector(vStripFiles, m_SIFTDetectZoom);
}

//注意：挑点时不能用像素坐标来做，应该转换成空间坐标进行挑点
//直接用像素坐标，利用仿射变换挑点时对于平行条带有效，对于垂直条带失败。 2016.4.26
#include "orsGeometry/orsPointPairSet.h"
void CMainFrame::OnLidarmatchAnn()
{
	//导出连接点文件
	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("match init (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="初始匹配文件";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strTrackName=outputdlg.GetPathName();

	/////////////////////////////////////////////////////////////////
	int minOverlap = 2; //最小的连接点数

//	int zoom = 8;
	char ovLevel[4];
	_itoa(m_SIFTDetectZoom, ovLevel, 10);

	ors_int32 blkSizeX = 512, blkSizeY = 512;		//提点时的分块大小
	ors_int32 gridNumX = 0, gridNumY = 0;
	ors_int32 gridSizeX = 1024, gridSizeY = 1024;	//匹配时的分块大小


	//	myPtPair *ptPairVec=NULL;
	int maxLen=0;
	int matchNum, refineNum;
	orsMatchLinks	mtLinkMap;
//	CATNPtIndex	index1(getPlatform()), index2(getPlatform());

	ref_ptr<orsIImageService>  imageService = 
		ORS_PTR_CAST(orsIImageService, getPlatform()->getService(ORS_SERVICE_IMAGE) );
// 
	ref_ptr<orsIImageSourceReader> imgReader0, imgReader1;
	orsIImageGeometry *pImgGeo0=NULL, *pImgGeo1=NULL;

	ref_ptr<orsIAlg2DPointPairsCheck> pairsCheck;
	//pairsCheck = ORS_CREATE_OBJECT( orsIAlg2DPointPairsCheck, ORS_ALG_2DPOINTPAIR_CHECK_DEFAULT );
	pairsCheck = ORS_CREATE_OBJECT( orsIAlg2DPointPairsCheck, ORS_ALG_2DPOINTPAIR_CHECK_RANSAC );

	ref_ptr<orsI2DFeatureSet> featureSet0_ov = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
	ref_ptr<orsI2DFeatureSet> featureSet1_ov = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

	ref_ptr<orsI2DFeatureSet> featureSet0 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );
	ref_ptr<orsI2DFeatureSet> featureSet1 = ORS_CREATE_OBJECT( orsI2DFeatureSet, ORS_2DFEATURESET_SIFT );

 	ref_ptr<orsIAlg2DFeatureRegistering> keyMatch;
 	keyMatch = ORS_CREATE_OBJECT( orsIAlg2DFeatureRegistering, ORS_ALG_2DFEATURE_REGISTER_DEFAULT );
	keyMatch->setThresholds(0.75, 0.6);

	ref_ptr<orsIAlg2DFeatureRegistering> blkKeyMatch;
	blkKeyMatch = ORS_CREATE_OBJECT( orsIAlg2DFeatureRegistering, ORS_ALG_2DFEATURE_REGISTER_DEFAULT );
	//blkKeyMatch->setThresholds(0.7, 0.75);


	CArray<Align_LidLine, Align_LidLine>  *pLidList=m_AlgPrj.GetLidList();
	CArray<Align_Image, Align_Image> *pImgList=m_AlgPrj.GetImgList();
	int i, j, k;
	int lidNum = pLidList->GetSize();

	orsAdjPtPAIR *ptPairVec;
	//std::vector<orsAdjPtPAIR> ptPairVec;

	orsArray<orsString> vStripFiles;
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine LidItem=pLidList->GetAt(i);
		vStripFiles.push_back(orsString(LidItem.GridName.GetBuffer(0)));
	}
// 	for(i=0; i<pImgList->GetSize(); i++)
// 	{
// 		Align_Image imgItem=pImgList->GetAt(i);
// 		vStripFiles.push_back(orsString(imgItem.ImgName.GetBuffer(0)));
// 	}

	ImageMatchSetPlatform(getPlatform());

	//对Lidar进行两两匹配
	for(i=0; i<pLidList->GetSize()-1; i++)
	{
		Align_LidLine LidItem1=pLidList->GetAt(i);

 		imgReader0 = imageService->openImageFile(LidItem1.GridName.GetBuffer(0));
 		pImgGeo0 = imgReader0->GetImageGeometry();
//		index1.Open(LidItem1.LasName);

		orsString siftName0 = LidItem1.GridName.GetBuffer(0);
		siftName0 += _T(".sift.bin");
		featureSet0->load( siftName0 );
		keyMatch->setDstFeatureSet( featureSet0.get() );

		int refWid, refHei;

		{
			ref_ptr<orsIImageSource> refImg = getImageService()->openImageFile( LidItem1.GridName );

			refWid = refImg->getWidth();
			refHei = refImg->getHeight();
		}

		//匹配缩略图
		if(m_SIFTDetectZoom > 1)
		{
			//orsString siftFileName_ov = vStripFiles[iStrip] + _T("_zoomOut16.sift.bin");
			orsString siftFileName_ov = LidItem1.GridName.GetBuffer(0);
			siftFileName_ov = siftFileName_ov + _T("_zoomOut") + ovLevel + _T(".sift.bin");
			// 读入特征点文件
			featureSet0_ov->load( siftFileName_ov );

			if( !keyMatch->setDstFeatureSet( featureSet0_ov.get() ) )
				return;
		}

		for(j=i+1; j<pLidList->GetSize(); j++)
		{
			Align_LidLine LidItem2=pLidList->GetAt(j);

			std::vector<orsAdjPtPAIR> ptPairVec;
			ptPairVec.clear();

 			imgReader1 = imageService->openImageFile(LidItem2.GridName.GetBuffer(0));
 			pImgGeo1 = imgReader1->GetImageGeometry();
//			index2.Open(LidItem2.LasName);

			orsString siftName1 = LidItem2.GridName.GetBuffer(0);
			siftName1 += _T(".sift.bin");
			featureSet1->load( siftName1 );

			int n;
			bool bOverviewMatch = false;	//缩略图匹配是否成功
			orsPtPairsAffinePARA pairAffine;
			orsPtPairsDLTPARA    pairDLT;
			if(m_SIFTDetectZoom > 1)
			{
				orsString siftFileName1_ov = LidItem2.GridName.GetBuffer(0);
				siftFileName1_ov = siftFileName1_ov + _T("_zoomOut") + ovLevel + _T(".sift.bin");

				// 读入特征点文件
				featureSet1_ov->load( siftFileName1_ov );

				orsAdjPtPAIR *ptPairVec_ov = keyMatch->findMatchedPairs( featureSet1_ov.get(), &n );

				for(int ii=0; ii<n; ii++)
				{//提取三维坐标
					double X, Y;
					pImgGeo0->IntersectWithZ(ptPairVec_ov[ii].x1, ptPairVec_ov[ii].y1, 0, 
						&X, &Y);
					ptPairVec_ov[ii].x1 = X; ptPairVec_ov[ii].y1 = Y;
					pImgGeo1->IntersectWithZ(ptPairVec_ov[ii].x0, ptPairVec_ov[ii].y0, 0, 
						&X, &Y);
					ptPairVec_ov[ii].x0 = X;
					ptPairVec_ov[ii].y0 = Y;
				}
				orsMatchedFtPAIR *pMtPairs = pairsCheck->check_2DTransform( ptPairVec_ov, &n, ORS_GEOMETRY_TRANSFORM2D_AFFINE/*ORS_GEOMETRY_TRANSFORM2D_DLT*/, 3 );
				orsPointPairSet ptPairSet;
				
				
				if( n > 7 )	
				{
					bOverviewMatch = true;
					ptPairSet.alloc( n );

					orsKeyPOINT keyPoint;

					int i;
					for( i=0; i<n; i++ )
					{
						featureSet0_ov->getFetureVector( pMtPairs[i].dstFtId, &keyPoint, NULL );

						ptPairSet.m_ptPairs[i].x0 = keyPoint.x;
						ptPairSet.m_ptPairs[i].y0 = keyPoint.y;

						featureSet1_ov->getFetureVector( pMtPairs[i].srcFtId, &keyPoint, NULL );

						ptPairSet.m_ptPairs[i].x1 = keyPoint.x;
						ptPairSet.m_ptPairs[i].y1 = keyPoint.y;
					}

					ptPairSet.getAffineTransorm( &pairAffine );
					ptPairSet.GetDLTTransorm(&pairDLT);

					printf("overView, a = %8.1lf %8.4lf %8.4lf\n", pairAffine.a[0], pairAffine.a[1], pairAffine.a[2] );
					printf("overView, b = %8.1lf %8.4lf %8.4lf\n", pairAffine.b[0], pairAffine.b[1], pairAffine.b[2] );

				}
				else
				{//缩略图匹配失败
					pairAffine.a[0]=0.0;	pairAffine.a[1]=1.0;	pairAffine.a[2]=0.0;
					pairAffine.b[0]=0.0;	pairAffine.b[1]=0.0;	pairAffine.b[2]=1.0;

					pairDLT.a[0]=1.0;	pairDLT.a[1]=0.0;	pairDLT.a[2]=0.0;
					pairDLT.a[3]=0.0;	pairDLT.a[4]=1.0;	pairDLT.a[5]=0.0;
					pairDLT.a[6]=0.0;	pairDLT.a[7]=0.0;	pairDLT.a[8]=1.0;
				}
			}
			else
			{//不做变换
				pairAffine.a[0]=0.0;	pairAffine.a[1]=1.0;	pairAffine.a[2]=0.0;
				pairAffine.b[0]=0.0;	pairAffine.b[1]=0.0;	pairAffine.b[2]=1.0;

				pairDLT.a[0]=1.0;	pairDLT.a[1]=0.0;	pairDLT.a[2]=0.0;
				pairDLT.a[3]=0.0;	pairDLT.a[4]=1.0;	pairDLT.a[5]=0.0;
				pairDLT.a[6]=0.0;	pairDLT.a[7]=0.0;	pairDLT.a[8]=1.0;
			}
			// 相对关系确定完毕

			/////// 分块匹配
			if( /*bGridSizeSet*/true )	
			{
				gridNumX = refWid / gridSizeX +1;
				gridNumY = refHei / gridSizeY +1;
			}
			else	
			{
				gridSizeX = refWid / gridNumX;
				gridSizeY = refHei / gridNumY;
			}


			int iRow, iCol;
			int iBlkX, iBlkY;

			//	std::vector<orsMatchedFtPAIR> initMatchPair;

			for( iBlkY=0; iBlkY<gridNumY; iBlkY++)
			{
				iRow = iBlkY*gridSizeY + blkSizeY/2;  /* + gridSizeY/2 - blkSizeY/2;*/

				assert( iRow >=0 );

				for( iBlkX=0; iBlkX<gridNumX; iBlkX++)
				{
					iCol = iBlkX*gridSizeX + blkSizeX/2;/* + gridSizeX/2 - blkSizeX/2;*/

					assert( iCol >= 0 );

					orsRect_i rect0( iCol, iRow, iCol+gridSizeX, iRow+gridSizeY );	

					if(iCol+gridSizeX > refWid-blkSizeX/2)
						rect0.m_xmax = refWid-blkSizeX/2;
					if(iRow+gridSizeY > refHei-blkSizeY/2)
						rect0.m_ymax = refHei-blkSizeY/2;

					// 主影像，dst
					if( !blkKeyMatch->setDstFeatureSet( featureSet0.get(), &rect0 ) )
						continue;

					//////////////////////////////////////////////////////////////////////////
					orsRect_i rect1;

					{
						int x[4], y[4];

						x[0] = iCol;	y[0] = iRow;
						x[1] = iCol+gridSizeX;	y[1] = iRow;
						x[2] = iCol+gridSizeX;	y[2] = iRow + gridSizeY;
						x[3] = iCol;	y[3] = iRow + gridSizeY;

						double xf, yf;
						int xfMin=999999, yfMin=999999, xfMax=-999999, yfMax=-999999;

 						double *a = pairAffine.a;
 						double *b = pairAffine.b;

						double *l = pairDLT.a;

						int i; 
						for( i=0; i<4; i++ )
						{
							if(bOverviewMatch)
							{
							//affine
 							xf = a[0] + a[1]*x[i] + a[2]*y[i];
 							yf = b[0] + b[1]*x[i] + b[2]*y[i];

							//DLT
// 							double w = l[6]*x[i]+l[7]*y[i]+l[8];
// 							xf = (l[0]*x[i] + l[1]*y[i] + l[2])/w;
// 							yf = (l[3]*x[i] + l[4]*y[i] + l[5])/w;
							}
							else
							{//用初始的几何模型
								double X, Y;
								pImgGeo0->IntersectWithZ(x[i], y[i], 0, 
									&X, &Y);

								pImgGeo1->Project(X, Y, 0, &xf, &yf);
							}

							if( xfMin > xf )	xfMin = xf;
							if( yfMin > yf )	yfMin = yf;

							if( xfMax < xf )	xfMax = xf;
							if( yfMax < yf )	yfMax = yf;
						}

						rect1.m_xmin = xfMin;	rect1.m_xmax = xfMax;
						rect1.m_ymin = yfMin;	rect1.m_ymax = yfMax;
					}								

					orsAdjPtPAIR *ptBlkPairVec = blkKeyMatch->findMatchedPairs( featureSet1.get(), &matchNum, &rect1 );

					for( k=0; k<matchNum; k++ )
					{
						ptPairVec.push_back(ptBlkPairVec[k]);
					}
				}
			}
//			KeyMatch_images(LidItem1.GridName.GetBuffer(0), LidItem2.GridName.GetBuffer(0),
//			 				&ptPairVec, matchNum, 8);

			//挑粗差
			// 			double Mout[9];
			// 			EstimateTransform_Affine(ptPairVec, matchNum, refineNum,3, Mout);

			refineNum = matchNum = ptPairVec.size();

			for(int ii=0; ii<matchNum; ii++)
			{//提取三维坐标
				double X, Y;
				pImgGeo0->IntersectWithZ(ptPairVec[ii].x1, ptPairVec[ii].y1, 0, 
					&X, &Y);
				ptPairVec[ii].x1 = X; ptPairVec[ii].y1 = Y;
				pImgGeo1->IntersectWithZ(ptPairVec[ii].x0, ptPairVec[ii].y0, 0, 
					&X, &Y);
				ptPairVec[ii].x0 = X;
				ptPairVec[ii].y0 = Y;
			}
			const orsMatchedFtPAIR *pPairs;
//			pPairs = pairsCheck->check_2DTransform(&ptPairVec[0], &refineNum, ORS_GEOMETRY_TRANSFORM2D_DLT, 3.0 );

			ref_ptr<orsIProperty> pProp = getPlatform()->createProperty();
			pPairs = pairsCheck->check_Transform(&ptPairVec[0], &refineNum,
				ORS_ALG_2DPOINTPAIR_CHECK_CONSTRAIN_TYPE_HMATRIX, pProp.get(), 0);


			if( refineNum > 3 )	
			{
				orsKeyPOINT dstPt, srcPt;
				for( k=0; k<refineNum; k++ )
				{//提取物方坐标
					// iStrip
					featureSet0->getFetureVector(pPairs[k].dstFtId, &dstPt);

					// iStrip1
					featureSet1->getFetureVector(pPairs[k].srcFtId, &srcPt);

					//if(ptPairVec[k].bAccepted)
					mtLinkMap.AddPtPair( j, pPairs[k].srcFtId, &srcPt, i, pPairs[k].dstFtId, &dstPt );

				}
			}
		}
	}

	int count = mtLinkMap.RemoveDuplicatePoints();
	printf("%d duplicate points are removed\n", count );

	mtLinkMap.CountOverlap( vStripFiles.size() );

	mtLinkMAP &ptLinks = mtLinkMap.GetMatchLinks();

	int tieNum = ptLinks.size();
	std::vector<int> lidIDVec;
	for(i=0; i<pLidList->GetSize(); i++)
	{
		Align_LidLine LidItem=pLidList->GetAt(i);
		lidIDVec.push_back(LidItem.LineID);
	}
	OutputMatchLinks(&mtLinkMap, &vStripFiles, lidIDVec, minOverlap, strTrackName.GetBuffer(0));
	tieNum = ptLinks.size();

	//////////////////////////////////////////////////////////////////////////
	// 生成各影像的连接点文件
	for( i=0; i<vStripFiles.size(); i++ )
	{
		orsString outputFile = vStripFiles[i] + _T(".tiePts.txt");

		FILE *fpOut;

		fpOut = fopen( outputFile, 	"wt");

		if(i < lidNum)
		{
			imgReader1 = imageService->openImageFile(vStripFiles[i]);
			pImgGeo1 = imgReader1->GetImageGeometry();
		}

		if( NULL != fpOut ) {
			mtLinkMapITER iter, next;	
			int k=0;
			for( iter = ptLinks.begin(); iter != ptLinks.end(); iter++ )
			{
				if( !mtLinkMap.isVaild(iter) )
					continue;

				long tieNum = iter->second.count;
				if( tieNum < 100 && tieNum >= minOverlap ) 
				{
					k++;
					long iImg = mtLinkMap.getImgNum( iter->first );						
					long uniqueId =	mtLinkMap.getRootId( iter->first );

					if( i == iImg )	{

						double xi, yi;
						xi = iter->second.x;
						yi = iter->second.y;

						if(i<lidNum)
						{
							pImgGeo1->Project(iter->second.x, iter->second.y, 
								iter->second.z, &xi, &yi);
						}
						fprintf( fpOut, "%ld %.6f %.6f %.2f %f\n", uniqueId, xi, yi, 
							/*iter->second.scale*/0, /*iter->second.orient*/0 );										
					}

					////////////////////////////////////////////////////

					next = iter;
					for( j=1; j<tieNum; j++)
					{
						next = ptLinks.find( next->second.next );

						iImg = mtLinkMap.getImgNum( next->first );
						if( i == iImg )	{

							double xi, yi;
							xi = next->second.x;
							yi = next->second.y;

							if(i<lidNum)
							{
								pImgGeo1->Project(next->second.x, next->second.y, 
									next->second.z, &xi, &yi);
							}
							fprintf( fpOut, "%ld %.6f %.6f %.2f %f\n", uniqueId, xi, yi, 
								/*iter->second.scale*/0, /*iter->second.orient*/0 );										
						}
					}
				}
			}

			fclose( fpOut );
		}
	}
}


void CMainFrame::OnImagebaClassic()
{
	CFileDialog  dlg(TRUE,"txt",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"连接点文件(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString  strTirPtFile=dlg.GetPathName();

// 	orsCamSysExtraParam_TYPE calibType = CamGeo_rph;
// 	orsCamSysExtraParam      calibParam;
// 
// 	calibParam.lever_arm[0]=calibParam.lever_arm[1]=calibParam.lever_arm[2]=0;
// 	calibParam.boresight_angle[0]=calibParam.boresight_angle[1]=calibParam.boresight_angle[2]=0;

	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("BA result (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="空三结果";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strRstName=outputdlg.GetPathName();

	CString strAtpDir;
	if(!_SelectFolderDialog("设置空三输出目录...", &strAtpDir, NULL))
		return;

	if(m_AlgPrj.GetImgList()->GetSize()>3)
	{
		CIntATManager  atManager(getPlatform());

		lensDistortion lesDist;
		memset(&lesDist, 0, sizeof(lensDistortion));
// 		lesDist.k0 = 8.560790e-003;
// 		lesDist.k1 = -2.050420e-005;
// 		lesDist.k2 = 5.819490e-009;

		atManager.setOutputDir(strAtpDir.GetBuffer(0));

		//RCD105
//		atManager.setCameraParam(-0.0275,-0.4783,59.839,7162,5389,0.0068,0.0068, lesDist, ATT_rmNONE);

		double moutAngle[3];
		moutAngle[0] = 180.0*RAD_PER_DEG;
		moutAngle[1] = 0;
		moutAngle[2] = 90.0*RAD_PER_DEG;
		//DMC
		atManager.setCameraParam(0,0,120,7680,13824,0.012,0.012, lesDist, moutAngle);

		atManager.setAlignPrj(&m_AlgPrj);
		//		atManager.setTiePoints(strTirPtFile.GetBuffer(0));

//		atManager.get_OrientedArchorsFromLIDAR(0.1);

		atManager.loadTiePoints(strTirPtFile.GetBuffer(0));

//		CString strRstFile="H:\\AT_experiment\\baotou20110930\\CombinedAT\\ImageBA\\classic\\BA_Result.txt";

		atManager.imageBA(/*strTirPtFile.GetBuffer(0), */strRstName.GetBuffer(0), ImageBA_classic_W);

	}
}

void CMainFrame::OnImagebaGps()
{
	CFileDialog  dlg(TRUE,"txt",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"连接点文件(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString  strTirPtFile=dlg.GetPathName();

	// 	orsCamSysExtraParam_TYPE calibType = CamGeo_rph;
	// 	orsCamSysExtraParam      calibParam;
	// 
	// 	calibParam.lever_arm[0]=calibParam.lever_arm[1]=calibParam.lever_arm[2]=0;
	// 	calibParam.boresight_angle[0]=calibParam.boresight_angle[1]=calibParam.boresight_angle[2]=0;

	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("BA result (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="空三结果";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strRstName=outputdlg.GetPathName();

	CString strAtpDir;
	if(!_SelectFolderDialog("设置空三输出目录...", &strAtpDir, NULL))
		return;

	if(m_AlgPrj.GetImgList()->GetSize()>3)
	{
		CIntATManager  atManager(getPlatform());

		lensDistortion lesDist;
		memset(&lesDist, 0, sizeof(lensDistortion));
// 		lesDist.k0 = 8.560790e-003;
// 		lesDist.k1 = -2.050420e-005;
// 		lesDist.k2 = 5.819490e-009;

		atManager.setOutputDir(strAtpDir.GetBuffer(0));

		double moutAngle[3];
		moutAngle[0] = 180.0*RAD_PER_DEG;
		moutAngle[1] = 0;
		moutAngle[2] = 90.0*RAD_PER_DEG;

		atManager.setCameraParam(0,0,59.839,7162,5389,0.0068,0.0068, lesDist, moutAngle);

		
		//		atManager.setTiePoints(strTirPtFile.GetBuffer(0));
		m_AlgPrj.loadImageTimeStamp("H:\\AT_experiment\\baotou20110930\\GPSAT\\imageTimeStamp.txt");

		atManager.setAlignPrj(&m_AlgPrj);

		atManager.get_OrientedArchorsFromLIDAR(0.1);

		atManager.loadTiePoints(strTirPtFile.GetBuffer(0));

//		CString strRstFile="H:\\AT_experiment\\baotou20110930\\GPSAT\\BA_Result.txt";

		atManager.imageBA(/*strTirPtFile.GetBuffer(0), */strRstName.GetBuffer(0), /*ImageBA_GPS_supported*/ImageBA_classic_W);

	}
}


void CMainFrame::OnPatbClassic()
{
	CFileDialog  dlg(TRUE,"con",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"con文件(*.con)|*.con|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString  strconFile=dlg.GetPathName();

	CFileDialog  imdlg(TRUE,"im",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"im文件(*.im)|*.im|(*.*)|*.*||",NULL);

	if(imdlg.DoModal()!=IDOK)
		return;

	CString  strimFile=imdlg.GetPathName();

	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("BA result (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="空三结果";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strRstName=outputdlg.GetPathName();

	CString strAtpDir;
	if(!_SelectFolderDialog("设置空三输出目录...", &strAtpDir, NULL))
		return;

// 	if(m_AlgPrj.GetImgList()->GetSize()>3)
// 	{
		CIntATManager  atManager(getPlatform());

		atManager.loadPATBconFile(strconFile.GetBuffer(0));
		atManager.loadGPSEOFile("H:\\AT_experiment\\PATBGPS_\\PATB\\GPSEO.txt");

//		atManager.loadoriFile("H:\\AT_experiment\\PATBGPS_\\PATB\\PATB.ori");

//		atManager.calImageAngleElement_byGPS();
		atManager.loadPATBimFile(strimFile.GetBuffer(0));
//		atManager.loadadjFile("H:\\AT_experiment\\PATBGPS_\\PATB\\PATB.adj");

		lensDistortion lesDist;
		memset(&lesDist, 0, sizeof(lensDistortion));
// 		// 		lesDist.k0 = 8.560790e-003;
// 		// 		lesDist.k1 = -2.050420e-005;
// 		// 		lesDist.k2 = 5.819490e-009;
// 
 		atManager.setOutputDir(strAtpDir.GetBuffer(0));
 		atManager.setCameraParam(-0.18,0,100.5,11310,17310,0.006,0.006, lesDist);
// 
// 		atManager.setAlignPrj(&m_AlgPrj);
// 		//		atManager.setTiePoints(strTirPtFile.GetBuffer(0));
// 
// 		//		atManager.get_OrientedArchorsFromLIDAR(0.1);
// 
// 		atManager.loadTiePoints(strTirPtFile.GetBuffer(0));
// 
// 		//		CString strRstFile="H:\\AT_experiment\\baotou20110930\\GPSAT\\BA_Result.txt";
// 

		
		atManager.imageBA(/*strTirPtFile.GetBuffer(0), */strRstName.GetBuffer(0), ImageBA_classic_W);


}


void CMainFrame::OnPATBGPSBA()
{
	CFileDialog  dlg(TRUE,"con",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"con文件(*.con)|*.con|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString  strconFile=dlg.GetPathName();

	CFileDialog  imdlg(TRUE,"im",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"im文件(*.im)|*.im|(*.*)|*.*||",NULL);

	if(imdlg.DoModal()!=IDOK)
		return;

	CString  strimFile=imdlg.GetPathName();

	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("BA result (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="空三结果";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strRstName=outputdlg.GetPathName();

	CString strAtpDir;
	if(!_SelectFolderDialog("设置空三输出目录...", &strAtpDir, NULL))
		return;

	// 	if(m_AlgPrj.GetImgList()->GetSize()>3)
	// 	{
	CIntATManager  atManager(getPlatform());

	atManager.loadPATBconFile(strconFile.GetBuffer(0));
	atManager.loadGPSEOFile("H:\\AT_experiment\\PATBGPS_\\PATB\\GPSEO.txt");

	//		atManager.loadoriFile("H:\\AT_experiment\\PATBGPS_\\PATB\\PATB.ori");

	//		atManager.calImageAngleElement_byGPS();
	atManager.loadPATBimFile(strimFile.GetBuffer(0));
	//		atManager.loadadjFile("H:\\AT_experiment\\PATBGPS_\\PATB\\PATB.adj");

	lensDistortion lesDist;
	memset(&lesDist, 0, sizeof(lensDistortion));
	// 		// 		lesDist.k0 = 8.560790e-003;
	// 		// 		lesDist.k1 = -2.050420e-005;
	// 		// 		lesDist.k2 = 5.819490e-009;
	// 
	atManager.setOutputDir(strAtpDir.GetBuffer(0));
	atManager.setCameraParam(-0.18,0,100.5,11310,17310,0.006,0.006, lesDist);
	// 
	// 		atManager.setAlignPrj(&m_AlgPrj);
	// 		//		atManager.setTiePoints(strTirPtFile.GetBuffer(0));
	// 
	// 		//		atManager.get_OrientedArchorsFromLIDAR(0.1);
	// 
	// 		atManager.loadTiePoints(strTirPtFile.GetBuffer(0));
	// 
	// 		//		CString strRstFile="H:\\AT_experiment\\baotou20110930\\GPSAT\\BA_Result.txt";
	// 


	atManager.imageBA(/*strTirPtFile.GetBuffer(0), */strRstName.GetBuffer(0), ImageBA_GPS_supported);
}


void CMainFrame::OnVisualsfmdataClassicba()
{
	CFileDialog  dlg(TRUE,"txt",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"连接点文件(*.nvm)|*.nvm|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString  strTirPtFile=dlg.GetPathName();

	// 	orsCamSysExtraParam_TYPE calibType = CamGeo_rph;
	// 	orsCamSysExtraParam      calibParam;
	// 
	// 	calibParam.lever_arm[0]=calibParam.lever_arm[1]=calibParam.lever_arm[2]=0;
	// 	calibParam.boresight_angle[0]=calibParam.boresight_angle[1]=calibParam.boresight_angle[2]=0;

	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("BA result (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="空三结果";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strRstName=outputdlg.GetPathName();

	CString strAtpDir;
	if(!_SelectFolderDialog("设置空三输出目录...", &strAtpDir, NULL))
		return;

	if(m_AlgPrj.GetImgList()->GetSize()>3)
	{
		CIntATManager  atManager(getPlatform());

		lensDistortion lesDist;
		memset(&lesDist, 0, sizeof(lensDistortion));
		// 		lesDist.k0 = 8.560790e-003;
		// 		lesDist.k1 = -2.050420e-005;
		// 		lesDist.k2 = 5.819490e-009;

		atManager.setOutputDir(strAtpDir.GetBuffer(0));
		double moutAngle[3];
		moutAngle[0] = 180.0*RAD_PER_DEG;
		moutAngle[1] = 0;
		moutAngle[2] = 90.0*RAD_PER_DEG;
		atManager.setCameraParam(0,0,59.839,7162,5389,0.0068,0.0068, lesDist, moutAngle);


		//		atManager.setTiePoints(strTirPtFile.GetBuffer(0));
		m_AlgPrj.loadImageTimeStamp("H:\\AT_experiment\\baotou20110930\\GPSAT\\imageTimeStamp.txt");

		atManager.setAlignPrj(&m_AlgPrj);

		atManager.get_OrientedArchorsFromLIDAR(0.1);
		atManager.output_OrientedArchors("H:\\AT_experiment\\baotou20110930\\GPSAT\\nvm\\archors.txt");

		atManager.loadnvmFile(strTirPtFile.GetBuffer(0));

		//		CString strRstFile="H:\\AT_experiment\\baotou20110930\\GPSAT\\BA_Result.txt";

		atManager.imageBA(/*strTirPtFile.GetBuffer(0), */strRstName.GetBuffer(0), /*ImageBA_GPS_supported*/ImageBA_classic_W);

	}
}

int LidMC_VP_ascending_tieID(const void *p1, const void  *p2)
{
	LidMC_VP *elem1, *elem2;
	elem1=(LidMC_VP *)p1;
	elem2=(LidMC_VP *)p2;

	if(elem1->tieID < elem2->tieID)
		return -1;
	else 
		if(elem1->tieID > elem2->tieID)
			return 1;
		else
			return 0;
}


#include "ATNPointIndex.h"
#include "LidCalib/VP_Model.h"
void CMainFrame::OnLidcalibVcm()
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
//	CATNPtIndex	index(getPlatform());

	CKDTPtIndex index(getPlatform());

	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidMC_VP *pLidVP=NULL;
	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch, nVP, iVP;
	//	double *pZObs=NULL;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	int nLine, nTies;
	POINT3D pt3D, *ptBuf;
	int ngcp;
	//	ATNTP_Point *pATNPoints=NULL;


	pTieList=m_AlgPrj.GetTieObjList();
	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	//	nTies=20;	//测试用

	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为1
		pTieLut[i]=-1;
	}

	LidPt_SurvInfo atnpts;
	std::vector<LidPt_SurvInfo> ATN_pts;
	ATN_pts.assign(3, atnpts);
	std::vector<LidPt_SurvInfo> ATN_Vec;
	LidPt_SurvInfo  tempSurInfo;
	std::vector<LidPt_SurvInfo> surv_Vec;
	nPatch=0;
	nVP=0;
	ngcp=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pTieChain=pTieList->GetAt(i);  //遍历每一个点链

		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());

		if(pTieChain->type==TO_PATCH)
		{
			nPatch+=pTieChain->pChain->GetSize();	
			//			continue;
		}

		if(pTieChain->type==TO_POINT)
			nVP+=pTieChain->pChain->GetSize();
		

		if(pTieChain->bGCP) 
			ngcp+=pTieChain->pChain->GetSize();


		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	if(nVP>0)
		pLidVP=new LidMC_VP[nVP];

	if(nPatch>0)
		pLidPlanes=new LidMC_Plane[nPatch];
	//	pATNPoints=new ATNTP_Point[nPatch*3];

	//	pZObs=new double[nPatch];
	int ptNum;
	locateresult   result;
	int tieID=0;
	iPatch=0; iVP=0;
	double radius=2.0;
	double rmse_th=0.2;
	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点


			k=pTieLut[j*nLine+i];
			if(k<0)  //判断当前连接点是否存在于指定条带上
				continue;

			pTieChain=pTieList->GetAt(j);

			pObj=pTieChain->pChain->GetAt(k);

			if(pTieChain->type==TO_PATCH)
			{
				ptNum=((TiePatch*)pObj)->ptNum;
				ptBuf=((TiePatch*)pObj)->pt3D;

				pLidPlanes[iPatch].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidPlanes[iPatch].tieID=pTieChain->TieID-1;
				tieID++;

				index.GetPoints(ptBuf, ptNum, pLidPlanes[iPatch].pLidPoints);

				iPatch++;
			}
			else if(pTieChain->type==TO_POINT)
			{
				char tptype =  ((TiePoint*)pObj)->tpType;

				pt3D=((TiePoint*)pObj)->pt3D;

				pLidVP[iVP].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidVP[iVP].tieID=pTieChain->TieID-1;
//				pLidVP[iVP].eX=pChain->gcp.X;
//				pLidVP[iVP].eY=pChain->gcp.Y;
//				pLidVP[iVP].eZ=pChain->gcp.Z;
				tieID++;

				double rmse;
				result=index.GetTriangleVertex(&pt3D, radius, &ATN_pts/*pLidVP[iVP].pLidPoints*/, &rmse); 
				//				ASSERT(result!=OUTSIDE);
				
				if(result==OUTSIDE)
				{//扫描漏洞，剔除
// 					pLidVP[iVP].VP_type=TP_type_horizon;
// 					pLidVP[iVP].vx=pt3D.X;
// 					pLidVP[iVP].vy=pt3D.Y;
// 					pLidVP[iVP].reliability=0;
// 
// 					iVP++;
					continue;
				}

				pLidVP[iVP].pLidPoints->assign(3, tempSurInfo);
//				ATN2LidPt_SurvInfo(&ATN_pts[0], &(*(pLidVP[iVP].pLidPoints))[0], 3);

				ATN_pts[0] = (*(pLidVP[iVP].pLidPoints))[0];
				ATN_pts[1] = (*(pLidVP[iVP].pLidPoints))[1];
				ATN_pts[2] = (*(pLidVP[iVP].pLidPoints))[2];

				ATN_Vec.insert(ATN_Vec.end(), ATN_pts.begin(), ATN_pts.end());
				surv_Vec.insert(surv_Vec.end(), pLidVP[iVP].pLidPoints->begin(), pLidVP[iVP].pLidPoints->end());

				pLidVP[iVP].vx=pt3D.X;
				pLidVP[iVP].vy=pt3D.Y;
				pLidVP[iVP].vz=TriangleInterpolate(&ATN_pts, pt3D.X, pt3D.Y);

				if(!tptype)
				{//未定义连接点类型，通过粗糙度计算
					if(rmse<rmse_th)
						pLidVP[iVP].VP_type=TP_type_hor_ver;
					else
						pLidVP[iVP].VP_type=TP_type_horizon;
				}
				else
				{//直接使用预先定义的连接点类型
					pLidVP[iVP].VP_type=tptype;
				}
				iVP++;
			}
		}

	}

	if(nVP>iVP)	nVP=iVP;

	//对pLidVP按连接点ID排序
	qsort(pLidVP, nVP, sizeof(LidMC_VP), LidMC_VP_ascending_tieID);

	//对连接点进行检查
	int TieHorNum=0, /*TieHorVerNum=0, */TieVerNum=0;
	int tiePtNum=0;
	for(i=0; i<nVP; )
	{
		int sPos, ePos;
		tieID=pLidVP[i].tieID;
		char vp_type=pLidVP[i].VP_type;
		bool bUpdate=false;

		tiePtNum++;
		sPos=i;
		ePos=i;

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(pLidVP[i].VP_type!=vp_type)
			{
				bUpdate=true;
			}
			ePos=i;

			i++;
		}

		if(bUpdate)
		{
			vp_type=TP_type_horizon;
			for(j=sPos; j<=ePos; j++)
			{
				pLidVP[j].VP_type=vp_type;
			}
		}

		if(vp_type == TP_type_hor_ver)
		{
			TieHorNum++;
			TieVerNum++;
		}
		else if(vp_type == TP_type_horizon)	
			TieHorNum++;
		else if(vp_type == TP_type_vertical)	
			TieVerNum++;

	}

	TRACE("total tie point num: %d\n", tiePtNum);
//	TRACE("horizon and vertical tie point num: %d\n", TieHorVerNum);
	TRACE("horizon tie point num: %d\n", TieHorNum);
	TRACE("vertical tie point num: %d\n", TieVerNum);

	//为控制点赋值
	for(i=0; i<ngcp; i++)
	{
		j=pLidVP[i].tieID;

		//		if(i<ngcp)
		//		{//控制点必须排在最开始
		pTieChain=pTieList->GetAt(j);
		ASSERT(pTieChain->bGCP);

		pLidVP[i].eX=pTieChain->gcp.X;
		pLidVP[i].eY=pTieChain->gcp.Y;
		pLidVP[i].eZ=pTieChain->gcp.Z;
		//		}
	}

	//调整连接点ID,连续排列 (漏洞点删除后占用的ID)
	for(i=0, j=0; i<nVP; )
	{
		tieID=pLidVP[i].tieID;
		if(tieID!=j)
		{	
			pLidVP[i].tieID=j;	
		}

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(tieID!=j)
			{	
				pLidVP[i].tieID=j;	
			}
			i++;
		}
		j++;
	}

	orsLidSysParam oriParam;
	memset(&oriParam, 0, sizeof(orsLidSysParam));

	double pX[8];	//线元素，角元素
	memset(pX, 0, sizeof(double)*8);

	//	pX[3]=/*-0.00576609*/0;	pX[4]=/*0.00258278*/0;	pX[5]=/*-0.00138682*/0;

	CFileDialog  outputdlg(FALSE," ",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	
	outputdlg.m_ofn.lpstrTitle="保存平差解算参数";

	if(outputdlg.DoModal()!=IDOK)
		goto FUNC_END;

	{
		CString  strOutput=outputdlg.GetPathName();

		CalibParam_Type ctype;
	//	ctype=Calib_rph_sa0;
	//	ctype=Calib_rph;
	//	ctype=Calib_rph_drange;
	//	ctype=Calib_rph_drange_sa0;
		ctype=Calib_rph;


		pX[0] = 0;
		pX[1] = 0;
		pX[2] = 0;
		pX[3] = 0;
		pX[4] = 0;
		pX[5] = 0;

//		_calLaserPosition(&surv_Vec[0],nVP*3, &oriParam, pX, ctype);
//		_calLaserPosition(&ATN_Vec[0],nVP*3, oriParam, pX, Calib_rph);
		//	ctype=Calib_rphxyz_sa0;
		//	ctype=Calib_rphxyz_sa0_sac;

		//有线性系统改正
		//	LidMC_Misalign_VP_Triangle_xyzrph_semiXYZ(nPatch, ngcp, pLidPatch, pX, 6);
		//无系统改正
		CLidCalib_VCP calib;
		calib.LidMC_Misalign_VCP(strOutput.GetBuffer(128), nVP, ngcp, pLidVP, LidAdj_TP_UseHor,
			pX, oriParam, ctype);
	}

FUNC_END:
	if(pLidVP)	delete[] pLidVP;	pLidVP=NULL;
	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pTieLut)  delete pTieLut;  pTieLut=NULL;
	//	if(pATNPoints)	delete pATNPoints;	pATNPoints=NULL;
}

//按时间顺序排列
int LidPt_SurvInfo_ascending_time(const void *p1, const void  *p2)
{
	LidPt_SurvInfo *elem1, *elem2;
	elem1=(LidPt_SurvInfo *)p1;
	elem2=(LidPt_SurvInfo *)p2;

	if(elem1->POS_Info.time < elem2->POS_Info.time)
		return -1;
	else 
		if(elem1->POS_Info.time > elem2->POS_Info.time)
			return 1;
		else
			return 0;
}

void decentering(LidMC_VP *pLidVP, int Num, double center[3])
{
	int i, j;
	int total_pt_num = 0;
	std::vector<LidPt_SurvInfo> *pLidPoints=NULL;

	center[0] = center[1] = center[2] = 0;
	for(i=0; i<Num; i++)
	{
		pLidPoints = pLidVP[i].pLidPoints;
		for(j=0; j<pLidPoints->size(); j++)
		{
			center[0] += (*pLidPoints)[j].x;
			center[1] += (*pLidPoints)[j].y;
			center[2] += (*pLidPoints)[j].z;
		}
		total_pt_num += pLidPoints->size();
	}

	center[0] /= total_pt_num;
	center[1] /= total_pt_num;
	center[2] /= total_pt_num;

	for(i=0; i<Num; i++)
	{
		pLidPoints = pLidVP[i].pLidPoints;
		for(j=0; j<pLidPoints->size(); j++)
		{
			(*pLidPoints)[j].x -= center[0];
			(*pLidPoints)[j].y -= center[1];
			(*pLidPoints)[j].z -= center[2];
			(*pLidPoints)[j].POS_Info.coord.X -= center[0];
			(*pLidPoints)[j].POS_Info.coord.Y -= center[1];
			(*pLidPoints)[j].POS_Info.coord.Z -= center[2];
		}
		pLidVP[i].vx -= center[0];
		pLidVP[i].vy -= center[1];
		pLidVP[i].vz -= center[2];

		pLidVP[i].eX -= center[0];
		pLidVP[i].eY -= center[1];
		pLidVP[i].eZ -= center[2];
	}
}

//用时间内插法来构造观测值
void CMainFrame::OnLidcalibTimeinterpolation()
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
//	CATNPtIndex	index(getPlatform());
	CKDTPtIndex index(getPlatform());
	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidMC_VP *pLidVP=NULL;
	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch, nVP, iVP;
	//	double *pZObs=NULL;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	int nLine, nTies;
	POINT3D pt3D, *ptBuf;
	int ngcp;
	//	ATNTP_Point *pATNPoints=NULL;

	pTieList=m_AlgPrj.GetTieObjList();
	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	//	nTies=20;	//测试用

	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为1
		pTieLut[i]=-1;
	}

	std::vector<LidPt_SurvInfo> ATN_pts;
	LidPt_SurvInfo  tempSurInfo;
	nPatch=0;
	nVP=0;
	ngcp=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计连接点数
		pTieChain=pTieList->GetAt(i);  //遍历每一个点链

		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());

		if(pTieChain->type==TO_PATCH)
		{
			nPatch+=pTieChain->pChain->GetSize();	
			//			continue;
		}

		if(pTieChain->type==TO_POINT)
			nVP+=pTieChain->pChain->GetSize();


		if(pTieChain->bGCP) 
			ngcp+=pTieChain->pChain->GetSize();


		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	if(nVP>0)
		pLidVP=new LidMC_VP[nVP];

	if(nPatch>0)
		pLidPlanes=new LidMC_Plane[nPatch];
	//	pATNPoints=new ATNTP_Point[nPatch*3];

	//	pZObs=new double[nPatch];
	ref_ptr<orsITrajectory> traj;
	traj = ORS_CREATE_OBJECT(orsITrajectory, ORS_LIDARGEOMETRY_TRAJECTORY_DEFAULT);

	orsIMatrixService *matrixService;
	matrixService = ( orsIMatrixService *)getPlatform()->getService( ORS_SERVICE_MATRIX );

	ref_ptr<orsILidarGeoModel> lidGeo;
	lidGeo = ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_RIGOROUS);

	int ptNum;
	locateresult   result;
	int tieID=0;
	iPatch=0; iVP=0;
	double radius=2.0;
	double rmse_th=0.2;
	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点


			k=pTieLut[j*nLine+i];
			if(k<0)  //判断当前连接点是否存在于指定条带上
				continue;

			pTieChain=pTieList->GetAt(j);

			pObj=pTieChain->pChain->GetAt(k);

			if(pTieChain->type==TO_PATCH)
			{
				ptNum=((TiePatch*)pObj)->ptNum;
				ptBuf=((TiePatch*)pObj)->pt3D;

				pLidPlanes[iPatch].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidPlanes[iPatch].tieID=pTieChain->TieID-1;
				tieID++;

				index.GetPoints(ptBuf, ptNum, pLidPlanes[iPatch].pLidPoints);

				iPatch++;
			}
			else if(pTieChain->type==TO_POINT)
			{
				char tptype =  ((TiePoint*)pObj)->tpType;

				pt3D=((TiePoint*)pObj)->pt3D;

				pLidVP[iVP].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidVP[iVP].tieID=pTieChain->TieID-1;
				//				pLidVP[iVP].eX=pChain->gcp.X;
				//				pLidVP[iVP].eY=pChain->gcp.Y;
				//				pLidVP[iVP].eZ=pChain->gcp.Z;
				tieID++;

				double rmse;
				result=index.GetTriangleVertex(&pt3D, radius, &ATN_pts/*pLidVP[iVP].pLidPoints*/, &rmse); 
				//				ASSERT(result!=OUTSIDE);

				if(result==OUTSIDE)
				{//扫描漏洞，剔除
		
					continue;
				}

				pLidVP[iVP].pLidPoints->assign(3, tempSurInfo);
//				ATN2LidPt_SurvInfo(&ATN_pts[0], &(*(pLidVP[iVP].pLidPoints))[0], 3);
				ATN_pts[0] = (*(pLidVP[iVP].pLidPoints))[0];
				ATN_pts[1] = (*(pLidVP[iVP].pLidPoints))[1];
				ATN_pts[2] = (*(pLidVP[iVP].pLidPoints))[2];

				
				pLidVP[iVP].vx=pt3D.X;
				pLidVP[iVP].vy=pt3D.Y;
				pLidVP[iVP].vz=TriangleInterpolate(&ATN_pts, pt3D.X, pt3D.Y);


				/////////////////////////////////////////////////////
				//用时间内插出观测值
				orsPOSParam pos_t, pos0, pos1;

				pos_t.time = TriangleInterpolate_time(&ATN_pts, pt3D.X, pt3D.Y);
				qsort(&(*(pLidVP[iVP].pLidPoints))[0], 3, sizeof(LidPt_SurvInfo), LidPt_SurvInfo_ascending_time);

				assert(pos_t.time>(*(pLidVP[iVP].pLidPoints))[0].POS_Info.time
					&& pos_t.time<(*(pLidVP[iVP].pLidPoints))[2].POS_Info.time);

				
				if(pos_t.time < (*(pLidVP[iVP].pLidPoints))[1].POS_Info.time)
				{
					pos0 = (*(pLidVP[iVP].pLidPoints))[0].POS_Info;
					pos1 = (*(pLidVP[iVP].pLidPoints))[1].POS_Info;
				}
				else
				{
					pos0 = (*(pLidVP[iVP].pLidPoints))[1].POS_Info;
					pos1 = (*(pLidVP[iVP].pLidPoints))[2].POS_Info;
				}
				
				matrixService->RotateMatrix_rph(pos0.r, pos0.p, pos0.h, pos0.R);
				matrixService->RotateMatrix_rph(pos1.r, pos1.p, pos1.h, pos1.R);
				traj->Interpolate_linear(&pos_t, &pos0, &pos1);

				matrixService->R2rph(pos_t.R, &pos_t.r, &pos_t.p, &pos_t.h);

				(*(pLidVP[iVP].pLidPoints))[0].x=pLidVP[iVP].vx;
				(*(pLidVP[iVP].pLidPoints))[0].y=pLidVP[iVP].vy;
				(*(pLidVP[iVP].pLidPoints))[0].z=pLidVP[iVP].vz;
				(*(pLidVP[iVP].pLidPoints))[0].POS_Info = pos_t;
// 				(*(pLidVP[iVP].pLidPoints))[0].vecRange[0] = 0;
// 				(*(pLidVP[iVP].pLidPoints))[1].vecRange[1] = 0;
// 				(*(pLidVP[iVP].pLidPoints))[2].vecRange[2] = 0;
				lidGeo->cal_RangeVec(&(*(pLidVP[iVP].pLidPoints))[0], 1);
				//内插点完成
				///////////////////////////////////////////////////

				if(!tptype)
				{//未定义连接点类型，通过粗糙度计算
					if(rmse<rmse_th)
						pLidVP[iVP].VP_type=TP_type_hor_ver;
					else
						pLidVP[iVP].VP_type=TP_type_horizon;
				}
				else
				{//直接使用预先定义的连接点类型
					pLidVP[iVP].VP_type=tptype;
				}
				iVP++;
			}
		}
	}

	if(nVP>iVP)	nVP=iVP;

	//对pLidVP按连接点ID排序
	qsort(pLidVP, nVP, sizeof(LidMC_VP), LidMC_VP_ascending_tieID);

	//去中心（平差前的一个重要步骤）
	double center[3];
	decentering(pLidVP, nVP, center);

	//对连接点进行检查
	int TieHorNum=0, TieHorVerNum=0, TieVerNum=0;
	int tiePtNum=0;
	for(i=0; i<nVP; )
	{
		int sPos, ePos;
		tieID=pLidVP[i].tieID;
		char vp_type=pLidVP[i].VP_type;
		bool bUpdate=false;

		tiePtNum++;
		sPos=i;
		ePos=i;

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(pLidVP[i].VP_type!=vp_type)
			{
				bUpdate=true;
			}
			ePos=i;

			i++;
		}

		if(bUpdate)
		{
			vp_type=TP_type_horizon;
			for(j=sPos; j<=ePos; j++)
			{
				pLidVP[j].VP_type=vp_type;
			}
		}

		if(vp_type & TP_type_hor_ver)	TieHorVerNum++;
		else if(vp_type & TP_type_horizon)	TieHorNum++;
		else if(vp_type & TP_type_vertical)	TieVerNum++;

	}

	TRACE("total tie point num: %d\n", tiePtNum);
	TRACE("horizon and vertical tie point num: %d\n", TieHorVerNum);
	TRACE("horizon tie point num: %d\n", TieHorNum);
	TRACE("vertical tie point num: %d\n", TieVerNum);

	//为控制点赋值
	for(i=0; i<ngcp; i++)
	{
		j=pLidVP[i].tieID;

		//		if(i<ngcp)
		//		{//控制点必须排在最开始
		pTieChain=pTieList->GetAt(j);
		ASSERT(pTieChain->bGCP);

		pLidVP[i].eX=pTieChain->gcp.X;
		pLidVP[i].eY=pTieChain->gcp.Y;
		pLidVP[i].eZ=pTieChain->gcp.Z;
		//		}
	}

	//调整连接点ID,连续排列 (漏洞点删除后占用的ID)
	for(i=0, j=0; i<nVP; )
	{
		tieID=pLidVP[i].tieID;
		if(tieID!=j)
		{	
			pLidVP[i].tieID=j;	
		}

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(tieID!=j)
			{	
				pLidVP[i].tieID=j;	
			}
			i++;
		}
		j++;
	}

	orsLidSysParam oriParam;
	memset(&oriParam, 0, sizeof(orsLidSysParam));

	double pX[8];	//线元素，角元素
	memset(pX, 0, sizeof(double)*8);

	//	pX[3]=/*-0.00576609*/0;	pX[4]=/*0.00258278*/0;	pX[5]=/*-0.00138682*/0;

	CFileDialog  outputdlg(FALSE," ",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	
	outputdlg.m_ofn.lpstrTitle="保存平差解算参数";

	if(outputdlg.DoModal()!=IDOK)
		goto FUNC_END;

	{
		CString  strOutput=outputdlg.GetPathName();

		CalibParam_Type ctype;
		//	ctype=Calib_rph_sa0;
		//	ctype=Calib_rph;
		//	ctype=Calib_rph_drange;
		//	ctype=Calib_rph_drange_sa0;
		ctype=Calib_rph/*|LidAdj_leverArm*/;


		pX[0] = 0;
		pX[1] = 0;
		pX[2] = 0;
		pX[3] = 0;
		pX[4] = 0;
		pX[5] = 0;


		//	ctype=Calib_rphxyz_sa0;
		//	ctype=Calib_rphxyz_sa0_sac;

		//有线性系统改正
		//	LidMC_Misalign_VP_Triangle_xyzrph_semiXYZ(nPatch, ngcp, pLidPatch, pX, 6);
		//无系统改正
		CLidCalib_VCP calib;
		calib.LidMC_Misalign_NN(strOutput.GetBuffer(128), nVP, ngcp, pLidVP, LidAdj_TP_UseHorVer,pX, oriParam, ctype);
	}

FUNC_END:
	if(pLidVP)	delete[] pLidVP;	pLidVP=NULL;
	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pTieLut)  delete pTieLut;  pTieLut=NULL;
}

//利用最近点的检校
void CMainFrame::OnLidcalibNnp()
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
//	CATNPtIndex	index(getPlatform());
	CKDTPtIndex index(getPlatform());

	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidMC_VP *pLidVP=NULL;
	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch, nVP, iVP;
	//	double *pZObs=NULL;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	int nLine, nTies;
	POINT3D pt3D, *ptBuf;
	int ngcp;
	//	ATNTP_Point *pATNPoints=NULL;

	pTieList=m_AlgPrj.GetTieObjList();
	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	//	nTies=20;	//测试用
	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为1
		pTieLut[i]=-1;
	}

	std::vector<LidPt_SurvInfo> ATN_pts;
	LidPt_SurvInfo  tempSurInfo;
	nPatch=0;
	nVP=0;
	ngcp=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pTieChain=pTieList->GetAt(i);  //遍历每一个点链

		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());

		if(pTieChain->type==TO_PATCH)
		{
			nPatch+=pTieChain->pChain->GetSize();	
			//			continue;
		}

		if(pTieChain->type==TO_POINT)
			nVP+=pTieChain->pChain->GetSize();

		if(pTieChain->bGCP) 
			ngcp+=pTieChain->pChain->GetSize();


		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	if(nVP>0)
		pLidVP=new LidMC_VP[nVP];

	if(nPatch>0)
		pLidPlanes=new LidMC_Plane[nPatch];
	//	pATNPoints=new ATNTP_Point[nPatch*3];

	//	pZObs=new double[nPatch];
	int ptNum;
	locateresult   result;
	int tieID=0;
	iPatch=0; iVP=0;
	double radius=2.0;
	double rmse_th=0.2;
	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点
			k=pTieLut[j*nLine+i];
			if(k<0)  //判断当前连接点是否存在于指定条带上
				continue;

			pTieChain=pTieList->GetAt(j);
			pObj=pTieChain->pChain->GetAt(k);

			if(pTieChain->type==TO_PATCH)
			{
				ptNum=((TiePatch*)pObj)->ptNum;
				ptBuf=((TiePatch*)pObj)->pt3D;

				pLidPlanes[iPatch].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidPlanes[iPatch].tieID=pTieChain->TieID-1;
				tieID++;

				index.GetPoints(ptBuf, ptNum, pLidPlanes[iPatch].pLidPoints);

				iPatch++;
			}
			else if(pTieChain->type==TO_POINT)
			{
				char tptype =  ((TiePoint*)pObj)->tpType;

				pt3D=((TiePoint*)pObj)->pt3D;

				pLidVP[iVP].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidVP[iVP].tieID=pTieChain->TieID-1;
				tieID++;

				double dis;
				index.GetNNPoint(&pt3D, radius, &ATN_pts/*pLidVP[iVP].pLidPoints*/, &dis); 
				//				ASSERT(result!=OUTSIDE);

				if(dis > radius)
				{//扫描漏洞，剔除
					continue;
				}

				pLidVP[iVP].pLidPoints->assign(1, tempSurInfo);
//				ATN2LidPt_SurvInfo(&ATN_pts[0], &(*(pLidVP[iVP].pLidPoints))[0], 1);
				ATN_pts[0] = (*(pLidVP[iVP].pLidPoints))[0];

				pLidVP[iVP].vx = (*(pLidVP[iVP].pLidPoints))[0].x;
				pLidVP[iVP].vy = (*(pLidVP[iVP].pLidPoints))[0].y;
				pLidVP[iVP].vz = (*(pLidVP[iVP].pLidPoints))[0].z;

				if(!tptype)
				{//未定义连接点类型，通过粗糙度计算
//					if(rmse<rmse_th)
						pLidVP[iVP].VP_type=TP_type_horizon;
// 					else
// 						pLidVP[iVP].VP_type=TP_type_horizon;
				}
				else
				{//直接使用预先定义的连接点类型
					pLidVP[iVP].VP_type=tptype;
				}
				iVP++;
			}
		}

	}

	if(nVP>iVP)	nVP=iVP;

	//对pLidVP按连接点ID排序
	qsort(pLidVP, nVP, sizeof(LidMC_VP), LidMC_VP_ascending_tieID);

	//对连接点进行检查
	int TieHorNum=0, TieHorVerNum=0, TieVerNum=0;
	int tiePtNum=0;
	for(i=0; i<nVP; )
	{
		int sPos, ePos;
		tieID=pLidVP[i].tieID;
		char vp_type=pLidVP[i].VP_type;
		bool bUpdate=false;

		tiePtNum++;
		sPos=i;
		ePos=i;

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(pLidVP[i].VP_type!=vp_type)
			{
				bUpdate=true;
			}
			ePos=i;

			i++;
		}

		if(bUpdate)
		{
			vp_type=TP_type_horizon;
			for(j=sPos; j<=ePos; j++)
			{
				pLidVP[j].VP_type=vp_type;
			}
		}

		if(vp_type & TP_type_hor_ver)	TieHorVerNum++;
		else if(vp_type & TP_type_horizon)	TieHorNum++;
		else if(vp_type & TP_type_vertical)	TieVerNum++;

	}

	TRACE("total tie point num: %d\n", tiePtNum);
	TRACE("horizon and vertical tie point num: %d\n", TieHorVerNum);
	TRACE("horizon tie point num: %d\n", TieHorNum);
	TRACE("vertical tie point num: %d\n", TieVerNum);

	//为控制点赋值
	for(i=0; i<ngcp; i++)
	{
		j=pLidVP[i].tieID;

		//		if(i<ngcp)
		//		{//控制点必须排在最开始
		pTieChain=pTieList->GetAt(j);
		ASSERT(pTieChain->bGCP);

		pLidVP[i].eX=pTieChain->gcp.X;
		pLidVP[i].eY=pTieChain->gcp.Y;
		pLidVP[i].eZ=pTieChain->gcp.Z;
		//		}
	}

	//调整连接点ID,连续排列 (漏洞点删除后占用的ID)
	for(i=0, j=0; i<nVP; )
	{
		tieID=pLidVP[i].tieID;
		if(tieID!=j)
		{	
			pLidVP[i].tieID=j;	
		}

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(tieID!=j)
			{	
				pLidVP[i].tieID=j;	
			}
			i++;
		}
		j++;
	}

	orsLidSysParam oriParam;
	memset(&oriParam, 0, sizeof(orsLidSysParam));

	double pX[8];	//线元素，角元素
	memset(pX, 0, sizeof(double)*8);

	//	pX[3]=/*-0.00576609*/0;	pX[4]=/*0.00258278*/0;	pX[5]=/*-0.00138682*/0;

	CFileDialog  outputdlg(FALSE," ",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	
	outputdlg.m_ofn.lpstrTitle="保存平差解算参数";

	if(outputdlg.DoModal()!=IDOK)
		goto FUNC_END;

	{
		CString  strOutput=outputdlg.GetPathName();

		CalibParam_Type ctype;
		//	ctype=Calib_rph_sa0;
		//	ctype=Calib_rph;
		//	ctype=Calib_rph_drange;
		//	ctype=Calib_rph_drange_sa0;
		ctype=Calib_rph;

		//有线性系统改正
		//	LidMC_Misalign_VP_Triangle_xyzrph_semiXYZ(nPatch, ngcp, pLidPatch, pX, 6);
		//无系统改正
		CLidCalib_VCP calib;
		calib.LidMC_Misalign_NN(strOutput.GetBuffer(128), nVP, ngcp, pLidVP, LidAdj_TP_UseHor,pX, oriParam, ctype);
	}

FUNC_END:
	if(pLidVP)	delete[] pLidVP;	pLidVP=NULL;
	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pTieLut)  delete pTieLut;  pTieLut=NULL;
	//	if(pATNPoints)	delete pATNPoints;	pATNPoints=NULL;
}


#include "LidCalib\PlaneConstraint_Model.h"
int LidMC_Plane_ascending_tieID(const void *p1, const void  *p2)
{
	LidMC_Plane *elem1, *elem2;
	elem1=(LidMC_Plane *)p1;
	elem2=(LidMC_Plane *)p2;

	if(elem1->tieID < elem2->tieID)
		return -1;
	else 
		if(elem1->tieID > elem2->tieID)
			return 1;
		else
			return 0;
}
//共面条件检校方法
void CMainFrame::OnLidcalibSkaloud()
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
//	CATNPtIndex	index(getPlatform());

	CKDTPtIndex index(getPlatform());
	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch;
	//	double x0, y0;
	POINT3D *pt3D;
	int ptNum;
	int nLine, nTies;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	//	std::vector<ATNTP_Point> *pLidPoints;


	pTieList=m_AlgPrj.GetTieObjList();
	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见

	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为-1
		pTieLut[i]=-1;
	}

	nPatch=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pChain=pTieList->GetAt(i);  //遍历每一个点链

		//		if(pChain->TieID==6) continue;
		ASSERT(pChain->objNum==pChain->pChain->GetSize());
		nPatch+=pChain->pChain->GetSize();	

		for(j=0; j<pChain->objNum; j++)
		{
			pObj=pChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	pLidPlanes=new LidMC_Plane[nPatch];
	iPatch=0;
	int tieID=0;
	//	vector<ATNTP_Point> tmpPoints;

	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点
			k=pTieLut[j*nLine+i];
			if(k<0)
				continue;

			pChain=pTieList->GetAt(j);
			//			if(pChain->TieID==6) continue;
			pObj=pChain->pChain->GetAt(k);

			ptNum=((TiePatch*)pObj)->ptNum;
			pt3D=((TiePatch*)pObj)->pt3D;


			pLidPlanes[iPatch].pLidPoints=new std::vector<LidPt_SurvInfo>;
			index.GetPoints(pt3D, ptNum, pLidPlanes[iPatch].pLidPoints);
			pLidPlanes[iPatch].tieID=pChain->TieID-1;
			tieID++;

			iPatch++;
		}

	}

	//对pLidPlanes按连接点ID排序
	qsort(pLidPlanes, nPatch, sizeof(LidMC_Plane), LidMC_Plane_ascending_tieID);

	////////////////////////////////////////////////////////////////////////
	//重新统计连接平面，删除少于两个的面片的连接面

	iPatch = 0;
	int ptnumTh = 4;
	tieID = -1;
	int numOfPatch=0;
	int startID, endID;
	int ndummy = 0;

	for(i=0, startID=0; i<nPatch; i++)
	{
		if(tieID != pLidPlanes[i].tieID)
		{//开始新的连接面

			tieID = pLidPlanes[i].tieID;
			endID = i;

			if(numOfPatch<2)
			{//连接面不足两个，则删除整个连接面
				for(j=startID; j<endID; j++)
				{
					if(pLidPlanes[j].tieID > 0)
					{//点数大于阈值，但由于其他条带上的连接面片过小，需要删除整个连接面链
						pLidPlanes[j].tieID = -1;
						ndummy++;
					}
				}
			}
			//重新计数
			startID = i;
			numOfPatch = 1;
			if(pLidPlanes[i].pLidPoints->size() < ptnumTh)
			{
				pLidPlanes[i].tieID = -1;
				ndummy++;
				numOfPatch--;
			}
		}
		else
		{
			if(pLidPlanes[i].pLidPoints->size() < ptnumTh)
			{
				pLidPlanes[i].tieID = -1;
				ndummy++;
				numOfPatch--;
			}
			else
				numOfPatch++;
		}
	}

	//补上最后一个
	if(numOfPatch<2)
	{//连接面不足两个，则删除整个连接面
		for(j=startID; j<nPatch; j++)
		{
			if(pLidPlanes[j].tieID > 0)
			{//点数大于阈值，但由于其他条带上的连接面片过小，需要删除整个连接面链
				pLidPlanes[j].tieID = -1;
				ndummy++;
			}
		}
	}

	//重新排序，前ndummy为删除的面片
	qsort(pLidPlanes, nPatch, sizeof(LidMC_Plane), LidMC_Plane_ascending_tieID);

	///////////////////////////////////////////////////////
	//将tieID调整为连续的
	tieID = 0;
	int curID=0;
	for(i=ndummy, startID=ndummy; i<nPatch; i++)
	{
		if(tieID != pLidPlanes[i].tieID)
		{
			tieID = pLidPlanes[i].tieID;
			endID = i;

			for(j=startID; j<endID; j++)
			{
				pLidPlanes[j].tieID =curID;
			}
			startID = i;
			curID++;
		}

	}

	//补上最后一个
	for(i=startID; j<nPatch; j++)
	{
		pLidPlanes[j].tieID =curID;
	}


	//ransac筛选出共面点
	LidPlane_ransac(pLidPlanes+ndummy, nPatch-ndummy);


	orsLidSysParam oriParam;
	memset(&oriParam, 0, sizeof(orsLidSysParam));

	CalibParam_Type type;
	//	type = Calib_rph;
	//	type = Calib_rph_sa0;
	type = Calib_rph_drange;
	//	type = Calib_rph_drange_sa0;

	double pX[5];
	int nCalib;
	memset(pX, 0, sizeof(double)*5);
	//用初值重新计算点集
	//	pX[3]=/*-0.00576609*/0;	pX[4]=/*0.00258278*/0;	pX[5]=/*-0.00138682*/0;

	// 	pX[0] = oriParam.misalign_angle[0] = -0.001961905;
	// 	pX[1] = oriParam.misalign_angle[1] = 0.002657077;
	// 	pX[2] = oriParam.misalign_angle[2] = -0.001377952;
	// 	pX[3] = oriParam.sa0 = -0.003186274;

	switch (type)
	{
	case Calib_rph:
		nCalib = 3;
		break;
	case Calib_rphxyz:
		nCalib = 6;
		break;
	case Calib_rph_appXYZ:
		nCalib = 6;
		break;
	case Calib_rphxyz_sa0:
		nCalib = 7;
		break;
	case Calib_rphxyz_sa0_sac:
		nCalib = 8;
		break;
	case Calib_rph_sa0:
		nCalib = 4;
		break;
	case Calib_rph_drange:
		nCalib = 4;
		break;
	case Calib_rph_drange_sa0:
		nCalib = 5;
		break;
	default:
		nCalib = 0;
	}

	CFileDialog  outdlg(FALSE,"检校结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput = outdlg.GetPathName();		//导出检校结果


	CLidCalib_Skaloud calib;
	calib.LidMC_Misalign_plane(strOutput.GetBuffer(128), nPatch-ndummy, pLidPlanes+ndummy, pX, oriParam, type);


	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pTieLut)		delete[] pTieLut;		pTieLut=NULL;	
}

#include "LidStripAdj/LidAdjustment.h"
void CMainFrame::OnLidadjPossup()
{
	CLidAdjustment  lidAdj(getPlatform());

	CFileDialog  dlg(TRUE,"txt",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"连接点文件(*.txt)|*.txt|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString  strTiePtFile=dlg.GetPathName();

	// 	orsCamSysExtraParam_TYPE calibType = CamGeo_rph;
	// 	orsCamSysExtraParam      calibParam;
	// 
	// 	calibParam.lever_arm[0]=calibParam.lever_arm[1]=calibParam.lever_arm[2]=0;
	// 	calibParam.boresight_angle[0]=calibParam.boresight_angle[1]=calibParam.boresight_angle[2]=0;

	CFileDialog	outputdlg( FALSE, " ", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("SA result (*.txt)|*.txt|All Files(*.*)|*.*||"));
	outputdlg.m_ofn.lpstrTitle="条带平差结果";	

	if(outputdlg.DoModal()!=IDOK)
		return;

	CString	strRstName=outputdlg.GetPathName();

	if(m_AlgPrj.GetLidList()->GetSize() >= 2)
	{
		lidAdj.setAlignPrj(&m_AlgPrj);
		lidAdj.loadTiePoints(strTiePtFile.GetBuffer(0));

		lidAdj.Lidar_StripAdjustment(strRstName.GetBuffer(0), LidSA_POSDrift);
	}
}

#include "flann/flann.h"
void CMainFrame::OnAccuracyanalystZdis()
{
	CString   strRefLas;
	CString	  strComLas;
	CString	  strZdis;

	int		interval;
	float	searchRadius;

	ref_ptr<orsIPointCloudReader> refLas = ORS_CREATE_OBJECT( orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_LAS );
	if(refLas.get() == NULL)
		return;
	if(!refLas->open(strRefLas))
		return;

	ref_ptr<orsIPointCloudReader> comLas = ORS_CREATE_OBJECT( orsIPointCloudReader, ORS_INTERFACE_POINTCLOUDSOURCE_READER_LAS );
	if(comLas.get() == NULL)
		return;
	if(!comLas->open(strComLas))
		return;

	int ptNum = refLas->get_number_of_points();
	int sampleNum = ptNum / interval;
	float *pt_coord=NULL;
	float *zDis=NULL;

	pt_coord = new float[sampleNum*3];
	zDis = new float[sampleNum];
	memset(zDis, 0, sizeof(float)*sampleNum);

	double xyz[3];
	int i=0, id=0;

	while (refLas->read_point(xyz))
	{
		if(id%interval==0){
			pt_coord[i++] = xyz[0];
			pt_coord[i++] = xyz[1];
			pt_coord[i++] = xyz[2];
		}
		id++;
	}

	FLANNParameters flannPara;
	flannPara.log_level = LOG_ERROR;		// LOG_INFO
	flannPara.log_destination = NULL;

	flannPara.algorithm = KDTREE;
	flannPara.checks = 32;
	flannPara.trees = 8;
	flannPara.branching = 32;
	flannPara.iterations = 7;
	flannPara.target_precision = -1;

	float speedup;
	FLANN_INDEX flannIndex=NULL;
	flannIndex = flann_build_index( pt_coord, sampleNum, 3, &speedup, &flannPara);

	id=0;
	i=0;
	while(comLas->read_point(xyz))
	{
		if(id%interval != 0)
			continue;

		float fxyz[3];
		fxyz[0] = xyz[0];
		fxyz[1] = xyz[1];
		fxyz[2] = xyz[2];

		float dists;
		int	 result;

		int iError = flann_find_nearest_neighbors_index( flannIndex, fxyz, 1, 
			&result, &dists, 1, flannPara.checks, &flannPara);

		if(dists > searchRadius)
			continue;

		zDis[i] = pt_coord[i*3+2] - fxyz[2];
		i++;
	}

	FILE *fp=NULL;
	fp = fopen(strZdis, "wt");

	int zNum = i;
	for(i=0; i<zNum; i++)
	{
		fprintf(fp, "%.1f\n", zDis[i]);
	}
	

	if(refLas.get()!=NULL)
		refLas->close();
	if(comLas.get()!=NULL)
		comLas->close();
	if(fp)
		fclose(fp);
	if(pt_coord)
		delete[] pt_coord;
	if(zDis)
		delete[] zDis;
}


//Skaloud's method
void CMainFrame::OnCoplanarRigorousmodel()
{
	CFileDialog  dlg(TRUE,"Corr",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	int nPatch;
	int objType;
	LidMC_Plane *pLidPlanes=NULL;

	read_CorrespFile((LPCTSTR)strFileName, &objType, &nPatch, (LidMC_Obj**)&pLidPlanes);

#if 0
	//测试提取的点云是否正确
	ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	char pFileName[128];
	for(int i=0; i<nPatch; i++)
	{
		int sID = (*(pLidPlanes[i].pLidPoints))[0].stripID;
		sprintf(pFileName, "H:\\temp\\%d_%d.las", sID, pLidPlanes[i].tieID);
		writer->open(pFileName, lasPOINT);

		for(int j=0; j<pLidPlanes[i].pLidPoints->size(); j++)
		{
			double coord[3];
			coord[0] = (*(pLidPlanes[i].pLidPoints))[j].x;
			coord[1] = (*(pLidPlanes[i].pLidPoints))[j].y;
			coord[2] = (*(pLidPlanes[i].pLidPoints))[j].z;
			writer->write_point(coord);
		}

		writer->close();
	}
#endif

	orsLidSysParam oriParam;
	memset(&oriParam, 0, sizeof(orsLidSysParam));

	CalibParam_Type type;
	type = Calib_rph;
	//	type = Calib_rph_sa0;
	//type = Calib_rph_drange;
	//	type = Calib_rph_drange_sa0;

	double pX[5];
	int nCalib;
	memset(pX, 0, sizeof(double)*5);
	//用初值重新计算点集
	//	pX[3]=/*-0.00576609*/0;	pX[4]=/*0.00258278*/0;	pX[5]=/*-0.00138682*/0;

	 	pX[0] = oriParam.boresight_angle[0] = 0.0;
	 	pX[1] = oriParam.boresight_angle[1] = 0.0;
	 	pX[2] = oriParam.boresight_angle[2] = 0.0;
	// 	pX[3] = oriParam.sa0 = -0.003186274;

	switch (type)
	{
	case Calib_rph:
		nCalib = 3;
		break;
	case Calib_rphxyz:
		nCalib = 6;
		break;
	case Calib_rph_appXYZ:
		nCalib = 6;
		break;
	case Calib_rphxyz_sa0:
		nCalib = 7;
		break;
	case Calib_rphxyz_sa0_sac:
		nCalib = 8;
		break;
	case Calib_rph_sa0:
		nCalib = 4;
		break;
	case Calib_rph_drange:
		nCalib = 4;
		break;
	case Calib_rph_drange_sa0:
		nCalib = 5;
		break;
	default:
		nCalib = 0;
	}


	CFileDialog  outdlg(FALSE,"检校结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput = outdlg.GetPathName();		//导出检校结果


	CLidCalib_Skaloud calib;
	LidCalib_SetPlatform(getPlatform());
	calib.LidMC_Misalign_plane(strOutput.GetBuffer(128), nPatch, pLidPlanes, pX, oriParam, type);


	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
//	if(pTieLut)		delete[] pTieLut;		pTieLut=NULL;	
}




#include "CalibUtility/BestFitLib.h"
#include "dpsMatrix.h"
#include "LidCalib/CalibModel_Yuan.h"
//Yuan's model
void CMainFrame::OnCoplanarNotrajYuan()
{
	CFileDialog  dlg(TRUE,"Corr",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	int nPatch;
	int objType;
	LidMC_Plane *pLidPlanes=NULL;

	read_CorrespFile((LPCTSTR)strFileName, &objType, &nPatch, (LidMC_Obj**)&pLidPlanes);

#if 0
	LidMC_Plane *pLidPlanesPart=NULL;
	pLidPlanesPart = new LidMC_Plane[nPatch];

	int n_Part=0;
	for(int i=0; i<nPatch; i++)
	{
		if((*(pLidPlanes[i].pLidPoints))[0].stripID<2)
			continue;

		pLidPlanesPart[n_Part].s1 = pLidPlanes[i].s1;
		pLidPlanesPart[n_Part].s2 = pLidPlanes[i].s2;
		pLidPlanesPart[n_Part].s3 = pLidPlanes[i].s3;
		pLidPlanesPart[n_Part].s4 = pLidPlanes[i].s4;
		pLidPlanesPart[n_Part].tieID = pLidPlanes[i].tieID;
		pLidPlanesPart[n_Part].pLidPoints = new std::vector<LidPt_SurvInfo>;
		*(pLidPlanesPart[n_Part].pLidPoints) = *(pLidPlanes[i].pLidPoints);
		n_Part++;
	}


#endif


#if 0
	//测试提取的点云是否正确
	ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	char pFileName[128];
	for(int i=0; i<nPatch; i++)
	{
		int sID = (*(pLidPlanes[i].pLidPoints))[0].stripID;
		sprintf(pFileName, "H:\\temp\\%d_%d.las", sID, pLidPlanes[i].tieID);
		writer->open(pFileName, lasPOINT);

		for(int j=0; j<pLidPlanes[i].pLidPoints->size(); j++)
		{
			double coord[3];
			coord[0] = (*(pLidPlanes[i].pLidPoints))[j].x;
			coord[1] = (*(pLidPlanes[i].pLidPoints))[j].y;
			coord[2] = (*(pLidPlanes[i].pLidPoints))[j].z;
			writer->write_point(coord);
		}

		writer->close();
	}
#endif


	orsLidYuanParam *pOriParam=NULL;


	/////////////////////////////////////////////////////
	//准备每条带的旋转矩阵和参考中心点
	////////////////////////////////////////////////////
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	Align_LidLine lidLine;

	int i, j, k;
	int nLine;


	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();

	pOriParam = new orsLidYuanParam[nLine];   //每条带参数
	memset(pOriParam, 0, sizeof(orsLidYuanParam)*nLine);

	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);


		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead(lidLine.LasName);
		//		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead("H:\\temp\\LDR101108_011429_ATN_TP.LAS");
		assert(pReader);

		double coord[3];
		std::vector<orsPOINT3D>  strip_edges;
		//		std::vector<orsPOINT2D>  cent_line;
		std::vector<double>  cent_line;
		std::vector<double>  line_param;

		pReader->read_point(coord);
		bool scan_direction_flag = pReader->get_scan_direction_flag();
		// 		center.X = coord[0];
		// 		center.Y = coord[1];
		// 		center.Z = coord[2];

		while(pReader->read_point(coord))
		{


			// 			if(pReader->get_edge_of_flight_line())
			// 			{
			// 				int eee=1;
			// 			}

			//首先找到同一扫描线的两个边缘点，然后计算中心点坐标
			if(scan_direction_flag==pReader->get_scan_direction_flag())
				continue;

			// 			center.X += coord[0];
			// 			center.Y += coord[1];
			// 			center.Z += coord[2];

			scan_direction_flag = pReader->get_scan_direction_flag();
			orsPOINT3D pt;
			pt.X = coord[0];
			pt.Y = coord[1];
			pt.Z = coord[2];
			strip_edges.push_back(pt);			
		}
		pReader->close();

		//中心线的中心（飞行轨迹中心）
		orsPOINT3D  center;
		center.X = center.Y = center.Z = 0;

		orsPOINT2D pt2D;
		//默认边缘点成对出现
		for(j=0; j<strip_edges.size(); j++)
		{
			if(j%2==0)
			{
				pt2D.x=strip_edges[j].X;
				pt2D.y=strip_edges[j].Y;
			}
			else
			{
				pt2D.x += strip_edges[j].X;
				pt2D.y += strip_edges[j].Y;

				pt2D.x /= 2;
				pt2D.y /= 2;

				cent_line.push_back(pt2D.x);
				cent_line.push_back(pt2D.y);
				center.X += pt2D.x;
				center.Y += pt2D.y;
			}

		}


#if 0
		//测试提取的点云中心线是否正确
		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_centerline.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<cent_line.size(); j++)
		{
			double coord[3];
			coord[0] = cent_line[j].x;
			coord[1] = cent_line[j].y;
			coord[2] = 0;
			writer->write_point(coord);
		}

		writer->close();

		sprintf(pFileName, "H:\\temp\\%d_stripEdges.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<strip_edges.size(); j++)
		{
			double coord[3];
			coord[0] = strip_edges[j].X;
			coord[1] = strip_edges[j].Y;
			coord[2] = strip_edges[j].Z;
			writer->write_point(coord);
		}

		writer->close();
#endif
		//		line2DFitting(cent_line, line_param);   //line_param: nx, ny, ax, ay

		double line_grad, line_intercept;
		int cent_line_ptNum = cent_line.size()/2;
		center.X /= cent_line_ptNum;
		center.Y /= cent_line_ptNum;

		line_fitting(cent_line_ptNum, &cent_line[0], &line_grad, &line_intercept);
		//确定直线的方向
		double x1, y1, x2, y2;
		double theta = atan(line_grad);//直线方向角
		if(line_grad>1.0)
		{//slope > 45deg
			x1 = cent_line[0];
			x2 = cent_line[2*(cent_line_ptNum-1)];
			y1 = line_grad*x1+line_intercept;
			y2 = line_grad*x2+line_intercept;

			if((x2-x1)*line_grad < 0)
			{
				theta += PI;
			}
		}
		else
		{// slope < 45 deg
			y1 = cent_line[1];
			y2 = cent_line[2*cent_line_ptNum-1];
			x1 = (y1-line_intercept)/line_grad;
			x2 = (y2-line_intercept)/line_grad;

			if((y2-y1)*line_grad < 0)
			{
				theta += PI;
			}
		}


		pOriParam[i].stripCenter = center;
		pOriParam[i].stripCenter.Z = lidLine.averFH;

		//		if(fabs(line_param[0])>1e-6)
		//顺时针旋转，角度为负
		getMatrixService()->RotateMatrix_heading(/*line_param[1]/line_param[0]*/-theta, pOriParam[i].rotMatZ);

#if 0
		//测试点云改正后是否正确

		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_rotated.las", i);
		writer->open(pFileName, lasPOINT);

		pReader->reopen();
		while(pReader->read_point(coord))
		{
			coord[0] -= center.X;
			coord[1] -= center.Y;
			coord[2] -= center.Z;
			double tmp[3];
			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
			//			memcpy(tmp, coord, sizeof(double)*3);

			unsigned short intensity = pReader->get_intensity();
			unsigned char return_number = pReader->get_return_number();
			unsigned char number_of_returns_of_given_pulse = pReader->get_number_of_returns();
			unsigned char classification = pReader->get_classification();
			double gpstime = pReader->get_gpstime();

			unsigned char scan_direction_flag = pReader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = pReader->get_edge_of_flight_line();
			char scan_angle_rank = pReader->get_scan_angle_rank();
			unsigned char user_data = pReader->get_user_data();
			unsigned short point_source_ID = pReader->get_point_source_ID();

			//			writer->write_point(tmp);
			writer->write_point(tmp, intensity, return_number, 
				number_of_returns_of_given_pulse, scan_direction_flag, 
				edge_of_flight_line, classification, scan_angle_rank, user_data, gpstime);
		}

		// 		for(j=0; j<cent_line.size()/2; j++)
		// 		{
		// 			double coord[3];
		// 			coord[0] = cent_line[2*j] - center.X;
		// 			coord[1] = cent_line[2*j+1] - center.Y;
		// 			coord[2] = 0;
		// 
		// 			double tmp[3];
		// 			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
		// 			writer->write_point(tmp);
		// 		}

		writer->close();
#endif

	}

	//未知数求解


	double pX[5];
	int nCalib;
	memset(pX, 0, sizeof(double)*5);
	//用初值重新计算点集
//	pX[3] = 0.000374028;  //heading
//	pX[4] = -0.004084307; //roll

	CFileDialog  outdlg(FALSE,"检校结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput = outdlg.GetPathName();		//导出检校结果


	CLidCalib_Yuan calib;
	LidCalib_SetPlatform(getPlatform());
	calib.LidMC_Calib_Coplanar(strOutput.GetBuffer(128), nPatch/*n_Part*/, 0, (LidMC_Obj*)pLidPlanes/*pLidPlanesPart*/, 
		pX, pOriParam, nLine);


	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pOriParam)	delete[] pOriParam;		pOriParam=NULL;
}

#include "Lidcalib/CalibModel_Jing.h"
void CMainFrame::OnCoplanarNotrajJing()
{
	CFileDialog  dlg(TRUE,"Corr",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	int nPatch;
	int objType;
	LidMC_Plane *pLidPlanes=NULL;

	read_CorrespFile((LPCTSTR)strFileName, &objType, &nPatch, (LidMC_Obj**)&pLidPlanes);

#if 0
	LidMC_Plane *pLidPlanesPart=NULL;
	pLidPlanesPart = new LidMC_Plane[nPatch];

	int n_Part=0;
	for(int i=0; i<nPatch; i++)
	{
		if((*(pLidPlanes[i].pLidPoints))[0].stripID<2)
			continue;

		pLidPlanesPart[n_Part].s1 = pLidPlanes[i].s1;
		pLidPlanesPart[n_Part].s2 = pLidPlanes[i].s2;
		pLidPlanesPart[n_Part].s3 = pLidPlanes[i].s3;
		pLidPlanesPart[n_Part].s4 = pLidPlanes[i].s4;
		pLidPlanesPart[n_Part].tieID = pLidPlanes[i].tieID;
		pLidPlanesPart[n_Part].pLidPoints = new std::vector<LidPt_SurvInfo>;
		*(pLidPlanesPart[n_Part].pLidPoints) = *(pLidPlanes[i].pLidPoints);
		n_Part++;
	}


#endif


#if 0
	//测试提取的点云是否正确
	ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	char pFileName[128];
	for(int i=0; i<nPatch; i++)
	{
		int sID = (*(pLidPlanes[i].pLidPoints))[0].stripID;
		sprintf(pFileName, "H:\\temp\\%d_%d.las", sID, pLidPlanes[i].tieID);
		writer->open(pFileName, lasPOINT);

		for(int j=0; j<pLidPlanes[i].pLidPoints->size(); j++)
		{
			double coord[3];
			coord[0] = (*(pLidPlanes[i].pLidPoints))[j].x;
			coord[1] = (*(pLidPlanes[i].pLidPoints))[j].y;
			coord[2] = (*(pLidPlanes[i].pLidPoints))[j].z;
			writer->write_point(coord);
		}

		writer->close();
	}
#endif


	orsLidJingParam *pOriParam=NULL;


	/////////////////////////////////////////////////////
	//准备每条带的旋转矩阵和参考中心点
	////////////////////////////////////////////////////
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	Align_LidLine lidLine;

	int i, j, k;
	int nLine;


	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();

	pOriParam = new orsLidJingParam[nLine];   //每条带参数
	memset(pOriParam, 0, sizeof(orsLidJingParam)*nLine);

	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);


		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead(lidLine.LasName);
		//		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead("H:\\temp\\LDR101108_011429_ATN_TP.LAS");
		assert(pReader);

		double coord[3];
		std::vector<orsPOINT3D>  strip_edges;
		//		std::vector<orsPOINT2D>  cent_line;
		std::vector<double>  cent_line;
		std::vector<double>  line_param;

		pReader->read_point(coord);
		bool scan_direction_flag = pReader->get_scan_direction_flag();
		// 		center.X = coord[0];
		// 		center.Y = coord[1];
		// 		center.Z = coord[2];

		while(pReader->read_point(coord))
		{


			// 			if(pReader->get_edge_of_flight_line())
			// 			{
			// 				int eee=1;
			// 			}

			//首先找到同一扫描线的两个边缘点，然后计算中心点坐标
			if(scan_direction_flag==pReader->get_scan_direction_flag())
				continue;

			// 			center.X += coord[0];
			// 			center.Y += coord[1];
			// 			center.Z += coord[2];

			scan_direction_flag = pReader->get_scan_direction_flag();
			orsPOINT3D pt;
			pt.X = coord[0];
			pt.Y = coord[1];
			pt.Z = coord[2];
			strip_edges.push_back(pt);			
		}
		pReader->close();

		//中心线的中心（飞行轨迹中心）
		orsPOINT3D  center;
		center.X = center.Y = center.Z = 0;

		orsPOINT2D pt2D;
		//默认边缘点成对出现
		for(j=0; j<strip_edges.size(); j++)
		{
			if(j%2==0)
			{
				pt2D.x=strip_edges[j].X;
				pt2D.y=strip_edges[j].Y;
			}
			else
			{
				pt2D.x += strip_edges[j].X;
				pt2D.y += strip_edges[j].Y;

				pt2D.x /= 2;
				pt2D.y /= 2;

				cent_line.push_back(pt2D.x);
				cent_line.push_back(pt2D.y);
				center.X += pt2D.x;
				center.Y += pt2D.y;
			}

		}


#if 0
		//测试提取的点云中心线是否正确
		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_centerline.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<cent_line.size(); j++)
		{
			double coord[3];
			coord[0] = cent_line[j].x;
			coord[1] = cent_line[j].y;
			coord[2] = 0;
			writer->write_point(coord);
		}

		writer->close();

		sprintf(pFileName, "H:\\temp\\%d_stripEdges.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<strip_edges.size(); j++)
		{
			double coord[3];
			coord[0] = strip_edges[j].X;
			coord[1] = strip_edges[j].Y;
			coord[2] = strip_edges[j].Z;
			writer->write_point(coord);
		}

		writer->close();
#endif
		//		line2DFitting(cent_line, line_param);   //line_param: nx, ny, ax, ay

		double line_grad, line_intercept;
		int cent_line_ptNum = cent_line.size()/2;
		center.X /= cent_line_ptNum;
		center.Y /= cent_line_ptNum;

		line_fitting(cent_line_ptNum, &cent_line[0], &line_grad, &line_intercept);
		//确定直线的方向
		double x1, y1, x2, y2;
		double theta = atan(line_grad);//直线方向角
		if(line_grad>1.0)
		{//slope > 45deg
			x1 = cent_line[0];
			x2 = cent_line[2*(cent_line_ptNum-1)];
			y1 = line_grad*x1+line_intercept;
			y2 = line_grad*x2+line_intercept;

			if((x2-x1)*line_grad < 0)
			{
				theta += PI;
			}
		}
		else
		{// slope < 45 deg
			y1 = cent_line[1];
			y2 = cent_line[2*cent_line_ptNum-1];
			x1 = (y1-line_intercept)/line_grad;
			x2 = (y2-line_intercept)/line_grad;

			if((y2-y1)*line_grad < 0)
			{
				theta += PI;
			}
		}


		pOriParam[i].stripCenter = center;
		pOriParam[i].stripCenter.Z = lidLine.averFH;

		//		if(fabs(line_param[0])>1e-6)
		//顺时针旋转，角度为负
		getMatrixService()->RotateMatrix_heading(/*line_param[1]/line_param[0]*/-theta, pOriParam[i].rotMatZ);

#if 0
		//测试点云改正后是否正确

		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_rotated.las", i);
		writer->open(pFileName, lasPOINT);

		pReader->reopen();
		while(pReader->read_point(coord))
		{
			coord[0] -= center.X;
			coord[1] -= center.Y;
			coord[2] -= center.Z;
			double tmp[3];
			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
			//			memcpy(tmp, coord, sizeof(double)*3);

			unsigned short intensity = pReader->get_intensity();
			unsigned char return_number = pReader->get_return_number();
			unsigned char number_of_returns_of_given_pulse = pReader->get_number_of_returns();
			unsigned char classification = pReader->get_classification();
			double gpstime = pReader->get_gpstime();

			unsigned char scan_direction_flag = pReader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = pReader->get_edge_of_flight_line();
			char scan_angle_rank = pReader->get_scan_angle_rank();
			unsigned char user_data = pReader->get_user_data();
			unsigned short point_source_ID = pReader->get_point_source_ID();

			//			writer->write_point(tmp);
			writer->write_point(tmp, intensity, return_number, 
				number_of_returns_of_given_pulse, scan_direction_flag, 
				edge_of_flight_line, classification, scan_angle_rank, user_data, gpstime);
		}

		// 		for(j=0; j<cent_line.size()/2; j++)
		// 		{
		// 			double coord[3];
		// 			coord[0] = cent_line[2*j] - center.X;
		// 			coord[1] = cent_line[2*j+1] - center.Y;
		// 			coord[2] = 0;
		// 
		// 			double tmp[3];
		// 			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
		// 			writer->write_point(tmp);
		// 		}

		writer->close();
#endif

	}

	//未知数求解


	double pX[5];
	int nCalib;
	memset(pX, 0, sizeof(double)*5);
	//用初值重新计算点集
// 	pX[3] = 0.000374028;  //heading
// 	pX[4] = -0.004084307; //roll

	CFileDialog  outdlg(FALSE,"检校结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput = outdlg.GetPathName();		//导出检校结果


	CLidCalib_Jing calib;
	LidCalib_SetPlatform(getPlatform());
	calib.LidMC_Calib_Coplanar(strOutput.GetBuffer(128), nPatch/*n_Part*/, 0, (LidMC_Obj*)pLidPlanes/*pLidPlanesPart*/, 
		pX, pOriParam, nLine);


	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pOriParam)	delete[] pOriParam;		pOriParam=NULL;
}

//Ressl's method
void CMainFrame::OnCoplanarNotrajRessl()
{
	CFileDialog  dlg(TRUE,"Corr",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	int nPatch;
	int objType;
	LidMC_Plane *pLidPlanes=NULL;

	read_CorrespFile((LPCTSTR)strFileName, &objType, &nPatch, (LidMC_Obj**)&pLidPlanes);

#if 0
	LidMC_Plane *pLidPlanesPart=NULL;
	pLidPlanesPart = new LidMC_Plane[nPatch];

	int n_Part=0;
	for(int i=0; i<nPatch; i++)
	{
		if((*(pLidPlanes[i].pLidPoints))[0].stripID<2)
			continue;

		pLidPlanesPart[n_Part].s1 = pLidPlanes[i].s1;
		pLidPlanesPart[n_Part].s2 = pLidPlanes[i].s2;
		pLidPlanesPart[n_Part].s3 = pLidPlanes[i].s3;
		pLidPlanesPart[n_Part].s4 = pLidPlanes[i].s4;
		pLidPlanesPart[n_Part].tieID = pLidPlanes[i].tieID;
		pLidPlanesPart[n_Part].pLidPoints = new std::vector<LidPt_SurvInfo>;
		*(pLidPlanesPart[n_Part].pLidPoints) = *(pLidPlanes[i].pLidPoints);
		n_Part++;
	}


#endif


#if 0
	//测试提取的点云是否正确
	ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	char pFileName[128];
	for(int i=0; i<nPatch; i++)
	{
		int sID = (*(pLidPlanes[i].pLidPoints))[0].stripID;
		sprintf(pFileName, "H:\\temp\\%d_%d.las", sID, pLidPlanes[i].tieID);
		writer->open(pFileName, lasPOINT);

		for(int j=0; j<pLidPlanes[i].pLidPoints->size(); j++)
		{
			double coord[3];
			coord[0] = (*(pLidPlanes[i].pLidPoints))[j].x;
			coord[1] = (*(pLidPlanes[i].pLidPoints))[j].y;
			coord[2] = (*(pLidPlanes[i].pLidPoints))[j].z;
			writer->write_point(coord);
		}

		writer->close();
	}
#endif


	orsLidResslParam *pOriParam=NULL;


	/////////////////////////////////////////////////////
	//准备每条带的旋转矩阵和参考中心点
	////////////////////////////////////////////////////
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	Align_LidLine lidLine;

	int i, j, k;
	int nLine;


	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();

	pOriParam = new orsLidResslParam[nLine];   //每条带参数
	memset(pOriParam, 0, sizeof(orsLidResslParam)*nLine);

	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);


		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead(lidLine.LasName);
		//		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead("H:\\temp\\LDR101108_011429_ATN_TP.LAS");
		assert(pReader);

		double coord[3];
		std::vector<orsPOINT3D>  strip_edges;
		//		std::vector<orsPOINT2D>  cent_line;
		std::vector<double>  cent_line;
		std::vector<double>  line_param;

		pReader->read_point(coord);
		bool scan_direction_flag = pReader->get_scan_direction_flag();
		// 		center.X = coord[0];
		// 		center.Y = coord[1];
		// 		center.Z = coord[2];

		while(pReader->read_point(coord))
		{


			// 			if(pReader->get_edge_of_flight_line())
			// 			{
			// 				int eee=1;
			// 			}

			//首先找到同一扫描线的两个边缘点，然后计算中心点坐标
			if(scan_direction_flag==pReader->get_scan_direction_flag())
				continue;

			// 			center.X += coord[0];
			// 			center.Y += coord[1];
			// 			center.Z += coord[2];

			scan_direction_flag = pReader->get_scan_direction_flag();
			orsPOINT3D pt;
			pt.X = coord[0];
			pt.Y = coord[1];
			pt.Z = coord[2];
			strip_edges.push_back(pt);			
		}
		pReader->close();

		//中心线的中心（飞行轨迹中心）
		orsPOINT3D  center;
		center.X = center.Y = center.Z = 0;

		orsPOINT2D pt2D;
		//默认边缘点成对出现
		for(j=0; j<strip_edges.size(); j++)
		{
			if(j%2==0)
			{
				pt2D.x=strip_edges[j].X;
				pt2D.y=strip_edges[j].Y;
			}
			else
			{
				pt2D.x += strip_edges[j].X;
				pt2D.y += strip_edges[j].Y;

				pt2D.x /= 2;
				pt2D.y /= 2;

				cent_line.push_back(pt2D.x);
				cent_line.push_back(pt2D.y);
				center.X += pt2D.x;
				center.Y += pt2D.y;
			}

		}


#if 0
		//测试提取的点云中心线是否正确
		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_centerline.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<cent_line.size(); j++)
		{
			double coord[3];
			coord[0] = cent_line[j].x;
			coord[1] = cent_line[j].y;
			coord[2] = 0;
			writer->write_point(coord);
		}

		writer->close();

		sprintf(pFileName, "H:\\temp\\%d_stripEdges.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<strip_edges.size(); j++)
		{
			double coord[3];
			coord[0] = strip_edges[j].X;
			coord[1] = strip_edges[j].Y;
			coord[2] = strip_edges[j].Z;
			writer->write_point(coord);
		}

		writer->close();
#endif
		//		line2DFitting(cent_line, line_param);   //line_param: nx, ny, ax, ay

		double line_grad, line_intercept;
		int cent_line_ptNum = cent_line.size()/2;
		center.X /= cent_line_ptNum;
		center.Y /= cent_line_ptNum;

		line_fitting(cent_line_ptNum, &cent_line[0], &line_grad, &line_intercept);
		//确定直线的方向
		double x1, y1, x2, y2;
		double theta = atan(line_grad);//直线方向角
		if(line_grad>1.0)
		{//slope > 45deg
			x1 = cent_line[0];
			x2 = cent_line[2*(cent_line_ptNum-1)];
			y1 = line_grad*x1+line_intercept;
			y2 = line_grad*x2+line_intercept;

			if((x2-x1)*line_grad < 0)
			{
				theta += PI;
			}
		}
		else
		{// slope < 45 deg
			y1 = cent_line[1];
			y2 = cent_line[2*cent_line_ptNum-1];
			x1 = (y1-line_intercept)/line_grad;
			x2 = (y2-line_intercept)/line_grad;

			if((y2-y1)*line_grad < 0)
			{
				theta += PI;
			}
		}


		pOriParam[i].stripCenter = center;
		pOriParam[i].stripCenter.Z = lidLine.averFH;

		//		if(fabs(line_param[0])>1e-6)
		//顺时针旋转，角度为负
		getMatrixService()->RotateMatrix_heading(/*line_param[1]/line_param[0]*/-theta, pOriParam[i].rotMatZ);

#if 0
		//测试点云改正后是否正确

		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_rotated.las", i);
		writer->open(pFileName, lasPOINT);

		pReader->reopen();
		while(pReader->read_point(coord))
		{
			coord[0] -= center.X;
			coord[1] -= center.Y;
			coord[2] -= center.Z;
			double tmp[3];
			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
			//			memcpy(tmp, coord, sizeof(double)*3);

			unsigned short intensity = pReader->get_intensity();
			unsigned char return_number = pReader->get_return_number();
			unsigned char number_of_returns_of_given_pulse = pReader->get_number_of_returns();
			unsigned char classification = pReader->get_classification();
			double gpstime = pReader->get_gpstime();

			unsigned char scan_direction_flag = pReader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = pReader->get_edge_of_flight_line();
			char scan_angle_rank = pReader->get_scan_angle_rank();
			unsigned char user_data = pReader->get_user_data();
			unsigned short point_source_ID = pReader->get_point_source_ID();

			//			writer->write_point(tmp);
			writer->write_point(tmp, intensity, return_number, 
				number_of_returns_of_given_pulse, scan_direction_flag, 
				edge_of_flight_line, classification, scan_angle_rank, user_data, gpstime);
		}

		// 		for(j=0; j<cent_line.size()/2; j++)
		// 		{
		// 			double coord[3];
		// 			coord[0] = cent_line[2*j] - center.X;
		// 			coord[1] = cent_line[2*j+1] - center.Y;
		// 			coord[2] = 0;
		// 
		// 			double tmp[3];
		// 			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
		// 			writer->write_point(tmp);
		// 		}

		writer->close();
#endif

	}

	//未知数求解


	double pX[5];
	int nCalib;
	memset(pX, 0, sizeof(double)*5);
	//用初值重新计算点集
	//	pX[3] = 0.000374028;  //heading
	//	pX[4] = -0.004084307; //roll

	CFileDialog  outdlg(FALSE,"检校结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput = outdlg.GetPathName();		//导出检校结果


	CLidCalib_Ressl calib;
	LidCalib_SetPlatform(getPlatform());
	calib.LidMC_Calib(strOutput.GetBuffer(128), nPatch/*n_Part*/, 0, (LidMC_Obj*)pLidPlanes/*pLidPlanesPart*/, objType, pX, pOriParam, nLine);


	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pOriParam)	delete[] pOriParam;		pOriParam=NULL;
}

bool CMainFrame::Extract_CorrespondingPlanes(LidMC_Plane **pLidPatch, int &nPlanes)
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
	//	CATNPtIndex	index(getPlatform());

	CKDTPtIndex index(getPlatform());
	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;

	LidMC_Plane *pLidPlanes=NULL;
	int nPatch, iPatch;
	//	double x0, y0;
	POINT3D *pt3D;
	int ptNum;
	int nLine, nTies;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	//	std::vector<ATNTP_Point> *pLidPoints;


	pTieList=m_AlgPrj.GetTieObjList();
	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	if(nLine==0 || nTies==0)
		return false;

	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见

	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为-1
		pTieLut[i]=-1;
	}

	nPatch=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pChain=pTieList->GetAt(i);  //遍历每一个点链

		//		if(pChain->TieID==6) continue;
		ASSERT(pChain->objNum==pChain->pChain->GetSize());
		nPatch+=pChain->pChain->GetSize();	

		for(j=0; j<pChain->objNum; j++)
		{
			pObj=pChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	pLidPlanes=new LidMC_Plane[nPatch];
	*pLidPatch = pLidPlanes;

	iPatch=0;
	int tieID=0;
	//	vector<ATNTP_Point> tmpPoints;

	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点
			k=pTieLut[j*nLine+i];
			if(k<0)
				continue;

			pChain=pTieList->GetAt(j);
			//			if(pChain->TieID==6) continue;
			pObj=pChain->pChain->GetAt(k);

			ptNum=((TiePatch*)pObj)->ptNum;
			pt3D=((TiePatch*)pObj)->pt3D;


			pLidPlanes[iPatch].pLidPoints=new std::vector<LidPt_SurvInfo>;
			index.GetPoints(pt3D, ptNum, pLidPlanes[iPatch].pLidPoints);  //从多边形中取点
			for(int ip=0; ip<pLidPlanes[iPatch].pLidPoints->size(); ip++)
			{
				(*(pLidPlanes[iPatch].pLidPoints))[ip].stripID = i;  //记录点云所属的条带号
			}

			pLidPlanes[iPatch].tieID=pChain->TieID-1;
			tieID++;

			iPatch++;
		}

	}

	//对pLidPlanes按连接点ID排序
	qsort(pLidPlanes, nPatch, sizeof(LidMC_Plane), LidMC_Plane_ascending_tieID);

	////////////////////////////////////////////////////////////////////////
	//重新统计连接平面，删除少于两个的面片的连接面

	iPatch = 0;
	int ptnumTh = 4;
	tieID = -1;
	int numOfPatch=0;
	int startID, endID;
	int ndummy = 0;

	for(i=0, startID=0; i<nPatch; i++)
	{
		if(tieID != pLidPlanes[i].tieID)
		{//开始新的连接面

			tieID = pLidPlanes[i].tieID;
			endID = i;

			if(numOfPatch<2)
			{//连接面不足两个，则删除整个连接面
				for(j=startID; j<endID; j++)
				{
					if(pLidPlanes[j].tieID > 0)
					{//点数大于阈值，但由于其他条带上的连接面片过小，需要删除整个连接面链
						pLidPlanes[j].tieID = -1;
						ndummy++;
					}
				}
			}
			//重新计数
			startID = i;
			numOfPatch = 1;
			if(pLidPlanes[i].pLidPoints->size() < ptnumTh)
			{
				pLidPlanes[i].tieID = -1;
				ndummy++;
				numOfPatch--;
			}
		}
		else
		{
			if(pLidPlanes[i].pLidPoints->size() < ptnumTh)
			{
				pLidPlanes[i].tieID = -1;
				ndummy++;
				numOfPatch--;
			}
			else
				numOfPatch++;
		}
	}

	//补上最后一个
	if(numOfPatch<2)
	{//连接面不足两个，则删除整个连接面
		for(j=startID; j<nPatch; j++)
		{
			if(pLidPlanes[j].tieID > 0)
			{//点数大于阈值，但由于其他条带上的连接面片过小，需要删除整个连接面链
				pLidPlanes[j].tieID = -1;
				ndummy++;
			}
		}
	}

	//重新排序，前ndummy为删除的面片
	qsort(pLidPlanes, nPatch, sizeof(LidMC_Plane), LidMC_Plane_ascending_tieID);

	///////////////////////////////////////////////////////
	//将tieID调整为连续的
	tieID = 0;
	int curID=0;
	for(i=ndummy, startID=ndummy; i<nPatch; i++)
	{
		if(tieID != pLidPlanes[i].tieID)
		{
			tieID = pLidPlanes[i].tieID;
			endID = i;

			for(j=startID; j<endID; j++)
			{
				pLidPlanes[j].tieID =curID;
			}
			startID = i;
			curID++;
		}

	}

	//补上最后一个
	for(i=startID; j<nPatch; j++)
	{
		pLidPlanes[j].tieID =curID;
	}

	*pLidPatch = pLidPlanes+ndummy;
	nPlanes = nPatch-ndummy;

	if(pTieLut)		delete[] pTieLut;		pTieList=NULL;
	return true;
}



bool CMainFrame::Extract_ConjugatePoints(LidMC_VP **pLidConPts, int &nPts)
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	//	orsVector<orsIImageGeometry*> 	imgGeoList;

	int i, j, k;
	TieObjChain *pTieChain=NULL;
	Align_LidLine lidLine;
	TieObject *pObj=NULL;
	//	CATNPtIndex	index(getPlatform());

	CKDTPtIndex index(getPlatform());

	int srcID;
	//	std::vector<ATNTP_Point> ATNpt_List;
	LidMC_VP *pLidVP=NULL;
	int nVP, iVP;
	//	double *pZObs=NULL;
	long *pTieLut=NULL;	//连接点的查找表矩阵
	int nLine, nTies;
	POINT3D pt3D, *ptBuf;
	int ngcp;
	//	ATNTP_Point *pATNPoints=NULL;


	pTieList=m_AlgPrj.GetTieObjList();
	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();
	nTies=pTieList->GetSize();

	if(nLine==0 || nTies==0)
		return false;

	pTieLut=new long[nTies*nLine];	//记录每个连接点是否在各条带上可见
	for(i=0; i<nTies*nLine; i++)
	{//不存在连接点时为-1
		pTieLut[i]=-1;
	}

	LidPt_SurvInfo atnpts;
	std::vector<LidPt_SurvInfo> ATN_pts;
	ATN_pts.assign(3, atnpts);
	std::vector<LidPt_SurvInfo> ATN_Vec;
	LidPt_SurvInfo  tempSurInfo;
	std::vector<LidPt_SurvInfo> surv_Vec;
	nVP=0;
	ngcp=0;
	for(i=0; i<pTieList->GetSize(); i++)
	{//统计虚拟连接点数
		pTieChain=pTieList->GetAt(i);  //遍历每一个点链

		ASSERT(pTieChain->objNum==pTieChain->pChain->GetSize());

		if(pTieChain->type==TO_POINT)
			nVP+=pTieChain->pChain->GetSize();


		if(pTieChain->bGCP) 
			ngcp+=pTieChain->pChain->GetSize();


		for(j=0; j<pTieChain->objNum; j++)
		{
			pObj=pTieChain->pChain->GetAt(j);

			if(pObj->sourceType==ST_LiDAR)
			{//记录在链表中的序号
				srcID=pObj->sourceID-1;
				pTieLut[i*nLine+srcID]=j;
			}
		}
	}

	if(nVP>0)
		pLidVP=new LidMC_VP[nVP];
	

	*pLidConPts = pLidVP;


	int ptNum;
	locateresult   result;
	int tieID=0;
	iVP=0;
	double radius=2.0;
	double rmse_th=0.2;
	//按条带顺序初始化连接点，保证不重复用索引
	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		if(index.Open(lidLine.LasName.GetBuffer(256))==false)
			continue;

		tieID=0;
		for(j=0; j<nTies; j++)
		{//遍历当前条带所有连接点


			k=pTieLut[j*nLine+i];
			if(k<0)  //判断当前连接点是否存在于指定条带上
				continue;

			pTieChain=pTieList->GetAt(j);

			pObj=pTieChain->pChain->GetAt(k);

			if(pTieChain->type==TO_POINT)
			{
				char tptype =  ((TiePoint*)pObj)->tpType;

				pt3D=((TiePoint*)pObj)->pt3D;

				pLidVP[iVP].pLidPoints=new std::vector<LidPt_SurvInfo>;
				pLidVP[iVP].tieID=pTieChain->TieID-1;
				//				pLidVP[iVP].eX=pChain->gcp.X;
				//				pLidVP[iVP].eY=pChain->gcp.Y;
				//				pLidVP[iVP].eZ=pChain->gcp.Z;
				tieID++;

				double rmse;
				result=index.GetTriangleVertex(&pt3D, radius, &ATN_pts/*pLidVP[iVP].pLidPoints*/, &rmse); 
				//				ASSERT(result!=OUTSIDE);

				if(result==OUTSIDE)
				{//扫描漏洞，剔除
					// 					pLidVP[iVP].VP_type=TP_type_horizon;
					// 					pLidVP[iVP].vx=pt3D.X;
					// 					pLidVP[iVP].vy=pt3D.Y;
					// 					pLidVP[iVP].reliability=0;
					// 
					// 					iVP++;
					continue;
				}

				pLidVP[iVP].pLidPoints->assign(3, tempSurInfo);
				//				ATN2LidPt_SurvInfo(&ATN_pts[0], &(*(pLidVP[iVP].pLidPoints))[0], 3);

				for(int ii=0; ii<3; ii++)
				{
					(*(pLidVP[iVP].pLidPoints))[ii] = ATN_pts[ii];
					(*(pLidVP[iVP].pLidPoints))[ii].stripID = i;
				}

				ATN_Vec.insert(ATN_Vec.end(), ATN_pts.begin(), ATN_pts.end());
				surv_Vec.insert(surv_Vec.end(), pLidVP[iVP].pLidPoints->begin(), pLidVP[iVP].pLidPoints->end());

				pLidVP[iVP].vx=pt3D.X;
				pLidVP[iVP].vy=pt3D.Y;
				pLidVP[iVP].vz=TriangleInterpolate(&ATN_pts, pt3D.X, pt3D.Y);

				if(!tptype)
				{//未定义连接点类型，通过粗糙度计算
					if(rmse<rmse_th)
						pLidVP[iVP].VP_type=TP_type_hor_ver;
					else
						pLidVP[iVP].VP_type=TP_type_horizon;
				}
				else
				{//直接使用预先定义的连接点类型
					pLidVP[iVP].VP_type=tptype;
				}
				iVP++;
			}
		}

	}

	if(nVP>iVP)	nVP=iVP;

	nPts = nVP;

	//对pLidVP按连接点ID排序
	qsort(pLidVP, nVP, sizeof(LidMC_VP), LidMC_VP_ascending_tieID);

	//对连接点进行检查
	int TieHorNum=0, /*TieHorVerNum=0, */TieVerNum=0;
	int tiePtNum=0;
	for(i=0; i<nVP; )
	{
		int sPos, ePos;
		tieID=pLidVP[i].tieID;
		char vp_type=pLidVP[i].VP_type;
		bool bUpdate=false;

		tiePtNum++;
		sPos=i;
		ePos=i;

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(pLidVP[i].VP_type!=vp_type)
			{
				bUpdate=true;
			}
			ePos=i;

			i++;
		}

		if(bUpdate)
		{
			vp_type=TP_type_horizon;
			for(j=sPos; j<=ePos; j++)
			{
				pLidVP[j].VP_type=vp_type;
			}
		}

		if(vp_type == TP_type_hor_ver)
		{
			TieHorNum++;
			TieVerNum++;
		}
		else if(vp_type == TP_type_horizon)	
			TieHorNum++;
		else if(vp_type == TP_type_vertical)	
			TieVerNum++;

	}

	TRACE("total tie point num: %d\n", tiePtNum);
	//	TRACE("horizon and vertical tie point num: %d\n", TieHorVerNum);
	TRACE("horizon tie point num: %d\n", TieHorNum);
	TRACE("vertical tie point num: %d\n", TieVerNum);

	//为控制点赋值
	for(i=0; i<ngcp; i++)
	{
		j=pLidVP[i].tieID;

		//		if(i<ngcp)
		//		{//控制点必须排在最开始
		pTieChain=pTieList->GetAt(j);
		ASSERT(pTieChain->bGCP);

		pLidVP[i].eX=pTieChain->gcp.X;
		pLidVP[i].eY=pTieChain->gcp.Y;
		pLidVP[i].eZ=pTieChain->gcp.Z;
		//		}
	}

	//调整连接点ID,连续排列 (漏洞点删除后占用的ID)
	for(i=0, j=0; i<nVP; )
	{
		tieID=pLidVP[i].tieID;
		if(tieID!=j)
		{	
			pLidVP[i].tieID=j;	
		}

		i++;
		while(pLidVP[i].tieID==tieID)
		{
			if(tieID!=j)
			{	
				pLidVP[i].tieID=j;	
			}
			i++;
		}
		j++;
	}

	if(pTieLut)		delete[] pTieLut;		pTieLut=NULL;
	return true;
}

//将观测值（同名对应）提取到独立文件中，方便后续平差调用
void CMainFrame::OnLidarcalibrationExtractobsvs()
{
	TieObj_Type objType;
	if(m_AlgPrj.GetTieObjList()->GetSize()>0)
	{
		objType = m_AlgPrj.GetTieObjList()->GetAt(0)->type;
	}

	LidMC_Plane *pLidPlanes=NULL;
	LidMC_VP    *pLidVPs=NULL;
	LidMC_Obj   *pLidObjs=NULL;
	int nObjNum;

	if(objType == TO_POINT)
	{
		Extract_ConjugatePoints(&pLidVPs, nObjNum);

		pLidObjs = (LidMC_Obj*)(pLidVPs);
	}
	else if(objType == TO_PATCH)
	{
		Extract_CorrespondingPlanes(&pLidPlanes, nObjNum);
		//ransac筛选出共面点
		LidPlane_ransac(pLidPlanes, nObjNum);
		pLidObjs = (LidMC_Obj*)(pLidPlanes);

	}
	else if(objType == TO_LINE)
	{

	}

	CFileDialog  dlg(FALSE,"Corr",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);	

	if(dlg.DoModal()!=IDOK)
		return;

	CString strPath=dlg.GetPathName();

	write_CorrespFile((LPCTSTR)strPath, nObjNum, pLidObjs);

//	if(pTieLut)		delete[] pTieLut;		pTieLut=NULL;
	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
	if(pLidVPs)		delete[] pLidVPs;		pLidVPs=NULL;
}


#include "dl_dxf.h"
#include "CalibUtility/Dxf_IO.h"
//从手工选定的dxf文件中确定对应多边形，根据文件名确定对应关系
void CMainFrame::OnLidarmatchExtractcorresppolylines()
{
	CArray<TieObjChain*, TieObjChain*> *pTieList=NULL;
	TieObjChain *pCurChain=NULL;

	m_AlgPrj.ReleaseTieList(); //清空连接面列表
	pTieList=m_AlgPrj.GetTieObjList();

	CString strFolder;
	if(!_SelectFolderDialog("同名多边形文件目录...", &strFolder, NULL))
		return;

	CFileFind	fileFind;
	CString	strTmp, strFileName;
	BOOL	bIsEnd;

	CDxfImporter creationClass;
	DL_Dxf dxf;
	

	strTmp.Format("%s\\*.dxf",strFolder);
	bIsEnd=fileFind.FindFile(strTmp.GetBuffer(0));
	while(bIsEnd)
	{
		bIsEnd=fileFind.FindNextFile();
		if(!fileFind.IsDots())
		{
			strFileName=fileFind.GetFilePath();
			if(!fileFind.IsDirectory())
			{//是文件,读取dxf信息
				//验证文件名是否为合法
				CString purefileName = _ExtractFileName(strFileName);

				if(purefileName.Find( "poly_" ) == -1)
					continue;

				CString  strTmp1, strTmp2;
				int sID=0, tID=0;

				int pos;
				pos = purefileName.Find('s');
				strTmp1=purefileName.Right(pos+1);
				pos = strTmp1.Find('_');
				strTmp2 = strTmp1.Left(pos);
				sID = atoi(strTmp2.GetBuffer(0));

				int length = purefileName.GetLength();
				pos = purefileName.ReverseFind('t');
				strTmp1 = purefileName.Right(length-pos-1);
				tID = atoi(strTmp1.GetBuffer(0));

				if(sID==0 || tID==0)
				{//文件名错误
					assert(false);
					continue;
				}

				creationClass.clean();
				if (!dxf.in(strFileName.GetBuffer(0), &creationClass)) 
				{ // if file open failed
					assert(false);
					continue;
				}

				if(tID <= pTieList->GetSize())
				{//存在chain
					pCurChain = pTieList->GetAt(tID-1);
				}
				else
				{//创建新chain
					pCurChain = new TieObjChain;
					pCurChain->TieID=tID;
					pCurChain->type=TO_PATCH;
					pCurChain->bDel=false;
					pCurChain->pChain=new CArray<TieObject*, TieObject*>; //无连接点
					pCurChain->objNum=0;
					pTieList->Add(pCurChain);
				}
				TiePatch *pCurObj = new TiePatch;
			
				pCurObj->sourceID=sID;
				pCurObj->sourceType=ST_LiDAR;
				pCurObj->objType=TO_PATCH;

				pCurObj->ptNum=creationClass.m_polyVertices->size();
				pCurObj->pt2D = new orsPOINT2D[pCurObj->ptNum];;
				pCurObj->pt3D = new orsPOINT3D[pCurObj->ptNum];

				for(int i=0; i<pCurObj->ptNum; i++)
				{
					pCurObj->pt3D[i] = (*(creationClass.m_polyVertices))[i];
				}

				pCurChain->pChain->Add((TieObject*)pCurObj);
				pCurChain->objNum++;
			}
		}
	}

	fileFind.Close();

}

#include "CalibUtility/cp_IO.h"

void CMainFrame::OnLidarcorrectionRigorousmodel()
{
	OnPreproccessAtn2las();
}


#include "DlgLidCorrectYuan.h"
void CMainFrame::OnLidarcorrectionYuanmodel()
{
	int i, j;
	CDlgLidCorrectYuan dlg;
	CString lasName;

	if(dlg.DoModal()!=IDOK)
		return;

	LidCalib_ModelType type;
	void *CalibParam=NULL; 
	int num;

	if(!read_CPfile(dlg.m_strCalibFile, type, &CalibParam, num))
		return;

	ref_ptr<orsIProcessMsgBar> process;
	process = ORS_CREATE_OBJECT( orsIProcessMsgBar, "ors.process.bar.mfc");
	process->InitProgressBar("点云纠正...", "wait...", dlg.m_lasNameVec.GetSize());

	ref_ptr<orsIPointCloudWriter> laswriter = ORS_CREATE_OBJECT( orsIPointCloudWriter, 
		ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );

	if(type == LCMT_Rigorous)
	{

	}
	else if(type == LCMT_Ressl)
	{

	}
	else if(type == LCMT_TrajAided)
	{

	}
	else if(type == LCMT_Yuan)
	{
		for(i=0; i<dlg.m_lasNameVec.GetSize(); i++)
		{
			lasName = dlg.m_lasNameVec[i];

			orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead(lasName);
			if(!pReader)
				continue;

			CString pureName = _ExtractFileFullName(lasName);
			lasName = dlg.m_strOutputDir + "\\" + pureName;

			if(!laswriter->open(lasName, lasPOINT_GPS))
				continue;

			double coord[3];
			std::vector<orsPOINT3D>  strip_edges;
			std::vector<double>  cent_line;
			std::vector<double>  line_param;
			orsPOINT3D  center;
			center.X = center.Y = center.Z = 0;

			pReader->read_point(coord);
			bool scan_direction_flag = pReader->get_scan_direction_flag();

			while(pReader->read_point(coord))
			{

				//首先找到同一扫描线的两个边缘点，然后计算中心点坐标
				if(scan_direction_flag==pReader->get_scan_direction_flag())
					continue;

				center.X += coord[0];
				center.Y += coord[1];
				center.Z += coord[2];

				scan_direction_flag = pReader->get_scan_direction_flag();
				orsPOINT3D pt;
				pt.X = coord[0];
				pt.Y = coord[1];
				pt.Z = coord[2];
				strip_edges.push_back(pt);			
			}
			int ptNum = strip_edges.size();
			center.X /= ptNum;
			center.Y /= ptNum;
			center.Z /= ptNum;

			orsPOINT2D pt2D;
			//默认边缘点成对出现
			for(j=0; j<strip_edges.size(); j++)
			{
				if(j%2==0)
				{
					pt2D.x=strip_edges[j].X;
					pt2D.y=strip_edges[j].Y;
				}
				else
				{
					pt2D.x += strip_edges[j].X;
					pt2D.y += strip_edges[j].Y;

					pt2D.x /= 2;
					pt2D.y /= 2;

					cent_line.push_back(pt2D.x);
					cent_line.push_back(pt2D.y);
				}

			}
			double line_grad, line_intercept;
			int cent_line_ptNum = cent_line.size()/2;
			line_fitting(cent_line_ptNum, &cent_line[0], &line_grad, &line_intercept);
			//确定直线的方向
			double x1, y1, x2, y2;
			double theta = atan(line_grad);//直线方向角
			if(line_grad>1.0)
			{//slope > 45deg
				x1 = cent_line[0];
				x2 = cent_line[2*(cent_line_ptNum-1)];
				y1 = line_grad*x1+line_intercept;
				y2 = line_grad*x2+line_intercept;

				if((x2-x1)*line_grad < 0)
				{
					theta += PI;
				}
			}
			else
			{// slope < 45 deg
				y1 = cent_line[1];
				y2 = cent_line[2*cent_line_ptNum-1];
				x1 = (y1-line_intercept)/line_grad;
				x2 = (y2-line_intercept)/line_grad;

				if((y2-y1)*line_grad < 0)
				{
					theta += PI;
				}
			}

			orsLidYuanParam *pOriParam = (orsLidYuanParam*)CalibParam;

			pOriParam->stripCenter = center;
			getMatrixService()->RotateMatrix_heading(/*line_param[1]/line_param[0]*/-theta, pOriParam->rotMatZ);
			
			orsILidarGeoModel *pLidarModel=ORS_CREATE_OBJECT(orsILidarGeoModel, ORS_LIDAR_GEOMETRYMODEL_YUAN);
			pLidarModel->setSysParam((void*)(pOriParam));
			
			pReader->reopen();
			while (pReader->read_point(coord))
			{
				orsPOINT3D pt_coord, dst_coord;
				pt_coord.X = coord[0]; pt_coord.Y = coord[1]; pt_coord.Z = coord[2];
				pLidarModel->cal_Laserfootprint(&pt_coord, &dst_coord, 1);
				coord[0] = dst_coord.X;
				coord[1] = dst_coord.Y;
				coord[2] = dst_coord.Z;

				unsigned short intensity = pReader->get_intensity();
				unsigned char return_number = pReader->get_return_number();
				unsigned char number_of_returns_of_given_pulse = pReader->get_number_of_returns();
				unsigned char classification = pReader->get_classification();
				double gpstime = pReader->get_gpstime();

				unsigned char scan_direction_flag = pReader->get_scan_direction_flag();
				unsigned char edge_of_flight_line = pReader->get_edge_of_flight_line();
				char scan_angle_rank = pReader->get_scan_angle_rank();
				unsigned char user_data = pReader->get_user_data();
				unsigned short point_source_ID = pReader->get_point_source_ID();

				laswriter->write_point(coord, intensity, return_number, 
					number_of_returns_of_given_pulse, scan_direction_flag, edge_of_flight_line, classification, scan_angle_rank, user_data, gpstime);
			}

			pReader->close();
			laswriter->close();
			process->SetPos(i+1);
		}
	}
}


void CMainFrame::OnLidarcalibrationVcp()
{
	CFileDialog  dlg(TRUE,"Corr",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	int nPatch;
	int objType;
	LidMC_VP *pLidObjs=NULL;

	read_CorrespFile((LPCTSTR)strFileName, &objType, &nPatch, (LidMC_Obj**)&pLidObjs);

	orsLidSysParam oriParam;
	memset(&oriParam, 0, sizeof(orsLidSysParam));

	double pX[8];	//线元素，角元素
	memset(pX, 0, sizeof(double)*8);

	//	pX[3]=/*-0.00576609*/0;	pX[4]=/*0.00258278*/0;	pX[5]=/*-0.00138682*/0;

	CFileDialog  outputdlg(FALSE," ",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	
	outputdlg.m_ofn.lpstrTitle="保存平差解算参数";

	if(outputdlg.DoModal()!=IDOK)
		goto FUNC_END;

	{
		CString  strOutput=outputdlg.GetPathName();

		CalibParam_Type ctype;
		//	ctype=Calib_rph_sa0;
		//	ctype=Calib_rph;
		//	ctype=Calib_rph_drange;
		//	ctype=Calib_rph_drange_sa0;
		ctype=Calib_rph;


		pX[0] = 0;
		pX[1] = 0;
		pX[2] = 0;
		pX[3] = 0;
		pX[4] = 0;
		pX[5] = 0;

		//		_calLaserPosition(&surv_Vec[0],nVP*3, &oriParam, pX, ctype);
		//		_calLaserPosition(&ATN_Vec[0],nVP*3, oriParam, pX, Calib_rph);
		//	ctype=Calib_rphxyz_sa0;
		//	ctype=Calib_rphxyz_sa0_sac;

		//有线性系统改正
		//	LidMC_Misalign_VP_Triangle_xyzrph_semiXYZ(nPatch, ngcp, pLidPatch, pX, 6);
		//无系统改正
		
		CLidCalib_VCP calib;
		LidCalib_SetPlatform(getPlatform());
		calib.LidMC_Misalign_VCP(strOutput.GetBuffer(128), nPatch, 0, pLidObjs, LidAdj_TP_UseHor,
			pX, oriParam, ctype);
	}

FUNC_END:
	if(pLidObjs)	delete[] pLidObjs;	pLidObjs=NULL;
// 	if(pLidPlanes)	delete[] pLidPlanes;	pLidPlanes=NULL;
// 	if(pTieLut)  delete pTieLut;  pTieLut=NULL;
	//	if(pATNPoints)	delete pATNPoints;	pATNPoints=NULL;
}


void CMainFrame::OnConjugatepointsNotrajYuan()
{
	CFileDialog  dlg(TRUE,"Corr",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	int nPatch;
	int objType;
	LidMC_VP *pLidObjs=NULL;

	read_CorrespFile((LPCTSTR)strFileName, &objType, &nPatch, (LidMC_Obj**)&pLidObjs);

#if 0
	LidMC_Plane *pLidPlanesPart=NULL;
	pLidPlanesPart = new LidMC_Plane[nPatch];

	int n_Part=0;
	for(int i=0; i<nPatch; i++)
	{
		if((*(pLidPlanes[i].pLidPoints))[0].stripID<2)
			continue;

		pLidPlanesPart[n_Part].s1 = pLidPlanes[i].s1;
		pLidPlanesPart[n_Part].s2 = pLidPlanes[i].s2;
		pLidPlanesPart[n_Part].s3 = pLidPlanes[i].s3;
		pLidPlanesPart[n_Part].s4 = pLidPlanes[i].s4;
		pLidPlanesPart[n_Part].tieID = pLidPlanes[i].tieID;
		pLidPlanesPart[n_Part].pLidPoints = new std::vector<LidPt_SurvInfo>;
		*(pLidPlanesPart[n_Part].pLidPoints) = *(pLidPlanes[i].pLidPoints);
		n_Part++;
	}


#endif


#if 0
	//测试提取的点云是否正确
	ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	char pFileName[128];
	for(int i=0; i<nPatch; i++)
	{
		int sID = (*(pLidPlanes[i].pLidPoints))[0].stripID;
		sprintf(pFileName, "H:\\temp\\%d_%d.las", sID, pLidPlanes[i].tieID);
		writer->open(pFileName, lasPOINT);

		for(int j=0; j<pLidPlanes[i].pLidPoints->size(); j++)
		{
			double coord[3];
			coord[0] = (*(pLidPlanes[i].pLidPoints))[j].x;
			coord[1] = (*(pLidPlanes[i].pLidPoints))[j].y;
			coord[2] = (*(pLidPlanes[i].pLidPoints))[j].z;
			writer->write_point(coord);
		}

		writer->close();
	}
#endif


	orsLidYuanParam *pOriParam=NULL;


	/////////////////////////////////////////////////////
	//准备每条带的旋转矩阵和参考中心点
	////////////////////////////////////////////////////
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	Align_LidLine lidLine;

	int i, j, k;
	int nLine;


	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();

	pOriParam = new orsLidYuanParam[nLine];   //每条带参数
	memset(pOriParam, 0, sizeof(orsLidYuanParam)*nLine);

	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);
		

		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead(lidLine.LasName);
		//		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead("H:\\temp\\LDR101108_011429_ATN_TP.LAS");
		assert(pReader);

		double coord[3];
		std::vector<orsPOINT3D>  strip_edges;
		//		std::vector<orsPOINT2D>  cent_line;
		std::vector<double>  cent_line;
		std::vector<double>  line_param;

		pReader->read_point(coord);
		bool scan_direction_flag = pReader->get_scan_direction_flag();
		// 		center.X = coord[0];
		// 		center.Y = coord[1];
		// 		center.Z = coord[2];

		while(pReader->read_point(coord))
		{


			// 			if(pReader->get_edge_of_flight_line())
			// 			{
			// 				int eee=1;
			// 			}

			//首先找到同一扫描线的两个边缘点，然后计算中心点坐标
			if(scan_direction_flag==pReader->get_scan_direction_flag())
				continue;

// 			center.X += coord[0];
// 			center.Y += coord[1];
// 			center.Z += coord[2];

			scan_direction_flag = pReader->get_scan_direction_flag();
			orsPOINT3D pt;
			pt.X = coord[0];
			pt.Y = coord[1];
			pt.Z = coord[2];
			strip_edges.push_back(pt);			
		}
		pReader->close();

		//中心线的中心（飞行轨迹中心）
		orsPOINT3D  center;
		center.X = center.Y = center.Z = 0;

		orsPOINT2D pt2D;
		//默认边缘点成对出现
		for(j=0; j<strip_edges.size(); j++)
		{
			if(j%2==0)
			{
				pt2D.x=strip_edges[j].X;
				pt2D.y=strip_edges[j].Y;
			}
			else
			{
				pt2D.x += strip_edges[j].X;
				pt2D.y += strip_edges[j].Y;

				pt2D.x /= 2;
				pt2D.y /= 2;

				cent_line.push_back(pt2D.x);
				cent_line.push_back(pt2D.y);
				center.X += pt2D.x;
				center.Y += pt2D.y;
			}

		}


#if 0
		//测试提取的点云中心线是否正确
		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_centerline.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<cent_line.size(); j++)
		{
			double coord[3];
			coord[0] = cent_line[j].x;
			coord[1] = cent_line[j].y;
			coord[2] = 0;
			writer->write_point(coord);
		}

		writer->close();

		sprintf(pFileName, "H:\\temp\\%d_stripEdges.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<strip_edges.size(); j++)
		{
			double coord[3];
			coord[0] = strip_edges[j].X;
			coord[1] = strip_edges[j].Y;
			coord[2] = strip_edges[j].Z;
			writer->write_point(coord);
		}

		writer->close();
#endif
		//		line2DFitting(cent_line, line_param);   //line_param: nx, ny, ax, ay

		double line_grad, line_intercept;
		int cent_line_ptNum = cent_line.size()/2;
		center.X /= cent_line_ptNum;
		center.Y /= cent_line_ptNum;

		line_fitting(cent_line_ptNum, &cent_line[0], &line_grad, &line_intercept);
		//确定直线的方向
		double x1, y1, x2, y2;
		double theta = atan(line_grad);//直线方向角
		if(line_grad>1.0)
		{//slope > 45deg
			x1 = cent_line[0];
			x2 = cent_line[2*(cent_line_ptNum-1)];
			y1 = line_grad*x1+line_intercept;
			y2 = line_grad*x2+line_intercept;

			if((x2-x1)*line_grad < 0)
			{
				theta += PI;
			}
		}
		else
		{// slope < 45 deg
			y1 = cent_line[1];
			y2 = cent_line[2*cent_line_ptNum-1];
			x1 = (y1-line_intercept)/line_grad;
			x2 = (y2-line_intercept)/line_grad;

			if((y2-y1)*line_grad < 0)
			{
				theta += PI;
			}
		}


		pOriParam[i].stripCenter = center;
		pOriParam[i].stripCenter.Z = lidLine.averFH;

		//		if(fabs(line_param[0])>1e-6)
		//顺时针旋转，角度为负
		getMatrixService()->RotateMatrix_heading(/*line_param[1]/line_param[0]*/-theta, pOriParam[i].rotMatZ);

#if 0
		//测试点云改正后是否正确

		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_rotated.las", i);
		writer->open(pFileName, lasPOINT);

		pReader->reopen();
		while(pReader->read_point(coord))
		{
			coord[0] -= center.X;
			coord[1] -= center.Y;
			coord[2] -= center.Z;
			double tmp[3];
			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
			//			memcpy(tmp, coord, sizeof(double)*3);

			unsigned short intensity = pReader->get_intensity();
			unsigned char return_number = pReader->get_return_number();
			unsigned char number_of_returns_of_given_pulse = pReader->get_number_of_returns();
			unsigned char classification = pReader->get_classification();
			double gpstime = pReader->get_gpstime();

			unsigned char scan_direction_flag = pReader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = pReader->get_edge_of_flight_line();
			char scan_angle_rank = pReader->get_scan_angle_rank();
			unsigned char user_data = pReader->get_user_data();
			unsigned short point_source_ID = pReader->get_point_source_ID();

			//			writer->write_point(tmp);
			writer->write_point(tmp, intensity, return_number, 
				number_of_returns_of_given_pulse, scan_direction_flag, 
				edge_of_flight_line, classification, scan_angle_rank, user_data, gpstime);
		}

		// 		for(j=0; j<cent_line.size()/2; j++)
		// 		{
		// 			double coord[3];
		// 			coord[0] = cent_line[2*j] - center.X;
		// 			coord[1] = cent_line[2*j+1] - center.Y;
		// 			coord[2] = 0;
		// 
		// 			double tmp[3];
		// 			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
		// 			writer->write_point(tmp);
		// 		}

		writer->close();
#endif

	}

	//未知数求解


	double pX[5];
	int nCalib;
	memset(pX, 0, sizeof(double)*5);
	//用初值重新计算点集
//	pX[3] = 0.000374028;  //heading
//	pX[4] = -0.004084307; //roll

	CFileDialog  outdlg(FALSE,"检校结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput = outdlg.GetPathName();		//导出检校结果


	CLidCalib_Yuan calib;
	LidCalib_SetPlatform(getPlatform());
	calib.LidMC_Calib_ConjugatePoints(strOutput.GetBuffer(128), nPatch/*n_Part*/, 0, pLidObjs/*pLidPlanesPart*/, 
		LidAdj_TP_UseHor, pX, pOriParam, nLine);


	if(pLidObjs)	delete[] pLidObjs;		pLidObjs=NULL;
	if(pOriParam)	delete[] pOriParam;		pOriParam=NULL;
}




void CMainFrame::OnConjugatepointsNotrajJing()
{
	CFileDialog  dlg(TRUE,"Corr",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	int nPatch;
	int objType;
	LidMC_VP *pLidObjs=NULL;

	read_CorrespFile((LPCTSTR)strFileName, &objType, &nPatch, (LidMC_Obj**)&pLidObjs);

#if 0
	LidMC_Plane *pLidPlanesPart=NULL;
	pLidPlanesPart = new LidMC_Plane[nPatch];

	int n_Part=0;
	for(int i=0; i<nPatch; i++)
	{
		if((*(pLidPlanes[i].pLidPoints))[0].stripID<2)
			continue;

		pLidPlanesPart[n_Part].s1 = pLidPlanes[i].s1;
		pLidPlanesPart[n_Part].s2 = pLidPlanes[i].s2;
		pLidPlanesPart[n_Part].s3 = pLidPlanes[i].s3;
		pLidPlanesPart[n_Part].s4 = pLidPlanes[i].s4;
		pLidPlanesPart[n_Part].tieID = pLidPlanes[i].tieID;
		pLidPlanesPart[n_Part].pLidPoints = new std::vector<LidPt_SurvInfo>;
		*(pLidPlanesPart[n_Part].pLidPoints) = *(pLidPlanes[i].pLidPoints);
		n_Part++;
	}


#endif


#if 0
	//测试提取的点云是否正确
	ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	char pFileName[128];
	for(int i=0; i<nPatch; i++)
	{
		int sID = (*(pLidPlanes[i].pLidPoints))[0].stripID;
		sprintf(pFileName, "H:\\temp\\%d_%d.las", sID, pLidPlanes[i].tieID);
		writer->open(pFileName, lasPOINT);

		for(int j=0; j<pLidPlanes[i].pLidPoints->size(); j++)
		{
			double coord[3];
			coord[0] = (*(pLidPlanes[i].pLidPoints))[j].x;
			coord[1] = (*(pLidPlanes[i].pLidPoints))[j].y;
			coord[2] = (*(pLidPlanes[i].pLidPoints))[j].z;
			writer->write_point(coord);
		}

		writer->close();
	}
#endif


	orsLidJingParam *pOriParam=NULL;


	/////////////////////////////////////////////////////
	//准备每条带的旋转矩阵和参考中心点
	////////////////////////////////////////////////////
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	Align_LidLine lidLine;

	int i, j, k;
	int nLine;


	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();

	pOriParam = new orsLidJingParam[nLine];   //每条带参数
	memset(pOriParam, 0, sizeof(orsLidJingParam)*nLine);

	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);


		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead(lidLine.LasName);
		//		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead("H:\\temp\\LDR101108_011429_ATN_TP.LAS");
		assert(pReader);

		double coord[3];
		std::vector<orsPOINT3D>  strip_edges;
		//		std::vector<orsPOINT2D>  cent_line;
		std::vector<double>  cent_line;
		std::vector<double>  line_param;

		pReader->read_point(coord);
		bool scan_direction_flag = pReader->get_scan_direction_flag();
		// 		center.X = coord[0];
		// 		center.Y = coord[1];
		// 		center.Z = coord[2];

		while(pReader->read_point(coord))
		{


			// 			if(pReader->get_edge_of_flight_line())
			// 			{
			// 				int eee=1;
			// 			}

			//首先找到同一扫描线的两个边缘点，然后计算中心点坐标
			if(scan_direction_flag==pReader->get_scan_direction_flag())
				continue;

			// 			center.X += coord[0];
			// 			center.Y += coord[1];
			// 			center.Z += coord[2];

			scan_direction_flag = pReader->get_scan_direction_flag();
			orsPOINT3D pt;
			pt.X = coord[0];
			pt.Y = coord[1];
			pt.Z = coord[2];
			strip_edges.push_back(pt);			
		}
		pReader->close();

		//中心线的中心（飞行轨迹中心）
		orsPOINT3D  center;
		center.X = center.Y = center.Z = 0;

		orsPOINT2D pt2D;
		//默认边缘点成对出现
		for(j=0; j<strip_edges.size(); j++)
		{
			if(j%2==0)
			{
				pt2D.x=strip_edges[j].X;
				pt2D.y=strip_edges[j].Y;
			}
			else
			{
				pt2D.x += strip_edges[j].X;
				pt2D.y += strip_edges[j].Y;

				pt2D.x /= 2;
				pt2D.y /= 2;

				cent_line.push_back(pt2D.x);
				cent_line.push_back(pt2D.y);
				center.X += pt2D.x;
				center.Y += pt2D.y;
			}

		}


#if 0
		//测试提取的点云中心线是否正确
		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_centerline.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<cent_line.size(); j++)
		{
			double coord[3];
			coord[0] = cent_line[j].x;
			coord[1] = cent_line[j].y;
			coord[2] = 0;
			writer->write_point(coord);
		}

		writer->close();

		sprintf(pFileName, "H:\\temp\\%d_stripEdges.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<strip_edges.size(); j++)
		{
			double coord[3];
			coord[0] = strip_edges[j].X;
			coord[1] = strip_edges[j].Y;
			coord[2] = strip_edges[j].Z;
			writer->write_point(coord);
		}

		writer->close();
#endif
		//		line2DFitting(cent_line, line_param);   //line_param: nx, ny, ax, ay

		double line_grad, line_intercept;
		int cent_line_ptNum = cent_line.size()/2;
		center.X /= cent_line_ptNum;
		center.Y /= cent_line_ptNum;

		line_fitting(cent_line_ptNum, &cent_line[0], &line_grad, &line_intercept);
		//确定直线的方向
		double x1, y1, x2, y2;
		double theta = atan(line_grad);//直线方向角
		if(line_grad>1.0)
		{//slope > 45deg
			x1 = cent_line[0];
			x2 = cent_line[2*(cent_line_ptNum-1)];
			y1 = line_grad*x1+line_intercept;
			y2 = line_grad*x2+line_intercept;

			if((x2-x1)*line_grad < 0)
			{
				theta += PI;
			}
		}
		else
		{// slope < 45 deg
			y1 = cent_line[1];
			y2 = cent_line[2*cent_line_ptNum-1];
			x1 = (y1-line_intercept)/line_grad;
			x2 = (y2-line_intercept)/line_grad;

			if((y2-y1)*line_grad < 0)
			{
				theta += PI;
			}
		}


		pOriParam[i].stripCenter = center;
		pOriParam[i].stripCenter.Z = lidLine.averFH;

		//		if(fabs(line_param[0])>1e-6)
		//顺时针旋转，角度为负
		getMatrixService()->RotateMatrix_heading(/*line_param[1]/line_param[0]*/-theta, pOriParam[i].rotMatZ);

#if 0
		//测试点云改正后是否正确

		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_rotated.las", i);
		writer->open(pFileName, lasPOINT);

		pReader->reopen();
		while(pReader->read_point(coord))
		{
			coord[0] -= center.X;
			coord[1] -= center.Y;
			coord[2] -= center.Z;
			double tmp[3];
			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
			//			memcpy(tmp, coord, sizeof(double)*3);

			unsigned short intensity = pReader->get_intensity();
			unsigned char return_number = pReader->get_return_number();
			unsigned char number_of_returns_of_given_pulse = pReader->get_number_of_returns();
			unsigned char classification = pReader->get_classification();
			double gpstime = pReader->get_gpstime();

			unsigned char scan_direction_flag = pReader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = pReader->get_edge_of_flight_line();
			char scan_angle_rank = pReader->get_scan_angle_rank();
			unsigned char user_data = pReader->get_user_data();
			unsigned short point_source_ID = pReader->get_point_source_ID();

			//			writer->write_point(tmp);
			writer->write_point(tmp, intensity, return_number, 
				number_of_returns_of_given_pulse, scan_direction_flag, 
				edge_of_flight_line, classification, scan_angle_rank, user_data, gpstime);
		}

		// 		for(j=0; j<cent_line.size()/2; j++)
		// 		{
		// 			double coord[3];
		// 			coord[0] = cent_line[2*j] - center.X;
		// 			coord[1] = cent_line[2*j+1] - center.Y;
		// 			coord[2] = 0;
		// 
		// 			double tmp[3];
		// 			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
		// 			writer->write_point(tmp);
		// 		}

		writer->close();
#endif

	}

	//未知数求解


	double pX[5];
	int nCalib;
	memset(pX, 0, sizeof(double)*5);
	//用初值重新计算点集
	//	pX[3] = 0.000374028;  //heading
	//	pX[4] = -0.004084307; //roll

	CFileDialog  outdlg(FALSE,"检校结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput = outdlg.GetPathName();		//导出检校结果


	CLidCalib_Jing calib;
	LidCalib_SetPlatform(getPlatform());
	calib.LidMC_Calib_ConjugatePoints(strOutput.GetBuffer(128), nPatch/*n_Part*/, 0, pLidObjs/*pLidPlanesPart*/, 
		LidAdj_TP_UseHor, pX, pOriParam, nLine);


	if(pLidObjs)	delete[] pLidObjs;		pLidObjs=NULL;
	if(pOriParam)	delete[] pOriParam;		pOriParam=NULL;
}


void CMainFrame::OnConjugatepointsNotrajRessl()
{
	CFileDialog  dlg(TRUE,"Corr",NULL,OFN_HIDEREADONLY|OFN_FILEMUSTEXIST,"观测值文件(*.Corr)|*.Corr|(*.*)|*.*||",NULL);

	if(dlg.DoModal()!=IDOK)
		return;

	CString strFileName = dlg.GetPathName();

	int nPatch;
	int objType;
	LidMC_VP *pLidObjs=NULL;

	read_CorrespFile((LPCTSTR)strFileName, &objType, &nPatch, (LidMC_Obj**)&pLidObjs);

#if 0
	LidMC_Plane *pLidPlanesPart=NULL;
	pLidPlanesPart = new LidMC_Plane[nPatch];

	int n_Part=0;
	for(int i=0; i<nPatch; i++)
	{
		if((*(pLidPlanes[i].pLidPoints))[0].stripID<2)
			continue;

		pLidPlanesPart[n_Part].s1 = pLidPlanes[i].s1;
		pLidPlanesPart[n_Part].s2 = pLidPlanes[i].s2;
		pLidPlanesPart[n_Part].s3 = pLidPlanes[i].s3;
		pLidPlanesPart[n_Part].s4 = pLidPlanes[i].s4;
		pLidPlanesPart[n_Part].tieID = pLidPlanes[i].tieID;
		pLidPlanesPart[n_Part].pLidPoints = new std::vector<LidPt_SurvInfo>;
		*(pLidPlanesPart[n_Part].pLidPoints) = *(pLidPlanes[i].pLidPoints);
		n_Part++;
	}


#endif


#if 0
	//测试提取的点云是否正确
	ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
	char pFileName[128];
	for(int i=0; i<nPatch; i++)
	{
		int sID = (*(pLidPlanes[i].pLidPoints))[0].stripID;
		sprintf(pFileName, "H:\\temp\\%d_%d.las", sID, pLidPlanes[i].tieID);
		writer->open(pFileName, lasPOINT);

		for(int j=0; j<pLidPlanes[i].pLidPoints->size(); j++)
		{
			double coord[3];
			coord[0] = (*(pLidPlanes[i].pLidPoints))[j].x;
			coord[1] = (*(pLidPlanes[i].pLidPoints))[j].y;
			coord[2] = (*(pLidPlanes[i].pLidPoints))[j].z;
			writer->write_point(coord);
		}

		writer->close();
	}
#endif


	orsLidResslParam *pOriParam=NULL;


	/////////////////////////////////////////////////////
	//准备每条带的旋转矩阵和参考中心点
	////////////////////////////////////////////////////
	CArray<Align_LidLine, Align_LidLine> *pLidLines=NULL;
	Align_LidLine lidLine;

	int i, j, k;
	int nLine;


	pLidLines=m_AlgPrj.GetLidList();

	nLine=pLidLines->GetSize();

	pOriParam = new orsLidResslParam[nLine];   //每条带参数
	memset(pOriParam, 0, sizeof(orsLidResslParam)*nLine);

	for(i=0; i<nLine; i++)
	{
		lidLine=pLidLines->GetAt(i);


		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead(lidLine.LasName);
		//		orsIPointCloudReader *pReader = getPointCloudlService()->openPointFileForRead("H:\\temp\\LDR101108_011429_ATN_TP.LAS");
		assert(pReader);

		double coord[3];
		std::vector<orsPOINT3D>  strip_edges;
		//		std::vector<orsPOINT2D>  cent_line;
		std::vector<double>  cent_line;
		std::vector<double>  line_param;

		pReader->read_point(coord);
		bool scan_direction_flag = pReader->get_scan_direction_flag();
		// 		center.X = coord[0];
		// 		center.Y = coord[1];
		// 		center.Z = coord[2];

		while(pReader->read_point(coord))
		{


			// 			if(pReader->get_edge_of_flight_line())
			// 			{
			// 				int eee=1;
			// 			}

			//首先找到同一扫描线的两个边缘点，然后计算中心点坐标
			if(scan_direction_flag==pReader->get_scan_direction_flag())
				continue;

			// 			center.X += coord[0];
			// 			center.Y += coord[1];
			// 			center.Z += coord[2];

			scan_direction_flag = pReader->get_scan_direction_flag();
			orsPOINT3D pt;
			pt.X = coord[0];
			pt.Y = coord[1];
			pt.Z = coord[2];
			strip_edges.push_back(pt);			
		}
		pReader->close();

		//中心线的中心（飞行轨迹中心）
		orsPOINT3D  center;
		center.X = center.Y = center.Z = 0;

		orsPOINT2D pt2D;
		//默认边缘点成对出现
		for(j=0; j<strip_edges.size(); j++)
		{
			if(j%2==0)
			{
				pt2D.x=strip_edges[j].X;
				pt2D.y=strip_edges[j].Y;
			}
			else
			{
				pt2D.x += strip_edges[j].X;
				pt2D.y += strip_edges[j].Y;

				pt2D.x /= 2;
				pt2D.y /= 2;

				cent_line.push_back(pt2D.x);
				cent_line.push_back(pt2D.y);
				center.X += pt2D.x;
				center.Y += pt2D.y;
			}

		}


#if 0
		//测试提取的点云中心线是否正确
		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_centerline.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<cent_line.size(); j++)
		{
			double coord[3];
			coord[0] = cent_line[j].x;
			coord[1] = cent_line[j].y;
			coord[2] = 0;
			writer->write_point(coord);
		}

		writer->close();

		sprintf(pFileName, "H:\\temp\\%d_stripEdges.las", i);
		writer->open(pFileName, lasPOINT);

		for(j=0; j<strip_edges.size(); j++)
		{
			double coord[3];
			coord[0] = strip_edges[j].X;
			coord[1] = strip_edges[j].Y;
			coord[2] = strip_edges[j].Z;
			writer->write_point(coord);
		}

		writer->close();
#endif
		//		line2DFitting(cent_line, line_param);   //line_param: nx, ny, ax, ay

		double line_grad, line_intercept;
		int cent_line_ptNum = cent_line.size()/2;
		center.X /= cent_line_ptNum;
		center.Y /= cent_line_ptNum;

		line_fitting(cent_line_ptNum, &cent_line[0], &line_grad, &line_intercept);
		//确定直线的方向
		double x1, y1, x2, y2;
		double theta = atan(line_grad);//直线方向角
		if(line_grad>1.0)
		{//slope > 45deg
			x1 = cent_line[0];
			x2 = cent_line[2*(cent_line_ptNum-1)];
			y1 = line_grad*x1+line_intercept;
			y2 = line_grad*x2+line_intercept;

			if((x2-x1)*line_grad < 0)
			{
				theta += PI;
			}
		}
		else
		{// slope < 45 deg
			y1 = cent_line[1];
			y2 = cent_line[2*cent_line_ptNum-1];
			x1 = (y1-line_intercept)/line_grad;
			x2 = (y2-line_intercept)/line_grad;

			if((y2-y1)*line_grad < 0)
			{
				theta += PI;
			}
		}


		pOriParam[i].stripCenter = center;
		pOriParam[i].stripCenter.Z = lidLine.averFH;

		//		if(fabs(line_param[0])>1e-6)
		//顺时针旋转，角度为负
		getMatrixService()->RotateMatrix_heading(/*line_param[1]/line_param[0]*/-theta, pOriParam[i].rotMatZ);

#if 0
		//测试点云改正后是否正确

		ref_ptr<orsIPointCloudWriter> writer = ORS_CREATE_OBJECT( orsIPointCloudWriter, ORS_INTERFACE_POINTCLOUDSOURCE_WRITER_LAS );
		char pFileName[128];

		sprintf(pFileName, "H:\\temp\\%d_rotated.las", i);
		writer->open(pFileName, lasPOINT);

		pReader->reopen();
		while(pReader->read_point(coord))
		{
			coord[0] -= center.X;
			coord[1] -= center.Y;
			coord[2] -= center.Z;
			double tmp[3];
			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
			//			memcpy(tmp, coord, sizeof(double)*3);

			unsigned short intensity = pReader->get_intensity();
			unsigned char return_number = pReader->get_return_number();
			unsigned char number_of_returns_of_given_pulse = pReader->get_number_of_returns();
			unsigned char classification = pReader->get_classification();
			double gpstime = pReader->get_gpstime();

			unsigned char scan_direction_flag = pReader->get_scan_direction_flag();
			unsigned char edge_of_flight_line = pReader->get_edge_of_flight_line();
			char scan_angle_rank = pReader->get_scan_angle_rank();
			unsigned char user_data = pReader->get_user_data();
			unsigned short point_source_ID = pReader->get_point_source_ID();

			//			writer->write_point(tmp);
			writer->write_point(tmp, intensity, return_number, 
				number_of_returns_of_given_pulse, scan_direction_flag, 
				edge_of_flight_line, classification, scan_angle_rank, user_data, gpstime);
		}

		// 		for(j=0; j<cent_line.size()/2; j++)
		// 		{
		// 			double coord[3];
		// 			coord[0] = cent_line[2*j] - center.X;
		// 			coord[1] = cent_line[2*j+1] - center.Y;
		// 			coord[2] = 0;
		// 
		// 			double tmp[3];
		// 			matrix_product(3,3,3,1,pOriParam[i].rotMatZ, coord, tmp);
		// 			writer->write_point(tmp);
		// 		}

		writer->close();
#endif

	}

	//未知数求解


	double pX[5];
	int nCalib;
	memset(pX, 0, sizeof(double)*5);
	//用初值重新计算点集
	//	pX[3] = 0.000374028;  //heading
	//	pX[4] = -0.004084307; //roll

	CFileDialog  outdlg(FALSE,"检校结果",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"txt文件(*.txt)|*.txt|(*.*)|*.*||",NULL);	

	if(outdlg.DoModal()!=IDOK)
		return;
	CString strOutput = outdlg.GetPathName();		//导出检校结果


	CLidCalib_Ressl calib;
	LidCalib_SetPlatform(getPlatform());
	calib.LidMC_Calib_ConjugatePoints(strOutput.GetBuffer(128), nPatch/*n_Part*/, 0, pLidObjs/*pLidPlanesPart*/, 
		LidAdj_TP_UseHor, pX, pOriParam, nLine);


	if(pLidObjs)	delete[] pLidObjs;		pLidObjs=NULL;
	if(pOriParam)	delete[] pOriParam;		pOriParam=NULL;
}


//Zhang's method
void CMainFrame::OnCoplanarTraj()
{
	// TODO: Add your command handler code here
}