vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO bluetiger9/SmtpClient-for-Qt
	REF 863843c34e03b50d41b8ae8ab6af00e9351693ca
    SHA512 cf50007fd6b6068468cee0cf32c8db62fd74d0c8e99890eed8e163900d5ee317ea69ccc5982e03e33565576088b74c2e763b574d041d927a6f97fdab603b5738
    HEAD_REF V2.0
	
)

set(QTDIR $ENV{QTDIR})
set(CMAKE_PREFIX_PATH ${QTDIR}) 
set(ENV{QT_DIR} ${QTDIR})
set(ENV{Qt6_DIR} ${QTDIR})
set(CMAKE_AUTOMOC ON)

set (options
	"-DBUILD_SHARED_LIBS=OFF "
	"-DBUILD_DEMOS=OFF "
	"-DBUILD_TESTS=OFF "
)

message ("****************************************************************")
message ("EV QTDIR: " ${QTDIR})
message ("Setting build options: " ${options})
message ("****************************************************************")

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
	OPTIONS ${options}
)

vcpkg_cmake_install()

vcpkg_copy_pdbs()

vcpkg_fixup_pkgconfig()

vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/${PORT}") 

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")