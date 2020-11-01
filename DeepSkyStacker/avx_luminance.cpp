#include "StdAfx.h"
#include "avx_luminance.h"
#include "avx_cfa.h"
#include "avx.h"

#if defined(AVX_INTRINSICS) && defined(_M_X64)

AvxLuminance::AvxLuminance(CMemoryBitmap& inputbm, CMemoryBitmap& outbm) noexcept :
	inputBitmap{ inputbm },
	outputBitmap{ outbm },
	avxReady{ true }
{
	if (!AvxSupport::checkSimdAvailability())
		avxReady = false;

	// Check output bitmap (must be monochrome-double).
	if (!AvxSupport{ outputBitmap }.isMonochromeBitmapOfType<double>())
		avxReady = false;
}

int AvxLuminance::computeLuminanceBitmap(const size_t lineStart, const size_t lineEnd)
{
	if (doComputeLuminance<WORD>(lineStart, lineEnd) == 0)
		return 0;
	if (doComputeLuminance<unsigned long>(lineStart, lineEnd) == 0)
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
	constexpr size_t vectorLen = 16;
	const size_t nrVectors = width / vectorLen;

	const auto scaleAndStoreLuminance = [](const __m256d d0, const __m256d d1, const __m256d d2, const __m256d d3, double *const pOut) -> void
	{
		const __m256d scalingFactor = _mm256_set1_pd(1.0 / 255.0);
		_mm256_storeu_pd(pOut, _mm256_mul_pd(d0, scalingFactor));
		_mm256_storeu_pd(pOut + 4, _mm256_mul_pd(d1, scalingFactor));
		_mm256_storeu_pd(pOut + 8, _mm256_mul_pd(d2, scalingFactor));
		_mm256_storeu_pd(pOut + 12, _mm256_mul_pd(d3, scalingFactor));
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
		AvxCfaProcessing avxCfa{0, 0, inputBitmap};
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
				*pOut = static_cast<double>(minColor) + static_cast<double>(maxColor) * (0.5 / 255.0);
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
				*pOut = static_cast<double>(grey) * (1.0 / 255.0);
			}
		}
		return 0;
	}

	return 1;
}

template <class T>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::colorLuminance(const T *const pRed, const T *const pGreen, const T *const pBlue)
{
	const __m256i red = AvxSupport::read16PackedShort(pRed);
	const __m256i green = AvxSupport::read16PackedShort(pGreen);
	const __m256i blue = AvxSupport::read16PackedShort(pBlue);
	const __m256i minColor = _mm256_min_epu16(_mm256_min_epu16(red, green), blue);
	const __m256i maxColor = _mm256_max_epu16(_mm256_max_epu16(red, green), blue);
	const __m256i minMaxAvg = _mm256_avg_epu16(minColor, maxColor);
	return AvxSupport::wordToPackedDouble(minMaxAvg);
}

/*
template <>
inline std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::colorLuminance(const std::uint32_t* const pRed, const std::uint32_t* const pGreen, const std::uint32_t* const pBlue)
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
*/
/*
template <>
inline std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::colorLuminance(const float* const pRed, const float* const pGreen, const float* const pBlue)
{
	const auto averageColors = [](const __m256i red, const __m256i green, const __m256i blue) -> __m256
	{
		const __m256i minColor = _mm256_min_epi32(_mm256_min_epi32(red, green), blue);
		const __m256i maxColor = _mm256_max_epi32(_mm256_max_epi32(red, green), blue);
		const __m256 sum = _mm256_cvtepi32_ps(_mm256_add_epi32(minColor, maxColor));
//		return _mm256_fmadd_ps(maxColor, _mm256_set1_ps(0.5f), _mm256_fmadd_ps(minColor, _mm256_set1_ps(0.5f), _mm256_setzero_ps()));
		return _mm256_mul_ps(sum, _mm256_set1_ps(0.5f));
	};

	__m256i red = _mm256_cvttps_epi32(_mm256_loadu_ps(pRed));
	__m256i green = _mm256_cvttps_epi32(_mm256_loadu_ps(pGreen));
	__m256i blue = _mm256_cvttps_epi32(_mm256_loadu_ps(pBlue));
	const auto [d1, d2] = AvxSupport::cvtPsPd(averageColors(red, green, blue));

	red = _mm256_cvttps_epi32(_mm256_loadu_ps(pRed + 8));
	green = _mm256_cvttps_epi32(_mm256_loadu_ps(pGreen + 8));
	blue = _mm256_cvttps_epi32(_mm256_loadu_ps(pBlue + 8));
	const auto [d3, d4] = AvxSupport::cvtPsPd(averageColors(red, green, blue));

	return { d1, d2, d3, d4 };
}
*/
template <class T>
std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::greyLuminance(const T* const pGray)
{
	const __m256i gray = AvxSupport::read16PackedShort(pGray);
	return AvxSupport::wordToPackedDouble(gray);
}
/*
template <>
inline std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::greyLuminance(const std::uint32_t* const pGrey)
{
	const auto [d1, d2] = AvxSupport::cvtEpu32Pd(_mm256_loadu_si256((const __m256i*)pGrey));
	const auto [d3, d4] = AvxSupport::cvtEpu32Pd(_mm256_loadu_si256((const __m256i*)(pGrey + 8)));
	return multiplyPd({ d1, d2, d3, d4 }, normalizationFactor<std::uint32_t>());
}

template <>
inline std::tuple<__m256d, __m256d, __m256d, __m256d> AvxLuminance::greyLuminance(const float* const pGrey)
{
	const auto [d1, d2] = AvxSupport::cvtPsPd(_mm256_loadu_ps(pGrey));
	const auto [d3, d4] = AvxSupport::cvtPsPd(_mm256_loadu_ps(pGrey + 8));
	return { d1, d2, d3, d4 };
}
*/
#endif
