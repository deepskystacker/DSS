#include "StdAfx.h"
#include "avx_histogram.h"
#include "avx.h"
#include "avx_cfa.h"

#if defined(AVX_INTRINSICS) && defined(_M_X64)

#include <immintrin.h>

AvxHistogram::AvxHistogram(CMemoryBitmap& inputbm) :
	redHisto(HistogramSize(), 0),
	greenHisto{},
	blueHisto{},
	inputBitmap{ inputbm },
	avxReady{ AvxSupport::checkSimdAvailability() },
	allRunsSuccessful{ true }
{
	if (avxReady && AvxSupport{ inputBitmap }.isColorBitmapOrCfa())
	{
		greenHisto.resize(HistogramSize(), 0);
		blueHisto.resize(HistogramSize(), 0);
	}

	static_assert(sizeof(HistogramVectorType::value_type) == sizeof(int));
}

int AvxHistogram::calcHistogram(const size_t lineStart, const size_t lineEnd)
{
	const auto rval = [this](const int rv) -> int
	{
		if (rv != 0)
			this->allRunsSuccessful = false;
		return rv;
	};

	if (!avxReady)
		return rval(1);

	if (doCalcHistogram<WORD>(lineStart, lineEnd) == 0)
		return rval(0);
	if (doCalcHistogram<std::uint32_t>(lineStart, lineEnd) == 0)
		return rval(0);
	if (doCalcHistogram<float>(lineStart, lineEnd) == 0)
		return rval(0);

	return rval(1);
}

template <class T>
int AvxHistogram::doCalcHistogram(const size_t lineStart, const size_t lineEnd)
{
	// Check input bitmap.
	const AvxSupport avxInputSupport{ inputBitmap };
	if (!avxInputSupport.isColorBitmapOfType<T>() && !avxInputSupport.isMonochromeBitmapOfType<T>()) // Monochrome includes CFA
		return 1;

	constexpr size_t vectorLen = 8;
	const size_t width = inputBitmap.Width();
	const size_t nrVectors = width / vectorLen;

	// AVX makes no sense for super-small arrays.
	if (width < 256 || inputBitmap.Height() < 32)
		return 1;

	const auto calcHistoOfVectorEpi32 = [](const __m256i colorVec, auto& histogram) -> void
	{
		const auto [nrEqualColors, bitMask] = detectConflictsEpi32(colorVec);

		const __m256i sourceHisto = _mm256_i32gather_epi32((const int*)&*histogram.begin(), colorVec, 4);
		const __m256i updatedHisto = _mm256_add_epi32(sourceHisto, nrEqualColors);

		if ((bitMask & 1) == 0) // No conflict
			histogram[_mm256_cvtsi256_si32(colorVec)] = _mm256_cvtsi256_si32(updatedHisto);
		if ((bitMask & (1 << 4)) == 0)
			histogram[_mm256_extract_epi32(colorVec, 1)] = _mm256_extract_epi32(updatedHisto, 1);
		if ((bitMask & (1 << 8)) == 0)
			histogram[_mm256_extract_epi32(colorVec, 2)] = _mm256_extract_epi32(updatedHisto, 2);
		if ((bitMask & (1 << 12)) == 0)
			histogram[_mm256_extract_epi32(colorVec, 3)] = _mm256_extract_epi32(updatedHisto, 3);
		const __m128i colorHiLane = _mm256_extracti128_si256(colorVec, 1);
		const __m128i histoHiLane = _mm256_extracti128_si256(updatedHisto, 1);
		if ((bitMask & (1 << 16)) == 0)
			histogram[_mm_cvtsi128_si32(colorHiLane)] = _mm_cvtsi128_si32(histoHiLane);
		if ((bitMask & (1 << 20)) == 0)
			histogram[_mm_extract_epi32(colorHiLane, 1)] = _mm_extract_epi32(histoHiLane, 1);
		if ((bitMask & (1 << 24)) == 0)
			histogram[_mm_extract_epi32(colorHiLane, 2)] = _mm_extract_epi32(histoHiLane, 2);
		if ((bitMask & (1 << 28)) == 0)
			histogram[_mm_extract_epi32(colorHiLane, 3)] = _mm_extract_epi32(histoHiLane, 3);
	};
/*
	const auto calcHistoOfVectorEpi16 = [](const T* const pColor, auto& histogram) -> void
	{
		const __m256i colorVec = AvxSupport::read16PackedShort(pColor);
		const auto [nrEqualColors, bitMask] = detectConflictsEpi16(colorVec);

		const __m256i loHisto = _mm256_i32gather_epi32((const int*)&*histogram.begin(), _mm256_cvtepu16_epi32(_mm256_castsi256_si128(colorVec)), 4);
		const __m256i hiHisto = _mm256_i32gather_epi32((const int*)&*histogram.begin(), _mm256_cvtepu16_epi32(_mm256_extracti128_si256(colorVec, 1)), 4);
		const __m256i loUpdated = _mm256_add_epi32(loHisto, _mm256_cvtepu16_epi32(_mm256_castsi256_si128(nrEqualColors)));
		const __m256i hiUpdated = _mm256_add_epi32(hiHisto, _mm256_cvtepu16_epi32(_mm256_extracti128_si256(nrEqualColors, 1)));

		if ((bitMask & 1) == 0) // no conflict
			histogram[_mm256_extract_epi16(colorVec, 0)] = _mm256_cvtsi256_si32(loUpdated);
		if ((bitMask & (1 << 2)) == 0)
			histogram[_mm256_extract_epi16(colorVec, 1)] = _mm256_extract_epi32(loUpdated, 1);
		if ((bitMask & (1 << 4)) == 0)
			histogram[_mm256_extract_epi16(colorVec, 2)] = _mm256_extract_epi32(loUpdated, 2);
		if ((bitMask & (1 << 6)) == 0)
			histogram[_mm256_extract_epi16(colorVec, 3)] = _mm256_extract_epi32(loUpdated, 3);
		__m128i histoHiLane = _mm256_extracti128_si256(loUpdated, 1);
		if ((bitMask & (1 << 8)) == 0)
			histogram[_mm256_extract_epi16(colorVec, 4)] = _mm_cvtsi128_si32(histoHiLane);
		if ((bitMask & (1 << 10)) == 0)
			histogram[_mm256_extract_epi16(colorVec, 5)] = _mm_extract_epi32(histoHiLane, 1);
		if ((bitMask & (1 << 12)) == 0)
			histogram[_mm256_extract_epi16(colorVec, 6)] = _mm_extract_epi32(histoHiLane, 2);
		if ((bitMask & (1 << 14)) == 0)
			histogram[_mm256_extract_epi16(colorVec, 7)] = _mm_extract_epi32(histoHiLane, 3);
		const __m128i colorHiLane = _mm256_extracti128_si256(colorVec, 1);
		if ((bitMask & (1 << 16)) == 0)
			histogram[_mm_extract_epi16(colorHiLane, 0)] = _mm256_cvtsi256_si32(hiUpdated);
		if ((bitMask & (1 << 18)) == 0)
			histogram[_mm_extract_epi16(colorHiLane, 1)] = _mm256_extract_epi32(hiUpdated, 1);
		if ((bitMask & (1 << 20)) == 0)
			histogram[_mm_extract_epi16(colorHiLane, 2)] = _mm256_extract_epi32(hiUpdated, 2);
		if ((bitMask & (1 << 22)) == 0)
			histogram[_mm_extract_epi16(colorHiLane, 3)] = _mm256_extract_epi32(hiUpdated, 3);
		histoHiLane = _mm256_extracti128_si256(hiUpdated, 1);
		if ((bitMask & (1 << 24)) == 0)
			histogram[_mm_extract_epi16(colorHiLane, 4)] = _mm_cvtsi128_si32(histoHiLane);
		if ((bitMask & (1 << 26)) == 0)
			histogram[_mm_extract_epi16(colorHiLane, 5)] = _mm_extract_epi32(histoHiLane, 1);
		if ((bitMask & (1 << 28)) == 0)
			histogram[_mm_extract_epi16(colorHiLane, 6)] = _mm_extract_epi32(histoHiLane, 2);
		if ((bitMask & (1 << 30)) == 0)
			histogram[_mm_extract_epi16(colorHiLane, 7)] = _mm_extract_epi32(histoHiLane, 3);
	};
*/
	const bool isCFA = avxInputSupport.isMonochromeCfaBitmapOfType<T>();

	if (avxInputSupport.isColorBitmapOfType<T>() || isCFA)
	{
		AvxCfaProcessing avxCfa{ 0, 0, inputBitmap };
		if (isCFA)
		{
			avxCfa.init(lineStart, lineEnd);
			avxCfa.interpolate(lineStart, lineEnd, 1);
		}

		for (size_t row = 0, lineNdx = lineStart; lineNdx < lineEnd; ++row, ++lineNdx)
		{
			const T* pRedPixels = isCFA ? avxCfa.redCfaPixels<T>(row * width) : &avxInputSupport.redPixels<T>().at(lineNdx * width);
			const T* pGreenPixels = isCFA ? avxCfa.greenCfaPixels<T>(row * width) : &avxInputSupport.greenPixels<T>().at(lineNdx * width);
			const T* pBluePixels = isCFA ? avxCfa.blueCfaPixels<T>(row * width) : &avxInputSupport.bluePixels<T>().at(lineNdx * width);

			for (size_t counter = 0; counter < nrVectors; ++counter, pRedPixels += vectorLen, pGreenPixels += vectorLen, pBluePixels += vectorLen)
			{
				calcHistoOfVectorEpi32(AvxSupport::read8PackedInt(pRedPixels), redHisto);
				calcHistoOfVectorEpi32(AvxSupport::read8PackedInt(pGreenPixels), greenHisto);
				calcHistoOfVectorEpi32(AvxSupport::read8PackedInt(pBluePixels), blueHisto);
			}
			for (size_t n = nrVectors * vectorLen; n < width; ++n, ++pRedPixels, ++pGreenPixels, ++pBluePixels)
			{
				++redHisto[static_cast<size_t>(*pRedPixels)];
				++greenHisto[static_cast<size_t>(*pGreenPixels)];
				++blueHisto[static_cast<size_t>(*pBluePixels)];
			}
		}
		return 0;
	}

	if (avxInputSupport.isMonochromeBitmapOfType<T>())
	{
		for (size_t lineNdx = lineStart; lineNdx < lineEnd; ++lineNdx)
		{
			const T* pGrayPixels = &avxInputSupport.grayPixels<T>().at(lineNdx * width);
			for (size_t counter = 0; counter < nrVectors; ++counter, pGrayPixels += vectorLen)
			{
				calcHistoOfVectorEpi32(AvxSupport::read8PackedInt(pGrayPixels), redHisto);
			}
			for (size_t n = nrVectors * vectorLen; n < width; ++n, ++pGrayPixels)
			{
				++redHisto[static_cast<size_t>(*pGrayPixels)];
			}
		}
		return 0;
	}

	return 1;
}

inline std::tuple<__m256i, std::uint32_t> AvxHistogram::detectConflictsEpi32(const __m256i a) noexcept
{
	__m256i counter = _mm256_set1_epi32(1);
	std::uint32_t bitMask = 0;

	__m256i shifted = AvxSupport::shiftRightEpi32<1>(a);
	__m256i c = _mm256_blend_epi32(_mm256_cmpeq_epi32(a, shifted), _mm256_setzero_si256(), 0x80);
	counter = _mm256_sub_epi32(counter, c);
	bitMask |= (_mm256_movemask_epi8(c) << 4);

	shifted = AvxSupport::shiftRightEpi32<1>(shifted);
	c = _mm256_blend_epi32(_mm256_cmpeq_epi32(a, shifted), _mm256_setzero_si256(), 0xc0);
	counter = _mm256_sub_epi32(counter, c);
	bitMask |= (_mm256_movemask_epi8(c) << 8);

	shifted = AvxSupport::shiftRightEpi32<1>(shifted);
	c = _mm256_blend_epi32(_mm256_cmpeq_epi32(a, shifted), _mm256_setzero_si256(), 0xe0);
	counter = _mm256_sub_epi32(counter, c);
	bitMask |= (_mm256_movemask_epi8(c) << 12);

	shifted = AvxSupport::shiftRightEpi32<1>(shifted);
	c = _mm256_blend_epi32(_mm256_cmpeq_epi32(a, shifted), _mm256_setzero_si256(), 0xf0);
	counter = _mm256_sub_epi32(counter, c);
	bitMask |= (_mm256_movemask_epi8(c) << 16);

	shifted = AvxSupport::shiftRightEpi32<1>(shifted);
	c = _mm256_blend_epi32(_mm256_cmpeq_epi32(a, shifted), _mm256_setzero_si256(), 0xf8);
	counter = _mm256_sub_epi32(counter, c);
	bitMask |= (_mm256_movemask_epi8(c) << 20);

	shifted = AvxSupport::shiftRightEpi32<1>(shifted);
	c = _mm256_blend_epi32(_mm256_cmpeq_epi32(a, shifted), _mm256_setzero_si256(), 0xfc);
	counter = _mm256_sub_epi32(counter, c);
	bitMask |= (_mm256_movemask_epi8(c) << 24);

	shifted = AvxSupport::shiftRightEpi32<1>(shifted);
	c = _mm256_blend_epi32(_mm256_cmpeq_epi32(a, shifted), _mm256_setzero_si256(), 0xfe);
	counter = _mm256_sub_epi32(counter, c);
	bitMask |= (_mm256_movemask_epi8(c) << 28);

	return { counter, bitMask };
}

inline std::tuple<__m256i, std::uint32_t> AvxHistogram::detectConflictsEpi16(const __m256i a) noexcept
{
	__m256i counter = _mm256_set1_epi16(1);
	std::uint32_t bitMask = 0;

	__m256i shifted = AvxSupport::shiftRightEpi8<2>(a);
	__m256i mask = _mm256_set_epi32(0x0ffff, -1, -1, -1, -1, -1, -1, -1);

	const auto shiftAndCompare = [a, &counter, &bitMask, &shifted, &mask](const int bytePos) noexcept -> void
	{
		const __m256i c = _mm256_and_si256(_mm256_cmpeq_epi16(a, shifted), mask);
		counter = _mm256_sub_epi16(counter, c);
		bitMask |= (_mm256_movemask_epi8(c) << bytePos);
		shifted = AvxSupport::shiftRightEpi8<2>(shifted);
		mask = _mm256_shuffle_epi8(mask, _mm256_set_epi32(0xffff0f0e, 0x0d0c0b0a, 0x09080706, 0x05040302, 0x0f0e0f0e, 0x0d0c0b0a, 0x09080706, 0x05040302));
	};

	shiftAndCompare(2);
	shiftAndCompare(4);
	shiftAndCompare(6);
	shiftAndCompare(8);
	shiftAndCompare(10);
	shiftAndCompare(12);
	shiftAndCompare(14);
	shiftAndCompare(16);
	mask = _mm256_bsrli_epi128(mask, 2);
	shiftAndCompare(18);
	shiftAndCompare(20);
	shiftAndCompare(22);
	shiftAndCompare(24);
	shiftAndCompare(26);
	shiftAndCompare(28);
	const __m256i c = _mm256_and_si256(_mm256_cmpeq_epi16(a, shifted), mask);
	counter = _mm256_sub_epi16(counter, c);
	bitMask |= (_mm256_movemask_epi8(c) << 30);

	return { counter, bitMask };
}

int AvxHistogram::mergeHistograms(HistogramVectorType& red, HistogramVectorType& green, HistogramVectorType& blue)
{
	if (!avxReady)
		return 1;

	const auto mergeHisto = [](HistogramVectorType& targetHisto, const HistogramVectorType& sourceHisto) -> void
	{
		if (targetHisto.size() == HistogramSize() && sourceHisto.size() == HistogramSize())
		{
			constexpr size_t nrVectors = HistogramSize() / 8;
			auto* pTarget{ &*targetHisto.begin() };
			const auto* pSource{ &*sourceHisto.begin() };

			for (size_t n = 0; n < nrVectors; ++n, pTarget += 8, pSource += 8)
			{
				const __m256i tgt = _mm256_add_epi32(_mm256_loadu_si256((const __m256i*)pTarget), _mm256_loadu_si256((const __m256i*)pSource));
				_mm256_storeu_si256((__m256i*)pTarget, tgt);
			}
			for (size_t n = nrVectors * 8; n < HistogramSize(); ++n)
				targetHisto[n] += sourceHisto[n];
		}
		else // Why do we get here?
		{
			for (size_t n = 0; n < targetHisto.size(); ++n) // Let's hope, the targetHisto is larger in size than the sourceHisto.
				targetHisto[n] += sourceHisto[n];
		}
	};

	mergeHisto(red, redHisto);
	mergeHisto(green, greenHisto.empty() ? redHisto : greenHisto);
	mergeHisto(blue, blueHisto.empty() ? redHisto : blueHisto);

	return 0;
}

bool AvxHistogram::histogramSuccessful() const
{
	return allRunsSuccessful;
};

#endif
