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
	entropyData{ entrdat }
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

	if (AvxSimdCheck::checkSimdAvailability())
	{
		const size_t nrVectors = vectorsPerLine * height;
		xCoordinates.resize(nrVectors);
		yCoordinates.resize(nrVectors);
		redPixels.resize(nrVectors);
		resizeColorVectors(nrVectors);
	}
}

int AvxStacking::stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, std::shared_ptr<CMemoryBitmap> outputBitmap, const int pixelSizeMultiplier)
{
	static_assert(sizeof(unsigned int) == sizeof(std::uint32_t));

	if (AvxSimdCheck::checkSimdAvailability())
	{
		Avx256Stacking avxStacking{ *this };
		return avxStacking.stack(pixelTransformDef, taskInfo, backgroundCalibrationDef, outputBitmap, pixelSizeMultiplier);
	}
	else
	{
		NonAvxStacking nonAvxStacking{ *this };
		return nonAvxStacking.stack(pixelTransformDef, taskInfo, backgroundCalibrationDef, outputBitmap, pixelSizeMultiplier);
	}
}

// ****************
// Non-AVX Stacking
// ****************

int NonAvxStacking::stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, std::shared_ptr<CMemoryBitmap> outputBitmap, const int pixelSizeMultiplier)
{
	const int width = this->stackData.width;
	PIXELDISPATCHVECTOR vPixels;
	vPixels.reserve(16);

	const bool isColor = AvxBitmapUtil{ this->stackData.entropyData.inputBitmap }.isColorBitmapOrCfa();

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

			float Red = crColor.red;
			float Green = crColor.green;
			float Blue = crColor.blue;

			if (backgroundCalibrationDef.m_BackgroundCalibrationMode != BCM_NONE)
				backgroundCalibrationDef.ApplyCalibration(Red, Green, Blue);

			if ((0 != Red || 0 != Green || 0 != Blue) && DSSRect { 0, 0, this->stackData.resultWidth, this->stackData.resultHeight }.contains(ptOut))
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
							if (isColor)
							{
								double fOldRed, fOldGreen, fOldBlue;

								this->stackData.entropyData.pEntropyCoverage->GetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
								fOldRed += Pixel.m_fPercentage * fRedEntropy;
								fOldGreen += Pixel.m_fPercentage * fGreenEntropy;
								fOldBlue += Pixel.m_fPercentage * fBlueEntropy;
								this->stackData.entropyData.pEntropyCoverage->SetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);

								outputBitmap->GetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
								fOldRed += Red * Pixel.m_fPercentage * fRedEntropy;
								fOldGreen += Green * Pixel.m_fPercentage * fGreenEntropy;
								fOldBlue += Blue * Pixel.m_fPercentage * fBlueEntropy;
								outputBitmap->SetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
							}
							else
							{
								double fOldGray;

								this->stackData.entropyData.pEntropyCoverage->GetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
								fOldGray += Pixel.m_fPercentage * fRedEntropy;
								this->stackData.entropyData.pEntropyCoverage->SetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);

								outputBitmap->GetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
								fOldGray += Red * Pixel.m_fPercentage * fRedEntropy;
								outputBitmap->SetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
							}
						}

						double fPreviousRed, fPreviousGreen, fPreviousBlue;

						this->stackData.tempBitmap.GetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
						fPreviousRed += static_cast<double>(Red) / 256.0 * Pixel.m_fPercentage;
						fPreviousGreen += static_cast<double>(Green) / 256.0 * Pixel.m_fPercentage;
						fPreviousBlue += static_cast<double>(Blue) / 256.0 * Pixel.m_fPercentage;
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
