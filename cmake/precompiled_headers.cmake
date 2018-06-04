
# Function to define the precompiled header file and identify the generator sourve file

function (SET_TARGET_PRECOMPILED_HEADER Target PrecompiledHeader PrecompiledSource)
   if (MSVC)
      set_target_properties (
         ${Target} 
         PROPERTIES 
        
         COMPILE_FLAGS "/Yu${PrecompiledHeader}"
      )
      set_source_files_properties (
         ${PrecompiledSource} 
         PROPERTIES 
        
         COMPILE_FLAGS "/Yc${PrecompiledHeader}"
      )
   endif (MSVC)
endfunction(SET_TARGET_PRECOMPILED_HEADER)

# Function to exempt a list of source files from precompiled header processing

function (IGNORE_PRECOMPILED_HEADER SourceFiles)
   if (MSVC)  
      foreach (filename ${SourceFiles})
         update_source_files_compile_flags (${filename} "/Y-")
      endforeach (filename)
   endif (MSVC)
endfunction(IGNORE_PRECOMPILED_HEADER)
