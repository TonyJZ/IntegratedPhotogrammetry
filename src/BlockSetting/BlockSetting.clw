; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CImgSettingPage
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "blocksetting.h"
LastPage=0

ClassCount=11
Class1=CBatchPosInputDlg
Class2=CBlockSettingApp
Class3=CBlockSettingDlg
Class4=CCameraSettingDialog
Class5=CCoordSysPage
Class6=CCoordSysTransDlg
Class7=CImgSettingPage
Class8=CInputPosDialog
Class9=CLidarSettingPage
Class10=CModelSpecificPage
Class11=CProjectionPage

ResourceCount=10
Resource1=IDD_DIALOG_CAMERA
Resource2=IDD_ProjectionDlg
Resource3=IDD_DIALOG_POS
Resource4=IDD_CoordinateSysDlg
Resource5=IDD_BlockSetting
Resource6=IDD_LidarSettingDlg
Resource7=IDD_DIALOG_BatchPOS
Resource8=IDD_ImageSettingDlg
Resource9=IDD_ModelSpecificDlg
Resource10=ID_CoordSysTransDlg

[CLS:CBatchPosInputDlg]
Type=0
BaseClass=CDialog
HeaderFile=BatchPosInputDlg.h
ImplementationFile=BatchPosInputDlg.cpp
LastObject=CBatchPosInputDlg
Filter=D
VirtualFilter=dWC

[CLS:CBlockSettingApp]
Type=0
BaseClass=CWinApp
HeaderFile=BlockMApp.h
ImplementationFile=BlockMApp.cpp

[CLS:CBlockSettingDlg]
Type=0
BaseClass=CDialog
HeaderFile=BlockSettingDlg.h
ImplementationFile=BlockSettingDlg.cpp

[CLS:CCameraSettingDialog]
Type=0
BaseClass=CDialog
HeaderFile=CameraSettingDialog.h
ImplementationFile=CameraSettingDialog.cpp
LastObject=IDC_EDIT_PixelWid

[CLS:CCoordSysPage]
Type=0
BaseClass=CDialog
HeaderFile=CoordSysPage.h
ImplementationFile=CoordSysPage.cpp

[CLS:CCoordSysTransDlg]
Type=0
BaseClass=CDialog
HeaderFile=CoordSysTransDlg.h
ImplementationFile=CoordSysTransDlg.cpp

[CLS:CImgSettingPage]
Type=0
BaseClass=CDialog
HeaderFile=ImgSettingPage.h
ImplementationFile=ImgSettingPage.cpp
LastObject=IDC_AddImage
Filter=D
VirtualFilter=dWC

[CLS:CInputPosDialog]
Type=0
BaseClass=CDialog
HeaderFile=InputPosDialog.h
ImplementationFile=InputPosDialog.cpp
LastObject=IDC_BUTTON_CoordTrans

[CLS:CLidarSettingPage]
Type=0
BaseClass=CDialog
HeaderFile=LidarSettingPage.h
ImplementationFile=LidarSettingPage.cpp

[CLS:CModelSpecificPage]
Type=0
BaseClass=CDialog
HeaderFile=ModelSpecificPage.h
ImplementationFile=ModelSpecificPage.cpp
Filter=D
VirtualFilter=dWC
LastObject=CModelSpecificPage

[CLS:CProjectionPage]
Type=0
BaseClass=CDialog
HeaderFile=ProjectionPage.h
ImplementationFile=ProjectionPage.cpp

[DLG:IDD_DIALOG_BatchPOS]
Type=1
Class=CBatchPosInputDlg
ControlCount=13
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_Grid,MFCGridCtrl,1342242816
Control4=IDC_EDIT_POSPath,edit,1350631552
Control5=IDC_BUTTON_OpenPOS,button,1342242816
Control6=IDC_STATIC,button,1342177287
Control7=IDC_RADIO_360,button,1342308361
Control8=IDC_RADIO_400,button,1342177289
Control9=IDC_RADIO_Radians,button,1342177289
Control10=IDC_STATIC,button,1342177287
Control11=IDC_RADIO_YXZ,button,1342308361
Control12=IDC_RADIO_XYZ,button,1342177289
Control13=IDC_BUTTON_CoordSysTrans,button,1476460544

[DLG:IDD_BlockSetting]
Type=1
Class=CBlockSettingDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_TAB,SysTabControl32,1342177280

[DLG:IDD_DIALOG_CAMERA]
Type=1
Class=CCameraSettingDialog
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

[DLG:IDD_CoordinateSysDlg]
Type=1
Class=CCoordSysPage
ControlCount=3
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177287
Control3=IDC_ImportCoordSys,button,1342242816

[DLG:ID_CoordSysTransDlg]
Type=1
Class=CCoordSysTransDlg
ControlCount=13
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_HorizontalDatum,edit,1350631552
Control6=IDC_HorizontalUnit,combobox,1344339970
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_VerticalDatum,edit,1350631552
Control11=IDC_VerticalUnit,combobox,1344339970
Control12=IDOK,button,1342242817
Control13=IDCANCEL,button,1342242816

[DLG:IDD_ImageSettingDlg]
Type=1
Class=CImgSettingPage
ControlCount=6
Control1=IDC_ImgGRID,MFCGridCtrl,1342242816
Control2=IDC_AddImage,button,1342242816
Control3=IDC_AddCamera,button,1342242816
Control4=IDC_AddPOS,button,1342242816
Control5=IDC_CHECK_CopyImg,button,1342242819
Control6=IDC_STATIC,button,1342177287

[DLG:IDD_DIALOG_POS]
Type=1
Class=CInputPosDialog
ControlCount=24
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_Xs,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EDIT_Ys,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_EDIT_Zs,edit,1350631552
Control10=IDC_STATIC,button,1342177287
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_Phi,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_EDIT_Omega,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT_Kappa,edit,1350631552
Control17=IDC_STATIC,button,1342177287
Control18=IDC_RADIO_360,button,1342308361
Control19=IDC_RADIO_400,button,1342177289
Control20=IDC_RADIO_Radians,button,1342177289
Control21=IDC_STATIC,button,1342177287
Control22=IDC_RADIO_YXZ,button,1342308361
Control23=IDC_RADIO_XYZ,button,1342177289
Control24=IDC_BUTTON_CoordTrans,button,1476460544

[DLG:IDD_LidarSettingDlg]
Type=1
Class=CLidarSettingPage
ControlCount=5
Control1=IDC_LidarGRID,MFCGridCtrl,1342242816
Control2=IDC_BUTTON_AddLIDAR,button,1342242817
Control3=IDC_BUTTON_CoordTrans,button,1342242816
Control4=IDC_CHECK_CopyLIDAR,button,1342242819
Control5=IDC_STATIC,button,1342177287

[DLG:IDD_ModelSpecificDlg]
Type=1
Class=CModelSpecificPage
ControlCount=11
Control1=IDC_STATIC,static,1342308352
Control2=IDC_MinHeight,edit,1350631552
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342308352
Control5=IDC_MaxHeight,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_FlyOverlap,edit,1350631552
Control8=IDC_STATIC,static,1342308352
Control9=IDC_StripOverlap,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_Scale,edit,1350631552

[DLG:IDD_ProjectionDlg]
Type=1
Class=CProjectionPage
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT1,edit,1350631552
Control5=IDC_EDIT2,edit,1350631552
Control6=IDC_EDIT3,edit,1350631552

