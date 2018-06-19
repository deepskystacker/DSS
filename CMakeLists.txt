cmake_minimum_required (VERSION 3.10.0) # make sure the cmake functions we use are supported

# Name the master build artifact (make file, IDE file etc.)

project (DeepSkyStacker.cmake)

message (STATUS "Target IDE: ${CMAKE_GENERATOR}")

if ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
   set (DSS_64_BIT ON)
else ()
   set (DSS_32_BIT ON)
endif ()

# Load our custom cmake modules that we need for this build (add our module location
# to cmake's search path and then include the required modules)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include (precompiled_headers)              # functions for managing precompiled headers
include (update_compile_flags)             # function for setting compiler and linker options
include (enable_parallel_builds)           # function for user settable parallel build option
include (enable_release_with_debug_builds) # function for user settable release with debug info build option
include (configure_ide_options)            # functions for setting target build configurations and ide behaviour
include (set_dss_build_directories)        # function for setting build directories used by DSS based on generator and config

# Let user choose if multiprocessor builds should be supported
# Let user choose if release with debug info builds should be supported

add_option_for_parallel_build (DSS_ENABLE_PARALLEL_BUILD)
add_option_for_relwithdebuginfo_build (DSS_INCLUDE_RELEASE_WITH_DEBUG_INFO_CONFIG)

# Limit the allowed build configurations

if (DSS_INCLUDE_RELEASE_WITH_DEBUG_INFO_CONFIG)
   set_ide_build_configurations ("Debug;Release;RelWithDebugInfo")
else ()
   set_ide_build_configurations ("Debug;Release")
endif ()

# Set the build directory paths

set_dss_build_directories (DSS_PREBUILT_LIBRARY_PATH)

# Enable IDE nested source file folders

enable_ide_source_file_folders ()


# Set compiler and linker options common to all projects

add_compile_options ("/EHsc")                                    # Exception handling is synchronous (/EHa has time/space and changed behavour of catch () consequences)
add_compile_options ("/D WIN32")                                 # Select Windows specific conditional code
add_compile_options ("/D _WINDOWS")                              # Identifies target platform is Windows
add_compile_options ("$<$<CONFIG:Debug>:/D _DEBUG>")             # Select debug config specific conditional code
add_compile_options ("$<$<CONFIG:Release>:/D NDEBUG>")           # Select release config specific conditional code
add_compile_options ("$<$<CONFIG:RelWithDebugInfo>:/D NDEBUG>")  # Select release config specific conditional code
add_compile_options ("/D _UNICODE")                              # Select unicode specific conditional code
add_compile_options ("/D UNICODE")                               # Select unicode specific conditional code

                                                                 # The function add_link_options is coming in cmake 3.12.x ... meantime use our custom function ...
update_link_flags ("/LTCG")                                      # Perform whole program optimisation at link time (Link Time Code Generation)
update_link_flags ("/OPT:REF")                                   # Have the linker remove unreferenced code
update_link_flags ("/OPT:ICF")                                   # Have the linker remove redundant (duplicate) data sections (COMDAT)

if (DSS_32_BIT)
   update_link_flags ("/LARGEADDRESSAWARE")                      # Maximise the address space for 32 bit builds
   update_link_flags ("/SAFESEH")                                # Security feature for 32 bit builds - stops 'evil' exception handler injection attacks
endif ()

if (DSS_64_BIT)
endif ()

# Generate the IDE file with following projects - each build definition is defined in the referenced
# source file subdirectory

add_subdirectory (libjpg                  libjpg)                    # library
add_subdirectory (libtiff                 libtiff)                   # library
add_subdirectory (QHTML_static/datasource datasource)                # library
add_subdirectory (QHTML_static/debughlp   debughlp)                  # library
add_subdirectory (QHTML_static/guitools   guitools)                  # library
add_subdirectory (QHTML_static/imglib     imglib)                    # library
add_subdirectory (QHTML_static/qhtm       qhtm)                      # library
add_subdirectory (ZCLass                  ZCLass)                    # library
add_subdirectory (DeepSkyStacker          DeepSkyStacker)            # program
add_subdirectory (DeepSkyStackerCL        DeepSkyStackerCL)          # program
add_subdirectory (DeepSkyStackerLive      DeepSkyStackerLive)        # program
