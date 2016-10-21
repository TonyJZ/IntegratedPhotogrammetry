; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=3
Class1=CImageRectifyApp
LastClass=CInorSettingDlg
NewFileInclude2=#include "ImageRectify.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
Resource1=IDD_SetCameraDlg
Class2=CInorSettingDlg
LastTemplate=CDialog
Class3=CSetCameraDlg
Resource2=IDD_InorSettingDLG

[CLS:CImageRectifyApp]
Type=0
HeaderFile=ImageRectify.h
ImplementationFile=ImageRectify.cpp
Filter=N
LastObject=CImageRectifyApp

[DLG:IDD_SetCameraDlg]
Type=1
Class=CSetCameraDlg
ControlCount=34
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_x0,edit,1350631552
Control8=IDC_EDIT_y0,edit,1350631552
Control9=IDC_EDIT_f,edit,1350631552
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_EDIT_ImgWid,edit,1350631552
Control14=IDC_EDIT_ImgHei,edit,1350631552
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_EDIT_PixelWid,edit,1350631552
Control19=IDC_EDIT_PixelHei,edit,1350631552
Control20=IDC_STATIC,button,1342177287
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_EDIT_k1,edit,1350631552
Control25=IDC_EDIT_k2,edit,1350631552
Control26=IDC_EDIT_k3,edit,1350631552
Control27=IDC_STATIC,button,1342177287
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_EDIT_p1,edit,1350631552
Control31=IDC_EDIT_p2,edit,1350631552
Control32=IDC_RADIO_ALL,button,1476526089
Control33=IDC_RADIO_Selected,button,1476395017
Control34=IDC_STATIC,button,1476395015

[DLG:IDD_InorSettingDLG]
Type=1
Class=CInorSettingDlg
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_ImgGRID,MFCGridCtrl,1342242816
Control4=IDC_AddImage,button,1342242816
Control5=IDC_LoadCamera,button,1342242816
Control6=IDC_SetCamera,button,1342242816
Control7=IDC_STATIC,button,1342177287
Control8=IDC_OutputDir,edit,1350631552
Control9=IDC_SetOutputDir,button,1342242816

[CLS:CInorSettingDlg]
Type=0
HeaderFile=InorSettingDlg.h
ImplementationFile=InorSettingDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_SetOutputDir
VirtualFilter=dWC

[CLS:CSetCameraDlg]
Type=0
HeaderFile=SetCameraDlg.h
ImplementationFile=SetCameraDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CSetCameraDlg
VirtualFilter=dWC

