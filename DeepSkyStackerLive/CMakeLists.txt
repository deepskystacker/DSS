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
    "res/DSSLive32x32.png"
    "res/DSSLive32x32@2x.png"
)

if(WIN32)
    set(Resources ${Resources}
	"../DeepSkyStacker/Icon/DSSLive.ico"
    )
endif()

set (Qt_Resource_Files
    "resources.qrc"
    )

if(WIN32)
    set(Resource_Files
	"Icon.rc"
	"version.rc"
    )
endif()

source_group("Resources" FILES ${Resources}	${Resource_Files})

if(APPLE)
    # And the following tells CMake where to find and install the file itself.
    set(app_icon_macos "${CMAKE_SOURCE_DIR}/DeepSkyStacker/Icon/DSSLive.icns")
    set_source_files_properties(${app_icon_macos} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
endif()

set(ui_Files
    "ui/ChartTab.ui"
    "ui/DeepSkyStackerLive.ui"
    "ui/EmailSettings.ui"
    "ui/ImageViewer.ui"
    "ui/RestartMonitoring.ui"
    "ui/SettingsTab.ui"
)
source_group("Qt\\ui" FILES ${ui_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
	${i18n_Files}
    ${ui_Files}
    ${Resources}
    ${Qt_Resource_Files}
    ${Resource_Files}
    ${Tools}
    ${app_icon_macos}
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
qt_add_resources(ALL_FILES ${Qt_Resource_Files})
qt_add_executable(DeepSkyStackerLive ${ALL_FILES})

if(APPLE)
    set_target_properties(DeepSkyStackerLive PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_BUNDLE_NAME "DeepSkyStackerLive"
        MACOSX_BUNDLE_ICON_FILE "DSSLive-2.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.github.deepskystacker.deepskystackerlive"
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2018-2025, David C. Partridge; Copyright © 2006-2019, Luc Coiffier"
        INSTALL_RPATH "@executable_path/../Frameworks"
)
endif()
if(WIN32)
    set_target_properties(DeepSkyStackerLive PROPERTIES
        WIN32_EXECUTABLE ON
    )
endif()

set_target_properties(DeepSkyStackerLive PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

###############################################################################
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
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
	"$<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/libs/Win64/DebugLibs>"
	"$<$<CONFIG:Release>:${CMAKE_SOURCE_DIR}/libs/Win64/ReleaseLibs>"
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
    DeepSkyStackerAVX
    ZClass
)
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::container)
target_link_libraries(${PROJECT_NAME} PRIVATE Exiv2::exiv2lib)
target_link_libraries(${PROJECT_NAME} PRIVATE expat::expat)
target_link_libraries(${PROJECT_NAME} PRIVATE libraw::raw_r)
if(NOT WIN32)
target_link_libraries(${PROJECT_NAME} PRIVATE CFITSIO::cfitsio)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE TIFF::TIFF)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)
if(LINUX)
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

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    SmtpMime
)

if(WIN32)
set(ADDITIONAL_LIBRARY_DEPENDENCIES ${ADDITIONAL_LIBRARY_DEPENDENCIES}
    cfitsio
    user32
    psapi
    htmlhelp
)
endif()
#
# libomp.dylib was manually installed into /usr/local/lib on MacOS
#
if(APPLE)
set(ADDITIONAL_LIBRARY_DEPENDENCIES ${ADDITIONAL_LIBRARY_DEPENDENCIES}
    omp
)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if(APPLE)
    add_custom_command(TARGET DeepSkyStackerLive
        POST_BUILD
        COMMAND "$ENV{HOME}/.unlock_keychain"
    )
endif()
if(LINUX)
    add_custom_command(TARGET DeepSkyStackerLive
        POST_BUILD
        #
        # Copy the .desktop file and icon file
        #
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DeepSkyStackerLive.desktop ${CMAKE_INSTALL_PREFIX}/DeepSkyStackerLive.desktop
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/DeepSkyStacker/Icon/DSSLive-2.icns ${CMAKE_INSTALL_PREFIX}/DSSLive-2.icns
)
endif()

if(NOT LINUX)
    set (deploy_tool_options_arg "-verbose=1")
    if(APPLE)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} -hardened-runtime -no-strip ")
        set(deploy_tool_options_arg "${deploy_tool_options_arg} \"-codesign=Developer ID Application: David Partridge (VH8AYT3C7Y)\"")
    elseif(WIN32)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --pdb")
    endif()

    # Generate a deployment script to be executed at install time
    # App bundles on macOS have an .app suffix
    if(APPLE)
        set(executable_path "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:DeepSkyStackerLive>.app")
    else()
        message ("Target filename:"  $<TARGET_FILE_NAME:DeepSkyStackerLive>)
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

