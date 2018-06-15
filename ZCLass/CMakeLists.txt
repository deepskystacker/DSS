# define source files in intended IDE groupings
   
set (
   SOURCE_FILES 
   
   source/ZExcBase.cpp
   source/ZExcept.cpp
   source/zmstrlck.cpp
   source/zreslock.cpp
   source/ZTrace.cpp
)

set (
   HEADER_FILES 
   
   zdefs.h
   zexcbase.h
   zexcept.h
   zmstrlck.h
   zptr.h
   zreslock.h
   zsingle.h
   Ztrace.h
)

# define the Visual Studio IDE source file groupings
   
source_group ("Source Files" FILES ${SOURCE_FILES})
source_group ("Header Files" FILES ${HEADER_FILES})

# define the source files used for building the library target
# these will all appear in the IDE

add_library (
   ZCLass 
   
   STATIC
   
   ${SOURCE_FILES}
   ${HEADER_FILES}
)

# Organise the library in the IDE

set_target_properties (
   ZCLass

   PROPERTIES

   FOLDER "Libraries") 

# define the header file search paths

target_include_directories (
   ZCLass 
   PRIVATE

   .
)

# define preprocessor variables

target_compile_definitions (
   ZCLass 
   
   PRIVATE 
   
   WIN32
   NDEBUG
   _LIB
   _CRT_SECURE_NO_WARNINGS
   
##   _UNICODE
##   UNICODE
)
