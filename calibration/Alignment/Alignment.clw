; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Alignment.h"
LastPage=0

ClassCount=8
Class1=CAlignmentApp
Class2=CAlignmentDoc
Class3=CAlignmentView
Class4=CMainFrame
Class12=COutputBar

ResourceCount=6
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class5=CAboutDlg
Class6=CWorkspaceBar
Class7=CWorkspaceBar2
Resource3=IDD_ABOUTBOX (English (U.S.))
Resource4=IDR_CONTEXT_MENU (English (U.S.))
Resource5=IDD_ImportDataDlg
Class8=CImportLidDlg
Resource6=IDR_MAINFRAME (English (U.S.))

[CLS:CAlignmentApp]
Type=0
HeaderFile=Alignment.h
ImplementationFile=Alignment.cpp
Filter=N

[CLS:CAlignmentDoc]
Type=0
HeaderFile=AlignmentDoc.h
ImplementationFile=AlignmentDoc.cpp
Filter=N

[CLS:CAlignmentView]
Type=0
HeaderFile=AlignmentView.h
ImplementationFile=AlignmentView.cpp
Filter=C


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
BaseClass=CFrameWnd
VirtualFilter=fWC
LastObject=CMainFrame




[CLS:CAboutDlg]
Type=0
HeaderFile=Alignment.cpp
ImplementationFile=Alignment.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg

[CLS:CWorkspaceBar]
Type=0
HeaderFile=WorkspaceBar.h
ImplementationFile=WorkspaceBar.cpp
Filter=W
LastObject=ID_ImportData

[CLS:CWorkspaceBar2]
Type=0
HeaderFile=WorkspaceBar2.h
ImplementationFile=WorkspaceBar2.cpp
Filter=W

[CLS:COutputBar]
Type=0
HeaderFile=OutputBar.h
ImplementationFile=OutputBar.cpp
Filter=W

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
CommandCount=16
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command16=ID_APP_ABOUT

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
CommandCount=14
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE


[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_Prj_NEW
Command2=ID_Prj_OPEN
Command3=ID_Prj_SAVE
Command4=ID_ImportLidData
Command5=ID_OpenTieMeasure
Command6=ID_TiePOINT
Command7=ID_TieLine
Command8=ID_TiePatch
CommandCount=8

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CMainFrame
Command1=ID_Prj_NEW
Command2=ID_Prj_OPEN
Command3=ID_Prj_SAVE
Command4=ID_Prj_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_VIEW_STATUS_BAR
Command8=ID_VIEW_APPLOOK_2000
Command9=ID_VIEW_APPLOOK_XP
Command10=ID_VIEW_APPLOOK_2003
Command11=ID_VIEW_APPLOOK_VS2005
Command12=ID_VIEW_APPLOOK_WIN_XP
Command13=ID_VIEW_APPLOOK_2007
Command14=ID_VIEW_APPLOOK_2007_1
Command15=ID_VIEW_APPLOOK_2007_2
Command16=ID_VIEW_APPLOOK_2007_3
Command17=ID_VIEW_APPLOOK_VS2008
Command18=ID_APP_ABOUT
CommandCount=18

[MNU:IDR_CONTEXT_MENU (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_CUT
Command2=ID_EDIT_COPY
Command3=ID_EDIT_PASTE
CommandCount=3

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_COMPANY_URL,button,1342242816

[DLG:IDD_ImportDataDlg]
Type=1
Class=CImportLidDlg
ControlCount=11
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_FileSetName,edit,1484849280
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_ImgName,edit,1350631552
Control8=IDC_BUTTON_ImgName,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_ATNName,edit,1350631552
Control11=IDC_BUTTON_AtnName,button,1342242816

[CLS:CImportLidDlg]
Type=0
HeaderFile=ImportLidDlg.h
ImplementationFile=ImportLidDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_BUTTON_ImgName
VirtualFilter=dWC

