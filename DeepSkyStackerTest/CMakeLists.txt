set(PROJECT_NAME DeepSkyStackerTest)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "catch.h"
    "stdafx.h"
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
    "stdafx.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
################################################################################
# Includes for CMake from *.props
################################################################################
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "$ENV{QtMsBuild}/qt_defaults.cmake")
use_props(${PROJECT_NAME} Debug   "$ENV{QtMsBuild}/Qt.cmake")
use_props(${PROJECT_NAME} Release "$ENV{QtMsBuild}/Qt.cmake")

set(ROOT_NAMESPACE DeepSkyStackerTest)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "QtVS_v304"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/../ZCLass;"
    "${CMAKE_CURRENT_SOURCE_DIR}/../DeepSkyStackerKernel;"
    "$ENV{Boost_1_80_0}"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "_DEBUG"
    ">"
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "_CONSOLE;"
    "UNICODE;"
    "_UNICODE"
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

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    ZCLass
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    DeepSkyStackerKernel
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "ZClass"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ZClass/x64/debug;"
        "C:/Program Files %28x86%29/Visual Leak Detector/lib/Win64"
    ">"
    "$<$<CONFIG:Release>:"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ZClass/x64/release"
    ">"
)

