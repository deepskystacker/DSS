# define source files in intended groupings

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
   
   PCLTools.cpp
   PCLTools.h
   SetUILanguage.cpp
   SetUILanguage.h
   StdAfx.cpp
   StdAfx.h
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
   ../DCRaw/dcraw.c
   DeBloom.cpp
   DeBloom.h 
   DeepStack.cpp
   DeepStack.h 
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
   Delaunay.cpp
   Delaunay.h 
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

# define the IDE source file groupings
   
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
source_group ("Tools"                    FILES ${TOOLS_FILES})


# define the sources for the executable target


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
   ${TOOLS_FILES}
   )