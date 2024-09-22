#include "stdafx.h"
#include "RegisterEngine.h"
#include "PixelTransform.h"

namespace {

	struct CStarAxisInfo final
	{
		int m_lAngle{ 0 };
		double m_fRadius{ 0.0 };
		double m_fSum{ 0.0 };
	};

	inline void NormalizeAngle(int& lAngle)
	{
		while (lAngle >= 360)
			lAngle -= 360;
		while (lAngle < 0)
			lAngle += 360;
	}

	//
	// Calculates the exact position of a star as the center of gravity using the pixel values around the center pixel.
	//
	bool computeStarCenter(const CGrayBitmap& inputBitmap, CStar& star, const double backgroundLevel)
	{
		double fSumX = 0;
		double fSumY = 0;
		double fNrValuesX = 0;
		double fNrValuesY = 0;
		double fAverageX = 0;
		double fAverageY = 0;
		int lNrLines = 0;

		const auto GetCoords = [](const int lo, const int hi)
		{
			return std::views::iota(static_cast<size_t>(lo), static_cast<size_t>(hi) + 1);
		};

		for (const size_t y : GetCoords(star.m_rcStar.top, star.m_rcStar.bottom))
		{
			fSumX = 0;
			fNrValuesX = 0;
			for (const size_t x : GetCoords(star.m_rcStar.left, star.m_rcStar.right))
			{
				double fValue;
				inputBitmap.GetPixel(x, y, fValue);
				fSumX += fValue * x;
				fNrValuesX += fValue;
			}
			if (fNrValuesX > 0)
			{
				lNrLines++;
				fAverageX += fSumX / fNrValuesX;
			}
		}
		fAverageX /= static_cast<double>(lNrLines);

		int lNrColumns = 0;
		for (const size_t x : GetCoords(star.m_rcStar.left, star.m_rcStar.right))
		{
			fSumY = 0;
			fNrValuesY = 0;
			for (const size_t y : GetCoords(star.m_rcStar.top, star.m_rcStar.bottom))
			{
				double fValue;
				inputBitmap.GetPixel(x, y, fValue);
				fSumY += fValue * y;
				fNrValuesY += fValue;
			}
			if (fNrValuesY > 0)
			{
				lNrColumns++;
				fAverageY += fSumY / fNrValuesY;
			}
		}
		fAverageY /= static_cast<double>(lNrColumns);

		star.m_fX = fAverageX;
		star.m_fY = fAverageY;

		// Then compute the radius
		double fSquareSumX = 0;
		double fStdDevX = 0;
		fSumX = 0;
		fNrValuesX = 0;
		const size_t yCoord = std::round(star.m_fY);
		for (const size_t x : GetCoords(star.m_rcStar.left, star.m_rcStar.right))
		{
			double fValue;
			inputBitmap.GetPixel(x, yCoord, fValue);
			fValue = std::max(0.0, fValue - backgroundLevel);
			fSumX += fValue * x;
			fSquareSumX += (x - star.m_fX) * (x - star.m_fX) * fValue;
			fNrValuesX += fValue;
		}
		fStdDevX = std::sqrt(fSquareSumX / fNrValuesX);

		double fSquareSumY = 0;
		double fStdDevY = 0;
		fSumY = 0;
		fNrValuesY = 0;
		const size_t xCoord = std::round(star.m_fX);
		for (const size_t y : GetCoords(star.m_rcStar.top, star.m_rcStar.bottom))
		{
			double fValue;
			inputBitmap.GetPixel(xCoord, y, fValue);
			fValue = std::max(0.0, fValue - backgroundLevel);
			fSumY += fValue * y;
			fSquareSumY += (y - star.m_fY) * (y - star.m_fY) * fValue;
			fNrValuesY += fValue;
		}
		fStdDevY = std::sqrt(fSquareSumY / fNrValuesY);

		// The radius is the average of the standard deviations
		star.m_fMeanRadius = (fStdDevX + fStdDevY) * (1.5 / 2.0);

		return std::abs(fStdDevX - fStdDevY) < CRegisteredFrame::RoundnessTolerance;
	}

	struct PixelDirection
	{
		double m_fIntensity{ 0.0 };
		int m_Radius{ 0 };
		std::int8_t m_lNrBrighterPixels{ 0 };
		std::int8_t m_Ok{ 2 };
		std::int8_t m_lXDir{ 0 };
		std::int8_t m_lYDir{ 0 };

		constexpr PixelDirection(const std::int8_t x, const std::int8_t y) noexcept : m_lXDir{ x }, m_lYDir{ y } {}
		constexpr PixelDirection(const PixelDirection&) noexcept = default;
		constexpr PixelDirection(PixelDirection&&) noexcept = default;
		template <typename T> PixelDirection& operator=(T&&) = delete;
	};

	void findStarShape(const CGrayBitmap& bitmap, CStar& star)
	{
		std::vector<CStarAxisInfo>	vStarAxises;
		double						fMaxHalfRadius = 0.0;
		double						fMaxCumulated = 0.0;
		int						lMaxHalfRadiusAngle = 0.0;

		// Preallocate the vector for the inner loop.
		PIXELDISPATCHVECTOR		vPixels;
		vPixels.reserve(10);

		const int width = bitmap.Width();
		const int height = bitmap.Height();

		for (int lAngle = 0; lAngle < 360; lAngle += 10)
		{
			double					fSquareSum = 0.0;
			double					fSum = 0.0;
			double					fNrValues = 0.0;

			for (double fPos = 0.0; fPos <= star.m_fMeanRadius * 2.0; fPos += 0.10)
			{
				constexpr double GradRadFactor = 3.14159265358979323846 / 180.0;
				const double fX = star.m_fX + std::cos(lAngle * GradRadFactor) * fPos;
				const double fY = star.m_fY + std::sin(lAngle * GradRadFactor) * fPos;
				double fLuminance = 0;

				// Compute luminance at fX, fY
				vPixels.resize(0);
				ComputePixelDispatch(QPointF(fX, fY), vPixels);

				for (const CPixelDispatch& pixel : vPixels)
				{
					if (pixel.m_lX < 0 || pixel.m_lX >= width || pixel.m_lY < 0 || pixel.m_lY >= height)
						continue;

					double fValue;
					bitmap.GetPixel(static_cast<size_t>(pixel.m_lX), static_cast<size_t>(pixel.m_lY), fValue);
					fLuminance += fValue * pixel.m_fPercentage;
				}
				fSquareSum += fPos * fPos * fLuminance * 2;
				fSum += fLuminance;
				fNrValues += fLuminance * 2;
			}

			const double fStdDev = fNrValues > 0.0 ? std::sqrt(fSquareSum / fNrValues) : 0.0;
			CStarAxisInfo ai{ .m_lAngle = lAngle, .m_fRadius = fStdDev * 1.5, .m_fSum = fSum };

			if (ai.m_fSum > fMaxCumulated)
			{
				fMaxCumulated = ai.m_fSum;
				fMaxHalfRadius = ai.m_fRadius;
				lMaxHalfRadiusAngle = ai.m_lAngle;
			}

			vStarAxises.push_back(std::move(ai));
		}

		// Get the biggest value - this is the major axis
		star.m_fLargeMajorAxis = fMaxHalfRadius;
		star.m_fMajorAxisAngle = lMaxHalfRadiusAngle;

		int			lSearchAngle;
		bool			bFound = false;

		lSearchAngle = lMaxHalfRadiusAngle + 180;
		NormalizeAngle(lSearchAngle);

		for (int i = 0; i < vStarAxises.size() && !bFound; i++)
		{
			if (vStarAxises[i].m_lAngle == lSearchAngle)
			{
				bFound = true;
				star.m_fSmallMajorAxis = vStarAxises[i].m_fRadius;
			}
		}

		bFound = false;
		lSearchAngle = lMaxHalfRadiusAngle + 90;
		NormalizeAngle(lSearchAngle);

		for (int i = 0; i < vStarAxises.size() && !bFound; i++)
		{
			if (vStarAxises[i].m_lAngle == lSearchAngle)
			{
				bFound = true;
				star.m_fLargeMinorAxis = vStarAxises[i].m_fRadius;
			}
		}

		bFound = false;
		lSearchAngle = lMaxHalfRadiusAngle + 210;
		NormalizeAngle(lSearchAngle);

		for (int i = 0; i < vStarAxises.size() && !bFound; i++)
		{
			if (vStarAxises[i].m_lAngle == lSearchAngle)
			{
				bFound = true;
				star.m_fSmallMinorAxis = vStarAxises[i].m_fRadius;
			}
		}
	}
}

namespace DSS {

	//
	// RegisterSubRect performs the star detection in the bitmap 'inputBitmap', within the rectangle 'rc', using the threshold 'detectionThreshold'.
	// The found stars will be inserted into the std::set<CStar> 'stars'.
	// 
	// MT, August 2024
	// RegisterSubRect has been improved. Primarily to better work together with the new auto-threshold algorithm for the registration process.
	// The detection threshold can become as low as 0.07%, so we needed a few enhancements.
	// 
	// (*) There is no local buffer for the input pixels anymore, we directly read the inputBitmap. To avoid concurrency issues, inputBitmap is const now.
	// (*) We calculate a local (in the rectangle 'rc') background level using the local histogram of the grey values, rather than using a 
	//     global background level over the entire bitmap.
	// (*) There is a cache for the background level and the maxIntensity value (a simple pair<double,double>*). This avoids calculating these values multiple times 
	//     if RegisterSubrect is called iteratively for different thresholds.
	//     The cache is initialised to -Inf, so in this case background and maxIntensity are calculated and then stored in the cache for the next iteration (if any).
	// (*) We work with the raw grey values in the range [0, 256), so we avoid the continued multiplication with the normalization factor 256.
	// (*) A hot pixel prevention check. This is needed, because at low thresholds, many hot pixels would otherwise be found. They are 
	//     small, bright, and often perfectly circular -> actually seem like a perfect star.
	// (*) For small stars (radius < 10 pixels), we do an additional check for the ratios of the diameters in 2x2 perpendicular directions 
	//     (up/dn - le/ri; and the 45 deg directions).
	//     The diameter in the 2 prependicular directions may not differ more than 1.3 : 1.
	//     This avoids detecting small speckles which are more or less circular and just resulting from a collection of noise around the pixel (i, j).
	//
	// CGrayBitmap is a typedef for CGrayBitmapT<double>
	// So the gray raw values are in the range [0, 256), CGrayBitmap::m_fMultiplier is 256.0.
	// getValue() ant getUncheckedValue() return values in the range [0.0, 256.0).
	// GetPixel() returns values in the range [0.0, 1.0).
	//

	size_t registerSubRect(const CGrayBitmap& inputBitmap, const double detectionThreshold, const DSSRect& rc, STARSET& stars,
		std::pair<double, double>* backgroundLevelCache, const QPointF& comet)
	{
		enum class Dirs {
			Up = 0, Right, Down, Left, UpRight, DnRight, DnLeft, UpLeft
		};

		const bool useCachedValues = backgroundLevelCache != nullptr && std::isfinite(backgroundLevelCache->first);
		double maxIntensity = useCachedValues ? backgroundLevelCache->first : std::numeric_limits<double>::min();
		size_t nStars{ 0 };

		constexpr size_t HistoSize = 256 * 32;
		namespace ranges = std::ranges;
		std::vector<int> histo(useCachedValues ? 0 : HistoSize + 1, 0); // +1 for safety reasons.
		const auto CalcHisto = [&inputBitmap, &histo, &maxIntensity, backgroundLevelCache, useCachedValues](const ranges::view auto xRange, const ranges::view auto yRange)
		{
			if (useCachedValues)
				return;
			for (const size_t y : yRange)
				for (const size_t x : xRange)
				{
					const double value = inputBitmap.getUncheckedValue(x, y); // Range [0, 256)
					maxIntensity = std::max(maxIntensity, value);
					++histo[value * 32.0]; // Implicit type cast generates the fastest code.
				}
			if (backgroundLevelCache != nullptr)
				backgroundLevelCache->first = maxIntensity;
		};

		CalcHisto(std::views::iota(rc.left, rc.right), std::views::iota(rc.top, rc.bottom));

		const auto GetBackgroundValue = [&histo, backgroundLevelCache, useCachedValues](const int width, const int height) -> double
		{
			if (useCachedValues)
				return backgroundLevelCache->second;
			const size_t fiftyPercentValues = (static_cast<size_t>(width) * static_cast<size_t>(height)) / 2 - 1;
			size_t nrValues = 0;
			size_t fiftyPercentQuantile = static_cast<size_t>(-1);
			while (nrValues < fiftyPercentValues)
			{
				++fiftyPercentQuantile;
				nrValues += histo[fiftyPercentQuantile];
			}
			const double v = static_cast<double>(fiftyPercentQuantile) / static_cast<double>(HistoSize);
			if (backgroundLevelCache != nullptr)
				backgroundLevelCache->second = v;
			return v;
		};

		const double backgroundLevel = 256.0 * GetBackgroundValue(rc.width(), rc.height()); // Range [0.0, 256.0)  Background level in inner rectangle.
		const double intensityThreshold = 256.0 * detectionThreshold + backgroundLevel; // Range [0.0, 256.0)

		if (maxIntensity >= intensityThreshold)
		{
			// deltaRadius is the maximum allowed difference (in pixels) of the radii of a star-candidate around its center (in 8 directions).
			// Optimum would be 0 (totally circular star), but we allow up to 3 pixels.
			for (int deltaRadius = 0; deltaRadius < 4; ++deltaRadius)
			{
				for (int j = rc.top; j < rc.bottom; j++)
				{
					for (int i = rc.left; i < rc.right; i++)
					{
						const double fIntensity = inputBitmap.getUncheckedValue(i, j); // [0, 256)

						if (fIntensity >= intensityThreshold)
						{
							bool bNew = true;
							const QPoint ptTest{ i, j };

							// Check that this pixel is not already used for another star.
							for (STARSET::const_iterator it = stars.lower_bound(CStar(ptTest.x() - STARMAXSIZE, 0)); it != stars.cend() && bNew; ++it) // Note: stars are sorted by x-coordinate.
							{
								if (it->IsInRadius(ptTest))
									bNew = false;
								else if (it->m_fX > ptTest.x() + STARMAXSIZE) // Stop if stars are too far away in x-direction.
									break;
							}

							// This pixel at coordinates (i, j) is not yet part of an already detected star.
							if (bNew)
							{
								// Search around the point in 8 directions.
								// If a brighter pixel is found -> NO star (either one pixel 5% brighter OR at least 2 pixels just brighter).
								//
								// Note: DO NOT change the order of these directions, you risk confusing the algorithm! See enum Dirs!
								//
								std::array<PixelDirection, 8> directions{ {
									//  Up       Right   Down    Left     UpRight  DnRight  DnLeft   UpLeft   -> MUST match the enum Dirs!
										{0, -1}, {1, 0}, {0, 1}, {-1, 0}, {1, -1}, {1, 1},  {-1, 1}, {-1, -1}
								} };
								// Set the luminance values of the 8 directions.
								for (auto& testPixel : directions)
								{
									testPixel.m_fIntensity = inputBitmap.getUncheckedValue(i + testPixel.m_lXDir, j + testPixel.m_lYDir); // [0, 256)
								}

								// Hot pixel prevention.
								// The pixel is a hot-pixel, if: of the 8 surrounding pixels, (i) 7 are darker than the center minus background-noise, and (ii) 4 are much darker.
								const auto isHotPixel = [&directions, backgroundLevel, th1 = fIntensity - backgroundLevel, th2 = 0.6 * (fIntensity - backgroundLevel)]() -> bool
								{
									int numberOfDarkerPixels = 0;
									int numberOfMuchDarkerPixels = 0;
									for (const auto& direction : directions)
									{
										const double testValue = direction.m_fIntensity - backgroundLevel;
										if (testValue < th1)
										{
											++numberOfDarkerPixels;
											if (testValue < th2)
												++numberOfMuchDarkerPixels;
										}
									}
									return numberOfDarkerPixels >= 7 && numberOfMuchDarkerPixels >= 4;
								};

								bool bBrighterPixel = false;
								bool bMainOk = !isHotPixel();
								int	lMaxRadius = 0;

								// We search the pixels around the center (i, j) up to a distance of 'STARMAXSIZE'.
								// We'll check, if the center is the brightest pixel and we find much darker pixels around it.
								// If so, then the center will be a star-candidate.
								for (int testedRadius = 1; testedRadius < STARMAXSIZE && bMainOk && !bBrighterPixel; ++testedRadius)
								{
									// Here just set the luminance values of the 8 directions in the distance 'testedRadius'.
									if (testedRadius > 1)
										for (auto& testPixel : directions)
											testPixel.m_fIntensity = inputBitmap.getUncheckedValue(i + testPixel.m_lXDir * testedRadius, j + testPixel.m_lYDir * testedRadius); // [0, 256)

									bMainOk = false;
									for (auto& testPixel : directions) // Check in the 8 directions
									{
										if (bBrighterPixel)
											break;
										if (testPixel.m_Ok) // m_Ok initialized to 2.
										{
											// Is the intensity in this direction and this distance smaller than 25% of the center pixel?
											if (testPixel.m_fIntensity - backgroundLevel < 0.25 * (fIntensity - backgroundLevel))
											{
												testPixel.m_Radius = testedRadius;
												--testPixel.m_Ok;
												lMaxRadius = std::max(lMaxRadius, testedRadius);
											}
											// If we found a pixel brighter than +5% of the center -> stop, NO star at the center pixel.
											else if (testPixel.m_fIntensity > 1.05 * fIntensity)
												bBrighterPixel = true;
											// ELSE just count the number of pixels that are brighter than the center.
											else if (testPixel.m_fIntensity > fIntensity)
												++testPixel.m_lNrBrighterPixels;
										}
										// As long as we did not yet find at least 2 pixels darker than 25% of the center -> cannot be a star.
										if (testPixel.m_Ok)
											bMainOk = true;
										if (testPixel.m_lNrBrighterPixels > 2) // If at least 2 pixels are brighter than the center -> NO star.
											bBrighterPixel = true;
									} // Loop over 8 test directions.
								}
								//
								// If bMainOk == false -> there is a candidate star at the center pixel (i, j).
								// This is the case, if
								//   Max. 1 pixel brighter than the center, no pixel brighter than +5%.
								//   In every of the 8 test directions we found 2 pixels that are darker than 25% of the center (above the background level).
								//   The largest distance (over all directions) of such a darker pixel is at least 2 pixels (so stars cannot be too small).
								//   - Additionally we stored for every direction the distance of the second of the darker pixels in m_Radius.
								//
								// Now, check the circularity (also called 'roundness') by evaluating the 
								// radius (see above, the distance of the second of the darker pixels) and 
								// comparing it for each direction to the other directions.
								//
								if (!bMainOk && !bBrighterPixel && (lMaxRadius > 2)) // We found darker pixels, no brighter pixels, candidate is not too small.
								{
									int maxDeltaRadii = 0;
									const auto compareDeltaRadii = [deltaRadius, &directions, &maxDeltaRadii](std::ranges::viewable_range auto dirs) -> bool
									{
										bool OK = true;
										for (const Dirs k1 : dirs)
										{
											const auto radiusToCompare = directions[static_cast<size_t>(k1)].m_Radius;
											for (const Dirs k2 : dirs)
											{
												const int deltaR = std::abs(directions[static_cast<size_t>(k2)].m_Radius - radiusToCompare);
												maxDeltaRadii = std::max(maxDeltaRadii, deltaR);
												OK = OK && (deltaR <= deltaRadius); // DeltaRadius is the max. allowed difference of radii in all directions (outer loop 0 -> 4).
											}
										}
										return OK;
									};
									// Compare directions up, down, left, right: delta of radii must be smaller than deltaRadius (loop 0 -> 4)
									// Compare the 4 diagonal directions: delta of radii must also be smaller than deltaRadius.
									bool validCandidate =
										compareDeltaRadii(std::array{ Dirs::Up, Dirs::Right, Dirs::Down, Dirs::Left })
										&& compareDeltaRadii(std::array{ Dirs::UpRight, Dirs::DnRight, Dirs::DnLeft, Dirs::UpLeft });

									// Additional check for super-small stars, which could be "larger" hot-pixels or just noise.
									// The ratio of the radii must not be too large.
									const auto checkDiameterRatio = [lMaxRadius, &directions](const Dirs d1, const Dirs d2, const Dirs d3, const Dirs d4) -> bool
									{
										if (lMaxRadius > 10)
											return true;
										const auto diameter1 = directions[static_cast<size_t>(d1)].m_Radius + directions[static_cast<size_t>(d2)].m_Radius;
										const auto diameter2 = directions[static_cast<size_t>(d3)].m_Radius + directions[static_cast<size_t>(d4)].m_Radius;
										const double ratio1 = diameter1 != 0 ? diameter2 / static_cast<double>(diameter1) : 0; // 0 if one of the diameters is 0
										const double ratio2 = diameter2 != 0 ? diameter1 / static_cast<double>(diameter2) : 0; // 0 if one of the diameters is 0
										return ratio1 <= 1.3 && ratio2 <= 1.3;
									};
									validCandidate = validCandidate
										&& checkDiameterRatio(Dirs::Up, Dirs::Down, Dirs::Right, Dirs::Left)
										&& checkDiameterRatio(Dirs::UpRight, Dirs::DnLeft, Dirs::DnRight, Dirs::UpLeft);

									const double fMeanRadius1 = // top, bottom, left, right
										std::accumulate(directions.cbegin(), directions.cbegin() + 4, 0.0, [](const double acc, const PixelDirection& d) { return acc + d.m_Radius; })
										/ 4.0;
									const double fMeanRadius2 = // the four 45-degrees-direction
										std::accumulate(directions.cbegin() + 4, directions.cend(), 0.0, [](const double acc, const PixelDirection& d) { return acc + d.m_Radius; })
										* 0.3535533905932737622; // sqrt(2.0) / 4.0;

									int	lLeftRadius = 0; // Largest extension of star to the left of the center.
									int	lRightRadius = 0; // Largest extension of star to the right of the center.
									int	lTopRadius = 0; // Largest extension of star above the center.
									int	lBottomRadius = 0; // Largest extension of star below the center.

									for (const auto& testPixel : directions)
									{
										if (testPixel.m_lXDir < 0)
											lLeftRadius = std::max(lLeftRadius, static_cast<int>(testPixel.m_Radius));
										else if (testPixel.m_lXDir > 0)
											lRightRadius = std::max(lRightRadius, static_cast<int>(testPixel.m_Radius));
										if (testPixel.m_lYDir < 0)
											lTopRadius = std::max(lTopRadius, static_cast<int>(testPixel.m_Radius));
										else if (testPixel.m_lYDir > 0)
											lBottomRadius = std::max(lBottomRadius, static_cast<int>(testPixel.m_Radius));
									}
									//
									// **********************************
									// * Danger! Danger, Will Robinson! *
									// **********************************
									// 
									// This rectangle is INCLUSIVE so the loops over the m_rcStar rectangle MUST use
									// <= limit, not the more normal < limit.
									// 
									// It is not safe to change this as the data is saved in the .info.txt files
									// that the code reads during "Compute Offsets" processing.
									// 
									// So, while technically incorrect, it must not be changed otherwise it would
									// create an incompatibility with existing info.txt files that were created by
									// earlier releases of the code.
									//
									// MT, August 2024: The ONLY function still using star.m_rcStar ist:
									//    CDarkFrame::FillExcludedPixelList(const STARVECTOR * pStars, EXCLUDEDPIXELVECTOR & vExcludedPixels);
									// Every other function which used to use m_rcStar is now commented out -> not any more used.
									// Ironically, this function uses the ("correct" ?) limits with < NOT <= :
									// See DarkFrame.cpp, line 387:
									//    for (int x = rcStar.left; x < rcStar.right; x++) 
									//       for (int y = rcStar.top; y < rcStar.bottom; y++)
									// 
									// The new CStar::m_fCircularity parameter:
									// It measures how circular ("round") a star is by dividing the brightness above noise by (0.1 + maxDeltaRadii), where maxDeltaRadii is
									// the maximum difference of the radii in pixels of the star around the center in the 8 test-directions.
									// So stars which are perfectly round will be weighted by 10 (1/0.1), the others by 0.91 or less.
									//
									if (validCandidate)
									{
										// The new star to add:
										CStar ms(ptTest.x(), ptTest.y());
										ms.m_fIntensity = fIntensity / 256.0;
										ms.m_rcStar = DSSRect{ ptTest.x() - lLeftRadius, ptTest.y() - lTopRadius, ptTest.x() + lRightRadius, ptTest.y() + lBottomRadius };
										ms.m_fPercentage = 1.0;
										ms.m_fCircularity = (fIntensity - backgroundLevel) / (0.1 + maxDeltaRadii); // MT, Aug. 2024: m_fDeltaRadius was not used anywhere.
										ms.m_fMeanRadius = (fMeanRadius1 + fMeanRadius2) / 2.0;

										// Compute the real position (correct m_fX, m_fY, m_fMeanRadius).
										if (computeStarCenter(inputBitmap, ms, backgroundLevel * (1.0 / 256.0)))
										{
											// Check last overlap condition
											{
												constexpr double RadiusFactor = CRegisteredFrame::RadiusFactor;
												for (STARSET::const_iterator it = stars.lower_bound(CStar(ms.m_fX - ms.m_fMeanRadius * RadiusFactor - STARMAXSIZE, 0)); it != stars.cend() && validCandidate; ++it)
												{
													// If the candidate is closer to one of the already found stars -> NO candidate any more.
													if (Distance(ms.m_fX, ms.m_fY, it->m_fX, it->m_fY) < (ms.m_fMeanRadius + it->m_fMeanRadius) * RadiusFactor)
														validCandidate = false;
													else if (it->m_fX > ms.m_fX + ms.m_fMeanRadius * RadiusFactor + STARMAXSIZE) // Stop if stars to compare are too far away in x-direction.
														break;
												}
											}
											// Check comet intersection
											if (std::isfinite(comet.x()) && ms.IsInRadius(comet))
												validCandidate = false;

											if (validCandidate)
											{
												ms.m_fQuality = (10 - deltaRadius) + fIntensity / 256.0 - ms.m_fMeanRadius;
												findStarShape(inputBitmap, ms);
												stars.insert(std::move(ms));
												++nStars;
											}
										}
									}
								}
							} // Pixel not already used for another star
						} // fIntensity >= intensityThreshold ?
					} // for i left -> right
				} // for j top -> bottom
			} // for deltaradius 0 -> 4
		}

		return nStars;
	}
}
