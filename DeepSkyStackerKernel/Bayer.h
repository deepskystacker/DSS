#pragma once
#include "cfa.h"

typedef enum
{
	BAYER_UNKNOWN = 0,
	BAYER_RED = 1,
	BAYER_GREEN = 2,
	BAYER_BLUE = 3,
	BAYER_CYAN = 0xA,
	BAYER_GREEN2 = 0xB,
	BAYER_MAGENTA = 0xC,
	BAYER_YELLOW = 0xD,
	BAYER_NRCOLORS = 0xD + 1
} BAYERCOLOR;

inline int CMYGZeroIndex(BAYERCOLOR Color) { return Color - BAYER_CYAN; }

BAYERCOLOR GetBayerColor(size_t baseX, size_t baseY, CFATYPE CFAType, int xOffset = 0, int yOffset = 0);
bool IsBayerBlueLine(size_t baseY, CFATYPE CFAType, int yOffset = 0);
bool IsBayerBlueColumn(size_t baseX, CFATYPE CFAType, int xOffset = 0);
bool IsBayerRedLine(size_t baseY, CFATYPE CFAType, int yOffset = 0);
bool IsBayerRedColumn(size_t baseX, CFATYPE CFAType, int xOffset = 0);
