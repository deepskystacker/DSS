set(PROJECT_NAME DeepSkyStacker)
message("Configuring project: " ${PROJECT_NAME})

qt_standard_project_setup()

set(COMPILE_WARNING_AS_ERROR YES)

set(CMAKE_AUTORCC ON)

################################################################################
# Source groups
################################################################################
set(Dialogs
    "About.cpp"
    "About.h"
    "ExplorerBar.cpp"
    "ExplorerBar.h"
)
source_group("Dialogs" FILES ${Dialogs})

set(Dialogs__Processing
    "processingcontrols.cpp"
    "processingcontrols.h"
    "ProcessingDlg.cpp"
    "ProcessingDlg.h"
    "ProcessingSettings.h"
    "ProcessingSettingsDlg.cpp"
    "ProcessingSettingsDlg.h"
    "RGBTab.cpp"
    "SavePicture.cpp"
    "SavePicture.h"
    "StarMaskDlg.cpp"
    "StarMaskDlg.h"
)
source_group("Dialogs\\Processing" FILES ${Dialogs__Processing})

set(Dialogs__Registering
    "AskRegistering.cpp"
    "AskRegistering.h"
    "RegisterSettings.cpp"
    "RegisterSettings.h"
)
source_group("Dialogs\\Registering" FILES ${Dialogs__Registering})

set(Dialogs__Settings
    "AlignmentParameters.cpp"
    "AlignmentParameters.h"
    "CometStacking.cpp"
    "CometStacking.h"
    "IntermediateFiles.cpp"
    "IntermediateFiles.h"
    "OutputTab.cpp"
    "OutputTab.h"
    "PostCalibration.cpp"
    "PostCalibration.h"
    "RawDDPSettings.cpp"
    "RawDDPSettings.h"
    "ResultParameters.cpp"
    "ResultParameters.h"
    "StackingParameters.cpp"
    "StackingParameters.h"
    "StackSettings.cpp"
    "StackSettings.h"
)
source_group("Dialogs\\Settings" FILES ${Dialogs__Settings})

set(Dialogs__Stacking
    "BackgroundOptions.cpp"
    "BackgroundOptions.h"
    "BatchStacking.cpp"
    "BatchStacking.h"
    "CheckAbove.cpp"
    "CheckAbove.h"
    "DropFilesDlg.cpp"
    "DropFilesDlg.h"
    "ImageProperties.cpp"
    "ImageProperties.h"
    "picturelist.cpp"
    "picturelist.h"
    "RecommendedSettings.cpp"
    "RecommendedSettings.h"
    "RenameGroup.cpp"
    "RenameGroup.h"
    "SaveEditChanges.cpp"
    "SaveEditChanges.h"
    "StackingDlg.cpp"
    "StackingDlg.h"
    "StackRecap.cpp"
    "StackRecap.h"
)
source_group("Dialogs\\Stacking" FILES ${Dialogs__Stacking})

set(Header_Files
    "../DeepSkyStackerKernel/histogram.h"
    "DeepSkyStacker.h"
    "Delaunay.h"
    "dslr.h"
    "DSS-versionhelpers.h"
    "DSS-winapifamily.h"
    "DSS-winpackagefamily.h"
    "lowerdockwidget.h"
    "mrulist.h"
    "stdafx.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "BaseDialog.cpp"
    "DeepSkyStacker.cpp"
    "Delaunay.cpp"
    "editstars.cpp"
    "lowerdockwidget.cpp"
    "progressdlg.cpp"
    "selectrect.cpp"
    "toolbar.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(Ui_Files
	"ui/About.ui"
	"ui/AlignmentParameters.ui"
	"ui/AskRegistering.ui"
	"ui/BackgroundOptions.ui"
	"ui/BatchStacking.ui"
	"ui/CheckAbove.ui"
	"ui/CometStacking.ui"
	"ui/DropFilesDlg.ui"
	"ui/ExplorerBar.ui"
	"ui/ImageProperties.ui"
	"ui/IntermediateFiles.ui"
	"ui/OutputTab.ui"
	"ui/PictureList.ui"
	"ui/PostCalibration.ui"
	"ui/ProcessingControls.ui"
	"ui/ProcessingDlg.ui"
	"ui/ProcessingSettingsDlg.ui"
	"ui/ProgressDlg.ui"
	"ui/RawDDPSettings.ui"
	"ui/RecommendedSettings.ui"
	"ui/RegisterSettings.ui"
	"ui/RenameGroup.ui"
	"ui/ResultParameters.ui"
	"ui/SaveEditChanges.ui"
	"ui/StackingDlg.ui"
	"ui/StackingParameters.ui"
	"ui/StackRecap.ui"
	"ui/StackSettings.ui"
	"ui/StarMaskDlg.ui"
	)
	
source_group("Qt\\ui" FILES ${Ui_Files})

set(i18n_Files
	"i18n/DeepSkyStacker_ca.ts"
	"i18n/DeepSkyStacker_cs.ts"
	"i18n/DeepSkyStacker_de.ts"
	"i18n/DeepSkyStacker_en.ts"
	"i18n/DeepSkyStacker_es.ts"
	"i18n/DeepSkyStacker_fr.ts"
	"i18n/DeepSkyStacker_it.ts"
	"i18n/DeepSkyStacker_ja_JP.ts"
	"i18n/DeepSkyStacker_nl.ts"
	"i18n/DeepSkyStacker_pt_BR.ts"
	"i18n/DeepSkyStacker_ro.ts"
	"i18n/DeepSkyStacker_ru.ts"
	"i18n/DeepSkyStacker_tr.ts"
	"i18n/DeepSkyStacker_zh_CN.ts"
	"i18n/DeepSkyStacker_zh_TW.ts"
	)
	
source_group("Qt\\i18n" FILES ${i18n_Files} "i18n/translations.qrc"
    )

set(Resources
	"Icon.rc"
	"Icon/app.ico"
	"resources.qrc"
	"Version.rc"
    "res/4Corners.bmp"
    "res/4Corners.png"
    "res/asinh.bmp"
    "res/BiasColour.png"
    "res/BiasColour@2x.png"
    "res/BiasCYMG.png"
    "res/BiasCYMG@2x.png"
    "res/BiasGreyscale.png"
    "res/BiasGreyscale@2x.png"
    "res/BiasRGB.png"
    "res/BiasRGB@2x.png"
    "res/bmp00001.bmp"
    "res/brazil.png"
    "res/ButtonComet_Disabled.png"
    "res/ButtonComet_Down.png"
    "res/ButtonComet_Hot.png"
    "res/ButtonComet_Up.png"
    "res/ButtonSave_Disabled.png"
    "res/ButtonSave_Down.png"
    "res/ButtonSave_Hot.png"
    "res/ButtonSave_Up.png"
    "res/ButtonSelect_Disabled.png"
    "res/ButtonSelect_Down.png"
    "res/ButtonSelect_Hot.png"
    "res/ButtonSelect_Up.png"
    "res/ButtonStar_Disabled.png"
    "res/ButtonStar_Down.png"
    "res/ButtonStar_Hot.png"
    "res/ButtonStar_Up.png"
    "res/Calibration_Linear.bmp"
    "res/Calibration_Rational.bmp"
    "res/Catalan.png"
    "res/Comet_Freeze.bmp"
    "res/Comet_Normal.bmp"
    "res/Comet_Trails.bmp"
    "res/cuberoot.bmp"
    "res/CurveMethods.pspimage"
    "res/custommode.bmp"
    "res/Czech.png"
    "res/DarkColour.png"
    "res/DarkColour@2x.png"
    "res/DarkCYMG.png"
    "res/DarkCYMG@2x.png"
    "res/darkfram.ico"
    "res/darkframe.ico"
    "res/DarkGreyscale.png"
    "res/DarkGreyscale@2x.png"
    "res/DarkRGB.png"
    "res/DarkRGB@2x.png"
    "res/Dutch.png"
    "res/FlatColour.png"
    "res/FlatColour@2x.png"
    "res/FlatCYMG.png"
    "res/FlatCYMG@2x.png"
    "res/flatframe.ico"
    "res/FlatGreyscale.png"
    "res/FlatGreyscale@2x.png"
    "res/FlatRGB.png"
    "res/FlatRGB@2x.png"
    "res/GammaOff.bmp"
    "res/GammaOn.bmp"
    "res/German.png"
    "res/hdrdown.bmp"
    "res/hdrup.bmp"
    "res/Heart.png"
    "res/icon1.ico"
    "res/ImageList_CFA_CYMG.bmp"
    "res/ImageList_CFA_RGB.bmp"
    "res/ImageList_Color.bmp"
    "res/ImageList_GrayScale.bmp"
    "res/ImageList_Mask.bmp"
    "res/intersectionmode.bmp"
    "res/Italian.png"
    "res/LightColour.png"
    "res/LightColour@2x.png"
    "res/LightCYMG.png"
    "res/LightCYMG@2x.png"
    "res/lightfra.ico"
    "res/lightframe.ico"
    "res/LightGreyscale.png"
    "res/LightGreyscale@2x.png"
    "res/LightRGB.png"
    "res/LightRGB@2x.png"
    "res/linear.bmp"
    "res/linear_en.bmp"
    "res/linear_fr.bmp"
    "res/log.bmp"
    "res/loglog.bmp"
    "res/logo.png"
    "res/logsqrt.bmp"
    "res/MaskStarShapes.pspimage"
    "res/mosaicmode.bmp"
    "res/normalmode.bmp"
    "res/offsetframe.ico"
    "res/Pattern_BGGR.bmp"
    "res/Pattern_CYGMCYMG.bmp"
    "res/Pattern_GBRG.bmp"
    "res/Pattern_GRBG.bmp"
    "res/Pattern_RGGB.bmp"
    "res/Portugal.png"
    "res/prc.png"
    "res/RGBCalibration_Maximum.bmp"
    "res/RGBCalibration_Middle.bmp"
    "res/RGBCalibration_Minimum.bmp"
    "res/RGBCalibration_None.bmp"
    "res/Romanian.png"
    "res/Russian.png"
    "res/Spanish.png"
    "res/sqrt.bmp"
    "res/StarShape_Bell.bmp"
    "res/StarShape_Cone.bmp"
    "res/StarShape_Cubic.bmp"
    "res/StarShape_Quadratic.bmp"
    "res/StarShape_TruncatedBell.bmp"
    "res/StarShape_TruncatedCone.bmp"
    "res/Taiwan.png"
    "res/Turkish.png"
	)
	
source_group("Resources" FILES ${Resources}	)

set(Tools
    "../Tools/imageview.cpp"
    "../Tools/QElidedLabel.cpp"
    "../Tools/QLinearGradientCtrl.cpp"
)
source_group("Tools" FILES ${Tools})

set(ALL_FILES
    ${Dialogs}
    ${Dialogs__Processing}
    ${Dialogs__Registering}
    ${Dialogs__Settings}
    ${Dialogs__Stacking}
    ${Header_Files}
	${Ui_Files}
	${i18n_Files} "i18n/translations.qrc"
	${Resources}
    ${Source_Files}
    ${Tools}
)

################################################################################
# Target
################################################################################
qt_add_executable(DeepSkyStacker ${ALL_FILES})

set_source_files_properties(${i18n_Files}
     PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/i18n")

qt_add_lrelease(TS_FILES ${i18n_Files})

set_target_properties(DeepSkyStacker PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)
set_target_properties(DeepSkyStacker PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

################################################################################
# Include directories
################################################################################
target_include_directories(DeepSkyStacker PUBLIC
    .
    ../DeepSkyStackerKernel
    GeneratedFiles/$<CONFIG>
    GeneratedFiles
    ../LibRaw
    ../ZClass
    ../Tools
  )

if(WIN32)
target_include_directories(${PROJECT_NAME} PUBLIC
   ../Zlib
   ../LibTiff
   ../include
   ../include/exiv2
   ../include/SmtpMime
   "$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
   "$ENV{Boost_1_80_0}"
)
endif()

target_precompile_headers(DeepSkyStacker PRIVATE
	stdafx.h
	)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(DeepSkyStacker PRIVATE
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
    target_compile_options(DeepSkyStacker PRIVATE
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
    target_link_options(DeepSkyStacker PRIVATE
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
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fopenmp
    )
endif()

if(WIN32)
target_link_directories(DeepSkyStacker PRIVATE
	"$<$<CONFIG:Debug>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/DebugLibs>"
	"$<$<CONFIG:Release>:C:/Users/amonra/Documents/GitHub/DSS/libs/Win64/ReleaseLibs>"
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/lib/Win64>"
	)
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(DeepSkyStacker PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
	DeepSkyStackerKernel
    libraw
    ZClass
)
if(WIN32)
target_link_libraries(DeepSkyStacker PRIVATE
    libtiff
)
else()
target_link_libraries(DeepSkyStacker PRIVATE
    #TIFF::tiff
    #Exiv2::exiv2lib
    #expat::expat
    JPEG::JPEG
    OpenMP::OpenMP_CXX
    Threads::Threads
    ZLIB::ZLIB
)
endif()

if(WIN32)
set(DEBUG_DEPENDENCIES exiv2d libexpatd zlibstaticd cfitsio)
set(RELEASE_DEPENDENCIES exiv2 libexpat zlibstatic cfitsio)
else()
set(DEBUG_DEPENDENCIES tiff.a exiv2.a expat.a z.a cfitsio.a brotlidec jpeg inih INIReader gomp)
set(RELEASE_DEPENDENCIES tiff.a exiv2.a expat.a z.a cfitsio.a brotlidec jpeg inih INIReader gomp)
#set(DEBUG_DEPENDENCIES tiff.a cfitsio.a brotlidec inih INIReader)
#set(RELEASE_DEPENDENCIES tiff.a cfitsio.a brotlidec inih INIReader)
endif()

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:${DEBUG_DEPENDENCIES}>"
    "$<$<CONFIG:Release>:${RELEASE_DEPENDENCIES}>"
)

if(WIN32)
set(ADDITIONAL_LIBRARY_DEPENDENCIES ${ADDITIONAL_LIBRARY_DEPENDENCIES}
    user32
    psapi
    htmlhelp
)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if(NOT LINUX)
    set (deploy_tool_options_arg "")
    if(APPLE)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --hardened-runtime")
    elseif(WIN32)
        set(deploy_tool_options_arg "${deploy_tool_options_arg} --pdb")
    endif()

    # Generate a deployment script to be executed at install time
    # App bundles on macOS have an .app suffix
    if(APPLE)
        set(executable_path "$<TARGET_FILE_NAME:DeepSkyStacker>.app")
    else()
        message ("Target filename:"  $<TARGET_FILE_NAME:DeepSkyStacker>)
        set(executable_path "${CMAKE_INSTALL_BINDIR}/$<TARGET_FILE_NAME:DeepSkyStacker>")
    endif()
    message ("executable_path: " ${executable_path})
    message ("deploy tools options arg: " ${deploy_tool_options_arg})
     qt_generate_deploy_script(
         TARGET DeepSkyStacker
         OUTPUT_SCRIPT deploy_script
         CONTENT "
     qt_deploy_runtime_dependencies(
         EXECUTABLE \"${executable_path}\"
         DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
     )"
     )

    INSTALL (TARGETS DeepSkyStacker)
    INSTALL (SCRIPT ${deploy_script})
endif()

#[=[
if(WIN32)
################################################################################
# Post build events
################################################################################
set (deployment_string
        ${WINDEPLOYQT_EXECUTABLE} " --verbose 2 --dir . --plugindir plugins "
        "--compiler-runtime --pdb "
        $<TARGET_FILE_NAME:DeepSkyStacker>
    )
message ("Deployment command: " ${deployment_string})

ADD_CUSTOM_COMMAND(
  TARGET DeepSkyStacker
  POST_BUILD
  COMMAND
    ${WINDEPLOYQT_EXECUTABLE} --verbose 2 --dir . --plugindir plugins
        --compiler-runtime --pdb
        $<TARGET_FILE_NAME:DeepSkyStacker>
       
    )
endif()
#]=]
