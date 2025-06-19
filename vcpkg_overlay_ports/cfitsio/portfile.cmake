vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO HEASARC/cfitsio
    REF cfitsio-${VERSION}
    SHA512 b33c27070b311a831c2a40780549966ef3cc36b7052efd2fe8e44ce07061d9ed8297a822153cc6365469824aaec61c307f6d37adeae79e8b51ca620876b4733a
    HEAD_REF main
)

set (options
	"-DUSE_BZIP2=ON "
	"-DUSE_CURL=OFF "
	"-DTESTS=OFF "
	"-DUTILS=OFF"
)
if(VCPKG_TARGET_IS_WINDOWS)
	set (options "${options}; -DUSE_PTHREADS=OFF")
else()
	set (options "${options}; -DUSE_PTHREADS=ON")
endif()

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
#
# cfitsio puts its cmake files into /lib/cmake/cfitsio, but vcpkg doesn't want
# them there, causing error messages: 
# 1> [CMake] D:\Github\DSS\./vcpkg_overlay_ports\cfitsio\portfile.cmake: warning: This port installs the following CMake files in places CMake files are not expected. CMake files should be installed in ${CURRENT_PACKAGES_DIR}/share/${PORT}. To suppress this message, add set(VCPKG_POLICY_SKIP_MISPLACED_CMAKE_FILES_CHECK enabled)
# 1> [CMake] D:\Github\DSS\build\vcpkg_installed\vcpkg\pkgs\cfitsio_x64-windows-static-md: note: the files are relative to ${CURRENT_PACKAGES_DIR} here
# 1> [CMake] note: lib/cmake/cfitsio/cfitsioConfig.cmake
# 1> [CMake] note: lib/cmake/cfitsio/cfitsioConfigVersion.cmake
# 1> [CMake] note: lib/cmake/cfitsio/cfitsioTargets-release.cmake
# 1> [CMake] note: lib/cmake/cfitsio/cfitsioTargets.cmake
# 1> [CMake] note: debug/lib/cmake/cfitsio/cfitsioConfig.cmake
# 1> [CMake] note: debug/lib/cmake/cfitsio/cfitsioConfigVersion.cmake
# 1> [CMake] note: debug/lib/cmake/cfitsio/cfitsioTargets-debug.cmsake
# 1> [CMake] note: debug/lib/cmake/cfitsio/cfitsioTargets.cmake
#
# So tell vcpkg to fix this for us.
#
vcpkg_cmake_config_fixup(CONFIG_PATH "lib/cmake/${PORT}") 

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/licenses/License.txt")