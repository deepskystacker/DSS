#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "expat::expat" for configuration "Release"
set_property(TARGET expat::expat APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(expat::expat PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libexpatMD.lib"
  )

list(APPEND _cmake_import_check_targets expat::expat )
list(APPEND _cmake_import_check_files_for_expat::expat "${_IMPORT_PREFIX}/lib/libexpatMD.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
