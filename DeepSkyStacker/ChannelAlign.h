#pragma once

#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "PixelTransform.h"
namespace DSS { class ProgressBase; }

class CChannelAlign
{
private :
	std::shared_ptr<CMemoryBitmap> AlignChannel(CMemoryBitmap* pBitmap, CPixelTransform& PixTransform, DSS::ProgressBase* pProgress);
	void CopyBitmap(const CMemoryBitmap* pSrcBitmap, CMemoryBitmap* pTgtBitmap) const;

public:
	CChannelAlign()
	{}

	~CChannelAlign()
	{}

	bool AlignChannels(CMemoryBitmap* pBitmap, DSS::ProgressBase* pProgress);
};

