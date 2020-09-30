#include "StdAfx.h"
#include "avx_filter.h"
#include "avx.h"
#include <tuple>

template <>
AvxImageFilter<double>::AvxImageFilter(CInternalMedianFilterEngineT<double>* filEng) :
	filterEngine{ filEng }
{
}

template <class T>
AvxImageFilter<T>::AvxImageFilter(CInternalMedianFilterEngineT<T>* filEng) :
	filterEngine{ nullptr }
{
}

template <class T>
int AvxImageFilter<T>::filter(const size_t lineStart, const size_t lineEnd)
{
	if constexpr (!std::is_same<T, double>::value)
		return 1;
	if (!AvxSupport::checkCpuFeatures())
		return 1;
	if (filterEngine == nullptr)
		return 1;
	if (filterEngine->m_lFilterSize != 1)
		return 1;
	if (filterEngine->m_CFAType != CFATYPE_NONE)
		return 1;

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
	const auto getPrevAndNext = [](const __m256 vector, const double* const p, __m256& prev, __m256& next) -> void
	{
		const __m128 newElements = _mm256_cvtpd_ps(_mm256_i32gather_pd(p, _mm_setr_epi32(-1, 0, 0, 8), 8));
//		const __m128 prevElement = _mm256_cvtpd_ps(_mm256_broadcast_sd(p - 1));
//		const __m128 nextElement = _mm256_cvtpd_ps(_mm256_broadcast_sd(p + 8));
		const __m256 shiftedLeft = _mm256_permutevar8x32_ps(vector, _mm256_setr_epi32(0, 0, 1, 2, 3, 4, 5, 6));
		const __m256 shiftedRight = _mm256_permutevar8x32_ps(vector, _mm256_setr_epi32(1, 2, 3, 4, 5, 6, 7, 7));
		prev = _mm256_blend_ps(shiftedLeft, _mm256_castps128_ps256(newElements), 0x01);
		next = _mm256_blend_ps(shiftedRight, _mm256_insertf128_ps(_mm256_setzero_ps(), newElements, 1), 0x80);
	};
	const auto advancePointersAndVectors = [&](const size_t n) -> void
	{
		std::advance(pIn, n);
		std::advance(pInPrev, n);
		std::advance(pInNext, n);
		std::advance(pOut, n);

		thisLine = load8Ps(pIn);
		getPrevAndNext(thisLine, pIn, thisLinePrev, thisLineNext);
//		thisLinePrev = load8Ps(pIn - 1);
//		thisLineNext = load8Ps(pIn + 1);

		prevLine = load8Ps(pInPrev);
		getPrevAndNext(prevLine, pInPrev, prevLinePrev, prevLineNext);
//		prevLinePrev = load8Ps(pInPrev - 1);
//		prevLineNext = load8Ps(pInPrev + 1);

		nextLine = load8Ps(pInNext);
		getPrevAndNext(nextLine, pInNext, nextLinePrev, nextLineNext);
//		nextLinePrev = load8Ps(pInNext - 1);
//		nextLineNext = load8Ps(pInNext + 1);
	};
	const auto storeMedian = [&pOut](const __m256 median) -> void
	{
		_mm256_storeu_pd(pOut, _mm256_cvtps_pd(_mm256_castps256_ps128(median)));
		_mm256_storeu_pd(pOut + 4, _mm256_cvtps_pd(_mm256_extractf128_ps(median, 1)));
	};
	const auto lastVectorsOfLine = [&]() -> void
	{
		if (nrVectors * 8 == width)
		{
			thisLineNext = thisLine;
			prevLineNext = prevLine;
			nextLineNext = nextLine;
		}
	};

	for (size_t row = lineStart; row < lineEnd; ++row)
	{
		thisLine = load8Ps(pIn);
		thisLinePrev = thisLine;
		thisLineNext = load8Ps(pIn + 1);
		prevLine = load8Ps(pInPrev);
		prevLinePrev = prevLine;
		prevLineNext = load8Ps(pInPrev + 1);
		nextLine = load8Ps(pInNext);
		nextLinePrev = nextLine;
		nextLineNext = load8Ps(pInNext + 1);

		for (size_t counter = 0; counter < nrVectors - 1; ++counter)
		{
			const __m256 median = median9(prevLinePrev, prevLine, prevLineNext, thisLinePrev, thisLine, thisLineNext, nextLinePrev, nextLine, nextLineNext);
			storeMedian(median);
			advancePointersAndVectors(8);
		}
		// Last vector
		lastVectorsOfLine();
		const __m256 median = median9(prevLinePrev, prevLine, prevLineNext, thisLinePrev, thisLine, thisLineNext, nextLinePrev, nextLine, nextLineNext);
		storeMedian(median);

		const size_t remainingPixels = width - nrVectors * 8;
		if (remainingPixels > 0)
		{
			advancePointersAndVectors(remainingPixels);
			const __m256 median = median9(prevLinePrev, prevLine, prevLineNext, thisLinePrev, thisLine, thisLineNext, nextLinePrev, nextLine, nextLineNext);
			for (size_t n = nrVectors * 8, ndx = 0; n < width; ++n, ++ndx, ++pOut)
				pOut[n] = static_cast<double>(median.m256_f32[ndx]);
		}

		pOut = filterEngine->m_pvOutValues + (row + 1) * width;
		pInPrev = filterEngine->m_pvInValues + row * width;
		pIn = pInPrev + width;
		if (row == height - 2) // Next round will be the last line of the bitmap
			pInNext = pIn;
		else
			pInNext = pIn + width;
	}

	return 0;
}

// Explicit template instantiation for the types we need.
template AvxImageFilter<unsigned char>;
template AvxImageFilter<unsigned short>;
template AvxImageFilter<unsigned long>;
template AvxImageFilter<float>;
template AvxImageFilter<double>;
