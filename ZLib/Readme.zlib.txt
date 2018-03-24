The version of zlib currently used is 1.2.11.  The file zlib1211.zio was downloaded from
the zlib website and unpacked.

The VS solution file D:\zlib-1.2.11\contrib\vstudio\vc14\zlibvc.sln was opened in VS2017
and converted to the current VS release.   The only project that I built was the zlibstat
project (for static libs).  I modified the project configurations to remove the ZLIB_WINAPI
pre-processor directive as some of the code we use (e.g. CFitsIO) uses the standard C 
calling convention for zlib.

I needed manually to build the assembler code in the following directories:

contrib\masmx86
contrib\masmx64

and had to modify the bld_ml32.bat file in the masmx86 directory to add the /safeseh flag:

ml /safeseh /coff /Zi /c /Flmatch686.lst match686.asm
ml /safeseh /coff /Zi /c /Flinffas32.lst inffas32.asm

Note that I needed to locate ml64.exe in VS2017 directories and copy it to the masmx64 
directory as the directory it was in was not in the path for the VS2017 command prompt.

I also changed the configuration of the Debug Projects to generate a .pdb file.

The various versions of zlibstat.lib were then copied to this directory.