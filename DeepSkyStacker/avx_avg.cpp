#include "StdAfx.h"
#include "avx_avg.h"
#include "avx.h"

#if defined(AVX_INTRINSICS) && defined(_M_X64)

#include <immintrin.h>

AvxAccumulation::AvxAccumulation(const CRect& resultRect, const CTaskInfo& tInfo, C48BitColorBitmap* pTemp, C96BitFloatColorBitmap* pOut) :
	resultWidth{ resultRect.Width() }, resultHeight{ resultRect.Height() },
	pTempBitmap{ pTemp }, pOutput{ pOut },
	taskInfo{ tInfo }
{
}

template <typename T>
bool AvxAccumulation::checkBitmap(CColorBitmapT<T> *const pBitmap, const int nrBits) {
	if (pBitmap == nullptr || pBitmap->BitPerSample() != nrBits || !pBitmap->isTopDown())
		return false;
	CBitmapCharacteristics bitmapCharacteristic;
	pBitmap->GetCharacteristics(bitmapCharacteristic);
	if (bitmapCharacteristic.m_lBitsPerPixel != nrBits || bitmapCharacteristic.m_lNrChannels != 3)
		return false;
	return true;
}

int AvxAccumulation::accumulate(const int nrStackedBitmaps)
{
	if (!AvxStacking::checkCpuFeatures())
		return 1;
	if (!checkBitmap(pTempBitmap, 16))
		return 1;
	if (!checkBitmap(pOutput, 32))
		return 1;

	const int nrVectors = resultWidth / 16;

	if (taskInfo.m_Method == MBP_FASTAVERAGE)
	{
		const WORD* pRed{ &pTempBitmap->m_Red.m_vPixels[0] }, * pGreen{ &pTempBitmap->m_Green.m_vPixels[0] }, * pBlue{ &pTempBitmap->m_Blue.m_vPixels[0] };
		float* pOutRed{ &pOutput->m_Red.m_vPixels[0] }, * pOutGreen{ &pOutput->m_Green.m_vPixels[0] }, * pOutBlue{ &pOutput->m_Blue.m_vPixels[0] };
		const __m256 nrStacked = _mm256_set1_ps(static_cast<float>(nrStackedBitmaps));
		const __m256 nrStacked1 = _mm256_set1_ps(static_cast<float>(nrStackedBitmaps + 1));

		const auto accumulate = [nrStacked, nrStacked1](const WORD* pIn, float* pOut) -> void
		{
			const __m256i color = _mm256_loadu_si256((const __m256i*)pIn); // 16 unsigned short with color

			__m256 oldColor = _mm256_loadu_ps(pOut); // first 8 float with old color
			_mm256_storeu_ps(pOut, _mm256_div_ps(_mm256_add_ps(_mm256_mul_ps(oldColor, nrStacked), AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(color, 0))), nrStacked1));

			oldColor = _mm256_loadu_ps(pOut + 8); // next 8 float with old color
			_mm256_storeu_ps(pOut + 8, _mm256_div_ps(_mm256_add_ps(_mm256_mul_ps(oldColor, nrStacked), AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(color, 1))), nrStacked1));
		};

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
	else if (taskInfo.m_Method == MBP_MAXIMUM)
	{
		const WORD* pRed{ &pTempBitmap->m_Red.m_vPixels[0] }, * pGreen{ &pTempBitmap->m_Green.m_vPixels[0] }, * pBlue{ &pTempBitmap->m_Blue.m_vPixels[0] };
		float* pOutRed{ &pOutput->m_Red.m_vPixels[0] }, * pOutGreen{ &pOutput->m_Green.m_vPixels[0] }, * pOutBlue{ &pOutput->m_Blue.m_vPixels[0] };

		const auto maximum = [](const WORD* pIn, float* pOut) -> void
		{
			const __m256i color = _mm256_loadu_si256((const __m256i*)pIn); // 16 unsigned short with color

			__m256 oldColor = _mm256_loadu_ps(pOut); // first 8 float with old color
			_mm256_storeu_ps(pOut, _mm256_max_ps(oldColor, AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(color, 0))));

			oldColor = _mm256_loadu_ps(pOut + 8); // next 8 float with old color
			_mm256_storeu_ps(pOut + 8, _mm256_max_ps(oldColor, AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(color, 1))));
		};

		for (int row = 0; row < resultHeight; ++row)
		{
			for (int counter = 0; counter < nrVectors; ++counter, pRed += 16, pGreen += 16, pBlue += 16, pOutRed += 16, pOutGreen += 16, pOutBlue += 16) {
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

	return 1;
}
#endif
