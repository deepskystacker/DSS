#pragma once

#include "PixelTransform.h"
#include "StackingTasks.h"
#include "BackgroundCalibration.h"
#include "BitmapExt.h"
#include <vector>


#if defined(AVX_INTRINSICS) && defined(_M_X64)
class AvxStacking {
	long lineStart, lineEnd, colEnd;
	int width, height;
	int resultWidth, resultHeight;
	std::vector<float> xCoordinates;
	std::vector<float> yCoordinates;
	std::vector<float> redPixels;
	std::vector<float> greenPixels;
	std::vector<float> bluePixels;
	CMemoryBitmap& inputBitmap;
	CMemoryBitmap& tempBitmap;
public:
	AvxStacking() = delete;
	AvxStacking(long lStart, long lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const CRect& resultRect);
	AvxStacking(const AvxStacking&) = delete;
	AvxStacking(AvxStacking&&) = delete;
	AvxStacking& AvxStacking::operator=(const AvxStacking&) = delete;

	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier);

	static bool checkCpuFeatures() noexcept;
private:
	int pixelTransform(const CPixelTransform& pixelTransformDef);
	int backgroundCalibration(const CBackgroundCalibration& backgroundCalibrationDef);
	template <bool ISRGB>
	int pixelDispatchAndAccumulate();
};

#else

class AvxStacking
{
public:
	AvxStacking(long lStart, long lEnd, CMemoryBitmap& inputbm, CMemoryBitmap& tempbm, const CRect& resultRect) {}
	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, const long pixelSizeMultiplier)
	{
		return 1;
	}
};
#endif


#if defined(AVX_INTRINSICS) && defined(_M_X64)
class AvxSupport
{
	// Unfortunately, we cannot use const here, because the member function are hardly never const declared. :-(
	CMemoryBitmap& bitmap;
	CColorBitmapT<WORD>* pColorBitmap;
	CGrayBitmapT<WORD>* pGrayBitmap;
public:
	AvxSupport(CMemoryBitmap& b) noexcept;

	bool isColorBitmap() const;
	bool isMonochromeBitmap() const;

	const std::vector<WORD>& redPixels() const { return pColorBitmap->m_Red.m_vPixels; }
	const std::vector<WORD>& greenPixels() const { return pColorBitmap->m_Green.m_vPixels; }
	const std::vector<WORD>& bluePixels() const { return pColorBitmap->m_Blue.m_vPixels; }
	const std::vector<WORD>& grayPixels() const { return pGrayBitmap->m_vPixels; }

	std::vector<WORD>& redPixels() { return pColorBitmap->m_Red.m_vPixels; }
	std::vector<WORD>& greenPixels() { return pColorBitmap->m_Green.m_vPixels; }
	std::vector<WORD>& bluePixels() { return pColorBitmap->m_Blue.m_vPixels; }
	std::vector<WORD>& grayPixels() { return pGrayBitmap->m_vPixels; }

	const int width() const;

	template <class T>
	bool isBitmapOfType() const;

	inline static __m256 wordToPackedFloat(const __m128i x) noexcept { return _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(x)); }
};
#endif
