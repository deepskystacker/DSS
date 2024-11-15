set(PROJECT_NAME DeepSkyStackerCL)
message("Configuring project: " ${PROJECT_NAME})

set(COMPILE_WARNING_AS_ERROR YES)
SET (CMAKE_INSTALL_BINDIR ".")

qt_standard_project_setup()

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
	
source_group("Qt\\i18n" FILES ${i18n_Files} "i18n/translations.qrc"
    )

set(Resources
	"Version.rc"
	)
	
source_group("Resources" FILES ${Resources}	)

set(ALL_FILES
    ${Header_Files}
	${i18n_Files} "i18n/translations.qrc"
    ${Source_Files}
	${Resources}
)

################################################################################
# Target
################################################################################
qt_add_executable(DeepSkyStackerCL ${ALL_FILES})

set_source_files_properties(${i18n_Files}
     PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/i18n")

qt_add_lrelease(TS_FILES ${i18n_Files})

set_target_properties(DeepSkyStackerCL PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
set_target_properties(DeepSkyStackerCL PROPERTIES
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
target_include_directories(DeepSkyStackerCL PUBLIC
    .
    ../DeepSkyStackerKernel
    ../LibRaw
    ../ZClass
    ../Tools
  )

if(WIN32)
target_include_directories(DeepSkyStackerCL PUBLIC
   ../Zlib
   ../LibTiff
   ../CFitsIO
   ../include
   "$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
   "$ENV{Boost_1_80_0}"
)
endif()

target_precompile_headers(DeepSkyStackerCL PRIVATE
	stdafx.h
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
        /GL-;
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
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fopenmp
    )
endif()

if(WIN32)
target_link_directories(${PROJECT_NAME} PRIVATE
	"$<$<CONFIG:Debug>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/DebugLibs>"
	"$<$<CONFIG:Release>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/ReleaseLibs>"
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
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

if(WIN32)
target_link_directories(DeepSkyStackerCL PRIVATE
	"$<$<CONFIG:Debug>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/DebugLibs>"
	"$<$<CONFIG:Release>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/ReleaseLibs>"
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
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
    libraw
    libtiff
    ZClass
)

set(DEBUG_DEPENDENCIES exiv2d libexpatd zlibstaticd)
set(RELEASE_DEPENDENCIES exiv2 libexpat zlibstatic)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:${DEBUG_DEPENDENCIES}>"
    "$<$<CONFIG:Release>:${RELEASE_DEPENDENCIES}>"
    "cfitsio"
)

if(WIN32)
set(ADDITIONAL_LIBRARY_DEPENDENCIES ${ADDITIONAL_LIBRARY_DEPENDENCIES}
    psapi
)

endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

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
    set(executable_path "$<TARGET_FILE_NAME:DeepSkyStackerCL>.app")
else()
    message ("Qt Deploy Bin Dir: " ${QT_DEPLOY_BIN_DIR})
    message ("Qt Deploy Lib Dir: " ${QT_DEPLOY_LIB_DIR})
    message ("Target filename:"  $<TARGET_FILE_NAME:DeepSkyStackerCL>)
    set(executable_path "${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:DeepSkyStackerCL>")
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

INSTALL (TARGETS DeepSkyStackerCL)
INSTALL (SCRIPT ${deploy_script})



