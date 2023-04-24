#include "stdafx.h"
#include "Ztrace.h"
#include "BitmapBase.h"
#include "BitmapCharacteristics.h"
#include "GrayBitmap.h"
#include "ColorBitmap.h"

std::shared_ptr<CMemoryBitmap> CreateBitmap(const CBitmapCharacteristics& bc)
{
	ZFUNCTRACE_RUNTIME();

	if (bc.m_lNrChannels == 1)
	{
		if (bc.m_lBitsPerPixel == 8)
		{
			ZTRACE_RUNTIME("Creating 8 Gray bit memory bitmap");
			return std::make_shared<C8BitGrayBitmap>();
		}
		else if (bc.m_lBitsPerPixel == 16)
		{
			ZTRACE_RUNTIME("Creating 16 Gray bit memory bitmap");
			return std::make_shared<C16BitGrayBitmap>();
		}
		else if (bc.m_lBitsPerPixel == 32)
		{
			if (bc.m_bFloat)
			{
				ZTRACE_RUNTIME("Creating 32 float Gray bit memory bitmap");
				return std::make_shared<C32BitFloatGrayBitmap>();
			}
			else
			{
				ZTRACE_RUNTIME("Creating 32 Gray bit memory bitmap");
				return std::make_shared<C32BitGrayBitmap>();
			}
		}
	}
	else if (bc.m_lNrChannels == 3)
	{
		if (bc.m_lBitsPerPixel == 8)
		{
			ZTRACE_RUNTIME("Creating 8 RGB bit memory bitmap");
			return std::make_shared<C24BitColorBitmap>();
		}
		else if (bc.m_lBitsPerPixel == 16)
		{
			ZTRACE_RUNTIME("Creating 16 RGB bit memory bitmap");
			return std::make_shared<C48BitColorBitmap>();
		}
		else if (bc.m_lBitsPerPixel == 32)
		{
			if (bc.m_bFloat)
			{
				ZTRACE_RUNTIME("Creating 32 float RGB bit memory bitmap");
				return std::make_shared<C96BitFloatColorBitmap>();
			}
			else
			{
				ZTRACE_RUNTIME("Creating 32 RGB bit memory bitmap");
				return std::make_shared<C96BitColorBitmap>();
			}
		}
	}

	return std::shared_ptr<CMemoryBitmap>{};
}
