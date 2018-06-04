cmake_minimum_required (VERSION 3.10.0)

# Name the master build artifact (make file, IDE file etc.)

project (DeepSkyStacker.cmake)

# Load our custom cmake modules that we need for this build

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (precompiled_headers)     # functions for managing precompiled headers
include (update_compile_flags)    # function for setting compiler options
include (enable_parallel_builds)  # function for user settable parallel build option

# Limit the allowed build configurations

set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Enable IDE (nested) source file folders

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Let user choose if multiprocessor builds should be supported

add_option_for_parallel_build (DSS_ENABLE_PARALLEL_BUILD)


# Generate the IDE file with following projects - each build definition is defined in the referenced
# source file subdirectory

add_subdirectory (DeepSkyStacker     DeepSkyStacker)
add_subdirectory (DeepSkyStackerCL   DeepSkyStackerCL)
add_subdirectory (DeepSkyStackerLive DeepSkyStackerLive)
