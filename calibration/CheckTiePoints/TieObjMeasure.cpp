// TieObjMeasure.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TieObjMeasure.h"
#include "TieObjMeasureDlg.h"
#include "gui\cmdline.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTieObjMeasureApp

BEGIN_MESSAGE_MAP(CTieObjMeasureApp, CWinApp)
	//{{AFX_MSG_MAP(CTieObjMeasureApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTieObjMeasureApp construction

CTieObjMeasureApp::CTieObjMeasureApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTieObjMeasureApp object

CTieObjMeasureApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTieObjMeasureApp initialization

BOOL CTieObjMeasureApp::InitInstance()
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

	CCommandLine cmdLine;
	
	CString strFlag;
	CString AlignPrjFile, strInstance, strType;
	
	cmdLine.GetFirstParameter( strFlag, AlignPrjFile  );
	
	cmdLine.GetNextParameter( strFlag, strInstance  );
	
//	ASSERT( strFlag.FindOneOf( "-HWND" ) > -1 );
	
	DWORD hWnd=strtoul(strInstance.GetBuffer(256), NULL, 16);

	cmdLine.GetNextParameter( strFlag, strType);
//	ASSERT( strFlag.FindOneOf( "-TO_Type" ) > -1 );
	int type=atoi(strType.GetBuffer(128));

	CTieObjMeasureDlg dlg(AlignPrjFile.GetBuffer(256), (HWND)hWnd);
	m_pMainWnd = &dlg;

	dlg.m_ObjType=type;

	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
