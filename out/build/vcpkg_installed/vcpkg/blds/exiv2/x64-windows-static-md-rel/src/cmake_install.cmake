# Install script for directory: D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/pkgs/exiv2_x64-windows-static-md")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/lib/exiv2.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/exiv2" TYPE FILE FILES
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/basicio.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/bmffimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/bmpimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/config.h"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/convert.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/cr2image.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/crwimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/datasets.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/easyaccess.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/epsimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/error.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/exif.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/exiv2.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/futils.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/gifimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/image.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/image_types.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/iptc.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/jp2image.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/jpgimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/metadatum.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/mrwimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/orfimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/pgfimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/photoshop.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/preview.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/properties.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/psdimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/rafimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/rw2image.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/slice.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/tags.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/tgaimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/tiffimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/types.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/value.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/version.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/webpimage.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/xmp_exiv2.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/src/v0.28.3-747a4cd3ad.clean/src/../include/exiv2/xmpsidecar.hpp"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/exv_conf.h"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/exiv2lib_export.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2/exiv2Export.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2/exiv2Export.cmake"
         "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/src/CMakeFiles/Export/7c214f9ecc114891e0d2d508bbbeac60/exiv2Export.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2/exiv2Export-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2/exiv2Export.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2" TYPE FILE FILES "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/src/CMakeFiles/Export/7c214f9ecc114891e0d2d508bbbeac60/exiv2Export.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2" TYPE FILE FILES "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/src/CMakeFiles/Export/7c214f9ecc114891e0d2d508bbbeac60/exiv2Export-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/exiv2" TYPE FILE FILES
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/src/exiv2ConfigVersion.cmake"
    "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/src/exiv2Config.cmake"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/Github/DSS/out/build/vcpkg_installed/vcpkg/blds/exiv2/x64-windows-static-md-rel/src/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
