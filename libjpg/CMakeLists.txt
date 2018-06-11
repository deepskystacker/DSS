# define source files in intended IDE groupings
   
set (
   SOURCE_FILES 
   
   jcapimin.c
   jcapistd.c
   jccoefct.c
   jccolor.c
   jcdctmgr.c
   jchuff.c
   jcinit.c
   jcmainct.c
   jcmarker.c
   jcmaster.c
   jcomapi.c
   jcparam.c
   jcphuff.c
   jcprepct.c
   jcsample.c
   jctrans.c
   jdapimin.c
   jdapistd.c
   jdatadst.c
   jdatasrc.c
   jdcolor.c
   jddctmgr.c
   jdhuff.c
   jdinput.c
   jdmainct.c
   jdmarker.c
   jdmaster.c
   jdmerge.c
   jdphuff.c
   jdpostct.c
   jdsample.c
   jdtrans.c
   jerror.c
   jfdctflt.c
   jfdctfst.c
   jfdctint.c
   jidctflt.c
   jidctfst.c
   jidctint.c
   jidctred.c
   jmemmgr.c
   jmemnobs.c
   jquant1.c
   jquant2.c
   jutils.c
)

set (
   HEADER_FILES 
   
   jchuff.h
   jconfig.h
   jdct.h
   jdhuff.h
   jerror.h
   jinclude.h
   jmemsys.h
   jmorecfg.h
   jpegint.h
   jpeglib.h
   jversion.h
   
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
   libjpg 
   
   STATIC
   
   ${SOURCE_FILES}
   ${HEADER_FILES}
)

# define the header file search paths

target_include_directories (
   libjpg 
   PRIVATE

   .
)

# define preprocessor variables

target_compile_definitions (
   libjpg 
   
   PRIVATE 
   
   WIN32
   NDEBUG
   _LIB
   _CRT_SECURE_NO_WARNINGS
   
##   _UNICODE
##   UNICODE
)
