include(CMakeFindDependencyMacro)
find_dependency(lcms2 CONFIG)
include(${CMAKE_CURRENT_LIST_DIR}/lcms-targets.cmake)
