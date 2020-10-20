#pragma once

#include "BitmapExt.h"
#include <vector>

#if defined(AVX_INTRINSICS) && defined(_M_X64)

class AvxCfaProcessing
{
private:
	std::vector<WORD> redPixels;
	std::vector<WORD> greenPixels;
	std::vector<WORD> bluePixels;
	CMemoryBitmap& inputBitmap;
public:
	AvxCfaProcessing() = delete;
	AvxCfaProcessing(const size_t lineStart, const size_t lineEnd, CMemoryBitmap& inputbm);
	AvxCfaProcessing(const AvxCfaProcessing&) = default;
	AvxCfaProcessing(AvxCfaProcessing&&) = delete;
	AvxCfaProcessing& operator=(const AvxCfaProcessing&) = delete;

	void init(const size_t lineStart, const size_t lineEnd);
	int interpolate(const size_t lineStart, const size_t lineEnd, const long pixelSizeMultiplier);

	template <class T>
	inline const T* redCfaPixels(const size_t index)
	{
		if constexpr (std::is_same<T, WORD>::value)
			return &this->redPixels[index];
		else
			return nullptr;
	}
	template <class T>
	inline const T* greenCfaPixels(const size_t index)
	{
		if constexpr (std::is_same<T, WORD>::value)
			return &this->greenPixels[index];
		else
			return nullptr;
	}
	template <class T>
	inline const T* blueCfaPixels(const size_t index)
	{
		if constexpr (std::is_same<T, WORD>::value)
			return &this->bluePixels[index];
		else
			return nullptr;
	}
private:
	int interpolateGrayCFA2Color(const size_t lineStart, const size_t lineEnd);
};

#else

class AvxCfaProcessing
{
public:
	AvxCfaProcessing(const size_t, const size_t, CMemoryBitmap&) {}
	void init(const size_t, const size_t) {}
	int interpolate(const size_t, const size_t, const long) { return 1; }
};

#endif
