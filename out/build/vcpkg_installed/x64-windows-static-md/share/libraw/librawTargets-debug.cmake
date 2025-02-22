#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libraw::raw" for configuration "Debug"
set_property(TARGET libraw::raw APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libraw::raw PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/manual-link/rawd.lib"
  )

list(APPEND _cmake_import_check_targets libraw::raw )
list(APPEND _cmake_import_check_files_for_libraw::raw "${_IMPORT_PREFIX}/debug/lib/manual-link/rawd.lib" )

# Import target "libraw::raw_r" for configuration "Debug"
set_property(TARGET libraw::raw_r APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libraw::raw_r PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/raw_rd.lib"
  )

list(APPEND _cmake_import_check_targets libraw::raw_r )
list(APPEND _cmake_import_check_files_for_libraw::raw_r "${_IMPORT_PREFIX}/debug/lib/raw_rd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
