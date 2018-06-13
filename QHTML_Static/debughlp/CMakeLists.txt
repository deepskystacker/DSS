# define source files in intended IDE groupings
   
set (
   SOURCE_FILES 
      
   StdAfx.cpp
   
   ApiFailure.cpp
   AssertFailed.cpp
   DebugHlp.cpp
   Trace.cpp
   vmalprintf.cpp
)

set (
   HEADER_FILES 
   
   StdAfx.h

   DebugHlp.h
   
   ../reuse/winhelper.h
)

# define the Visual Studio IDE source file groupings
   
if (MSVC)
   source_group ("Source Files" FILES ${SOURCE_FILES})
   source_group ("Header Files" FILES ${HEADER_FILES})
endif (MSVC)

# define the source files used for building the library target
# these will all appear in the IDE

add_library (
   debughlp 
   
   STATIC
   
   ${SOURCE_FILES}
   ${HEADER_FILES}
)

# Organise the library in the IDE

set_target_properties (
   debughlp 

   PROPERTIES

   FOLDER "Libraries/QHTML Libs") 

# define the header file search paths

target_include_directories (
   debughlp 
   PRIVATE

   .
   ..
)

# define preprocessor variables

target_compile_definitions (
   debughlp 
   
   PRIVATE 
   
   WIN32
   NDEBUG
   USE_LIBTIFF_STATIC
   ZLIB_WINAPI
   _CRT_SECURE_NO_WARNINGS
   
##   _UNICODE
##   UNICODE
)

# Name the precompiled header and generator source files

set_target_precompiled_header (
   debughlp 
   
   StdAfx.h 
   StdAfx.cpp
)

# suppress warnings on selected 3rd party sources

##if (MSVC)
##   update_source_files_compile_flags (
##      tif_unix.c
##
##      "/wd4028 /wd4996"
##   )
##endif (MSVC)
