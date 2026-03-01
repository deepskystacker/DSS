#pragma once
/****************************************************************************
**
** Copyright (C) 2024, 2025 Martin Toeltsch
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
#include "BitMapFiller.h"

namespace DSS { class OldProgressBase; }

class AvxBitmapFiller : public BitmapFillerBase
{
private:
	std::vector<std::uint8_t> sourceBuffer;
public:
	AvxBitmapFiller(CMemoryBitmap* pB, DSS::OldProgressBase* pP);
	AvxBitmapFiller(const AvxBitmapFiller&) = default; // For cloning.
	virtual ~AvxBitmapFiller() override {}

	virtual bool isThreadSafe() const override;
	virtual std::unique_ptr<BitmapFillerInterface> clone() override;

	virtual size_t Write(const void* source, const size_t bytesPerPixel, const size_t nrPixels, const size_t rowIndex) override;
};

class NonAvxBitmapFiller : public BitmapFillerBase
{
public:
	NonAvxBitmapFiller(CMemoryBitmap* pB, DSS::OldProgressBase* pP);
	NonAvxBitmapFiller(const NonAvxBitmapFiller&) = default; // For cloning.
	virtual ~NonAvxBitmapFiller() override {}

	virtual bool isThreadSafe() const override;
	virtual std::unique_ptr<BitmapFillerInterface> clone() override;

	virtual size_t Write(const void* source, const size_t bytesPerPixel, const size_t nrPixels, const size_t rowIndex) override;
};
