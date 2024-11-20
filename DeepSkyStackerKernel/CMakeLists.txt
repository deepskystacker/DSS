set(PROJECT_NAME DeepSkyStackerKernel)
message("Configuring project: " ${PROJECT_NAME})
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

qt_standard_project_setup()

set(COMPILE_WARNING_AS_ERROR YES)

set(CMAKE_AUTORCC ON)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "AHDDemosaicing.h"
    "avx_includes.h"
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
    "avx_simd_check.h"
    "avx_simd_factory.h"
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
    "BezierAdjust.h"
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
    "ImageListModel.h"
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
    "StarMask.h"
    "Stars.h"
    "stdafx.h"
    "TaskInfo.h"
    "TIFFUtil.h"
    "tracecontrol.h"
    "Workspace.h"
)
if(WIN32)
set(Header_Files
    ${Header_Files}
    "StackWalker.h"
)
endif()
source_group("Header Files" FILES ${Header_Files})

set(i18n_Files
	"i18n/DeepSkyStackerKernel_ca.ts"
	"i18n/DeepSkyStackerKernel_cs.ts"
	"i18n/DeepSkyStackerKernel_de.ts"
	"i18n/DeepSkyStackerKernel_en.ts"
	"i18n/DeepSkyStackerKernel_es.ts"
	"i18n/DeepSkyStackerKernel_fr.ts"
	"i18n/DeepSkyStackerKernel_it.ts"
	"i18n/DeepSkyStackerKernel_ja_JP.ts"
	"i18n/DeepSkyStackerKernel_nl.ts"
	"i18n/DeepSkyStackerKernel_pt_BR.ts"
	"i18n/DeepSkyStackerKernel_ro.ts"
	"i18n/DeepSkyStackerKernel_ru.ts"
	"i18n/DeepSkyStackerKernel_tr.ts"
	"i18n/DeepSkyStackerKernel_zh_CN.ts"
	"i18n/DeepSkyStackerKernel_zh_TW.ts"
	)
	
source_group("Qt\\i18n" FILES ${i18n_Files} "i18n/translations_kernel.qrc"
)

set(AVX_Files
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
)

set(Source_Files
    "AHDDemosaicing.cpp"
    "avx_simd_check.cpp"
    ${AVX_Files}
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
    "StarMask.cpp"
    "TaskInfo.cpp"
    "TIFFUtil.cpp"
    "tracecontrol.cpp"
    "Workspace.cpp"
)
if(WIN32)
    set (Source_Files
        ${Source_Files}
        "StackWalker.cpp"
    )
endif()
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
	${i18n_Files} "i18n/translations_kernel.qrc"
	${Source_Files}
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
set_source_files_properties(${AVX_Files}
    PROPERTIES
        COMPILE_OPTIONS "-mavx;-mavx2;-mfma;-mssse3;-msse4.2")
set_source_files_properties(avx_simd_check.cpp
    PROPERTIES
        COMPILE_OPTIONS "-mxsave")
endif()

################################################################################
# Target
################################################################################
qt_add_library(DeepSkyStackerKernel STATIC ${ALL_FILES})

set_source_files_properties(${i18n_Files}
     PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/i18n")

qt_add_lrelease(TS_FILES ${i18n_Files})

target_link_libraries(DeepSkyStackerKernel PRIVATE Qt6::Widgets Qt6::Network)

################################################################################
# Output directory
################################################################################
set_target_properties(DeepSkyStackerKernel PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
)
set_target_properties(DeepSkyStackerKernel PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
    .
    ../DeepSkyStacker;
    ../ZClass
    ../LibRaw
    ../Tools
)

if(WIN32)
target_include_directories(${PROJECT_NAME} PRIVATE
    ../include
    ../LibTiff
    ../Zlib
 	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
    "$ENV{Boost_1_80_0}"
)
endif()


target_precompile_headers(DeepSkyStackerKernel PRIVATE
	stdafx.h
	)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
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
    "_CONSOLE;"
    "_CRT_SECURE_NO_DEPRECATE"
)

if (MSVC)
target_compile_definitions(${PROJECT_NAME} PRIVATE
	_WINDOWS
	_UNICODE
	UNICODE
	)
endif()

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
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fopenmp
    )
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
#[=[
target_link_libraries(${PROJECT_NAME} PRIVATE
    libraw
    libtiff
    ZClass
)
]=]

