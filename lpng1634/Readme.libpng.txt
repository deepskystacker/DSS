The external build of libpng as static libraries from the lpng1634.zip file downloaded from
http://www.libpng.org/pub/png/libpng.html proved to be somewhat of a pain.

The VS solution and project files in the lpng1634\projects\vstudio directory were opened in
VS2017 and converted.

The default project was changed to be libpng without problem but that was configured to 
build a DLL by default.  Luckily a configuration to build static libs did exist but was setup
to use static C runtime rather than /MD or /MDd  as appropriate (to use the DLL run time).  Quite
a number of other changes were necessary to allow for 64 bit builds as well as 32 bit.

For this reason I have taken the unusual step of including the zip file of the whole lpng1634
directory stucture at the point of completing the builds (and to allow future changes if
necessary).

David Partridge 21 March 2018