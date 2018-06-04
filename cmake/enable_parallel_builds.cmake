# Let the user choose if multiprocessor builds should be supported

function (ADD_OPTION_FOR_PARALLEL_BUILD VariableName)
   option (${VariableName} "Enable faster multi-processor parallel builds (if supported)" ON)

   if (${VariableName})
      if (MSVC)
         set (CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   /MP")
         set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
         message (STATUS "Parallel multi-processor build generated")
      endif ()
   else ()
      message (STATUS "Non-parallel build generated")
   endif ()
endfunction (ADD_OPTION_FOR_PARALLEL_BUILD)
