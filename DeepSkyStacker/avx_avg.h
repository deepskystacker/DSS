#pragma once

#include "PixelTransform.h"
#include "StackingTasks.h"
#include "BackgroundCalibration.h"
#include "BitmapExt.h"


#if defined(AVX_INTRINSICS) && defined(_M_X64)
class AvxAccumulation
{
	int resultWidth, resultHeight;
	CMemoryBitmap& tempBitmap;
	CMemoryBitmap& outputBitmap;
	const CTaskInfo& taskInfo;
public:
	AvxAccumulation() = delete;
	AvxAccumulation(const CRect& resultRect, const CTaskInfo& tInfo, CMemoryBitmap& tempbm, CMemoryBitmap& outbm) noexcept;
	AvxAccumulation(const AvxAccumulation&) = delete;
	AvxAccumulation(AvxAccumulation&&) = delete;
	AvxAccumulation& AvxAccumulation::operator=(const AvxAccumulation&) = delete;

	int accumulate(const int nrStackedBitmaps);
private:
	template <class T_IN, class T_OUT>
	int doAccumulate(const int nrStackedBitmaps);
};

#else

class AvxAccumulation
{
public:
	AvxAccumulation(const CRect& resultRect, const CTaskInfo& tInfo, CMemoryBitmap& tempbm, CMemoryBitmap& outbm) {}
	int accumulate(const int)
	{
		return 1;
	}
};

#endif