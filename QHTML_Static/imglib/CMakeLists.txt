# define source files in intended IDE groupings
   
set (
   SOURCE_FILES 
      
   stdafx.cpp
   
   CompatibleDC.h
   Config.h
   DecodeBMP.cpp
   DecodeGIF.cpp
   DecodeICO.cpp
   DecodeJPEG.cpp
   DecodeMNG.cpp
   DecodePCX.cpp
   DecodePNG.cpp
   DecodeRIF.cpp
   Dib.cpp
   dib.h
   DibLoader.cpp
   DibLoader.h
   Frame.cpp
   FrameData.h
   gif.h
   Image.cpp
   ImgLib.h
   ImgLibInt.h
   RIFFormat.h
)

set (
   HEADER_FILES 
   
   stdafx.h

   targetver.h
)

# define the Visual Studio IDE source file groupings
   
source_group ("Source Files" FILES ${SOURCE_FILES})
source_group ("Header Files" FILES ${HEADER_FILES})

# define the source files used for building the library target
# these will all appear in the IDE

add_library (
   imglib 
   
   STATIC
   
   ${SOURCE_FILES}
   ${HEADER_FILES}
)

# Organise the library in the IDE

set_target_properties (
   imglib 

   PROPERTIES

   FOLDER "Libraries/QHTML Libs") 

# define the header file search paths

target_include_directories (
   imglib 

   PRIVATE

   .
   ..
   ../../lpng1634
   ../../libjpg
   ../../ZLib
)

# define preprocessor variables

target_compile_definitions (
   imglib 
   
   PRIVATE 
   
   WIN32
   NDEBUG
   _CRT_SECURE_NO_WARNINGS
   
##   _UNICODE
##   UNICODE
)

# Name the precompiled header and generator source files

set_target_precompiled_header (
   imglib 
   
   stdafx.h 
   stdafx.cpp
)
