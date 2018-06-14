cmake_minimum_required (VERSION 3.10.0) # make sure the cmake functions we use are supported

# Name the master build artifact (make file, IDE file etc.)

project (DeepSkyStacker.cmake)

message (STATUS "Target platform: ${CMAKE_GENERATOR}")

# Load our custom cmake modules that we need for this build (add our module location
# to cmake's search path and then include the required modules)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (precompiled_headers)       # functions for managing precompiled headers
include (update_compile_flags)      # function for setting compiler options
include (enable_parallel_builds)    # function for user settable parallel build option
include (configure_ide_options)     # functions for setting target build configurations and ide behaviour
include (set_dss_build_directories) # function for setting build directories used by DSS based on generator and config

# Limit the allowed build configurations

set_ide_build_configurations ("Debug;Release")

# Set the build directory paths

set_dss_build_directories (DSS_PREBUILT_LIBRARY_PATH)

# Enable IDE (nested) source file folders

enable_ide_source_file_folders ()

# Let user choose if multiprocessor builds should be supported

add_option_for_parallel_build (DSS_ENABLE_PARALLEL_BUILD)

# Generate the IDE file with following projects - each build definition is defined in the referenced
# source file subdirectory

add_subdirectory (libjpg                  libjpg)                    # library
add_subdirectory (libtiff                 libtiff)                   # library
add_subdirectory (QHTML_static/datasource datasource)                # library
add_subdirectory (QHTML_static/debughlp   debughlp)                  # library
add_subdirectory (QHTML_static/guitools   guitools)                  # library
add_subdirectory (QHTML_static/imglib     imglib)                    # library
add_subdirectory (ZCLass                  ZCLass)                    # library
add_subdirectory (DeepSkyStacker          DeepSkyStacker)            # program
add_subdirectory (DeepSkyStackerCL        DeepSkyStackerCL)          # program
add_subdirectory (DeepSkyStackerLive      DeepSkyStackerLive)        # program
