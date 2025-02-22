set(PROJECT_NAME DeepSkyStackerTest)
message("Configuring project: " ${PROJECT_NAME})

set(COMPILE_WARNING_AS_ERROR YES)
SET (CMAKE_INSTALL_BINDIR ".")

qt_standard_project_setup()

################################################################################
# Source groups
################################################################################
set(Header_Files
    "catch.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "AvxAccumulateTest.cpp"
    "AvxAccumulateTest.h"
    "AvxCfaTest.cpp"
    "AvxEntropyTest.cpp"
    "AvxEntropyTest.h"
    "AvxHistogramTest.cpp"
    "AvxStackingTest.cpp"
    "BitMapFillerTest.cpp"
    "DeepSkyStackerTest.cpp"
    "DssRectTest.cpp"
    "OpenMpTest.cpp"
    "PixelIteratorTest.cpp"
    "RegisterTest.cpp"
    "SkyBackGroupTest.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
qt_add_executable(DeepSkyStackerTest ${ALL_FILES})

set_source_files_properties(${i18n_Files}
     PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/i18n")

qt_add_lrelease(TS_FILES ${i18n_Files})

set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE ON
)

################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    .
    ../DeepSkyStackerKernel
    ../LibRaw
    ../ZClass
  )

if(WIN32)
target_include_directories(${PROJECT_NAME} PUBLIC
   ../Zlib
   ../LibTiff
   ../CFitsIO
   ../include
   "$ENV{Boost_1_80_0}"
)
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
	stdafx.h
	)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
  "$<$<CONFIG:Debug>:Z_TRACE_DEVELOP>"
  "$<$<CONFIG:Release>:NDEBUG>"
  NOMINMAX
  DSS_COMMANDLINE
  _CONSOLE
  LIBRAW_NODLL
  _CRT_SECURE_NO_DEPRECATE
  USE_LIBTIFF_STATIC
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:
            /GL-;
            /Oi;
            /Gy;
            /openmp
        >
        /permissive-;
        /sdl;
        /W4;
        /WX;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        /wd4828;
        /wd4702;
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /DEBUG;
        /SUBSYSTEM:CONSOLE
    )
endif()

if(WIN32)
target_link_directories(DeepSkyStackerTest PRIVATE
	"$<$<CONFIG:Debug>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/DebugLibs>"
	"$<$<CONFIG:Release>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/ReleaseLibs>"
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
	)
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
    DeepSkyStackerKernel
	libraw
	libtiff
	ZClass
)

#
# Force the values of QT_DEPLOY... variables
#
set (QT_DEPLOY_BIN_DIR ".")
set (QT_DEPLOY_LIBEXEC_DIR ".")
set (QT_DEPLOY_LIB_DIR ".")
set (QT_DEPLOY_PLUGINS_DIR ".")

#
# Set options for the deployment script.   The value of:
# 
# --libdir .
# 
# is set to work around a bug in qt_deploy_runtime_dependencies which uses
#
# --libdir bin
# 
# even though QT_DEPLOY_LIB_DIR is set to .
#
set (deploy_tool_options_arg "--libdir .")
if(APPLE)
    set(deploy_tool_options_arg "${deploy_tool_options_arg} --hardened-runtime")
elseif(WIN32)
    set(deploy_tool_options_arg "${deploy_tool_options_arg} --pdb")
endif()

# Generate a deployment script to be executed at install time
# App bundles on macOS have an .app suffix
if(APPLE)
    set(executable_path "$<TARGET_FILE_NAME:${PROJECT_NAME}>.app")
else()
    message ("Qt Deploy Bin Dir: " ${QT_DEPLOY_BIN_DIR})
    message ("Qt Deploy Lib Dir: " ${QT_DEPLOY_LIB_DIR})
    message ("Target filename:"  $<TARGET_FILE_NAME:${PROJECT_NAME}>)
    set(executable_path "${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>")
endif()
message ("executable_path: " ${executable_path})
message ("deploy tools options arg: " ${deploy_tool_options_arg})
 qt_generate_deploy_script(
     TARGET DeepSkyStackerTest
     OUTPUT_SCRIPT deploy_script
     CONTENT "
 qt_deploy_runtime_dependencies(
     EXECUTABLE \"${executable_path}\"
     DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
 )"
 )

INSTALL (TARGETS ${PROJECT_NAME})
INSTALL (SCRIPT ${deploy_script})



