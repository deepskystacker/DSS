#include "StdAfx.h"
#include "avx_luminance.h"
#include "avx_cfa.h"
#include "avx.h"

AvxLuminance::AvxLuminance(CMemoryBitmap& inputbm, CMemoryBitmap& outbm) noexcept :
	inputBitmap{ inputbm },
	outputBitmap{ outbm },
	avxReady{ true }
{
	if (!AvxSupport::checkCpuFeatures())
		avxReady = false;

	// Check output bitmap (must be monochrome-double).
	if (!AvxSupport{ outputBitmap }.isMonochromeBitmapOfType<double>())
		avxReady = false;
}

int AvxLuminance::computeLuminanceBitmap(const size_t lineStart, const size_t lineEnd)
{
	if (doComputeLuminance<WORD>(lineStart, lineEnd) == 0)
		return 0;
	if (doComputeLuminance<std::uint32_t>(lineStart, lineEnd) == 0)
		return 0;
	if (doComputeLuminance<float>(lineStart, lineEnd) == 0)
		return 0;

	return 1;
}

template <class T>
int AvxLuminance::doComputeLuminance(const size_t lineStart, const size_t lineEnd)
{
	if (!avxReady)
		return 1;

	// Check input bitmap.
	const AvxSupport avxInputSupport{ inputBitmap };
	if (!avxInputSupport.isColorBitmapOfType<T>() && !avxInputSupport.isMonochromeBitmapOfType<T>()) // Monochrome includes CFA
		return 1;

	AvxSupport avxOutputSupport{ outputBitmap };
	const size_t width = inputBitmap.Width();
	const size_t height = inputBitmap.Height();
	const size_t nrVectors = width / 16;

	const auto storeLuminance = [](const __m256d d0, const __m256d d1, const __m256d d2, const __m256d d3, double *const pOut) -> void
	{
		_mm256_storeu_pd(pOut, d0);
		_mm256_storeu_pd(pOut + 4, d1);
		_mm256_storeu_pd(pOut + 8, d2);
		_mm256_storeu_pd(pOut + 12, d3);
	};

	const bool isCFA = avxInputSupport.isMonochromeCfaBitmapOfType<T>();

	if (avxInputSupport.isColorBitmapOfType<T>() || isCFA)
	{
		AvxCfaProcessing avxCfa{0, 0, inputBitmap};
		if (isCFA)
		{
			avxCfa.init(lineStart, lineEnd);
			avxCfa.interpolate(lineStart, lineEnd, 1);
		}

		for (size_t row = lineStart, lineNdx = 0; row < lineEnd; ++row, ++lineNdx)
		{
			const T* pRedPixels = isCFA ? avxCfa.redCfaPixels<T>(lineNdx * width) : &avxInputSupport.redPixels<T>().at(row * width);
			const T* pGreenPixels = isCFA ? avxCfa.greenCfaPixels<T>(lineNdx * width) : &avxInputSupport.greenPixels<T>().at(row * width);
			const T* pBluePixels = isCFA ? avxCfa.blueCfaPixels<T>(lineNdx * width) : &avxInputSupport.bluePixels<T>().at(row * width);
			double* pOut = &avxOutputSupport.grayPixels<double>().at(row * width);

			for (size_t counter = 0; counter < nrVectors; ++counter, pRedPixels += 16, pGreenPixels += 16, pBluePixels += 16, pOut += 16)
			{
				const auto [d0, d1, d2, d3] = colorLuminance(pRedPixels, pGreenPixels, pBluePixels);
				storeLuminance(d0, d1, d2, d3, pOut);
			}
			for (size_t n = nrVectors * 16; n < width; ++n, ++pRedPixels, ++pGreenPixels, ++pBluePixels, ++pOut)
			{
				const T red = *pRedPixels;
				const T green = *pGreenPixels;
				const T blue = *pBluePixels;
				const T minColor = std::min(std::min(red, green), blue);
				const T maxColor = std::max(std::max(red, green), blue);
				*pOut = static_cast<double>(minColor + maxColor) * 0.5 * normalizationFactor<T>();
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

			for (size_t counter = 0; counter < nrVectors; ++counter, pGreyPixels += 16, pOut += 16)
			{
				const auto [d0, d1, d2, d3] = greyLuminance(pGreyPixels);
				storeLuminance(d0, d1, d2, d3, pOut);
			}
			for (size_t n = nrVectors * 16; n < width; ++n, ++pGreyPixels, ++pOut)
			{
				const T grey = *pGreyPixels;
				*pOut = static_cast<double>(grey) * normalizationFactor<T>();
			}
		}
		return 0;
	}

	return 1;
}

template <class T>
constexpr double AvxLuminance::normalizationFactor() noexcept { return 1.0 / 255.0; }
template <>
constexpr double AvxLuminance::normalizationFactor<WORD>() noexcept { return 1.0 / 255.0; } // 255 looks strange, but this is how it's done in the original implementation.
template <>
constexpr double AvxLuminance::normalizationFactor<std::uint32_t>() noexcept { return 1.0 / (65536.0 * 255); }

template <class T>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::colorLuminance(const T *const pRed, const T *const pGreen, const T *const pBlue)
{
	throw std::runtime_error("Type not implemented");
}
template <>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::colorLuminance(const WORD *const pRed, const WORD *const pGreen, const WORD *const pBlue)
{
	const __m256i red = _mm256_loadu_si256((const __m256i*)pRed);
	const __m256i green = _mm256_loadu_si256((const __m256i*)pGreen);
	const __m256i blue = _mm256_loadu_si256((const __m256i*)pBlue);
	const __m256i minColor = _mm256_min_epu16(_mm256_min_epu16(red, green), blue);
	const __m256i maxColor = _mm256_max_epu16(_mm256_max_epu16(red, green), blue);
	const __m256i minMaxAvg = _mm256_avg_epu16(minColor, maxColor);
	return multiplyPd(AvxSupport::wordToPackedDouble(minMaxAvg), normalizationFactor<WORD>());
}

template <>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::colorLuminance(const std::uint32_t* const pRed, const std::uint32_t* const pGreen, const std::uint32_t* const pBlue)
{
	__m256i red = _mm256_loadu_si256((const __m256i*)pRed);
	__m256i green = _mm256_loadu_si256((const __m256i*)pGreen);
	__m256i blue = _mm256_loadu_si256((const __m256i*)pBlue);
	__m256i minColor = _mm256_min_epu32(_mm256_min_epu32(red, green), blue);
	__m256i maxColor = _mm256_max_epu32(_mm256_max_epu32(red, green), blue);
	const auto [d1, d2] = AvxSupport::cvtEpu32Pd(_mm256_add_epi32(_mm256_srli_epi32(minColor, 1), _mm256_srli_epi32(maxColor, 1))); // Note: add_epi32(epi32, epi32) is identical result to add_epu32(epu32, epu32)

	red = _mm256_loadu_si256((const __m256i*)(pRed + 8));
	green = _mm256_loadu_si256((const __m256i*)(pGreen + 8));
	blue = _mm256_loadu_si256((const __m256i*)(pBlue + 8));
	minColor = _mm256_min_epu32(_mm256_min_epu32(red, green), blue);
	maxColor = _mm256_max_epu32(_mm256_max_epu32(red, green), blue);
	const auto [d3, d4] = AvxSupport::cvtEpu32Pd(_mm256_add_epi32(_mm256_srli_epi32(minColor, 1), _mm256_srli_epi32(maxColor, 1)));

	return multiplyPd({d1, d2, d3, d4}, normalizationFactor<std::uint32_t>());
}

template <>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::colorLuminance(const float* const pRed, const float* const pGreen, const float* const pBlue)
{
	__m256 red = _mm256_floor_ps(_mm256_loadu_ps(pRed));
	__m256 green = _mm256_floor_ps(_mm256_loadu_ps(pGreen));
	__m256 blue = _mm256_floor_ps(_mm256_loadu_ps(pBlue));
	__m256 minColor = _mm256_min_ps(_mm256_min_ps(red, green), blue);
	__m256 maxColor = _mm256_max_ps(_mm256_max_ps(red, green), blue);
	const auto [d1, d2] = AvxSupport::cvtPsPd(_mm256_fmadd_ps(maxColor, _mm256_set1_ps(0.5f), _mm256_fmadd_ps(minColor, _mm256_set1_ps(0.5f), _mm256_setzero_ps())));

	red = _mm256_floor_ps(_mm256_loadu_ps(pRed + 8));
	green = _mm256_floor_ps(_mm256_loadu_ps(pGreen + 8));
	blue = _mm256_floor_ps(_mm256_loadu_ps(pBlue + 8));
	minColor = _mm256_min_ps(_mm256_min_ps(red, green), blue);
	maxColor = _mm256_max_ps(_mm256_max_ps(red, green), blue);
	const auto [d3, d4] = AvxSupport::cvtPsPd(_mm256_fmadd_ps(maxColor, _mm256_set1_ps(0.5f), _mm256_fmadd_ps(minColor, _mm256_set1_ps(0.5f), _mm256_setzero_ps())));

	return multiplyPd({ d1, d2, d3, d4 }, normalizationFactor<float>());
}

template <class T>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::greyLuminance(const T* const pGrey)
{
	throw std::runtime_error("Type not implemented");
}

template <>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::greyLuminance(const WORD* const pGrey)
{
	const __m256i grey = _mm256_loadu_si256((const __m256i*)pGrey);
	return multiplyPd(AvxSupport::wordToPackedDouble(grey), normalizationFactor<WORD>());
}

template <>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::greyLuminance(const std::uint32_t* const pGrey)
{
	const auto [d1, d2] = AvxSupport::cvtEpu32Pd(_mm256_loadu_si256((const __m256i*)pGrey));
	const auto [d3, d4] = AvxSupport::cvtEpu32Pd(_mm256_loadu_si256((const __m256i*)(pGrey + 8)));
	return multiplyPd({ d1, d2, d3, d4 }, normalizationFactor<std::uint32_t>());
}

template <>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::greyLuminance(const float* const pGrey)
{
	const auto [d1, d2] = AvxSupport::cvtPsPd(_mm256_loadu_ps(pGrey));
	const auto [d3, d4] = AvxSupport::cvtPsPd(_mm256_loadu_ps(pGrey + 8));
	return { d1, d2, d3, d4 };
}
