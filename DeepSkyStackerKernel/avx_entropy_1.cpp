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
#include "avx_entropy.h"
#include "avx_bitmap_util.h"
#include "avx_cfa.h"
#include "avx_histogram.h"
#include "avx_simd_check.h"
#include "Multitask.h"

AvxEntropy::AvxEntropy(const CMemoryBitmap& inputbm, const CEntropyInfo& entrinfo, CMemoryBitmap* entropycov) :
	inputBitmap{ inputbm },
	entropyInfo{ entrinfo },
	pEntropyCoverage{ entropycov },
	avxEnabled(AvxSimdCheck::checkSimdAvailability())
{
	if (pEntropyCoverage != nullptr && avxEnabled)
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
	if (avxEnabled)
	{
		return avxCalcEntropies(squareSize, nSquaresX, nSquaresY, redEntropies, greenEntropies, blueEntropies);
	}
	else return 1; // AVX not available, return error code 1
}

