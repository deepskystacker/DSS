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

#include "BitmapBase.h"
#include "avx_simd_factory.h"
#include "avx_includes.h"

class AvxCfaProcessing
{
private:
	using VectorElementType = __m512i;
	using VectorType = std::vector<VectorElementType>;

	friend class Avx256CfaProcessing;
	friend class NonAvxCfaProcessing;

	VectorType redPixels;
	VectorType greenPixels;
	VectorType bluePixels;
	const CMemoryBitmap& inputBitmap;
	size_t vectorsPerLine;
	bool avxEnabled{ false };

public:
	AvxCfaProcessing() = delete;
	AvxCfaProcessing(const size_t lineStart, const size_t lineEnd, const CMemoryBitmap& inputbm);
	AvxCfaProcessing(const AvxCfaProcessing&) = default;
	AvxCfaProcessing(AvxCfaProcessing&&) = delete;
	AvxCfaProcessing& operator=(const AvxCfaProcessing&) = delete;

	void init(const size_t lineStart, const size_t lineEnd);
	int interpolate(const size_t lineStart, const size_t lineEnd, const int pixelSizeMultiplier);

	inline size_t nrVectorsPerLine() const { return this->vectorsPerLine; }

	template <class T>
	inline const T* redCfaLine(const size_t rowIndex) const
	{
		if constexpr (std::is_same<T, std::uint16_t>::value)
			return reinterpret_cast<const std::uint16_t*>(this->redPixels.data() + (rowIndex * vectorsPerLine));
		else
			return nullptr;
	}
	template <class T>
	inline const T* greenCfaLine(const size_t rowIndex) const
	{
		if constexpr (std::is_same<T, std::uint16_t>::value)
			return reinterpret_cast<const std::uint16_t*>(this->greenPixels.data() + (rowIndex * vectorsPerLine));
		else
			return nullptr;
	}
	template <class T>
	inline const T* blueCfaLine(const size_t rowIndex) const
	{
		if constexpr (std::is_same<T, std::uint16_t>::value)
			return reinterpret_cast<const std::uint16_t*>(this->bluePixels.data() + (rowIndex * vectorsPerLine));
		else
			return nullptr;
	}

	inline const VectorElementType* redCfaBlock() const { return this->redPixels.data(); }
	inline const VectorElementType* greenCfaBlock() const { return this->greenPixels.data(); }
	inline const VectorElementType* blueCfaBlock() const { return this->bluePixels.data(); }
private:
	std::uint16_t* redCfaLine(const size_t rowIndex) { return const_cast<std::uint16_t*>(static_cast<const AvxCfaProcessing*>(this)->redCfaLine<std::uint16_t>(rowIndex)); }
	std::uint16_t* greenCfaLine(const size_t rowIndex) { return const_cast<std::uint16_t*>(static_cast<const AvxCfaProcessing*>(this)->greenCfaLine<std::uint16_t>(rowIndex)); }
	std::uint16_t* blueCfaLine(const size_t rowIndex) { return const_cast<std::uint16_t*>(static_cast<const AvxCfaProcessing*>(this)->blueCfaLine<std::uint16_t>(rowIndex)); }
};

// *********************************
// ************ AVX-256 ************
// *********************************

class Avx256CfaProcessing : public SimdFactory<Avx256CfaProcessing>
{
private:
	friend class AvxCfaProcessing;
	friend class SimdFactory<Avx256CfaProcessing>;

	AvxCfaProcessing& cfaData;
	Avx256CfaProcessing(AvxCfaProcessing& ad) : cfaData{ ad } {}

	int interpolate(const size_t lineStart, const size_t lineEnd, const int pixelSizeMultiplier);

	// RG_ROW==0 for RGGB or BGGR pattern, RG_ROW==1 for GBRG or GRBG pattern.
	// REVERSE==0: RGGB/GBRG, REVERSE==1: BGGR/GRBG
	// In other words, if REVERSE==1, then R and B have reversed roles (the G channel remains unchanged).
	template <int RG_ROW, int REVERSE>
	int interpolateGrayCFA2Color(const size_t lineStart, const size_t lineEnd);
};

// ************************************************************
// The non-SIMD implementation to be used with the SimdSelector
// ************************************************************

class NonAvxCfaProcessing : public SimdFactory<NonAvxCfaProcessing>
{
private:
	friend class AvxCfaProcessing;
	friend class SimdFactory<NonAvxCfaProcessing>;

	AvxCfaProcessing& cfaData;
	NonAvxCfaProcessing(AvxCfaProcessing& ad) : cfaData{ ad } {}

	int interpolate(const size_t lineStart, const size_t lineEnd, const int pixelSizeMultiplier);
};
