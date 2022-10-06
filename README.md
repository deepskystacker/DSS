DeepSkyStacker
==============

This is the github repository for all the DeepSkyStacker source code.

DeepSkyStacker is a freeware created in 2006 by Luc Coiffier

It was open sourced in 2018 and is maintained by a small and dedicated team:

	David C. Partidge, Tony Cook, Mat Draper, Simon C. Smith, Vitali Pelenjow,
	Michal Schulz, Martin Toeltsch

The code is open source under the terms of the BSD 3-Clause License (see LICENSE).

Copyright (c) 2006-2019, LucCoiffier 
Copyright (c) 2018-2022, David C. Partridge, Tony Cook, Mat Draper,
					Simon C. Smith, Vitali Pelenjow, Michal Schulz,
					Martin Toeltsch
					
Building DeepSkyStacker:
------------------------

You will need to install Visual Studio 2019 (16.11.18), Qt 6.4.0 and Qt VS Tools 2.9.1 (rev 6) for
VS2019.   You can also use VS2022 and Qt VS Tools 2.9.1 (rev 6) for VS2022.

Please **do not** accept VS2022's offer to upgrade the build environment the first time you open the
DeepSkyStacker.VS2019.sln file

You will also need to install Boost 1.80.0.  Once you have done that, you need to create an environment
variable called Boost_1_80_0 with the location of the Boost installation directory e.g.:
C:\Boost_1_80_0

You will need to use *Extensions/Qt VS Tools/Qt Versions* to set up a named Qt Installation:

	Called 6.4.0_msvc2019_64 pointing to the msvc2019_64 sub-directory of your Qt install
		for example C:\Qt\6.4.0\msvc2019_64
		
You will also need to install [Visual Leak Detector](https://github.com/Azure/vld/releases), which
the project will expect to find at C:\Program Files (x86)\Visual Leak Detector.
In the Visual Leak Detector installation wizard, be sure to check "Add VLD directory to your environmental path".

