#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "lcms2::lcms2" for configuration "Debug"
set_property(TARGET lcms2::lcms2 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(lcms2::lcms2 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/lcms2.lib"
  )

list(APPEND _cmake_import_check_targets lcms2::lcms2 )
list(APPEND _cmake_import_check_files_for_lcms2::lcms2 "${_IMPORT_PREFIX}/debug/lib/lcms2.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
