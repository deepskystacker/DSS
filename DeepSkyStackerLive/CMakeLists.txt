set(PROJECT_NAME DeepSkyStackerLive)
message("Configuring project: " ${PROJECT_NAME})

set(COMPILE_WARNING_AS_ERROR YES)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "dssliveenums.h"
    "LiveSettings.h"
 )
source_group("Header Files" FILES ${Header_Files})

if(WIN32)
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
endif()

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
)
source_group("Source Files" FILES ${Source_Files})

set(Tools
    "../Tools/imageview.cpp"
    "../Tools/QLinearGradientCtrl.cpp"
)
source_group("Tools" FILES ${Tools})

set(i18n_Files
	"i18n/DeepSkyStackerLive_ca.ts"
	"i18n/DeepSkyStackerLive_cs.ts"
	"i18n/DeepSkyStackerLive_de.ts"
	"i18n/DeepSkyStackerLive_en.ts"
	"i18n/DeepSkyStackerLive_en_GB.ts"
	"i18n/DeepSkyStackerLive_es.ts"
	"i18n/DeepSkyStackerLive_fr.ts"
	"i18n/DeepSkyStackerLive_it.ts"
	"i18n/DeepSkyStackerLive_ja_JP.ts"
	"i18n/DeepSkyStackerLive_nl.ts"
	"i18n/DeepSkyStackerLive_pt_BR.ts"
	"i18n/DeepSkyStackerLive_ro.ts"
	"i18n/DeepSkyStackerLive_ru.ts"
	"i18n/DeepSkyStackerLive_tr.ts"
	"i18n/DeepSkyStackerLive_zh_CN.ts"
	"i18n/DeepSkyStackerLive_zh_TW.ts"
	)	
source_group("Qt\\i18n" FILES ${i18n_Files} "i18n/translations.qrc"
    )
	
set(Resources
	"../DeepSkyStacker/Icon.rc"
	"../DeepSkyStacker/Icon/app.ico"
	"resources.qrc"
	"version.rc"
	)

set (Resource_Files
    "resources.qrc"
    )
	
source_group("Resources" FILES ${Resources}	${Resource_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
	${i18n_Files}
	${Resources}
    ${Tools}
)
if(WIN32)
  set (ALL_FILES ${ALL_FILES} ${Header_Files__SmtpMime}
)
endif()

################################################################################
# Target
################################################################################
qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES ca cs de es fr it ja_JP nl pt_BR ro ru tr zh_CN zh_TW)
qt_add_translations(${PROJECT_NAME}
    TS_FILE_DIR i18n
    PLURALS_TS_FILE i18n/${PROJECT_NAME}_en.ts
    PLURALS_TS_FILE i18n/${PROJECT_NAME}_en_GB.ts
    )
qt_add_resources(ALL_FILES ${Resource_Files})
qt_add_executable(DeepSkyStackerLive ${ALL_FILES})

set_target_properties(DeepSkyStackerLive PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
set_target_properties(DeepSkyStackerLive PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    .
    ../DeepSkyStackerKernel
    ../ZClass
    ../Tools
  )

if(WIN32)
target_include_directories(${PROJECT_NAME} PUBLIC
   ../include
   ../include/SmtpMime
   "$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
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
	    -W4 /wd4652 /wd4702 /wd4828 
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
        /SUBSYSTEM:WINDOWS;
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
	"$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/libs/Win64/DebugLibs>"
	"$<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/libs/Win64/ReleaseLibs>"
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
	)
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::Charts
	DeepSkyStackerKernel
    ZClass
)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::container)
target_link_libraries(${PROJECT_NAME} PRIVATE Exiv2::exiv2lib)
target_link_libraries(${PROJECT_NAME} PRIVATE expat::expat)
target_link_libraries(${PROJECT_NAME} PRIVATE CFITSIO::cfitsio)
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
    CURL::libcurl
)
endif()

if(APPLE)
target_link_directories(${PROJECT_NAME} PRIVATE /usr/local/lib)
endif()
set(ADDITIONAL_LIBRARY_DEPENDENCIES SmtpMime)

if(WIN32)
set(ADDITIONAL_LIBRARY_DEPENDENCIES ${ADDITIONAL_LIBRARY_DEPENDENCIES}
    user32
    psapi
    htmlhelp
)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if(NOT LINUX)
    set (deploy_tool_options_arg "-libpath .")
    if(APPLE)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} -hardened-runtime")
    elseif(WIN32)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --pdb")
    endif()

    # Generate a deployment script to be executed at install time
    # App bundles on macOS have an .app suffix
    if(APPLE)
        set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStackerLive>.app")
    else()
        message ("Target filename:"  $<TARGET_FILE_NAME:DeepSkyStacker>)
        set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStackerLive>")
    endif()
    message ("executable_path: " ${executable_path})
    message ("deploy tools options arg: " ${deploy_tool_options_arg})
     qt_generate_deploy_script(
         TARGET DeepSkyStackerLive
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

