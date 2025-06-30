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
    "TimeStampTest.cpp"
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

set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE OFF
)

################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/${PLATFORM_NAME}/$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/${PLATFORM_NAME}/$<CONFIG>/"
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    .
    ../DeepSkyStackerKernel
    ../ZClass
  )

if(WIN32)
target_include_directories(${PROJECT_NAME} PUBLIC
   ../include
   "$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
	libraw::raw_r
    TIFF::TIFF
    #Exiv2::exiv2lib
    #expat::expat
    JPEG::JPEG
    ZLIB::ZLIB
)
if(LINUX)
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenMP::OpenMP_CXX
    Threads::Threads
)
endif()
#
# libomp.dylib was manually installed into /usr/local/lib on MacOS
#
if(APPLE)
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenMP::OpenMP_CXX
)
endif()

if(WIN32)
target_link_directories(${PROJECT_NAME} PRIVATE
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
	)
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
	pch.h
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
target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
    Qt6::Gui
    DeepSkyStackerKernel
	ZClass
)

if(WIN32)
    add_custom_command(TARGET DeepSkyStackerTest
        POST_BUILD
        #
        # Copy the .pdb file
        #
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/DeepSkyStackerTest.pdb
    )
endif()

if(NOT LINUX)
    set (deploy_tool_options_arg "-verbose=1")
    if(APPLE)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} -hardened-runtime -no-strip ")
    elseif(WIN32)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --compiler-runtime --pdb")
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --dir . --plugindir plugins")
endif()

    # Generate a deployment script to be executed at install time
    # App bundles on macOS have an .app suffix
    if(APPLE)
        set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStackerTest>.app")
    else()
        message ("Target filename:"  $<TARGET_FILE_NAME:DeepSkyStackerLive>)
        set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStackerTest>")
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
else()
qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
)
endif()
install (SCRIPT ${deploy_script})
install(TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)




