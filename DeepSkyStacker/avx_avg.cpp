#include "StdAfx.h"
#include "avx_avg.h"
#include "avx.h"

#if defined(AVX_INTRINSICS) && defined(_M_X64)

#include <immintrin.h>

AvxAccumulation::AvxAccumulation(const CRect& resultRect, const CTaskInfo& tInfo, CMemoryBitmap& tempbm, CMemoryBitmap& outbm) noexcept :
	resultWidth{ resultRect.Width() }, resultHeight{ resultRect.Height() },
	tempBitmap{ tempbm },
	outputBitmap{ outbm },
	taskInfo{ tInfo }
{}

// *********************************************************************************************
// Sept. 2020: Only works for output bitmaps of type float (which is currently always the case).
// There is a static type check below.
// *********************************************************************************************

int AvxAccumulation::accumulate(const int nrStackedBitmaps)
{
	if (!AvxSupport::checkSimdAvailability())
		return 1;

	if (doAccumulate<WORD, float>(nrStackedBitmaps) == 0)
		return 0;
	if (doAccumulate<std::uint32_t, float>(nrStackedBitmaps) == 0)
		return 0;
	if (doAccumulate<float, float>(nrStackedBitmaps) == 0)
		return 0;
	return 1;
}

template <class T_IN, class T_OUT>
int AvxAccumulation::doAccumulate(const int nrStackedBitmaps)
{
	// Output bitmap is always float
	if constexpr (!std::is_same<T_OUT, float>::value)
		return 1;

	const AvxSupport avxTempBitmap{ tempBitmap };

	if (!avxTempBitmap.bitmapHasCorrectType<T_IN>())
		return 1;
	if (!AvxSupport{outputBitmap}.bitmapHasCorrectType<T_OUT>())
		return 1;

	const int nrVectors = resultWidth / 16;

	if (taskInfo.m_Method == MBP_FASTAVERAGE)
	{
		const __m256 nrStacked = _mm256_set1_ps(static_cast<float>(nrStackedBitmaps));
		const __m256 nrStacked1 = _mm256_set1_ps(static_cast<float>(nrStackedBitmaps + 1));

		const auto accumulate = [&nrStacked, &nrStacked1](const T_IN* pIn, T_OUT* pOut) -> void
		{
			const auto [newColorLo8, newColorHi8] = AvxSupport::read16PackedSingle(pIn);
			const auto [oldColorLo8, oldColorHi8] = AvxSupport::read16PackedSingle(pOut);
			// If T_OUT != float, then we need to change these lines (below too).
			_mm256_storeu_ps(pOut,     _mm256_div_ps(_mm256_fmadd_ps(oldColorLo8, nrStacked, newColorLo8), nrStacked1)); // (oldColor * nrStacked + newColor) / (nrStacked + 1)
			_mm256_storeu_ps(pOut + 8, _mm256_div_ps(_mm256_fmadd_ps(oldColorHi8, nrStacked, newColorHi8), nrStacked1));
		};

		if (avxTempBitmap.isColorBitmap())
		{
			const T_IN *pRed{ &*avxTempBitmap.redPixels<T_IN>().begin() }, *pGreen{ &*avxTempBitmap.greenPixels<T_IN>().begin() }, *pBlue{ &*avxTempBitmap.bluePixels<T_IN>().begin() };
			auto *const pOutput = dynamic_cast<CColorBitmapT<T_OUT>*>(&outputBitmap);
			if (pOutput == nullptr)
				return 1;
			T_OUT *pOutRed{ &*pOutput->m_Red.m_vPixels.begin() }, *pOutGreen{ &*pOutput->m_Green.m_vPixels.begin() }, *pOutBlue{ &*pOutput->m_Blue.m_vPixels.begin() };

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
		if (avxTempBitmap.isMonochromeBitmap())
		{
			const T_IN* pGray{ &*avxTempBitmap.grayPixels<T_IN>().begin() };
			auto *const pOutput = dynamic_cast<CGrayBitmapT<T_OUT>*>(&outputBitmap);
			if (pOutput == nullptr)
				return 1;
			T_OUT* pOut{ &*pOutput->m_vPixels.begin() };

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
		const auto maximum = [](const T_IN* pIn, T_OUT* pOut) -> void
		{
			const auto [newColorLo8, newColorHi8] = AvxSupport::read16PackedSingle(pIn);
			const auto [oldColorLo8, oldColorHi8] = AvxSupport::read16PackedSingle(pOut);
			_mm256_storeu_ps(pOut,     _mm256_max_ps(oldColorLo8, newColorLo8));
			_mm256_storeu_ps(pOut + 8, _mm256_max_ps(oldColorHi8, newColorHi8));
		};

		if (avxTempBitmap.isColorBitmap())
		{
			const T_IN *pRed{ &*avxTempBitmap.redPixels<T_IN>().begin() }, *pGreen{ &*avxTempBitmap.greenPixels<T_IN>().begin() }, *pBlue{ &*avxTempBitmap.bluePixels<T_IN>().begin() };
			auto* const pOutput = dynamic_cast<CColorBitmapT<T_OUT>*>(&outputBitmap);
			if (pOutput == nullptr)
				return 1;
			T_OUT *pOutRed{ &*pOutput->m_Red.m_vPixels.begin() }, *pOutGreen{ &*pOutput->m_Green.m_vPixels.begin() }, *pOutBlue{ &*pOutput->m_Blue.m_vPixels.begin() };

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
		if (avxTempBitmap.isMonochromeBitmap())
		{
			const T_IN* pGray{ &*avxTempBitmap.grayPixels<T_IN>().begin() };
			auto *const pOutput = dynamic_cast<CGrayBitmapT<T_OUT>*>(&outputBitmap);
			if (pOutput == nullptr)
				return 1;
			T_OUT* pOut{ &*pOutput->m_vPixels.begin() };

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
