#include "StdAfx.h"
#include "BitMapFiller.h"
#include "avx_bitmap_filler.h"
#include "avx_support.h"


BitmapFillerInterface::BitmapFillerInterface(CMemoryBitmap* pB, CDSSProgress* pP) :
	pProgress{ pP },
	pBitmap{ pB }
{}

std::unique_ptr<BitmapFillerInterface> BitmapFillerInterface::makeBitmapFiller(CMemoryBitmap* pBitmap, CDSSProgress* pProgress)
{
	if (AvxSupport::checkSimdAvailability())
		return std::make_unique<AvxBitmapFiller>(pBitmap, pProgress);
	else
		return std::make_unique<BitMapFiller>(pBitmap, pProgress);
}
