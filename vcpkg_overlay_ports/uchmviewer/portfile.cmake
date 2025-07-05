vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO deepskystacker/uChmViewer
	REF d073ff24655bb28c026fe001a30a8408910f49f8
    SHA512 89ae5cd77ab8415f62c94f1baf42a859d6d77b1059c6453098fa801799a794f02e096ac78dd1ad69ac865be662ac95e6c6018486c95c42e4ea3f62c2aff882d1
    HEAD_REF master
)

set(QTDIR $ENV{QTDIR})
set(CMAKE_PREFIX_PATH ${QTDIR}) 
set(ENV{QT_DIR} ${QTDIR})
set(ENV{Qt6_DIR} ${QTDIR})

set(CMAKE_AUTOMOC ON)

set (options
	"-DUSE_WEBENGINE=ON "
	"-DUSE_MACOS_BUNDLE=OFF "
	"-DUSE_GETTEXT=OFF "
	"-DIS_VCPKG_BUILD=ON "
#	"-DUSE_STATIC_CHMLIB=ON "
	"-DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=13.4 "
)

message ( "****************************************************************" )
message ( "EV QTDIR: " ${QTDIR})
message ( "Setting build options: " ${options} )
message ( "****************************************************************" )

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS ${options}
)

vcpkg_cmake_install()

vcpkg_fixup_pkgconfig()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_copy_tools(
        TOOL_NAMES uchmviewer
        AUTO_CLEAN
)
		
vcpkg_install_copyright( FILE_LIST "${SOURCE_PATH}/COPYING" )