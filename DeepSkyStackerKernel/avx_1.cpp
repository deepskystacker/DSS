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
#include "avx.h"
#include "avx_bitmap_util.h"
#include "avx_entropy.h"
#include "PixelTransform.h"
#include "TaskInfo.h"
#include "EntropyInfo.h"
#include "BackgroundCalibration.h"
#include "avx_simd_check.h"
#include "dssrect.h"

AvxStacking::AvxStacking(const int lStart, const int lEnd, const CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const DSSRect& resultRect, AvxEntropy& entrdat) :
	lineStart{ lStart }, lineEnd{ lEnd }, colEnd{ inputbm.Width() },
	width{ colEnd }, height{ lineEnd - lineStart },
	resultWidth{ resultRect.width() }, resultHeight{ resultRect.height() },
	vectorsPerLine{ AvxBitmapUtil::numberOfAvxVectors<float, VectorElementType>(width) },
	xCoordinates(width >= 0 && height >= 0 ? vectorsPerLine * height : 0),
	yCoordinates(width >= 0 && height >= 0 ? vectorsPerLine * height : 0),
	redPixels(width >= 0 && height >= 0 ? vectorsPerLine * height : 0),
	greenPixels{},
	bluePixels{},
	inputBitmap{ inputbm },
	tempBitmap{ tempbm },
	avxCfa{ static_cast<size_t>(lStart), static_cast<size_t>(lEnd), inputbm },
	entropyData{ entrdat },
	avxEnabled{ AvxSimdCheck::checkSimdAvailability() }
{
	if (width < 0 || height < 0)
		throw std::invalid_argument("End index smaller than start index for line or column of AvxStacking");

	resizeColorVectors(vectorsPerLine * height);
}

void AvxStacking::init(const int lStart, const int lEnd)
{
	lineStart = lStart;
	lineEnd = lEnd;
	height = lineEnd - lineStart;

	if (avxEnabled)
	{
		const size_t nrVectors = vectorsPerLine * height;
		xCoordinates.resize(nrVectors);
		yCoordinates.resize(nrVectors);
		redPixels.resize(nrVectors);
		resizeColorVectors(nrVectors);
	}
}

int AvxStacking::stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, std::shared_ptr<BackgroundCalibrationInterface> backgroundCalibration, std::shared_ptr<CMemoryBitmap> outputBitmap, const int pixelSizeMultiplier)
{
	static_assert(sizeof(unsigned int) == sizeof(std::uint32_t));

	return SimdSelector<Avx256Stacking, NonAvxStacking>(
		this, [&](auto&& o) { return o.stack(pixelTransformDef, taskInfo, backgroundCalibration, outputBitmap, pixelSizeMultiplier); }
	);
}

// ****************
// Non-AVX Stacking
// ****************

int NonAvxStacking::stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, std::shared_ptr<BackgroundCalibrationInterface> backgroundCalibration, std::shared_ptr<CMemoryBitmap> outputBitmap, const int pixelSizeMultiplier)
{
	const int width = this->stackData.width;
	PIXELDISPATCHVECTOR vPixels;
	vPixels.reserve(16);

	// True color or CFA
	const bool entropyIsColor = !stackData.entropyData.inputBitmap.IsMonochrome() || stackData.entropyData.inputBitmap.IsCFA();

	for (int j = this->stackData.lineStart; j < this->stackData.lineEnd; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			const QPointF ptOut = pixelTransformDef.transform(QPointF(i, j));

			COLORREF16 crColor;
			double fRedEntropy = 1.0, fGreenEntropy = 1.0, fBlueEntropy = 1.0;

			if (taskInfo.m_Method == MBP_ENTROPYAVERAGE)
				this->stackData.entropyData.entropyInfo.GetPixel(i, j, fRedEntropy, fGreenEntropy, fBlueEntropy, crColor);
			else
				this->stackData.inputBitmap.GetPixel16(i, j, crColor);

			const auto [red, green, blue] = backgroundCalibration->calibratePixel(crColor.red, crColor.green, crColor.blue);

			if ((0 != red || 0 != green || 0 != blue) && DSSRect { 0, 0, this->stackData.resultWidth, this->stackData.resultHeight }.contains(ptOut))
			{
				vPixels.resize(0);
				ComputePixelDispatch(ptOut, pixelSizeMultiplier, vPixels);

				for (CPixelDispatch& Pixel : vPixels)
				{
					// For each plane adjust the values
					if (Pixel.m_lX >= 0 && Pixel.m_lX < this->stackData.resultWidth && Pixel.m_lY >= 0 && Pixel.m_lY < this->stackData.resultHeight)
					{
						// Special case for entropy average
						if (taskInfo.m_Method == MBP_ENTROPYAVERAGE)
						{
							if (entropyIsColor)
							{
								double fOldRed, fOldGreen, fOldBlue;

								this->stackData.entropyData.pEntropyCoverage->GetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
								fOldRed += Pixel.m_fPercentage * fRedEntropy;
								fOldGreen += Pixel.m_fPercentage * fGreenEntropy;
								fOldBlue += Pixel.m_fPercentage * fBlueEntropy;
								this->stackData.entropyData.pEntropyCoverage->SetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);

								outputBitmap->GetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
								fOldRed += red * Pixel.m_fPercentage * fRedEntropy;
								fOldGreen += green * Pixel.m_fPercentage * fGreenEntropy;
								fOldBlue += blue * Pixel.m_fPercentage * fBlueEntropy;
								outputBitmap->SetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
							}
							else
							{
								double fOldGray;

								this->stackData.entropyData.pEntropyCoverage->GetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
								fOldGray += Pixel.m_fPercentage * fRedEntropy;
								this->stackData.entropyData.pEntropyCoverage->SetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);

								outputBitmap->GetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
								fOldGray += red * Pixel.m_fPercentage * fRedEntropy;
								outputBitmap->SetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
							}
						}

						double fPreviousRed, fPreviousGreen, fPreviousBlue;

						this->stackData.tempBitmap.GetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
						fPreviousRed += red / 256.0 * Pixel.m_fPercentage;
						fPreviousGreen += green / 256.0 * Pixel.m_fPercentage;
						fPreviousBlue += blue / 256.0 * Pixel.m_fPercentage;
						fPreviousRed = std::min(fPreviousRed, 255.0);
						fPreviousGreen = std::min(fPreviousGreen, 255.0);
						fPreviousBlue = std::min(fPreviousBlue, 255.0);
						this->stackData.tempBitmap.SetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
					}
				}
			}
		}
	}

	return 0;
}
