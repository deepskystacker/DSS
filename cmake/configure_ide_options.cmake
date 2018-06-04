# Set the build configurations

function (SET_BUILD_CONFIGURATIONS ConfigList)
   set (CMAKE_CONFIGURATION_TYPES ${ConfigList} CACHE STRING "" FORCE)
endfunction (SET_BUILD_CONFIGURATIONS)

# Allow source files to appear to be in (nested) subfolders

function (ENABLE_IDE_SOURCE_FILE_FOLDERS)
   set_property (GLOBAL PROPERTY USE_FOLDERS ON)
endfunction (ENABLE_IDE_SOURCE_FILE_FOLDERS)
