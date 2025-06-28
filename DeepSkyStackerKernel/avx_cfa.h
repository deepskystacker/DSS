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

#include "BitmapBase.h"
#include "avx_includes.h"

class AvxCfaProcessing
{
private:
	typedef __m512i VectorElementType;
	typedef std::vector<VectorElementType> VectorType;

	friend class Avx256CfaProcessing;

	VectorType redPixels;
	VectorType greenPixels;
	VectorType bluePixels;
	const CMemoryBitmap& inputBitmap;
	size_t vectorsPerLine;
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
			return reinterpret_cast<const std::uint16_t*>(&this->redPixels[rowIndex * vectorsPerLine]);
		else
			return nullptr;
	}
	template <class T>
	inline const T* greenCfaLine(const size_t rowIndex) const
	{
		if constexpr (std::is_same<T, std::uint16_t>::value)
			return reinterpret_cast<const std::uint16_t*>(&this->greenPixels[rowIndex * vectorsPerLine]);
		else
			return nullptr;
	}
	template <class T>
	inline const T* blueCfaLine(const size_t rowIndex) const
	{
		if constexpr (std::is_same<T, std::uint16_t>::value)
			return reinterpret_cast<const std::uint16_t*>(&this->bluePixels[rowIndex * vectorsPerLine]);
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

class Avx256CfaProcessing
{
private:
	friend class AvxCfaProcessing;

	AvxCfaProcessing& avxData;
	Avx256CfaProcessing(AvxCfaProcessing& ad) : avxData{ ad } {}

	template <int RG_ROW> // RG_ROW==0 for RGGB pattern, RG_ROW==1 for GBRG pattern.
	int interpolateGrayCFA2Color(const size_t lineStart, const size_t lineEnd);
};
