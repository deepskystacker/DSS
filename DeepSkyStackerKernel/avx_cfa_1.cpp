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
#include "avx_cfa.h"
#include "avx_bitmap_util.h"
#include "avx_simd_check.h"

AvxCfaProcessing::AvxCfaProcessing(const size_t lineStart, const size_t lineEnd, const CMemoryBitmap& inputbm) :
	redPixels{},
	greenPixels{},
	bluePixels{},
	inputBitmap{ inputbm },
	vectorsPerLine{ 0 },
	avxEnabled{ AvxSimdCheck::checkSimdAvailability()}
{
	init(lineStart, lineEnd);
}

void AvxCfaProcessing::init(const size_t lineStart, const size_t lineEnd) // You should be sure that lineEnd >= lineStart!
{
	const size_t height = lineEnd - lineStart;
	vectorsPerLine = AvxBitmapUtil::numberOfAvxVectors<std::uint16_t, VectorElementType>(inputBitmap.Width());
	const size_t nrVectors = vectorsPerLine * height;
	if (nrVectors != 0 && AvxBitmapUtil{ inputBitmap }.isMonochromeCfaBitmapOfType<std::uint16_t>())
	{
		redPixels.resize(nrVectors);
		greenPixels.resize(nrVectors);
		bluePixels.resize(nrVectors);
	}
}

int AvxCfaProcessing::interpolate(const size_t lineStart, const size_t lineEnd, const int pixelSizeMultiplier)
{
	if (!(this->inputBitmap.IsMonochrome() && this->inputBitmap.IsCFA()))
		return 1;
	return SimdSelector<Avx256CfaProcessing, NonAvxCfaProcessing>(this, [&](auto&& o) { return o.interpolate(lineStart, lineEnd, pixelSizeMultiplier); });
}

// ----------------------
// Non-SIMD interpolation
// ----------------------

int NonAvxCfaProcessing::interpolate(const size_t lineStart, const size_t lineEnd, const int)
{
	const auto width = static_cast<size_t>(this->cfaData.inputBitmap.Width());
	for (size_t row = 0; row < (lineEnd - lineStart); ++row)
	{
		std::uint16_t* pRed = cfaData.redCfaLine(row);
		std::uint16_t* pGreen = cfaData.greenCfaLine(row);
		std::uint16_t* pBlue = cfaData.blueCfaLine(row);
		for (size_t col = 0; col < width; ++col, ++pRed, ++pGreen, ++pBlue)
		{
			COLORREF16 rgb16;
			this->cfaData.inputBitmap.GetPixel16(col, row + lineStart, rgb16);
			*pRed = rgb16.red;
			*pGreen = rgb16.green;
			*pBlue = rgb16.blue;
		}
	}
	return 0;
}
