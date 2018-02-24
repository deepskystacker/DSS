To build the CFITSIO dll library using Visual C++:

	Execute the vcvars32.bat file that is distributed with Visual C++

	Unpack the CFITSIO source files (cfit2xxx.zip) into temporary directory

	In a dos command window, cd to that directory and enter the 
        following commands:

		nmake winDumpExts.mak
		nmake makefile.vcc
			(ignore the compiler warning messages)

	This creates the cfitsio.def, cfitsio.dll, and cfitsio.lib files.  The
	fitsio.h and longnam.h files are also needed to compile any programs
	that use CFITSIO.

	To test that the library works correctly, enter:
		cl /MD testprog.c cfitsio.lib
		testprog.exe

	The testprog program prints out a long list of diagnostic messages
	that should end with "Status = 0; OK - no error"


To build a program using the CFITSIO dll:

	Include the header files fitsio.h and longnam.h in your source code

	Link your program using the .lib file


To build the test program using Microsoft Visual C++:

	cl /MD testprog.c cfitsio.lib

To build the test program using Borland C:

	bcc32 -f testprog.c cfitsio.lib

NOTE: The /MD command line switch must be specified on the cl command line
to force the compiler/linker to use the appropriete runtime library.  
If this switch is omitted, then the fits_report_error function in CFITSIO
will likely crash.  

When building programs in the Visual Studio environment, one can force
the equivalent of the /MD switch by selecting 'Settings...' under the
'Project' menu, then click on the C/C++ tab and select the 'Code
Generator' category.  Then under 'User Run-time Library' select
'Multithreaded DLL'.
