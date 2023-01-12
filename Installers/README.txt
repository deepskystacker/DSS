Welcome to DeepSkyStacker 5.1.0 Beta 2

This release is the start of the process of converting the code to Qt so that it can be ported to platforms other than Windows.
Only 64 bit versions of Windows 10 and later are supported in this release.

The translation of messages and the UI should now be complete.

Here are the main changes that were made for DeepSkyStacker 5.1.0:

1. The bulk of the code for the "Stacking" panel has been converted to Qt.  This includes a completely reworked image display.

2. The image list can now be undocked from the bottom of the Stacking panel so that it operates as a separate window.  The "Explorer" bar (left panel) can also be undocked.

3. It is now possible to rename all groups with the exception of the initial group (Main Group).

4. Some fields in the image list (Type, ISO/Gain, and Exposure) can the double-clicked to change the values.

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

13. Update Libraw to 0.20.2

14. Fix a problem where the image list wasn't always hidden when switching to the Processing panel.

15. Reduce the depth of the rows in the image list so it takes less space.

16. Change to AVX checking code to resolve a problem with SIGILL on older CPUs.

17. Add code to capture non C++ exceptions (e.g. SIGINT, SIGILL, SIGFPE, SIGSEGV, and SIGTERM) and write a debugging backtrace to stderr and to the trace file if active.

18. Update Libraw to 0.21.0

19. Bug fix for Comet/Star editing mode being forgotten when switching between images.

20. Don't remove files from image list if user says not to erase them.  Ensure image list information is updated.

21. Fix for incorrect output when Comet Stacking with SIMD (it just did a normal stack).

22. Fix for some issues with the progress dialog.   More to come.

23. Ensure that the image list as always displayed when opening DSS.