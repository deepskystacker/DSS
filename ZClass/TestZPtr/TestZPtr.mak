# Microsoft Developer Studio Generated NMAKE File, Based on TestZPtr.dsp
!IF "$(CFG)" == ""
CFG=TestZPtr - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TestZPtr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TestZPtr - Win32 Release" && "$(CFG)" != "TestZPtr - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestZPtr.mak" CFG="TestZPtr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestZPtr - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "TestZPtr - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "TestZPtr - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\TestZPtr.exe"

!ELSE 

ALL : "ZCLass - Win32 Release" "$(OUTDIR)\TestZPtr.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ZCLass - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\TestZPtr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\TestZPtr.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W4 /WX /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\TestZPtr.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestZPtr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\TestZPtr.pdb" /machine:I386 /out:"$(OUTDIR)\TestZPtr.exe" 
LINK32_OBJS= \
	"$(INTDIR)\TestZPtr.obj" \
	"..\Release\ZCLass.lib"

"$(OUTDIR)\TestZPtr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestZPtr - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\TestZPtr.exe"

!ELSE 

ALL : "ZCLass - Win32 Debug" "$(OUTDIR)\TestZPtr.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ZCLass - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\TestZPtr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\TestZPtr.exe"
	-@erase "$(OUTDIR)\TestZPtr.ilk"
	-@erase "$(OUTDIR)\TestZPtr.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W4 /WX /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\TestZPtr.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestZPtr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\TestZPtr.pdb" /debug /machine:I386 /out:"$(OUTDIR)\TestZPtr.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\TestZPtr.obj" \
	"..\Debug\ZCLass.lib"

"$(OUTDIR)\TestZPtr.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("TestZPtr.dep")
!INCLUDE "TestZPtr.dep"
!ELSE 
!MESSAGE Warning: cannot find "TestZPtr.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TestZPtr - Win32 Release" || "$(CFG)" == "TestZPtr - Win32 Debug"
SOURCE=.\TestZPtr.cpp

"$(INTDIR)\TestZPtr.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "TestZPtr - Win32 Release"

"ZCLass - Win32 Release" : 
   cd "\Projects\ZCLass"
   $(MAKE) /$(MAKEFLAGS) /F .\ZCLass.mak CFG="ZCLass - Win32 Release" 
   cd ".\TestZPtr"

"ZCLass - Win32 ReleaseCLEAN" : 
   cd "\Projects\ZCLass"
   $(MAKE) /$(MAKEFLAGS) /F .\ZCLass.mak CFG="ZCLass - Win32 Release" RECURSE=1 CLEAN 
   cd ".\TestZPtr"

!ELSEIF  "$(CFG)" == "TestZPtr - Win32 Debug"

"ZCLass - Win32 Debug" : 
   cd "\Projects\ZCLass"
   $(MAKE) /$(MAKEFLAGS) /F .\ZCLass.mak CFG="ZCLass - Win32 Debug" 
   cd ".\TestZPtr"

"ZCLass - Win32 DebugCLEAN" : 
   cd "\Projects\ZCLass"
   $(MAKE) /$(MAKEFLAGS) /F .\ZCLass.mak CFG="ZCLass - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\TestZPtr"

!ENDIF 


!ENDIF 

