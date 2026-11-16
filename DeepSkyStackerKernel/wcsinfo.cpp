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
		constexpr double degToRad = std::numbers::pi / 180.0;
		constexpr double radToDeg = 180.0 / std::numbers::pi;

		// Relative test: the matrix entries are tiny (degrees/pixel) so an
		// absolute threshold is meaningless.  Non-finite values count as
		// singular too.
		bool isSingular(double cd11, double cd12, double cd21, double cd22)
		{
			const double determinant = cd11 * cd22 - cd12 * cd21;
			const double norm = fabs(cd11) + fabs(cd12) + fabs(cd21) + fabs(cd22);
			return !std::isfinite(norm) || norm == 0.0 || !(fabs(determinant) > 1e-12 * norm * norm);
		}

		// CTYPE is "cccc-ppp": a four character coordinate type padded with
		// '-' ("RA--", "DEC-"), then '-' and a three character projection
		// code.  A distortion suffix such as "-SIP" may follow, which is
		// accepted (the distortion terms themselves are ignored).
		bool isTanAxis(const QString& ctype, QLatin1StringView coordinate)
		{
			const QString type = ctype.trimmed().toUpper();
			return type.startsWith(coordinate) && type.mid(4, 4) == QLatin1StringView("-TAN");
		}

		// CUNIT defaults to degrees for celestial axes if it is absent.
		bool isDegrees(const QString& cunit)
		{
			const QString unit = cunit.trimmed().toLower();
			return unit.isEmpty() || unit == QLatin1StringView("deg") ||
				unit == QLatin1StringView("degree") || unit == QLatin1StringView("degrees");
		}
	}

	// The FITS WCS standard (Greisen & Calabretta, papers I and II) allows
	// the pixel to intermediate world coordinate transformation to be given
	// in three ways.  In order of precedence here:
	//
	//  1. CDi_j		Used directly.  If any one is present the missing
	//					ones default to 0.
	//  2. CDELTi, PCi_j	CDi_j = CDELTi * PCi_j.  Missing PCi_j default to
	//					the identity matrix and missing CDELTi to 1.
	//  3. CDELTi, CROTA2	The older AIPS convention, with rho = CROTA2
	//					(0 if absent, i.e. CDELT alone is an unrotated image):
	//						CD1_1 =  CDELT1 * cos(rho)
	//						CD1_2 = -CDELT2 * sin(rho)
	//						CD2_1 =  CDELT1 * sin(rho)
	//						CD2_2 =  CDELT2 * cos(rho)
	bool WCSInfo::setFromKeywords(const WCSKeywords& keys)
	{
		crpix1 = keys.crpix1.value_or(0.0);
		crpix2 = keys.crpix2.value_or(0.0);
		crval1 = keys.crval1.value_or(0.0);
		crval2 = keys.crval2.value_or(0.0);

		// CRPIX and CRVAL do have defaults (0), but a header without them
		// hasn't been plate solved, so don't claim to have a WCS solution.
		const bool haveReference = keys.crpix1 && keys.crpix2 && keys.crval1 && keys.crval2;

		matrixSource = MatrixSource::None;
		cd11 = cd12 = cd21 = cd22 = 0.0;
		const double cdelt1 = keys.cdelt1.value_or(1.0);
		const double cdelt2 = keys.cdelt2.value_or(1.0);

		if (keys.cd11 || keys.cd12 || keys.cd21 || keys.cd22)
		{
			matrixSource = MatrixSource::CD;
			cd11 = keys.cd11.value_or(0.0);
			cd12 = keys.cd12.value_or(0.0);
			cd21 = keys.cd21.value_or(0.0);
			cd22 = keys.cd22.value_or(0.0);
		}
		else if (keys.pc11 || keys.pc12 || keys.pc21 || keys.pc22)
		{
			matrixSource = MatrixSource::CdeltPC;
			cd11 = cdelt1 * keys.pc11.value_or(1.0);
			cd12 = cdelt1 * keys.pc12.value_or(0.0);
			cd21 = cdelt2 * keys.pc21.value_or(0.0);
			cd22 = cdelt2 * keys.pc22.value_or(1.0);
		}
		else if (keys.cdelt1 && keys.cdelt2)
		{
			matrixSource = MatrixSource::CdeltCrota;
			const double rho = keys.crota2.value_or(0.0) * degToRad;
			cd11 = cdelt1 * cos(rho);
			cd12 = -cdelt2 * sin(rho);
			cd21 = cdelt1 * sin(rho);
			cd22 = cdelt2 * cos(rho);
		}
		const bool haveMatrix = (matrixSource != MatrixSource::None);

		// pixelToWorld() and worldToPixel() assume axis 1 is RA and axis 2
		// is Dec, a gnomonic (TAN) projection, and degrees.
		const bool supported =
			isTanAxis(keys.ctype1, QLatin1StringView("RA--")) &&
			isTanAxis(keys.ctype2, QLatin1StringView("DEC-")) &&
			isDegrees(keys.cunit1) && isDegrees(keys.cunit2);

		ok = haveReference && haveMatrix && supported && !isSingular(cd11, cd12, cd21, cd22);
		return ok;
	}

	// FITS numbers pixels from 1 with the pixel centre at the integer
	// value, DSS from 0, so the reference pixel moves by one.
	//
	// The WCS solution always describes the pixels in the order they are
	// stored in the file.  DSS reverses the rows of a bottom-up file when
	// it loads it, so bitmap row = height - FITS row.  That mirrors the Y
	// axis, which negates the second column of the CD matrix (the
	// derivatives with respect to Y).
	void WCSInfo::toBitmapCoordinates(int height, bool bottomUp)
	{
		crpix1 -= 1.0;
		if (bottomUp)
		{
			crpix2 = height - crpix2;
			cd12 = -cd12;
			cd22 = -cd22;
		}
		else
			crpix2 -= 1.0;
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

		double ra = ra0;
		double dec = dec0;
		if (rho != 0.0)
		{
			const double c = atan(rho);
			const double sinC = sin(c);
			const double cosC = cos(c);

			// Clamped because rounding can take the value marginally outside
			// [-1, 1] close to the poles, for which asin() returns NaN.
			dec = asin(std::clamp(cosC * sin(dec0) + eta * sinC * cos(dec0) / rho, -1.0, 1.0));
			ra = ra0 + atan2(xi * sinC, rho * cos(dec0) * cosC - eta * sin(dec0) * sinC);
		}

		// Normalise RA to [0, 360).  Adding 360 to a tiny negative value
		// rounds to exactly 360, hence the final test.
		double raDeg = fmod(ra * radToDeg, 360.0);
		if (raDeg < 0.0)
			raDeg += 360.0;
		if (raDeg >= 360.0)
			raDeg = 0.0;
		return std::make_tuple(raDeg, dec * radToDeg);
	}

	std::optional<std::tuple<double, double>> WCSInfo::worldToPixel(double ra, double dec) const
	{
		ZASSERT(ok);
		const double determinant = cd11 * cd22 - cd12 * cd21;
		if (isSingular(cd11, cd12, cd21, cd22))
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

	// Direction of the image +Y axis, in degrees from north, positive
	// towards west (the same sense as the FITS CROTA2 keyword).  The Dec
	// row of the matrix is used because its scale is positive for normal
	// (east-left) images, whereas cd11 is negative, which would add 180
	// degrees.
	// This only describes one axis, so it says nothing about whether the
	// image is mirrored: the difference of two of these is NOT in general
	// the rotation between two frames, use transformationTo() for that.
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

	std::optional<std::tuple<double, double>> WCSInfo::mapTo(const WCSInfo& referenceWCSInfo, double x, double y) const
	{
		ZASSERT(ok);
		ZASSERT(referenceWCSInfo.ok);
		const auto [ra, dec] = pixelToWorld(x, y);
		return referenceWCSInfo.worldToPixel(ra, dec);
	}

	// Both frames are gnomonic projections of the same sky, so the exact
	// mapping between them is a projective one (straight lines stay
	// straight).  For frames that overlap enough to be stacked that is
	// very nearly affine, and the DSS "bilinear" transformation
	//
	//		x' = W * (a0 + a1*X + a2*Y + a3*X*Y)		X = x / W
	//		y' = H * (b0 + b1*X + b2*Y + b3*X*Y)		Y = y / H
	//
	// represents it well.  It has four coefficients per axis, so they are
	// determined exactly by where the four corners (X, Y = 0 or 1) of this
	// frame fall in the reference frame.  Working from mapped points rather
	// than from the two CD matrices also accounts for the rotation of the
	// field between two different tangent points (large near the poles).
	//
	// With super-pixel de-Bayering each rendered pixel covers 2x2 pixels of
	// the image the WCS solution describes.  Rendered pixel r is centred
	// on WCS pixel binning * r + (binning - 1) / 2.
	std::optional<CBilinearParameters> WCSInfo::transformationTo(const WCSInfo& referenceWCSInfo,
		int renderedWidth, int renderedHeight, int binning, int referenceBinning) const
	{
		ZASSERT(ok);
		ZASSERT(referenceWCSInfo.ok);
		if (renderedWidth <= 0 || renderedHeight <= 0 || binning <= 0 || referenceBinning <= 0)
			return std::nullopt;

		const double width = renderedWidth;
		const double height = renderedHeight;
		const double binOffset = (binning - 1) / 2.0;
		const double referenceBinOffset = (referenceBinning - 1) / 2.0;

		// Where the corner at (X, Y) lands, normalised in the same way as
		// the result of the bilinear polynomial.
		const auto mapCorner = [&](double X, double Y) -> std::optional<std::tuple<double, double>>
			{
				const auto mapped = mapTo(referenceWCSInfo,
					binning * X * width + binOffset, binning * Y * height + binOffset);
				if (!mapped)
					return std::nullopt;
				const auto [x, y] = *mapped;
				return std::make_tuple(
					(x - referenceBinOffset) / referenceBinning / width,
					(y - referenceBinOffset) / referenceBinning / height);
			};

		const auto corner00 = mapCorner(0.0, 0.0);
		const auto corner10 = mapCorner(1.0, 0.0);
		const auto corner01 = mapCorner(0.0, 1.0);
		const auto corner11 = mapCorner(1.0, 1.0);
		if (!corner00 || !corner10 || !corner01 || !corner11)
			return std::nullopt;

		const auto [x00, y00] = *corner00;
		const auto [x10, y10] = *corner10;
		const auto [x01, y01] = *corner01;
		const auto [x11, y11] = *corner11;

		CBilinearParameters parameters;		// Constructed as TT_BILINEAR
		parameters.fXWidth = width;
		parameters.fYWidth = height;
		parameters.a0 = x00;
		parameters.a1 = x10 - x00;
		parameters.a2 = x01 - x00;
		parameters.a3 = x11 - x10 - x01 + x00;
		parameters.b0 = y00;
		parameters.b1 = y10 - y00;
		parameters.b2 = y01 - y00;
		parameters.b3 = y11 - y10 - y01 + y00;

		// The sign of the Jacobian determinant at the centre of the frame
		// (X = Y = 0.5) tells us about parity: if it is negative one frame
		// is a mirror image of the other (e.g. one of them was taken
		// through a star diagonal, or stored in the other row order without
		// saying so), and no rotation can align them.  The different
		// normalisation of the two axes scales the off-diagonal terms by
		// H/W and W/H, which cancels in the determinant.
		const double determinant =
			(parameters.a1 + 0.5 * parameters.a3) * (parameters.b2 + 0.5 * parameters.b3) -
			(parameters.a2 + 0.5 * parameters.a3) * (parameters.b1 + 0.5 * parameters.b3);
		if (!std::isfinite(determinant) || determinant <= 0.0)
			return std::nullopt;

		return parameters;
	}
}
