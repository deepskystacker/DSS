# Microsoft Developer Studio Generated NMAKE File, Based on TestZTrace.dsp
!IF "$(CFG)" == ""
CFG=TestZTrace - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TestZTrace - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TestZTrace - Win32 Release" && "$(CFG)" != "TestZTrace - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestZTrace.mak" CFG="TestZTrace - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestZTrace - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "TestZTrace - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "TestZTrace - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\TestZTrace.exe"

!ELSE 

ALL : "ZCLass - Win32 Release" "$(OUTDIR)\TestZTrace.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ZCLass - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\TestZTrace.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\TestZTrace.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestZTrace.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\TestZTrace.pdb" /machine:I386 /out:"$(OUTDIR)\TestZTrace.exe" 
LINK32_OBJS= \
	"$(INTDIR)\TestZTrace.obj" \
	"..\Release\ZCLass.lib"

"$(OUTDIR)\TestZTrace.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestZTrace - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\TestZTrace.exe"

!ELSE 

ALL : "ZCLass - Win32 Debug" "$(OUTDIR)\TestZTrace.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ZCLass - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\TestZTrace.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\TestZTrace.exe"
	-@erase "$(OUTDIR)\TestZTrace.ilk"
	-@erase "$(OUTDIR)\TestZTrace.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestZTrace.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\TestZTrace.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TestZTrace.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\TestZTrace.obj" \
	"..\Debug\ZCLass.lib"

"$(OUTDIR)\TestZTrace.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("TestZTrace.dep")
!INCLUDE "TestZTrace.dep"
!ELSE 
!MESSAGE Warning: cannot find "TestZTrace.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TestZTrace - Win32 Release" || "$(CFG)" == "TestZTrace - Win32 Debug"
SOURCE=.\TestZTrace.cpp

"$(INTDIR)\TestZTrace.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "TestZTrace - Win32 Release"

"ZCLass - Win32 Release" : 
   cd "\Projects\ZCLass"
   $(MAKE) /$(MAKEFLAGS) /F .\ZCLass.mak CFG="ZCLass - Win32 Release" 
   cd ".\TestZTrace"

"ZCLass - Win32 ReleaseCLEAN" : 
   cd "\Projects\ZCLass"
   $(MAKE) /$(MAKEFLAGS) /F .\ZCLass.mak CFG="ZCLass - Win32 Release" RECURSE=1 CLEAN 
   cd ".\TestZTrace"

!ELSEIF  "$(CFG)" == "TestZTrace - Win32 Debug"

"ZCLass - Win32 Debug" : 
   cd "\Projects\ZCLass"
   $(MAKE) /$(MAKEFLAGS) /F .\ZCLass.mak CFG="ZCLass - Win32 Debug" 
   cd ".\TestZTrace"

"ZCLass - Win32 DebugCLEAN" : 
   cd "\Projects\ZCLass"
   $(MAKE) /$(MAKEFLAGS) /F .\ZCLass.mak CFG="ZCLass - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\TestZTrace"

!ENDIF 


!ENDIF 

