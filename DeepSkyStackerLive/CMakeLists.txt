set(PROJECT_NAME DeepSkyStackerLive)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "dssliveenums.h"
    "LiveSettings.h"
    "resource.h"
    "stdafx.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Header_Files__SmtpMime
    "../include/SmtpMime/emailaddress.h"
    "../include/SmtpMime/mimeattachment.h"
    "../include/SmtpMime/mimebase64encoder.h"
    "../include/SmtpMime/mimebase64formatter.h"
    "../include/SmtpMime/mimebytearrayattachment.h"
    "../include/SmtpMime/mimecontentencoder.h"
    "../include/SmtpMime/mimefile.h"
    "../include/SmtpMime/mimehtml.h"
    "../include/SmtpMime/mimeinlinefile.h"
    "../include/SmtpMime/mimemessage.h"
    "../include/SmtpMime/mimemultipart.h"
    "../include/SmtpMime/mimepart.h"
    "../include/SmtpMime/mimeqpencoder.h"
    "../include/SmtpMime/mimeqpformatter.h"
    "../include/SmtpMime/mimetext.h"
    "../include/SmtpMime/quotedprintable.h"
    "../include/SmtpMime/SmtpMime"
    "../include/SmtpMime/smtpmime_global.h"
)
source_group("Header Files\\SmtpMime" FILES ${Header_Files__SmtpMime})

set(Source_Files
    "ChartTab.cpp"
    "DeepSkyStackerLive.cpp"
    "EmailSettings.cpp"
    "fileregistrar.cpp"
    "filestacker.cpp"
    "foldermonitor.cpp"
    "footprint.cpp"
    "imageviewer.cpp"
    "LiveSettings.cpp"
    "progresslive.cpp"
    "RestartMonitoring.cpp"
    "settingstab.cpp"
    "stdafx.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(Tools
    "../Tools/imageview.cpp"
    "../Tools/QLinearGradientCtrl.cpp"
)
source_group("Tools" FILES ${Tools})

set(ALL_FILES
    ${Header_Files}
    ${Header_Files__SmtpMime}
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
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "$ENV{QtMsBuild}/qt_defaults.cmake")
use_props(${PROJECT_NAME} Debug   "$ENV{QtMsBuild}/Qt.cmake")
use_props(${PROJECT_NAME} Debug   "../Debug.cmake")
use_props(${PROJECT_NAME} Release "$ENV{QtMsBuild}/Qt.cmake")
use_props(${PROJECT_NAME} Release "../Release.cmake")

set(ROOT_NAMESPACE DeepSkyStackerLive)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "QtVS_v304"
)
################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
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
    "${CMAKE_CURRENT_SOURCE_DIR}/../DeepSkyStackerKernel;"
    "${CMAKE_CURRENT_SOURCE_DIR}/.;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ChartCtrl;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../SMTP;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ZCLass;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Tools;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LibTiff;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LibRaw;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../CFitsIO;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../zlib;"
    "$ENV{Boost_1_80_0}"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "Z_TRACE_DEVELOP"
    ">"
    "$ENV{Qt_DEFINES_};"
    "_AFXDLL;"
    "NOMINMAX;"
    "LIBRAW_NODLL;"
    "USE_LIBTIFF_STATIC;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /RTC1
        >
        $<$<CONFIG:Release>:
            /GL-;
            /GS-
        >
        /W4;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        -Zc:__cplusplus;
        /openmp:experimental;
        /wd4828;
        /wd4652;
        /wd4702;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    string(CONCAT FILE_CL_OPTIONS
        "$<$<CONFIG:Debug>:"
            "/WX-"
        ">"
    )
    source_file_compile_options(stdafx.cpp ${FILE_CL_OPTIONS})
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /NODEFAULTLIB:MSVCRT
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
    "$(Qt_LIBS_);"
    "psapi;"
    "htmlhelp;"
    "cfitsio;"
    "SmtpMime"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

