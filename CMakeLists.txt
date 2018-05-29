cmake_minimum_required (VERSION 3.10.0)

project (DeepSkyStacker.cmake)

set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory (DeepSkyStacker     DeepSkyStacker)
add_subdirectory (DeepSkyStackerCL   DeepSkyStackerCL)
add_subdirectory (DeepSkyStackerLive DeepSkyStackerLive)

#add_executable (DeepSkyStackerCL)
#add_executable (DeepSkyStackerLive)