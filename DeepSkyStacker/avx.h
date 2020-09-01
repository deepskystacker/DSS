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
	CMemoryBitmap& bitmap;
public:
	AvxStacking() = delete;
	AvxStacking(long lStart, long lEnd, CMemoryBitmap& bitmap, const CRect& resultRect);
	AvxStacking(const AvxStacking&) = delete;
	AvxStacking(AvxStacking&&) = delete;
	AvxStacking& AvxStacking::operator=(const AvxStacking&) = delete;
	static bool checkCpuFeatures();
	int stack(const CPixelTransform& pixelTransformDef, const CTaskInfo& taskInfo, const CBackgroundCalibration& backgroundCalibrationDef, C48BitColorBitmap* pBitmap, C48BitColorBitmap* pTempBitmap, const long pixelSizeMultiplier);
private:
	int pixelTransform(const CPixelTransform& pixelTransformDef);
	int backgroundCalibration(const CBackgroundCalibration& backgroundCalibrationDef, C48BitColorBitmap* pBitmap);
	int pixelDispatchAndAccumulate(C48BitColorBitmap* pBitmap, C48BitColorBitmap* pTempBitmap);
public:
	inline static __m256 wordToPackedFloat(const __m128i x) noexcept { return _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(x)); }
};

#else

class AvxStacking
{
public:
	AvxStacking(long, long, CMemoryBitmap&, const CRect&) {}
	int stack(const CPixelTransform&, const CTaskInfo&, const CBackgroundCalibration&, C48BitColorBitmap*, C48BitColorBitmap*, const long)
	{
		return 1;
	}
};
#endif
