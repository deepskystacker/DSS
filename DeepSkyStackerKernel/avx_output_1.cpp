/****************************************************************************
**
** Copyright (C) 2020, 2025 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
#include "pch.h"
#include "avx_includes.h"
#include "avx_output.h"
#include "avx_simd_check.h"
#include "avx_bitmap_util.h"
#include "avx_median.h"
#include "MultiBitmap.h"
#include "ColorMultiBitmap.h"
#include "GreyMultiBitmap.h"

AvxOutputComposition::AvxOutputComposition(CMultiBitmap& mBitmap, CMemoryBitmap& outputbm) :
	inputBitmap{ mBitmap },
	outputBitmap{ outputbm }
{

}

int AvxOutputComposition::compose(const int line, std::vector<void*> const& lineAddresses)
{
	if (!AvxSimdCheck::checkSimdAvailability())
		return 1;
	// Homogenization not implemented with AVX
	if (inputBitmap.GetHomogenization())
		return 1;
	// Output must be float values
	if (AvxBitmapUtil{ outputBitmap }.bitmapHasCorrectType<float>() == false)
		return 1;	// If this is not equal, something went wrong and we cannot continue without risking access violations.
	if (lineAddresses.size() != inputBitmap.GetNrAddedBitmaps())
		return 1;
	// No line addresses?
	if (lineAddresses.empty())
		return 1;

	return avxCompose(line, lineAddresses);
}
