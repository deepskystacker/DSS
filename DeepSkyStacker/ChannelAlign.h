#ifndef __CHANNELALIGN_H__
#define __CHANNELALIGN_H__

#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "PixelTransform.h"

class CChannelAlign
{
private :
	std::shared_ptr<CMemoryBitmap> AlignChannel(const CMemoryBitmap* pBitmap, CPixelTransform& PixTransform, CDSSProgress* pProgress);
	void CopyBitmap(const CMemoryBitmap* pSrcBitmap, CMemoryBitmap* pTgtBitmap) const;

public:
	CChannelAlign()
	{}

	~CChannelAlign()
	{}

	bool AlignChannels(CMemoryBitmap* pBitmap, CDSSProgress* pProgress);
};

#endif
