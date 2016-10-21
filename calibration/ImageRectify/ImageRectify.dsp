# Microsoft Developer Studio Project File - Name="ImageRectify" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ImageRectify - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ImageRectify.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ImageRectify.mak" CFG="ImageRectify - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ImageRectify - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ImageRectify - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ImageRectify - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386

!ELSEIF  "$(CFG)" == "ImageRectify - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ  /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\OpenRS\DPS\include" /I "\OpenRS\IntegratedPhotogrammetry\include" /I "\OpenRS\desktop\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_Image_Rectify_Inor_LIB_" /Yu"stdafx.h" /FD /GZ    /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 orsBaseD.lib dpsBaseVC60D.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"\openRS\desktop\debug\vc60\ImageRectify.dll" /implib:"\openRS\desktop\debug\vc60\ImageRectify.lib" /pdbtype:sept /libpath:"\openRS\desktop\lib\vc60" /libpath:"\openRS\desktop\debug\vc60" /libpath:"\openrs\dps\lib\vc60"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ImageRectify - Win32 Release"
# Name "ImageRectify - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ImageRectify.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageRectify.def
# End Source File
# Begin Source File

SOURCE=.\ImageRectify.rc
# End Source File
# Begin Source File

SOURCE=.\InorSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\orsImageRectify_Inor.cpp
# End Source File
# Begin Source File

SOURCE=.\SetCameraDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ImageRectify.h
# End Source File
# Begin Source File

SOURCE=.\InorSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\orsImageRectify_Inor.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SetCameraDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ImageRectify.rc2
# End Source File
# End Group
# Begin Group "GridCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\IntegratedPhotogrammetry\include\GridCtrl\GridCtrl_src\GridCell.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\IntegratedPhotogrammetry\include\GridCtrl\GridCtrl_src\GridCellBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\IntegratedPhotogrammetry\include\GridCtrl\GridCtrl_src\GridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\IntegratedPhotogrammetry\include\GridCtrl\GridCtrl_src\GridDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\IntegratedPhotogrammetry\include\GridCtrl\GridCtrl_src\InPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\IntegratedPhotogrammetry\include\GridCtrl\GridCtrl_src\TitleTip.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
