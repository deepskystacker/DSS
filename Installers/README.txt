Welcome to DeepSkyStacker 5.1.6 Beta 1
===============================

Only 64 bit versions of Windows 10 and later are supported in this release.

Changes since the last release:

1. Bug fix: The Image List dock widget was not always visible (e.g. after DSS was maximised and then closed with the processing dialog visible).

Welcome to DeepSkyStacker 5.1.5
===============================

Only 64 bit versions of Windows 10 and later are supported in this release.

Known problems:

1. When the image exposure is less than 1ms and double-click to edit is used, if the user clicks away from the editor, then the exposure is set to zero.
   This requires too much work to fix in this release, as we will need to implement our own edit control.

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

2. The image list can now be undocked from the bottom of the Stacking panel so that it operates as a separate window.  The "Explorer" bar (left panel) can also be undocked.

3. It is now possible to rename all groups with the exception of the initial group (Main Group).

4. Some fields in the image list (Type, ISO/Gain, and Exposure) can be double-clicked to change the values.

5. A large number of internal changes have been made with the intent of facilitating future enhancements and/or to improve processing.

6. SIMD (Single Instruction Multiple Data - also known as Advanced Vector Extensions or AVX) support for decoding raw images and for registration and stacking of RGGB images.  It *can* deliver dramatic reductions in processing times, but it depends on your processor and clock speed, so don't assume it will be faster.   As an example, Martin Toeltsch (who wrote the code) reports times to process 10 Nikon NEF files (on his computer):

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

11. The location for storing DeepSkyStacker settings files has changed from %ProgramData%\DeepSkyStacker (typically C:\ProgramData\DeepSkyStacker) to %AppData%\DeepSkyStacker\DeepSkyStacker5 (typically C:\Users\<username>\AppData\Roaming\DeepSkyStacker\DeepSkyStacker5).  You may wish to copy any old settings files to the new location.

12. A file association is now created during installation so that .dssfilelist files will be opened by DeepSkyStacker.

13. Add code to capture non C++ exceptions (e.g. SIGINT, SIGILL, SIGFPE, SIGSEGV, and SIGTERM) and write a debugging backtrace to stderr and to the trace file if active.

14. Change message for incompatible images to report the reason.

24. Registering and stacking now overlap processing with reading the images.   For n images where time to load each image is L and time to process each image is P, the total time will now typically be n*L + P (when L > P) or L + n*P.   Typically, the time to load the images will predominate on faster systems or those that use real disk drives.

15. Remove manual setting of "Set Black Point to Zero", this is now determined automatically.

16. Enable the Comet tab in Stacking Settings when it is invoked from Register Settings and Comet data is available.

17. Change LibRaw supported camera list so that "Olympus OM-1" is recognised as well as "OM Digital Solutions OM-1"

18. Update Libraw to 0.21.1

19. Bug fix - active tab jumped back to Main Group after drop of files when another group was active.
