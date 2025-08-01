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
#include "avx_includes.h"
#include "avx_luminance.h"
#include "avx_cfa.h"
#include "avx_support.h"
#include "avx_bitmap_util.h"

namespace
{
	template <class T>
	std::tuple<__m256d, __m256d, __m256d, __m256d> colorLuminance(const T* const pRed, const T* const pGreen, const T* const pBlue)
	{
		const __m256i red = AvxSupport::read16PackedShort(pRed);
		const __m256i green = AvxSupport::read16PackedShort(pGreen);
		const __m256i blue = AvxSupport::read16PackedShort(pBlue);
		const __m256i minColor = _mm256_min_epu16(_mm256_min_epu16(red, green), blue);
		const __m256i maxColor = _mm256_max_epu16(_mm256_max_epu16(red, green), blue);
		const __m256i minMaxAvg = _mm256_avg_epu16(minColor, maxColor);
		return AvxSupport::wordToPackedDouble(minMaxAvg);
	}

	std::tuple<__m256d, __m256d, __m256d, __m256d> greyLuminance(const auto* const pGray)
	{
		const __m256i gray = AvxSupport::read16PackedShort(pGray);
		return AvxSupport::wordToPackedDouble(gray);
	}
}

int AvxLuminance::computeLuminanceBitmap(const size_t lineStart, const size_t lineEnd)
{
	if (!avxEnabled)
		return 1; // AVX not available.

	int rval = 1;
	if (doComputeLuminance<std::uint16_t>(lineStart, lineEnd) == 0
		|| doComputeLuminance<std::uint32_t>(lineStart, lineEnd) == 0
		|| doComputeLuminance<float>(lineStart, lineEnd) == 0)
	{
		rval = 0;
	}
	return AvxSupport::zeroUpper(rval);
}

template <class T>
int AvxLuminance::doComputeLuminance(const size_t lineStart, const size_t lineEnd)
{
	constexpr double scalingFactor = 1.0 / 256.0;

	// Check input bitmap.
	const AvxBitmapUtil avxInputSupport{ inputBitmap };
	if (!avxInputSupport.isColorBitmapOfType<T>() && !avxInputSupport.isMonochromeBitmapOfType<T>()) // Monochrome includes CFA
		return 1;

	AvxBitmapUtil avxOutputSupport{ outputBitmap };
	const size_t width = inputBitmap.Width();
	constexpr size_t vectorLen = 16;
	const size_t nrVectors = width / vectorLen;

	const auto scaleAndStoreLuminance = [scalingFactor](const __m256d d0, const __m256d d1, const __m256d d2, const __m256d d3, double* const pOut) -> void
	{
		const __m256d vScalingFactor = _mm256_set1_pd(scalingFactor);
		_mm256_storeu_pd(pOut, _mm256_mul_pd(d0, vScalingFactor));
		_mm256_storeu_pd(pOut + 4, _mm256_mul_pd(d1, vScalingFactor));
		_mm256_storeu_pd(pOut + 8, _mm256_mul_pd(d2, vScalingFactor));
		_mm256_storeu_pd(pOut + 12, _mm256_mul_pd(d3, vScalingFactor));
	};

	const auto readColorValue = [](const T* const pColor) -> T
	{
		if constexpr (std::is_integral<T>::value && sizeof(T) == 4) // 32 bit integral type
			return (*pColor) >> 16;
		else
			return *pColor;
	};

	const bool isCFA = avxInputSupport.isMonochromeCfaBitmapOfType<T>();

	if (avxInputSupport.isColorBitmapOfType<T>() || isCFA)
	{
		AvxCfaProcessing avxCfa{ 0, 0, inputBitmap };
		if (isCFA)
		{
			avxCfa.init(lineStart, lineEnd);
			avxCfa.interpolate(lineStart, lineEnd, 1);
		}

		for (size_t row = lineStart, lineNdx = 0; row < lineEnd; ++row, ++lineNdx)
		{
			const T* pRedPixels = isCFA ? avxCfa.redCfaLine<T>(lineNdx) : &avxInputSupport.redPixels<T>().at(row * width);
			const T* pGreenPixels = isCFA ? avxCfa.greenCfaLine<T>(lineNdx) : &avxInputSupport.greenPixels<T>().at(row * width);
			const T* pBluePixels = isCFA ? avxCfa.blueCfaLine<T>(lineNdx) : &avxInputSupport.bluePixels<T>().at(row * width);
			double* pOut = &avxOutputSupport.grayPixels<double>().at(row * width);

			for (size_t counter = 0; counter < nrVectors; ++counter, pRedPixels += vectorLen, pGreenPixels += vectorLen, pBluePixels += vectorLen, pOut += vectorLen)
			{
				const auto [d0, d1, d2, d3] = colorLuminance(pRedPixels, pGreenPixels, pBluePixels);
				scaleAndStoreLuminance(d0, d1, d2, d3, pOut);
			}
			// Remaining pixels of line.
			for (size_t n = nrVectors * vectorLen; n < width; ++n, ++pRedPixels, ++pGreenPixels, ++pBluePixels, ++pOut)
			{
				const T red = readColorValue(pRedPixels);
				const T green = readColorValue(pGreenPixels);
				const T blue = readColorValue(pBluePixels);
				const T minColor = std::min(std::min(red, green), blue);
				const T maxColor = std::max(std::max(red, green), blue);
				*pOut = (static_cast<double>(minColor) + static_cast<double>(maxColor)) * (0.5 * scalingFactor);
			}
		}
		return 0;
	}

	if (avxInputSupport.isMonochromeBitmapOfType<T>())
	{
		for (size_t row = lineStart; row < lineEnd; ++row)
		{
			const T* pGreyPixels = &avxInputSupport.grayPixels<T>().at(row * width);
			double* pOut = &avxOutputSupport.grayPixels<double>().at(row * width);

			for (size_t counter = 0; counter < nrVectors; ++counter, pGreyPixels += vectorLen, pOut += vectorLen)
			{
				const auto [d0, d1, d2, d3] = greyLuminance(pGreyPixels);
				scaleAndStoreLuminance(d0, d1, d2, d3, pOut);
			}
			// Remaining pixels of line.
			for (size_t n = nrVectors * vectorLen; n < width; ++n, ++pGreyPixels, ++pOut)
			{
				const T grey = readColorValue(pGreyPixels);
				*pOut = static_cast<double>(grey) * scalingFactor;
			}
		}
		return 0;
	}

	return 1;
}
