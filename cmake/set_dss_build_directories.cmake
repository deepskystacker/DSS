
# This function sets the paths to various DSS build directories
# that depend on build platform and build configuration

function (SET_DSS_BUILD_DIRECTORIES LibPath)

   if ("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win64/$(Configuration)Libs" PARENT_SCOPE)
   else ()
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win32/$(Configuration)Libs" PARENT_SCOPE)   
   endif ()
   
endfunction (SET_DSS_BUILD_DIRECTORIES)
