# define source files in intended groupings

set (
   SOURCE_FILES 
   
   DeepSkyStacker.cpp
   )

set (
   KERNEL_FILES
   
   AHDDemosaicing.cpp
   )
   
set (
   DIALOGS_FILES
   
   About.cpp
   )
   
set (
   DIALOGS_IMAGESINKS_FILES
   
   ImageSinks.cpp
   )
   
set (
   DIALOGS_LIBRARY_FILES
   
   LibraryDlg.cpp
   )
   
set (
   DIALOGS_PROCESSING_FILES
   
   LuminanceTab.cpp
   )
   
set (
   DIALOGS_REGISTERING_FILES
   
   AskRegistering.cpp
   )
   
set (
   DIALOGS_SETTINGS_FILES
   
   FitsFilesTab.cpp
   )
   
set (
   DIALOGS_STACKING_FILES
   
   AlignmentParameters.cpp
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
source_group ("Kernel"                   FILES ${KERNEL_FILES})


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
   ${KERNEL_FILES}
   )