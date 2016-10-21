# Microsoft Developer Studio Project File - Name="LidCalib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LidCalib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LidCalib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LidCalib.mak" CFG="LidCalib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LidCalib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LidCalib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LidCalib - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "\openrs\desktop\include" /I "..\lastool" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_LIDCALIB_" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 matrix.lib tindll.lib orsBase.lib /nologo /subsystem:windows /dll /machine:I386 /def:".\LidCalib.def" /out:"\openrs\desktop\release\vc60/LidCalib.dll" /implib:"\openrs\desktop\release\vc60/LidCalib.lib" /libpath:"\openrs\desktop\lib\vc60" /libpath:"\openrs\desktop\release\vc60"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "LidCalib - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "\openrs\desktop\include" /I "..\lastool" /I "F:\OpenSource\computer vision\LM\levmar-2.5" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_LIDCALIB_" /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 matrix.lib tindll.lib orsBaseD.lib levmar.lib /nologo /subsystem:windows /dll /debug /machine:I386 /def:".\LidCalib.def" /out:"\openrs\desktop\debug\vc60/LidCalib.dll" /implib:"\openrs\desktop\debug\vc60/LidCalib.lib" /pdbtype:sept /libpath:"\openrs\desktop\lib\vc60" /libpath:"\openrs\desktop\debug\vc60"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LidCalib - Win32 Release"
# Name "LidCalib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ATN_TPReader.cpp
# End Source File
# Begin Source File

SOURCE=.\ATNPointIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\ATNReader.cpp
# End Source File
# Begin Source File

SOURCE=.\CircleDetection.cpp
# End Source File
# Begin Source File

SOURCE=.\Geometry.cpp
# End Source File
# Begin Source File

SOURCE=.\GMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\LidarGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\LidCalib.cpp
# End Source File
# Begin Source File

SOURCE=.\LidCalib.def
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\LidCalib.rc
# End Source File
# Begin Source File

SOURCE=.\ModelCorrection.cpp
# End Source File
# Begin Source File

SOURCE=..\pubFunc\MyLine.cpp
# End Source File
# Begin Source File

SOURCE=.\PlaneConstraint_Model.cpp
# End Source File
# Begin Source File

SOURCE=.\PointInterpolation.cpp
# End Source File
# Begin Source File

SOURCE=.\SetTPlaneDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Transform.cpp
# End Source File
# Begin Source File

SOURCE=.\TrjReader.cpp
# End Source File
# Begin Source File

SOURCE=.\TS_ScanBin_reader.cpp
# End Source File
# Begin Source File

SOURCE=.\VP_Model.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\addon.h
# End Source File
# Begin Source File

SOURCE=.\ATN_TPReader.h
# End Source File
# Begin Source File

SOURCE=.\ATNPointIndex.h
# End Source File
# Begin Source File

SOURCE=.\ATNReader.h
# End Source File
# Begin Source File

SOURCE=.\CalibBasedef.h
# End Source File
# Begin Source File

SOURCE=.\CircleDetection.h
# End Source File
# Begin Source File

SOURCE=.\Geometry.h
# End Source File
# Begin Source File

SOURCE=.\gmatrix.h
# End Source File
# Begin Source File

SOURCE=.\LidarGeometry.h
# End Source File
# Begin Source File

SOURCE=.\LidCalib.h
# End Source File
# Begin Source File

SOURCE=.\ModelCorrection.h
# End Source File
# Begin Source File

SOURCE=..\pubFunc\MyLine.h
# End Source File
# Begin Source File

SOURCE=.\PlaneConstraint_Model.h
# End Source File
# Begin Source File

SOURCE=.\PointInterpolation.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SetTPlaneDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Transform.h
# End Source File
# Begin Source File

SOURCE=.\Trj_Def.h
# End Source File
# Begin Source File

SOURCE=.\TrjReader.h
# End Source File
# Begin Source File

SOURCE=.\TS_ScanBin_reader.h
# End Source File
# Begin Source File

SOURCE=.\VP_Model.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\LidCalib.rc2
# End Source File
# End Group
# Begin Group "lastool"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\lastool\lasdefinitions.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader0compressed.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader0raw.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader1compressed.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader1raw.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader2compressed.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader2raw.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader3compressed.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointreader3raw.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter0compressed.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter0raw.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter1compressed.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter1raw.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter2compressed.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter2raw.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter3compressed.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laspointwriter3raw.h
# End Source File
# Begin Source File

SOURCE=..\lastool\lasreader.cpp
# End Source File
# Begin Source File

SOURCE=..\lastool\lasreader.h
# End Source File
# Begin Source File

SOURCE=..\lastool\lasreader_fast.cpp
# End Source File
# Begin Source File

SOURCE=..\lastool\lasreader_fast.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laswriter.cpp
# End Source File
# Begin Source File

SOURCE=..\lastool\laswriter.h
# End Source File
# Begin Source File

SOURCE=..\lastool\laswriter_fast.cpp
# End Source File
# Begin Source File

SOURCE=..\lastool\laswriter_fast.h
# End Source File
# Begin Source File

SOURCE=..\lastool\predicates.cpp
# End Source File
# Begin Source File

SOURCE=..\lastool\triangulate.cpp
# End Source File
# Begin Source File

SOURCE=..\lastool\triangulate.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
