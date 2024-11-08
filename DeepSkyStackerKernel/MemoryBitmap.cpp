#include "stdafx.h"
#include "MemoryBitmap.h"
#include "ztrace.h"
#include "CFABitmapInfo.h"

CMemoryBitmap::CMemoryBitmap() :
	m_bTopDown{ false },
	m_bMaster{ false },
	m_bCFA{ false },
	m_fExposure{ 0.0 },
	m_fAperture{ 0.0 },
	m_lISOSpeed{ 0 },
	m_lGain{ -1 },
	m_lNrFrames{ 0 }
{
}

void CMemoryBitmap::CopyFrom(const CMemoryBitmap& mb)
{
	m_ExtraInfo = mb.m_ExtraInfo;
	m_bTopDown = mb.m_bTopDown;
	m_bMaster = mb.m_bMaster;
	m_bCFA = mb.m_bCFA;
	m_fExposure = mb.m_fExposure;
	m_fAperture = mb.m_fAperture;
	m_lISOSpeed = mb.m_lISOSpeed;
	m_lGain = mb.m_lGain;
	m_lNrFrames = mb.m_lNrFrames;
	m_strDescription = mb.m_strDescription;
	m_DateTime = mb.m_DateTime;
	m_filterName = mb.m_filterName;
}

void CMemoryBitmap::GetPixel16(const size_t i, const size_t j, COLORREF16& crResult) const
{
	constexpr double scalingFactor = double{ 1. + std::numeric_limits<unsigned char>::max() };
	constexpr double maxValue = double{ std::numeric_limits<unsigned short>::max() };
	// Use get pixel
	double fRed, fGreen, fBlue;
	GetPixel(i, j, fRed, fGreen, fBlue);

	crResult.red = static_cast<std::uint16_t>(std::min(fRed * scalingFactor, maxValue));
	crResult.green = static_cast<std::uint16_t>(std::min(fGreen * scalingFactor, maxValue));
	crResult.blue = static_cast<std::uint16_t>(std::min(fBlue * scalingFactor, maxValue));
}
