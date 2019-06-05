!verbose 4

!define PRODUCT_NAME       "DeepSkyStacker"
!define NAMESUFFIX         " (64 bit)"

!define DSS_ICON           "..\DeepSkyStacker\Icon\DSS.ico"

!define DSS_HELP_FR        "Aide DeepSkyStacker.chm"
!define DSS_HELP_ES        "Ayuda DeepSkyStacker.chm"
!define DSS_HELP_EN        "DeepSkyStacker Help.chm"
!define DSS_HELP_DE        "DeepSkyStacker Hilfe.chm"
!define DSS_HELP_PT        "DeepSkyStacker Ajuda.chm"

!define DSS_RUNTIME_MFC    "mfc140.dll"
!define DSS_RUNTIME_CPP    "msvcp140.dll"
!define DSS_RUNTIME_C      "vcruntime140.dll"

!define DSS_PRODUCT        "DeepSkyStacker${NAMESUFFIX}"            # For start menu
!define DSS_VERSION        "4.2.0"                                  # For control panel
!define DSS_VERSION_SUFFIX "Beta 1"                                 # For control panel (e.g. "beta 1")
!define DSS_PUBLISHER      "The DeepSkyStacker Team"       # For control panel

!define DSS_NAME           "DeepSkyStacker${NAMESUFFIX}"
!define DSS_FILE           "DeepSkyStacker"

!define DSSCL_NAME         "DeepSkyStacker Command Line${NAMESUFFIX}"
!define DSSCL_FILE         "DeepSkyStackerCL"

!define DSSLIVE_NAME       "DeepSkyStacker Live${NAMESUFFIX}"
!define DSSLIVE_FILE       "DeepSkyStackerLive"

!define DSS_README_NAME    "README"
!define DSS_README_FILE    "README.txt"


!define DSS_UNINSTALL_NAME "DeepSkyStacker${NAMESUFFIX} Uninstaller"
!define DSS_UNINSTALL_FILE "DSS64-Remove"

!define DSS_REG_UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}64"

CRCCheck On


# define installer name

OutFile "DSS64-Setup.exe"
 
; Take the highest execution level available
; This means that if it's possible to, we become an administrator
RequestExecutionLevel Admin

# set the install directory - the programs are 64 bit versions

InstallDir "$PROGRAMFILES64\${DSS_PRODUCT}"


# Enable/disable UI features we do/dont want

ShowInstDetails       show
ShowUninstDetails     show

Name                  "${DSS_NAME}"
Icon                  "${DSS_ICON}"
UninstallIcon         "${DSS_ICON}"

var PreviousUninstaller

# default installer section start

Section

  # Modify UI behaviours
  
  SetDetailsPrint     both

  # define output path

  SetOutPath $INSTDIR
 
  # Uninstall previous version silently
  ReadRegStr $PreviousUninstaller HKLM ${DSS_REG_UNINSTALL_PATH} "QuietUninstallString"
  
  ExecWait '"$PreviousUninstaller" _?=$INSTDIR'


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
  File "${DSS_README_FILE}"

  # define uninstaller name

  WriteUninstaller "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"
  

  # create desktop shortcut for the apps with UIs

  CreateShortCut "$DESKTOP\${DSS_NAME}.lnk"     "$INSTDIR\${DSS_FILE}.exe" ""
  CreateShortCut "$DESKTOP\${DSSLIVE_NAME}.lnk" "$INSTDIR\${DSSLIVE_FILE}.exe" ""
 
  # create start-menu items 


  CreateDirectory "$SMPROGRAMS\${DSS_PRODUCT}"
  CreateShortCut  "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_NAME}.lnk"        "$INSTDIR\${DSS_FILE}.exe"     "" "$INSTDIR\${DSS_FILE}.exe" 0 
  CreateShortCut  "$SMPROGRAMS\${DSS_PRODUCT}\${DSSLIVE_NAME}.lnk"    "$INSTDIR\${DSSLIVE_FILE}.exe" "" "$INSTDIR\${DSSLIVE_FILE}.exe" 0 
  CreateShortCut  "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_README_NAME}.lnk" "$INSTDIR\${DSS_README_FILE}"  "" "$INSTDIR\${DSS_FILE}.exe" 0 

  CreateShortCut  "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_UNINSTALL_NAME}.lnk" "$INSTDIR\${DSS_UNINSTALL_FILE}.exe" "" "$INSTDIR\${DSS_UNINSTALL_FILE}.exe" 0

  # write uninstall information to the registry
 
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "Publisher"            "${DSS_PUBLISHER}"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "DisplayName"          "${DSS_PRODUCT} ${DSS_VERSION} ${DSS_VERSION_SUFFIX} (remove only)"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "DisplayVersion"       "${DSS_VERSION}"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "DisplayIcon"          "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "UninstallString"      "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "QuietUninstallString" "$INSTDIR\${DSS_UNINSTALL_FILE}.exe /S"  

  # Create the uninstaller program
  
  WriteUninstaller "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"

  # default section end

SectionEnd


 
# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"

Section "Uninstall"
 
  # Modify UI behaviours
  
  SetDetailsPrint     textonly


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
  Delete "$INSTDIR\${DSS_README_FILE}"

  RmDir  "$INSTDIR"
 
  # Delete Start Menu Shortcuts and Desktop shortcuts
   
  Delete "$DESKTOP\${DSS_NAME}.lnk"
  Delete "$DESKTOP\${DSSLIVE_NAME}.lnk"
  
  Delete "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_NAME}.lnk"
  Delete "$SMPROGRAMS\${DSS_PRODUCT}\${DSSLIVE_NAME}.lnk"
  Delete "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_UNINSTALL_NAME}.lnk"
  Delete "$SMPROGRAMS\${DSS_PRODUCT}\${DSS_README_NAME}.lnk"
  RmDir  "$SMPROGRAMS\${DSS_PRODUCT}"
  
  # Delete Product And Uninstall Registry Entries
  
  DeleteRegKey HKEY_LOCAL_MACHINE "${DSS_REG_UNINSTALL_PATH}\" 
  

SectionEnd
