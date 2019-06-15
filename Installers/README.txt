Welcome to DeepSkyStacker 4.2.0 Beta 3

The main change in this release is to use LibRaw version 19.2 instead of dcraw to capture images from RAW image files.
This should provide support for many recent cameras which wasn't possible using dcraw.
The resulting images should be very much the same (if not 100% identical) to the results obtained using DeepSkyStacker 4.1.1.

There is one exception:

The "Use Auto White Balance" white balance button is disabled for this release.
You can either accept the default which is "Daylight White Balance", or select "Use Camera White Balance".
If you select the latter, and camera was set to "Auto White Balance" when the images were taken, then the camera calculated "Auto White Balance" co-efficients will be used.
We may provide an update to support doing our own "Auto White Balance" in the future.

We've also fixed a number of problems and made some other enhancements since 4.1.1

1. Support for images from cameras that use the Fujitsu X-Trans sensor (new in 4.2.0 Beta 2).
   The image is converted to a 3 colour RGB image and interpolated using Frank Markesteijn's algorithm using LibRaw::Process().
   Please note that the interpolation is quite slow (about 40 seconds per image on my Core i& laptop).
   The image is then imported as if it were a Foveon colour image.

2. BSD 3 clause license file added.

3. A fix was made to correct crashes caused by a thread-unsafe assignment in the smart pointer code.

4. The "Stop" button on the progress dialogs should now actually work!

5. The space key can now be used to check/uncheck images in the picture list.

6. The user's default browser will be used instead of IE to open URLs.

Welcome to DeepSkyStacker 4.1.1

    The 32 bit version has been changed to build the zlib code without the optimising "asm" option to fix a crashing problem ("asm" is not officially supported in zlib and has not been maintained)

    The 64 bit version has been changed to build the zlib code without the optimising "asm" option as a precaution (the 64 bit version wasnt crashing as far as we know but as the "asm" option isn't officially supported we have elected not to use it)

*****

Welcome to DeepSkyStacker 4.1.0

This version of DeepskyStacker is supported on Windows 7, Windows 8, Windows 8.1 and Windows 10 and completely supercedes DeepSkyStacker 3.3.2, 3.3.4 and 3.3.6.

We now provide DeepSkyStacker and the other programs as both 32-bit (x86) and 64-bit (x64) builds based upon the latest version of dcraw. There are separate installers for the 32-bit and 64-bit versions.

Installing the 32-bit build will remove earlier versions of DeepSkyStacker (3.3.2 and upward). If you are running an earlier version than 3.3.2, please remove it manually.

You can install both the 32-bit and the 64-bit versions on 64-bit versions of Windows without problems.

PLEASE NOTE: If anyone who installs this gets: "This program can't start because api-ms-win-crt-runtime-l1-1-0.dll is missing" when they try to open the programs, the solution is to install:

https://support.microsoft.com/en-us/help/2999226/update-for-universal-c-runtime-in-windows

for your specific version of Windows.

This problem should only affect users of Vista / Win7 / Win 8.x who are behind on WindowsUpdates, or are running an offline system and opting out of WindowsUpdates.
Enjoy,

Luc Coiffier, Tony Cook, David C. Partridge

We've fixed a number of problems since 4.1.0 beta 1:

    Fixed a problem when using drizzle and Intersection mode which resulted in an output image that was 4x or 9x too large with the image in the upper left corner.

    Worked around a problem with incorrect display of the left panel on high resolution monitors. (Set the DPI Aware flag in the application manifest to No).

    Changed our build of libtiff so that we use Unix style rather than Win32 style I/O as the Win32 code is wrong in x64 mode (known problem with libtiff).

    A number of other size_t/DWORD/long issues in the code were fixed for the 64 bit build (to ensure computed values were indeed 64 bit sized).

    Small change to improve the responsiveness of the post-processing screen (faster display of the final image).

