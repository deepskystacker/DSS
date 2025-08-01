/****************************************************************************
**
** Copyright (C) 2024, 2025 Martin Toeltsch
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
#include "pch.h"
#include "avx_includes.h"
#include "avx_entropy.h"
#include "avx_support.h"
#include "avx_bitmap_util.h"
#include "avx_cfa.h"
#include "avx_histogram.h"
#include "Multitask.h"

int AvxEntropy::avxCalcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies)
{
	int rval = 1;
	if (doCalcEntropies<std::uint16_t>(squareSize, nSquaresX, nSquaresY, redEntropies, greenEntropies, blueEntropies) == 0
		|| doCalcEntropies<std::uint32_t>(squareSize, nSquaresX, nSquaresY, redEntropies, greenEntropies, blueEntropies) == 0
		|| doCalcEntropies<float>(squareSize, nSquaresX, nSquaresY, redEntropies, greenEntropies, blueEntropies) == 0)
	{
		rval = 0;
	}
	return AvxSupport::zeroUpper(rval);
}

template <class T>
int AvxEntropy::doCalcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies)
{
	// Check input bitmap. 
	const AvxBitmapUtil avxInputSupport{ inputBitmap };
	if (!avxInputSupport.isColorBitmapOfType<T>() && !avxInputSupport.isMonochromeBitmapOfType<T>()) // Monochrome includes CFA 
		return 1;

	constexpr int vectorLen = 16;
	const int width = inputBitmap.Width();
	const int height = inputBitmap.Height();

	const auto getDistribution = [](const auto& histogram, T value) -> float
	{
		constexpr size_t Unsigned_short_max = size_t{ std::numeric_limits<std::uint16_t>::max() };
		if constexpr (std::is_integral<T>::value && sizeof(T) == 4) // 32 bit integral type 
			value >>= 16;
		return static_cast<float>(histogram[std::min(static_cast<size_t>(value), Unsigned_short_max)]);
	};

	const auto calcEntropyOfSquare = [squareSize, width, height, vectorLen, &getDistribution](const int col, const int row, const T* const pColor, auto& histogram) -> EntropyVectorType::value_type
	{
		const int xmin = col * squareSize;
		const int xmax = std::min(xmin + squareSize, width);
		const int nx = xmax - xmin;
		const int ymin = row * squareSize;
		const int ymax = std::min(ymin + squareSize, height);
		const int nrVectors = nx / vectorLen;
		memset(histogram.data(), 0, histogram.size() * sizeof(histogram[0]));

		for (int y = ymin; y < ymax; ++y)
		{
			const T* p = pColor + y * width + xmin;
			for (int n = 0; n < nrVectors; ++n, p += vectorLen)
			{
				const auto [lo, hi] = AvxSupport::read16PackedInt(p);
				Avx256Histogram::calcHistoOfVectorEpi32(lo, histogram);
				Avx256Histogram::calcHistoOfVectorEpi32(hi, histogram);
			}
			// Rest of line
			for (int x = xmin + nrVectors * vectorLen; x < xmax; ++x, ++p)
				Avx256Histogram::addToHisto(histogram, *p);
		}

		const float N = static_cast<float>(nx * (ymax - ymin));
		const float lnN = std::log(N);
		float entropy = 0.0f;
		__m256 avxEntropy = _mm256_setzero_ps();
		const int* const pHisto = histogram.data();

		for (int y = ymin; y < ymax; ++y)
		{
			const T* p = pColor + y * width + xmin;
			for (int n = 0; n < nrVectors; ++n, p += vectorLen)
			{
				const auto [lo, hi] = AvxSupport::read16PackedInt(p);
				const __m256 lh = _mm256_cvtepi32_ps(_mm256_i32gather_epi32(pHisto, lo, 4));
				const __m256 hh = _mm256_cvtepi32_ps(_mm256_i32gather_epi32(pHisto, hi, 4));
				const __m256 r0 = _mm256_fmadd_ps(lh, _mm256_sub_ps(_mm256_set1_ps(lnN), avxLog(lh)), avxEntropy);
				avxEntropy = _mm256_fmadd_ps(hh, _mm256_sub_ps(_mm256_set1_ps(lnN), avxLog(hh)), r0);
			}
			// Rest of line adds to float entropy.
			for (int x = xmin + nrVectors * vectorLen; x < xmax; ++x, ++p)
			{
				const float d = getDistribution(histogram, *p);
				entropy += d * (lnN - std::log(d));
			}
		}
		// Accumulate float entropy and horizontal sum of avxEntropy.
		const __m256 r0 = _mm256_hadd_ps(_mm256_hadd_ps(avxEntropy, _mm256_setzero_ps()), _mm256_setzero_ps()); // ., ., ., e4+e5+e6+e7, ., ., ., e0+e1+e2+e3
		entropy += _mm_cvtss_f32(_mm_add_ps(_mm256_castps256_ps128(r0), _mm256_extractf128_ps(r0, 1)));
		
		return entropy / (N * std::log(2.0f));
	};

	const auto calcEntropy = [nSquaresX, nSquaresY, &calcEntropyOfSquare](const T* const pColor, EntropyVectorType& entropyVector) -> void
	{
#pragma warning (suppress: 4189)
		const int nrEnabledThreads = Multitask::GetNrProcessors(); // Returns 1 if multithreading disabled by user, otherwise # HW threads
		constexpr size_t HistoSize = std::numeric_limits<std::uint16_t>::max() + size_t{ 1 };
		std::vector<int> histogram(HistoSize, 0);

#pragma omp parallel for default(shared) firstprivate(histogram) schedule(dynamic, 50) if(nrEnabledThreads - 1) 
		for (int y = 0; y < nSquaresY; ++y)
		{
			for (int x = 0, ndx = y * nSquaresX; x < nSquaresX; ++x, ++ndx)
			{
				entropyVector[ndx] = calcEntropyOfSquare(x, y, pColor, histogram);
			}
		}
	};

	const bool isCFA = avxInputSupport.isMonochromeCfaBitmapOfType<T>();

	if (avxInputSupport.isColorBitmapOfType<T>() || isCFA)
	{
		AvxCfaProcessing avxCfa{ 0, 0, inputBitmap };
		if (isCFA)
		{
			const size_t lineEnd = inputBitmap.Height();
			avxCfa.init(0, lineEnd);
			avxCfa.interpolate(0, lineEnd, 1);
		}

		const T* pRedPixels = isCFA ? avxCfa.redCfaLine<T>(0) : &avxInputSupport.redPixels<T>().at(0);
		const T* pGreenPixels = isCFA ? avxCfa.greenCfaLine<T>(0) : &avxInputSupport.greenPixels<T>().at(0);
		const T* pBluePixels = isCFA ? avxCfa.blueCfaLine<T>(0) : &avxInputSupport.bluePixels<T>().at(0);

		calcEntropy(pRedPixels, redEntropies);
		calcEntropy(pGreenPixels, greenEntropies);
		calcEntropy(pBluePixels, blueEntropies);

		return 0;
	}

	if (avxInputSupport.isMonochromeBitmapOfType<T>())
	{
		const T* pGrayPixels = &avxInputSupport.grayPixels<T>().at(0);
		calcEntropy(pGrayPixels, redEntropies);

		memcpy(&greenEntropies[0], &redEntropies[0], redEntropies.size() * sizeof(EntropyVectorType::value_type));
		memcpy(&blueEntropies[0], &redEntropies[0], redEntropies.size() * sizeof(EntropyVectorType::value_type));

		return 0;
	}

	return 1;
}
