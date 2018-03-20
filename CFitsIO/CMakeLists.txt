# CFITSIO CMakeLists.txt

# Suppress warning about WIN32 no longer being defined on Cygwin:
SET(CMAKE_LEGACY_CYGWIN_WIN32 0)

PROJECT(CFITSIO)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.0)

# Allow @rpath token in target install name on Macs.
# See "cmake --help-policy CMP0042" for more information.
IF(POLICY CMP0042)
  CMAKE_POLICY(SET CMP0042 NEW)
ENDIF()

INCLUDE (${CMAKE_ROOT}/Modules/CheckLibraryExists.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)

# Allow the developer to select if Dynamic or Static libraries are built
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" ON)
OPTION (USE_PTHREADS "Thread-safe build (using pthreads)" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
set (LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}")
set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/")

# Define project version
SET(${PROJECT_NAME}_MAJOR_VERSION 3)
SET(${PROJECT_NAME}_MINOR_VERSION 43)
SET(${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION})
SET(LIB_NAME cfitsio)

# Microsoft Visual Studio:
IF(MSVC OR BORLAND)
    # Define
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
    # Need an empty unistd.h in MSVC for flex-generated eval_l.c:
    FILE(WRITE ${CMAKE_SOURCE_DIR}/unistd.h "")
    INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
ENDIF()

IF(BORLAND)
    # Suppress spurious Borland compiler warnings about "Suspicious
    # pointer arithmetic", "Possibly incorrect assignment", and
    # "Comparing signed and unsigned values".
    ADD_DEFINITIONS(-w-spa)
    ADD_DEFINITIONS(-w-pia)
    ADD_DEFINITIONS(-w-csu)
ENDIF()

#add_subdirectory (src)

SET (LIB_TYPE STATIC)
IF (BUILD_SHARED_LIBS)
  SET (LIB_TYPE SHARED)
ENDIF (BUILD_SHARED_LIBS)


FILE(GLOB H_FILES "*.h")

IF (USE_PTHREADS)
    FIND_PACKAGE(pthreads REQUIRED)
    INCLUDE_DIRECTORIES(${PTHREADS_INCLUDE_DIR})
    ADD_DEFINITIONS(-D_REENTRANT)
ENDIF()

# Math library (not available in MSVC or MINGW)
IF (MSVC OR MINGW)
    SET(M_LIB "")
ELSE()
    FIND_LIBRARY(M_LIB m)
ENDIF()

# Support for remote file drivers is not implemented for native Windows:
IF (NOT MSVC)
    # Find library needed for gethostbyname:
    CHECK_FUNCTION_EXISTS("gethostbyname" CMAKE_HAVE_GETHOSTBYNAME)
    IF(NOT CMAKE_HAVE_GETHOSTBYNAME)
       CHECK_LIBRARY_EXISTS("nsl" "gethostbyname" "" CMAKE_HAVE_GETHOSTBYNAME)
    ENDIF()

    # Find library needed for connect:
    CHECK_FUNCTION_EXISTS("connect" CMAKE_HAVE_CONNECT)
    IF(NOT CMAKE_HAVE_CONNECT)
       CHECK_LIBRARY_EXISTS("socket" "connect" "" CMAKE_HAVE_CONNECT)
    ENDIF()

    # Define HAVE_NET_SERVICES if gethostbyname & connect were found:
    IF (CMAKE_HAVE_GETHOSTBYNAME AND CMAKE_HAVE_CONNECT)
        ADD_DEFINITIONS(-DHAVE_NET_SERVICES)
    ENDIF ()

    # Find curl library, for HTTPS support:
    FIND_PACKAGE(CURL)
    IF (CURL_FOUND)
        INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
        ADD_DEFINITIONS(-DCFITSIO_HAVE_CURL)
    ENDIF()
ENDIF()

SET(SRC_FILES
    buffers.c cfileio.c checksum.c drvrfile.c drvrmem.c
    drvrnet.c drvrsmem.c drvrgsiftp.c editcol.c edithdu.c eval_l.c
    eval_y.c eval_f.c fitscore.c getcol.c getcolb.c getcold.c getcole.c
    getcoli.c getcolj.c getcolk.c getcoll.c getcols.c getcolsb.c
    getcoluk.c getcolui.c getcoluj.c getkey.c group.c grparser.c
    histo.c iraffits.c
    modkey.c putcol.c putcolb.c putcold.c putcole.c putcoli.c
    putcolj.c putcolk.c putcoluk.c putcoll.c putcols.c putcolsb.c
    putcolu.c putcolui.c putcoluj.c putkey.c region.c scalnull.c
    swapproc.c wcssub.c wcsutil.c imcompress.c quantize.c ricecomp.c
    pliocomp.c fits_hcompress.c fits_hdecompress.c simplerng.c
    zlib/zuncompress.c zlib/zcompress.c
    f77_wrap1.c f77_wrap2.c f77_wrap3.c f77_wrap4.c
)

#
# We don't want to include the zlib stuff if we are building a static library,
# but if we are building a DLL, then we do, so add the zlib files here
#
IF (BUILD_SHARED_LIBS)
    set(SRC_FILES ${SRC_FILES}
        zlib/adler32.c zlib/crc32.c
        zlib/inffast.c zlib/inftrees.c zlib/trees.c zlib/zutil.c
        zlib/deflate.c zlib/infback.c zlib/inflate.c zlib/uncompr.c
        )
ENDIF()
    

ADD_LIBRARY(${LIB_NAME} ${LIB_TYPE} ${H_FILES} ${SRC_FILES})
TARGET_LINK_LIBRARIES(${LIB_NAME} ${PTHREADS_LIBRARY} ${M_LIB} ${CURL_LIBRARIES})

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES VERSION ${${PROJECT_NAME}_VERSION} SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION})
install(TARGETS ${LIB_NAME} DESTINATION ${LIB_DESTINATION})
install(FILES ${H_FILES} DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

#
# Only build test code and executables if building the DLL
#
IF (BUILD_SHARED_LIBS)
	ENABLE_TESTING()

	ADD_EXECUTABLE(TestProg testprog.c)
	TARGET_LINK_LIBRARIES(TestProg ${LIB_NAME})
	ADD_TEST(TestProg TestProg)
	# Copy testprog.tpt to build directory to allow quick test
	# of ./TestProg (or .\Release\TestProg.exe in MSVC):
	FILE(COPY ${CMAKE_SOURCE_DIR}/testprog.tpt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

	ADD_EXECUTABLE(cookbook cookbook.c)
	TARGET_LINK_LIBRARIES(cookbook ${LIB_NAME})
	ADD_TEST(cookbook cookbook)

	ADD_EXECUTABLE(FPack fpack.c fpackutil.c)
	TARGET_LINK_LIBRARIES(FPack ${LIB_NAME})

	ADD_EXECUTABLE(Funpack funpack.c fpackutil.c)
	TARGET_LINK_LIBRARIES(Funpack ${LIB_NAME})

	ADD_EXECUTABLE(Fitscopy fitscopy.c)
	TARGET_LINK_LIBRARIES(Fitscopy ${LIB_NAME})

	# To expands the command line arguments in Windows, see:
	# http://msdn.microsoft.com/en-us/library/8bch7bkk.aspx
	if(MSVC)
	  set_target_properties(FPack Funpack PROPERTIES 
	    LINK_FLAGS "setargv.obj"
	  )
	endif(MSVC)
ENDIF(BUILD_SHARED_LIBS)