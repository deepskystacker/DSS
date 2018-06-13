# define source files in intended IDE groupings
   
set (
   SOURCE_FILES 
      
   stdafx.cpp
   
   BufferDataSource.cpp
   BufferDataSource.h
   DataSourceABC.cpp
   DataSourceABC.h
   FileDataSource.cpp
   FileDataSource.h
)

set (
   HEADER_FILES 
   
   stdafx.h

   targetver.h
)

# define the Visual Studio IDE source file groupings
   
if (MSVC)
   source_group ("Source Files" FILES ${SOURCE_FILES})
   source_group ("Header Files" FILES ${HEADER_FILES})
endif (MSVC)

# define the source files used for building the library target
# these will all appear in the IDE

add_library (
   datasource 
   
   STATIC
   
   ${SOURCE_FILES}
   ${HEADER_FILES}
)

# Organise the library in the IDE

set_target_properties (
   datasource 

   PROPERTIES

   FOLDER "Libraries/QHTML Libs") 

# define the header file search paths

target_include_directories (
   datasource 
   PRIVATE

   .
   ..
)

# define preprocessor variables

target_compile_definitions (
   datasource 
   
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
   datasource  
   
   stdafx.h 
   stdafx.cpp
)

# suppress warnings on selected 3rd party sources

##if (MSVC)
##   update_source_files_compile_flags (
##      tif_unix.c
##
##      "/wd4028 /wd4996"
##   )
##endif (MSVC)
