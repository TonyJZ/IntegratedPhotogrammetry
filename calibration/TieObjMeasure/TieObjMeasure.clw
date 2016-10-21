; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTieObjMeasureDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "TieObjMeasure.h"

ClassCount=3
Class1=CTieObjMeasureApp
Class2=CTieObjMeasureDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_TIEOBJMEASURE_DIALOG
Resource4=IDR_MENU

[CLS:CTieObjMeasureApp]
Type=0
HeaderFile=TieObjMeasure.h
ImplementationFile=TieObjMeasure.cpp
Filter=N
LastObject=CTieObjMeasureApp

[CLS:CTieObjMeasureDlg]
Type=0
HeaderFile=TieObjMeasureDlg.h
ImplementationFile=TieObjMeasureDlg.cpp
Filter=D
LastObject=ID_SaveAllObjs
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=TieObjMeasureDlg.h
ImplementationFile=TieObjMeasureDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_TIEOBJMEASURE_DIALOG]
Type=1
Class=CTieObjMeasureDlg
ControlCount=19
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177287
Control3=IDC_BUTTON_NewObj,button,1342242816
Control4=IDC_GRID1,MFCGridCtrl,1342242816
Control5=IDC_GRID2,MFCGridCtrl,1342242816
Control6=IDC_RADIO1,button,1342308361
Control7=IDC_RADIO2,button,1342177289
Control8=IDC_RADIO3,button,1342177289
Control9=IDC_BUTTON_DeleteObj,button,1342242816
Control10=IDC_BUTTON_Save,button,1342242816
Control11=IDC_BUTTON_DeleteItem,button,1342242816
Control12=IDC_IsGCP,button,1342242819
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308352
Control15=IDC_GCP_X,edit,1350631552
Control16=IDC_STATIC,static,1342308352
Control17=IDC_GCP_Y,edit,1350631552
Control18=IDC_STATIC,static,1342308352
Control19=IDC_GCP_Z,edit,1350631552

[MNU:IDR_MENU]
Type=1
Class=?
Command1=ID_SaveAllObjs
CommandCount=1

