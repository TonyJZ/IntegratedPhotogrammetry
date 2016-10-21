// IPLR.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "IPLR.h"
#include "MainFrm.h"
#include "IPLRView.h"
#include "orsBase\orsUtil.h"
#include "..\BlockSetting\BlockSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPLRApp

BEGIN_MESSAGE_MAP(CIPLRApp, CWinApp)
	//{{AFX_MSG_MAP(CIPLRApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_Prj_NEW, OnPrjNEW)
	ON_COMMAND(ID_Prj_OPEN, OnPrjOPEN)
	ON_COMMAND(ID_Prj_Close, OnPrjClose)
	ON_COMMAND(ID_Prj_SAVE, OnPrjSAVE)
	ON_COMMAND(ID_Prj_SAVE_AS, OnPrjSAVEAS)
	ON_UPDATE_COMMAND_UI(ID_SIFT_KeyPoint, OnUpdateSIFTKeyPoint)
	ON_COMMAND(ID_ModifyProject, OnModifyProject)
	ON_UPDATE_COMMAND_UI(ID_ModifyProject, OnUpdateModifyProject)
	ON_UPDATE_COMMAND_UI(ID_ExporImgaop, OnUpdateExporImgaop)
	ON_UPDATE_COMMAND_UI(ID_ExportImgiop, OnUpdateExportImgiop)
	ON_UPDATE_COMMAND_UI(ID_Exportblv, OnUpdateExportblv)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPLRApp construction


orsIPlatform *g_pPlatform = NULL;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

CIPLRApp::CIPLRApp()
{
	ors_string errorinfo;
	g_pPlatform = orsInitialize(errorinfo, true );
	
//	FastDspSetPlatform( g_pPlatform );

	m_pBlockManager=NULL;
}

CIPLRApp::~CIPLRApp()
{
	BCGCBProCleanUp();
	
	orsUninitialize();

	if(m_pBlockManager)
	{
		delete m_pBlockManager;
		m_pBlockManager=NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CIPLRApp object

CIPLRApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIPLRApp initialization

BOOL CIPLRApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CString str;
	str.LoadString(IDR_MAINFRAME);

//	CSingleDocTemplate* pDocTemplate;
	m_pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CIPLRDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CIPLRView));
	AddDocTemplate(m_pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CIPLRApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CIPLRApp message handlers
CIPLRDoc *CIPLRApp::GetCurDocument()
{
	CDocument  *pDoc = NULL;
	POSITION	nPos = NULL;
	
	nPos = m_pDocTemplate->GetFirstDocPosition();
	while (NULL != nPos)
	{
		pDoc = m_pDocTemplate->GetNextDoc(nPos);
		if(pDoc->IsKindOf(RUNTIME_CLASS(CIPLRDoc)))
			break;
	}
	if (!pDoc) {
		OnFileNew();
		pDoc = GetTailDocument();
	}
	
	return (CIPLRDoc *)(pDoc);
}

CIPLRDoc *CIPLRApp::GetTailDocument()
{
	CDocument  *pDoc = NULL;
	POSITION	nPos = NULL;
	CString		strTitle = _T("");
	
	nPos = m_pDocTemplate->GetFirstDocPosition();
	if (NULL != nPos)
	{
		pDoc = m_pDocTemplate->GetNextDoc(nPos);
		if (NULL == nPos) {
			return (CIPLRDoc *)(pDoc);
		}
		pDoc = m_pDocTemplate->GetNextDoc(nPos);
	}
	
	return (CIPLRDoc *)(pDoc);
}


void CIPLRApp::SetCurDocTitle()
{
	char	chTitle[128] = "";
	CString	strTmp;
	CIPLRDoc  *pDoc = GetCurDocument();
	
	memset(chTitle, 0, sizeof(char)*128);
	if (NULL == m_pBlockManager) 
	{
		strTmp.LoadString(IDS_NullBlock);
	}
	else
	{
		strTmp = m_pBlockManager->GetAlmName();
	}
	strcpy(chTitle, strTmp.GetBuffer(128));
	pDoc->SetTitle(chTitle);
}

void CIPLRApp::OnPrjNEW() 
{
	CIPLRDoc* pDoc=0;
	CString	strTmp = _T("");


	CString	strOpenFileName;
	CString	strCurFileName;
	
	pDoc=GetCurDocument();
	if(pDoc==0)
	{
		AfxMessageBox("打开文档失败!", MB_ICONEXCLAMATION);
		return;
	}
	//获取工程全路径
//	strOpenFileName=CString(chPrjPath);//"test";//dlg.GetPathName();
	
	CFileDialog	dlg( FALSE, "blk", NULL,OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, _T("Block File (*.blk)|*.blk|All Files(*.*)|*.*||"));

	if(dlg.DoModal()==IDOK)
	{//create project
		strOpenFileName=dlg.GetPathName();

		HINSTANCE oldRcHandle = AfxGetResourceHandle();
		
#ifdef _DEBUG
		AfxSetResourceHandle ( GetModuleHandle("BlockSettingD.dll") );
#else
		AfxSetResourceHandle ( GetModuleHandle("BlockSetting.dll") );
#endif

		if(m_pBlockManager==0)
		{
			m_pBlockManager=new CALMPrjManager;
			ASSERT(m_pBlockManager);
			
		

			m_pBlockManager->CreateALMPrj(strOpenFileName.GetBuffer(0));
			
			CBlockSettingDlg	blockdlg(m_pBlockManager, 0);
			blockdlg.DoModal();
			m_pBlockManager->SaveALMPrjFile();
			SetCurDocTitle();	//设置工程名
		}
		else
		{
			strCurFileName=m_pBlockManager->GetAlmFile();
			if(strCurFileName.CompareNoCase(strOpenFileName)!=0)	//新建的测区名与原来的测区不同名
			{
				CBlockSettingDlg	blockdlg(m_pBlockManager, 0);
				m_pBlockManager->CreateALMPrj(strOpenFileName.GetBuffer(0));
				blockdlg.DoModal();
				m_pBlockManager->SaveALMPrjFile();
				SetCurDocTitle();	//设置工程名
			}
			else
			{//新建测区与当前测区是同一个
				AfxMessageBox("测区已打开!", MB_OK);
				return;
			}
			
		}

		AfxSetResourceHandle ( oldRcHandle );
	}
	else
	{
		return;
	}
	
	//初始化各个窗口
	//todo
	
// 	orsArray <ref_ptr<orsIGuiExtension> > &vExtensions = ((CMainFrame*)m_pMainWnd)->GetExtensions();
// 	
// 	for( int i=0; i< vExtensions.size(); i++ )
// 	{
// 		orsIGuiExtensionALM *pDemoExt = ORS_PTR_CAST( orsIGuiExtensionALM,  vExtensions[i] );
// 		
// 		if( pDemoExt )
// 			pDemoExt->SetALMInfo( m_pALM );
// 	}
	
	
//	pDoc->LoadForPOSView();
	
}

void CIPLRApp::OnPrjOPEN() 
{
	CFileDialog dlg( TRUE, "blk", NULL,OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("Block File (*.blk)|*.blk|All Files(*.*)|*.*||"));

	if(dlg.DoModal()!=IDOK)
		return;

	if(m_pBlockManager==0)
	{
		m_pBlockManager=new CALMPrjManager;
	}

	m_pBlockManager->OpenALMPrjFile(dlg.GetPathName());
	
}

void CIPLRApp::OnPrjClose() 
{
	// TODO: Add your command handler code here
	
}

void CIPLRApp::OnPrjSAVE() 
{
	// TODO: Add your command handler code here
	
}

void CIPLRApp::OnPrjSAVEAS() 
{
	// TODO: Add your command handler code here
	
}

void CIPLRApp::OnUpdateSIFTKeyPoint(CCmdUI* pCmdUI) 
{
	if(m_pBlockManager==NULL || m_pBlockManager->GetImageCount()==0)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
	
}

void CIPLRApp::OnModifyProject() 
{
	HINSTANCE oldRcHandle = AfxGetResourceHandle();
	
#ifdef _DEBUG
	AfxSetResourceHandle ( GetModuleHandle("BlockSettingD.dll") );
#else
	AfxSetResourceHandle ( GetModuleHandle("BlockSetting.dll") );
#endif
	
	
	CBlockSettingDlg	blockdlg(m_pBlockManager, 0);
	if(blockdlg.DoModal()==IDOK)
		m_pBlockManager->SaveALMPrjFile();
		
	AfxSetResourceHandle ( oldRcHandle );
	
}

void CIPLRApp::OnUpdateModifyProject(CCmdUI* pCmdUI) 
{
	if(m_pBlockManager==NULL)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
	
}

void CIPLRApp::OnUpdateExporImgaop(CCmdUI* pCmdUI) 
{
	if(m_pBlockManager==NULL)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
	
}

void CIPLRApp::OnUpdateExportImgiop(CCmdUI* pCmdUI) 
{
	if(m_pBlockManager==NULL)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CIPLRApp::OnUpdateExportblv(CCmdUI* pCmdUI) 
{
	if(m_pBlockManager==NULL)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}
