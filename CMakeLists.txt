cmake_minimum_required (VERSION 3.10.0)

# Name the master build artifact (make file, IDE file etc.)

project (DeepSkyStacker.cmake)

# Limit the allowed build configurations

set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Enable IDE (nested) source file folders

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Choose if multiprocessor builds should be supported

if (MSVC)
   set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP")
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
   message (STATUS "Added parallel build arguments to CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
   message (STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif ()


# Define support functions for file option

   # Update Compiler Flag properties

function (UPDATE_SOURCE_FILES_COMPILE_FLAGS SourceFiles CompileFlags)
   foreach (filename ${SourceFiles})
      get_source_file_property (OLD_FLAGS ${filename} COMPILE_FLAGS)

      if (${OLD_FLAGS} STREQUAL "NOTFOUND")
         set_source_files_properties (${filename} PROPERTIES COMPILE_FLAGS "${CompileFlags}")
      else ()
         set_source_files_properties (${filename} PROPERTIES COMPILE_FLAGS "${OLD_FLAGS} ${CompileFlags}")
      endif ()
   endforeach (filename)   
endfunction (UPDATE_SOURCE_FILES_COMPILE_FLAGS)


# Define support functions for precompiled headers

   # set PCH for a VS project (Target)

function (SET_TARGET_PRECOMPILED_HEADER Target PrecompiledHeader PrecompiledSource)
   if (MSVC)
      set_target_properties (
         ${Target} 
         PROPERTIES 
        
         COMPILE_FLAGS "/Yu${PrecompiledHeader}"
      )
      set_source_files_properties (
         ${PrecompiledSource} 
         PROPERTIES 
        
         COMPILE_FLAGS "/Yc${PrecompiledHeader}"
      )
   endif (MSVC)
endfunction(SET_TARGET_PRECOMPILED_HEADER)

   # ignore PCH for a specified list of files

function (IGNORE_PRECOMPILED_HEADER SourceFiles)
   if (MSVC)  
      foreach (filename ${SourceFiles})
         update_source_files_compile_flags (${filename} "/Y-")
      endforeach (filename)
   endif (MSVC)
endfunction(IGNORE_PRECOMPILED_HEADER)




# Generate the IDE file with following projects - each build definition is defined in the referenced
# source file subdirectory

add_subdirectory (DeepSkyStacker     DeepSkyStacker)
add_subdirectory (DeepSkyStackerCL   DeepSkyStackerCL)
add_subdirectory (DeepSkyStackerLive DeepSkyStackerLive)
