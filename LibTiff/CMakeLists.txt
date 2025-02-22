set(PROJECT_NAME libtiff)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "t4.h"
    "tif_config.h"
    "tif_dir.h"
    "tif_fax3.h"
    "tif_hash_set.h"
    "tif_predict.h"
    "tiff.h"
    "tiffconf.h"
    "tiffconf.vc.h"
    "tiffconf.wince.h"
    "tiffio.h"
    "tiffio.hxx"
    "tiffiop.h"
    "tiffvers.h"
    "uvcode.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "tif_aux.c"
    "tif_close.c"
    "tif_codec.c"
    "tif_color.c"
    "tif_compress.c"
    "tif_dir.c"
    "tif_dirinfo.c"
    "tif_dirread.c"
    "tif_dirwrite.c"
    "tif_dumpmode.c"
    "tif_error.c"
    "tif_extension.c"
    "tif_fax3.c"
    "tif_fax3sm.c"
    "tif_flush.c"
    "tif_getimage.c"
    "tif_hash_set.c"
    "tif_jbig.c"
    "tif_jpeg.c"
    "tif_jpeg_12.c"
    "tif_lerc.c"
    "tif_luv.c"
    "tif_lzma.c"
    "tif_lzw.c"
    "tif_next.c"
    "tif_ojpeg.c"
    "tif_open.c"
    "tif_packbits.c"
    "tif_pixarlog.c"
    "tif_predict.c"
    "tif_print.c"
    "tif_read.c"
    "tif_stream.cxx"
    "tif_strip.c"
    "tif_swab.c"
    "tif_thunder.c"
    "tif_tile.c"
    "tif_unix.c"
    "tif_version.c"
    "tif_warning.c"
    "tif_webp.c"
    "tif_write.c"
    "tif_zip.c"
    "tif_zstd.c"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(libtiff STATIC ${ALL_FILES})
set_target_properties(libtiff PROPERTIES FOLDER "Libraries")

set(ROOT_NAMESPACE libtiff)

################################################################################
# Output directory
################################################################################
set_target_properties(libtiff PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/./$ENV{PlatformTarget}/$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/./$ENV{PlatformTarget}/$<CONFIG>/"
)
set_target_properties(libtiff PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
    .
	../include
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "ZIP_SUPPORT;"
    "_LIB;"
    "_CRT_NONSTDC_NO_WARNINGS;"
    "_CRT_SECURE_NO_WARNINGS;"
    "USE_LIBTIFF_STATIC;"
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(${PROJECT_NAME} PRIVATE
	WIN32
	_WINDOWS
	_UNICODE
	UNICODE
	)
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Gm-;
            /RTC1
        >
        $<$<CONFIG:Release>:
            /GL-;
            /GF;
            /Gy
        >
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
endif()

