#pragma once

#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "PixelTransform.h"
namespace DSS { class ProgressBase; }

class CChannelAlign
{
private:
	static std::shared_ptr<CMemoryBitmap> AlignChannel(const CMemoryBitmap* pBitmap, const CPixelTransform& PixTransform, DSS::ProgressBase* pProgress);
	static void CopyBitmap(std::shared_ptr<const CMemoryBitmap> pSrcBitmap, CMemoryBitmap* pTgtBitmap);

public:
	static bool AlignChannels(std::shared_ptr<CMemoryBitmap> pBitmap, DSS::ProgressBase* pProgress);
};
