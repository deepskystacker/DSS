set(PROJECT_NAME DeepSkyStackerKernel)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "AHDDemosaicing.h"
    "avx.h"
    "avx_avg.h"
    "avx_bitmap_filler.h"
    "avx_cfa.h"
    "avx_entropy.h"
    "avx_filter.h"
    "avx_histogram.h"
    "avx_luminance.h"
    "avx_median.h"
    "avx_output.h"
    "avx_support.h"
    "BackgroundCalibration.h"
    "Bayer.h"
    "BilinearParameters.h"
    "BitmapBase.h"
    "BitmapCharacteristics.h"
    "BitmapConstants.h"
    "BitmapExt.h"
    "BitmapExtraInfo.h"
    "BitMapFiller.h"
    "BitmapInfo.h"
    "BitmapIterator.h"
    "BitmapPartFile.h"
    "cfa.h"
    "CFABitmapInfo.h"
    "ChannelAlign.h"
    "ColorBitmap.h"
    "ColorHelpers.h"
    "ColorMultiBitmap.h"
    "ColorRef.h"
    "CosmeticEngine.h"
    "DarkFrame.h"
    "DeBloom.h"
    "DeepStack.h"
    "dssbase.h"
    "DSSCommon.h"
    "DSSProgress.h"
    "dssrect.h"
    "DSSTools.h"
    "DSSVersion.h"
    "DynamicStats.h"
    "EntropyInfo.h"
    "ExceptionHandling.h"
    "ExtraInfo.h"
    "Filters.h"
    "FITSUtil.h"
    "FlatFrame.h"
    "FlatPart.h"
    "FrameInfo.h"
    "FrameInfoSupport.h"
    "FrameList.h"
    "GrayBitmap.h"
    "GreyMultiBitmap.h"
    "group.h"
    "histogram.h"
    "LinearInterpolationh.h"
    "MasterFrames.h"
    "MatchingStars.h"
    "matrix.h"
    "MedianFilterEngine.h"
    "MemoryBitmap.h"
    "MultiBitmap.h"
    "Multitask.h"
    "PixelTransform.h"
    "RationalInterpolation.h"
    "RAWUtils.h"
    "RegisterEngine.h"
    "RunningStackingEngine.h"
    "Settings.h"
    "SkyBackground.h"
    "StackedBitmap.h"
    "StackingEngine.h"
    "StackingTasks.h"
    "StackWalker.h"
    "StarMask.h"
    "Stars.h"
    "StdAfx.h"
    "TaskInfo.h"
    "TIFFUtil.h"
    "tracecontrol.h"
    "Workspace.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "AHDDemosaicing.cpp"
    "avx.cpp"
    "avx_avg.cpp"
    "avx_bitmap_filler.cpp"
    "avx_cfa.cpp"
    "avx_entropy.cpp"
    "avx_filter.cpp"
    "avx_histogram.cpp"
    "avx_luminance.cpp"
    "avx_output.cpp"
    "avx_support.cpp"
    "BackgroundCalibration.cpp"
    "Bayer.cpp"
    "BilinearParameters.cpp"
    "BitmapBase.cpp"
    "BitmapExt.cpp"
    "BitMapFiller.cpp"
    "BitmapInfo.cpp"
    "ChannelAlign.cpp"
    "ColorBitmap.cpp"
    "ColorHelpers.cpp"
    "ColorMultiBitmap.cpp"
    "CosmeticEngine.cpp"
    "DarkFrame.cpp"
    "DeBloom.cpp"
    "DeepStack.cpp"
    "DSSProgress.cpp"
    "DSSTools.cpp"
    "EntropyInfo.cpp"
    "ExceptionHandling.cpp"
    "Filters.cpp"
    "FITSUtil.cpp"
    "FlatFrame.cpp"
    "FrameInfo.cpp"
    "FrameList.cpp"
    "GrayBitmap.cpp"
    "GreyMultiBitmap.cpp"
    "group.cpp"
    "ImageListModel.cpp"
    "imageloader.cpp"
    "MasterFrames.cpp"
    "MatchingStars.cpp"
    "MedianFilterEngine.cpp"
    "MemoryBitmap.cpp"
    "MultiBitmapProcess.cpp"
    "Multitask.cpp"
    "QEventLogger.cpp"
    "RAWUtils.cpp"
    "RegisterCore.cpp"
    "RegisterEngine.cpp"
    "RunningStackingEngine.cpp"
    "Settings.cpp"
    "StackedBitmap.cpp"
    "StackingEngine.cpp"
    "StackingTasks.cpp"
    "StackWalker.cpp"
    "StarMask.cpp"
    "stdafx.cpp"
    "TaskInfo.cpp"
    "TIFFUtil.cpp"
    "tracecontrol.cpp"
    "Workspace.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
################################################################################
# Includes for CMake from *.props
################################################################################
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "$ENV{QtMsBuild}/qt_defaults.cmake")
use_props(${PROJECT_NAME} Debug   "../Debug.cmake")
use_props(${PROJECT_NAME} Debug   "$ENV{QtMsBuild}/Qt.cmake")
use_props(${PROJECT_NAME} Release "../Release.cmake")
use_props(${PROJECT_NAME} Release "$ENV{QtMsBuild}/Qt.cmake")

set(ROOT_NAMESPACE DeepSkyStackerKernel)

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
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
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
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Program Files (x86)/Visual Leak Detector/include"
    ">"
    "${CMAKE_CURRENT_SOURCE_DIR}/../DeepSkyStacker;"
    "${CMAKE_CURRENT_SOURCE_DIR}/GeneratedFiles/$<CONFIG>;"
    "${CMAKE_CURRENT_SOURCE_DIR}/GeneratedFiles;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../ZCLass;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../LibRaw;"
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
        "Z_DEVELOP"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG;"
        "USE_LIBTIFF_STATIC"
    ">"
    "_UNICODE;"
    "UNICODE;"
    "NOMINMAX;"
    "LIBRAW_NODLL;"
    "WIN32;"
    "_CONSOLE;"
    "_CRT_SECURE_NO_DEPRECATE;"
    "_MBCS"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Gm-;
            /RTC1
        >
        $<$<CONFIG:Release>:
            /GL-;
            /GS-
        >
        /MP;
        /W4;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        -Zc:__cplusplus;
        /openmp:experimental;
        /wd4828;
        /wd4652;
        /wd4702;
        /wd4244;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /NODEFAULTLIB:MSVCRT
        >
        $<$<CONFIG:Release>:
            /OPT:ICF
        >
        /MACHINE:X64;
        /SUBSYSTEM:CONSOLE;
        /DYNAMICBASE:NO
    )
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    libraw
    libtiff
    ZCLass
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "gdi32;"
    "gdiplus;"
    "zlibstatic"
)
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

