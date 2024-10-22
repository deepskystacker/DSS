set(PROJECT_NAME DeepSkyStacker)

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
    "Histogram.h"
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
    "Delaunay.h"
    "dslr.h"
    "DSS-versionhelpers.h"
    "DSS-winapifamily.h"
    "DSS-winpackagefamily.h"
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
    "StdAfx.cpp"
    "toolbar.cpp"
)
source_group("Source Files" FILES ${Source_Files})

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
    ${Source_Files}
    ${Tools}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
################################################################################
# Includes for CMake from *.props
################################################################################
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "$ENV{QtMsBuild}/qt.cmake")
use_props(${PROJECT_NAME} Debug   "../Debug.cmake")
use_props(${PROJECT_NAME} Release "../Release.cmake")

set(ROOT_NAMESPACE DeepSkyStacker)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "QtVS_v303"
)
################################################################################
# Target name
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    TARGET_NAME_DEBUG   "${OUTPUT_DIRECTORY}${TARGET_NAME}$ENV{TargetExt}"
    TARGET_NAME_RELEASE "${OUTPUT_DIRECTORY}${TARGET_NAME}$ENV{TargetExt}"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "FALSE"
)
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
    $<$<CONFIG:Debug>:
        MultiThreadedDebugDLL
    >
    $<$<CONFIG:Release>:
        MultiThreadedDLL
    >
    $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/.;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../DeepSkyStackerKernel;"
    "${CMAKE_CURRENT_SOURCE_DIR}/GeneratedFiles/$<CONFIG>;"
    "${CMAKE_CURRENT_SOURCE_DIR}/GeneratedFiles;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LibRaw;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ZCLass;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Tools;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LibTiff;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../CFitsIO;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Zlib;"
    "$ENV{Boost_1_80_0}"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG;"
        "Z_TRACE_DEVELOP"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "_UNICODE;"
    "UNICODE;"
    "NOMINMAX;"
    "LIBRAW_NODLL;"
    "WIN32;"
    "QT_CORE_LIB;"
    "QT_GUI_LIB;"
    "QT_WIDGETS_LIB;"
    "_CRT_SECURE_NO_DEPRECATE;"
    "USE_LIBTIFF_STATIC;"
    "_MBCS"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Gm-;
            /Zi
        >
        $<$<CONFIG:Release>:
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /GS-
        >
        /MP;
        /GL-;
        /W4;
        -Zc:__cplusplus;
        /openmp:experimental;
        /wd4828;
        /wd4652;
        /wd4702;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /NODEFAULTLIB:MSVCRT;
            /NODEFAULTLIB:libcmt;
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

################################################################################
# Post build events
################################################################################
add_custom_command_if(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMANDS
    COMMAND   $<CONFIG:Debug> call $ENV{QtToolsPath}\\qtenv2.bat
    COMMAND   $<CONFIG:Debug> call "$ENV{VCInstallDir}Auxiliary\\Build\\vcvars64.bat"
    COMMAND   $<CONFIG:Debug> $ENV{QtToolsPath}\\windeployqt --pdb $<SHELL_PATH:$<TARGET_FILE:${PROJECT_NAME}>>
    COMMAND $<CONFIG:Release> call $ENV{QtToolsPath}\\qtenv2.bat
    COMMAND $<CONFIG:Release> call "$ENV{VCInstallDir}Auxiliary\\Build\\vcvars64.bat"
    COMMAND $<CONFIG:Release> $ENV{QtToolsPath}\\windeployqt $<SHELL_PATH:$<TARGET_FILE:${PROJECT_NAME}>>
    COMMENT "Add necessary Qt stuff to the output directory"
)

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    DeepSkyStackerKernel
    libraw
    libtiff
    ZCLass
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:"
        "exiv2d;"
        "libexpatd;"
        "zlibstaticd"
    ">"
    "$<$<CONFIG:Release>:"
        "exiv2;"
        "libexpat;"
        "zlibstatic"
    ">"
    "gdi32;"
    "gdiplus;"
    "psapi;"
    "$(Qt_LIBS_);"
    "htmlhelp;"
    "cfitsio"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x64"
)

