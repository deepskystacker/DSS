# Microsoft Developer Studio Project File - Name="lcms" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lcms - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lcms.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lcms.mak" CFG="lcms - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lcms - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lcms - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lcms - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "LibRelease"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0x417 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\MS\lcms.lib"

!ELSEIF  "$(CFG)" == "lcms - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "LibDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0x417 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\MS\lcmsd.lib"

!ENDIF 

# Begin Target

# Name "lcms - Win32 Release"
# Name "lcms - Win32 Debug"
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
# Begin Group "Documentation"

# PROP Default_Filter ".txt"
# Begin Source File

SOURCE=..\..\doc\LCMSAPI.TXT
# End Source File
# Begin Source File

SOURCE=..\..\doc\TUTORIAL.TXT
# End Source File
# End Group
# End Target
# End Project
