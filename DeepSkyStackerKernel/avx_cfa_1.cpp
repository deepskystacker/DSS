#include "pch.h"
#include "avx_includes.h"
#include "avx_cfa.h"
#include "avx_bitmap_util.h"
#include "avx_simd_check.h"

AvxCfaProcessing::AvxCfaProcessing(const size_t lineStart, const size_t lineEnd, const CMemoryBitmap& inputbm) :
	redPixels{},
	greenPixels{},
	bluePixels{},
	inputBitmap{ inputbm },
	vectorsPerLine{ 0 }
{
	init(lineStart, lineEnd);
}

void AvxCfaProcessing::init(const size_t lineStart, const size_t lineEnd) // You should be sure that lineEnd >= lineStart!
{
	const size_t height = lineEnd - lineStart;
	vectorsPerLine = AvxBitmapUtil::numberOfAvxVectors<std::uint16_t, VectorElementType>(inputBitmap.Width());
	const size_t nrVectors = vectorsPerLine * height;
	if (nrVectors != 0 && AvxBitmapUtil{ inputBitmap }.isMonochromeCfaBitmapOfType<std::uint16_t>())
	{
		redPixels.resize(nrVectors);
		greenPixels.resize(nrVectors);
		bluePixels.resize(nrVectors);
	}
}

