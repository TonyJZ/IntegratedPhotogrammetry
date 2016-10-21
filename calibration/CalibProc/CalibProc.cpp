// CalibProc.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"


#include "CalibProc.h"


#include "MainFrm.h"
#include "CalibProcDoc.h"
#include "CalibProcView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCalibProcApp

BEGIN_MESSAGE_MAP(CCalibProcApp, CWinApp)
//{{AFX_MSG_MAP(CCalibProcApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
// Standard print setup command
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalibProcApp construction
orsIPlatform *g_pPlatform;

CCalibProcApp::CCalibProcApp()
{
	ors_string errorinfo;
	g_pPlatform = orsInitialize(errorinfo, true );
}

orsIPlatform *getPlatform()
{
	return g_pPlatform;
}

CCalibProcApp::~CCalibProcApp()
{
	orsUninitialize();
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CCalibProcApp object

CCalibProcApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCalibProcApp initialization

BOOL CCalibProcApp::InitInstance()
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
	
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCalibProcDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CCalibProcView));
	AddDocTemplate(pDocTemplate);
	
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
	CString	m_text1;
	CString	m_text2;
	CString	m_text3;
	CString	m_text4;
	double	m_text5;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_text1 = _T("");
	m_text2 = _T("");
	m_text3 = _T("");
	m_text4 = _T("");
	m_text5 = 0.0;
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_EDIT1, m_text1);
	DDX_Text(pDX, IDC_EDIT2, m_text2);
	DDX_Text(pDX, IDC_EDIT3, m_text3);
	DDX_Text(pDX, IDC_EDIT4, m_text4);
	DDX_Text(pDX, IDC_EDIT5, m_text5);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CCalibProcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CCalibProcApp message handlers


void CAboutDlg::OnButton1() 
{
	
	// using namespace std;
	//    string m_text1 = "00" ;//= "3a "; //3a为16进制, 16进制就是无符号的int < <==> > MFC的BYTE型
	char *ptr;
	
	typedef union
	{
		float          float_data;
		unsigned char  Hex_data[4];
	}Float_Data;
	Float_Data Data_float;      //符点数共用体
	// TODO: Add your control notification handler code here
	UpdateData();
	Data_float.Hex_data[0] = (char)(strtol(m_text1, &ptr, 16));//strtol 把十六进制变为整形，(BYTE) 把十进制整形变为十六进制
	Data_float.Hex_data[1] = (char)strtol(m_text2, &ptr, 16);
	Data_float.Hex_data[2] = (char)strtol(m_text3, &ptr, 16);
	Data_float.Hex_data[3] = (BYTE)strtol(m_text4, &ptr, 16);
	
	
	
	m_text5 = Data_float.float_data;
	UpdateData(FALSE);
	
	
}

void CAboutDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
    char str[1];
	// unsigned char a1[4];
	typedef union
	{
		float          float_data;
		unsigned char  Hex_data[4];
	}Float_Data;
    Float_Data Data_float;      //符点数共用体
	UpdateData();
	Data_float.float_data = m_text5;
	// a1[0] = Data_float.Hex_data[ 0 ] ; 
	m_text1 = itoa(Data_float.Hex_data[ 0 ], str, 16);
	m_text2 = itoa(Data_float.Hex_data[ 1 ], str, 16);
	m_text3 = itoa(Data_float.Hex_data[ 2 ], str, 16);
	m_text4 = itoa(Data_float.Hex_data[ 3 ], str, 16);
	//m_text3 = sprintf(str, "%x", 1); //将100转为16进制表示的字符串。
	UpdateData(FALSE); 
}
