set(PROJECT_NAME DeepSkyStackerCL)
message("Configuring project: " ${PROJECT_NAME})

set(COMPILE_WARNING_AS_ERROR YES)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "../DeepSkyStackerKernel/dssbase.h"
    "../DeepSkyStackerKernel/dssrect.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "DeepSkyStackerCL.cpp"
    "progressconsole.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(i18n_Files
	"i18n/DeepSkyStackerCL_ca.ts"
	"i18n/DeepSkyStackerCL_cs.ts"
	"i18n/DeepSkyStackerCL_de.ts"
	"i18n/DeepSkyStackerCL_en.ts"
	"i18n/DeepSkyStackerCL_en_GB.ts"
	"i18n/DeepSkyStackerCL_es.ts"
	"i18n/DeepSkyStackerCL_fr.ts"
	"i18n/DeepSkyStackerCL_it.ts"
	"i18n/DeepSkyStackerCL_ja_JP.ts"
	"i18n/DeepSkyStackerCL_nl.ts"
	"i18n/DeepSkyStackerCL_pt_BR.ts"
	"i18n/DeepSkyStackerCL_ro.ts"
	"i18n/DeepSkyStackerCL_ru.ts"
	"i18n/DeepSkyStackerCL_tr.ts"
	"i18n/DeepSkyStackerCL_zh_CN.ts"
	"i18n/DeepSkyStackerCL_zh_TW.ts"
	)
	
source_group("Qt\\i18n" FILES ${i18n_Files}
    )

if(WIN32)
    set(Resources
	"../DeepSkyStacker/Icon.rc"
	"Version.rc"
	"../DeepSkyStacker/Icon/DeepSkyStacker.ico"
    )
endif()
	
source_group("Resources" FILES ${Resources}	)

set(ALL_FILES
    ${Header_Files}
	${i18n_Files}
    ${Source_Files}
	${Resources}
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
qt_add_executable(DeepSkyStackerCL ${ALL_FILES})

set_target_properties(DeepSkyStackerCL PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

#
# We don't create a bundle file for DSSCL, just a normal executable which gets copied
# into the DSS app bundle when that is built.
#
if(APPLE)
    set_target_properties(DeepSkyStackerCL PROPERTIES
        MACOSX_BUNDLE OFF
)
endif()
if(WIN32)
    set_target_properties(DeepSkyStackerCL PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

################################################################################
# Include directories
################################################################################
target_include_directories(DeepSkyStackerCL PUBLIC
    .
    ../DeepSkyStackerKernel
    ../ZClass
    ../Tools
  )

if(WIN32)
target_include_directories(DeepSkyStackerCL PUBLIC
   ../include
   "$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
)
endif()

target_precompile_headers(DeepSkyStackerCL PRIVATE
	pch.h
	)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(DeepSkyStackerCL PRIVATE
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
        /utf-8;
        /EHsc
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /PROFILE
        >
        $<$<CONFIG:Release>:
            /OPT:ICF
        >
        /DEBUG:FULL;
        /MACHINE:X64;
        /SUBSYSTEM:CONSOLE;
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
target_link_directories(${PROJECT_NAME} PRIVATE
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
	"$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/libs/Win64/DebugLibs>"
	"$<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/libs/Win64/ReleaseLibs>"
	)
endif()


################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(DeepSkyStackerCL PRIVATE
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
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(DeepSkyStackerCL PRIVATE
        $<$<CONFIG:Debug>:
            /NODEFAULTLIB:MSVCRT;
            /DEBUG
        >
        $<$<CONFIG:Release>:
            /DEBUG:FULL;
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
target_link_libraries(DeepSkyStackerCL PRIVATE
    Qt6::Core
    Qt6::Gui
	DeepSkyStackerKernel
    DeepSkyStackerAVX
    ZClass
)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::container)
target_link_libraries(${PROJECT_NAME} PRIVATE Exiv2::exiv2lib)
target_link_libraries(${PROJECT_NAME} PRIVATE expat::expat)
#if(NOT WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE CFITSIO::cfitsio)
#endif() 
target_link_libraries(${PROJECT_NAME} PRIVATE libraw::raw_r)
target_link_libraries(${PROJECT_NAME} PRIVATE TIFF::TIFF)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
if(NOT WIN32)
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
#   cfitsio
    user32
    psapi
)
endif()

if(WIN32)
    add_custom_command(TARGET DeepSkyStackerCL
        POST_BUILD
        #
        # Copy the .pdb
        #
        COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/DeepSkyStackerCL.pdb
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

#
# We don't deploy/install DSSCL on macOS, instead it gets copied into the DSS
# app bundle. The DSS app bundle *is* deployed using the qt_deploy_app_script
#
if(NOT APPLE)
    if (NOT LINUX)
        set (deploy_tool_options_arg "")
        if(APPLE)
            set(deploy_tool_options_arg "${deploy_tool_options_arg} -hardened-runtime -no-strip")
            set(deploy_tool_options_arg "${deploy_tool_options_arg} -codesign=Developer ID Application: David Partridge (VH8AYT3C7Y)")
        elseif(WIN32)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --compiler-runtime --pdb")
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --dir . --plugindir plugins")
        endif()

        # Generate a deployment script to be executed at install time
        # App bundles on macOS have an .app suffix
        if(APPLE)
            set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStackerCL>.app")
        else()
            message ("Target filename:"  $<TARGET_FILE_NAME:DeepSkyStackerCL>)
            set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStackerCL>")
        endif()

        message ("executable_path: " ${executable_path})
        message ("deploy tools options arg: " ${deploy_tool_options_arg})
         qt_generate_deploy_script(
             TARGET DeepSkyStackerCL
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
    install (TARGETS ${PROJECT_NAME} BUNDLE DESTINATION .)
endif()
