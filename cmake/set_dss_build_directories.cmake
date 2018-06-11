
# This function sets the paths to various DSS build directories
# that depend on build platforma and build configuration

function (SET_DSS_BUILD_DIRECTORIES LibPath)
   
   if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017 Win64")           
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win64/$(Configuration)Libs" PARENT_SCOPE)
      
   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win32/$(Configuration)Libs"  PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015 Win64")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win64/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win32/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 2013 Win64")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win64/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 2013")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win32/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 2012 Win64")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win64/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 2012")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win32/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 2010 Win64")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win64/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 2010")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win32/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008 Win64")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win64/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win32/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005 Win64")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win64/$(Configuration)Libs" PARENT_SCOPE)

   elseif (${CMAKE_GENERATOR} STREQUAL "Visual Studio 8 2005")
      set (${LibPath} "${CMAKE_CURRENT_SOURCE_DIR}/libs/Win32/$(Configuration)Libs" PARENT_SCOPE)

   endif ()

endfunction (SET_DSS_BUILD_DIRECTORIES)
