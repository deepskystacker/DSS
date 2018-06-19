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
   
source_group ("Source Files" FILES ${SOURCE_FILES})
source_group ("Header Files" FILES ${HEADER_FILES})

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
   
   _CRT_SECURE_NO_WARNINGS
   _LIB
   USE_LIBTIFF_STATIC
   ZLIB_WINAPI
)

# Name the precompiled header and generator source files

set_target_precompiled_header (
   debughlp 
   
   StdAfx.h 
   StdAfx.cpp
)
