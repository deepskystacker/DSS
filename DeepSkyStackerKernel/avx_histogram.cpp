#include "stdafx.h"
#include "avx_includes.h"
#include "avx_histogram.h"
#include "avx_support.h"
#include "histogram.h"
#include "BezierAdjust.h"
#include "ColorHelpers.h"

AvxHistogram::AvxHistogram(const CMemoryBitmap& inputbm) :
	avxReady{ AvxSimdCheck::checkSimdAvailability() },
	redHisto(HistogramSize(), 0),
	greenHisto(HistogramSize(), 0),
	blueHisto(HistogramSize(), 0),
	avxCfa{ 0, 0, inputbm },
	inputBitmap{ inputbm }
{
	static_assert(sizeof(HistogramVectorType::value_type) == sizeof(int));
}

int AvxHistogram::calcHistogram(const size_t lineStart, const size_t lineEnd, const double multiplier)
{
	return SimdSelector<Avx256Histogram, NonAvxHistogram>(this, [&](auto&& o) { return o.calcHistogram(lineStart, lineEnd, multiplier); });
}

int AvxHistogram::mergeHistograms(HistogramVectorType& red, HistogramVectorType& green, HistogramVectorType& blue)
{
	const auto mergeHisto = [this](HistogramVectorType& targetHisto, const HistogramVectorType& sourceHisto) -> void
	{
		if (this->avxReady && targetHisto.size() == HistogramSize() && sourceHisto.size() == HistogramSize())
		{
			constexpr size_t VecLen = sizeof(__m256i) / sizeof(int);
			constexpr size_t nrVectors = HistogramSize() / VecLen;
			auto* pTarget = targetHisto.data();
			const auto* pSource = sourceHisto.data();

			for (size_t n = 0; n < nrVectors; ++n, pTarget += VecLen, pSource += VecLen)
			{
				const __m256i tgt = _mm256_add_epi32(_mm256_loadu_si256((const __m256i*)pTarget), _mm256_loadu_si256((const __m256i*)pSource));
				_mm256_storeu_si256((__m256i*)pTarget, tgt);
			}
			for (size_t n = nrVectors * VecLen; n < HistogramSize(); ++n)
				targetHisto[n] += sourceHisto[n];
		}
		else // !avxReady
		{
			for (size_t n = 0; n < sourceHisto.size(); ++n) // Let's hope, the targetHisto is not smaller in size than the sourceHisto.
				targetHisto[n] += sourceHisto[n];
		}
	};

	const bool isColor = AvxSupport{ inputBitmap }.isColorBitmapOrCfa();

	mergeHisto(red, redHisto);
	mergeHisto(green, isColor ? greenHisto : redHisto);
	mergeHisto(blue, isColor ? blueHisto : redHisto);

	return this->avxReady ? AvxSupport::zeroUpper(0) : 0;
}

// *****************
// AVX-256 Histogram
// *****************

int Avx256Histogram::calcHistogram(const size_t lineStart, const size_t lineEnd, const double)
{
	if (!this->histoData.avxReady)
		return 1;

	if (doCalcHistogram<std::uint16_t>(lineStart, lineEnd) == 0)
		return AvxSupport::zeroUpper(0);
	if (doCalcHistogram<std::uint32_t>(lineStart, lineEnd) == 0)
		return AvxSupport::zeroUpper(0);
	if (doCalcHistogram<float>(lineStart, lineEnd) == 0)
		return AvxSupport::zeroUpper(0);
	if (doCalcHistogram<double>(lineStart, lineEnd) == 0)
		return AvxSupport::zeroUpper(0);

	return 1;
}

template <class T>
int Avx256Histogram::doCalcHistogram(const size_t lineStart, const size_t lineEnd)
{
	// Check input bitmap.
	const AvxSupport avxInputSupport{ histoData.inputBitmap };
	if (!avxInputSupport.isColorBitmapOfType<T>() && !avxInputSupport.isMonochromeBitmapOfType<T>()) // Monochrome includes CFA
		return 1;

	constexpr size_t vectorLen = 16;
	const size_t width = histoData.inputBitmap.Width();
	const size_t nrVectors = width / vectorLen;

	// AVX makes no sense for super-small arrays.
	if (width < 256 || histoData.inputBitmap.Height() < 32)
		return 1;

	const auto calcHistoOfTwoVectorsEpi32 = [](const std::tuple<__m256i, __m256i>& twoVectors, auto& histogram) -> void
	{
		const auto [lo, hi] = twoVectors;
		calcHistoOfVectorEpi32(lo, histogram);
		calcHistoOfVectorEpi32(hi, histogram);
	};

	const bool isCFA = avxInputSupport.isMonochromeCfaBitmapOfType<T>();

	// Color bitmap (incl. CFA)
	// ------------------------
	if (avxInputSupport.isColorBitmapOfType<T>() || isCFA)
	{
		if constexpr (std::is_same<T, double>::value) // color-double not supported. 
			return 1;
		else
		{
			if (isCFA)
			{
				histoData.avxCfa.init(lineStart, lineEnd);
				histoData.avxCfa.interpolate(lineStart, lineEnd, 1);
			}

			for (size_t row = lineStart, lineNdx = 0; row < lineEnd; ++row, ++lineNdx)
			{
				const T* pRedPixels = isCFA ? histoData.avxCfa.redCfaLine<T>(lineNdx) : &avxInputSupport.redPixels<T>().at(row * width);
				const T* pGreenPixels = isCFA ? histoData.avxCfa.greenCfaLine<T>(lineNdx) : &avxInputSupport.greenPixels<T>().at(row * width);
				const T* pBluePixels = isCFA ? histoData.avxCfa.blueCfaLine<T>(lineNdx) : &avxInputSupport.bluePixels<T>().at(row * width);

				for (size_t counter = 0; counter < nrVectors; ++counter, pRedPixels += vectorLen, pGreenPixels += vectorLen, pBluePixels += vectorLen)
				{
					calcHistoOfTwoVectorsEpi32(AvxSupport::read16PackedInt(pRedPixels), histoData.redHisto);
					calcHistoOfTwoVectorsEpi32(AvxSupport::read16PackedInt(pGreenPixels), histoData.greenHisto);
					calcHistoOfTwoVectorsEpi32(AvxSupport::read16PackedInt(pBluePixels), histoData.blueHisto);
				}
				for (size_t n = nrVectors * vectorLen; n < width; ++n, ++pRedPixels, ++pGreenPixels, ++pBluePixels)
				{
					addToHisto(histoData.redHisto, *pRedPixels);
					addToHisto(histoData.greenHisto, *pGreenPixels);
					addToHisto(histoData.blueHisto, *pBluePixels);
				}
			}
			return 0;
		}
	}

	// Note:
	// Gray input bitmaps of type double use a fix scaling factor of 256.
	// This is for the histogram in the registering process, where the color values come from the luminance calculation. They are in the range [0, 256).
	// Thus, they need up-scaling by a factor of 256.
	if (avxInputSupport.isMonochromeBitmapOfType<T>())
	{
		for (size_t row = lineStart; row < lineEnd; ++row)
		{
			const T* pGrayPixels = &avxInputSupport.grayPixels<T>().at(row * width);
			for (size_t counter = 0; counter < nrVectors; ++counter, pGrayPixels += vectorLen)
			{
				if constexpr (std::is_same<T, double>::value)
					calcHistoOfTwoVectorsEpi32(AvxSupport::read16PackedInt(pGrayPixels, _mm256_set1_pd(256.0)), histoData.redHisto);
				else
					calcHistoOfTwoVectorsEpi32(AvxSupport::read16PackedInt(pGrayPixels), histoData.redHisto);
			}
			for (size_t n = nrVectors * vectorLen; n < width; ++n, ++pGrayPixels)
			{
				addToHisto(histoData.redHisto, *pGrayPixels);
			}
		}
		return 0;
	}

	return 1;
}

// Conflict detection: Number of equal elements + blocking mask. 
std::tuple<__m256i, std::uint32_t> Avx256Histogram::detectConflictsEpi32(const __m256i a) noexcept
{
	__m256i counter = _mm256_set1_epi32(1);
	std::uint32_t bitMask = 0;

	__m256i shifted = AvxSupport::shiftRightEpi32<1>(a);
	__m256i c = _mm256_blend_epi32(_mm256_cmpeq_epi32(a, shifted), _mm256_setzero_si256(), 0x80); // Set highest mask to "no conflict" (=0) 
	counter = _mm256_sub_epi32(counter, c); // Add one where there is a conflict with the element left from it (by one position). 
	bitMask |= (_mm256_movemask_epi8(c) << 4); // If there is a conflict with the element left from it -> set the mask of that element to 1. 

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

template <class T>
void Avx256Histogram::calcHistoOfVectorEpi32(const __m256i colorVec, T& histogram)
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
}

// Explicit template instantiation for the type we use (vector<int>).
template void Avx256Histogram::calcHistoOfVectorEpi32(const __m256i colorVec, std::vector<int>& histogram);


// *****************
// Non-AVX Histogram
// *****************

int NonAvxHistogram::calcHistogram(const size_t lineStart, const size_t lineEnd, const double multiplier)
{
	const size_t width = histoData.inputBitmap.Width();
	const double fMultiplier = multiplier * 256.0;

	for (size_t row = lineStart; row < lineEnd; ++row)
	{
		for (size_t col = 0; col < width; ++col)
		{
			double fRed, fGreen, fBlue;
			this->histoData.inputBitmap.GetPixel(col, row, fRed, fGreen, fBlue);

			constexpr auto ColorToIndex = [](const double color) {
				constexpr double Maxvalue = static_cast<double>(std::numeric_limits<std::uint16_t>::max());
				return static_cast<size_t>(std::min(color, Maxvalue));
			};

			++this->histoData.redHisto[ColorToIndex(fRed * fMultiplier)];
			++this->histoData.greenHisto[ColorToIndex(fGreen * fMultiplier)];
			++this->histoData.blueHisto[ColorToIndex(fBlue * fMultiplier)];
		}
	}

	return 0;
}



// *************
// Bezier class
// *************
namespace {

	void adjust(const DSS::HistogramAdjust& hadj, float* const p, const size_t len)
	{
		using VecType = __m256;
		constexpr size_t VecLen = sizeof(VecType) / sizeof(float);

		const VecType mn = _mm256_set1_ps(hadj.GetMin());
		const VecType mx = _mm256_set1_ps(hadj.GetMax());
		const VecType sh = _mm256_set1_ps(hadj.GetShift());
		const VecType omn = _mm256_set1_ps(hadj.getOriginalMinimum());
		const VecType omx = _mm256_set1_ps(hadj.getOriginalMaximum());
		const VecType umn = _mm256_set1_ps(hadj.getUsedMinimum());
		const VecType umx = _mm256_set1_ps(hadj.getUsedMaximum());
		const VecType usedMinusOrgMin = _mm256_sub_ps(umn, omn);
		const VecType orgMinusUsedMax = _mm256_sub_ps(omx, umx);
		const VecType minMinusOrgMin = _mm256_sub_ps(mn, omn);
		const VecType orgMaxMinusMax = _mm256_sub_ps(omx, mx);
		const VecType usedMaxMinusMin = _mm256_sub_ps(umx, umn);

		const auto a1 = [&](const VecType value) -> VecType
		{
			const VecType valMinusMax = _mm256_sub_ps(value, mx);

			const VecType v1 = _mm256_fmadd_ps(_mm256_div_ps(value, _mm256_max_ps(_mm256_set1_ps(1.0f), minMinusOrgMin)), usedMinusOrgMin, omn); // m_fOrgMin + fValue / std::max(1.0, m_fMin - m_fOrgMin) * (m_fUsedMin - m_fOrgMin)
			const VecType v2 = _mm256_fnmadd_ps(_mm256_div_ps(valMinusMax, _mm256_max_ps(_mm256_set1_ps(1.0f), orgMaxMinusMax)), orgMinusUsedMax, omx); // m_fOrgMax - (fValue - m_fMax) / std::max(1.0, m_fOrgMax - m_fMax) * (m_fOrgMax - m_fUsedMax)
			const VecType v3 = _mm256_div_ps(_mm256_sub_ps(value, mn), _mm256_max_ps(_mm256_set1_ps(1.0f), _mm256_sub_ps(mx, mn)));
			const VecType adjusted = avxLog(_mm256_fmadd_ps(_mm256_sqrt_ps(v3), _mm256_set1_ps(1.7f), _mm256_set1_ps(1.0f))); // log(pow(fValue, 1/2.0)*1.7+1);
			const VecType v4 = _mm256_fmadd_ps(adjusted, usedMaxMinusMin, umn); // m_fUsedMin + AdjustValue((fValue - m_fMin) / std::max(1.0, (m_fMax - m_fMin))) * (m_fUsedMax - m_fUsedMin)
			const VecType v5 = _mm256_fmadd_ps(usedMaxMinusMin, sh, _mm256_blendv_ps(_mm256_blendv_ps(v4, v2, _mm256_cmp_ps(value, mx, _CMP_GT_OQ)), v1, _mm256_cmp_ps(value, mn, _CMP_LT_OQ)));

			return _mm256_blendv_ps(_mm256_blendv_ps(v5, v2, _mm256_cmp_ps(v5, omx, _CMP_GT_OQ)), v1, _mm256_cmp_ps(v5, omn, _CMP_LT_OQ));
		};

		for (size_t n = 0; n < len / VecLen; ++n)
		{
			const VecType v = a1(_mm256_loadu_ps(p + n * VecLen));
			_mm256_storeu_ps(p + n * VecLen, v);
		}

		// mask: Set lower N epi32 to -1, remaining higher epi32 to 0. N = static_cast<int>(len % VecLen)
		const __m256i mask = AvxSupport::shiftRightVarEpi8(_mm256_set1_epi32(-1), static_cast<int>(VecLen - len % VecLen) * 4);
		_mm256_maskstore_ps( // store int[n] IF maskbit[31] == 1
			p + (len / VecLen) * VecLen,
			mask,
			a1(_mm256_maskload_ps(p + (len / VecLen) * VecLen, mask)) // maskload: element is zeroed out when the high bit of mask[n] == 0.
		);
		// len   mask
		//   8   0  0  0  0  0  0  0  0
		//   9   0  0  0  0  0  0  0 -1
		//  10   0  0  0  0  0  0 -1 -1
		//  ...
		//  15   0 -1 -1 -1 -1 -1 -1 -1
	}

	inline void finalScale(float* const p, const size_t len)
	{
		using VecType = __m256;
		constexpr size_t VecLen = sizeof(VecType) / sizeof(float);

		const auto scale = [factor = _mm256_set1_ps(1.0f / 255.0f)](const VecType x) -> VecType
		{
			return _mm256_min_ps(_mm256_mul_ps(x, factor), _mm256_set1_ps(255.0f));
		};

		for (size_t n = 0; n < len / VecLen; ++n)
		{
			const VecType v = scale(_mm256_loadu_ps(p + n * VecLen));
			_mm256_storeu_ps(p + n * VecLen, v);
		}

		const __m256i mask = AvxSupport::shiftRightVarEpi8(_mm256_set1_epi32(-1), static_cast<int>(VecLen - len % VecLen) * 4);
		_mm256_maskstore_ps(
			p + (len / VecLen) * VecLen,
			mask,
			scale(_mm256_maskload_ps(p + (len / VecLen) * VecLen, mask)) // maskload sets non-loaded elements to zero.
		);
	}
}

AvxBezierAndSaturation::AvxBezierAndSaturation(const size_t bufferLen) :
	avxSupported{ AvxSimdCheck::checkSimdAvailability() },
	redBuffer(bufferLen), greenBuffer(bufferLen), blueBuffer(bufferLen),
	bezierX{}, bezierY{}
{}

void AvxBezierAndSaturation::copyData(const float* const pRedPixel, const float* const pGreenPixel, const float* const pBluePixel, const size_t bufferLen, const bool monochrome)
{
	if (bufferLen != this->redBuffer.size())
	{
		this->redBuffer.resize(bufferLen);
		this->greenBuffer.resize(bufferLen);
		this->blueBuffer.resize(bufferLen);
	}
	memcpy(this->redBuffer.data(), pRedPixel, bufferLen * sizeof(float));
	memcpy(this->greenBuffer.data(), monochrome ? pRedPixel : pGreenPixel, bufferLen * sizeof(float));
	memcpy(this->blueBuffer.data(), monochrome ? pRedPixel : pBluePixel, bufferLen * sizeof(float));
}

std::tuple<float*, float*, float*> AvxBezierAndSaturation::getBufferPtr()
{
	return { this->redBuffer.data(), this->greenBuffer.data(), this->blueBuffer.data() };
}


int AvxBezierAndSaturation::toHsl()
{
	return SimdSelector<Avx256BezierAndSaturation, NonAvxBezierAndSaturation>(this, [](auto&& o) { return o.avxToHsl(); });
}

int AvxBezierAndSaturation::avxAdjustRGB(const int nBitmaps, const DSS::RGBHistogramAdjust& histoAdjust)
{
	return SimdSelector<Avx256BezierAndSaturation, NonAvxBezierAndSaturation>(this, [&](auto&& o) { return o.avxAdjustRGB(nBitmaps, histoAdjust); });
}

int AvxBezierAndSaturation::avxToRgb(const bool markOverAndUnderExposure)
{
	return SimdSelector<Avx256BezierAndSaturation, NonAvxBezierAndSaturation>(
		this,
		[markOverAndUnderExposure](auto&& o) { return o.avxToRgb(markOverAndUnderExposure); }
	);
}

int AvxBezierAndSaturation::avxBezierAdjust(const size_t len)
{
	return SimdSelector<Avx256BezierAndSaturation, NonAvxBezierAndSaturation>(this, [len](auto&& o) { return o.avxBezierAdjust(len); });
}

int AvxBezierAndSaturation::avxBezierSaturation(const size_t len, const float saturationShift)
{
	return SimdSelector<Avx256BezierAndSaturation, NonAvxBezierAndSaturation>(this, [len, saturationShift](auto&& o) { return o.avxBezierSaturation(len, saturationShift); });
}

// ----------------------------------------------------------------------------------
// AVX-256 Bezier functions
// ----------------------------------------------------------------------------------

#pragma warning (disable: 4100)

// static
__m256i Avx256BezierAndSaturation::avx256LowerBoundPs(const float* const pValues, const unsigned int N, const __m256 refVal)
{
	__m256i notYetFound = _mm256_set1_epi32(-1); // 0xffff
	__m256i n = _mm256_set1_epi32(N);
	__m256i first = _mm256_setzero_si256();

	do
	{
		const __m256i n2 = _mm256_srli_epi32(n, 1); // n/2
		const __m256i m = _mm256_add_epi32(first, n2);
		const __m256 v = _mm256_i32gather_ps(pValues, m, sizeof(float));
		const __m256i isSmaller = _mm256_and_si256(notYetFound, _mm256_castps_si256(_mm256_cmp_ps(v, refVal, _CMP_LT_OQ))); // smaller = notFound && value[m] < refVal;
		n = _mm256_blendv_epi8(n2, _mm256_sub_epi32(n, _mm256_sub_epi32(n2, _mm256_set1_epi32(-1))), isSmaller); // if (smaller) n = n-(n2+1); else n = n2;  => n = n/2-1 OR n/2
		first = _mm256_blendv_epi8(first, _mm256_sub_epi32(m, _mm256_set1_epi32(-1)), isSmaller); // if (smaller) first = m+1;
		notYetFound = _mm256_cmpgt_epi32(n, _mm256_setzero_si256()); // if (n > 0) -> result not found.
	} while (_mm256_testc_si256(_mm256_setzero_si256(), notYetFound) == 0); // testc(0, b): if (b == 0) return 1; else return 0;  ==> iterates while notYetFound != 0

	return first;
}

int Avx256BezierAndSaturation::avxAdjustRGB(const int nBitmaps, const DSS::RGBHistogramAdjust& histoAdjust)
{
	if (!this->histoData.avxSupported)
		return 1;

	const size_t len = this->histoData.redBuffer.size();

	const float scale = 255.0f / static_cast<float>(nBitmaps);
	const auto redAdjust = histoAdjust.GetRedAdjust();
	const auto greenAdjust = histoAdjust.GetGreenAdjust();
	const auto blueAdjust = histoAdjust.GetBlueAdjust();

	if (redAdjust.GetAdjustMethod() != HistogramAdjustmentCurve::LogSquareRoot || greenAdjust.GetAdjustMethod() != HistogramAdjustmentCurve::LogSquareRoot || blueAdjust.GetAdjustMethod() != HistogramAdjustmentCurve::LogSquareRoot)
		return 2;

	for (size_t n = 0; n < len; ++n)
	{
		this->histoData.redBuffer[n] *= scale;
		this->histoData.greenBuffer[n] *= scale;
		this->histoData.blueBuffer[n] *= scale;
	}

	adjust(redAdjust, this->histoData.redBuffer.data(), len);
	adjust(greenAdjust, this->histoData.greenBuffer.data(), len);
	adjust(blueAdjust, this->histoData.blueBuffer.data(), len);

	finalScale(this->histoData.redBuffer.data(), len);
	finalScale(this->histoData.greenBuffer.data(), len);
	finalScale(this->histoData.blueBuffer.data(), len);

	return AvxSupport::zeroUpper(0);
}

int Avx256BezierAndSaturation::avxToHsl()
{
	if (!this->histoData.avxSupported)
		return 1;

	const size_t len = this->histoData.redBuffer.size();

	using VecType = __m256;
	constexpr size_t VecLen = sizeof(VecType) / sizeof(float);

	const auto toHSL = [](const VecType r, const VecType g, const VecType b) -> std::tuple<VecType, VecType, VecType>
	{
		const VecType mn = _mm256_min_ps(_mm256_min_ps(r, g), b);
		const VecType mx = _mm256_max_ps(_mm256_max_ps(r, g), b);
		const VecType diff = _mm256_sub_ps(mx, mn);
		const VecType sum = _mm256_add_ps(mx, mn);

		const VecType l = _mm256_div_ps(sum, _mm256_set1_ps(510.0f));

		const VecType s1 = _mm256_div_ps(diff, sum);
		const VecType s2 = _mm256_div_ps(diff, _mm256_sub_ps(_mm256_set1_ps(510.0f), sum));
		const VecType s = _mm256_blendv_ps(s2, s1, _mm256_cmp_ps(l, _mm256_set1_ps(0.5f), _CMP_LE_OQ)); // S = (L <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

		const VecType rn = _mm256_div_ps(_mm256_sub_ps(mx, r), diff);
		const VecType gn = _mm256_div_ps(_mm256_sub_ps(mx, g), diff);
		const VecType bn = _mm256_div_ps(_mm256_sub_ps(mx, b), diff);
		const VecType h1 = _mm256_fmadd_ps(_mm256_set1_ps(60.0f), _mm256_sub_ps(bn, gn), _mm256_set1_ps(360.0f)); // 60.0f * (6.0f + bnorm - gnorm); // = 60*(bn-gn) + 360
		const VecType h2 = _mm256_fmadd_ps(_mm256_set1_ps(60.0f), _mm256_sub_ps(rn, bn), _mm256_set1_ps(120.0f)); // 60.0f * (2.0f + rnorm - bnorm); // = 60*(rn-bn) + 120
		const VecType h3 = _mm256_fmadd_ps(_mm256_set1_ps(60.0f), _mm256_sub_ps(gn, rn), _mm256_set1_ps(240.0f)); // 60.0f * (4.0f + gnorm - rnorm); // = 60*(gn-rn) + 240
		const VecType h4 = _mm256_blendv_ps(_mm256_blendv_ps(h1, h2, _mm256_cmp_ps(g, mx, _CMP_EQ_OQ)), h3, _mm256_cmp_ps(b, mx, _CMP_EQ_OQ));
		const VecType h = _mm256_blendv_ps(h4, _mm256_sub_ps(h4, _mm256_set1_ps(360.0f)), _mm256_cmp_ps(h4, _mm256_set1_ps(360.0f), _CMP_GT_OQ)); // if (H > 360.0f) H = H - 360.0f;

		const VecType minNeqMax = _mm256_cmp_ps(mn, mx, _CMP_NEQ_OQ); // minval != maxval ? mask=0xff...f : mask=0

		return { _mm256_and_ps(h, minNeqMax), _mm256_and_ps(s, minNeqMax), l }; // if (maxval == minval) {0,0,l} else {h,s,l}
	};

	const auto [pr, pg, pb] = this->histoData.getBufferPtr();

	for (size_t n = 0; n < len / VecLen; ++n)
	{
		const auto [h, s, l] = toHSL(_mm256_loadu_ps(pr + n * VecLen), _mm256_loadu_ps(pg + n * VecLen), _mm256_loadu_ps(pb + n * VecLen));
		_mm256_storeu_ps(pr + n * VecLen, h);
		_mm256_storeu_ps(pg + n * VecLen, s);
		_mm256_storeu_ps(pb + n * VecLen, l);
	}

	const __m256i mask = AvxSupport::shiftRightVarEpi8(_mm256_set1_epi32(-1), static_cast<int>(VecLen - len % VecLen) * 4);
	const auto [h, s, l] = toHSL(
		_mm256_maskload_ps(pr + (len / VecLen) * VecLen, mask), // maskload sets non-loaded elements to zero.
		_mm256_maskload_ps(pg + (len / VecLen) * VecLen, mask),
		_mm256_maskload_ps(pb + (len / VecLen) * VecLen, mask)
	);
	_mm256_maskstore_ps(pr + (len / VecLen) * VecLen, mask, h);
	_mm256_maskstore_ps(pg + (len / VecLen) * VecLen, mask, s);
	_mm256_maskstore_ps(pb + (len / VecLen) * VecLen, mask, l);

	return AvxSupport::zeroUpper(0);
}

int Avx256BezierAndSaturation::avxToRgb(const bool markOverAndUnderExposure)
{
	if (!this->histoData.avxSupported)
		return 1;

	const size_t len = this->histoData.redBuffer.size();
	using VecType = __m256;
	constexpr size_t VecLen = sizeof(VecType) / sizeof(float);

	const auto rgb1 = [](const VecType rm1, const VecType rm2, const VecType rmdiff, const VecType rh) -> VecType
	{
		const VecType rh1 = _mm256_blendv_ps(rh, _mm256_sub_ps(rh, _mm256_set1_ps(360.0f)), _mm256_cmp_ps(rh, _mm256_set1_ps(360.0f), _CMP_GT_OQ)); // rh>360 ? rh-360 : rh;
		const VecType rh2 = _mm256_blendv_ps(rh1, _mm256_add_ps(rh, _mm256_set1_ps(360.0f)), _mm256_cmp_ps(rh, _mm256_setzero_ps(), _CMP_LT_OQ)); // rh<0   ? rh+360 : rh1;

		const VecType rm11 = _mm256_fmadd_ps(rmdiff, _mm256_div_ps(rh2, _mm256_set1_ps(60.0f)), rm1); // rm1 + (rm2 - rm1) * rh / 60.0f;
		const VecType rm12 = _mm256_fmadd_ps(rmdiff, _mm256_div_ps(_mm256_sub_ps(_mm256_set1_ps(240.0f), rh), _mm256_set1_ps(60.0f)), rm1); // rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;

		const VecType r1 = _mm256_blendv_ps(rm1, rm12, _mm256_cmp_ps(rh2, _mm256_set1_ps(240.0f), _CMP_LT_OQ)); //_mm512_mask_blend_ps(_mm512_cmplt_ps_mask(rh2, _mm512_set1_ps(240.0f)), rm1, rm12); // if (rh < 240.0f) rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;
		const VecType r2 = _mm256_blendv_ps(r1, rm2, _mm256_cmp_ps(rh2, _mm256_set1_ps(180.0f), _CMP_LT_OQ)); //_mm512_mask_blend_ps(_mm512_cmplt_ps_mask(rh2, _mm512_set1_ps(180.0f)), r1, rm2); // if (rh < 180.0f) rm1 = rm2;
		const VecType r3 = _mm256_blendv_ps(r2, rm11, _mm256_cmp_ps(rh2, _mm256_set1_ps(60.0f), _CMP_LT_OQ)); //_mm512_mask_blend_ps(_mm512_cmplt_ps_mask(rh2, _mm512_set1_ps(60.0f)), r2, rm11); // if (rh < 60.0f) rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;

		return _mm256_mul_ps(r3, _mm256_set1_ps(255.0f)); //_mm512_mul_ps(r3, _mm512_set1_ps(255.0f));
	};

	const auto toRGB = [&rgb1, markOverAndUnderExposure](const VecType h, const VecType s, const VecType l) -> std::tuple<VecType, VecType, VecType>
	{
		const VecType rm21 = _mm256_fmadd_ps(l, s, l); // L + L * S
		const VecType rm22 = _mm256_fnmadd_ps(l, s, _mm256_add_ps(l, s)); // L + S - L * S
		const VecType rm2 = _mm256_blendv_ps(rm22, rm21, _mm256_cmp_ps(l, _mm256_set1_ps(0.5f), _CMP_LE_OQ)); // if (L <= 0.5f) { rm2 = L + L * S; } else { rm2 = L + S - L * S; }
		const VecType rm1 = _mm256_fmsub_ps(_mm256_set1_ps(2.0f), l, rm2); // rm1 = (2.0 * L - rm2);
		const VecType rmdiff = _mm256_sub_ps(rm2, rm1);

		const VecType l255 = _mm256_mul_ps(l, _mm256_set1_ps(255.0f)); // L * 255
		const auto sequ0 = _mm256_cmp_ps(s, _mm256_setzero_ps(), _CMP_EQ_OQ); // S == 0 ?
		const auto notoverexposed = _mm256_cmp_ps(l255, _mm256_set1_ps(255.0f), _CMP_LE_OQ); // L <= 255 ?
		const auto notunderexposed = _mm256_cmp_ps(l255, _mm256_set1_ps(2.0f), _CMP_GT_OQ); // L > 2 ?

		const VecType rResult = _mm256_blendv_ps(rgb1(rm1, rm2, rmdiff, _mm256_add_ps(h, _mm256_set1_ps(120.0f))), l255, sequ0); // if (S == 0) R = L * 255; else R = rgb1(...,h+120);
		const VecType gResult = _mm256_blendv_ps(rgb1(rm1, rm2, rmdiff, h), l255, sequ0); // if (S == 0) G = L * 255; else G = rgb1(...,h);
		const VecType bResult = _mm256_blendv_ps(rgb1(rm1, rm2, rmdiff, _mm256_sub_ps(h, _mm256_set1_ps(120.0f))), l255, sequ0); // if (S == 0) B = L * 255; else B = rgb1(...,h-120);

		return {
			markOverAndUnderExposure ? _mm256_and_ps(notunderexposed,
				_mm256_blendv_ps(_mm256_set1_ps(255.0f), rResult, notoverexposed)
			) : rResult,
			markOverAndUnderExposure ? _mm256_and_ps(_mm256_and_ps(notoverexposed, notunderexposed), gResult) : gResult,
			markOverAndUnderExposure ? _mm256_and_ps(notoverexposed,
				_mm256_blendv_ps(_mm256_set1_ps(255.0f), bResult, notunderexposed)
			) : bResult
		};
	};

	const auto [pr, pg, pb] = this->histoData.getBufferPtr();

	for (size_t n = 0; n < len / VecLen; ++n)
	{
		const auto [r, g, b] = toRGB(_mm256_loadu_ps(pr + n * VecLen), _mm256_loadu_ps(pg + n * VecLen), _mm256_loadu_ps(pb + n * VecLen));
		_mm256_storeu_ps(pr + n * VecLen, r);
		_mm256_storeu_ps(pg + n * VecLen, g);
		_mm256_storeu_ps(pb + n * VecLen, b);
	}

	const __m256i mask = AvxSupport::shiftRightVarEpi8(_mm256_set1_epi32(-1), static_cast<int>(VecLen - len % VecLen) * 4);
	const auto [r, g, b] = toRGB(
		_mm256_maskload_ps(pr + (len / VecLen) * VecLen, mask), // maskload sets non-loaded elements to zero.
		_mm256_maskload_ps(pg + (len / VecLen) * VecLen, mask),
		_mm256_maskload_ps(pb + (len / VecLen) * VecLen, mask)
	);
	_mm256_maskstore_ps(pr + (len / VecLen) * VecLen, mask, r);
	_mm256_maskstore_ps(pg + (len / VecLen) * VecLen, mask, g);
	_mm256_maskstore_ps(pb + (len / VecLen) * VecLen, mask, b);

	return AvxSupport::zeroUpper(0);
}

int Avx256BezierAndSaturation::avxBezierAdjust(const size_t len)
{
	if (!this->histoData.avxSupported)
		return 1;

	using VecType = __m256;
	constexpr size_t VecLen = sizeof(VecType) / sizeof(float);

	float* const pBlue = this->histoData.blueBuffer.data();
	const int nBezier = static_cast<int>(this->histoData.bezierX.size());

	const auto gatherBezierData = [this, pBlue, nBezier](const VecType arg) -> VecType
	{
		const __m256i ndx = avx256LowerBoundPs(this->histoData.bezierX.data(), nBezier, arg);
		return _mm256_mask_i32gather_ps(arg, this->histoData.bezierY.data(), ndx, _mm256_castsi256_ps(_mm256_cmpgt_epi32(_mm256_set1_epi32(nBezier), ndx)), sizeof(float));
	};

	for (size_t n = 0; n < len / VecLen; ++n)
	{
		const VecType values = gatherBezierData(_mm256_loadu_ps(pBlue + n * VecLen));
		_mm256_storeu_ps(pBlue + n * VecLen, values);
	}

	const __m256i mask = AvxSupport::shiftRightVarEpi8(_mm256_set1_epi32(-1), static_cast<int>(VecLen - len % VecLen) * 4);
	const VecType values = gatherBezierData(_mm256_maskload_ps(pBlue + (len / VecLen) * VecLen, mask)); // maskload sets non-loaded elements to zero.
	_mm256_maskstore_ps(pBlue + (len / VecLen) * VecLen, mask, values);

	return AvxSupport::zeroUpper(0);
}

int Avx256BezierAndSaturation::avxBezierSaturation(const size_t len, const float saturationShift)
{
	if (saturationShift == 0)
		return 0;
	if (!this->histoData.avxSupported)
		return 1;

	using VecType = __m256;
	constexpr size_t VecLen = sizeof(VecType) / sizeof(float);

	const auto satShift = [shiftVal = _mm256_set1_ps(saturationShift > 0 ? 10.0f / saturationShift : -0.1f * saturationShift)](const VecType v) -> VecType
	{
		return avxPow(v, shiftVal);
	};

	float* const pGreen = this->histoData.greenBuffer.data();

	for (size_t n = 0; n < len / VecLen; ++n) {
		_mm256_storeu_ps(pGreen + n * VecLen, satShift(_mm256_loadu_ps(pGreen + n * VecLen)));
	}

	const __m256i mask = AvxSupport::shiftRightVarEpi8(_mm256_set1_epi32(-1), static_cast<int>(VecLen - len % VecLen) * 4);
	_mm256_maskstore_ps(pGreen + (len / VecLen) * VecLen, mask, satShift(_mm256_maskload_ps(pGreen + (len / VecLen) * VecLen, mask)));

	return AvxSupport::zeroUpper(0);
}

// ------------------------
// Non AVX Bezier functions
// ------------------------

int NonAvxBezierAndSaturation::avxAdjustRGB(const int nBitmaps, const class DSS::RGBHistogramAdjust& histoAdjust)
{
	const float scale = 255.0f / static_cast<float>(nBitmaps);

	for (size_t n = 0, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		this->histoData.redBuffer[n] *= scale;
		this->histoData.greenBuffer[n] *= scale;
		this->histoData.blueBuffer[n] *= scale;
	}
	for (size_t n = 0, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		double r = this->histoData.redBuffer[n];
		double g = this->histoData.greenBuffer[n];
		double b = this->histoData.blueBuffer[n];
		histoAdjust.Adjust(r, g, b);

		this->histoData.redBuffer[n] = static_cast<float>(r);
		this->histoData.greenBuffer[n] = static_cast<float>(g);
		this->histoData.blueBuffer[n] = static_cast<float>(b);
	}

	constexpr size_t VecLen = sizeof(__m128) / sizeof(float);
	constexpr float scalingFactor = static_cast<float>(1.0 / 255.0);

	// SSE2 code is OK, because every x64 CPU supports it.
	const auto loadAndScale = [scalingVector = _mm_set1_ps(scalingFactor), minVal = _mm_set1_ps(255.0f)](const float* pValue) -> __m128 {
		return _mm_min_ps(_mm_mul_ps(_mm_loadu_ps(pValue), scalingVector), minVal);
	};

	for (size_t n = 0, nVecs = this->histoData.redBuffer.size() / VecLen; n < nVecs; ++n)
	{
		__m128 r = loadAndScale(this->histoData.redBuffer.data() + n * 4);
		__m128 g = loadAndScale(this->histoData.greenBuffer.data() + n * 4);
		__m128 b = loadAndScale(this->histoData.blueBuffer.data() + n * 4);
		_mm_storeu_ps(this->histoData.redBuffer.data() + n * 4,   r);
		_mm_storeu_ps(this->histoData.greenBuffer.data() + n * 4, g);
		_mm_storeu_ps(this->histoData.blueBuffer.data() + n * 4,  b);
	}
	for (size_t n = (this->histoData.redBuffer.size() / VecLen) * VecLen, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		this->histoData.redBuffer[n] = std::min(this->histoData.redBuffer[n] * scalingFactor, 255.0f);
		this->histoData.greenBuffer[n] = std::min(this->histoData.greenBuffer[n] * scalingFactor, 255.0f);
		this->histoData.blueBuffer[n] = std::min(this->histoData.blueBuffer[n] * scalingFactor, 255.0f);
	}

	return 0;
}

int NonAvxBezierAndSaturation::avxToHsl()
{
	for (size_t n = 0, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		double h, s, l;
		ToHSL(this->histoData.redBuffer[n], this->histoData.greenBuffer[n], this->histoData.blueBuffer[n], h, s, l);
		this->histoData.redBuffer[n] = h;
		this->histoData.greenBuffer[n] = s;
		this->histoData.blueBuffer[n] = l;
	}
	return 0;
}

int NonAvxBezierAndSaturation::avxBezierAdjust(const size_t len)
{
	for (size_t n = 0, bufferLen = this->histoData.blueBuffer.size(); n < bufferLen; ++n)
	{
		const auto it = std::lower_bound(this->histoData.bezierX.cbegin(), this->histoData.bezierX.cend(), this->histoData.blueBuffer[n]);
		const auto ndx = it - this->histoData.bezierX.cbegin();
		if (ndx < this->histoData.bezierX.size())
			this->histoData.blueBuffer[n] = this->histoData.bezierY[ndx];
	}
	return 0;
}

int NonAvxBezierAndSaturation::avxBezierSaturation(const size_t len, const float saturationShift)
{
	if (saturationShift == 0)
		return 0;

	const auto satShift = [shiftVal = saturationShift > 0 ? 10.0f / saturationShift : -0.1f * saturationShift](const float v) -> float
	{
		return std::pow(v, shiftVal);
	};

	std::transform(std::cbegin(this->histoData.greenBuffer), std::cend(this->histoData.greenBuffer), std::begin(this->histoData.greenBuffer), satShift);

	return 0;
}

int NonAvxBezierAndSaturation::avxToRgb(const bool markOverAndUnderExposure)
{
	for (size_t n = 0, bufferLen = this->histoData.redBuffer.size(); n < bufferLen; ++n)
	{
		const float l = this->histoData.blueBuffer[n];

		double r, g, b;
		ToRGB(this->histoData.redBuffer[n], this->histoData.greenBuffer[n], l, r, g, b);

		if (markOverAndUnderExposure)
		{
			const bool notoverexposed = l <= 1.0f;
			const bool notunderexposed = l > (2.0f / 255.0f);

			this->histoData.redBuffer[n] = notoverexposed ? (notunderexposed ? r : 0.0f) : 255.0f;
			this->histoData.greenBuffer[n] = (notoverexposed && notunderexposed) ? g : 0.0f;
			this->histoData.blueBuffer[n] = notunderexposed ? (notoverexposed ? b : 0.0f) : 255.0f;
		}
		else
		{
			this->histoData.redBuffer[n] = r;
			this->histoData.greenBuffer[n] = g;
			this->histoData.blueBuffer[n] = b;
		}
	}
	return 0;
}
