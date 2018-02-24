#ifndef __CHANNELALIGN_H__
#define __CHANNELALIGN_H__

#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "PixelTransform.h"

class CChannelAlign
{
private :
	BOOL	AlignChannel(CMemoryBitmap * pBitmap, CMemoryBitmap ** ppBitmap, CPixelTransform & PixTransform, CDSSProgress * pProgress);
	void	CopyBitmap(CMemoryBitmap * pSrcBitmap, CMemoryBitmap * pTgtBitmap);

public:
	
	CChannelAlign()
	{
		
	}

	~CChannelAlign()
	{
		
	}

	BOOL	AlignChannels(CMemoryBitmap * pBitmap, CDSSProgress * pProgress);
};

#endif __CHANNELALIGN_H__