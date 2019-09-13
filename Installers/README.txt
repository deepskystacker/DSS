Welcome to DeepSkyStacker 4.2.3

The main changes in this release are:

1. The stacking settings dialog width has been increased to reduce or eliminate the need to scroll the tabs.

2. Add .rw2 to list of RAW file extensions.

3. Change the colour scheme for DeepSkyStacker Live to make it more "night vision" friendly.

4. Recognise and use GAIN values in FITS file from CMOS/CCD cameras in a manner similar to how ISO is handled for matching darks, lights etc..   Please note this is not the same as EGAIN (e-/ADU).  It refers to the camera's scaling of EGAIN.

5. Update help files and add a new Dutch version of the help.

6. Provide the option to perform NO White Balance processing at all (requested by Ivo Jager for StarTools).

7. Apply White Balance scaling factors from the RAW/DDP settings dialogue before we scale the image data rather than after.  This should prevent the scaling from causing burn-out images which it could do previously.

8. Fix for issue #42 - the stacking method "Average" wasn't always reported in the recap html file.



Welcome to DeepSkyStacker 4.2.2

This release is all about performance, though other areas have been enhanced as well.  The main changes since 4.2.1 are:

1. Changes to star registration code to detect larger stars than before.   Star radius of up to 50 pixels is now valid (it was 20 pixels).  This was needed to accomodate higher resolution sensors.

2. Allow larger star sizes to the specified in the Star Mask creation dialogue.   Star diameter of up to 100 pixels is now allowed.   This was needed to accomodate higher resolution sensors.

3. OpenMP speedup of Star Mask image reading code.

4. Huge speedup of final image display by using much larger rectangles and OpenMP speedup of the bitmap extraction code.

5. Speed up star registration by updating progress indicator once every 25 iterations instead of each iteration.

6. Computing Final Picture using Kappa-Sigma Clip processing is now about 50 times faster!

7. Computing Final Picture using Auto Adaptive Weighted Average is now about 43 times faster!

8. Auto-Adaptive Weighted Average now re-written completely.  It should no longer produce whited out images or other strange results. Thanks to Michael A. Covington and Simon C. Smith for their immense help in locating references and getting this working.  Thanks are also due to Dr. Peter B. Stetson who very kindly provided a web link to his original lecture notes, emailed a PDF copy of his original paper, and provided helpful guidance.

9. Update LibRaw to 0.19.3.

10. Reverted Entropy calculation code in CEntropyInfo::InitSquareEntropies() to NOT compute in parallel - some called code wasn't thread safe.

11. Entropy Weighted Average complete stack of 50 lights is now about 20 times faster on my laptop, down from 4hrs 22min to 12min 55 sec.

12. Bug fix - changing "Set the black point to 0" or "Use Camera White Balance" after an image was loaded didn't change the rendering.

13. When loading raw files, check that the camera is listed in the LibRaw supported camera list.  Issue a warning if not.

14. User request to change the file open dialog to default to *all* supported image types including raw files.

15. Three times performance improvement in star registration.

16. As requested by Ray Butler, use relative paths in saved file lists while continuing to accept older file lists using absolute paths.

17. Miscellaneous other performance tweaks.

18. Display lens aperture information in the image lists for DeepSkyStacker and DeepSkyStackerLive.

19. Don't check for LibRaw camera support if processing DNG file.
 
Welcome to DeepSkyStacker 4.2.1

The main changes from 4.2.0 are:

1. Fixed a memory leak in the new code that uses LibRaw.

2. Further performance enhancements to Median-Kappa-Sigma processing (20% faster than 4.2.0).

3. Entropy calculation for Entropy Weighted Average now computed in parallel.

Welcome to DeepSkyStacker 4.2.0

The main change in this release is to use LibRaw version 19.2 instead of dcraw to capture images from RAW image files.
This should provide support for many recent cameras which wasn't possible using dcraw.

The raw data post-processing code has had some extra performance squeezed out if it by Vitali Pelenjow who took
my initial attempt at using OpenMP to parallelise the code and made it perform as it should (new in 4.2.0 Beta 4).

The resulting images should be very much the same (if not 100% identical) to the results obtained using DeepSkyStacker 4.1.1.

There is one thing that is incompatible with DeepSkyStacker 4.1.1:

The "Use Auto White Balance" white balance button has been disabled.

You can either accept the default which is "Daylight White Balance", or select "Use Camera White Balance".   If you select the latter, and camera was set to "Auto White Balance" when the images were taken, then the camera calculated "Auto White Balance" co-efficients will be used.   We don't recommend the use of Auto WB as each frame gets its own unique white balance which can prevent calibration frames such as flats and darks from working correctly.

We've also fixed a number of problems and made some other enhancements since 4.1.1

1. Support for images from cameras that use the Fujitsu X-Trans sensor (new in 4.2.0 Beta 2).
   The image is converted to a 3 colour RGB image and interpolated using Frank Markesteijn's algorithm using LibRaw::Process().
   Please note that the interpolation is quite slow (about 40 seconds per image on my Core i7 laptop).
   The image is then imported as if it were a Foveon colour image.

2. BSD 3 clause license file added.

3. A fix was made to correct crashes caused by a thread-unsafe assignment in the smart pointer code.

4. The "Stop" button on the progress dialogs should now actually work!

5. The space key can now be used to check/uncheck images in the picture list.

6. The user's default browser will be used instead of IE to open URLs.

7. Miscellaneous code cleanups and performance improvements including Median-Kappa-Sigma processing.

Welcome to DeepSkyStacker 4.1.1

    The 32 bit version has been changed to build the zlib code without the optimising "asm" option to fix a crashing
	problem ("asm" is not officially supported in zlib and has not been maintained)


    The 64 bit version has been changed to build the zlib code without the optimising "asm" option as a precaution 	(the 64 bit
	version wasnt crashing as far as we know but as the "asm" option isn't officially supported we have elected not to use it)

*****

Welcome to DeepSkyStacker 4.1.0

This version of DeepskyStacker is supported on Windows 7, Windows 8, Windows 8.1 and Windows 10 and completely
supercedes DeepSkyStacker 3.3.2, 3.3.4 and 3.3.6.

We now provide DeepSkyStacker and the other programs as both 32-bit (x86) and 64-bit (x64) builds based upon the latest
version of dcraw. There are separate installers for the 32-bit and 64-bit versions.

Installing the 32-bit build will remove earlier versions of DeepSkyStacker (3.3.2 and upward). If you are running an
earlier version than 3.3.2, please remove it manually.

You can install both the 32-bit and the 64-bit versions on 64-bit versions of Windows without problems.

PLEASE NOTE: If anyone who installs this gets: "This program can't start because api-ms-win-crt-runtime-l1-1-0.dll is
missing" when they try to open the programs, the solution is to install:

https://support.microsoft.com/en-us/help/2999226/update-for-universal-c-runtime-in-windows

for your specific version of Windows.

This problem should only affect users of Vista / Win7 / Win 8.x who are behind on WindowsUpdates, or are running
an offline system and opting out of WindowsUpdates.
Enjoy,

Luc Coiffier, Tony Cook, David C. Partridge

We've fixed a number of problems since 4.1.0 beta 1:

    Fixed a problem when using drizzle and Intersection mode which resulted in an output image that was 4x or 9x
	too large with the image in the upper left corner.

    Worked around a problem with incorrect display of the left panel on high resolution monitors.
	(Set the DPI Aware flag in the application manifest to No).

    Changed our build of libtiff so that we use Unix style rather than Win32 style I/O as the Win32 code is
	wrong in x64 mode (known problem with libtiff).

    A number of other size_t/DWORD/long issues in the code were fixed for the 64 bit build (to ensure computed
	values were indeed 64 bit sized).

    Small change to improve the responsiveness of the post-processing screen (faster display of the final image).

