##!include "MUI2.nsh"
!include "FileAssociation.nsh"
!include x64.nsh
!verbose 4

# Want to display Unicode readme
Unicode True

SetCompressor /SOLID lzma

!include "MUI.nsh"
!include "MUI_EXTRAPAGES.nsh"
!include "PathRemove.nsh"
!include "FileFunc.nsh"
!include "WinVer.nsh"
Function .onInit
${IfNot} ${AtLeastWin10}
${OrIfNot} ${RunningX64}
	MessageBox mb_iconStop "Windows 10 x64 is required to install DeepSkyStacker 5.1.2 or higher"
	Abort
${EndIf}
FunctionEnd

!define MUI_HEADERIMAGE

!define PRODUCT_NAME       "DeepSkyStacker"

!define DSS_ICON           "..\DeepSkyStacker\Icon\DSS.ico"

!define DSS_HELP_FR        "Aide DeepSkyStacker.chm"
!define DSS_HELP_ES        "Ayuda DeepSkyStacker.chm"
!define DSS_HELP_EN        "DeepSkyStacker Help.chm"
!define DSS_HELP_DE        "DeepSkyStacker Hilfe.chm"
!define DSS_HELP_PT        "DeepSkyStacker Ajuda.chm"
!define DSS_HELP_NL        "DeepSkyStacker Hulp.chm"

!define DSS_PRODUCT        "DeepSkyStacker"		# For start menu
!define DSS_VERSION        "5.1.8"			# For control panel
!define DSS_VERSION_SUFFIX ""			# For control panel (e.g. " Beta 1" or "") - note leading space
!define DSS_PUBLISHER      "The DeepSkyStacker Team"	# For control panel

!define DSS_NAME           "DeepSkyStacker"
!define DSS_FILE           "DeepSkyStacker"

!define DSSCL_NAME         "DeepSkyStacker Command Line"
!define DSSCL_FILE         "DeepSkyStackerCL"

!define DSSLIVE_NAME       "DeepSkyStacker Live"
!define DSSLIVE_FILE       "DeepSkyStackerLive"

!define DSS_README_NAME    "README"
!define DSS_README_FILE    "README.txt"
!define DSS_EULA		   "../LICENSE"


!define DSS_UNINSTALL_NAME "DeepSkyStacker Uninstaller"
!define DSS_UNINSTALL_FILE "DSS-Remove"

!define DSS_REG_UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

CRCCheck On

# define installer name

OutFile "DSS-Setup.exe"
 
RequestExecutionLevel Admin

# set the install directory - the programs are 64 bit versions

InstallDir "$PROGRAMFILES64\${DSS_PRODUCT}"

# Enable/disable UI features we do/dont want

ShowInstDetails       show
ShowUninstDetails     show

Name			"${DSS_NAME}"
Icon			"${DSS_ICON}"
UninstallIcon		"${DSS_ICON}"
!define MUI_ICON	"${DSS_ICON}"
!define MUI_UNICON 	"${DSS_ICON}"

var PreviousUninstaller

# Add EULA and custom installation pages.
!insertmacro MUI_PAGE_LICENSE "${DSS_EULA}"
!insertmacro MUI_PAGE_README  "${DSS_README_FILE}"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"

${ReadmeLanguage} "${LANG_ENGLISH}" \
          "DeepSkyStacker ${DSS_VERSION}${DSS_VERSION_SUFFIX} Readme.txt" \
          "Details about the new release of DeepSkyStacker" \
          "About $(^name):" \
          "$\n  Click on scrollbar arrows or press Page Down to review the entire text."

Section "Visual Studio Runtime"
  SetOutPath "$INSTDIR"
  File "..\x64\Release\vc_redist.x64.exe"
  SetRegView 64
  ReadRegDWORD $0 HKLM "Software\WOW6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\X64" "Bld"
  SetRegView 32
  ${If} $0 == ""
	#
	# vc_redist build 33816 isn't installed, so install it
	#
	DetailPrint "Visual Studio Runtime is not installed, so install it"
  	ExecWait "$INSTDIR\vc_redist.x64.exe /install /passive /norestart"
  ${Else}
	IntCmp $0 33816 equal less more
	equal:
	#
	# vc_redist build 33816 is installed, force a repair install
	#
	DetailPrint "Visual Studio Runtime build 33816 is already installed, do a repair install"
	ExecWait "$INSTDIR\vc_redist.x64.exe /repair /passive /norestart"
        Goto done
	less:
	#
	# vc_redist build 33816 isn't installed, so install it
	#
	DetailPrint "Visual Studio Runtime build $0 is installed, install build 33816"
  	ExecWait "$INSTDIR\vc_redist.x64.exe /install /passive /norestart"
	Goto done
	more:
	DetailPrint "Visual Studio Runtime build $0 is installed, do nothing"
 	Goto done
	done:
  ${Endif}
  Delete "$INSTDIR\vc_redist.x64.exe"
SectionEnd

# default installer section start
Section
  # Want to use "all users" area of StartMenu/Programs
  SetShellVarContext	all
  
  # define output path

  SetOutPath $INSTDIR
 
  # Uninstall previous version silently
  ReadRegStr $PreviousUninstaller HKLM ${DSS_REG_UNINSTALL_PATH} "UninstallString"
  ${If} $PreviousUninstaller != ""
        #
        # If the Uninstall string is malformed (executable name isn't double quoted)
        # we need to wrap it in double quotes
        #
        StrCpy $0 $PreviousUninstaller 1
        ${If} '"' != $0
              StrCpy $PreviousUninstaller '"$PreviousUninstaller"'
        ${Endif}

        #
        # Now strip out the directory name to feed to _? parameter
        #
        ${PathRemoveArgsAndQuotes} $0 $PreviousUninstaller
        ${GetParent} $0 $R0

        #
        # Finally uninstall the previous version silently
        #
        ExecWait '$PreviousUninstaller /S _?=$R0'
  ${EndIf}
  
  # specify the files that go in the output path

  File "..\x64\Release\${DSS_FILE}.exe"
  File "..\x64\Release\${DSS_FILE}.pdb"
  File "..\x64\Release\${DSSCL_FILE}.exe"
  File "..\x64\Release\${DSSCL_FILE}.pdb"
  File "..\x64\Release\${DSSLIVE_FILE}.exe"
  File "..\x64\Release\${DSSLIVE_FILE}.pdb"
	 
  File "..\x64\Release\Qt6Core.dll"
  File "..\x64\Release\Qt6Gui.dll"
  File "..\x64\Release\Qt6Network.dll"
  # File "..\x64\Release\Qt6Svg.dll"
  File "..\x64\Release\Qt6Widgets.dll"
  File "..\x64\Release\Qt6Charts.dll"
  #File "..\x64\Release\Qt6OpenGL.dll"
  #File "..\x64\Release\Qt6OpenGLWidgets.dll"
 
	  
  File /r "..\x64\Release\iconengines"
  File /r "..\x64\Release\imageformats"
  File /r "..\x64\Release\networkinformation"
  File /r "..\x64\Release\platforms"
  File /r "..\x64\Release\styles"
  File /r "..\x64\Release\tls"
  File /r "..\x64\Release\translations"
  
  File "..\Help\${DSS_HELP_FR}"
  File "..\Help\${DSS_HELP_ES}"
  File "..\Help\${DSS_HELP_EN}"
  File "..\Help\${DSS_HELP_DE}"
  File "..\Help\${DSS_HELP_PT}"
  # File "..\Help\${DSS_HELP_NL}"
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

  #
  # Create a file association for .dssfilelist
  #
  ${registerExtension} "$INSTDIR\${DSS_FILE}.exe" ".dssfilelist" "DeepSkyStacker Filelist"
  
  # write uninstall information to the registry
 
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "Publisher"            "${DSS_PUBLISHER}"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "DisplayName"          "${DSS_PRODUCT} ${DSS_VERSION}${DSS_VERSION_SUFFIX} (Remove only)"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "DisplayVersion"       "${DSS_VERSION}"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "DisplayIcon"          "$INSTDIR\${DSS_FILE}.exe"
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "UninstallString"      '"$INSTDIR\${DSS_UNINSTALL_FILE}.exe"'
  WriteRegStr HKLM "${DSS_REG_UNINSTALL_PATH}" "QuietUninstallString" '"$INSTDIR\${DSS_UNINSTALL_FILE}.exe" /S'

  # Create the uninstaller program
  
  WriteUninstaller "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"

  # default section end

SectionEnd


 
# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"

Section "Uninstall"
 
  # Want to use "all users" area of StartMenu/Programs
  SetShellVarContext	all
  
  # Always delete uninstaller first

  Delete "$INSTDIR\${DSS_UNINSTALL_FILE}.exe" 

  # now delete installed files

  Delete "$INSTDIR\${DSS_FILE}.exe"
  Delete "$INSTDIR\${DSS_FILE}.pdb"
  Delete "$INSTDIR\${DSSCL_FILE}.exe"
  Delete "$INSTDIR\${DSSCL_FILE}.pdb"
  Delete "$INSTDIR\${DSSLIVE_FILE}.exe"
  Delete "$INSTDIR\${DSSLIVE_FILE}.pdb"
  Delete "$INSTDIR\${DSS_HELP_FR}"
  Delete "$INSTDIR\${DSS_HELP_ES}"
  Delete "$INSTDIR\${DSS_HELP_EN}"
  Delete "$INSTDIR\${DSS_HELP_DE}"
  Delete "$INSTDIR\${DSS_HELP_PT}"
  # Delete "$INSTDIR\${DSS_HELP_NL}" 
  Delete "$INSTDIR\${DSS_README_FILE}"
  
  Delete "$INSTDIR\Qt6Core.dll"
  Delete "$INSTDIR\Qt6Gui.dll"
  Delete "$INSTDIR\Qt6Network.dll"
  Delete "$INSTDIR\Qt6Svg.dll"
  Delete "$INSTDIR\Qt6Widgets.dll"
  Delete "$INSTDIR\Qt6Charts.dll"
  Delete "$INSTDIR\Qt6OpenGL.dll"
  Delete "$INSTDIR\Qt6OpenGLWidgets.dll"

  
  Delete "$INSTDIR\iconengines\*"
  Delete "$INSTDIR\imageformats\*"
  Delete "$INSTDIR\networkinformation\*"
  Delete "$INSTDIR\platforms\*"
  Delete "$INSTDIR\styles\*"
  Delete "$INSTDIR\tls\*"
  Delete "$INSTDIR\translations\*"  
  Delete "$INSTDIR\${DSS_UNINSTALL_FILE}.exe"
  RmDir  "$INSTDIR\iconengines"
  RmDir  "$INSTDIR\imageformats" 
  RmDir  "$INSTDIR\networkinformation"
  RmDir  "$INSTDIR\platforms"  
  RmDir  "$INSTDIR\styles"
  RmDir  "$INSTDIR\tls"
  RmDir  "$INSTDIR\translations"

  RmDir  "$INSTDIR"
  
  #
  # de-register the .dssfilelist extension
  #
  ${unregisterExtension} ".dssfilelist" "DeepSkyStacker Filelist"
 
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
