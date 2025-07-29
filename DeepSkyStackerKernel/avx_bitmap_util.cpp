/****************************************************************************
**
** Copyright (C) 2024, 2025 Martin Toeltsch
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
#include "pch.h"
#include "avx_bitmap_util.h"
#include "BitmapCharacteristics.h"


AvxBitmapUtil::AvxBitmapUtil(const CMemoryBitmap& b) :
	bitmap{ b },
	pBitmap{ nullptr }
{}

AvxBitmapUtil::AvxBitmapUtil(CMemoryBitmap& b) :
	bitmap{ b },
	pBitmap{ &b }
{}

int AvxBitmapUtil::getNrChannels() const
{
	CBitmapCharacteristics bitmapCharacteristics;
	bitmap.GetCharacteristics(bitmapCharacteristics);
	return bitmapCharacteristics.m_lNrChannels;
}

bool AvxBitmapUtil::isColorBitmap() const
{
	return getNrChannels() == 3;
}

template <class T>
bool AvxBitmapUtil::isColorBitmapOfType() const
{
	const auto* const p = this->getColorPtr<T>();
	const bool isColor = p != nullptr && p->isTopDown();
	if constexpr (std::is_same<T, float>::value)
		return isColor && p->IsFloat() && p->GetMultiplier() == 256.0;
	else
		return isColor;
}

//
// Force instantiation of the template functions for the types we need.
//
template bool AvxBitmapUtil::isColorBitmapOfType<unsigned short>() const;

template bool AvxBitmapUtil::isColorBitmapOfType<unsigned int>() const;

template bool AvxBitmapUtil::isColorBitmapOfType<float>() const;

template bool AvxBitmapUtil::isColorBitmapOfType<double>() const;

bool AvxBitmapUtil::isMonochromeBitmap() const
{
	return getNrChannels() == 1;
}

template <class T>
bool AvxBitmapUtil::isMonochromeBitmapOfType() const
{
	if (const auto* const p = this->getGrayPtr<T>())
	{
		// Note that Monochrome bitmaps are always topdown -> no extra check required! CF. CGrayBitmap::GetOffset().
		if constexpr (std::is_same_v<T, float>)
			return (p->IsFloat() && !p->IsCFA() && p->GetMultiplier() == 256.0);
		else if constexpr (std::is_same_v<T, std::uint16_t>)
			return (!p->IsCFA() || isMonochromeCfaBitmapOfType<std::uint16_t>());
		else
			return !p->IsCFA();
	}
	else
		return false;
}

template <class T>
bool AvxBitmapUtil::isCompatibleInputBitmap() const
{
	// Check that the input bitmap is compatible with AVX SIMD operations.
	return (isColorBitmapOfType<T>() || isMonochromeBitmapOfType<T>());
}

//
// Force instantiation of the template functions for the types we need.
//
template bool AvxBitmapUtil::isMonochromeBitmapOfType<unsigned short>() const;

template bool AvxBitmapUtil::isMonochromeBitmapOfType<unsigned int>() const;

template bool AvxBitmapUtil::isMonochromeBitmapOfType<float>() const;

template bool AvxBitmapUtil::isMonochromeBitmapOfType<double>() const;

template bool AvxBitmapUtil::isCompatibleInputBitmap<std::uint16_t>() const;

template bool AvxBitmapUtil::isCompatibleInputBitmap<std::uint32_t>() const;

template bool AvxBitmapUtil::isCompatibleInputBitmap<float>() const;

template bool AvxBitmapUtil::isCompatibleInputBitmap<double>() const;

template <class T>
bool AvxBitmapUtil::isMonochromeCfaBitmapOfType() const
{
	// CFA only supported for T=16 bits unsigned
	if constexpr (std::is_same_v<T, std::uint16_t>)
	{
		const auto* const pGray = this->getGrayPtr<T>();
		// We support CFA only for RGGB or GBRG Bayer matrices with BILINEAR interpolation and no offsets.
		return (pGray != nullptr && pGray->IsCFA() && pGray->GetCFATransformation() == CFAT_BILINEAR && pGray->xOffset() == 0 && pGray->yOffset() == 0
			&& (pGray->GetCFAType() == CFATYPE_RGGB || pGray->GetCFAType() == CFATYPE_GBRG));
	}
	else
		return false;
}

bool AvxBitmapUtil::isColorBitmapOrCfa() const
{
	return isColorBitmap() || isMonochromeCfaBitmapOfType<std::uint16_t>();
}

CFATYPE AvxBitmapUtil::getCfaType() const
{
	if (auto* pGray = this->getGrayPtr<std::uint16_t>())
		return pGray->GetCFAType();
	else
		return CFATYPE_NONE;
}

const int AvxBitmapUtil::width() const {
	return bitmap.Width();
}

template <class T>
bool AvxBitmapUtil::bitmapHasCorrectType() const
{
	return (isColorBitmapOfType<T>() || isMonochromeBitmapOfType<T>());
}

// Explicit template instantiation for the types we need.

// Note that bitmapHasCorrectType() will also instantiate isColorBitmapOfType().
template bool AvxBitmapUtil::bitmapHasCorrectType<std::uint16_t>() const;
template bool AvxBitmapUtil::bitmapHasCorrectType<std::uint32_t>() const;
template bool AvxBitmapUtil::bitmapHasCorrectType<float>() const;
template bool AvxBitmapUtil::bitmapHasCorrectType<double>() const;

template bool AvxBitmapUtil::isMonochromeCfaBitmapOfType<std::uint16_t>() const;
template bool AvxBitmapUtil::isMonochromeCfaBitmapOfType<std::uint32_t>() const;
template bool AvxBitmapUtil::isMonochromeCfaBitmapOfType<float>() const;
template bool AvxBitmapUtil::isMonochromeCfaBitmapOfType<double>() const;
