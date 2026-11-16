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
	namespace
	{
		constexpr double degToRad = M_PI / 180.0;
		constexpr double radToDeg = 180.0 / M_PI;
	}

	// The CD matrix converts pixel offsets (from CRPIX) into "intermediate
	// world coordinates" (xi, eta) in degrees on the plane tangent to the
	// sky at (CRVAL1, CRVAL2).  Those must then be deprojected onto the
	// sphere (gnomonic/TAN projection, WCS Paper II section 5.1.3).
	std::tuple<double, double> WCSInfo::pixelToWorld(double x, double y) const
	{
		ZASSERT(ok);
		const double deltaX = x - crpix1;
		const double deltaY = y - crpix2;
		const double xi = (cd11 * deltaX + cd12 * deltaY) * degToRad;
		const double eta = (cd21 * deltaX + cd22 * deltaY) * degToRad;

		const double ra0 = crval1 * degToRad;
		const double dec0 = crval2 * degToRad;
		const double rho = std::hypot(xi, eta);

		if (rho == 0.0)
			return std::make_tuple(crval1, crval2);

		const double c = atan(rho);
		const double sinC = sin(c);
		const double cosC = cos(c);

		const double dec = asin(cosC * sin(dec0) + eta * sinC * cos(dec0) / rho);
		const double ra = ra0 + atan2(xi * sinC, rho * cos(dec0) * cosC - eta * sin(dec0) * sinC);

		double raDeg = fmod(ra * radToDeg, 360.0);
		if (raDeg < 0.0)
			raDeg += 360.0;
		return std::make_tuple(raDeg, dec * radToDeg);
	}

	std::optional<std::tuple<double, double>> WCSInfo::worldToPixel(double ra, double dec) const
	{
		ZASSERT(ok);
		const double determinant = cd11 * cd22 - cd12 * cd21;
		const double norm = fabs(cd11) + fabs(cd12) + fabs(cd21) + fabs(cd22);
		// Relative test: the matrix entries are tiny (degrees/pixel) so an
		// absolute threshold is meaningless.
		if (norm == 0.0 || fabs(determinant) <= 1e-12 * norm * norm)
			return std::nullopt;	// Cannot invert the matrix

		const double ra0 = crval1 * degToRad;
		const double dec0 = crval2 * degToRad;
		const double dRa = ra * degToRad - ra0;
		const double decR = dec * degToRad;

		// Gnomonic projection of the sphere onto the tangent plane.
		const double cosC = sin(dec0) * sin(decR) + cos(dec0) * cos(decR) * cos(dRa);
		if (cosC <= 0.0)
			return std::nullopt;	// Point is >= 90 degrees from the tangent point

		const double xi = cos(decR) * sin(dRa) / cosC * radToDeg;
		const double eta = (cos(dec0) * sin(decR) - sin(dec0) * cos(decR) * cos(dRa)) / cosC * radToDeg;

		// Invert the CD matrix.
		const double x = crpix1 + (cd22 * xi - cd12 * eta) / determinant;
		const double y = crpix2 + (-cd21 * xi + cd11 * eta) / determinant;
		return std::make_tuple(x, y);
	}

	// Rotation of the image Y axis relative to north (degrees).  The Dec
	// axis is used because its scale is positive for normal (east-left)
	// images, whereas cd11 is negative, which would add 180 degrees.
	double WCSInfo::rotationAngle() const
	{
		ZASSERT(ok);
		return atan2(-cd12, cd22) * radToDeg;
	}

	double WCSInfo::pixelScale() const
	{
		ZASSERT(ok);
		// sqrt(|det|) is the geometric mean scale and is unaffected by shear.
		return sqrt(fabs(cd11 * cd22 - cd12 * cd21)) * 3600.0;		// arcseconds/pixel
	}

	std::optional<std::tuple<double, double>> WCSInfo::offsetXY(const WCSInfo& referenceWCSInfo) const
	{
		ZASSERT(ok);

		// This frame's reference pixel (crpix1, crpix2) is at (crval1, crval2).
		// Find where that sky position falls in the reference frame; the
		// translation is the difference between the two pixel positions.
		const auto refPixel = referenceWCSInfo.worldToPixel(crval1, crval2);
		if (!refPixel)
			return std::nullopt;

		const auto [refX, refY] = *refPixel;
		return std::make_tuple(refX - crpix1, refY - crpix2);
	}

	double WCSInfo::rotationFrom(const WCSInfo& referenceWCSInfo) const
	{
		ZASSERT(ok);
		double angle = fmod(referenceWCSInfo.rotationAngle() - rotationAngle(), 360.0);
		// Normalize the angle to the range [-180, 180]
		if (angle < -180.0) angle += 360.0;
		else if (angle > 180.0) angle -= 360.0;
		return angle;
	}
}
