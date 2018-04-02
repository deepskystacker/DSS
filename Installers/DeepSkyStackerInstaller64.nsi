##!include "MUI2.nsh"

!define DSS_ICON           "..\DeepSkyStacker\Icon\DSS.ico"

!define DSS_HELP_FR        "Aide DeepSkyStacker.chm"
!define DSS_HELP_ES        "Ayuda DeepSkyStacker.chm"
!define DSS_HELP_EN        "DeepSkyStacker Help.chm"
!define DSS_HELP_DE        "DeepSkyStacker Hilfe.chm"
!define DSS_HELP_PT        "DeepSkyStacker Ajuda.chm"

!define DSS_RUNTIME_MFC    "mfc140.dll"
!define DSS_RUNTIME_CPP    "msvcp140.dll"
!define DSS_RUNTIME_C      "vcruntime140.dll"


!define DSS_PRODUCT        "DeepSkyStacker"                         # For start menu
!define DSS_VERSION        "4.1.0"                                  # For control panel
!define DSS_VERSION_SUFFIX "beta 1"                                 # For control panel
!define DSS_PUBLISHER      "Luc Coiffier"                           # For control panel

!define DSS_NAME           "DeepSkyStacker (64 bit)"
!define DSS_FILE           "DeepSkyStacker"

!define DSSCL_NAME         "DeepSkyStacker Command Line (64 bit)"
!define DSSCL_FILE         "DeepSkyStackerCL"

!define DSSLIVE_NAME       "DeepSkyStacker Live (64 bit)"
!define DSSLIVE_FILE       "DeepSkyStackerLive"


!define DSS_UNINSTALL_NAME "DeepSkyStacker Uninstaller (64 bit)"
!define DSS_UNINSTALL_FILE "DeepSkyStacker64Uninstaller"

CRCCheck On


# define installer name

OutFile "DeepSkyStacker64Installer.exe"
 
# set the install directory - the programs are 64 bit versions

InstallDir "$PROGRAMFILES64\${DSS_PRODUCT}"

# ** (Do we need this?) **

RequestExecutionLevel admin

# Enable/disable UI features we do/dont want

ShowInstDetails       nevershow
ShowUninstDetails     nevershow

Name                  "${DSS_NAME}"
Icon                  "${DSS_ICON}"
UninstallIcon         "${DSS_ICON}"



# default installer section start

Section

  # Modify UI behaviours
  
  ##SetDetailsPrint     none

  # define output path

  SetOutPath $INSTDIR
 
  # Uninstall previous version silently
  
  ExecWait '"$INSTDIR\${DSS_UNINSTALL_FILE}.exe" /S _?=$INSTDIR'


  # specify the files that go in the output path

  File "..\x64\Release\${DSS_FILE}.exe"
  File "..\x64\Release\${DSSCL_FILE}.exe"
  File "..\x64\Release\${DSSLIVE_FILE}.exe"
  File "..\Help\${DSS_HELP_FR}"
  File "..\Help\${DSS_HELP_ES}"
  File "..\Help\${DSS_HELP_EN}"
  File "..\Help\${DSS_HELP_DE}"
  File "..\Help\${DSS_HELP_PT}"
  File "RunTime64\${DSS_RUNTIME_MFC}"
  File "RunTime64\${DSS_RUNTIME_CPP}"
  File "RunTime64\${DSS_RUNTIME_C}"
 
  # define uninstaller name

  WriteUninstaller "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"
  

  # create desktop shortcut for the apps with UIs

  CreateShortCut "$DESKTOP\${DSS_NAME}.lnk"     "$INSTDIR\${DSS_FILE}.exe" ""
  CreateShortCut "$DESKTOP\${DSSLIVE_NAME}.lnk" "$INSTDIR\${DSSLIVE_FILE}.exe" ""
 
  # create start-menu items 


  CreateDirectory "$SMPROGRAMS\${DSS_PRODUCT}"
  CreateShortCut  "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_NAME}.lnk"     "$INSTDIR\${DSS_FILE}.exe"     "" "$INSTDIR\${DSS_FILE}.exe" 0 
  CreateShortCut  "$SMPROGRAMS\${DSS_PRODUCT}\${DSSLIVE_NAME}.lnk" "$INSTDIR\${DSSLIVE_FILE}.exe" "" "$INSTDIR\${DSSLIVE_FILE}.exe" 0 

  CreateShortCut  "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_UNINSTALL_NAME}.lnk" "$INSTDIR\${DSS_UNINSTALL_FILE}.exe" "" "$INSTDIR\${DSS_UNINSTALL_FILE}.exe" 0

  # write uninstall information to the registry
 
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${DSS_PRODUCT}64" "Publisher"            "${DSS_PUBLISHER}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${DSS_PRODUCT}64" "DisplayName"          "${DSS_PRODUCT} ${DSS_VERSION} ${DSS_VERSION_SUFFIX} (64 bit - remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${DSS_PRODUCT}64" "DisplayVersion"       "${DSS_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${DSS_PRODUCT}64" "DisplayIcon"          "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${DSS_PRODUCT}64" "UninstallString"      "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${DSS_PRODUCT}64" "QuietUninstallString" "$INSTDIR\${DSS_UNINSTALL_FILE}.exe /S"  

  # Create the uninstaller program
  
  WriteUninstaller "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"

  # default section end

SectionEnd


 
# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"

Section "Uninstall"
 
  # Modify UI behaviours
  
  ##SetDetailsPrint     none


  # Always delete uninstaller first

  Delete "$INSTDIR\${DSS_UNINSTALL_FILE}.exe" 

  # now delete installed files

  Delete "$INSTDIR\${DSS_FILE}.exe"
  Delete "$INSTDIR\${DSSCL_FILE}.exe"
  Delete "$INSTDIR\${DSSLIVE_FILE}.exe"
  Delete "$INSTDIR\${DSS_HELP_FR}"
  Delete "$INSTDIR\${DSS_HELP_ES}"
  Delete "$INSTDIR\${DSS_HELP_EN}"
  Delete "$INSTDIR\${DSS_HELP_DE}"
  Delete "$INSTDIR\${DSS_HELP_PT}"
  Delete "$INSTDIR\${DSS_RUNTIME_MFC}"
  Delete "$INSTDIR\${DSS_RUNTIME_CPP}"
  Delete "$INSTDIR\${DSS_RUNTIME_C}"
 
  RmDir  "$INSTDIR"
 
  # Delete Start Menu Shortcuts and Desktop shortcuts
   
  Delete "$DESKTOP\${DSS_NAME}.lnk"
  Delete "$DESKTOP\${DSSLIVE_NAME}.lnk"
  
  Delete "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_NAME}.lnk"
  Delete "$SMPROGRAMS\${DSS_PRODUCT}\${DSSLIVE_NAME}.lnk"
  Delete "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_UNINSTALL_NAME}.lnk"
  RmDir  "$SMPROGRAMS\${DSS_PRODUCT}"
  
  # Delete Product And Uninstall Registry Entries
  
  DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${DSS_PRODUCT}64\" 
  

SectionEnd
