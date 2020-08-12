DeepSkyStacker

This is the github repository for all the DeepSkyStacker source code.

DeepSkyStacker is a freeware created in 2006 by Luc Coiffier

It was open sourced in 2018 and is maintained by a small and dedicated team:

	David C. Partidge, Tony Cook, Mat Draper, Simon C. Smith, Vitali Pelenjow, Michal Schulz

The code is open source under the terms of the BSD 3-Clause License (see LICENSE).

Copyright (c) 2006-2019, LucCoiffier 
Copyright (c) 2018-2020, David C. Partridge, Tony Cook, Mat Draper,
					Simon C. Smith, Vitali Pelenjow, Michal Schulz
					
Building DeepSkyStacker:

You will need to install Visual Studio 2019 (16.7.1), Qt 5.15.0 and Qt VS Tools 2.5.2 for
VS2019.

You will need to use QT VS Tools Options dialog to set up two named Qt Installations:

	1. Called 5.15.0x86 pointing to msvc2019 sub-directory of your Qt install
		for example C:\Qt\5.15.0\msvc2019
	2. Called 5.15.0x64 pointing to the msvc2019_64 sub-directory of your Qt install
		for example C:\Qt\5.15.0\msvc2019_64
		
In addition you will need to set an environment variable called QtInstDir set to the
value C:\Qt\5.15.0\msvc2019_64 (assuming you are running on 64-bit Windows).

