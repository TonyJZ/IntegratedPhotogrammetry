// RegistViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RegistViewer.h"

#include "MainFrm.h"
#include "RegistViewerDoc.h"
#include "RegistViewerView.h"

#include "orsBase\orsUtil.h"
#include "gui\cmdline.h"
#include "gui\SingleImageView.h"

#include "orsGeometry\orsIGeometryService.h"

#include "orsMap/orsIMapService.h"
#include "gui\orsDispalyApi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerApp

BEGIN_MESSAGE_MAP(CRegistViewerApp, CWinApp)
	//{{AFX_MSG_MAP(CRegistViewerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerApp construction
orsIPlatform *g_pPlatform;

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

ORS_GET_MAP_SERVICE_IMPL();
ORS_GET_IMAGE_SERVICE_IMPL();
ORS_GET_SRS_SERVICE_IMPL();
ORS_GET_GEOMETRY_SERVICE_IMPL();

CRegistViewerApp::CRegistViewerApp()
{
	ors_string errorinfo;
	g_pPlatform = orsInitialize(errorinfo, true );
	
	FastDspSetPlatform( g_pPlatform );

	m_AlgWindow=0;
}

CRegistViewerApp::~CRegistViewerApp()
{
	orsUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRegistViewerApp object

CRegistViewerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerApp initialization

BOOL CRegistViewerApp::InitInstance()
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
	CCommandLine cmdLine;

	CString strFlag;
	CString lImgFilePath, strInstance, strTieType, strSrcType, strSrcID;

	cmdLine.GetFirstParameter( strFlag, lImgFilePath  );

	cmdLine.GetNextParameter( strFlag, strInstance  );

	//	ASSERT( strFlag.FindOneOf( "-HWND" ) > -1 );

	DWORD hWnd=strtoul(strInstance.GetBuffer(256), NULL, 16);

	cmdLine.GetNextParameter( strFlag, strTieType);
	//	ASSERT( strFlag.FindOneOf( "-TO_Type" ) > -1 );
	int type=atoi( strTieType.GetBuffer(128));

	//	CMainFrame* pMainFrm=(CMainFrame*) AfxGetMainWnd();

	m_AlgWindow=(HWND)hWnd;
	m_tieType=(TieObj_Type)type;

	cmdLine.GetNextParameter(strFlag, strSrcType);

	m_sourceType=(Source_Type)atoi( strSrcType.GetBuffer(128));
	

	cmdLine.GetNextParameter(strFlag, strSrcID);
	m_sourceID=atoi( strSrcID.GetBuffer(128));

	cmdLine.GetNextParameter(strFlag, m_strAlgName);

	m_bReproj=false;
	CString strReproj;
	cmdLine.GetNextParameter(strFlag, strReproj);
	m_bReproj = atoi( strReproj.GetBuffer(128));

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRegistViewerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CRegistViewerView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	
//	if( !lImgFilePath.IsEmpty() )
//		AfxGetApp()->OpenDocumentFile( lImgFilePath );

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
void CRegistViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRegistViewerApp message handlers


void CRegistViewerApp::OnFileOpen() 
{
	// TODO: Add your command handler code here
	ASSERT( m_pDocManager != NULL );
	
	CString filterExt = GetImageFilterExt();
	//CString filterExt = "Chart Files (*.xlc)|*.xlc|Worksheet Files (*.xls)|*.xls|Data Files (*.xlc;*.xls)|*.xlc; *.xls|All Files (*.*)|*.*||";
	
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filterExt, NULL);
	
	//CFileDialog dlg1(true,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filterExt, NULL);
	
	dlg.m_ofn.lpstrTitle = "Open an Image";
	
	if( dlg.DoModal() != IDOK ) return ;
	
	
	AfxGetApp()->OpenDocumentFile( dlg.GetPathName() );
}

CString CRegistViewerApp::GetImageFilterExt()
{
	CString filterExt;
	CString allExt1;
	
	char buf[80];	
	//orsImageFormatList l = GetImageFormatList();
	ref_ptr<orsIImageService> m_imageService = 
		ORS_PTR_CAST(orsIImageService, g_pPlatform->getService(ORS_SERVICE_IMAGE) );
	
	assert( NULL != m_imageService.get() );
	
	orsFileFormatList l = m_imageService->getSupportedImageFormats();
	
	
	for( int i=0; i<l.size(); i++ )
	{
		sprintf( buf, "%s(*.%s)|*.%s|", l[i].name.c_str(), l[i].ext.c_str(),l[i].ext.c_str() );
		filterExt += buf;
		if(i != (l.size()-1))
			sprintf(buf,"*.%s;",l[i].ext.c_str());
		else
			sprintf(buf,"*.%s",l[i].ext.c_str());
		allExt1 += buf;
	}
	
	filterExt += "All Supported Files (";
	filterExt += allExt1;
	filterExt += ")|";
	filterExt += allExt1;
	filterExt += "|";
	return filterExt;
}