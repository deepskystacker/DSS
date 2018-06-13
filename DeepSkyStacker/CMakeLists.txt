# define source files in intended IDE groupings

set (
   DIALOGS_FILES
   
   About.cpp
   About.h
   DeepStackerDlg.cpp
   DeepStackerDlg.h
   ExplorerBar.cpp
   ExplorerBar.h 
   FileProperty.cpp
   FileProperty.h 
   PictureListCtrl.cpp
   PictureListCtrl.h
   ProgressDlg.cpp
   ProgressDlg.h
)
   
set (
   DIALOGS_IMAGESINKS_FILES
   
   ImageSinks.cpp
   ImageSinks.h
   SaveEditChanges.cpp
   SaveEditChanges.h
)
   
set (
   DIALOGS_LIBRARY_FILES
   
   LibraryDlg.cpp
   LibraryDlg.h
)
   
set (
   DIALOGS_PROCESSING_FILES
   
   LuminanceTab.cpp
   LuminanceTab.h
   ProcessingDlg.cpp
   ProcessingDlg.h
   ProcessSettingsSheet.cpp
   ProcessSettingsSheet.h
   RGBTab.cpp
   RGBTab.h
   SaturationTab.cpp
   SaturationTab.h
   SavePicture.cpp
   SavePicture.h
   SettingsDlg.cpp
   SettingsDlg.h
   StarMaskDlg.cpp
   StarMaskDlg.h
)
   
set (
   DIALOGS_REGISTERING_FILES
   
   AskRegistering.cpp
   AskRegistering.h
   RegisterSettings.cpp
   RegisterSettings.h
   RegisterSettings_Actions.cpp
   RegisterSettings_Actions.h
   RegisterSettings_Advanced.cpp
   RegisterSettings_Advanced.h
)
   
set (
   DIALOGS_SETTINGS_FILES
   
   FitsFilesTab.cpp
   FitsFilesTab.h
   RawDDPSettings.cpp
   RawDDPSettings.h
   RawFilesTab.cpp
   RawFilesTab.h
)
   
set (
   DIALOGS_STACKING_FILES
   
   AlignmentParameters.cpp
   AlignmentParameters.h
   BackgroundLoading.cpp
   BackgroundLoading.h
   BackgroundOptions.cpp
   BackgroundOptions.h
   BatchStacking.cpp
   BatchStacking.h
   CheckAbove.cpp
   CheckAbove.h
   CometStacking.cpp
   CometStacking.h
   DropFilesDlg.cpp
   DropFilesDlg.h
   IntermediateFiles.cpp
   IntermediateFiles.h
   OutputTab.cpp
   OutputTab.h
   PostCalibration.cpp
   PostCalibration.h
   RecommandedSettings.cpp  # should have been RecommendedSettings
   RecommandedSettings.h
   ResultParameters.cpp
   ResultParameters.h
   StackingDlg.cpp
   StackingDlg.h
   StackingParameters.cpp
   StackingParameters.h
   StackRecap.cpp
   StackRecap.h
   StackSettings.cpp
   StackSettings.h
)
   
set (
   SOURCE_FILES 
   
   DeepSkyStacker.cpp
   DeepSkyStacker.h
   PCLTools.cpp
   PCLTools.h
   SetUILanguage.cpp
   SetUILanguage.h
   StdAfx.cpp
   StdAfx.h

   DeepSkyStacker.rc
   DeepSkyStackerCAT.rc
   DeepSkyStackerCN2.rc
   DeepSkyStackerCZ.rc
   DeepSkyStackerDE.rc
   DeepSkyStackerEN.rc
   DeepSkyStackerES.rc
   DeepSkyStackerFR.rc
   DeepSkyStackerIT.rc
   DeepSkyStackerNL.rc
   DeepSkyStackerPTB.rc
   DeepSkyStackerRO.rc
   DeepSkyStackerRU.rc
   DeepSkyStackerTR.rc   
)

set (
   HEADER_FILES 
   
   commonresource.h
   DSSMemory.h
   matrix.h 
   Resource.h
   resourceCZ.h
   resourceES.h 
   resourceFR.h 
   resourceIT.h
)

set (
   KERNEL_FILES
   
   AHDDemosaicing.cpp
   AHDDemosaicing.h
   BackgroundCalibration.cpp
   BackgroundCalibration.h
   BezierAdjust.h
   BitmapExt.cpp
   BitmapExt.h
   ChannelAlign.cpp
   ChannelAlign.h
   Common.h
   CosmeticEngine.cpp
   CosmeticEngine.h
   DarkFrame.cpp
   DarkFrame.h
   DeBloom.cpp
   DeBloom.h 
   DeepStack.cpp
   DeepStack.h 
   Delaunay.cpp
   Delaunay.h 
   DSSCommon.h 
   DSSProgress.h
   DSSTools.h 
   DSSVersion.h 
   EntropyInfo.cpp
   EntropyInfo.h
   Filters.cpp
   Filters.h 
   FITSUtil.cpp
   FITSUtil.h 
   FlatFrame.cpp
   FlatFrame.h 
   FrameInfo.cpp
   FrameInfo.h
   FrameList.cpp
   FrameList.h 
   Histogram.h 
   Library.cpp
   Library.h 
   MasterFrames.cpp
   MasterFrames.h 
   MatchingStars.cpp
   MatchingStars.h 
   MedianFilterEngine.h
   MultiBitmapProcess.cpp
   MultiBitmapProcess.h
   MultiTask.cpp
   MultiTask.h
   PixelTransform.h
   RAWUtils.cpp
   RAWUtils.h
   RegisterEngine.cpp
   RegisterEngine.h 
   RunningStackingEngine.cpp
   RunningStackingEngine.h
   Settings.cpp
   Settings.h
   StackedBitmap.cpp
   StackedBitmap.h
   StackingEngine.cpp
   StackingEngine.h
   StackingTasks.cpp
   StackingTasks.h 
   StarMask.cpp
   StarMask.h 
   Stars.h
   TIFFUtil.cpp
   TIFFUtil.h 
   Workspace.cpp
   Workspace.h

   ../DCRaw/dcraw.c
)
   
set (
   TOOLS_FILES
   
   ../Tools/BitmapSlider.cpp
   ../Tools/BitmapSlider.h
   ../Tools/BtnST.cpp
   ../Tools/BtnST.h 
   ../Tools/ButtonToolbar.cpp
   ../Tools/ButtonToolbar.h 
   ../Tools/cdxCRot90DC.cpp
   ../Tools/cdxCRot90DC.h 
   ../Tools/cdxCRotBevelLine.cpp
   ../Tools/cdxCRotBevelLine.h 
   ../Tools/cgfiltyp.cpp
   ../Tools/cgfiltyp.h 
   ../Tools/childprop.cpp
   ../Tools/childprop.h 
   ../Tools/ControlPos.cpp
   ../Tools/ControlPos.h
   ../Tools/CustomTabCtrl.cpp
   ../Tools/CustomTabCtrl.h 
   ../Tools/EasySize.h 
   ../Tools/FolderDlg.cpp 
   ../Tools/FolderDlg.h
   ../Tools/Gradient.cpp 
   ../Tools/Gradient.h 
   ../Tools/GradientCtrl.cpp 
   ../Tools/GradientCtrl.h 
   ../Tools/GradientCtrlImpl.cpp 
   ../Tools/GradientCtrlImpl.h 
   ../Tools/GradientCtrlImpl.cpp 
   ../Tools/Label.cpp 
   ../Tools/Label.h 
   ../Tools/ListViewCtrlEx.cpp 
   ../Tools/ListViewCtrlEx.h
   ../Tools/memdc.h  
   ../Tools/Registry.cpp 
   ../Tools/Registry.h  
   ../Tools/RegMFC.cpp  
   ../Tools/RichToolTipCtrl.cpp  
   ../Tools/RichToolTipCtrl.h   
   ../Tools/Schemadef.h   
   ../Tools/ScrollDialog.cpp  
   ../Tools/ScrollDialog.h   
   ../Tools/SmartPtr.h 
   ../Tools/SplitterControl.cpp  
   ../Tools/SplitterControl.h   
   ../Tools/ThemeUtil.cpp  
   ../Tools/ThemeUtil.h   
   ../Tools/Tmschema.h   
   ../Tools/ToolTipButton.cpp  
   ../Tools/ToolTipButton.h   
   ../Tools/WndImage.cpp  
   ../Tools/WndImage.h   
)

set (
   RESOURCE_FILES

   app.ico
   filelist.ico
   openfile.ico
   redo.bmp
   redo.ico
   settings.ico
   undo.ico

   Buttons/Base_Down.ico
   Buttons/Base_Hot.ico
   Buttons/Base_Up.ico
   
   Buttons/ButtonBase_down.bmp
   Buttons/ButtonBase_hot.bmp
   Buttons/ButtonBase_mask.bmp
   Buttons/ButtonBase_up.bmp

   Buttons/ButtonComet_Disabled.bmp
   Buttons/ButtonComet_Down.bmp
   Buttons/ButtonComet_Hot.bmp
   Buttons/ButtonComet_Up.bmp

   Buttons/ButtonSave_Disabled.bmp
   Buttons/ButtonSave_Down.bmp
   Buttons/ButtonSave_Hot.bmp
   Buttons/ButtonSave_Up.bmp

   Buttons/ButtonSelect_Disabled.bmp
   Buttons/ButtonSelect_Down.bmp
   Buttons/ButtonSelect_Hot.bmp
   Buttons/ButtonSelect_Up.bmp

   Buttons/ButtonStar_Disabled.bmp
   Buttons/ButtonStar_Down.bmp
   Buttons/ButtonStar_Hot.bmp
   Buttons/ButtonStar_Up.bmp

   res/4Corners.bmp
   
   res/Calibration_Linear.bmp
   res/Calibration_Rational.bmp

   res/Comet_Freeze.bmp
   res/Comet_Normal.bmp
   res/Comet_Trails.bmp

   res/GammaOff.bmp
   res/GammaOn.bmp

   res/hdrdown.bmp
   res/hdrup.bmp

   res/ImageList_CFA_CYMG.bmp
   res/ImageList_CFA_RGB.bmp
   res/ImageList_Color.bmp
   res/ImageList_GrayScale.bmp
   res/ImageList_Mask.bmp

   res/darkframe.ico
   res/flatframe.ico
   res/lightframe.ico
   res/offsetframe.ico

   res/asinh.bmp
   res/cuberoot.bmp
   res/linear.bmp
   res/linear_en.bmp
   res/linear_fr.bmp
   res/log.bmp
   res/loglog.bmp
   res/logsqrt.bmp
   res/sqrt.bmp

   res/custommode.bmp
   res/intersectionmode.bmp
   res/mosaicmode.bmp
   res/normalmode.bmp

   res/Pattern_BGGR.bmp
   res/Pattern_CYGMCYMG.bmp
   res/Pattern_GBRG.bmp
   res/Pattern_GRBG.bmp
   res/Pattern_RGGB.bmp

   res/RGBCalibration_Maximum.bmp
   res/RGBCalibration_Middle.bmp
   res/RGBCalibration_Minimum.bmp
   res/RGBCalibration_None.bmp

   res/StarShape_Bell.bmp
   res/StarShape_Cone.bmp
   res/StarShape_Cubic.bmp
   res/StarShape_Quadratic.bmp
   res/StarShape_TruncatedBell.bmp
   res/StarShape_TruncatedCone.bmp

   res/brazil.png
   res/Catalan.png
   res/Czech.png
   res/Dutch.png
   res/German.png
   res/Italian.png
   res/logo.png
   res/Portugal.png
   res/Romanian.png
   res/Russian.png
   res/Spanish.png
   res/Taiwan.png
   res/Turkish.png
)
   

# define the Visual Studio IDE source file groupings
   
if (MSVC)
   source_group ("Dialogs"                  FILES ${DIALOGS_FILES})
   source_group ("Dialogs\\ImageSinks"      FILES ${DIALOGS_IMAGESINKS_FILES})
   source_group ("Dialogs\\Library"         FILES ${DIALOGS_LIBRARY_FILES})
   source_group ("Dialogs\\Processing"      FILES ${DIALOGS_PROCESSING_FILES})
   source_group ("Dialogs\\Registering"     FILES ${DIALOGS_REGISTERING_FILES})
   source_group ("Dialogs\\Settings"        FILES ${DIALOGS_SETTINGS_FILES})
   source_group ("Dialogs\\Stacking"        FILES ${DIALOGS_STACKING_FILES})
   source_group ("Source Files"             FILES ${SOURCE_FILES})
   source_group ("Header Files"             FILES ${HEADER_FILES})
   source_group ("Kernel"                   FILES ${KERNEL_FILES})
   source_group ("Resource Files"           FILES ${RESOURCE_FILES})
   source_group ("Tools"                    FILES ${TOOLS_FILES})
endif (MSVC)

# define the source files used for building the executable target
# these will all appear in the IDE

add_executable (
   DeepSkyStacker 
   
   ${DIALOGS_FILES}
   ${DIALOGS_IMAGESINKS_FILES}
   ${DIALOGS_LIBRARY_FILES}
   ${DIALOGS_PROCESSING_FILES}
   ${DIALOGS_REGISTERING_FILES}
   ${DIALOGS_SETTINGS_FILES}
   ${DIALOGS_STACKING_FILES}
   ${SOURCE_FILES}
   ${HEADER_FILES}
   ${KERNEL_FILES}
   ${RESOURCE_FILES}
   ${TOOLS_FILES}
)

# define the header file search paths

target_include_directories (
   DeepSkyStacker 
   PRIVATE

   .
   ../Tools
   ../CFitsIO
   ../LibTIFF
   ../ZClass
)

# Name the precompiled header and generator source files

set_target_precompiled_header (
   DeepSkyStacker 
   
   stdafx.h 
   stdafx.cpp
)

# Exempt the files that don't reference the precompiled header

set (
   IGNORE_PCH_FILES

   ../DCRaw/dcraw.c
)

ignore_precompiled_header (${IGNORE_PCH_FILES})


# define preprocessor variables

target_compile_definitions (
   DeepSkyStacker
   
   PRIVATE 
   
   _AFXDLL 
   _MBCS

   WIN32
   NDEBUG                      # this is build type specific - sort out later
   _CRT_SECURE_NO_DEPRECATE
   USE_LIBTIFF_STATIC
)

# suppress warnings on selected 3rd party sources

if (MSVC)
   update_source_files_compile_flags (
      ../DCRaw/dcraw.c

      "/wd4005 /wd4013 /wd4018 /wd4133 /wd4146 /wd4244 /wd4267 /wd4305 /wd4996"
   )
endif (MSVC)

# Declare the libraries that are linked with the target

target_link_libraries ( 
   DeepSkyStacker
   
   libjpg
   libtiff
   ZCLass
   datasource
   debughlp
   ${DSS_PREBUILT_LIBRARY_PATH}/cfitsio.lib
   ${DSS_PREBUILT_LIBRARY_PATH}/libpng16.lib
   ${DSS_PREBUILT_LIBRARY_PATH}/zlibstat.lib
   )
