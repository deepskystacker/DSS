#include "pch.h"
#include "avx_luminance.h"

AvxLuminance::AvxLuminance(const CMemoryBitmap& inputbm, CMemoryBitmap& outbm) :
	inputBitmap{ inputbm },
	outputBitmap{ outbm }
{
}

