
# This function appends the given compile flags to a list of source files

function (UPDATE_SOURCE_FILES_COMPILE_FLAGS SourceFiles CompileFlags)
   foreach (filename ${SourceFiles})
      get_source_file_property (OLD_FLAGS ${filename} COMPILE_FLAGS)

      if (${OLD_FLAGS} STREQUAL "NOTFOUND")
         set_source_files_properties (${filename} PROPERTIES COMPILE_FLAGS "${CompileFlags}")
      else ()
         set_source_files_properties (${filename} PROPERTIES COMPILE_FLAGS "${OLD_FLAGS} ${CompileFlags}")
      endif ()
   endforeach (filename)   
endfunction (UPDATE_SOURCE_FILES_COMPILE_FLAGS)

# This function appends the given link flags to all subsequent targets

function (UPDATE_LINK_FLAGS LinkFlags)
   set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LinkFlags}" PARENT_SCOPE)
endfunction (UPDATE_LINK_FLAGS)
