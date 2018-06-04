cmake_minimum_required (VERSION 3.10.0) # make sure the cmake functions we use are supported

# Name the master build artifact (make file, IDE file etc.)

project (DeepSkyStacker.cmake)

# Load our custom cmake modules that we need for this build (add our module location
# to cmake's search path and then include the required modules)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (precompiled_headers)     # functions for managing precompiled headers
include (update_compile_flags)    # function for setting compiler options
include (enable_parallel_builds)  # function for user settable parallel build option
include (configure_ide_options)   # functions for setting build configurations and ide behaviour

# Limit the allowed build configurations

set_build_configurations ("Debug;Release")

# Enable IDE (nested) source file folders

enable_ide_source_file_folders ()

# Let user choose if multiprocessor builds should be supported

add_option_for_parallel_build (DSS_ENABLE_PARALLEL_BUILD)

# Generate the IDE file with following projects - each build definition is defined in the referenced
# source file subdirectory

add_subdirectory (DeepSkyStacker     DeepSkyStacker)
add_subdirectory (DeepSkyStackerCL   DeepSkyStackerCL)
add_subdirectory (DeepSkyStackerLive DeepSkyStackerLive)
