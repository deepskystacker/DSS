cmake_minimum_required (VERSION 3.10.0)

# Name the master build artifact (make file, IDE file etc.)

project (DeepSkyStacker.cmake)

# Set allowed build configurations

set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Enable IDE (nested) source file folders

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Generate the build parts - each build definition defined in the referenced
# source file subdirectory

add_subdirectory (DeepSkyStacker     DeepSkyStacker)
add_subdirectory (DeepSkyStackerCL   DeepSkyStackerCL)
add_subdirectory (DeepSkyStackerLive DeepSkyStackerLive)
