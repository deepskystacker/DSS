#pragma once

#include "BitmapBase.h"

class AvxCfaProcessing
{
private:
	typedef __m512i VectorElementType;
	typedef std::vector<VectorElementType> VectorType;

	friend class Avx256CfaProcessing;

	VectorType redPixels;
	VectorType greenPixels;
	VectorType bluePixels;
	CMemoryBitmap& inputBitmap;
	size_t vectorsPerLine;
	bool avxReady;
public:
	AvxCfaProcessing() = delete;
	AvxCfaProcessing(const size_t lineStart, const size_t lineEnd, CMemoryBitmap& inputbm);
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
