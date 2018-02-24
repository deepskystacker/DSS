# Microsoft Developer Studio Project File - Name="DeepSkyStacker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=DeepSkyStacker - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DeepSkyStacker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DeepSkyStacker.mak" CFG="DeepSkyStacker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DeepSkyStacker - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "DeepSkyStacker - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "DeepSkyStacker - Win32 Release Debug Info" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DeepSkyStacker - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /Ot /Ow /Op /Oy- /Ob1 /I "..\tools" /I "..\LibTIFF" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Ox /Og
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none /debug

!ELSEIF  "$(CFG)" == "DeepSkyStacker - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\tools" /I "..\LibTIFF" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "DeepSkyStacker - Win32 Release Debug Info"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DeepSkyStacker___Win32_Release_Debug_Info"
# PROP BASE Intermediate_Dir "DeepSkyStacker___Win32_Release_Debug_Info"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DeepSkyStacker___Win32_Release_Debug_Info"
# PROP Intermediate_Dir "DeepSkyStacker___Win32_Release_Debug_Info"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GR /GX /Ox /Ot /Ow /Og /Op /Oy- /I "..\tools" /I "..\LibTIFF" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GR /GX /Zi /Ot /Op /Oy- /I "..\tools" /I "..\LibTIFF" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Ox /Ow /Og
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none /debug
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib ws2_32.lib /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "DeepSkyStacker - Win32 Release"
# Name "DeepSkyStacker - Win32 Debug"
# Name "DeepSkyStacker - Win32 Release Debug Info"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BitmapExt.cpp
# End Source File
# Begin Source File

SOURCE=..\DCRaw\dcraw.c

!IF  "$(CFG)" == "DeepSkyStacker - Win32 Release"

# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DeepSkyStacker - Win32 Debug"

# ADD CPP /Ze
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "DeepSkyStacker - Win32 Release Debug Info"

# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /O2
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DeepSkyStacker.cpp
# End Source File
# Begin Source File

SOURCE=.\DeepSkyStacker.rc
# End Source File
# Begin Source File

SOURCE=.\DeepStack.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BezierAdjust.h
# End Source File
# Begin Source File

SOURCE=.\BitmapExt.h
# End Source File
# Begin Source File

SOURCE=.\DeepSkyStacker.h
# End Source File
# Begin Source File

SOURCE=.\DeepStack.h
# End Source File
# Begin Source File

SOURCE=.\Histogram.h
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

SOURCE=.\app.ico
# End Source File
# Begin Source File

SOURCE=.\res\asinh.bmp
# End Source File
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cuberoot.bmp
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\linear.bmp
# End Source File
# Begin Source File

SOURCE=.\res\log.bmp
# End Source File
# Begin Source File

SOURCE=.\res\loglog.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logsqrt.bmp
# End Source File
# Begin Source File

SOURCE=.\open.bmp
# End Source File
# Begin Source File

SOURCE=.\openfile.ico
# End Source File
# Begin Source File

SOURCE=.\redo.bmp
# End Source File
# Begin Source File

SOURCE=.\redo.ico
# End Source File
# Begin Source File

SOURCE=.\settings.ico
# End Source File
# Begin Source File

SOURCE=.\res\sqrt.bmp
# End Source File
# Begin Source File

SOURCE=.\undo.ico
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CheckAbove.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckAbove.h
# End Source File
# Begin Source File

SOURCE=.\DeepStackerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DeepStackerDlg.h
# End Source File
# Begin Source File

SOURCE=.\LuminanceTab.cpp
# End Source File
# Begin Source File

SOURCE=.\LuminanceTab.h
# End Source File
# Begin Source File

SOURCE=.\PictureListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\PictureListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ProcessingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessingDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProcessSettingsSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessSettingsSheet.h
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\RawDDPSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\RawDDPSettings.h
# End Source File
# Begin Source File

SOURCE=.\RegisteringDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegisteringDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegisterSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\RegisterSettings.h
# End Source File
# Begin Source File

SOURCE=.\RGBTab.cpp
# End Source File
# Begin Source File

SOURCE=.\RGBTab.h
# End Source File
# Begin Source File

SOURCE=.\SaturationTab.cpp
# End Source File
# Begin Source File

SOURCE=.\SaturationTab.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\StackingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StackingDlg.h
# End Source File
# Begin Source File

SOURCE=.\StackSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\StackSettings.h
# End Source File
# End Group
# Begin Group "Tools"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Tools\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\BtnST.h
# End Source File
# Begin Source File

SOURCE=..\Tools\cdxCRot90DC.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\cdxCRot90DC.h
# End Source File
# Begin Source File

SOURCE=..\Tools\cdxCRotBevelLine.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\cdxCRotBevelLine.h
# End Source File
# Begin Source File

SOURCE=..\Tools\childprop.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\childprop.h
# End Source File
# Begin Source File

SOURCE=..\Tools\ControlPos.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\ControlPos.h
# End Source File
# Begin Source File

SOURCE=..\Tools\Gradient.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\Gradient.h
# End Source File
# Begin Source File

SOURCE=..\Tools\GradientCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\gradientctrl.h
# End Source File
# Begin Source File

SOURCE=..\Tools\GradientCtrlImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\GradientCtrlImpl.h
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\RegMFC.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\WndImage.cpp
# End Source File
# Begin Source File

SOURCE=..\Tools\WndImage.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\DeepSkyStacker.manifest
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
