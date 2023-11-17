/****************************************************************************
**
** Copyright (C) 2023 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
// BitmapInfo.cpp : implementation file
//
#include "stdafx.h"
#include "BitmapInfo.h"
#include <zexcept.h>
#include <Ztrace.h>

using namespace Exiv2;

// Type for an Exiv2 Easy access function
using EasyAccessFct = Exiv2::ExifData::const_iterator(*)(const Exiv2::ExifData&);

bool RetrieveEXIFInfo(const fs::path& fileName, CBitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool result{ false };
	//
	// Use Exiv2 C++ class library to retrieve the EXIF information we want
	//
	std::string temp{ reinterpret_cast<const char*>(fileName.generic_u8string().c_str()) };
	auto image = ImageFactory::open(temp);
	ZASSERT(image.get() != nullptr);
	image->readMetadata();
	auto& exifData{ image->exifData() };
	ZTRACE_RUNTIME("Retrieving EXIF data from file: %s", fileName.generic_u8string().c_str());
	if (exifData.empty())
	{
		ZTRACE_RUNTIME("No EXIF data found in file");
		return result;
	}

	ExifData::const_iterator iterator{ };

	// Exposure time
	if (iterator = exposureTime(exifData); exifData.end() != iterator)
	{
		BitmapInfo.m_fExposure = iterator->toFloat();
		result = true;
	}
	
	// Aperture
	if (iterator = fNumber(exifData); exifData.end() != iterator)
	{
		BitmapInfo.m_fAperture = iterator->toFloat();
		result = true;
	}
	//else if (iterator = apertureValue(exifData); exifData.end() != iterator)
	//{
	//	BitmapInfo.m_fAperture = iterator->toFloat();
	//	result = true;
	//}

	// ISO
	if (iterator = isoSpeed(exifData); exifData.end() != iterator)
	{
		BitmapInfo.m_lISOSpeed = iterator->toInt64(0);	// Return 1st element if multi-element IFD
		result = true;
	}

	// Model
	if (iterator = model(exifData); exifData.end() != iterator )
	{
		BitmapInfo.m_strModel = QString(iterator->toString().c_str()).trimmed();
		result = true;
	}
	
	// Date/Time
	if (iterator = dateTimeOriginal(exifData); exifData.end() != iterator)
	{
		QString strDateTime{ iterator->toString().c_str() };

		// Parse the string : YYYY:MM:DD hh:mm:ss
		//                    0123456789012345678
		BitmapInfo.m_DateTime = QDateTime::fromString(strDateTime, "yyyy:MM:dd hh:mm:ss");
		result = true;
	}
	
	return result;
}