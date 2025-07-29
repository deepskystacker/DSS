#pragma once
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
#include "avx_includes.h"

/*
* Class for
* 1. Calculation of the entropy data of the squares,
* 2. Managing the entropy coverage (the coverage bitmap itself is calculated while stacking with the class AvxStacking).
*/
class CMemoryBitmap;
class CEntropyInfo;
class AvxEntropy
{
private:
	friend class Avx256Stacking;
	friend class NonAvxStacking;
	friend class AvxAccumulation;

	typedef std::vector<float> EntropyVectorType;
	typedef std::vector<__m512> EntropyLayerVectorType;

	const CMemoryBitmap& inputBitmap;
	const CEntropyInfo& entropyInfo;
	CMemoryBitmap* pEntropyCoverage;
	EntropyLayerVectorType redEntropyLayer;
	EntropyLayerVectorType greenEntropyLayer;
	EntropyLayerVectorType blueEntropyLayer;
	bool avxEnabled{ false };

public:
	AvxEntropy() = delete;
	AvxEntropy(const CMemoryBitmap& inputbm, const CEntropyInfo& entrinfo, CMemoryBitmap* entropycov);
	AvxEntropy(const AvxEntropy&) = default;
	AvxEntropy(AvxEntropy&&) = delete;
	AvxEntropy& operator=(const AvxEntropy&) = delete;
#if defined(UNIT_TESTS) 
	float* getRedEntropyLayer() { return reinterpret_cast<float*>(redEntropyLayer.data()); }
#endif 

	int calcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies);
	int avxCalcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies);
private:
	template <class T>
	int doCalcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies);
	void ComputeEntropies(int lMinX, int lMinY, int lMaxX, int lMaxY, double& fRedEntropy, double& fGreenEntropy, double& fBlueEntropy);
};
