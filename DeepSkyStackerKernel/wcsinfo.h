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

#include <optional>
#include <tuple>
#include "BilinearParameters.h"

namespace DSS
{
	// The WCS related keywords as read from a FITS header.  A keyword that
	// wasn't present is left as nullopt (or an empty string) so that
	// WCSInfo::setFromKeywords() can apply the defaults the FITS standard
	// defines for it.
	struct WCSKeywords
	{
		std::optional<double> crpix1, crpix2;
		std::optional<double> crval1, crval2;
		std::optional<double> cd11, cd12, cd21, cd22;
		std::optional<double> cdelt1, cdelt2;
		std::optional<double> pc11, pc12, pc21, pc22;
		std::optional<double> crota2;
		QString ctype1, ctype2;
		QString cunit1, cunit2;
	};

	class WCSInfo
	{
	public:
		// Which FITS keywords the CD matrix was obtained from.
		enum class MatrixSource
		{
			None,			// No usable matrix keywords
			CD,				// CDi_j
			CdeltPC,		// CDELTi and PCi_j
			CdeltCrota		// CDELTi and (optionally) CROTA2
		};

		double crval1{ 0.0 };	// Reference RA in degrees
		double crval2{ 0.0 };	// Reference Dec in degrees
		double cd11{ 0.0 };		// CD matrix element (RA)
		double cd12{ 0.0 };		// CD matrix element (RA)
		double cd21{ 0.0 };		// CD matrix element (Dec)
		double cd22{ 0.0 };		// CD matrix element (Dec)
		// The reference pixel and the CD matrix are as read from the FITS
		// header (pixel centres are 1-based, rows in file order) until
		// toBitmapCoordinates() has been called.  After that they refer to
		// the coordinates of the bitmap as DSS holds it in memory.
		double crpix1{ 0.0 };	// Reference pixel X
		double crpix2{ 0.0 };	// Reference pixel Y
		QString creator;		// Software that wrote the FITS file
		MatrixSource matrixSource{ MatrixSource::None };
		bool ok{ false };

		// Populates the reference point and CD matrix from FITS keywords,
		// deriving the CD matrix from CDELT + PC or CDELT + CROTA2 if there
		// are no CD keywords.  Sets and returns ok, which is true only for
		// a complete, invertible RA/Dec TAN projection in degrees.
		bool setFromKeywords(const WCSKeywords& keys);

		// Converts from FITS pixel coordinates to those of the bitmap DSS
		// loads from the file: pixel centres are 0-based, and the rows are
		// reversed if the file is stored bottom-up.  height is NAXIS2.
		void toBitmapCoordinates(int height, bool bottomUp);

		std::tuple<double, double> pixelToWorld(double x, double y) const;

		// Returns nullopt if the CD matrix is singular or the position is
		// not on the visible hemisphere of the tangent plane.
		std::optional<std::tuple<double, double>> worldToPixel(double ra, double dec) const;

		double rotationAngle() const;

		double pixelScale() const;

		// Maps a pixel position in this frame to the position in the
		// reference frame that shows the same point on the sky.
		std::optional<std::tuple<double, double>> mapTo(const WCSInfo& referenceWCSInfo, double x, double y) const;

		// The transformation that maps positions in this frame's rendered
		// bitmap (renderedWidth x renderedHeight) to positions in the
		// reference frame's rendered bitmap.  binning is the number of
		// pixels of the WCS solution along each side of a rendered pixel
		// (2 for a super-pixel de-Bayered image, otherwise 1).
		// Returns nullopt if there is no usable transformation, including
		// when one frame is a mirror image of the other.
		std::optional<CBilinearParameters> transformationTo(const WCSInfo& referenceWCSInfo,
			int renderedWidth, int renderedHeight, int binning = 1, int referenceBinning = 1) const;
	};
}
