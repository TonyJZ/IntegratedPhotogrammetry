; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCalibProcDoc
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "calibproc.h"
LastPage=0

ClassCount=5
Class1=CCalibProcApp
Class2=CAboutDlg
Class3=CCalibProcDoc
Class4=CCalibProcView
Class5=CMainFrame

ResourceCount=3
Resource1=IDD_SetMisalignDIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX

[CLS:CCalibProcApp]
Type=0
BaseClass=CWinApp
HeaderFile=CalibProc.h
ImplementationFile=CalibProc.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=CalibProc.cpp
ImplementationFile=CalibProc.cpp
LastObject=ID_check_Projection

[CLS:CCalibProcDoc]
Type=0
BaseClass=CDocument
HeaderFile=CalibProcDoc.h
ImplementationFile=CalibProcDoc.cpp
Filter=N
VirtualFilter=DC
LastObject=ID_ATNToLAS11

[CLS:CCalibProcView]
Type=0
BaseClass=CView
HeaderFile=CalibProcView.h
ImplementationFile=CalibProcView.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=14
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT1,edit,1350631552
Control8=IDC_EDIT2,edit,1350631552
Control9=IDC_EDIT3,edit,1350631552
Control10=IDC_EDIT4,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT5,edit,1350631552
Control13=IDC_BUTTON1,button,1342242816
Control14=IDC_BUTTON2,button,1342242816

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
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
Command16=ID_APP_ABOUT
Command17=ID_ReadTrj
Command18=ID_GPS_Interpolation
Command19=ID_Trajectory_Clipping
Command20=ID_READ_ATNFILE
Command21=ID_Cal_LaserPosition
Command22=ID_StatPointError
Command23=ID_check_Projection
Command24=ID_TransTPlane
Command25=ID_CalPos_TPlane
Command26=ID_ATN2TPlane
Command27=ID_ATNToLAS11
Command28=ID_TP2UTM
Command29=ID_Read_TS_Bin
Command30=ID_DetectCircleTarget
Command31=ID_SegmentCircle
Command32=ID_ImageTypeTrans
Command33=ID_SimulateRectangle
Command34=ID_GetBoundary
Command35=ID_GetOverlap
Command36=ID_GetOverlapData
Command37=ID_QC_Overlap
Command38=ID_Point_Interpolation
Command39=ID_Lidar_Point_Clip
Command40=ID_Geodetic_To_Geocentric
Command41=ID_CreateATNIdx
Command42=ID_TiePoint
Command43=ID_TEST_levmar
Command44=ID_CoplanarAdjustment
Command45=ID_GCP_WGS84_To_TPlane
CommandCount=45

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[ACL:IDR_MAINFRAME]
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

[DLG:IDD_SetMisalignDIALOG]
Type=1
Class=?
ControlCount=16
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_Roll,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_Pitch,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_Heading,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_Tx,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_EDIT_Ty,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT_Tz,edit,1350631552

