cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
message("CMake version: " ${CMAKE_VERSION})

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
cmake_policy(SET CMP0115 NEW)

if (APPLE)
set(CMAKE_OSX_DEPLOYMENT_TARGET 13.4 CACHE STRING "Minimum operating system version for deployment" FORCE)
set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "VH8AYT3C7Y" CACHE STRING "")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "Developer ID Application: David Partridge (VH8AYT3C7Y)" CACHE STRING "")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Manual" CACHE STRING "")
set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED TRUE CACHE BOOL "")
endif()

#
# Before setting the VCPKG toolchain up, add the CMAKE_PREFIX_PATH from the command line
# to the environment variable of the same name so it can be picked up for "inner builds"
#
set(ENV{CMAKE_PREFIX_PATH} ${CMAKE_PREFIX_PATH} $ENV{CMAKE_PREFIX_PATH})
message("CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH})
message("EV CMAKE_PREFIX_PATH: " $ENV{CMAKE_PREFIX_PATH})

set(CMAKE_TOOLCHAIN_FILE 
	"$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
	)

if(WIN32)
set(VCPKG_TARGET_TRIPLET
	"x64-windows-static-md"
	CACHE STRING "")
endif()
message ("VCPKG_TARGET_TRIPLET: " ${VCPKG_TARGET_TRIPLET})

# Require C++23 and enable extensions for all targets.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

project(DeepSkyStacker VERSION 6.1.3 LANGUAGES C CXX)

message ("System Name is: " ${CMAKE_SYSTEM_NAME})
message ("Prefix path: " ${CMAKE_PREFIX_PATH})

set (PLATFORM_NAME "x64" CACHE STRING "" FORCE )
if (APPLE)
	if ("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "arm64")
	set (PLATFORM_NAME "arm64" CACHE STRING "" FORCE)
	endif()
endif()

#
# install to DSS/x64 for Windows, and DSS/platform/x64 for other platforms
#
if(NOT CMAKE_BUILD_TYPE)
	message ("Build type was not set, setting it to: Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message ("CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})
message ("PLATFORM_NAME: " ${PLATFORM_NAME})
message ("CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	if(NOT WIN32)
		set(CMAKE_INSTALL_PREFIX $ENV{HOME}/build/${CMAKE_SYSTEM_NAME}/${PLATFORM_NAME}/${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
	else()
		set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/${PLATFORM_NAME}/${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
	endif()
endif()
message ("CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})
file (MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX})

set(CMAKE_INSTALL_BINDIR "." CACHE STRING "" FORCE)
set(CMAKE_INSTALL_LIBDIR "." CACHE STRING "" FORCE)
set(CMAKE_INSTALL_LIBEXECDIR "." CACHE STRING "" FORCE)
set(CMAKE_INSTALL_PLUGINSDIR "." CACHE STRING "" FORCE)

find_package(Qt6 6.10.0 REQUIRED COMPONENTS Core Gui Widgets Network Charts DBus OpenGLWidgets Concurrent LinguistTools Svg PrintSupport)

#set(CMAKE_FIND_DEBUG_MODE 1)
find_package(cfitsio REQUIRED)
find_package(boost_align CONFIG REQUIRED)
find_package(boost_container CONFIG REQUIRED)
find_package(boost_interprocess CONFIG REQUIRED)
if (APPLE)
find_package(boost_atomic CONFIG REQUIRED)	
find_package(Iconv REQUIRED)
find_package(CURL REQUIRED)
endif()
find_package(exiv2 CONFIG REQUIRED)
find_package(expat CONFIG REQUIRED)
find_package(TIFF 4.7.1 REQUIRED)
find_package(libraw CONFIG REQUIRED)
find_package(SmtpMime 2.0 REQUIRED)
if(NOT WIN32)
find_package(OpenMP REQUIRED)
find_package (Threads REQUIRED)
endif()
find_package(ZLIB REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS
	"./ui"
	CACHE STRING "" FORCE
	)

if(APPLE)
  include_directories(SYSTEM /usr/local/include)
endif()
	
################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

if(WIN32)
	set(CMAKE_CXX_FLAGS "/GR /EHsc")
endif()
if (APPLE)
	set(CMAKE_CXX_FLAGS "-g -Wno-switch-enum -Wno-switch")
#	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#		set (CMAKE_CXX_FLAGS "-g -Wno-switch-enum -Wno-switch -fsanitize=address -fsanitize=undefined -D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG")
#	endif()
	message("MacOS CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
endif()
if(LINUX)
	set(CMAKE_CXX_FLAGS "-g -Wno-ignored-attributes -msse -msse2 -mfpmath=sse -pthread")
endif()

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(qwt-6.3.0)
add_subdirectory(ZClass)
add_subdirectory(DeepSkyStackerKernel)
# 
# DeepSkyStackerCL must be built before DeepSkyStacker as we copy the executable
# into the DeepSkyStacker folder/MACOSX bundle
#
add_subdirectory(DeepSkyStackerCL)
add_subdirectory(DeepSkyStacker)
add_subdirectory(DeepSkyStackerLive)
add_subdirectory(DeepSkyStackerTest)

