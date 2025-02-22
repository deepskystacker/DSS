#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libjpeg-turbo::turbojpeg-static" for configuration "Debug"
set_property(TARGET libjpeg-turbo::turbojpeg-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libjpeg-turbo::turbojpeg-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "ASM_NASM;C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/turbojpeg.lib"
  )

list(APPEND _cmake_import_check_targets libjpeg-turbo::turbojpeg-static )
list(APPEND _cmake_import_check_files_for_libjpeg-turbo::turbojpeg-static "${_IMPORT_PREFIX}/debug/lib/turbojpeg.lib" )

# Import target "libjpeg-turbo::jpeg-static" for configuration "Debug"
set_property(TARGET libjpeg-turbo::jpeg-static APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(libjpeg-turbo::jpeg-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "ASM_NASM;C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/jpeg.lib"
  )

list(APPEND _cmake_import_check_targets libjpeg-turbo::jpeg-static )
list(APPEND _cmake_import_check_files_for_libjpeg-turbo::jpeg-static "${_IMPORT_PREFIX}/debug/lib/jpeg.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
