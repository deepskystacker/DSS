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
#include "pch.h"
#include "wcsinfo.h"

namespace DSS
{
	std::tuple<double, double> WCSInfo::pixelToWorld(double x, double y) const
	{
		ZASSERT(ok);
		double deltaX = x - crpix1;
		double deltaY = y - crpix2;
		double deltaRA = cd11 * deltaX + cd12 * deltaY;
		double deltaDec = cd21 * deltaX + cd22 * deltaY;
		double ra = crval1 + deltaRA;
		double dec = crval2 + deltaDec;
		return std::make_tuple(ra, dec);
	}

	std::tuple<double, double> WCSInfo::worldToPixel(double ra, double dec) const
	{
		ZASSERT(ok);
		double deltaRA = ra - crval1;
		double deltaDec = dec - crval2;
		double determinant = cd11 * cd22 - cd12 * cd21;
		if (fabs(determinant) < 1e-16)
			return std::make_tuple(0.0, 0.0); // Cannot invert the matrix
		double invCd11 = cd22 / determinant;
		double invCd12 = -cd12 / determinant;
		double invCd21 = -cd21 / determinant;
		double invCd22 = cd11 / determinant;
		double x = crpix1 + invCd11 * deltaRA + invCd12 * deltaDec;
		double y = crpix2 + invCd21 * deltaRA + invCd22 * deltaDec;
		return std::make_tuple(x, y);
	}

	double WCSInfo::rotationAngle() const
	{
		ZASSERT(ok);
		return atan2(cd21, cd11) * 180.0 / M_PI; // degrees
	}

	double WCSInfo::pixelScale() const
	{
		ZASSERT(ok);
		double sx = sqrt(cd11 * cd11 + cd21 * cd21);
		double sy = sqrt(cd12 * cd12 + cd22 * cd22);
		double mean = 0.5 * (sx + sy);
		return mean * 3600.0;		// arcseconds/pixel
	}

	std::tuple <double, double> WCSInfo::offsetXY(const WCSInfo& referenceWCSInfo) const
	{
		ZASSERT(ok);
		double ref_crpix1 = referenceWCSInfo.crpix1;
		double ref_crpix2 = referenceWCSInfo.crpix2;

		// Map this frame's coordinates to the reference frame's pixel coordinates
		auto [ref_x, ref_y] = referenceWCSInfo.worldToPixel(crval1, crval2);

		return std::make_tuple(ref_x - ref_crpix1, ref_y - ref_crpix2);
	}

	double WCSInfo::rotationFrom(const WCSInfo& referenceWCSInfo) const
	{
		ZASSERT(ok);
		double angle = referenceWCSInfo.rotationAngle() - rotationAngle();
		// Normalize the angle to the range [-180, 180]
		while (angle < -180.0) angle += 360.0;
		while (angle > 180.0) angle -= 360.0;
		return angle;
	}
}
