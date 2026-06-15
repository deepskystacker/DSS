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
#include "pch.h"
#include "BitmapInfo.h"
#include <zexcept.h>
#include <ztrace.h>

using namespace Exiv2;

// Type for an Exiv2 Easy access function
using EasyAccessFct = Exiv2::ExifData::const_iterator(*)(const Exiv2::ExifData&);

bool RetrieveEXIFInfo(const fs::path& fileName, BitmapInfo& BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool result{ false };
	//
	// Use Exiv2 C++ class library to retrieve the EXIF information we want
	//
	std::string temp{ reinterpret_cast<const char*>(fileName.generic_u8string().c_str()) };
	try
	{
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
			BitmapInfo.m_fExposure = static_cast<double>(iterator->toFloat());
			result = true;
		}

		// Aperture
		if (iterator = fNumber(exifData); exifData.end() != iterator)
		{
			BitmapInfo.m_fAperture = static_cast<double>(iterator->toFloat());
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
			BitmapInfo.m_lISOSpeed = static_cast<int>(iterator->toInt64(0));	// Return 1st element if multi-element IFD
			result = true;
		}

		// Model
		if (iterator = model(exifData); exifData.end() != iterator)
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
	}
	catch (std::exception& e)
	{
		const char* message{ e.what() };
		ZTRACE_RUNTIME("std::exception caught: %s", message);
		QString errorMessage{ message };
		DSSBase::instance()->reportError(errorMessage, "EXIV2 exception", DSSBase::Severity::Warning, DSSBase::Method::QErrorMessage);
		return false;
	}
	return result;
}

//////////////////////////////////////////////////////////////////////////
BitmapInfo::BitmapInfo()
{
	Init();
}

BitmapInfo::BitmapInfo(const BitmapInfo& bi)
{
	CopyFrom(bi);
}

BitmapInfo::BitmapInfo(const fs::path& fileName)
{
	Init();
	m_strFileName = fileName;
}

void BitmapInfo::CopyFrom(const BitmapInfo& bi)
{
	m_strFileName = bi.m_strFileName;
	m_strFileType = bi.m_strFileType;
	m_strModel = bi.m_strModel;
	m_lISOSpeed = bi.m_lISOSpeed;
	m_lGain = bi.m_lGain;
	m_fExposure = bi.m_fExposure;
	m_fAperture = bi.m_fAperture;
	m_lWidth = bi.m_lWidth;
	m_lHeight = bi.m_lHeight;
	m_lBitsPerChannel = bi.m_lBitsPerChannel;
	m_lNrChannels = bi.m_lNrChannels;
	m_bCanLoad = bi.m_bCanLoad;
	m_bFloat = bi.m_bFloat;
	m_CFAType = bi.m_CFAType;
	m_bMaster = bi.m_bMaster;
	m_bFITS16bit = bi.m_bFITS16bit;
	m_strDateTime = bi.m_strDateTime;
	m_DateTime = bi.m_DateTime;
	m_InfoTime = bi.m_InfoTime;
	m_ExtraInfo = bi.m_ExtraInfo;
	m_xBayerOffset = bi.m_xBayerOffset;
	m_yBayerOffset = bi.m_yBayerOffset;
	m_filterName = bi.m_filterName;
	wcsInfo = bi.wcsInfo;
}

void BitmapInfo::Init()
{
	m_lWidth = 0;
	m_lHeight = 0;
	m_lBitsPerChannel = 0;
	m_lNrChannels = 0;
	m_bCanLoad = false;
	m_CFAType = CFATYPE_NONE;
	m_bMaster = false;
	m_bFloat = false;
	m_lISOSpeed = 0;
	m_lGain = -1;
	m_fExposure = 0.0;
	m_fAperture = 0.0;
	m_bFITS16bit = false;
	m_xBayerOffset = 0;
	m_yBayerOffset = 0;
}

BitmapInfo& BitmapInfo::operator=(const BitmapInfo& bi)
{
	CopyFrom(bi);
	return (*this);
}

bool BitmapInfo::operator<(const BitmapInfo& other) const
{
	return (m_strFileName.compare(other.m_strFileName) < 0);
}

bool BitmapInfo::operator==(const BitmapInfo& other) const
{
	return this->m_strFileName.compare(other.m_strFileName) == 0;
}
bool BitmapInfo::CanLoad() const
{
	return m_bCanLoad;
}

bool BitmapInfo::IsCFA()
{
	return (m_CFAType != CFATYPE_NONE);
};

bool BitmapInfo::IsMaster()
{
	return m_bMaster;
};

void BitmapInfo::GetDescription(QString& strDescription)
{
	strDescription = m_strFileType;
	if (m_strModel.length() > 0)
		strDescription = m_strFileType + " " + m_strModel;
};

bool BitmapInfo::IsInitialized()
{
	return m_lWidth && m_lHeight;
}

