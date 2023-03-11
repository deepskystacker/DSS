DeepSkyStacker
==============

This is the github repository for all the DeepSkyStacker source code.

DeepSkyStacker is a freeware created in 2006 by Luc Coiffier

It was open sourced in 2018 and is maintained by a small and dedicated team:

	David C. Partidge, Tony Cook, Mat Draper, Simon C. Smith, Vitali Pelenjow,
	Michal Schulz, Martin Toeltsch, Iustin Amihaesei

The code is open source under the terms of the BSD 3-Clause License (see LICENSE).

Copyright (c) 2006-2019, Luc Coiffier 
Copyright (c) 2018-2023, David C. Partridge, Tony Cook, Mat Draper,
					Simon C. Smith, Vitali Pelenjow, Michal Schulz,
					Martin Toeltsch, Iustin Amihaesei
					
Building DeepSkyStacker:
------------------------

To work on the current development codebase which uses a VS solution file called DeepSkyStacker.VS2022.sln, you will need to install Visual Studio 2022 (17.5.1), Qt 6.4.0 and Qt VS Tools 2.10.1 (rev 2) for
VS2022.   For earlier code versions either VS2019 or VS2022 will work.

If you open DeepSkyStacker.VS2019.sln (for an earlier level) using VS2022, please **do not** accept VS2022's offer to upgrade the build environment the first time you open the
DeepSkyStacker.VS2019.sln file.

You will also need to install Boost 1.80.0.  Once you have done that, you need to create an environment
variable called Boost_1_80_0 with the location of the Boost installation directory e.g.:
C:\Boost_1_80_0

You will need to use *Extensions/Qt VS Tools/Qt Versions* to set up a named Qt Installation called:

	6.4.0_msvc2019_64
	
pointing to the msvc2019_64 sub-directory of your Qt install (for example C:\Qt\6.4.0\msvc2019_64).

We will upgrade to Qt 6.5.0 as soon as it ships because we want the Themes support that it provides, but you'll need to keep Qt 6.4.0 around to build 5.1.0.
When that happens you will also need to set up a named Qt Installation for that.
		
You will also need to install [Visual Leak Detector](https://github.com/oneiric/vld/releases/tag/v2.7.0), which
the project will expect to find at C:\Program Files (x86)\Visual Leak Detector.
In the Visual Leak Detector installation wizard, be sure to check "Add VLD directory to your environmental path".

