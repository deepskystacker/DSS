#include "StdAfx.h"
#include "avx_output.h"
#include "avx.h"

#if defined(AVX_INTRINSICS) && defined(_M_X64)

#include <immintrin.h>

AvxOutputComposition::AvxOutputComposition(CMultiBitmap& mBitmap, CMemoryBitmap* pOut) :
	inputBitmap{ mBitmap },
	pOutputBitmap{ dynamic_cast<C96BitFloatColorBitmap*>(pOut) },
	avxReady{ pOutputBitmap != nullptr }
{
	if (!AvxStacking::checkCpuFeatures())
		avxReady = false;
	// Homogenization not implemented with AVX
	if (inputBitmap.GetHomogenization())
		avxReady = false;
	// Input must be a RGB color bitmap with unsigned shorts
	if (inputBitmap.GetNrChannels() != 3 || inputBitmap.GetNrBytesPerChannel() != 2)
		avxReady = false;
	// We cannot consider more than 65536 bitmaps, because 16 bit unsigned short is used for N.
	if (inputBitmap.GetNrAddedBitmaps() > 0x0ffff)
		avxReady = false;
	// Output must be float values
	if (pOut->BitPerSample() != 32 || !pOut->IsFloat() || !pOut->isTopDown())
		avxReady = false;
}

int AvxOutputComposition::compose(const int line, std::vector<void*> const& lineAddresses)
{
	if (!avxReady)
		return 1;
	// If this is not equal, something went wrong and we cannot continue without risking access violations.
	if (lineAddresses.size() != inputBitmap.GetNrAddedBitmaps())
		return 1;

	switch (inputBitmap.GetProcessingMethod())
	{
		case MBP_SIGMACLIP: return processKappaSigma(line, lineAddresses);
		case MBP_AUTOADAPTIVE: return processAutoAdaptiveWeightedAverage(line, lineAddresses);
		default: return 2;
	}

	return 2;
}

int AvxOutputComposition::processKappaSigma(const int line, std::vector<void*> const& lineAddresses)
{
	const auto parameters = inputBitmap.GetProcessingParameters();

	const int width = pOutputBitmap->RealWidth();
	const int nrVectors = width / 16;

	const auto ps2epu16 = [](const __m256 x1, const __m256 x2) -> __m256i
	{
		return _mm256_permute4x64_epi64(_mm256_packus_epi32(_mm256_cvtps_epi32(x1), _mm256_cvtps_epi32(x2)), 0xd8);
	};
	const auto accumulateSquared = [](const __m256d accumulator, const __m128 colorValue) -> __m256d
	{
		const __m256d pd = _mm256_cvtps_pd(colorValue);
		return _mm256_fmadd_pd(pd, pd, accumulator);
	};
	const auto sigma = [](const __m256 sum, const __m256d sumSqLo, const __m256d sumSqHi, const __m256 N) -> __m256
	{
		// Sigma² = sumSquared / N - µ² = 1/N * (sumSquared - sum² / N)
		const __m256d Nlo = _mm256_cvtps_pd(_mm256_extractf128_ps(N, 0));
		const __m256d Nhi = _mm256_cvtps_pd(_mm256_extractf128_ps(N, 1));
		const __m256d sumLo = _mm256_cvtps_pd(_mm256_extractf128_ps(sum, 0));
		const __m256d sumHi = _mm256_cvtps_pd(_mm256_extractf128_ps(sum, 1));
		const __m256d sigmaSqLoN = _mm256_sub_pd(sumSqLo, _mm256_div_pd(_mm256_mul_pd(sumLo, sumLo), Nlo));
		const __m256d sigmaSqHiN = _mm256_sub_pd(sumSqHi, _mm256_div_pd(_mm256_mul_pd(sumHi, sumHi), Nhi));
		const __m256 sigmaSqN = _mm256_insertf128_ps(_mm256_castps128_ps256(_mm256_cvtpd_ps(sigmaSqLoN)), _mm256_cvtpd_ps(sigmaSqHiN), 1);
		return _mm256_sqrt_ps(_mm256_div_ps(sigmaSqN, N));
	};
	const auto sigmakappa = [&parameters](const __m256 sigma) -> __m256
	{
		return _mm256_mul_ps(sigma, _mm256_set1_ps(static_cast<float>(std::get<0>(parameters))));
	};
	const auto cmpGtEpu16 = [](const __m256i a, const __m256i b) -> __m256i
	{
		const __m256i highBit = _mm256_set1_epi16((short)0x8000);
		return _mm256_cmpgt_epi16(_mm256_xor_si256(a, highBit), _mm256_xor_si256(b, highBit));
	};

	const auto kappaSigmaLoop = [&](float* pOut, const int colorOffset) -> void
	{
		for (int counter = 0; counter < nrVectors; ++counter, pOut += 16)
		{
			__m256i lowerBound = _mm256_set1_epi16(unsigned short(1)); // Values of zero are ignored.
			__m256i upperBound = _mm256_set1_epi16(std::numeric_limits<WORD>::max());
			__m256 my1, my2;

			for (int iteration = 0; iteration < std::get<1>(parameters); ++iteration)
			{
				__m256 sum1 = _mm256_setzero_ps();
				__m256 sum2 = _mm256_setzero_ps();
				__m256i N = _mm256_setzero_si256();
				__m256d sumSq1 = _mm256_setzero_pd();
				__m256d sumSq2 = _mm256_setzero_pd();
				__m256d sumSq3 = _mm256_setzero_pd();
				__m256d sumSq4 = _mm256_setzero_pd();
				// Loop over the light frames
				for (auto frameAddress : lineAddresses)
				{
					const WORD* pColor = static_cast<WORD*>(frameAddress) + counter * 16ULL + colorOffset;
					const __m256i colorValue = _mm256_loadu_si256((const __m256i*)pColor);
					const __m256i outOfRangeMask = _mm256_or_si256(cmpGtEpu16(lowerBound, colorValue), cmpGtEpu16(colorValue, upperBound)); // 16 x 16 bit mask. One where value out of range.
					const __m256i effectiveValue = _mm256_andnot_si256(outOfRangeMask, colorValue); // Zero if outside (µ +- kappa*sigma).
					const __m256 lo8 = AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(effectiveValue, 0));
					const __m256 hi8 = AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(effectiveValue, 1));
					sum1 = _mm256_add_ps(sum1, lo8);
					sum2 = _mm256_add_ps(sum2, hi8);
					sumSq1 = accumulateSquared(sumSq1, _mm256_extractf128_ps(lo8, 0));
					sumSq2 = accumulateSquared(sumSq2, _mm256_extractf128_ps(lo8, 1));
					sumSq3 = accumulateSquared(sumSq3, _mm256_extractf128_ps(hi8, 0));
					sumSq4 = accumulateSquared(sumSq4, _mm256_extractf128_ps(hi8, 1));
					N = _mm256_adds_epu16(N, _mm256_blendv_epi8(_mm256_set1_epi16((short)1), _mm256_setzero_si256(), outOfRangeMask));
				}
				// Calc the new averages
				const __m256 N1 = AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(N, 0));
				const __m256 N2 = AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(N, 1));
				my1 = _mm256_blendv_ps(_mm256_div_ps(sum1, N1), _mm256_setzero_ps(), _mm256_cmp_ps(N1, _mm256_setzero_ps(), 0)); // Low 8 floats. Set 0 where N==0.
				my2 = _mm256_blendv_ps(_mm256_div_ps(sum2, N2), _mm256_setzero_ps(), _mm256_cmp_ps(N2, _mm256_setzero_ps(), 0)); // Hi 8 floats. Set 0 where N==0.
				// Update lower and upper bound with new µ +- kappa * sigma
				const __m256 sigma1 = sigma(sum1, sumSq1, sumSq2, N1);
				const __m256 sigma2 = sigma(sum2, sumSq3, sumSq4, N2);
				const __m256 sigmakappa1 = sigmakappa(sigma1);
				const __m256 sigmakappa2 = sigmakappa(sigma2);
				const __m256 upper1 = _mm256_add_ps(my1, sigmakappa1); // µ + sigma * kappa
				const __m256 upper2 = _mm256_add_ps(my2, sigmakappa2);
				const __m256 lower1 = _mm256_sub_ps(my1, sigmakappa1); // µ - sigma * kappa
				const __m256 lower2 = _mm256_sub_ps(my2, sigmakappa2);
				const __m256i noValuesMask = _mm256_cmpeq_epi16(N, _mm256_setzero_si256()); // 16 x 16 bit mask. One where N==0.
				lowerBound = _mm256_blendv_epi8(ps2epu16(lower1, lower2), _mm256_set1_epi16(unsigned short(1)), noValuesMask); // Set 1 where N==0.
				upperBound = _mm256_blendv_epi8(ps2epu16(upper1, upper2), _mm256_setzero_si256(), noValuesMask); // Set 0 where N==0.
			}
			_mm256_storeu_ps(pOut + static_cast<size_t>(line) * pOutputBitmap->Width(), my1);
			_mm256_storeu_ps(pOut + static_cast<size_t>(line) * pOutputBitmap->Width() + 8, my2);
		}
		// Rest of line
		for (int n = nrVectors * 16; n < width; ++n, ++pOut)
		{
			auto lowerBound = static_cast<unsigned short>(1);
			auto upperBound = std::numeric_limits<unsigned short>::max();
			float my;

			for (int iteration = 0; iteration < std::get<1>(parameters); ++iteration)
			{
				float sum = 0.0f;
				float N = 0.0f;
				float sumSq = 0.0f;
				for (auto frameAddress : lineAddresses)
				{
					const WORD* pColor = static_cast<WORD*>(frameAddress) + n + colorOffset;
					const WORD colorValue = *pColor;
					if (colorValue >= lowerBound && colorValue <= upperBound) {
						const float value = static_cast<float>(colorValue);
						sum += value;
						sumSq += value * value;
						++N;
					}
				}
				my = (N == 0.0f ? 0.0f : (sum / N));
				if (N == 0.0f || N == static_cast<float>(lineAddresses.size()))
					break;
				const float sigma = sqrtf(sumSq / N - my * my);
				const float sigmakappa = sigma * static_cast<float>(std::get<0>(parameters));
				lowerBound = static_cast<unsigned short>(my - sigmakappa);
				upperBound = static_cast<unsigned short>(my + sigmakappa);
			}
			*(pOut + static_cast<size_t>(line) * pOutputBitmap->Width()) = my;
		}
	};

	kappaSigmaLoop(&pOutputBitmap->m_Red.m_vPixels[0], 0);
	kappaSigmaLoop(&pOutputBitmap->m_Green.m_vPixels[0], width);
	kappaSigmaLoop(&pOutputBitmap->m_Blue.m_vPixels[0], width * 2);

	return 0;
}

int AvxOutputComposition::processAutoAdaptiveWeightedAverage(const int line, std::vector<void*> const& lineAddresses)
{
	const int nIterations = std::get<1>(inputBitmap.GetProcessingParameters());
	const int width = pOutputBitmap->RealWidth();
	const int nrVectors = width / 16;
	const __m256 N = _mm256_set1_ps(static_cast<float>(lineAddresses.size()));

	const auto autoAdaptLoop = [&](float* pOut, const int colorOffset) -> void
	{
		// Loop over the pixels of the row, process 16 at a time.
		for (int counter = 0; counter < nrVectors; ++counter, pOut += 16)
		{
			__m256 my1 = _mm256_setzero_ps();
			__m256 my2 = _mm256_setzero_ps();

			// Calculate initial (unweighted) mean.
			for (auto frameAddress : lineAddresses)
			{
				const WORD* pColor = static_cast<WORD*>(frameAddress) + counter * 16ULL + colorOffset;
				const __m256i colorValue = _mm256_loadu_si256((const __m256i*)pColor);
				my1 = _mm256_add_ps(my1, AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(colorValue, 0)));
				my2 = _mm256_add_ps(my2, AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(colorValue, 1)));
			}
			my1 = _mm256_div_ps(my1, N);
			my2 = _mm256_div_ps(my2, N);

			for (int iteration = 0; iteration < nIterations; ++iteration)
			{
				__m256 S1 = _mm256_setzero_ps();
				__m256 S2 = _mm256_setzero_ps();

				// Calculate sigma² related to µ of last iteration.
				for (auto frameAddress : lineAddresses)
				{
					const WORD* pColor = static_cast<WORD*>(frameAddress) + counter * 16ULL + colorOffset;
					const __m256i colorValue = _mm256_loadu_si256((const __m256i*)pColor);
					const __m256 d1 = _mm256_sub_ps(AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(colorValue, 0)), my1);
					const __m256 d2 = _mm256_sub_ps(AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(colorValue, 1)), my2);
					S1 = _mm256_fmadd_ps(d1, d1, S1); // Sum of (x-µ)²
					S2 = _mm256_fmadd_ps(d2, d2, S2);
				}
				const __m256 sigmaSq1 = _mm256_div_ps(S1, N); // sigma² = sum(x-µ)² / N
				const __m256 sigmaSq2 = _mm256_div_ps(S2, N);

				// Calculate new µ using current sigma².
				__m256 W1 = _mm256_setzero_ps();
				__m256 W2 = _mm256_setzero_ps();
				S1 = _mm256_setzero_ps();
				S2 = _mm256_setzero_ps();
				for (auto frameAddress : lineAddresses)
				{
					const WORD* pColor = static_cast<WORD*>(frameAddress) + counter * 16ULL + colorOffset;
					const __m256i colorValue = _mm256_loadu_si256((const __m256i*)pColor);
					const __m256 lo8 = AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(colorValue, 0));
					const __m256 hi8 = AvxStacking::wordToPackedFloat(_mm256_extracti128_si256(colorValue, 1));
					const __m256 d1 = _mm256_sub_ps(lo8, my1); // x-µ
					const __m256 d2 = _mm256_sub_ps(hi8, my2);
					const __m256 denominator1 = _mm256_fmadd_ps(d1, d1, sigmaSq1); // sigma² + (x-µ)²
					const __m256 denominator2 = _mm256_fmadd_ps(d2, d2, sigmaSq2);
					const __m256 weight1 = _mm256_blendv_ps(_mm256_div_ps(sigmaSq1, denominator1), _mm256_set1_ps(1.0f), _mm256_cmp_ps(denominator1, _mm256_setzero_ps(), 0)); // sigma² / (sigma² + (x-µ)²) = 1 / (1 + (x-µ)²/sigma²)
					const __m256 weight2 = _mm256_blendv_ps(_mm256_div_ps(sigmaSq2, denominator2), _mm256_set1_ps(1.0f), _mm256_cmp_ps(denominator2, _mm256_setzero_ps(), 0)); // Set weight to 1 when sigma==0.
					W1 = _mm256_add_ps(W1, weight1); // W = sum(weights)
					W2 = _mm256_add_ps(W2, weight2);
					S1 = _mm256_fmadd_ps(lo8, weight1, S1); // S = sum(x * weight)
					S2 = _mm256_fmadd_ps(hi8, weight2, S2);
				}

				my1 = _mm256_div_ps(S1, W1); // W=sum(weights) == 0 cannot happen.
				my2 = _mm256_div_ps(S2, W2);
			}
			_mm256_storeu_ps(pOut + static_cast<size_t>(line) * pOutputBitmap->Width(), my1);
			_mm256_storeu_ps(pOut + static_cast<size_t>(line) * pOutputBitmap->Width() + 8, my2);
		}

		// Rest of line
		const float N = static_cast<float>(lineAddresses.size());
		for (int n = nrVectors * 16; n < width; ++n, ++pOut)
		{
			float my{ 0.0f };
			// Calculate initial (unweighted) mean.
			for (auto frameAddress : lineAddresses)
			{
				const WORD* pColor = static_cast<WORD*>(frameAddress) + n + colorOffset;
				my += static_cast<float>(*pColor);
			}
			my /= N;

			for (int iteration = 0; iteration < nIterations; ++iteration)
			{
				float S{ 0.0f };

				// Calculate sigma² related to µ of last iteration.
				for (auto frameAddress : lineAddresses)
				{
					const WORD* pColor = static_cast<WORD*>(frameAddress) + n + colorOffset;
					const float d = static_cast<float>(*pColor) - my;
					S += (d * d);
				}
				const float sigmaSq = S / N;

				// Calculate new µ using current sigma².
				float W{ 0.0f };
				S = 0.0f;
				for (auto frameAddress : lineAddresses)
				{
					const WORD* pColor = static_cast<WORD*>(frameAddress) + n + colorOffset;
					const float color = static_cast<float>(*pColor);
					const float d = color - my;
					const float denominator = sigmaSq + d * d;
					const float w = denominator == 0.0f ? 1.0f : (sigmaSq / denominator);
					W += w;
					S += color * w;
				}
				my = S / W; // W cannot be zero.
			}

			*(pOut + static_cast<size_t>(line) * pOutputBitmap->Width()) = my;
		}
	};

	autoAdaptLoop(&pOutputBitmap->m_Red.m_vPixels[0], 0);
	autoAdaptLoop(&pOutputBitmap->m_Green.m_vPixels[0], width);
	autoAdaptLoop(&pOutputBitmap->m_Blue.m_vPixels[0], width * 2);

	return 0;
}
#endif
