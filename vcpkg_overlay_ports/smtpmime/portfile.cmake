vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO deepskystacker/SmtpClient-for-Qt
	REF 79eb946c5be718149847ea0bd015a3cebcbf9f98
    SHA512 00f3e14dd649d46a878b06f9c4a525907fa8f3072d58fc7bcb9e8a3a73c0289049e54c4170f8d024d1c001928e4af7d66a391e297be0b6a9f3cf07bc6ff2f16d
    HEAD_REF V2.0
	
)
set(ENV{QT_DIR} $ENV{QTDIR})
set(ENV{Qt6_DIR} $ENV{QTDIR})

set (options
	"-DBUILD_DEMOS=OFF "
	"-DBUILD_TESTS=OFF "
)

message ("****************************************************************")
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