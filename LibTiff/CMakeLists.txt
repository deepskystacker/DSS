# define source files in intended IDE groupings
   
set (
   SOURCE_FILES 
   
   ##mkg3states.c
   tif_aux.c
   tif_close.c
   tif_codec.c
   tif_color.c
   tif_compress.c
   tif_dir.c
   tif_dirinfo.c
   tif_dirread.c
   tif_dirwrite.c
   tif_dumpmode.c
   tif_error.c
   tif_extension.c
   tif_fax3.c
   tif_fax3sm.c
   tif_flush.c
   tif_getimage.c
   tif_jbig.c
   tif_jpeg.c
   tif_luv.c
   tif_lzma.c
   tif_lzw.c
   tif_next.c
   tif_ojpeg.c
   tif_open.c
   tif_packbits.c
   tif_pixarlog.c
   tif_predict.c
   tif_print.c
   tif_read.c
   tif_strip.c
   tif_swab.c
   tif_thunder.c
   tif_tile.c
   tif_unix.c
   tif_version.c
   tif_warning.c
   tif_write.c
   tif_zip.c
)

set (
   HEADER_FILES 
   
   t4.h
   tif_config.h
   tif_config.vc.h
   tif_config.wince.h
   tif_dir.h
   tif_fax3.h
   tif_predict.h
   tiff.h
   tiffconf.h
   tiffconf.vc.h
   tiffconf.wince.h
   tiffio.h
   tiffio.hxx
   tiffiop.h
   tiffvers.h
   uvcode.h
)

# define the Visual Studio IDE source file groupings
   
if (MSVC)
   source_group ("Source Files" FILES ${SOURCE_FILES})
   source_group ("Header Files" FILES ${HEADER_FILES})
endif (MSVC)

# define the source files used for building the library target
# these will all appear in the IDE

add_library (
   libtiff 
   
   STATIC
   
   ${SOURCE_FILES}
   ${HEADER_FILES}
)

# Organise the library in the IDE

set_target_properties (
   libtiff 

   PROPERTIES

   FOLDER "Libraries") 

# define the header file search paths

target_include_directories (
   libtiff 
   PRIVATE

   .
   ../libjpg
   ../ZLib
)

# define preprocessor variables

target_compile_definitions (
   libtiff 
   
   PRIVATE 
   
   WIN32
   NDEBUG
   USE_LIBTIFF_STATIC
   ZLIB_WINAPI
   _CRT_SECURE_NO_WARNINGS
   
##   _UNICODE
##   UNICODE
)

# suppress warnings on selected 3rd party sources

if (MSVC)
   update_source_files_compile_flags (
      tif_unix.c

      "/wd4028 /wd4996"
   )
endif (MSVC)
