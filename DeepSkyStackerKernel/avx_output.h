#pragma once
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

#include "BitmapBase.h"

class CMultiBitmap;
class AvxOutputComposition
{
private:
	CMultiBitmap& inputBitmap;
	CMemoryBitmap& outputBitmap;
public:
	AvxOutputComposition() = delete;
	AvxOutputComposition(CMultiBitmap& mBitmap, CMemoryBitmap& outputbm);
	AvxOutputComposition(const AvxOutputComposition&) = default;
	AvxOutputComposition(AvxOutputComposition&&) = delete;
	AvxOutputComposition& operator=(const AvxOutputComposition&) = delete;

	int compose(const int line, std::vector<void*> const& lineAddresses);
private:
	enum MethodSelection
	{
		KappaSigma = 0,
		MedianKappaSigma = 1,
		MedianOnly = 2
	};
	int avxCompose(const int line, std::vector<void*> const& lineAddresses);

	template <class INPUTTYPE, class OUTPUTTYPE>
	static bool bitmapColorOrGray(const CMultiBitmap& bitmap) noexcept;

	template <class T>
	static float convertToFloat(const T value) noexcept;

	template <MethodSelection Method>
	int processMedianKappaSigma(const int line, std::vector<void*> const& lineAddresses);

	template <typename T, MethodSelection Method>
	int doProcessMedianKappaSigma(const int line, std::vector<void*> const& lineAddresses);

	int processAutoAdaptiveWeightedAverage(const int line, std::vector<void*> const& lineAddresses);
	template <class T>
	int doProcessAutoAdaptiveWeightedAverage(const int line, std::vector<void*> const& lineAddresses);
};
