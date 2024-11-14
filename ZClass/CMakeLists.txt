set(PROJECT_NAME ZClass)
message("Configuring project: " ${PROJECT_NAME})
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(COMPILE_WARNING_AS_ERROR YES)

qt_standard_project_setup()

################################################################################
# Source groups
################################################################################
set(Header_Files
    "zdefs.h"
    "zexcbase.h"
    "zexcept.h"
    "zinttimr.h"
    "zmstrlck.h"
    "zptr.h"
    "zreslock.h"
    "zsingle.h"
    "ztrace.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "source/ZExcBase.cpp"
    "source/ZExcept.cpp"
    "source/zmstrlck.cpp"
    "source/zreslock.cpp"
    "source/ZTrace.cpp"
)
if(NOT WIN32)
set (Source_Files
    ${Source_Files}
    "source/zinttimr.cpp"
)
endif()

source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
qt_add_library(ZClass STATIC ${ALL_FILES})

set_target_properties(ZClass PROPERTIES FOLDER "Libraries")
target_link_libraries(ZClass PRIVATE Qt6::Core)

################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/./$ENV{PlatformTarget}/$<CONFIG>/"
    OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/./$ENV{PlatformTarget}/$<CONFIG>/"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
    .
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_include_directories(${PROJECT_NAME} PRIVATE
	"$<$<CONFIG:Debug>:C:/Program Files (x86)/Visual Leak Detector/include>"
)
endif()

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Release>:"
        "NDEBUG"
    ">"
    "_LIB"
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
target_compile_definitions(${PROJECT_NAME} PRIVATE
	WIN32
	_WINDOWS
	_SBCS
	)
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /RTC1
        >
        $<$<CONFIG:Release>:
            /GL-;
            /GF
        >
        /W3 /wd4996
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        /Zc:__cplusplus;
        /permissive-;
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
    )
endif()

