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
#include "pch.h"
#include "avx_includes.h"
#include "avx_filter.h"
#include "avx_support.h"
#include "MedianFilterEngine.h"

template <class T>
int AvxImageFilter<T>::avxFilter(const size_t lineStart, const size_t lineEnd)
{
	const auto cmpLT = [](__m256& a, __m256& b) -> void
	{
		const __m256 mask = _mm256_cmp_ps(a, b, 17); // 17: CMP_LT_OQ -> IF (a < b) mask=1 ELSE mask=0
		const __m256 smaller = _mm256_blendv_ps(b, a, mask); // IF (mask==1) TAKE second arg -> a < b ? a : b -> the smaller elements
		b = _mm256_blendv_ps(a, b, mask);  // a < b ? b : a -> the larger elements
		a = smaller;
	};

	const auto median9 = [&cmpLT](__m256 x0, __m256 x1, __m256 x2, __m256 x3, __m256 x4, __m256 x5, __m256 x6, __m256 x7, __m256 x8) -> __m256
	{
		cmpLT(x0, x1);

		cmpLT(x0, x2);
		cmpLT(x1, x2);

		cmpLT(x0, x3);
		cmpLT(x1, x3);
		cmpLT(x2, x3);

		cmpLT(x0, x4);
		cmpLT(x1, x4);
		cmpLT(x2, x4);
		cmpLT(x3, x4);

		cmpLT(x0, x5);
		cmpLT(x1, x5);
		cmpLT(x2, x5);
		cmpLT(x3, x5);
		cmpLT(x4, x5);

		cmpLT(x0, x6);
		cmpLT(x1, x6);
		cmpLT(x2, x6);
		cmpLT(x3, x6);
		cmpLT(x4, x6);
//		cmpLT(x5, x6);

		cmpLT(x0, x7);
		cmpLT(x1, x7);
		cmpLT(x2, x7);
		cmpLT(x3, x7);
		cmpLT(x4, x7);
//		cmpLT(x5, x7);
//		cmpLT(x6, x7);

		cmpLT(x0, x8);
		cmpLT(x1, x8);
		cmpLT(x2, x8);
		cmpLT(x3, x8);
		cmpLT(x4, x8);
//		cmpLT(x5, x8);
//		cmpLT(x6, x8);
//		cmpLT(x7, x8);

		return x4;
	};

	const size_t width = filterEngine->m_lWidth;
	const size_t height = filterEngine->m_lHeight;
	const size_t nrVectors = width / 8;

	const double* pIn{ filterEngine->m_pvInValues + lineStart * width };
	const double* pInPrev = (lineStart == 0) ? pIn : (pIn - width);
	const double* pInNext = (lineStart == height - 1) ? pIn : (pIn + width);
	double* pOut{ filterEngine->m_pvOutValues + lineStart * width };

	__m256 thisLine, thisLinePrev, thisLineNext;
	__m256 prevLine, prevLinePrev, prevLineNext;
	__m256 nextLine, nextLinePrev, nextLineNext;

	const auto load8Ps = [](const double *const pd) -> __m256
	{
		const __m128 lo = _mm256_cvtpd_ps(_mm256_loadu_pd(pd));
		const __m128 hi = _mm256_cvtpd_ps(_mm256_loadu_pd(pd + 4));
		return _mm256_set_m128(hi, lo);
	};
	const auto getPrevAndNext = [](const __m256 vector, const double* const p, __m256& prev, __m256& next, const __m128i elementIndices) -> void
	{
		const __m128 newElements = _mm256_cvtpd_ps(_mm256_i32gather_pd(p, elementIndices, 8));
//		const __m128 prevElement = _mm256_cvtpd_ps(_mm256_broadcast_sd(p - 1));
//		const __m128 nextElement = _mm256_cvtpd_ps(_mm256_broadcast_sd(p + 8));
		const __m256 shiftedLeft = _mm256_permutevar8x32_ps(vector, _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6));
		const __m256 shiftedRight = _mm256_permutevar8x32_ps(vector, _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 7));
		prev = _mm256_blend_ps(shiftedLeft, _mm256_castps128_ps256(newElements), 0x01);
		next = _mm256_blend_ps(shiftedRight, _mm256_insertf128_ps(_mm256_setzero_ps(), newElements, 1), 0x80);
	};
	const auto advancePointersAndVectors = [&](const size_t n, const __m128i elementIndices) -> void
	{
		std::advance(pIn, n);
		std::advance(pInPrev, n);
		std::advance(pInNext, n);
		std::advance(pOut, n);

		thisLine = load8Ps(pIn);
		getPrevAndNext(thisLine, pIn, thisLinePrev, thisLineNext, elementIndices);

		prevLine = load8Ps(pInPrev);
		getPrevAndNext(prevLine, pInPrev, prevLinePrev, prevLineNext, elementIndices);

		nextLine = load8Ps(pInNext);
		getPrevAndNext(nextLine, pInNext, nextLinePrev, nextLineNext, elementIndices);
	};
	const auto storeMedian = [&pOut](const __m256 median) -> void
	{
		_mm256_storeu_pd(pOut, _mm256_cvtps_pd(_mm256_castps256_ps128(median)));
		_mm256_storeu_pd(pOut + 4, _mm256_cvtps_pd(_mm256_extractf128_ps(median, 1)));
	};

	for (size_t row = lineStart; row < lineEnd; ++row)
	{
		// First column
		thisLine = load8Ps(pIn);
		getPrevAndNext(thisLine, pIn, thisLinePrev, thisLineNext, _mm_setr_epi32(0, 0, 0, 8));
		prevLine = load8Ps(pInPrev);
		getPrevAndNext(prevLine, pInPrev, prevLinePrev, prevLineNext, _mm_setr_epi32(0, 0, 0, 8));
		nextLine = load8Ps(pInNext);
		getPrevAndNext(nextLine, pInNext, nextLinePrev, nextLineNext, _mm_setr_epi32(0, 0, 0, 8));

		__m256 median = median9(prevLinePrev, prevLine, prevLineNext, thisLinePrev, thisLine, thisLineNext, nextLinePrev, nextLine, nextLineNext);
		storeMedian(median);

		// Main loop
		for (size_t counter = 1; counter < nrVectors - 1; ++counter)
		{
			advancePointersAndVectors(8, _mm_setr_epi32(-1, 0, 0, 8));
			median = median9(prevLinePrev, prevLine, prevLineNext, thisLinePrev, thisLine, thisLineNext, nextLinePrev, nextLine, nextLineNext);
			storeMedian(median);
		}

		const size_t remainingPixels = width - nrVectors * 8;

		// Last vector
		const __m128i lastIndex = remainingPixels != 0 ? _mm_setr_epi32(-1, 0, 0, 8) : _mm_setr_epi32(-1, 0, 0, 7); // Load following pixel only if there is one.
		advancePointersAndVectors(8, lastIndex);
		median = median9(prevLinePrev, prevLine, prevLineNext, thisLinePrev, thisLine, thisLineNext, nextLinePrev, nextLine, nextLineNext);
		storeMedian(median);

		// Last few pixels
		if (remainingPixels != 0)
		{
			advancePointersAndVectors(remainingPixels, _mm_setr_epi32(-1, 0, 0, 7));
			median = median9(prevLinePrev, prevLine, prevLineNext, thisLinePrev, thisLine, thisLineNext, nextLinePrev, nextLine, nextLineNext);
			for (size_t n = 8 - remainingPixels; n < 8; ++n)
				pOut[n] = static_cast<double>(accessSimdElementConst(median, n));
		}

		pOut = filterEngine->m_pvOutValues + (row + 1) * width;
		pInPrev = filterEngine->m_pvInValues + row * width;
		pIn = pInPrev + width;
		pInNext = (row == height - 2) ? pIn : (pIn + width); // If next round will be the last line of the bitmap -> pNext = pIn
	}

	return AvxSupport::zeroUpper(0);
}

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4661)
#endif
// Explicit template instantiation for the types we need.
template class AvxImageFilter<std::uint8_t>;
template class AvxImageFilter<std::uint16_t>;
template class AvxImageFilter<std::uint32_t>;
template class AvxImageFilter<float>;
template class AvxImageFilter<double>;
#if defined(_MSC_VER)
#pragma warning( pop )
#endif
