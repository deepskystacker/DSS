#include "StdAfx.h"
#include "BitMapFiller.h"
#include "avx_bitmap_filler.h"
#include "avx_support.h"


BitmapFillerInterface::BitmapFillerInterface(CMemoryBitmap* pB, CDSSProgress* pP) :
	pProgress{ pP },
	pBitmap{ pB }
{}

std::unique_ptr<BitmapFillerInterface> BitmapFillerInterface::makeBitmapFiller(CMemoryBitmap* pBitmap, CDSSProgress* pProgress, const double redWb, const double greenWb, const double blueWb)
{
	if (AvxSupport::checkSimdAvailability())
		return std::make_unique<AvxBitmapFiller>(pBitmap, pProgress, redWb, greenWb, blueWb);
	else
		return std::make_unique<BitMapFiller>(pBitmap, pProgress, redWb, greenWb, blueWb);
}

bool BitmapFillerInterface::isThreadSafe() const { return false; }
