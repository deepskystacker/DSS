This directory contains static libraries for CFITSIO 3.430 which were built externally
to the DSS project using CMake.

The libraries are in directories:

Win32\DebugLibs
Win32\ReleaseLibs
Win64\DebugLibs
Win64\ReleaseLibs

Those directory names were chosen so that their content won't be ignored by git based
on the contents of the .gitignore file which specifically excludes amongst many other
things:

[Dd]ebug/
[Dd]ebugPublic/
[Rr]elease/
[Rr]eleases/
x64/
x86/

The DeepSkyStacker project was modified to add ..\CFitsIO\$(Platform)\$(Configuration)Libs
at the beginning of the "Library Directories" so that the files are  picked up by our build.

As it was when download from the NASA website, the CMakeLists.txt included the zlib code that
they ship with CFitsIO.  When built into a static library, that conflicted with our zlib build.

I have changed the CMakeLists.txt to only inclube the CFitsIO code in a static library build.
Note that two files in the zlib subdirectory (zlib/zuncompress.c zlib/zcompress.c) are not part
of zlib, but part of CFitsIO.  I have included the modified CMakeLists.txt in this directory.

To build CFitsIO as a static libraries, unpack the cfit3430.zip into a directory - in my case
I just unpacked it into the root of my D: drive which created a cfitsio directory in the root.

Next I issued the following commands at a V2017 command prompt after navigating to the 
directory or drive containing the cfitsio directory after modifying the CMakeLists.txt
as mentioned above:

mkdir CFitsIO.x86
mkdir CFitsIO.x64
cd CFitsIO.x86
cmake -G "Visual Studio 14 2015" ../CFitsIO -DBUILD_SHARED_LIBS=OFF
cmake --build .
cmake --build . --config Release 
cd ../CFitsIO.x64
cmake -G "Visual Studio 14 2015 Win64" ../CFitsIO -DBUILD_SHARED_LIBS=OFF
cmake --build .
cmake --build . --config Release 

I then copied the .lib and .pdb files to the correct locations in our directory tree.

David Partridge 20 March 2018

