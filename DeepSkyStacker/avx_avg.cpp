#include "StdAfx.h"
#include "avx_avg.h"
#include "avx.h"

#if defined(AVX_INTRINSICS) && defined(_M_X64)

#include <immintrin.h>

AvxAccumulation::AvxAccumulation(const CRect& resultRect, const CTaskInfo& tInfo, CMemoryBitmap& tempbm, CMemoryBitmap& outbm) :
	resultWidth{ resultRect.Width() }, resultHeight{ resultRect.Height() },
	tempBitmap{ tempbm },
	outputBitmap{ outbm },
	taskInfo{ tInfo }
{}

int AvxAccumulation::accumulate(const int nrStackedBitmaps)
{
	const AvxSupport avxSupport{ tempBitmap };

	if (!AvxStacking::checkCpuFeatures())
		return 1;
	if (!avxSupport.isBitmapOfType<WORD>())
		return 1;
	if (!AvxSupport{outputBitmap}.isBitmapOfType<float>())
		return 1;

	const int nrVectors = resultWidth / 16;

	if (taskInfo.m_Method == MBP_FASTAVERAGE)
	{
		const __m256 nrStacked = _mm256_set1_ps(static_cast<float>(nrStackedBitmaps));
		const __m256 nrStacked1 = _mm256_set1_ps(static_cast<float>(nrStackedBitmaps + 1));

		const auto accumulate = [&nrStacked, &nrStacked1](const WORD* pIn, float* pOut) -> void
		{
			const __m256i color = _mm256_loadu_si256((const __m256i*)pIn); // 16 unsigned short with color

			__m256 oldColor = _mm256_loadu_ps(pOut); // first 8 float with old color
			__m256 newColor = AvxSupport::wordToPackedFloat(_mm256_extracti128_si256(color, 0));
			_mm256_storeu_ps(pOut, _mm256_div_ps(_mm256_fmadd_ps(oldColor, nrStacked, newColor), nrStacked1)); // (oldColor * nrStacked + newColor) / (nrStacked + 1)

			oldColor = _mm256_loadu_ps(pOut + 8); // next 8 float with old color
			newColor = AvxSupport::wordToPackedFloat(_mm256_extracti128_si256(color, 1));
			_mm256_storeu_ps(pOut + 8, _mm256_div_ps(_mm256_fmadd_ps(oldColor, nrStacked, newColor), nrStacked1));
		};

		if (avxSupport.isColorBitmap())
		{
			const WORD *pRed{ &*avxSupport.redPixels().begin() }, *pGreen{ &*avxSupport.greenPixels().begin() }, *pBlue{ &*avxSupport.bluePixels().begin() };
			auto *const pOutput = dynamic_cast<C96BitFloatColorBitmap*>(&outputBitmap);
			if (pOutput == nullptr)
				return 1;
			float *pOutRed{ &*pOutput->m_Red.m_vPixels.begin() }, *pOutGreen{ &*pOutput->m_Green.m_vPixels.begin() }, *pOutBlue{ &*pOutput->m_Blue.m_vPixels.begin() };

			for (int row = 0; row < resultHeight; ++row)
			{
				for (int counter = 0; counter < nrVectors; ++counter, pRed += 16, pGreen += 16, pBlue += 16, pOutRed += 16, pOutGreen += 16, pOutBlue += 16)
				{
					accumulate(pRed, pOutRed);
					accumulate(pGreen, pOutGreen);
					accumulate(pBlue, pOutBlue);
				}
				// Rest of line
				for (int n = nrVectors * 16; n < resultWidth; ++n, ++pRed, ++pGreen, ++pBlue, ++pOutRed, ++pOutGreen, ++pOutBlue)
				{
					*pOutRed = (*pOutRed * static_cast<float>(nrStackedBitmaps) + static_cast<float>(*pRed)) / static_cast<float>(nrStackedBitmaps + 1);
					*pOutGreen = (*pOutGreen * static_cast<float>(nrStackedBitmaps) + static_cast<float>(*pGreen)) / static_cast<float>(nrStackedBitmaps + 1);
					*pOutBlue = (*pOutBlue * static_cast<float>(nrStackedBitmaps) + static_cast<float>(*pBlue)) / static_cast<float>(nrStackedBitmaps + 1);
				}
			}
			return 0;
		}
		if (avxSupport.isMonochromeBitmap())
		{
			const WORD* pGray{ &*avxSupport.grayPixels().begin() };
			auto* const pOutput = dynamic_cast<CGrayBitmapT<float>*>(&outputBitmap);
			if (pOutput == nullptr)
				return 1;
			float* pOut{ &*pOutput->m_vPixels.begin() };

			for (int row = 0; row < resultHeight; ++row)
			{
				for (int counter = 0; counter < nrVectors; ++counter, pGray += 16, pOut += 16)
					accumulate(pGray, pOut);
				// Rest of line
				for (int n = nrVectors * 16; n < resultWidth; ++n, ++pGray, ++pOut)
					*pOut = (*pOut * static_cast<float>(nrStackedBitmaps) + static_cast<float>(*pGray)) / static_cast<float>(nrStackedBitmaps + 1);
			}
			return 0;
		}
		return 1;
	}
	else if (taskInfo.m_Method == MBP_MAXIMUM)
	{
		const auto maximum = [](const WORD* pIn, float* pOut) -> void
		{
			const __m256i color = _mm256_loadu_si256((const __m256i*)pIn); // 16 unsigned short with color

			__m256 oldColor = _mm256_loadu_ps(pOut); // first 8 float with old color
			_mm256_storeu_ps(pOut, _mm256_max_ps(oldColor, AvxSupport::wordToPackedFloat(_mm256_extracti128_si256(color, 0))));

			oldColor = _mm256_loadu_ps(pOut + 8); // next 8 float with old color
			_mm256_storeu_ps(pOut + 8, _mm256_max_ps(oldColor, AvxSupport::wordToPackedFloat(_mm256_extracti128_si256(color, 1))));
		};

		if (avxSupport.isColorBitmap())
		{
			const WORD *pRed{ &*avxSupport.redPixels().begin() }, *pGreen{ &*avxSupport.greenPixels().begin() }, *pBlue{ &*avxSupport.bluePixels().begin() };
			auto *const pOutput = dynamic_cast<C96BitFloatColorBitmap*>(&outputBitmap);
			if (pOutput == nullptr)
				return 1;
			float *pOutRed{ &*pOutput->m_Red.m_vPixels.begin() }, *pOutGreen{ &*pOutput->m_Green.m_vPixels.begin() }, *pOutBlue{ &*pOutput->m_Blue.m_vPixels.begin() };

			for (int row = 0; row < resultHeight; ++row)
			{
				for (int counter = 0; counter < nrVectors; ++counter, pRed += 16, pGreen += 16, pBlue += 16, pOutRed += 16, pOutGreen += 16, pOutBlue += 16)
				{
					maximum(pRed, pOutRed);
					maximum(pGreen, pOutGreen);
					maximum(pBlue, pOutBlue);
				}
				// Rest of line
				for (int n = nrVectors * 16; n < resultWidth; ++n, ++pRed, ++pGreen, ++pBlue, ++pOutRed, ++pOutGreen, ++pOutBlue)
				{
					*pOutRed = std::max(*pOutRed, static_cast<float>(*pRed));
					*pOutGreen = std::max(*pOutGreen, static_cast<float>(*pGreen));
					*pOutBlue = std::max(*pOutBlue, static_cast<float>(*pBlue));
				}
			}
			return 0;
		}
		if (avxSupport.isMonochromeBitmap())
		{
			const WORD* pGray{ &*avxSupport.grayPixels().begin() };
			auto *const pOutput = dynamic_cast<CGrayBitmapT<float>*>(&outputBitmap);
			if (pOutput == nullptr)
				return 1;
			float* pOut{ &*pOutput->m_vPixels.begin() };

			for (int row = 0; row < resultHeight; ++row)
			{
				for (int counter = 0; counter < nrVectors; ++counter, pGray += 16, pOut += 16)
					maximum(pGray, pOut);
				// Rest of line
				for (int n = nrVectors * 16; n < resultWidth; ++n, ++pGray, ++pOut)
					*pOut = std::max(*pOut, static_cast<float>(*pGray));
			}
			return 0;
		}
		return 1;
	}

	return 1;
}
#endif
