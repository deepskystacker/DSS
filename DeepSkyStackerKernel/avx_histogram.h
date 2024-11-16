#pragma once
#include "avx_cfa.h"
#include "avx_support.h"
#include "histogram.h"

class AvxHistogram
{
public:
	typedef std::vector<int> HistogramVectorType;
private:
	friend class Avx256Histogram;
	friend class NonAvxHistogram;

	bool avxReady;
	HistogramVectorType redHisto;
	HistogramVectorType greenHisto;
	HistogramVectorType blueHisto;
	AvxCfaProcessing avxCfa;
	const CMemoryBitmap& inputBitmap;
public:
	AvxHistogram() = delete;
	AvxHistogram(const CMemoryBitmap& inputbm);
	AvxHistogram(const AvxHistogram&) = default;
	AvxHistogram(AvxHistogram&&) = delete;
	AvxHistogram& operator=(const AvxHistogram&) = delete;

	int calcHistogram(const size_t lineStart, const size_t lineEnd, const double multiplier);
	int mergeHistograms(HistogramVectorType& red, HistogramVectorType& green, HistogramVectorType& blue);
	inline bool isAvxReady() const { return this->avxReady; }
private:
	static constexpr size_t HistogramSize() { return std::numeric_limits<std::uint16_t>::max() + size_t{ 1 }; }
};

class Avx256Histogram : public SimdFactory<Avx256Histogram>
{
private:
	friend class AvxHistogram;
	friend class SimdFactory<Avx256Histogram>;

	AvxHistogram& histoData;
	Avx256Histogram(AvxHistogram& hd) : histoData{ hd } {}

	int calcHistogram(const size_t lineStart, const size_t lineEnd, const double);

	template <class T>
	int doCalcHistogram(const size_t lineStart, const size_t lineEnd);
public:
	// Conflict detection: Number of equal elements + blocking mask. 
	inline static std::tuple<__m256i, std::uint32_t> detectConflictsEpi32(const __m256i a) noexcept
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
public:
	template <class T>
	inline static void calcHistoOfVectorEpi32(const __m256i colorVec, T& histogram)
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

	template <class H, class T>
	inline static void addToHisto(H& histo, T grayValue)
	{
		if constexpr (std::is_same<T, double>::value)
			grayValue *= 256.0;
		if constexpr (std::is_integral<T>::value && sizeof(T) == 4) // 32 bit integral type 
			grayValue >>= 16;
		constexpr size_t Unsigned_short_max = size_t{ std::numeric_limits<std::uint16_t>::max() };
		++histo[std::min(static_cast<size_t>(grayValue), Unsigned_short_max)];
	};
};


class NonAvxHistogram : public SimdFactory<NonAvxHistogram>
{
private:
	friend class AvxHistogram;
	friend class SimdFactory<NonAvxHistogram>;

	AvxHistogram& histoData;
	NonAvxHistogram(AvxHistogram& hd) : histoData{ hd } {}

	int calcHistogram(const size_t lineStart, const size_t lineEnd, const double multiplier);
};


// ---------------------------
// For the image colour editor
// ---------------------------

class AvxBezierAndSaturation
{
private:
	friend class Avx256BezierAndSaturation;
	friend class NonAvxBezierAndSaturation;

	std::vector<float> redBuffer;
	std::vector<float> greenBuffer;
	std::vector<float> blueBuffer;
	std::vector<float> bezierX;
	std::vector<float> bezierY;
	bool avxSupported;

public:
	AvxBezierAndSaturation(const size_t bufferLen) :
		avxSupported{ AvxSupport::checkSimdAvailability() },
		redBuffer(bufferLen), greenBuffer(bufferLen), blueBuffer(bufferLen),
		bezierX{}, bezierY{}
	{}
	AvxBezierAndSaturation(const AvxBezierAndSaturation&) = default;
	AvxBezierAndSaturation(AvxBezierAndSaturation&&) = delete;
	AvxBezierAndSaturation& operator=(const AvxBezierAndSaturation&) = delete;
private:
	int fillBezierBuffer(const auto& bezierPoints)
	{
		if (this->bezierX.empty())
		{
			this->bezierX.resize(bezierPoints.size());
			this->bezierY.resize(bezierPoints.size());
		}
		std::transform(bezierPoints.cbegin(), bezierPoints.cend(), this->bezierX.begin(), [](const auto& bcp) { return static_cast<float>(bcp.x); });
		std::transform(bezierPoints.cbegin(), bezierPoints.cend(), this->bezierY.begin(), [](const auto& bcp) { return static_cast<float>(bcp.y); });
		return 0;
	}
	int toHsl();
public:
	void copyData(const float* const pRedPixel, const float* const pGreenPixel, const float* const pBluePixel, const size_t bufferLen, const bool monochrome);
	std::tuple<float*, float*, float*> getBufferPtr();
	int avxAdjustRGB(const int nBitmaps, const class DSS::RGBHistogramAdjust& histoAdjust);
	int avxToHsl(const auto& bezierPoints)
	{
		const int rv = this->toHsl();
		return rv == 0 ? this->fillBezierBuffer(bezierPoints) : rv;
	}
	int avxToRgb(const bool markOverAndUnderExposure);
	int avxBezierAdjust(const size_t len);
	int avxBezierSaturation(const size_t len, const float saturationShift);
};


class Avx256BezierAndSaturation : public SimdFactory<Avx256BezierAndSaturation>
{
private:
	friend class AvxBezierAndSaturation;
	friend class SimdFactory<Avx256BezierAndSaturation>;

	AvxBezierAndSaturation& histoData;
	Avx256BezierAndSaturation(AvxBezierAndSaturation& d) : histoData{ d } {}
public:
	Avx256BezierAndSaturation(const Avx256BezierAndSaturation&) = delete;
	Avx256BezierAndSaturation& operator=(const Avx256BezierAndSaturation&) = delete;
public: // for unit tests
	static __m256i avx256LowerBoundPs(const float* const pValues, const unsigned int N, const __m256 refVal);
private:
	int avxAdjustRGB(const int nBitmaps, const class RGBHistogramAdjust& histoAdjust);
	int avxToHsl();
	int avxToRgb(const bool markOverAndUnderExposure);
	int avxBezierAdjust(const size_t len);
	int avxBezierSaturation(const size_t len, const float saturationShift);
};


class NonAvxBezierAndSaturation : public SimdFactory<NonAvxBezierAndSaturation>
{
private:
	friend class AvxBezierAndSaturation;
	friend class SimdFactory<NonAvxBezierAndSaturation>;

	AvxBezierAndSaturation& histoData;
	NonAvxBezierAndSaturation(AvxBezierAndSaturation& d) : histoData{ d } {}
public:
	NonAvxBezierAndSaturation(const NonAvxBezierAndSaturation&) = delete;
	NonAvxBezierAndSaturation& operator=(const NonAvxBezierAndSaturation&) = delete;
public: // for unit tests
	static __m256i avx256LowerBoundPs(const float* const pValues, const unsigned int N, const __m256 refVal);
private:
	int avxAdjustRGB(const int nBitmaps, const class RGBHistogramAdjust& histoAdjust);
	int avxToHsl();
	int avxToRgb(const bool markOverAndUnderExposure);
	int avxBezierAdjust(const size_t len);
	int avxBezierSaturation(const size_t len, const float saturationShift);
};
