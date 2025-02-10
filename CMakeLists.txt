cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(DeepSkyStacker VERSION 5.1.8 LANGUAGES C CXX)

# Require C++20 and disable extensions for all targets.
# NOTE: See further below for how to do this more robustly.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(CMAKE_PREFIX_PATH
	"C:/Qt/6.8.0/msvc2022_64"
	CACHE STRING "" FORCE
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	message ("WIN32 is set to ${WIN32}, LINUX is set to ${LINUX}")
	if(WIN32)
	    message(FATAL_ERROR "System name is set to ${CMAKE_SYSTEM_NAME} but WIN32 is set to ${WIN32}")
	endif()
else()
    message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} not yet supported!")
endif()
message ("System Name is: " ${CMAKE_SYSTEM_NAME} ", prefix path: " ${CMAKE_PREFIX_PATH})

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Gui Widgets Network Charts LinguistTools)

set(CMAKE_AUTOUIC_SEARCH_PATHS
	"./ui"
	CACHE STRING "" FORCE
	)
	
################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

#
# install to DSS/x64 for Windows, and DSS/platform/x64 for other platforms
#
if(NOT CMAKE_BUILD_TYPE)
	message ("Build type was not set, setting it to: Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	if(NOT WIN32)
		set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/${CMAKE_SYSTEM_NAME}/${CMAKE_VS_PLATFORM_NAME}/${CMAKE_BUILD_TYPE})
	else()
		set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/${CMAKE_VS_PLATFORM_NAME}/${CMAKE_BUILD_TYPE})
	endif()
endif()
message ("Install prefix set to: " ${CMAKE_INSTALL_PREFIX})

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

if(WIN32)
	set(CMAKE_CXX_FLAGS
		"/GR /EHsc"
		CACHE STRING "" FORCE
		)
else()
	set(CMAKE_CXX_FLAGS "" CACHE STRING "" FORCE)

	add_compile_options(-mavx2)
	add_compile_options(-mfma)
	add_link_options(-Wl,--copy-dt-needed-entries)
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
get_filename_component(LIBRAW_PATH LibRaw ABSOLUTE)
add_subdirectory(LibRaw-cmake)
add_subdirectory(SmtpClient-for-Qt)
add_subdirectory(ZClass)
add_subdirectory(DeepSkyStacker)
add_subdirectory(DeepSkyStackerCL)
add_subdirectory(DeepSkyStackerLive)
# add_subdirectory(DeepSkyStackerTest)
