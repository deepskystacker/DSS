#include "stdafx.h"
#include "Bayer.h"

BAYERCOLOR GetBayerColor(size_t baseX, size_t baseY, CFATYPE CFAType, int xOffset/*=0*/, int yOffset/*=0*/)
{
	const size_t x = baseX + xOffset;		// Apply the X Bayer offset if supplied
	const size_t y = baseY + yOffset;		// Apply the Y Bayer offset if supplied

	switch (CFAType)
	{
	case CFATYPE_NONE:
		return BAYER_UNKNOWN;
		break;
	case CFATYPE_BGGR:
		if (x & 1)
		{
			if (y & 1)
				return BAYER_RED;
			else
				return BAYER_GREEN;
		}
		else
		{
			if (y & 1)
				return BAYER_GREEN;
			else
				return BAYER_BLUE;
		};
		break;
	case CFATYPE_GRBG:
		if (x & 1)
		{
			if (y & 1)
				return BAYER_GREEN;
			else
				return BAYER_RED;
		}
		else
		{
			if (y & 1)
				return BAYER_BLUE;
			else
				return BAYER_GREEN;
		};
		break;
	case CFATYPE_GBRG:
		if (x & 1)
		{
			if (y & 1)
				return BAYER_GREEN;
			else
				return BAYER_BLUE;
		}
		else
		{
			if (y & 1)
				return BAYER_RED;
			else
				return BAYER_GREEN;
		};
		break;
	case CFATYPE_RGGB:
		if (x & 1)
		{
			if (y & 1)
				return BAYER_BLUE;
			else
				return BAYER_GREEN;
		}
		else
		{
			if (y & 1)
				return BAYER_GREEN;
			else
				return BAYER_RED;
		};
		break;
	default:
	{	// CYMG Type
		if (IsSimpleCYMG(CFAType))
		{
			// 2 lines and 2 columns repeated pattern
			if (y & 1)
			{
				if (x & 1)
					return (BAYERCOLOR)(CFAType & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 4) & 0xF);
			}
			else
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 8) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 12) & 0xF);
			};
		}
		else
		{
			// 4 lines and 2 columns repeated pattern
			if (y % 4 == 0)
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 24) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 28) & 0xF);
			}
			else if (y % 4 == 1)
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 16) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 20) & 0xF);
			}
			else if (y % 4 == 2)
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 8) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 12) & 0xF);
			}
			else
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 0) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 4) & 0xF);
			};
		};
	};
	};
}

//
// Add parameter yOffset to specify CFA Matrix offset to be applied (for FITS files)
//
bool	IsBayerBlueLine(size_t baseY, CFATYPE CFAType, int yOffset /* = 0 */)
{
	size_t y = baseY + yOffset;

	if ((CFAType == CFATYPE_GRBG) || (CFAType == CFATYPE_RGGB))
		return (y & 1) ? true : false;
	else
		return (y & 1) ? false : true;
}

//
// Add parameter xOffset to specify CFA Matrix offset to be applied (for FITS files)
//
bool IsBayerBlueColumn(size_t baseX, CFATYPE CFAType, int xOffset /* = 0 */)
{
	size_t x = baseX + xOffset;

	if ((CFAType == CFATYPE_GBRG) || (CFAType == CFATYPE_RGGB))
		return (x & 1) ? true : false;
	else
		return (x & 1) ? false : true;
}

bool IsBayerRedLine(size_t baseY, CFATYPE CFAType, int yOffset /* = 0 */)
{
	return !IsBayerBlueLine(baseY, CFAType, yOffset);
}

//
// Add parameter xOffset to specify CFA Matrix offset to be applied (for FITS files)
//
bool IsBayerRedColumn(size_t baseX, CFATYPE CFAType, int xOffset /* = 0 */)
{
	return !IsBayerBlueColumn(baseX, CFAType, xOffset);
}
