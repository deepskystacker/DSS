#pragma once
/****************************************************************************
**
** Copyright (C) 2020, 2025 David C. Partridge
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
#include "avx_cfa.h"
#include "avx_simd_factory.h"
#include "histogram.h"

class AvxHistogram
{
public:
	typedef std::vector<int> HistogramVectorType;
private:
	friend class Avx256Histogram;
	friend class NonAvxHistogram;

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
private:
	static constexpr size_t HistogramSize() { return std::numeric_limits<std::uint16_t>::max() + size_t{ 1 }; }
};

class Avx256Histogram
{
private:
	friend class AvxHistogram;

	AvxHistogram& histoData;
	Avx256Histogram(AvxHistogram& hd) : histoData{ hd } {}

	int calcHistogram(const size_t lineStart, const size_t lineEnd, const double);

	template <class T>
	int doCalcHistogram(const size_t lineStart, const size_t lineEnd);
public:
	// Conflict detection: Number of equal elements + blocking mask. 
	static std::tuple<__m256i, std::uint32_t> detectConflictsEpi32(const __m256i a) noexcept;
public:
	template <class T>
	static void calcHistoOfVectorEpi32(const __m256i colorVec, T& histogram);

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


class NonAvxHistogram
{
private:
	friend class AvxHistogram;

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

public:
	explicit AvxBezierAndSaturation(const size_t bufferLen);
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
	int avxAdjustRGB(const int nBitmaps, const DSS::RGBHistogramAdjust& histoAdjust);
	int avxToHsl(const auto& bezierPoints)
	{
		const int rv = this->toHsl();
		return rv == 0 ? this->fillBezierBuffer(bezierPoints) : rv;
	}
	int avxToRgb(const bool markOverAndUnderExposure);
	int avxBezierAdjust(const size_t len);
	int avxBezierSaturation(const size_t len, const float saturationShift);
};


class Avx256BezierAndSaturation
{
private:
	friend class AvxBezierAndSaturation;

	AvxBezierAndSaturation& histoData;
	Avx256BezierAndSaturation(AvxBezierAndSaturation& d) : histoData{ d } {}
public:
	Avx256BezierAndSaturation(const Avx256BezierAndSaturation&) = delete;
	Avx256BezierAndSaturation& operator=(const Avx256BezierAndSaturation&) = delete;
public: // for unit tests
	static __m256i avx256LowerBoundPs(const float* const pValues, const unsigned int N, const __m256 refVal);
private:
	int avxAdjustRGB(const int nBitmaps, const DSS::RGBHistogramAdjust& histoAdjust);
	int avxToHsl();
	int avxToRgb(const bool markOverAndUnderExposure);
	int avxBezierAdjust(const size_t len);
	int avxBezierSaturation(const size_t len, const float saturationShift);
};


class NonAvxBezierAndSaturation
{
private:
	friend class AvxBezierAndSaturation;

	AvxBezierAndSaturation& histoData;
	NonAvxBezierAndSaturation(AvxBezierAndSaturation& d) : histoData{ d } {}
public:
	NonAvxBezierAndSaturation(const NonAvxBezierAndSaturation&) = delete;
	NonAvxBezierAndSaturation& operator=(const NonAvxBezierAndSaturation&) = delete;
public: // for unit tests
	static __m256i avx256LowerBoundPs(const float* const pValues, const unsigned int N, const __m256 refVal);
private:
	int avxAdjustRGB(const int nBitmaps, const DSS::RGBHistogramAdjust& histoAdjust);
	int avxToHsl();
	int avxToRgb(const bool markOverAndUnderExposure);
	int avxBezierAdjust(const size_t len);
	int avxBezierSaturation(const size_t len, const float saturationShift);
};
