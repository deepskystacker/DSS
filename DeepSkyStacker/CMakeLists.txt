set(PROJECT_NAME DeepSkyStacker)
message("Configuring project: " ${PROJECT_NAME})

set(COMPILE_WARNING_AS_ERROR YES)

################################################################################
# Source groups
################################################################################
set(Dialogs
    "About.cpp"
    "About.h"
    "ExplorerBar.cpp"
    "ExplorerBar.h"
    "oldprogressdlg.cpp"
    "oldprogressdlg.h"
    "progressdlg.cpp"
    "progressdlg.h"
)
source_group("Dialogs" FILES ${Dialogs})

set(Dialogs__Processing
    "processingcontrols.cpp"
    "processingcontrols.h"
    "ProcessingDlg.cpp"
    "ProcessingDlg.h"
    "ProcessingSettings.h"
    "ProcessingSettingsDlg.cpp"
    "ProcessingSettingsDlg.h"
    "RGBTab.cpp"
    "SavePicture.cpp"
    "SavePicture.h"
    "StarMaskDlg.cpp"
    "StarMaskDlg.h"
)
source_group("Dialogs\\Processing" FILES ${Dialogs__Processing})

set(Dialogs__Registering
    "AskRegistering.cpp"
    "AskRegistering.h"
    "RegisterSettings.cpp"
    "RegisterSettings.h"
)
source_group("Dialogs\\Registering" FILES ${Dialogs__Registering})

set(Dialogs__Settings
    "AlignmentParameters.cpp"
    "AlignmentParameters.h"
    "CometStacking.cpp"
    "CometStacking.h"
    "IntermediateFiles.cpp"
    "IntermediateFiles.h"
    "OutputTab.cpp"
    "OutputTab.h"
    "PostCalibration.cpp"
    "PostCalibration.h"
    "RawDDPSettings.cpp"
    "RawDDPSettings.h"
    "ResultParameters.cpp"
    "ResultParameters.h"
    "StackingParameters.cpp"
    "StackingParameters.h"
    "StackSettings.cpp"
    "StackSettings.h"
)
source_group("Dialogs\\Settings" FILES ${Dialogs__Settings})

set(Dialogs__Stacking
    "BackgroundOptions.cpp"
    "BackgroundOptions.h"
    "BatchStacking.cpp"
    "BatchStacking.h"
    "CheckAbove.cpp"
    "CheckAbove.h"
    "DropFilesDlg.cpp"
    "DropFilesDlg.h"
    "flatchart.cpp"
    "flatchart.h"
    "ImageProperties.cpp"
    "ImageProperties.h"
    "picturelist.cpp"
    "picturelist.h"
    "QualityChart.cpp"
    "QualityChart.h"
    "RecommendedSettings.cpp"
    "RecommendedSettings.h"
    "RenameGroup.cpp"
    "RenameGroup.h"
    "SaveEditChanges.cpp"
    "SaveEditChanges.h"
    "StackingDlg.cpp"
    "StackingDlg.h"
    "StackRecap.cpp"
    "StackRecap.h"
)
source_group("Dialogs\\Stacking" FILES ${Dialogs__Stacking})

set(Header_Files
    "BaseDialog.h"
    "DeepSkyStacker.h"
    "Delaunay.h"
    "dslr.h"
    "DSS-versionhelpers.h"
    "DSS-winapifamily.h"
    "DSS-winpackagefamily.h"
    "editstars.h"
    "../DeepSkyStackerKernel/histogram.h"
    "lowerdockwidget.h"
    "mrulist.h"
    "pch.h"
    "selectrect.h"
    "toolbar.h"

)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "BaseDialog.cpp"
    "DeepSkyStacker.cpp"
    "Delaunay.cpp"
    "editstars.cpp"
    "lowerdockwidget.cpp"
    "selectrect.cpp"
    "toolbar.cpp"
)
if (LINUX)
    set(Source_Files ${Source_Files}
    "DeepSkyStacker.desktop"
    "DeepSkyStacker-uninstall.desktop"
    )
endif()
source_group("Source Files" FILES ${Source_Files})

set(Ui_Files
	"ui/About.ui"
	"ui/AlignmentParameters.ui"
	"ui/AskRegistering.ui"
	"ui/BackgroundOptions.ui"
	"ui/BatchStacking.ui"
	"ui/CheckAbove.ui"
	"ui/CometStacking.ui"
	"ui/DropFilesDlg.ui"
	"ui/ExplorerBar.ui"
    "ui/FlatChart.ui"
	"ui/ImageProperties.ui"
	"ui/IntermediateFiles.ui"
    "ui/OldProgressDlg.ui"
	"ui/OutputTab.ui"
	"ui/PictureList.ui"
	"ui/PostCalibration.ui"
	"ui/ProcessingControls.ui"
	"ui/ProcessingDlg.ui"
	"ui/ProcessingSettingsDlg.ui"
    "ui/ProgressDlg.ui"
	"ui/QualityChart.ui"
	"ui/RawDDPSettings.ui"
	"ui/RecommendedSettings.ui"
	"ui/RegisterSettings.ui"
	"ui/RenameGroup.ui"
	"ui/ResultParameters.ui"
	"ui/SaveEditChanges.ui"
	"ui/StackingDlg.ui"
	"ui/StackingParameters.ui"
	"ui/StackRecap.ui"
	"ui/StackSettings.ui"
	"ui/StarMaskDlg.ui"
	)
	
source_group("Qt\\ui" FILES ${Ui_Files})

set(i18n_Files
	"i18n/DeepSkyStacker_ca.ts"
	"i18n/DeepSkyStacker_cs.ts"
	"i18n/DeepSkyStacker_de.ts"
	"i18n/DeepSkyStacker_en.ts"
	"i18n/DeepSkyStacker_en_GB.ts"
    "i18n/DeepSkyStacker_es.ts"
	"i18n/DeepSkyStacker_fr.ts"
	"i18n/DeepSkyStacker_it.ts"
	"i18n/DeepSkyStacker_ja_JP.ts"
	"i18n/DeepSkyStacker_nl.ts"
	"i18n/DeepSkyStacker_pt_BR.ts"
	"i18n/DeepSkyStacker_ro.ts"
	"i18n/DeepSkyStacker_ru.ts"
	"i18n/DeepSkyStacker_tr.ts"
	"i18n/DeepSkyStacker_zh_CN.ts"
	"i18n/DeepSkyStacker_zh_TW.ts"
	)
	
source_group("Qt\\i18n" FILES ${i18n_Files})

set(Resources
    "res/4Corners.bmp"
    "res/4Corners.png"
    "res/asinh.bmp"
    "res/BiasColour.png"
    "res/BiasColour@2x.png"
    "res/BiasCYMG.png"
    "res/BiasCYMG@2x.png"
    "res/BiasGreyscale.png"
    "res/BiasGreyscale@2x.png"
    "res/BiasRGB.png"
    "res/BiasRGB@2x.png"
    "res/brazil.png"
    "res/ButtonComet_Disabled.png"
    "res/ButtonComet_Down.png"
    "res/ButtonComet_Hot.png"
    "res/ButtonComet_Up.png"
    "res/ButtonSave_Disabled.png"
    "res/ButtonSave_Down.png"
    "res/ButtonSave_Hot.png"
    "res/ButtonSave_Up.png"
    "res/ButtonSelect_Disabled.png"
    "res/ButtonSelect_Down.png"
    "res/ButtonSelect_Hot.png"
    "res/ButtonSelect_Up.png"
    "res/ButtonStar_Disabled.png"
    "res/ButtonStar_Down.png"
    "res/ButtonStar_Hot.png"
    "res/ButtonStar_Up.png"
    "res/Calibration_Linear.bmp"
    "res/Calibration_Rational.bmp"
    "res/Catalan.png"
    "res/Comet_Freeze.bmp"
    "res/Comet_Normal.bmp"
    "res/Comet_Trails.bmp"
    "res/cuberoot.bmp"
    "res/CurveMethods.pspimage"
    "res/custommode.bmp"
    "res/Czech.png"
    "res/DarkColour.png"
    "res/DarkColour@2x.png"
    "res/DarkCYMG.png"
    "res/DarkCYMG@2x.png"
    "res/DarkGreyscale.png"
    "res/DarkGreyscale@2x.png"
    "res/DarkRGB.png"
    "res/DarkRGB@2x.png"
    "res/Dutch.png"
    "res/FlatColour.png"
    "res/FlatColour@2x.png"
    "res/FlatCYMG.png"
    "res/FlatCYMG@2x.png"
    "res/FlatGreyscale.png"
    "res/FlatGreyscale@2x.png"
    "res/FlatRGB.png"
    "res/FlatRGB@2x.png"
    "res/GammaOff.bmp"
    "res/GammaOn.bmp"
    "res/German.png"
    "res/hdrdown.bmp"
    "res/hdrup.bmp"
    "res/Heart.png"
    "res/ImageList_CFA_CYMG.bmp"
    "res/ImageList_CFA_RGB.bmp"
    "res/ImageList_Color.bmp"
    "res/ImageList_GrayScale.bmp"
    "res/ImageList_Mask.bmp"
    "res/intersectionmode.bmp"
    "res/Italian.png"
    "res/LightColour.png"
    "res/LightColour@2x.png"
    "res/LightCYMG.png"
    "res/LightCYMG@2x.png"
    "res/LightGreyscale.png"
    "res/LightGreyscale@2x.png"
    "res/LightRGB.png"
    "res/LightRGB@2x.png"
    "res/linear.bmp"
    "res/linear_en.bmp"
    "res/linear_fr.bmp"
    "res/log.bmp"
    "res/loglog.bmp"
    "res/logo.png"
    "res/logsqrt.bmp"
    "res/MaskStarShapes.pspimage"
    "res/mosaicmode.bmp"
    "res/normalmode.bmp"
    "res/Pattern_BGGR.bmp"
    "res/Pattern_CYGMCYMG.bmp"
    "res/Pattern_GBRG.bmp"
    "res/Pattern_GRBG.bmp"
    "res/Pattern_RGGB.bmp"
    "res/Portugal.png"
    "res/prc.png"
    "res/RGBCalibration_Maximum.bmp"
    "res/RGBCalibration_Middle.bmp"
    "res/RGBCalibration_Minimum.bmp"
    "res/RGBCalibration_None.bmp"
    "res/Romanian.png"
    "res/Russian.png"
    "res/Spanish.png"
    "res/sqrt.bmp"
    "res/StarShape_Bell.bmp"
    "res/StarShape_Cone.bmp"
    "res/StarShape_Cubic.bmp"
    "res/StarShape_Quadratic.bmp"
    "res/StarShape_TruncatedBell.bmp"
    "res/StarShape_TruncatedCone.bmp"
    "res/Taiwan.png"
    "res/Turkish.png"
	)
if(WIN32)
    set(Resources ${Resources}
	"Icon/DeepSkyStacker.ico"
    )
endif()


set (Qt_Resource_Files
    "resources.qrc"
    )
	
if(WIN32)
    set(Resource_Files 
    "Icon.rc"
	"Version.rc"
    )
endif()

	
source_group("Resources" FILES ${Resources}	${Qt_Resource_Files} ${Resource_Files})

set(Tools
    "../Tools/imageview.cpp"
    "../Tools/griddata.cpp"
    "../Tools/QElidedLabel.cpp"
    "../Tools/QLinearGradientCtrl.cpp"
)
source_group("Tools" FILES ${Tools})

if(APPLE)
    # And the following tells CMake where to find and install the file itself.
    set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/Icon/DeepSkyStacker.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    set(file_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/Icon/DSS_filelist.icns")
    set_source_files_properties(${file_icon_macos} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
endif()

set(Help_Files
    "${CMAKE_SOURCE_DIR}/Help/Aide DeepSkyStacker.chm"
    "${CMAKE_SOURCE_DIR}/Help/Ayuda DeepSkyStacker.chm"
    "${CMAKE_SOURCE_DIR}/Help/DeepSkyStacker Ajuda.chm"
    "${CMAKE_SOURCE_DIR}/Help/DeepSkyStacker Help.chm"
    "${CMAKE_SOURCE_DIR}/Help/DeepSkyStacker Hilfe.chm"
	)
source_group("Help" FILES ${Help_Files})

set(ALL_FILES
    ${Dialogs}
    ${Dialogs__Processing}
    ${Dialogs__Registering}
    ${Dialogs__Settings}
    ${Dialogs__Stacking}
    ${Header_Files}
	${Ui_Files}
	${Resources}
    ${Qt_Resource_Files}
    ${Resource_Files}
    ${Source_Files}
    ${Tools}
    ${DSSCL}
    ${app_icon_macos}
	${file_icon_macos}
)

################################################################################
# Target
################################################################################
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES ca cs de es fr it ja_JP nl pt_BR ro ru tr zh_CN zh_TW)
qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR i18n
    PLURALS_TS_FILE i18n/${PROJECT_NAME}_en.ts
    PLURALS_TS_FILE i18n/${PROJECT_NAME}_en_GB.ts
    )
qt_add_resources(ALL_FILES ${Qt_Resource_Files})

qt_add_executable(DeepSkyStacker ${ALL_FILES})

if(APPLE)
    set_target_properties(DeepSkyStacker PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_BUNDLE_NAME "DeepSkyStacker"
        MACOSX_BUNDLE_ICON_FILE "DeepSkyStacker.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.deepskystacker.deepskystacker"
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2018-2025, David C. Partridge; Copyright © 2006-2019, Luc Coiffier"
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in"
        INSTALL_RPATH "@executable_path/../Frameworks"
)
endif()
if(WIN32)
    set_target_properties(DeepSkyStacker PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

set_target_properties(DeepSkyStacker PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Include directories
################################################################################
target_include_directories(DeepSkyStacker PUBLIC
    .
    ../DeepSkyStackerKernel
    GeneratedFiles/$<CONFIG>
    GeneratedFiles
    ../ZClass
    ../Tools
    ../qwt-6.3.0
  )

if(WIN32)
target_include_directories(${PROJECT_NAME} PUBLIC
   "$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
)
endif()

target_precompile_headers(DeepSkyStacker PRIVATE
	pch.h
	)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(DeepSkyStacker PRIVATE
  "$<$<CONFIG:Debug>:Z_TRACE_DEVELOP>"
  "$<$<CONFIG:Release>:NDEBUG>"
  NOMINMAX
  LIBRAW_NODLL
  _CRT_SECURE_NO_DEPRECATE
  USE_LIBTIFF_STATIC
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(DeepSkyStacker PRIVATE
        $<$<CONFIG:Debug>:
            /Gm-;
            /Zi
        >
        $<$<CONFIG:Release>:
            /Zi;
            /GS-
        >
	    -W4 /wd4244 /wd4652 /wd4702 /wd4828 
		-Zc:__cplusplus /openmp:experimental
        /MP;
        /EHsc
        /utf-8
    )
    target_link_options(DeepSkyStacker PRIVATE
        $<$<CONFIG:Debug>:
            /PROFILE
        >
        $<$<CONFIG:Release>:
            /OPT:ICF
        >
        /DEBUG:FULL;
        /MACHINE:X64;
        /SUBSYSTEM:WINDOWS;
        /DYNAMICBASE:NO
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(APPLE)
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Xclang -fopenmp
        )
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE
            -fopenmp
        )
    endif()
endif()

if(WIN32)
target_link_directories(DeepSkyStacker PRIVATE
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
	)
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(DeepSkyStacker PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::OpenGLWidgets
    Qt6::OpenGL
    Qt6::Concurrent
    Qt6::Svg
    Qt6::PrintSupport
	DeepSkyStackerKernel
    DeepSkyStackerAVX
    ZClass
    qwt-6.3.0
)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::container)
target_link_libraries(${PROJECT_NAME} PRIVATE Exiv2::exiv2lib)
target_link_libraries(${PROJECT_NAME} PRIVATE expat::expat)
target_link_libraries(${PROJECT_NAME} PRIVATE libraw::raw_r)
target_link_libraries(${PROJECT_NAME} PRIVATE CFITSIO::cfitsio)
target_link_libraries(${PROJECT_NAME} PRIVATE TIFF::TIFF)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
if(LINUX)
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenMP::OpenMP_CXX
    Threads::Threads
)
endif()
if(APPLE)
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenMP::OpenMP_CXX
    CURL::libcurl
)
endif()

if(APPLE)
target_link_directories(${PROJECT_NAME} PRIVATE /usr/local/lib)
endif()

if(WIN32)
set(ADDITIONAL_LIBRARY_DEPENDENCIES ${ADDITIONAL_LIBRARY_DEPENDENCIES}
    user32
    psapi
    htmlhelp
)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if(APPLE)
    add_custom_command(TARGET DeepSkyStacker
        POST_BUILD
        #
        # Now we've built the DSS .app bundle, copy the DSSCL binary into the bundle
        # The use of $<TARGET_FILE:DeepSkyStackerCL> in add_custom_command also adds
        # a dependency on the target, so that the DSSCL binary is built before this.
        #
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_BUNDLE_CONTENT_DIR:DeepSkyStacker>/MacOS/ $<TARGET_FILE:DeepSkyStackerCL> 
    	COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_BUNDLE_CONTENT_DIR:DeepSkyStacker>/MacOS/ ${CMAKE_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/tools/uchmviewer/uchmviewer
	    #
	    # Copy the InstallBuilder project file
	    #
	    COMMAND ${CMAKE_COMMAND} -E copy -t $ENV{HOME}/build ${CMAKE_SOURCE_DIR}/Installers/macOS/DeepSkyStacker.xml
	    #
	    # Copy the license file and readme file
	    #
        COMMAND ${CMAKE_COMMAND} -E copy -t $ENV{HOME}/build ${CMAKE_SOURCE_DIR}/LICENSE
        COMMAND ${CMAKE_COMMAND} -E copy -t $ENV{HOME}/build ${CMAKE_SOURCE_DIR}/Installers/README.txt
	    #
    	# Copy the help files
    	#
    	COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_BUNDLE_CONTENT_DIR:DeepSkyStacker>/Resources ${Help_Files}
    )
endif()
if(LINUX)
    add_custom_command(TARGET DeepSkyStacker
        POST_BUILD
        #
        # Copy the .desktop files, mime-type definition file and the icon files
        #
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR}/DeepSkyStacker.desktop
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR}/DeepSkyStacker-uninstall.desktop
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR}/com.github.deepskystacker-x-text-dssfilelist.xml
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR}/Icon/DeepSkyStacker.icns
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR}/Icon/DSS_filelist64x64.png
	    #
	    # Copy the InstallBuilder project file
	    #
	    COMMAND ${CMAKE_COMMAND} -E copy -t $ENV{HOME}/build ${CMAKE_SOURCE_DIR}/Installers/Linux/DeepSkyStacker.xml
	    #
	    # Copy the license file and readme file
	    #
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_SOURCE_DIR}/LICENSE
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_SOURCE_DIR}/Installers/README.txt
	    #
        # Copy the help files
        #
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/Help
	    COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX}/Help ${Help_Files} 
    )
endif()
if(WIN32)
    add_custom_command(TARGET DeepSkyStacker
        POST_BUILD
        #
        # Copy the .pdb
        #
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/DeepSkyStacker.pdb
	    #
	    # Copy the license file and readme file
	    #
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_SOURCE_DIR}/LICENSE
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_SOURCE_DIR}/Installers/README.txt
	    #
    	# Copy the help files
    	#
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/Help
	    COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX}/Help ${Help_Files} 
    )
endif()

if(NOT LINUX)
    set (deploy_tool_options_arg "-verbose=1")
    if(APPLE)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} -hardened-runtime -timestamp -no-strip")
        set(deploy_tool_options_arg "${deploy_tool_options_arg} \"-executable=$<TARGET_BUNDLE_CONTENT_DIR:DeepSkyStacker>/MacOS/DeepSkyStackerCL\"")
        set(deploy_tool_options_arg "${deploy_tool_options_arg} \"-executable=$<TARGET_BUNDLE_CONTENT_DIR:DeepSkyStacker>/MacOS/uchmviewer\"")
        set(deploy_tool_options_arg "${deploy_tool_options_arg} \"-codesign=Developer ID Application: David Partridge (VH8AYT3C7Y)\"")
    elseif(WIN32)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --compiler-runtime --pdb")
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --dir . --plugindir plugins")
    endif()

    # Generate a deployment script to be executed at install time
    # App bundles on macOS have an .app suffix
    if(APPLE)
        set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStacker>.app")
    else()
        message ("Target filename:"  $<TARGET_FILE_NAME:DeepSkyStacker>)
        set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStacker>")
    endif()

    message ("executable_path: " ${executable_path})
    message ("deploy tools options arg: " ${deploy_tool_options_arg})
     qt_generate_deploy_script(
         TARGET DeepSkyStacker
         OUTPUT_SCRIPT deploy_script
         CONTENT "
     qt_deploy_runtime_dependencies(
         EXECUTABLE \"${executable_path}\"
         DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
     )"
     )
else()
    qt_generate_deploy_app_script(
        TARGET ${PROJECT_NAME}
        OUTPUT_SCRIPT deploy_script
        DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
    )
endif()

if(APPLE)
install (CODE "execute_process(COMMAND \"$ENV{HOME}/.unlock_keychain\")")
endif()

install (SCRIPT ${deploy_script})
install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)

#[=[

if(WIN32)
################################################################################
# Post build events
################################################################################
set (deployment_string
        ${WINDEPLOYQT_EXECUTABLE} " --verbose 2 --dir . --plugindir plugins "
        "--compiler-runtime --pdb "
        $<TARGET_FILE_NAME:DeepSkyStacker>
    )
message ("Deployment command: " ${deployment_string})

ADD_CUSTOM_COMMAND(
  TARGET DeepSkyStacker
  POST_BUILD
  COMMAND
    ${WINDEPLOYQT_EXECUTABLE} --verbose 2 --dir . --plugindir plugins
        --compiler-runtime --pdb
        $<TARGET_FILE_NAME:DeepSkyStacker>
       
    )
endif()
]=]

