# Microsoft Developer Studio Project File - Name="BlockManager" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=BlockManager - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BlockManager.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BlockManager.mak" CFG="BlockManager - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BlockManager - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "BlockManager - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BlockManager - Win32 Release"

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

!ELSEIF  "$(CFG)" == "BlockManager - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\openrs\desktop\src\plugins\gui\orsImageLayerRender" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_BLOCKMANAGER_" /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"\openRS\desktop\debug\vc60/BlockManagerD.dll" /implib:"\openRS\desktop\debug\vc60/BlockManagerD.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "BlockManager - Win32 Release"
# Name "BlockManager - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BlockManager.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockManager.rc
# End Source File
# Begin Source File

SOURCE=.\BlockMApp.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockSettingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CoordSysPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ImgSettingPage.cpp
# End Source File
# Begin Source File

SOURCE=.\LidarSettingPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelSpecificPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ProjectionPage.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BlockManager.h
# End Source File
# Begin Source File

SOURCE=.\BlockMApp.h
# End Source File
# Begin Source File

SOURCE=.\BlockSettingDlg.h
# End Source File
# Begin Source File

SOURCE=.\CoordSysPage.h
# End Source File
# Begin Source File

SOURCE=.\ImgSettingPage.h
# End Source File
# Begin Source File

SOURCE=.\IPhBaseDef.h
# End Source File
# Begin Source File

SOURCE=.\LidarSettingPage.h
# End Source File
# Begin Source File

SOURCE=.\ModelSpecificPage.h
# End Source File
# Begin Source File

SOURCE=.\ProjectionPage.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\BlockManager.rc2
# End Source File
# End Group
# Begin Group "TabCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\desktop\src\plugins\gui\orsImageLayerRender\PropertiesTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\desktop\src\plugins\gui\orsImageLayerRender\PropertiesTabCtrl.h
# End Source File
# End Group
# Begin Group "GridCtrl"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=..\..\..\src\BlockManager\ReadMe.txt
# End Source File
# End Target
# End Project
