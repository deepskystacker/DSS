#pragma once
/****************************************************************************
**
** Copyright (C) 2020, 2022 David C. Partridge
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

namespace DSS
{
	class WCSInfo
	{
	public:


		double crval1{ 0.0 }; // Reference RA in degrees
		double crval2{ 0.0 }; // Reference Dec in degrees
		double cd11{ 0.0 };   // CD matrix element (RA)
		double cd12{ 0.0 };   // CD matrix element (RA)
		double cd21{ 0.0 };   // CD matrix element (Dec)
		double cd22{ 0.0 };   // CD matrix element (Dec)
		double crpix1{ 0.0 }; // Reference pixel X
		double crpix2{ 0.0 }; // Reference pixel Y
		bool ok{ false };

		std::tuple<double, double> pixelToWorld(double x, double y) const;

		std::tuple<double, double> worldToPixel(double ra, double dec) const;

		double rotationAngle() const;

		double pixelScale() const;

		std::tuple <double, double> offsetXY(const WCSInfo& referenceWCSInfo) const;

		double rotationFrom(const WCSInfo& referenceWCSInfo) const;
	};
}
