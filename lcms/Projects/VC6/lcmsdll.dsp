# Microsoft Developer Studio Project File - Name="lcmsdll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=lcmsdll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lcmsdll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lcmsdll.mak" CFG="lcmsdll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lcmsdll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "lcmsdll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lcmsdll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "lcmsdll___Win32_Release"
# PROP BASE Intermediate_Dir "lcmsdll___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "DllRelease"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LCMSDLL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Zp1 /MT /W3 /O2 /I "..\include" /I "..\..\include" /D "NDEBUG" /D "LCMS_DLL" /D "LCMS_DLL_BUILD" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fo"Release/" /Fd"Release/" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib uuid.lib /nologo /dll /pdb:none /machine:I386 /out:"..\..\bin\lcms.dll" /implib:"Release/lcms.lib"

!ELSEIF  "$(CFG)" == "lcmsdll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "lcmsdll___Win32_Debug"
# PROP BASE Intermediate_Dir "lcmsdll___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "DllDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LCMSDLL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LCMSDLL_EXPORTS" /D "LCMS_DLL" /D "LCMS_DLL_BUILD" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib libc.lib /nologo /dll /debug /machine:I386 /out:"..\..\bin\lcmsd.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "lcmsdll - Win32 Release"
# Name "lcmsdll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\cmscam02.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmscam97.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmscgats.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmscnvrt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmserr.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsgamma.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsgmt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsintrp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsio0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsio1.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmslut.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsmatsh.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsmtrx.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsnamed.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmspack.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmspcs.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsps2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmssamp.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsvirt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmswtpnt.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmsxform.c
# End Source File
# Begin Source File

SOURCE=..\..\src\lcms.def
# End Source File
# Begin Source File

SOURCE=.\lcms.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\icc34.h
# End Source File
# Begin Source File

SOURCE=..\..\include\lcms.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
