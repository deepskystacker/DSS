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
#include "pch.h"
#include "avx_includes.h"
#include "avx_filter.h"
#include "avx_simd_check.h"
#include "MedianFilterEngine.h"

template <>
AvxImageFilter<double>::AvxImageFilter(CInternalMedianFilterEngineT<double>* filEng) :
	filterEngine{ filEng }
{
}

template <class T>
AvxImageFilter<T>::AvxImageFilter(CInternalMedianFilterEngineT<T>*) :
	filterEngine{ nullptr }
{}

template <class T>
int AvxImageFilter<T>::filter(const size_t lineStart, const size_t lineEnd)
{
	if constexpr (!std::is_same<T, double>::value)
		return 1;
	if (!AvxSimdCheck::checkSimdAvailability())
		return 1;
	if (filterEngine == nullptr)
		return 1;
	if (filterEngine->m_lFilterSize != 1)
		return 1;
	if (filterEngine->m_CFAType != CFATYPE_NONE)
		return 1;
	return avxFilter(lineStart, lineEnd);
}
#if defined(_MSC_VER) 
#pragma warning(push)
#pragma warning(disable: 4661) // no suitable definition provided for explicit template instantiation request
#endif
// Explicit template instantiation for the types we need.
template class AvxImageFilter<std::uint8_t>;
template class AvxImageFilter<std::uint16_t>;
template class AvxImageFilter<std::uint32_t>;
template class AvxImageFilter<float>;
template class AvxImageFilter<double>;
#if defined(_MSC_VER)
#pragma warning( pop )
#endif
