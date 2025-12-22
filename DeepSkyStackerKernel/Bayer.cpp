#include "pch.h"
#include "Bayer.h"

BAYERCOLOR GetBayerColor(size_t baseX, size_t baseY, CFATYPE CFAType, size_t xOffset, size_t yOffset)
{
	const size_t x = baseX + xOffset;		// Apply the X Bayer offset if supplied
	const size_t y = baseY + yOffset;		// Apply the Y Bayer offset if supplied

	switch (CFAType)
	{
	case CFATYPE_NONE:
		return BAYER_UNKNOWN;
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
	default:
	{	// CYMG Type
		if (IsSimpleCYMG(CFAType))
		{
			// 2 lines and 2 columns repeated pattern
			if (y & 1)
			{
				if (x & 1)
					return static_cast<BAYERCOLOR>(CFAType & 0xF);
				else
					return static_cast<BAYERCOLOR>((CFAType >> 4) & 0xF);
			}
			else
			{
				if (x & 1)
					return static_cast<BAYERCOLOR>((CFAType >> 8) & 0xF);
				else
					return static_cast<BAYERCOLOR>((CFAType >> 12) & 0xF);
			};
		}
		else
		{
			// 4 lines and 2 columns repeated pattern
			if (y % 4 == 0)
			{
				if (x & 1)
					return static_cast<BAYERCOLOR>((CFAType >> 24) & 0xF);
				else
					return static_cast<BAYERCOLOR>((CFAType >> 28) & 0xF);
			}
			else if (y % 4 == 1)
			{
				if (x & 1)
					return static_cast<BAYERCOLOR>((CFAType >> 16) & 0xF);
				else
					return static_cast<BAYERCOLOR>((CFAType >> 20) & 0xF);
			}
			else if (y % 4 == 2)
			{
				if (x & 1)
					return static_cast<BAYERCOLOR>((CFAType >> 8) & 0xF);
				else
					return static_cast<BAYERCOLOR>((CFAType >> 12) & 0xF);
			}
			else
			{
				if (x & 1)
					return static_cast<BAYERCOLOR>((CFAType >> 0) & 0xF);
				else
					return static_cast<BAYERCOLOR>((CFAType >> 4) & 0xF);
			};
		};
	};
	};
}

//
// Add parameter yOffset to specify CFA Matrix offset to be applied (for FITS files)
//
bool	IsBayerBlueLine(size_t baseY, CFATYPE CFAType, size_t yOffset )
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
bool IsBayerBlueColumn(size_t baseX, CFATYPE CFAType, size_t xOffset )
{
	size_t x = baseX + xOffset;

	if ((CFAType == CFATYPE_GBRG) || (CFAType == CFATYPE_RGGB))
		return (x & 1) ? true : false;
	else
		return (x & 1) ? false : true;
}

bool IsBayerRedLine(size_t baseY, CFATYPE CFAType, size_t yOffset )
{
	return !IsBayerBlueLine(baseY, CFAType, yOffset);
}

//
// Add parameter xOffset to specify CFA Matrix offset to be applied (for FITS files)
//
bool IsBayerRedColumn(size_t baseX, CFATYPE CFAType, size_t xOffset )
{
	return !IsBayerBlueColumn(baseX, CFAType, xOffset);
}
