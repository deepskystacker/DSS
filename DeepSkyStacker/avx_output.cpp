#include "StdAfx.h"
#include "avx_output.h"
#include "avx.h"

#if defined(AVX_INTRINSICS) && defined(_M_X64)

#include <immintrin.h>

AvxOutputComposition::AvxOutputComposition(CMultiBitmap& mBitmap, CMemoryBitmap* pOut) :
	inputBitmap{ mBitmap },
	pOutput{ pOut }
{
}

int AvxOutputComposition::compose(const int line, std::vector<void*> const& lineAddresses)
{
	if (!AvxStacking::checkCpuFeatures())
		return 1;
	// Homogenization not implemented with AVX
	if (inputBitmap.GetHomogenization())
		return 1;
	// Input must be a RGB color bitmap with unsigned shorts
	if (inputBitmap.GetNrChannels() != 3 || inputBitmap.GetNrBytesPerChannel() != 2)
		return 1;
	// We cannot consider more than 65536 bitmaps
	if (inputBitmap.GetNrAddedBitmaps() > 0x0ffff)
		return 1;
	// Output must be float values
	if (pOutput->BitPerSample() != 32 || !pOutput->IsFloat() || !pOutput->isTopDown())
		return 1;

	C96BitFloatColorBitmap* const pOutputBitmap = dynamic_cast<C96BitFloatColorBitmap*>(pOutput);
	if (pOutputBitmap == nullptr)
		return 1;

	if (inputBitmap.GetProcessingMethod() == MBP_SIGMACLIP) {
		return processKappaSigma(line, lineAddresses, pOutputBitmap);
	}

	return 2;
}

int AvxOutputComposition::processKappaSigma(const int line, std::vector<void*> const& lineAddresses, C96BitFloatColorBitmap* const pOutputBitmap)
{
	const auto parameters = inputBitmap.GetProcessingParameters();

	const int width = pOutput->RealWidth();
	const int nrVectors = width / 16;
	const size_t nrLightframes = lineAddresses.size();

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
		for (int counter = 0; counter < nrVectors; ++counter, pOut += 16) {
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
					const WORD* pColor = static_cast<WORD*>(frameAddress) + counter * 16 + colorOffset;
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
			_mm256_storeu_ps(pOut + line * pOutputBitmap->Width(), my1);
			_mm256_storeu_ps(pOut + line * pOutputBitmap->Width() + 8, my2);
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
			*(pOut + line * pOutputBitmap->Width()) = my;
		}
	};

	kappaSigmaLoop(&pOutputBitmap->m_Red.m_vPixels[0], 0);
	kappaSigmaLoop(&pOutputBitmap->m_Green.m_vPixels[0], width);
	kappaSigmaLoop(&pOutputBitmap->m_Blue.m_vPixels[0], width * 2);

	return 0;
}
#endif
