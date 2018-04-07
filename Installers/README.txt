Welcome to DeepSkyStacker 4.1.0

We've fixed a number of problems since 4.1.0 beta 1:



1) Fixed a problem when using drizzle and Intersection mode which resulted in an output image that was 4x or 9x too large with the image in the upper left corner.

2) Worked around a problem with incorrect display of the left panel on high resolution monitors.  (Set the DPI Aware flag in the application manifest to No).

3) Changed our build of libtiff so that we use Unix style rather than Win32 style I/O as the Win32 code is wrong in x64 mode (known problem with libtiff).

4) A number of other size_t/DWORD/long issues in the code were fixed for the 64 bit build (to ensure computed values where indeed 64 bit sized).

5) Small change to improve the responsiveness of the post-processing screen (faster display of the final image).



PLEASE NOTE: If anyone who installs this gets: "This programme can't start because api-ms-win-crt-runtime-l1-1-0.dll is missing" when they try to

open the programs, the solution is to install:


https://support.microsoft.com/en-us/help/2999226/update-for-universal-c-runtime-in-windows


for your specific version of Windows.

This problem should only affect users of Vista / Win7 / Win 8.x who are behind on WindowsUpdates, or are running an offline system and opting out of WindowsUpdates. 