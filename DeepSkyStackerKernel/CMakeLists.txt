set(PROJECT_NAME DeepSkyStackerKernel)
message("Configuring project: " ${PROJECT_NAME})
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(COMPILE_WARNING_AS_ERROR YES)
set(CMAKE_AUTOMOC TRUE)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "AHDDemosaicing.h"
    "avx.h"
    "avx_avg.h"
    "avx_bitmap_filler.h"
    "avx_bitmap_util.h"
    "avx_cfa.h"
    "avx_entropy.h"
    "avx_filter.h"
    "avx_histogram.h"
    "avx_includes.h"
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
    "imageloader.h"
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
    "QMessageLogger.h"
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
    "pch.h"
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
elseif(NOT APPLE)
set(Header_Files
    ${Header_Files}
    "ExceptionHandling.h"
)
endif()
source_group("Header Files" FILES ${Header_Files})

set(i18n_Files
	"i18n/DeepSkyStackerKernel_ca.ts"
	"i18n/DeepSkyStackerKernel_cs.ts"
	"i18n/DeepSkyStackerKernel_de.ts"
	"i18n/DeepSkyStackerKernel_en.ts"
	"i18n/DeepSkyStackerKernel_en_GB.ts"
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
	
source_group("Qt\\i18n" FILES ${i18n_Files} 
)

#
# The files that contain AVX code that needs to be compiled with the AVX
# compiler flags and contain code that can ONLY be called when AVX is available
#
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
)

#
# The files that contain AVX code that needs to be compiled without the AVX
# compiler flags and contains code that can be called regardless of AVX availability
#
set(AVX_non-AVX_Files
    "avx_1.cpp"
    "avx_avg_1.cpp"
    "avx_bitmap_util.cpp"
    "avx_cfa_1.cpp"
    "avx_entropy_1.cpp"
    "avx_filter_1.cpp"
    "avx_histogram_1.cpp"
    "avx_luminance_1.cpp"
    "avx_simd_check.cpp"
    "avx_output_1.cpp"
)

set(Source_Files
    "AHDDemosaicing.cpp"
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
        "ExceptionHandling.cpp"
        "StackWalker.cpp"
    )
elseif(NOT APPLE)
    set(Source_Files
        ${Source_Files}
        "ExceptionHandling.cpp"
)
endif()
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
	${Source_Files}
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
message("CMAKE_OSX_ARCHITECTURES: " ${CMAKE_OSX_ARCHITECTURES})
message("LINUX: " ${LINUX}) 
message("APPLE: " ${APPLE})
message("CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})
if (LINUX OR (APPLE AND ("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "x86_64")))
message("Setting AVX compile options")
set_source_files_properties(${AVX_Files}
    PROPERTIES
        COMPILE_OPTIONS "-mavx;-mavx2;-mfma;-mssse3;-msse4.2")
set_source_files_properties(avx_simd_check.cpp
    PROPERTIES
        COMPILE_OPTIONS "-mxsave")
endif()
endif()

set_source_files_properties(${AVX_Files}
    PROPERTIES
        COMPILE_DEFINITIONS "Q_NUMERIC_NO_INTRINSICS")

################################################################################
# Target
################################################################################
set (CMAKE_AUTOMOC OFF)
add_library(DeepSkyStackerAVX STATIC ${AVX_Files} ${AVX_non-AVX_Files} ${Header_Files})
target_link_libraries(DeepSkyStackerAVX PRIVATE Qt6::Widgets Qt6::Network)
target_link_libraries(DeepSkyStackerAVX PRIVATE ZLIB::ZLIB)


################################################################################
# Output directory
################################################################################
set_target_properties(DeepSkyStackerAVX PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/${PLATFORM_NAME}/$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${PLATFORM_NAME}/$<CONFIG>/"
)
set_target_properties(DeepSkyStackerAVX PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Include directories
################################################################################
target_include_directories(DeepSkyStackerAVX PRIVATE
    .
    ../DeepSkyStacker;
    ../ZClass
)

if(WIN32)
target_include_directories(DeepSkyStackerAVX PRIVATE
 	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
)
endif()


################################################################################
# Compile definitions
################################################################################
target_compile_definitions(DeepSkyStackerAVX PRIVATE
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
target_compile_definitions(DeepSkyStackerAVX PRIVATE
	_WINDOWS
	)
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(DeepSkyStackerAVX PRIVATE
        $<$<CONFIG:Debug>:
            /Gm-;
            /RTC1
        >
        $<$<CONFIG:Release>:
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
        /utf-8;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    if(APPLE)
        target_compile_options(DeepSkyStackerAVX PRIVATE
            -Xclang -fopenmp
        )
    else()
        target_compile_options(DeepSkyStackerAVX PRIVATE
            -fopenmp
        )
    endif()
endif()

set (CMAKE_AUTOMOC ON)
################################################################################
# Target
################################################################################
qt_add_library(DeepSkyStackerKernel STATIC ${ALL_FILES})
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES ca cs de es fr it ja_JP nl pt_BR ro ru tr zh_CN zh_TW)
qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR i18n
    PLURALS_TS_FILE i18n/${PROJECT_NAME}_en.ts
    PLURALS_TS_FILE i18n/${PROJECT_NAME}_en_GB.ts
    )

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::container)
target_link_libraries(${PROJECT_NAME} PRIVATE Exiv2::exiv2lib)
target_link_libraries(${PROJECT_NAME} PRIVATE expat::expat)
target_link_libraries(${PROJECT_NAME} PRIVATE libraw::raw_r)
target_link_libraries(${PROJECT_NAME} PRIVATE TIFF::TIFF)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} PRIVATE DeepSkyStackerAVX)

################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/${PLATFORM_NAME}/$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${PLATFORM_NAME}/$<CONFIG>/"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
    .
    ../DeepSkyStacker;
    ../ZClass
    ../Tools
)

if(WIN32)
target_include_directories(${PROJECT_NAME} PRIVATE
 	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
)
endif()


target_precompile_headers(DeepSkyStackerKernel PRIVATE
	pch.h
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
        /utf-8;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
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

