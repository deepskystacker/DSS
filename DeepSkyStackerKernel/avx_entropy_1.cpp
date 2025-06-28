#include "pch.h"
#include "avx_includes.h"
#include "avx_entropy.h"
#include "avx_bitmap_util.h"
#include "avx_cfa.h"
#include "avx_histogram.h"
#include "avx_simd_check.h"
#include "Multitask.h"

AvxEntropy::AvxEntropy(const CMemoryBitmap& inputbm, const CEntropyInfo& entrinfo, CMemoryBitmap* entropycov) :
	inputBitmap{ inputbm },
	entropyInfo{ entrinfo },
	pEntropyCoverage{ entropycov }
{
	if (pEntropyCoverage != nullptr && AvxSimdCheck::checkSimdAvailability())
	{
		const size_t width = pEntropyCoverage->Width();
		const size_t height = pEntropyCoverage->Height();
		static_assert(std::is_same<__m512&, decltype(redEntropyLayer[0])>::value);
		const size_t nrVectors = AvxBitmapUtil::numberOfAvxVectors<float, __m512>(width);
		redEntropyLayer.resize(height * nrVectors);
		if (AvxBitmapUtil{ *pEntropyCoverage }.isColorBitmap())
		{
			greenEntropyLayer.resize(height * nrVectors);
			blueEntropyLayer.resize(height * nrVectors);
		}
	}
}

int AvxEntropy::calcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies)
{
	if (AvxSimdCheck::checkSimdAvailability())
	{
		return avxCalcEntropies(squareSize, nSquaresX, nSquaresY, redEntropies, greenEntropies, blueEntropies);
	}
	else return 1; // AVX not available, return error code 1
}

