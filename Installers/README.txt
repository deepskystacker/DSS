Welcome to DeepSkyStacker 5.1.8 Beta 5
======================================

Only 64 bit versions of Windows 10 and later are supported in this release.

This release is all about finishing the conversion of the code to use Qt instead of MFC.  That said, there are a few bug fixes,
and a number of enhancements as well.   This isn't a complete list as many changes were made to improve code quality and 
performance.

Known problems:

1. When the image exposure is less than 1ms and double-click to edit is used, if the user clicks away from the editor, then the exposure is set to zero.
   This requires too much work to fix in this release, as we will need to implement our own edit control.

2. If you install DeepSkyStacker and it won’t startup, then it is likely that you have hit a problem caused by another application’s install
   procedure overwriting msvcp140.dll with a back-level version.

   If this is the case, then you will find that the log file in “My Documents\DeepSkyStacker” contains lines like:

	00000030 2024/06/19 21:30:57.010 028564 00003520     >00007FFC81353020 (MSVCP140): (filename not available): Thrd_yield
	00000030 2024/06/19 21:30:57.010 028564 00003520     >
	00000031 2024/06/19 21:30:57.011 028564 00003520     >C:\Users\amonra\Documents\GitHub\DSS\DeepSkyStackerKernel\Workspace.cpp (456): Workspace::Workspace

   The solution to this problem is to download the latest version of the Visual C++ re-distributable (vc_redist) for x64 from:

	https://aka.ms/vs/17/release/vc_redist.x64.exe

   and running it, selecting the “Repair” option.

   For more details of the issue please see:

	https://stackoverflow.com/questions/78598141/first-stdmutexlock-crashes-in-application-built-with-latest-visual-studio

Changes since the last release:

1. Bug fix: When saving the project to a file-list, a default file name is suggested which is equal to the name of the current directory.

2. Bug fix: The circles around the stars and the comet stayed even after clearing the file list.

3. Lower the minimum detection threshold for registering from 2% to 1%.

4. Bug fix: Crash caused by trying to de-bayer 32 bits FITS files with the method AHD.

5. Bug fix: AVX support to prevent Illegal Instruction problem on earlier CPU types (e.g. Xeon X5650).

6. Bug fix: Add check box to control display of Black/White clipping in Processing panel.

7. Bug fix: Custom rectangle produces a partly empty stacking result when the SIMD acceleration is turned ON.

8. Implement an automatic threshold for star detection (which is the default) and a much better image quality indication (Quality) which is a
   Gaussian mean of star quality as compared to the Score which was the sum of all star qualities (calculated differently) and was dependent
   on the number of stars as well as their quality.

   The automatic threshold will be in the range 0.05% and 100%, and the target number of stars is ~50.

   The new Quality metric is primarily based on the average circularity ("roundness") of the stars in the image. It is
   largely independent of the number of detected stars.

   The background level, to which the star detection threshold refers, is now calculated locally per 250x250 pixel square rather than
   globally over the entire image.  This compensates for any background illumination gradients.

   All light frame sorting criteria have been switched to use the new Quality value instead of the Score (e.g. for stacking the best x% light frames).

   The first light frame to register will be either the first in the frame list, or the reference frame (if one was chosen). If auto-threshold
   is used, the detected threshold of this light frame will serve as the basis for the threshold search of the following frames.

   We suggest that you ensure that the first registered light frame is not significantly darker then the other frames (brighter is no problem),
   otherwise the number of detected stars might be very high.

9. Bug fix: Computation of the final star centres has been corrected. The old version was slightly biased towards left/up of the real centre.

10. Finish conversion of code to Qt - there should be no MFC stuff left.

11. Upgrade LibRaw to 0.23.1

12. Upgrade LibTIFF to 4.7.0

13. Bug fix: Fix broken handling of Up-Arrow, Right-Arrow, Down-Arrow, Left-Arrow; Page-Up and Page-Down keys for the sliders of the gradient control.

14. Bug fix: Attempting to load a FITS file from the image list failed with a message: "Failed to load ...".   This was caused by a change in the Mime
    types database shipped with the latest version of Qt.  It used to assign a type of "image/fits" to FITS files, but now assigns "application/fits".

15. Remove the code to check for LibRaw support of a camera.  The data in the camera list wasn't really good for that check and resulted in false
    negatives and false positives however hard we tried to "tweak" the code.

    This shouldn't cause any problems as a "not supported" decision simply resulted in a single warning message.

16. Bug fix: Add a small windows resource file so the application icon gets set.

17. Write all output text streams to a QByteArray buffer, and then use a single QFile::write() to write them.

18. Changes to allow the DeepSkyStacker main window to display correctly on a monitor with an effective size of 1280*720 (1920*1080 at 150% scaling).
    This was achieved by moving the controls for the processing panel into the lower dockable window and allowing the displayed image to have a
    minimum height of 100 pixels.   If you need to show the image larger, undock the lower dock window.

19. Add translations for Simplified Chinese thanks to 张博 (Zhang Bo).

20. Changes to DeepSkyStackerLive to use the new Quality metric instead of Score.

Welcome to DeepSkyStacker 5.1.6
===============================

Only 64 bit versions of Windows 10 and later are supported in this release.

This is primarily a bug fix release, but there are a few enhancements as well.

Changes since the last release:

1. Bug fix: The Image List dock widget was not always visible (e.g. after DSS was maximised and then closed with the processing dialog visible).

2. Modify CFITSIO 4.3.0 to support UTF-8 file names.  The fix has been submitted to the FITSIO team who look like they will adopt it.

3. Bug fix: Bright stars had a black centre (another cause of this fixed).

4. Support for non-ASCII characters in the path of the trace file.

5. Support for non-ASCII characters in the interprocess file lock.

6. Support for non-ASCII characters in the data to be written to the trace file.

7. Bug fix: Crash calling getValue() for colour images.

8. Bug fix: Problems with handling the name of directory for temporary file

9. Bug fix: Monochrome JPEG and PNG images wouldn't load.

10. Show white clipping and dark clipping in the Processing panel editing function as Red and Blue respectively.  Provide an option to enable/disable this.

11. Bug fix: Error processing colour TIFF files - code was checking for CFA information when it should not have.

12. Bug fix: File groups messed up after clearing the image list.

13. Bug fix: The output file did not have an appended file number when created from a file list name.

14. Bug fix: If another file list is selected the output file name was not updated.

15. Make the default path for a new file list the parent folder of the first light frame.

16. Bug fix: Unable to load monochrome JPEG or PNG files.

17. Bug fix: The picture list lost focus as an image finished loading.

18. Bug fix: FITSUtil::Open() did not correctly parse a FITS DATE-OBS or TIME-OBS containing milliseconds.

19. Bug fix: A number of problems with star and comet editing were fixed thanks to Martin Toeltsch.

20. Bug fix: Remove developer debugging code that required a C:\temp directory and just exited if it wasn't there.

21. Insert a "-" character beween filelist name and numeric suffix so the output file is called e.g.: NGC7331-001.tif rather NGC7331001.tif

22. Bug fix: Allow only one frame to be selected as the "reference frame".

23. Bug fix: Crash during startup while initialising the "Workspace" which caches the settings.

24. Bug fix: Crash during startup initialising ImageListModel array of icons.

Welcome to DeepSkyStacker 5.1.5
===============================

Only 64 bit versions of Windows 10 and later are supported in this release.

Changes since the last release:

1. Write Intermediate TIFF format files with COMPRESSION_NONE.  Final output files will still be compressed.

2. Remove any .stackinfo.txt files that already exist when re-registering images.
   This prevents incorrect registration of images when changing stacking settings and de-Bayer settings.

3. Change Raw/FITS DDP settings so that the Bayer transformation (Bilinear/AHD/Drizzle/SuperPixel) on the two tabs “mirror one another”.
   That’s to say if you change those settings on the FITS tab, then the RAW tab is also changed and vice-versa.
   There are no longer separate settings for the Bayer transformation for RAW and FITS (which has caused problems in the past).

4. Bug fix: Revert change to create masters as float - it caused incompatibility problems.

Welcome to DeepSkyStacker 5.1.4
===============================

Changes since the last release:

1. Upgrade CFITSIO library to 4.2.0

2. Always create a trace file in the DeepSkyStacker sub-folder of the user's Documents folder.  The file will be called e.g.

	DSSTrace_yyyy-mm-ddThh-hh-ssZ.log where the timestamp is GMT (UTC) time.

	The trace file will be deleted on normal application exit, but retained in the event of an exception.
	An option is provided to keep the file.

3. Add both Main Group and Group 1 at startup.

4. Initial code changes for Qt 6.5.1 Dark/Light Theme support

5. Add an option to turn off beeps on completion of registering/stacking.

6. Toolbar display improved (no square boxes round active/hovered buttons).

7. Upgrade libtiff to 4.5.0

8. Add code to handle TIFF tags TIFFTAG_CFAREPEATPATTERNDIM and TIFFTAG_CFAPATTERN when processing TIFF files in CFA format. These tags are defined in the TIFF/EP standard.
   Add code to handle EXIF tag EXIFTAG_CFAPATTERN which is defined in the EXIF standard.
   This allows automatic detection of the CFA pattern needed to decode the file.
   The only files that DSS writes as TIFF in CFA format are some of the master files.
   All other TIFF output files will remain RGB.

9. Change compression of TIFF format master files to use PKZIP Deflate instead of no compression.

10. Change default compression for saving images to PKZIP Deflate (was NONE).

11. Convert DeepSkyStackerLive to Qt.

12. Use floating point instead of 16-bit fixed point for master files.  Old master files will still work.

13. Prevent undocked sidebar and image list windows from being closed (never to be seen again).

14. Suppress warning message "ZIPEncode: Encoder error: buffer error." when writing TIFF files.

15. Remove Microsoft CGI+ dependency for loading jpeg and png images.

16. FITS output files didn't contain the number of images in the stack.  Save this using FITS keyword NCOMBINE, also report the number of images when loading FITS images.

17. Bug fix: Correct code for reading floating point FITS files (the value was incorrectly normalised - e.g.: 256.0 was read back as 259.996).

18. Bug fix: Value of FITS SOFTWARE keyword in output file truncated after first character.

19. Bug fix: Unable to stack images with no stars when using Alignment set to "No Alignment".

20. Improve handling for FITS DATE-OBS keyword.  DATE-OBS in the output file will now be set to the value of DATE-OBS from the first input file or, failing that, the creation timestamp or last modification timestamp of the file whichever is earlier.

21. Processing/Save picture to file... did not preserve FITS keywords when the final autosave file was written as a FITS file.

22. Bug fix: Delete key to delete images from the list didn't update the information lines.

23. Bug fix: Clear list didn't reset the status bar message.

24. Work around for Qt bug (QTBUG-46620) with restoring application position and size when re-opening the application after it was closed when maximised.

25. Mouse wheel zooming worked the opposite way from most other applications, now changed to be compatible.

26. Extend exposure time edit control to 4 decimal places instead of 3.

27. Check lights when loaded using either "Open picture files" or using Drag/Drop.

28. Correct Intermediate Files tab of Stacking parameters.  "Create a calibrated file ..." and "Create a registered/calibrated file ..." were inadvertently switched when porting the code for version 5.

29. Bug fix: Saving the intermediate calibrated image in de-Bayered format resulted in a corrupt file.

30. The comet position interpolation code has been re-written by Martin Toeltsch (thank you Martin).  Now you need only to:

    o Mark the position of the centre of the comet in 2 light frames with a large temporal distance (ideally the first and the last light frame in chronological order).

    o Marking the comet centre in additional light frames can potentially improve the results.

31. Change some of the colours used when the Windows Dark Theme is in use so that they are easier to read.

32. Revert change made for number 26 - QTimeEdit doesn't work right with 4 decimals.

33. Bug fix: Magnifying glass over image area was still displayed when mouse was moved directly to another window.

34. Bug fix: Selecting/Deselecting "Create a calibrated file for each light frame" should also Enable/Disable "Save a debayered image..."

35. Bug fix: Unable to cancel Registering or Stacking because the ProgressDlg was disabled (it had the wrong parent).

36. Bug fix: Save/Restore of docked window sizes (Explorer Bar, Picture List) wasn't working.

37. Update zlib to 1.3 (from 1.2.11).

38. Bug fix: Switching away from Settings/Output tab and then switching back reset the values to status quo ante.

39. Bug fix: Output file name when using file list name as the output name was incorrectly set to (e.g.) Batch1.tif when using a file list with a name Batch1.xxx.dssfilelist

40. Clear image list at end of batch stacking.

41. Upgrade CFITSIO library to 4.3.0

42. Bug fix: If a recommended setting is clicked to activate it the revised recommendations are appended to the browser when it should be cleared before they are displayed.

43. Bug fix: DSS-CL: If an output file is named specifically with the /O: parameter, this output file is always used. If it is omitted, it will generate a unique output file instead.

44. Bug fix: DSS-CL: If the compression level for a TIFF file is set via the /OCx parameter, this is now honoured.

45. Bug fix: When registering a set of images with different exposure times, the progress dialogue restarted counting at 1 and the progress bar was reset to 0 for each different exposure. The estimated time for completion got completely confused, and the progress bar reached 100% a number of times.

46. Bug fix: Prevent TIFF code issuing TIFFSetField: D:/dss.tif: Unknown pseudo-tag 65557, by only setting TIFFTAG_ZIPQUALITY if compression is set to COMPRESSION_DEFLATE.

47. Change the minimum update interval for the progress dialog to 0.1s (was 1s).

48. Bug fix: Units on the Processing pane's Luminance tab were displayed incorrectly as Å° instead of °.

49. Bug fix: Deadlock in DSSLive when stacking FITS files because CFITSReader::Read() didn't issue an End2() call to match the Start2() call.

50. Bug fix: Null pointer passed to XYSeries::append() by ChartTab.cpp.  Some code tidyups as well.

51. Add "How To" information to the help for the image editor built into DeepSkyStacker. This includes a short screen capture movie.

52. Resolve problem of clipped controls on processing page when the native language is set to Japanese.

Welcome to DeepSkyStacker 5.1.3
===============================

This is a bug fix release for problems reported against 5.1.0, 5.1.1, 5.1.2

1. Possible bug fix - DeepSkyStacker terminated at startup when running on ARM version of Windows 11 in x64 emulation mode.  Unable to test.

2. Bug fix - A corrupt info.txt file caused an infinite loop.

3. Diagnostic added - Report processor architecture and processor type in trace file and to stderr at startup.

4. Bug fix - Stacked FITS images in SuperPixel mode were displayed only in the top left corner.

5. Bug fix - Resolve occasional odd problems when using edit stars mode.

6. Bug fix - DeepSkyStackerCL was only displaying the help text regardless of command line input.

7. Enhancement - Reinstate Image Properties as a Qt based dialogue to allow changing e.g. exposure for multiple images at once

8. Bug fix - Fields in the image list and the group tabs were not updated when switching to another language.

9. Bug fix - remove all "Set Black Point to Zero" recommendations from "Recommended Settings"

10. Bug fix - Invalid input in RAW/DDP settings for scale factors caused an assertion failure

11. Bug fix - correct handling of file types (TIFF/FITS) in DeepSkyStackerCL for intermediate and final files

12. Bug fix - correct handling of Stacking Mode (Standard/Mosaic/Intersection/Custom)

13. Bug fix - unable to select a custom rectangle immediately after opening image file

14. Bug fix - fileids in filelist files were being incorrectly written as ANSI not UTF8

15. Bug fix - the selection for a custom rectangle was not always visible

16. Enhancement - reduce the minimum size for the image list to be two rows

17. Bug fix - correct problems with drag drop of a directory.

18. Enhancement - if Custom Rectangle mode is read from a filelist or settings file, switch to Intersection mode.

19. Bug fix - correct handling of reference frame in filelist.

20. Bug Fix - FITS/DDP choice of Camera was not being handled correctly.

21. Bug fix - ensure that reference frame is used a) when checked, and b) when not checked

22. Bug fix - Register settings set a value of 0 for the luminance threshold when it was initially set to 20.

Welcome to DeepSkyStacker 5.1.0
===============================

This release is the start of the process of converting the code to Qt so that it can be ported to platforms other than Windows.

Here are the main changes that were made for DeepSkyStacker 5.1.0:

1. The bulk of the code for the "Stacking" panel has been converted to Qt.  This includes a completely reworked image display.

2. The image list can now be undocked from the bottom of the Stacking panel so that it operates as a separate window.
   The "Explorer" bar (left panel) can also be undocked.

3. It is now possible to rename all groups with the exception of the initial group (Main Group).

4. Some fields in the image list (Type, ISO/Gain, and Exposure) can be double-clicked to change the values.

5. A large number of internal changes have been made with the intent of facilitating future enhancements and/or to improve processing.

6. SIMD (Single Instruction Multiple Data - also known as Advanced Vector Extensions or AVX) support for decoding raw images and for
   registration and stacking of RGGB images.  It *can* deliver dramatic reductions in processing times, but it depends on your
   processor and clock speed, so don't assume it will be faster.
   As an example, Martin Toeltsch (who wrote the code) reports times to process 10 Nikon NEF files (on his computer):

	Without SIMD  52s
	Using SIMD    8s
	
   This also works for GBRG images so Canon CR2 files will benefit from this work as well.   
	
7. Some further tuning of the OpenMP (multi-processor support) has been done.

8. The "Stacking" panel image display now caches the last twenty images displayed, so you can use it as a "blink comparator"

9. The configured settings that are stored in the Windows registry are not compatible with earlier releases which stored them in the registry hive:

   HKCU\Software\DeepSkyStacker\DeepSkyStacker

   so now the settings are held in a separate registry hive:

   HKCU\Software\DeepSkyStacker\DeepSkyStacker5

10. The "Processing" panel is still running MFC code but has minor changes to allow it to work as a child of a Qt window.

11. The location for storing DeepSkyStacker settings files has changed from %ProgramData%\DeepSkyStacker (typically C:\ProgramData\DeepSkyStacker)
   to %AppData%\DeepSkyStacker\DeepSkyStacker5 (typically C:\Users\<username>\AppData\Roaming\DeepSkyStacker\DeepSkyStacker5).
   You may wish to copy any old settings files to the new location.

12. A file association is now created during installation so that .dssfilelist files will be opened by DeepSkyStacker.

13. Add code to capture non C++ exceptions (e.g. SIGINT, SIGILL, SIGFPE, SIGSEGV, and SIGTERM) and write a debugging backtrace to stderr and to the trace file if active.

14. Change message for incompatible images to report the reason.

24. Registering and stacking now overlap processing with reading the images.   For n images where time to load each image is L
   and time to process each image is P, the total time will now typically be n*L + P (when L > P) or L + n*P.
   Typically, the time to load the images will predominate on faster systems or those that use real disk drives.

15. Remove manual setting of "Set Black Point to Zero", this is now determined automatically.

16. Enable the Comet tab in Stacking Settings when it is invoked from Register Settings and Comet data is available.

17. Change LibRaw supported camera list so that "Olympus OM-1" is recognised as well as "OM Digital Solutions OM-1"

18. Update Libraw to 0.21.1

19. Bug fix - active tab jumped back to Main Group after drop of files when another group was active.

20. When loading a file list that contains a file that's already loaded, report this using QErrorMessage instead of QMessageBox.
   This will allow the message to be suppressed by the user.
