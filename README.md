DeepSkyStacker
==============

This is the github repository for all the DeepSkyStacker source code.

DeepSkyStacker is freeware created in 2006 by Luc Coiffier

It was open sourced in 2018 and is maintained by a small and dedicated team:

	David C. Partidge, Tony Cook, Mat Draper, Simon C. Smith, Vitali Pelenjow,
	Michal Schulz, Martin Toeltsch, Iustin Amihaesei

The code is open source under the terms of the BSD 3-Clause License (see LICENSE).

Copyright (c) 2018-2025, David C. Partridge, Tony Cook, Mat Draper,
					Simon C. Smith, Vitali Pelenjow, Michal Schulz,
					Martin Toeltsch, Iustin Amihaesei

Copyright (c) 2006-2019, Luc Coiffier 

[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/deepskystacker/DSS)
					
Building DeepSkyStacker:
------------------------

To work on the current development codebase (for DeepSkyStacker 6.1.2) which uses a VS solution file called DeepSkyStacker.VS2022.sln,
you will need to install Visual Studio 2026.   For earlier code versions either VS2019 or VS2022 will work.

If you open a DeepSkyStacker solution file for 6.1.1 or earlier releases, please **do not** accept VS2026's offer to upgrade the build
environment the first time you open the solution file.

In addition to Visual Studio, you will need to install Qt 6.10.0, Qt VS Tools and Visual Leak Detector (see below).

You also need to install vcpkg on Windows and create an Lubuntu 22.04 build system (typically in a Virtual Machine).
Instructions on setting up the Linux build system are in "Setup Linux.txt".

There are also instructions for setting up a macOS build system.

### Qt 6.10.0,Qt VS Tools
Qt 6.10.0 is needed to build DeepSkyStacker 6.1.1 and the current development (master branch).

In addition to you you will also need to install the Visual Studio Extension *Qt VS Tools* : (Extensions/Manage Extensions).

You will need to use *Extensions/Qt VS Tools/Qt Versions* to set up the named Qt Installation called:

*6.10.0_msvc2022_64*
	
pointing to the relevant sub-directory of your Qt installations (for example C:\Qt\6.10.0\msvc2022_64).

### Visual Leak Detector
You should install the version of VLD that is maintained by Alex Vanden Abeele as that now appears to be the only
one that is being kept up to date.   The version what was maintained by the Microsoft Azure team hasn't been updated for a long time.
You can download version 2.5.12 of this from [Visual Leak Detector](https://github.com/avadae/vld/releases/tag/v2.5.12).
You should install it to C:\Program Files (x86)\Visual Leak Detector as that is the location that the DeepSkyStacker projects expect to find it.
In the Visual Leak Detector installation wizard, be sure to select Visual Studio integration.

