cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
set(CMAKE_TOOLCHAIN_FILE 
	"$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
	)

if(WIN32)
set(VCPKG_TARGET_TRIPLET
	"x64-windows-static-md"
	CACHE STRING "")
endif()
message ("VCPKG_TARGET_TRIPLET: " ${VCPKG_TARGET_TRIPLET})

# Require C++20 and disable extensions for all targets.
# NOTE: See further below for how to do this more robustly.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(DeepSkyStacker VERSION 5.1.8 LANGUAGES C CXX)

message ("System Name is: " ${CMAKE_SYSTEM_NAME})
message ("Prefix path: " ${CMAKE_PREFIX_PATH})

#
# Report platform name if speficied
#
if(NOT CMAKE_VS_PLATFORM_NAME)
    message("CMAKE_VS_PLATFORM_NAME not specified")
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
		set(CMAKE_VS_PLATFORM_NAME "arm64")
	else()
		set(CMAKE_VS_PLATFORM_NAME "x64")
	endif()
endif()
message("CMAKE_VS_PLATFORM_NAME set to: " ${CMAKE_VS_PLATFORM_NAME})

#
# install to DSS/x64 for Windows, and DSS/platform/x64 for other platforms
#
if(NOT CMAKE_BUILD_TYPE)
	message ("Build type was not set, setting it to: Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message ("CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})
message ("CMAKE_VS_PLATFORM_NAME: " ${CMAKE_VS_PLATFORM_NAME})
message ("CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	if(NOT WIN32)
		set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
	else()
		set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
	endif()
endif()
message ("CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})

set(CMAKE_INSTALL_BINDIR "." CACHE STRING "" FORCE)
set(CMAKE_INSTALL_LIBDIR "." CACHE STRING "" FORCE)
set(CMAKE_INSTALL_LIBEXECDIR "." CACHE STRING "" FORCE)
set(CMAKE_INSTALL_PLUGINSDIR "." CACHE STRING "" FORCE)

find_package(Qt6 6.8.2 REQUIRED COMPONENTS Core Gui Widgets Network Charts DBus OpenGLWidgets LinguistTools)

#set(CMAKE_FIND_DEBUG_MODE 1)
#
# Non-Windows use find_package to find stuff we want
#
#set(cfitsio_DIR "/usr/local/lib/cfitsio-4.5.0/")
find_package(cfitsio 4.5.0 REQUIRED)
find_package(boost_container CONFIG REQUIRED)
find_package(boost_interprocess CONFIG REQUIRED)
if (APPLE)
find_package(Iconv REQUIRED)
find_package(CURL REQUIRED)
endif()
find_package(exiv2 CONFIG REQUIRED)
find_package(expat CONFIG REQUIRED)
find_package(TIFF 4.7 REQUIRED)
find_package(libraw CONFIG REQUIRED)
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
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(CMAKE_CXX_FLAGS "-Wno-switch-enum -Wno-switch")
endif()
if(LINUX)
	set(CMAKE_CXX_FLAGS "-Wno-ignored-attributes -msse -msse2 -mfpmath=sse -pthread")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(DeepSkyStackerKernel)
add_subdirectory(ZClass)
add_subdirectory(DeepSkyStacker)
add_subdirectory(DeepSkyStackerCL)
add_subdirectory(DeepSkyStackerLive)
#[=[
add_subdirectory(DeepSkyStackerTest)
]=]

