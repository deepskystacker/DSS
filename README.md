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

You will need to install Visual Studio 2017, Qt 5.12.8 and Qt VS Tools 2.4.3

You will need to use QT VS Tools Options dialog to set up two named Qt Installations:

	1. Called 5.12.8x86 pointing to msvc2017 sub-directory of your Qt install
		for example C:\Qt\5.12.8\msvc2017
	2. Called 5.12.8x64 pointing to the msvc2017_64 sub-directory of your Qt install
		for example C:\Qt\5.12.8\msvc2017_64
		
In addition you will need to set an environment variable called QtInstallDir set to the
value C:\Qt\5.12.8\msvc2017_64 (assuming you are running on 64-bit Windows).

