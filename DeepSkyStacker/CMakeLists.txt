set(PROJECT_NAME DeepSkyStacker)

set(COMPILE_WARNING_AS_ERROR YES)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Widgets Network Charts)
qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

################################################################################
# Source groups
################################################################################
set(Dialogs
    "About.cpp"
    "ExplorerBar.cpp"
)
source_group("Dialogs" FILES ${Dialogs})

set(Dialogs__Processing
    "BezierAdjust.h"
    "processingcontrols.cpp"
    "ProcessingDlg.cpp"
    "ProcessingSettings.h"
    "ProcessingSettingsDlg.cpp"
    "RGBTab.cpp"
    "SavePicture.cpp"
    "StarMaskDlg.cpp"
)
source_group("Dialogs\\Processing" FILES ${Dialogs__Processing})

set(Dialogs__Registering
    "AskRegistering.cpp"
    "RegisterSettings.cpp"
)
source_group("Dialogs\\Registering" FILES ${Dialogs__Registering})

set(Dialogs__Settings
    "AlignmentParameters.cpp"
    "CometStacking.cpp"
    "IntermediateFiles.cpp"
    "OutputTab.cpp"
    "PostCalibration.cpp"
    "RawDDPSettings.cpp"
    "ResultParameters.cpp"
    "StackingParameters.cpp"
    "StackSettings.cpp"
)
source_group("Dialogs\\Settings" FILES ${Dialogs__Settings})

set(Dialogs__Stacking
    "BackgroundOptions.cpp"
    "BatchStacking.cpp"
    "CheckAbove.cpp"
    "DropFilesDlg.cpp"
    "ImageProperties.cpp"
    "picturelist.cpp"
    "RecommendedSettings.cpp"
    "RenameGroup.cpp"
    "SaveEditChanges.cpp"
    "StackingDlg.cpp"
    "StackRecap.cpp"
)
source_group("Dialogs\\Stacking" FILES ${Dialogs__Stacking})

set(Header_Files
    "../DeepSkyStackerKernel/histogram.h"
    "Delaunay.h"
    "dslr.h"
    "DSS-versionhelpers.h"
    "DSS-winapifamily.h"
    "DSS-winpackagefamily.h"
    "lowerdockwidget.cpp"
    "mrulist.h"
    "StdAfx.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "BaseDialog.cpp"
    "DeepSkyStacker.cpp"
    "Delaunay.cpp"
    "editstars.cpp"
    "progressdlg.cpp"
    "selectrect.cpp"
    "toolbar.cpp"
)
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
	"ui/ImageProperties.ui"
	"ui/IntermediateFiles.ui"
	"ui/OutputTab.ui"
	"ui/PictureList.ui"
	"ui/PostCalibration.ui"
	"ui/ProcessingControls.ui"
	"ui/ProcessingDlg.ui"
	"ui/ProcessingSettingsDlg.ui"
	"ui/ProgressDlg.ui"
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
	"i18n/DSS_ca.ts"
	"i18n/DSS_cs.ts"
	"i18n/DSS_de.ts"
	"i18n/DSS_en.ts"
	"i18n/DSS_es.ts"
	"i18n/DSS_fr.ts"
	"i18n/DSS_it.ts"
	"i18n/DSS_ja_JP.ts"
	"i18n/DSS_nl.ts"
	"i18n/DSS_pt_BR.ts"
	"i18n/DSS_ro.ts"
	"i18n/DSS_ru.ts"
	"i18n/DSS_tr.ts"
	"i18n/DSS_zh_CN.ts"
	"i18n/DSS_zh_TW.ts"
	)
	
source_group("Qt\\i18n" FILES ${i18n_Files} "i18n/translations.qrc"
    )

set(Resources
	"Icon.rc"
	"Icon/app.ico"
	"resources.qrc"
	"Version.rc"
	)
	
source_group("Resources" FILES ${Resources}	)

set(Tools
    "../Tools/imageview.cpp"
    "../Tools/QElidedLabel.cpp"
    "../Tools/QLinearGradientCtrl.cpp"
)
source_group("Tools" FILES ${Tools})

set(ALL_FILES
    ${Dialogs}
    ${Dialogs__Processing}
    ${Dialogs__Registering}
    ${Dialogs__Settings}
    ${Dialogs__Stacking}
    ${Header_Files}
	${Ui_Files}
	${i18n_Files} "i18n/translations.qrc"
	${Resources}
    ${Source_Files}
    ${Tools}
)

################################################################################
# Target
################################################################################
qt_add_executable(DeepSkyStacker ${ALL_FILES})

set_source_files_properties(${i18n_Files}
     PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/i18n")

qt_add_lrelease(TS_FILES ${i18n_Files})

set_target_properties(DeepSkyStacker PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/"
)
set_target_properties(DeepSkyStacker PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
set_target_properties(DeepSkyStacker PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

#[=[ 
# Well that was a waste of time - - only gets run at install
#
# Set options for the deployment script
#
set(deploy_tool_options_arg "--verbose 1")
if(APPLE)
    set(deploy_tool_options_arg ${deploy_tool_options_arg} --hardened-runtime)
elseif(WIN32)
    set(deploy_tool_options_arg ${deploy_tool_options_arg} --pdb --compiler-runtime)
endif()

# Generate a deployment script to be executed at install time
 qt_generate_deploy_script(
     TARGET DeepSkyStacker
     OUTPUT_SCRIPT deploy_script
     CONTENT "
 qt_deploy_runtime_dependencies(
     EXECUTABLE \"${executable_path}\"
     GENERATE_QT_CONF
     VERBOSE
 )")
 ]=]

set(ROOT_NAMESPACE DeepSkyStacker)

################################################################################
# Target name
################################################################################
#set_target_properties(DeepSkyStacker PROPERTIES
#    TARGET_NAME_DEBUG   "${OUTPUT_DIRECTORY}${TARGET_NAME}$ENV{TargetExt}"
#    TARGET_NAME_RELEASE "${OUTPUT_DIRECTORY}${TARGET_NAME}$ENV{TargetExt}"
#)
################################################################################
# Output directory
################################################################################



################################################################################
# Include directories
################################################################################
target_include_directories(DeepSkyStacker PUBLIC
    .
    ../DeepSkyStackerKernel
    GeneratedFiles/$<CONFIG>
    GeneratedFiles
    ../LibRaw
    ../ZClass
    ../Tools
  )

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_include_directories(${PROJECT_NAME} PUBLIC
   ../Zlib
   ../LibTiff
   ../CFitsIO
   ../include
   ../include/exiv2
   ../include/SmtpMime
   "$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
   "$ENV{Boost_1_80_0}"
)
endif()

target_precompile_headers(DeepSkyStacker PRIVATE
	stdafx.h
	)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(DeepSkyStacker PRIVATE
  "$<$<CONFIG:Debug>:Z_TRACE_DEVELOP>"
  "$<$<CONFIG:Release>:NDEBUG>"
  NOMINMAX
  LIBRAW_NODLL
  QT_CORE_LIB
  QT_GUI_LIB
  QT_WIDGETS_LIB
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
	    -W4 /wd4652 /wd4828 /wd4702;
		-Zc:__cplusplus /openmp:experimental
        /MP;
        /GL-;
        /EHsc
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
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_link_directories(DeepSkyStacker PRIVATE
	"$<$<CONFIG:Debug>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/DebugLibs>"
	"$<$<CONFIG:Release>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/ReleaseLibs>"
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
	)
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(DeepSkyStacker PRIVATE
	Qt6::Core
    Qt6::Widgets
	Qt6::Network
	DeepSkyStackerKernel
    libraw
    libtiff
    ZClass
)

set(DEBUG_DEPENDENCIES exiv2d libexpatd zlibstaticd)
set(RELEASE_DEPENDENCIES exiv2 libexpat zlibstatic)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
endif()

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:${DEBUG_DEPENDENCIES}>"
    "$<$<CONFIG:Release>:${RELEASE_DEPENDENCIES}>"
    "cfitsio"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(ADDITIONAL_LIBRARY_DEPENDENCIES ${ADDITIONAL_LIBRARY_DEPENDENCIES}
    user32
    psapi
    htmlhelp
)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

################################################################################
# Post build events
################################################################################
ADD_CUSTOM_COMMAND(
  TARGET DeepSkyStacker
  POST_BUILD
  COMMAND
    ${WINDEPLOYQT_EXECUTABLE} --pdb DeepSkyStacker.exe
       --verbose 1 --dir . --plugindir plugins --compiler-runtime
    )
