/****************************************************************************
**
** Copyright (C) 2026 David C. Partridge
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
#include "Multitask.h"
#include "StackedBitmap.h"
#include <algorithm>

namespace DSS
{
	//
	//
	// Adjust the image colour balance according to the values of:
	//   
	//   redShift
	//   greenShift
	//   blueShift
	// 
	// Whose values have a range of [-1.0, 1.0]
	// 
	// The image data is expected to have been normalised to the range [0, 1.0]
	//
	// The function does nothing if the image is monochrome, as colour balance
	// adjustments are not applicable to monochrome images.
	//
	void StackedBitmap::adjustColourBalance(float redShift, float greenShift, float blueShift)
	{
		if (m_bMonochrome)
			return;		// Colour balance adjustments are not applicable to monochrome images.
		if (redShift == 0.0f && greenShift == 0.0f && blueShift == 0.0f)
			return;		// No colour balance adjustments to apply

		float* buf[3] = {
			m_vRedPlane.data(),
			m_vGreenPlane.data(),
			m_vBluePlane.data() };

		const int nrProcessors{ Multitask::GetNrProcessors() };
		const int RLAYER = 0, GLAYER = 1, BLAYER = 2;

		std::int64_t i, n = m_lWidth * m_lHeight;
#pragma omp parallel for schedule(static) if (nrProcessors > 1)
		for (i = 0; i < n; i++)
		{
			float red = buf[RLAYER][i];
			float green = buf[GLAYER][i];
			float blue = buf[BLAYER][i];

			red += redShift;
			green += greenShift;
			blue += blueShift;

			red = std::clamp(red, 0.0f, 1.0f);
			green = std::clamp(green, 0.0f, 1.0f);
			blue = std::clamp(blue, 0.0f, 1.0f);

			buf[RLAYER][i] = red;
			buf[GLAYER][i] = green;
			buf[BLAYER][i] = blue;
		}
	}
} // namespace DSS
