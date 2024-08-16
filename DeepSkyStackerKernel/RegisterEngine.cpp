
#include "stdafx.h"
#include "RegisterEngine.h"
#include "Workspace.h"
#include "PixelTransform.h"
#include "Ztrace.h"
#include "BackgroundCalibration.h"
#include "Multitask.h"
#include "avx_luminance.h"
#include "ColorHelpers.h"
#include "Filters.h"
#include "StackingTasks.h"
#include "FITSUtil.h"
#include "TIFFUtil.h"
#include "MasterFrames.h"

/* ------------------------------------------------------------------- */

class CStarAxisInfo final
{
public :
	int m_lAngle{ 0 };
	double m_fRadius{ 0.0 };
	double m_fSum{ 0.0 };

public:
	CStarAxisInfo(const int angle, const double rad, const double sum):
		m_lAngle{ angle },
		m_fRadius{ rad },
		m_fSum{ sum }
	{}
	CStarAxisInfo(const CStarAxisInfo&) = default;
	~CStarAxisInfo() = default;
	CStarAxisInfo& operator=(const CStarAxisInfo&) = default;
};

inline	void NormalizeAngle(int & lAngle)
{
	while (lAngle >= 360)
		lAngle -= 360;
	while (lAngle < 0)
		lAngle += 360;
};

/* ------------------------------------------------------------------- */
void CRegisteredFrame::Reset()
{
	Workspace			workspace;

	m_vStars.clear();

	m_bInfoOk = false;

	m_bComet = false;
	m_fXComet = m_fYComet = -1;

	m_fMinLuminancy = workspace.value("Register/DetectionThreshold").toDouble() / 100.0;

	m_bApplyMedianFilter = workspace.value("Register/ApplyMedianFilter").toBool();
	m_fBackground = 0.0;

	m_SkyBackground.Reset();

	m_fOverallQuality = 0;
	m_fFWHM = 0;
	meanQuality = 0;
}

bool CRegisteredFrame::FindStarShape(const CMemoryBitmap* pBitmap, CStar& star)
{
	bool						bResult = false;
	std::vector<CStarAxisInfo>	vStarAxises;
	double						fMaxHalfRadius = 0.0;
	double						fMaxCumulated  = 0.0;
	int						lMaxHalfRadiusAngle = 0.0;

	// Preallocate the vector for the inner loop.
	PIXELDISPATCHVECTOR		vPixels;
	vPixels.reserve(10);

	const int width = pBitmap->Width();
	const int height = pBitmap->Height();

	for (int lAngle = 0; lAngle < 360; lAngle += 10)
	{
		double					fSquareSum = 0.0;
		double					fSum	   = 0.0;
		double					fNrValues  = 0.0;

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
				pBitmap->GetPixel(static_cast<size_t>(pixel.m_lX), static_cast<size_t>(pixel.m_lY), fValue);
				fLuminance += fValue * pixel.m_fPercentage;
			}
			fSquareSum += fPos * fPos * fLuminance * 2;
			fSum += fLuminance;
			fNrValues += fLuminance * 2;
		}

		const double fStdDev = fNrValues > 0.0 ? std::sqrt(fSquareSum / fNrValues) : 0.0;
		CStarAxisInfo ai{ lAngle, fStdDev * 1.5, fSum };

		if (ai.m_fSum > fMaxCumulated)
		{
			fMaxCumulated		= ai.m_fSum;
			fMaxHalfRadius		= ai.m_fRadius;
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

	for (int i = 0;i<vStarAxises.size() && !bFound;i++)
	{
		if (vStarAxises[i].m_lAngle == lSearchAngle)
		{
			bFound = true;
			star.m_fSmallMajorAxis = vStarAxises[i].m_fRadius;
		}
	}

	bFound		 = false;
	lSearchAngle = lMaxHalfRadiusAngle + 90;
	NormalizeAngle(lSearchAngle);

	for (int i = 0;i<vStarAxises.size() && !bFound;i++)
	{
		if (vStarAxises[i].m_lAngle == lSearchAngle)
		{
			bFound = true;
			star.m_fLargeMinorAxis = vStarAxises[i].m_fRadius;
		}
	}

	bFound		 = false;
	lSearchAngle = lMaxHalfRadiusAngle + 210;
	NormalizeAngle(lSearchAngle);

	for (int i = 0;i<vStarAxises.size() && !bFound;i++)
	{
		if (vStarAxises[i].m_lAngle == lSearchAngle)
		{
			bFound = true;
			star.m_fSmallMinorAxis = vStarAxises[i].m_fRadius;
		}
	}

	return bResult;
}

void CRegisteredFrame::ComputeOverallQuality()
{
	m_fOverallQuality = std::accumulate(m_vStars.cbegin(), m_vStars.cend(), 0.0, [](const double accu, const CStar& star) { return accu + star.m_fQuality; });

	std::vector<int> indexes(m_vStars.size());
	std::iota(indexes.begin(), indexes.end(), 0);
	const auto Projector = [&stars = this->m_vStars](const int ndx) { return stars[ndx].m_fQuality; };
	std::ranges::sort(indexes, std::greater{}, Projector); // Sort descending by quality of stars.
	constexpr std::array<double, 12> weights = {10.0, 10.0, 10.0, 9.0, 9.0, 8.0, 6.0, 4.0, 2.0, 2.0, 2.0, 1.5 };
	double sumWeights = 0;
	double sum = 0;
	for (int ndx = 0; const double q : std::views::transform(indexes, Projector))
	{
		const double w = ndx < 12 ? weights[ndx] : (ndx < 20 ? 1.0 : 0.2); // Star 0..11 weights-from-above, star 12..19 weight=1, then weight=0.2
		sumWeights += w;
		sum += w * q;
		++ndx;
	}

	this->meanQuality = sumWeights != 0 ? sum / sumWeights : 0.0;
}


namespace {

	bool computeStarCenter(const CGrayBitmap& inputBitmap, double& fX, double& fY, double& fRadius, const double backgroundLevel)
	{
		double fSumX = 0;
		double fSumY = 0;
		double fNrValuesX = 0;
		double fNrValuesY = 0;
		double fAverageX = 0;
		double fAverageY = 0;
		int lNrLines = 0;

		for (int j = fY - fRadius; j <= fY + fRadius; j++)
		{
			fSumX = 0;
			fNrValuesX = 0;
			for (int i = fX - fRadius; i <= fX + fRadius; i++)
			{
				double			fValue;

				inputBitmap.GetPixel(i, j, fValue);
				fSumX += fValue * i;
				fNrValuesX += fValue;
			}
			if (fNrValuesX)
			{
				lNrLines++;
				fAverageX += fSumX/fNrValuesX;
			}
		}
		fAverageX /= static_cast<double>(lNrLines);

		int lNrColumns = 0;
		for (int j = fX - fRadius; j <= fX + fRadius; j++)
		{
			fSumY = 0;
			fNrValuesY = 0;
			for (int i = fY - fRadius; i <= fY + fRadius; i++)
			{
				double fValue;
				inputBitmap.GetPixel(j, i, fValue);
				fSumY += fValue * i;
				fNrValuesY += fValue;
			}
			if (fNrValuesY)
			{
				lNrColumns++;
				fAverageY += fSumY/fNrValuesY;
			}
		}
		fAverageY /= static_cast<double>(lNrColumns);

		fX = fAverageX;
		fY = fAverageY;

		// Then compute the radius
		double fSquareSumX = 0;
		double fStdDevX = 0;
		fSumX = 0;
		fNrValuesX = 0;
		for (int i = fX - fRadius; i <= fX + fRadius; i++)
		{
			double fValue;
			inputBitmap.GetPixel(i, fY, fValue);
			fValue = std::max(0.0, fValue - backgroundLevel);
			fSumX		+= fValue * i;
			fSquareSumX += (i - fX) * (i - fX) * fValue;
			fNrValuesX	+= fValue;
		}
		fStdDevX = std::sqrt(fSquareSumX / fNrValuesX);

		double fSquareSumY = 0;
		double fStdDevY = 0;
		fSumY = 0;
		fNrValuesY = 0;
		for (int i = fY - fRadius; i <= fY + fRadius; i++)
		{
			double fValue;
			inputBitmap.GetPixel(fX, i, fValue);
			fValue = std::max(0.0, fValue - backgroundLevel);
			fSumY		+= fValue * i;
			fSquareSumY += (i - fY) * (i - fY) * fValue;
			fNrValuesY	+= fValue;
		}
		fStdDevY = std::sqrt(fSquareSumY / fNrValuesY);

		// The radius is the average of the standard deviations
		fRadius = (fStdDevX + fStdDevY) * (1.5 / 2.0);

		return std::abs(fStdDevX - fStdDevY) < CRegisteredFrame::m_fRoundnessTolerance;
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
}

//
// CGrayBitmap is a typedef for CGrayBitmapT<double>
// So the gray raw values are in the range [0, 256), CGrayBitmap::m_fMultiplier is 256.0.
// getValue() returns values in the range [0.0, 256.0).
// GetPixel() returns values in the range [0.0, 1.0).
//
size_t CRegisteredFrame::RegisterSubRect(const CGrayBitmap& inputBitmap, const double detectionThreshold, const DSSRect& rc, STARSET& stars, std::vector<double>& buffer)
{
	double maxIntensity = std::numeric_limits<double>::min();
	size_t nStars{ 0 };

	// Work with a local buffer. Copy the pixel values for the rect.
	const int width = rc.width();
	for (size_t j = rc.top, ndx = 0; j < rc.bottom; ++j)
	{
		for (size_t i = rc.left; i < rc.right; ++i, ++ndx)
		{
			const double value = inputBitmap.getValue(i, j); // Range [0, 256)
			buffer[ndx] = value;
			maxIntensity = std::max(maxIntensity, value);
		}
	}

	const auto getValue = [&buffer, top = rc.top, left = rc.left, width](const int x, const int y) -> double
	{
		return buffer[(y - top) * width + x - left];
	};

	const auto getBackgroundValue = [&buffer](const int width, const int height) -> double
	{
		constexpr size_t Maxvalue = size_t{ std::numeric_limits<std::uint16_t>::max() };
		constexpr size_t Maxsize = Maxvalue + 1;
		std::vector<int> vHistogram(Maxsize, 0);
		for (const auto value : std::views::take(buffer, width * height))
		{
			++vHistogram[std::min(static_cast<size_t>(value * 256.0), Maxvalue)]; // Indexes (=luminance values) are in range [0, 65535]
		}

		const int fiftyPercentValues = ((width - 1) * (height - 1)) / 2;
		int nrValues = 0;
		int fiftyPercentQuantile = 0;
		while (nrValues < fiftyPercentValues)
		{
			nrValues += vHistogram[fiftyPercentQuantile];
			++fiftyPercentQuantile;
		}

		return static_cast<double>(fiftyPercentQuantile) / static_cast<double>(Maxsize);
	};

	const double backgroundLevel = 256.0 * getBackgroundValue(width, rc.height()); // Range [0.0, 256.0)
	const double intensityThreshold = 256.0 * detectionThreshold + backgroundLevel; // Range [0.0, 256.0)

	if (maxIntensity >= intensityThreshold)
	{
		// Find how many wanabee stars are existing above 90% maximum luminance

		for (int deltaRadius = 0; deltaRadius < 4; ++deltaRadius)
		{
			for (int j = rc.top; j < rc.bottom; j++)
			{
				for (int i = rc.left; i < rc.right; i++)
				{
					const double fIntensity = getValue(i, j); // [0, 256)

					if (fIntensity >= intensityThreshold)
					{
						// Check that this pixel is not already used in a wanabee star
						bool bNew = true;
						const QPoint ptTest{ i, j };

						for (STARSET::const_iterator it = stars.lower_bound(CStar(ptTest.x() - STARMAXSIZE, 0)); it != stars.cend() && bNew; ++it) // Note: stars are sorted by x-coordinate.
						{
							if (it->IsInRadius(ptTest))
								bNew = false;
							else if (it->m_fX > ptTest.x() + STARMAXSIZE) // Stop if stars are too far away in x-direction.
								break;
						}

						if (bNew)
						{
							// Search around the point until intensity is divided by 2
							// STARMAXSIZE pixels radius max search
							std::array<PixelDirection, 8> directions{ {
								{0, -1}, {1, 0}, {0, 1}, {-1, 0}, {1, -1}, {1, 1}, {-1, 1}, {-1, -1}
							} };

							bool bBrighterPixel = false;
							bool bMainOk = true;
							int	lMaxRadius = 0;

							for (int testedRadius = 1; testedRadius < STARMAXSIZE && bMainOk && !bBrighterPixel; ++testedRadius)
							{
								for (auto& testPixel : directions)
								{
									inputBitmap.GetValue(i + testPixel.m_lXDir * testedRadius, j + testPixel.m_lYDir * testedRadius, testPixel.m_fIntensity); // [0, 256)
								}

								bMainOk = false;
								for (auto& testPixel : directions)
								{
									if (bBrighterPixel) break;
									if (testPixel.m_Ok)
									{
										if (testPixel.m_fIntensity - backgroundLevel < 0.25 * (fIntensity - backgroundLevel))
										{
											testPixel.m_Radius = testedRadius;
											--testPixel.m_Ok;
											lMaxRadius = std::max(lMaxRadius, testedRadius);
										}
										else if (testPixel.m_fIntensity > 1.05 * fIntensity)
											bBrighterPixel = true;
										else if (testPixel.m_fIntensity > fIntensity)
											++testPixel.m_lNrBrighterPixels;
									}

									if (testPixel.m_Ok)
										bMainOk = true;
									if (testPixel.m_lNrBrighterPixels > 2)
										bBrighterPixel = true;
								}
							}

							// Check the roundness of the wanabee star
							if (!bMainOk && !bBrighterPixel && (lMaxRadius > 2))
							{
								// Radiuses should be within deltaRadius pixels of each others
								//if (i>=1027 && i<=1035 && j>=2365 && j<=2372)
								//	DebugBreak();

								bool bWanabeeStarOk = true;

								for (size_t k1 = 0; (k1 < 4) && bWanabeeStarOk; k1++)
								{
									for (size_t k2 = 0; (k2 < 4) && bWanabeeStarOk; k2++)
									{
										if ((k1 != k2) && std::abs(directions[k2].m_Radius - directions[k1].m_Radius) > deltaRadius)
											bWanabeeStarOk = false;
									}
								}
								for (size_t k1 = 4; (k1 < 8) && bWanabeeStarOk; k1++)
								{
									for (size_t k2 = 4; (k2 < 8) && bWanabeeStarOk; k2++)
									{
										if ((k1 != k2) && std::abs(directions[k2].m_Radius - directions[k1].m_Radius) > deltaRadius)
											bWanabeeStarOk = false;
									}
								}

								const double fMeanRadius1 = // top, bottom, left, right
									std::accumulate(directions.cbegin(), directions.cbegin() + 4, 0.0, [](const double acc, const PixelDirection& d) { return acc + d.m_Radius; })
									/ 4.0;
								const double fMeanRadius2 = // the four 45-degrees-direction
									std::accumulate(directions.cbegin() + 4, directions.cend(), 0.0, [](const double acc, const PixelDirection& d) { return acc + d.m_Radius; })
									* 0.3535533905932737622; // sqrt(2.0) / 4.0;

								int	lLeftRadius = 0;
								int	lRightRadius = 0;
								int	lTopRadius = 0;
								int	lBottomRadius = 0;

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
								//    for (int x = rcStar.left; x < rcStar.right; x++) 
								//       for (int y = rcStar.top; y < rcStar.bottom; y++)


								if (bWanabeeStarOk)
								{
									// The new star to add:
									CStar ms(ptTest.x(), ptTest.y());
									ms.m_fIntensity = fIntensity / 256.0;
									ms.m_rcStar = DSSRect{ ptTest.x() - lLeftRadius, ptTest.y() - lTopRadius, ptTest.x() + lRightRadius, ptTest.y() + lBottomRadius };
									ms.m_fPercentage = 1.0;
									ms.m_fDeltaRadius = deltaRadius;
									ms.m_fMeanRadius= (fMeanRadius1 + fMeanRadius2) / 2.0;

									constexpr double radiusFactor = 2.35 / 1.5;

									// Compute the real position
									if (computeStarCenter(inputBitmap, ms.m_fX, ms.m_fY, ms.m_fMeanRadius, backgroundLevel * (1.0 / 256.0)))
									{
										// Check last overlap condition
										{
											for (STARSET::const_iterator it = stars.lower_bound(CStar(ms.m_fX - ms.m_fMeanRadius * radiusFactor - STARMAXSIZE, 0)); it != stars.cend() && bWanabeeStarOk; ++it)
											{
												if (Distance(ms.m_fX, ms.m_fY, it->m_fX, it->m_fY) < (ms.m_fMeanRadius + it->m_fMeanRadius) * radiusFactor)
													bWanabeeStarOk = false;
												else if (it->m_fX > ms.m_fX + ms.m_fMeanRadius * radiusFactor + STARMAXSIZE)
													break;
											}
										}

										// Check comet intersection
										if (m_bComet)
										{
											if (ms.IsInRadius(m_fXComet, m_fYComet))
												bWanabeeStarOk = false;
										}

										if (bWanabeeStarOk)
										{
											ms.m_fQuality = (10 - deltaRadius) + fIntensity / 256.0 - ms.m_fMeanRadius;
											FindStarShape(&inputBitmap, ms);
											stars.insert(std::move(ms));
											++nStars;
										}
									}
								}
							}
						}
					}
				} // for i left -> right
			} // for j top -> bottom
		} // for deltaradius 0 -> 4
	}

	return nStars;
}

/* ------------------------------------------------------------------- */

bool	CRegisteredFrame::SaveRegisteringInfo(const fs::path& szInfoFileName)
{
	bool bResult = false;
	QFile data(szInfoFileName);
	if (!data.open(QFile::WriteOnly | QFile::Truncate))
		return false;

	QTextStream fileOut(&data);	
	{
		fileOut << QString("OverallQuality = %1").arg(m_fOverallQuality, 0, 'f', 2) << Qt::endl;
		fileOut << "RedXShift = 0.0" << Qt::endl;
		fileOut << "RedYShift = 0.0" << Qt::endl;
		fileOut << "BlueXShift = 0.0" << Qt::endl;
		fileOut << "BlueYShift = 0.0" << Qt::endl;
		if (m_bComet)
			fileOut << QString("Comet = %1, %2").arg(m_fXComet, 0, 'f', 2).arg(m_fYComet, 0, 'f', 2) << Qt::endl;
		fileOut << QString("SkyBackground = %1").arg(m_SkyBackground.m_fLight, 0, 'f', 4) << Qt::endl;
		fileOut << QString{ "ThresholdPercent = %1" }.arg(100.0 * this->usedDetectionThreshold, 0, 'f', 3) << Qt::endl;
		fileOut << "NrStars = " << m_vStars.size() << Qt::endl;
		for (int i = 0; i<m_vStars.size();i++)
		{
			fileOut << "Star# = " << i << Qt::endl;
			fileOut << QString("Intensity = %1").arg(m_vStars[i].m_fIntensity, 0, 'f', 2) << Qt::endl;
			fileOut << QString("Quality = %1").arg(m_vStars[i].m_fQuality, 0, 'f', 2) << Qt::endl;
			fileOut << QString("MeanRadius = %1").arg(m_vStars[i].m_fMeanRadius, 0, 'f', 2) << Qt::endl;
			fileOut << "Rect = " <<	m_vStars[i].m_rcStar.left << ", " <<
									m_vStars[i].m_rcStar.top << ", " <<
									m_vStars[i].m_rcStar.right << ", " <<
									m_vStars[i].m_rcStar.bottom << Qt::endl;
			fileOut << QString("Center = %1, %2").arg(m_vStars[i].m_fX, 0, 'f', 2).arg(m_vStars[i].m_fY, 0, 'f', 2) << Qt::endl;
			fileOut << QString("Axises = %1, %2, %3, %4, %5")
									.arg(m_vStars[i].m_fMajorAxisAngle, 0, 'f', 2)
									.arg(m_vStars[i].m_fLargeMajorAxis, 0, 'f', 2)
									.arg(m_vStars[i].m_fSmallMajorAxis, 0, 'f', 2)
									.arg(m_vStars[i].m_fLargeMinorAxis, 0, 'f', 2)
									.arg(m_vStars[i].m_fSmallMinorAxis, 0, 'f', 2) << Qt::endl;
		}
		bResult = true;
	}
	return bResult;
}

/* ------------------------------------------------------------------- */

namespace {
bool GetNextValue(QTextStream* fileIn, QString& strVariable, QString& strValue)
{
	bool bResult = false;
	strVariable.clear();
	strValue.clear();
	
	if (!fileIn->atEnd())
	{
		const QString strText = fileIn->readLine();
		int nPos = strText.indexOf("="); // Search = sign
		if (nPos >= 0)
		{
			strVariable = strText.left(nPos - 1).trimmed();
			strValue = strText.right(strText.length() - nPos - 1).trimmed();
		}
		else
		{
			strVariable = strText.trimmed();
		}
		bResult = true;
	}
	return bResult;
}
}

/* ------------------------------------------------------------------- */

bool	CRegisteredFrame::LoadRegisteringInfo(const fs::path& szInfoFileName)
{
	// TODO: Convert to use std::filepath/QFile and QStrings
	ZFUNCTRACE_RUNTIME();

	const auto unsuccessfulReturn = [this]() -> bool
	{
		this->m_bInfoOk = false;
		return false;
	};

	QFile data(szInfoFileName);
	if (!data.open(QFile::ReadOnly))
		return unsuccessfulReturn();
	QTextStream fileIn(&data);

	QString strVariable;
	QString strValue;
	int lNrStars = 0;
	bool bEnd = false;

	m_bComet = false;

	// Read overall quality
	while (!bEnd)
	{
		if (GetNextValue(&fileIn, strVariable, strValue) == false) // It did not even find "NrStars".
			return unsuccessfulReturn();

		if (0 == strVariable.compare("OverallQuality", Qt::CaseInsensitive))
			m_fOverallQuality = strValue.toDouble();

		if (0 == strVariable.compare("Comet", Qt::CaseInsensitive))
		{
			// Parse value (X, Y)
			const QStringList items(strValue.split(","));
			if (items.count() == 2)
			{
				m_fXComet = items[0].toDouble();
				m_fYComet = items[1].toDouble();
				m_bComet = true;
			}
		}
		else if (0 == strVariable.compare("SkyBackground", Qt::CaseInsensitive))
			m_SkyBackground.m_fLight = strValue.toDouble();
		else if (0 == strVariable.compare("NrStars", Qt::CaseInsensitive))
		{
			lNrStars = strValue.toInt();
			bEnd = true;
		}
	}

	// Jump to the first [Star#]
	GetNextValue(&fileIn, strVariable, strValue);
	bEnd = false;
	for (int i = 0; i < lNrStars && !bEnd; i++)
	{
		bool bNextStar = false;
		CStar ms;
		ms.m_fPercentage  = 0;
		ms.m_fDeltaRadius = 0;

		while (!bNextStar)
		{
			GetNextValue(&fileIn, strVariable, strValue);
			if (!strVariable.compare("Intensity", Qt::CaseInsensitive))
				ms.m_fIntensity = strValue.toDouble();
			else if (!strVariable.compare("Quality", Qt::CaseInsensitive))
				ms.m_fQuality = strValue.toDouble();
			else if (!strVariable.compare("MeanRadius", Qt::CaseInsensitive))
				ms.m_fMeanRadius = strValue.toDouble();
			else if (!strVariable.compare("Rect", Qt::CaseInsensitive))
			{
				const QStringList items(strValue.split(","));
				if(items.count() != 4)
					return unsuccessfulReturn();
				ms.m_rcStar.setCoords(items[0].toInt(), items[1].toInt(), items[2].toInt(), items[3].toInt());
			}
			else if (!strVariable.compare("Axises", Qt::CaseInsensitive))
			{
				const QStringList items(strValue.split(","));
				if (items.count() != 5)
					return unsuccessfulReturn();

				ms.m_fMajorAxisAngle = items[0].toDouble();
				ms.m_fLargeMajorAxis = items[1].toDouble();
				ms.m_fSmallMajorAxis = items[2].toDouble();
				ms.m_fLargeMinorAxis = items[3].toDouble();
				ms.m_fSmallMinorAxis = items[4].toDouble();
			}
			else if (!strVariable.compare("Center", Qt::CaseInsensitive))
			{
				const QStringList items(strValue.split(","));
				if (items.count() != 2)
					return unsuccessfulReturn();

				ms.m_fX = items[0].toDouble();
				ms.m_fY = items[1].toDouble();
			}
			else
			{
				bEnd = strValue.isEmpty();
				bNextStar = !strVariable.compare("Star#", Qt::CaseInsensitive) || bEnd;
			}
		}

		if (ms.IsValid())
			m_vStars.push_back(std::move(ms));
	}

	ComputeFWHM();
	m_bInfoOk = true;
	return true;
}

/* ------------------------------------------------------------------- */

void CLightFrameInfo::Reset()
{
	CFrameInfo::Reset();
	CRegisteredFrame::Reset();

	m_fXOffset = 0;
	m_fYOffset = 0;
	m_fAngle = 0;
	m_bDisabled = false;
	m_pProgress = nullptr;
	m_bStartingFrame = false;
	m_vVotedPairs.clear();

	m_bTransformedCometPosition = false;

	m_bRemoveHotPixels = Workspace{}.value("Register/DetectHotPixels", false).toBool();
}

double	CLightFrameInfo::ComputeMedianValue(const CGrayBitmap& Bitmap)
{
	double					fResult = 0.0;
	CBackgroundCalibration	BackgroundCalibration;

	BackgroundCalibration.m_BackgroundCalibrationMode = BCM_PERCHANNEL;
	BackgroundCalibration.m_BackgroundInterpolation   = BCI_LINEAR;
	BackgroundCalibration.SetMultiplier(256.0);
	BackgroundCalibration.ComputeBackgroundCalibration(&Bitmap, true, m_pProgress);
	fResult = BackgroundCalibration.m_fTgtRedBk/256.0;

	return fResult;
};


double CLightFrameInfo::RegisterPicture(const CGrayBitmap& Bitmap, double threshold, const bool optimizeThreshold)
{
	ZFUNCTRACE_RUNTIME();
	// Try to find star by studying the variation of luminosity
	constexpr int SubRectWidth = STARMAXSIZE * 5;
	constexpr int SubRectHeight = STARMAXSIZE * 5;
	// int lProgress = 0;

	// First computed median value
	m_fBackground = ComputeMedianValue(Bitmap);

	m_SkyBackground.m_fLight = m_fBackground;

	if (m_pProgress != nullptr)
	{
		const int lNrSubRects = ((Bitmap.Width() - STARMAXSIZE * 2) / SubRectWidth * 2) * ((Bitmap.Height() - STARMAXSIZE * 2) / SubRectHeight * 2);
		const QString strText(QCoreApplication::translate("RegisterEngine", "Registering %1", "IDS_REGISTERINGNAME").
			arg(QString::fromStdU16String(filePath.generic_u16string())));
		m_pProgress->Start2(strText, lNrSubRects);
	}

	m_vStars.clear();

	constexpr int StarMaxSize = static_cast<int>(STARMAXSIZE);
	constexpr int rectSize = 5 * StarMaxSize;
	constexpr int stepSize = rectSize / 2;
	constexpr int Separation = 3;
	const int calcHeight = Bitmap.Height() - 2 * StarMaxSize;
	const int nrSubrectsY = (calcHeight - 1) / stepSize + 1;
	const int calcWidth = Bitmap.Width() - 2 * StarMaxSize;
	const int nrSubrectsX = (calcWidth - 1) / stepSize + 1;
	const size_t nPixels = static_cast<size_t>(Bitmap.Width()) * static_cast<size_t>(Bitmap.Height());
	const int nrEnabledThreads = CMultitask::GetNrProcessors(); // Returns 1 if multithreading disabled by user, otherwise # HW threads.

	STARSET stars1, stars2, stars3, stars4;
	std::atomic<int> nrSubrects{ 0 };
	std::atomic<size_t> nStars{ 0 };

	const auto stop = [&stars1, optimizeThreshold](const double thres) -> bool
	{
		return !optimizeThreshold || stars1.size() >= 50 || thres <= 0.00075; // IF optimizeThreshold == false THEN return always true (=stop after the first iteration).
	};
	auto newThreshold = [&stars1, n1 = size_t{ 0 }, n2 = size_t{ 0 }, optimizeThreshold](const double lastThreshold) mutable -> double
	{
		if (!optimizeThreshold) // IF optimizeThreshold == false THEN return last threshold.
			return lastThreshold;
		n2 = n1;
		n1 = stars1.size();
		const double gradient = (n1 != 0 && n2 != 0)
			?  (50.0 * n2) / (static_cast<double>(n1) * static_cast<double>(n1)) // 50 = wanted number of stars
			: 1.0;
		return lastThreshold * (lastThreshold >= 0.02 ? 0.5 : 0.8) / (gradient >= 1.0 ? gradient : (0.5 * gradient + 0.5));
	};

	double usedThreshold = threshold;
	do
	{
		int masterCount{ 0 };
		const auto progress = [this, &nrSubrects, &nStars, &masterCount]() -> void
		{
			if (m_pProgress == nullptr)
				return;
			++nrSubrects;
			if (omp_get_thread_num() == 0 && (++masterCount % 25) == 0) // Only master thread
			{
				const QString strText(QCoreApplication::translate("RegisterEngine", "Registering %1 (%2 stars)", "IDS_REGISTERINGNAMEPLUSTARS")
					.arg(filePath.filename().generic_u8string().c_str())
					.arg(nStars.load()));
				m_pProgress->Progress2(strText, nrSubrects.load());
			}
		};

		std::array<std::exception_ptr, 5> ePointers{ nullptr, nullptr, nullptr, nullptr, nullptr };

		const auto processDisjointArea = [this, threshold, StarMaxSize, &Bitmap, stepSize, rectSize, &progress, &nStars](
					const int yStart, const int yEnd, const int xStart, const int xEnd, STARSET& stars, std::exception_ptr& ePointer, std::vector<double>& buffer
			) -> void
		{
			try
			{
				const int rightmostColumn = static_cast<int>(Bitmap.Width()) - StarMaxSize;

				for (int rowNdx = yStart; rowNdx < yEnd; ++rowNdx)
				{
					const int top = StarMaxSize + rowNdx * stepSize;
					const int bottom = std::min(static_cast<int>(Bitmap.Height()) - StarMaxSize, top + rectSize);

					for (int colNdx = xStart; colNdx < xEnd; ++colNdx, progress())
					{
						nStars += RegisterSubRect(Bitmap,
							threshold,
							DSSRect(StarMaxSize + colNdx * stepSize, top, std::min(rightmostColumn, StarMaxSize + colNdx * stepSize + rectSize), bottom),
							stars,
							buffer
						);
					}
				}
			}
			catch (...)
			{
				ePointer = std::current_exception();
			}
		};

#pragma omp parallel default(none) shared(stars1, stars2, stars3, stars4, ePointers, nPixels, threshold) num_threads(std::min(nrEnabledThreads, 4)) if(nrEnabledThreads > 1)
{
			std::vector<double> dataBuffer(nPixels);
//#pragma omp master // There is no implied barrier.
//		ZTRACE_RUNTIME("Registering with %d OpenMP threads. Threshold = %f.", omp_get_num_threads(), threshold);
#pragma omp sections
		{
			// Upper left area
#pragma omp section
			processDisjointArea(0, (nrSubrectsY - Separation) / 2, 0, (nrSubrectsX - Separation) / 2, stars1, ePointers[0], dataBuffer);
			// Upper right area
#pragma omp section
			processDisjointArea(0, (nrSubrectsY - Separation) / 2, (nrSubrectsX - Separation) / 2 + Separation, nrSubrectsX, stars2, ePointers[1], dataBuffer);
			// Lower left area
#pragma omp section
			processDisjointArea((nrSubrectsY - Separation) / 2 + Separation, nrSubrectsY, 0, (nrSubrectsX - Separation) / 2, stars3, ePointers[2], dataBuffer);
			// Lower right area
#pragma omp section
			processDisjointArea((nrSubrectsY - Separation) / 2 + Separation, nrSubrectsY, (nrSubrectsX - Separation) / 2 + Separation, nrSubrectsX, stars4, ePointers[3], dataBuffer);
		}

#pragma omp sections
		{
#pragma omp section
			stars1.merge(stars2);
#pragma omp section
			stars3.merge(stars4);
		}

#pragma omp single
		{
			stars1.merge(stars3);
			// Remaining areas, all are overlapping with at least one other.
			// Vertically middle band, full height
			processDisjointArea(0, nrSubrectsY, (nrSubrectsX - Separation) / 2, (nrSubrectsX - Separation) / 2 + Separation, stars1, ePointers[4], dataBuffer);
			// Middle left
			processDisjointArea((nrSubrectsY - Separation) / 2, (nrSubrectsY - Separation) / 2 + Separation, 0, (nrSubrectsX - Separation) / 2, stars1, ePointers[4], dataBuffer);
			// Middle right
			processDisjointArea((nrSubrectsY - Separation) / 2, (nrSubrectsY - Separation) / 2 + Separation, (nrSubrectsX - Separation) / 2 + Separation,
				nrSubrectsX, stars1, ePointers[4], dataBuffer);

//				m_vStars.assign(stars1.cbegin(), stars1.cend());
		}

//#pragma omp sections
//	{
//#pragma omp section
//		ComputeOverallQuality();
//#pragma omp section
//		ComputeFWHM();
//	}

#pragma omp master // There is no implied barrier.
		ZTRACE_RUNTIME("Registering with %d OpenMP threads. Threshold = %f %%; #-Stars = %zu.", omp_get_num_threads(), threshold * 100, stars1.size());
} // omp parallel

		//
		// If there was at least one exception in the parallel OpenMP code -> re-throw it.
		//
		for (std::exception_ptr e : ePointers)
		{
			if (e != nullptr)
				std::rethrow_exception(e);
		}

		if (m_pProgress)
			m_pProgress->End2();

		usedThreshold = threshold;
		threshold = newThreshold(threshold);
	} while (!stop(threshold)); // loop over thresholds

	m_vStars.assign(stars1.cbegin(), stars1.cend());
	ComputeOverallQuality();
	ComputeFWHM();
	return usedThreshold;
}


class CComputeLuminanceTask
{
public:
	CGrayBitmap* m_pGrayBitmap;
	const CMemoryBitmap* m_pBitmap;
	ProgressBase* m_pProgress;

public:
	CComputeLuminanceTask(const CMemoryBitmap* pBm, CGrayBitmap* pGb, ProgressBase* pPrg) :
		m_pGrayBitmap{ pGb },
		m_pBitmap{ pBm },
		m_pProgress{ pPrg }
	{}

	~CComputeLuminanceTask() = default;
	void process();
private:
	void processNonAvx(const int lineStart, const int lineEnd);
};

void CComputeLuminanceTask::process()
{
	ZFUNCTRACE_RUNTIME();
	const int nrProcessors = CMultitask::GetNrProcessors();
	const int height = m_pBitmap->Height();
	int progress = 0;
	constexpr int lineBlockSize = 20;

	AvxLuminance avxLuminance{ *m_pBitmap, *m_pGrayBitmap };

#pragma omp parallel for schedule(static, 5) default(none) firstprivate(avxLuminance) if(nrProcessors > 1)
	for (int row = 0; row < height; row += lineBlockSize)
	{
		if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
			m_pProgress->Progress2(progress += nrProcessors * lineBlockSize);

		const int endRow = std::min(row + lineBlockSize, height);
		if (avxLuminance.computeLuminanceBitmap(row, endRow) != 0)
		{
			processNonAvx(row, endRow);
		}
	}
}

void CComputeLuminanceTask::processNonAvx(const int lineStart, const int lineEnd)
{
	const int width = m_pBitmap->Width();
	for (int row = lineStart; row < lineEnd; ++row)
	{
		for (int col = 0; col < width; ++col)
		{
			COLORREF16 crColor;
			m_pBitmap->GetPixel16(col, row, crColor);
			m_pGrayBitmap->SetPixel(col, row, GetLuminance(crColor));
		}
	}
}


std::shared_ptr<const CGrayBitmap> CLightFrameInfo::ComputeLuminanceBitmap(CMemoryBitmap* pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	m_lWidth = pBitmap->Width();
	m_lHeight = pBitmap->Height();

	if (m_bRemoveHotPixels)
		pBitmap->RemoveHotPixels(m_pProgress);

	// Try to find star by studying the variation of luminosity
	if (m_pProgress != nullptr)
	{
		const QString strText(QCoreApplication::translate("RegisterEngine", "Computing luminances %1", "IDS_COMPUTINGLUMINANCE")
			.arg(QString::fromStdU16String(filePath.generic_u16string())));
		m_pProgress->Start2(strText, m_lHeight);
	}

	std::shared_ptr<CGrayBitmap> pGrayBitmap = std::make_shared<CGrayBitmap>();
	ZTRACE_RUNTIME("Creating Gray memory bitmap %p (luminance)", pGrayBitmap.get());
	pGrayBitmap->Init(pBitmap->Width(), pBitmap->Height());

	CComputeLuminanceTask{ pBitmap, pGrayBitmap.get(), m_pProgress }.process();

	if (m_pProgress != nullptr)
		m_pProgress->End2();

	if (m_bApplyMedianFilter)
	{
		std::shared_ptr<const CGrayBitmap> pFiltered = std::dynamic_pointer_cast<const CGrayBitmap>(CMedianImageFilter{}.ApplyFilter(pGrayBitmap.get(), m_pProgress));
		if (static_cast<bool>(pFiltered))
			return pFiltered;
		else
			throw std::runtime_error("ComputeLuminanceBitmap: Median Image Filter did not return a GrayBitmap.");
	}
	else
		return pGrayBitmap;
}

//
// Note: Consistent results for auto-threshold registration are only guarateed if this function is called in the correct order of the lightframes. 
// This is, because the threshold used in the previous run influences the current run. 
//
void CLightFrameInfo::RegisterPicture(CMemoryBitmap* pBitmap, const int bitmapIndex)
{
	ZFUNCTRACE_RUNTIME();

	constexpr double ThresholdStartingValue = 0.65;

	const bool thresholdOptimization = this->m_fMinLuminancy == 0;
	static double previousThreshold = ThresholdStartingValue;
	const double threshold = thresholdOptimization ? (bitmapIndex <= 0 ? ThresholdStartingValue : previousThreshold) : this->m_fMinLuminancy;

	const std::shared_ptr<const CGrayBitmap> pGrayBitmap = ComputeLuminanceBitmap(pBitmap);
	if (static_cast<bool>(pGrayBitmap))
	{
		const double usedThres = RegisterPicture(*pGrayBitmap, threshold, thresholdOptimization);
		this->usedDetectionThreshold = usedThres;
		// IF auto-threshold: Take the threshold of the first lightframe as starting value for the following lightframes.
		previousThreshold = bitmapIndex == 0 ? usedThres : previousThreshold;
		ZTRACE_RUNTIME("Finished registering file # %d. Final threshold = %f; Found %zu stars; Quality=%f; MeanQuality=%f",
			bitmapIndex, usedThres, m_vStars.size(), this->m_fOverallQuality, this->meanQuality);
	}
}

bool CLightFrameInfo::ComputeStarShifts(CMemoryBitmap* pBitmap, CStar& star, double& fRedXShift, double& fRedYShift, double& fBlueXShift, double& fBlueYShift)
{
	// Compute star center for blue and red
	double fSumRedX = 0;
	double fSumRedY = 0;
	double fNrValuesRedX = 0;
	double fNrValuesRedY = 0;
	double fAverageRedX = 0;
	double fAverageRedY = 0;
	double fSumBlueX = 0;
	double fSumBlueY = 0;
	double fNrValuesBlueX = 0;
	double fNrValuesBlueY = 0;
	double fAverageBlueX = 0;
	double fAverageBlueY = 0;

	int lNrBlueLines = 0;
	int lNrRedLines = 0;

	for (int j = star.m_rcStar.top; j <= star.m_rcStar.bottom; j++)
	{
		fSumRedX = 0;
		fNrValuesRedX = 0;
		fSumBlueX = 0;
		fNrValuesBlueX = 0;
		for (int i = star.m_rcStar.left; i <= star.m_rcStar.right; i++)
		{
			double fRed, fGreen, fBlue;

			pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
			fSumRedX += fRed * i;
			fNrValuesRedX += fRed;
			fSumBlueX += fBlue * i;
			fNrValuesBlueX += fBlue;
		}
		if (fNrValuesRedX)
		{
			fAverageRedX += fSumRedX / fNrValuesRedX;
			lNrRedLines++;
		}
		if (fNrValuesBlueX)
		{
			fAverageBlueX += fSumBlueX / fNrValuesBlueX;
			lNrBlueLines++;
		}
	}
	if (lNrRedLines)
		fAverageRedX /= static_cast<double>(lNrRedLines);
	if (lNrBlueLines)
		fAverageBlueX /= static_cast<double>(lNrBlueLines);

	int lNrRedColumns = 0;
	int lNrBlueColumns = 0;
	for (int j = star.m_rcStar.left; j <= star.m_rcStar.right; j++)
	{
		fSumRedY = 0;
		fNrValuesRedY = 0;
		fSumBlueY = 0;
		fNrValuesBlueY = 0;
		for (int i = star.m_rcStar.top; i <= star.m_rcStar.bottom; i++)
		{
			double fRed, fGreen, fBlue;

			pBitmap->GetPixel(j, i, fRed, fGreen, fBlue);
			fSumRedY += fRed * i;
			fNrValuesRedY += fRed;
			fSumBlueY += fBlue * i;
			fNrValuesBlueY += fBlue;
		}
		if (fNrValuesRedY)
		{
			fAverageRedY += fSumRedY / fNrValuesRedY;
			lNrRedColumns++;
		}
		if (fNrValuesBlueY)
		{
			fAverageBlueY += fSumBlueY / fNrValuesBlueY;
			lNrBlueColumns++;
		}
	}
	if (lNrRedColumns)
		fAverageRedY /= static_cast<double>(lNrRedColumns);
	if (lNrBlueColumns)
		fAverageBlueY /= static_cast<double>(lNrBlueColumns);

	fRedXShift = fAverageRedX - star.m_fX;
	fRedYShift = fAverageRedY - star.m_fY;
	fBlueXShift = fAverageBlueX - star.m_fX;
	fBlueYShift = fAverageBlueY - star.m_fY;

	return (lNrRedColumns != 0) && (lNrRedLines != 0) && (lNrBlueLines != 0) && (lNrBlueColumns != 0);
}

/* ------------------------------------------------------------------- */
/*
void CLightFrameInfo::ComputeRedBlueShifting(CMemoryBitmap * pBitmap)
{
	int				i = 0;
	int				lNrShifts = 0;

	m_fRedXShift	  = 0;
	m_fRedYShift	  = 0;
	m_fBlueXShift	  = 0;
	m_fBlueYShift	  = 0;

	// For each detected star compute blue and red shift
	for (i = 0;i<m_vStars.size();i++)
	{
		double			fRedXShift,
						fRedYShift,
						fBlueXShift,
						fBlueYShift;

		if (ComputeStarShifts(pBitmap, m_vStars[i], fRedXShift, fRedYShift, fBlueXShift, fBlueYShift))
		{
			m_fRedXShift += fRedXShift;
			m_fRedYShift += fRedYShift;
			m_fBlueXShift += fBlueXShift;
			m_fBlueYShift += fBlueYShift;
			lNrShifts++;
		};
	};

	if (lNrShifts)
	{
		m_fRedXShift /= (double)lNrShifts;
		m_fRedYShift /= (double)lNrShifts;
		m_fBlueXShift /= (double)lNrShifts;
		m_fBlueYShift /= (double)lNrShifts;
	};
};
*/

void CLightFrameInfo::RegisterPicture(const fs::path& bitmap, double fMinLuminancy, bool bRemoveHotPixels, bool bApplyMedianFilter, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	Reset();
	filePath = bitmap;
	m_fMinLuminancy		= fMinLuminancy;
	m_fBackground		= 0.0;
	m_bRemoveHotPixels  = bRemoveHotPixels;
	m_bApplyMedianFilter= bApplyMedianFilter ? true : false;
	m_pProgress			= pProgress;

	CBitmapInfo			bmpInfo;
	bool				bLoaded;

	if (GetPictureInfo(filePath, bmpInfo) && bmpInfo.CanLoad())
	{
		QString strText;
		QString	strDescription;

		bmpInfo.GetDescription(strDescription);

		if (bmpInfo.m_lNrChannels == 3)
			strText = QCoreApplication::translate("RegisterEngine", "Loading %1 bit/ch %2 picture\n%3", "IDS_LOADRGBPICTURE").arg(bmpInfo.m_lBitsPerChannel)
			.arg(strDescription)
			.arg(QString::fromStdU16String(filePath.generic_u16string()));
		else
			strText = QCoreApplication::translate("RegisterEngine", "Loading %1 bits gray %2 picture\n%3", "IDS_LOADGRAYPICTURE").arg(bmpInfo.m_lBitsPerChannel)
			.arg(strDescription)
			.arg(QString::fromStdU16String(filePath.generic_u16string()));

		if (m_pProgress != nullptr)
			m_pProgress->Start2(strText, 0);

		std::shared_ptr<CMemoryBitmap> pBitmap;
		std::shared_ptr<QImage> pQImage;
		bLoaded = ::FetchPicture(filePath, pBitmap, this->m_PictureType == PICTURETYPE_FLATFRAME, m_pProgress, pQImage);

		if (m_pProgress != nullptr)
			m_pProgress->End2();

		if (bLoaded)
		{
			RegisterPicture(pBitmap.get(), -1); // -1 means, we do NOT register a series of frames.
//			ComputeRedBlueShifting(pBitmap);
		}
	}

	m_pProgress = nullptr;
}

/* ------------------------------------------------------------------- */

bool CLightFrameInfo::ReadInfoFileName()
{
	return LoadRegisteringInfo(m_strInfoFileName);
};

/* ------------------------------------------------------------------- */

void CLightFrameInfo::SaveRegisteringInfo()
{
	m_bInfoOk = CRegisteredFrame::SaveRegisteringInfo(m_strInfoFileName);
};

/* ------------------------------------------------------------------- */

void CLightFrameInfo::SetBitmap(fs::path path/*, bool bProcessIfNecessary, bool bForceRegister*/)
{
	TCHAR				szDrive[1+_MAX_DRIVE];
	TCHAR				szDir[1+_MAX_DIR];
	TCHAR				szFile[1+_MAX_FNAME];
	TCHAR				szExt[1+_MAX_EXT];
	TCHAR				szInfoName[1+_MAX_PATH];

	Reset();
	m_bInfoOk = false;
	filePath = path;
	_tsplitpath(filePath.c_str(), szDrive, szDir, szFile, szExt);
	_tmakepath(szInfoName, szDrive, szDir, szFile, _T(".info.txt"));

	m_strInfoFileName = szInfoName;

	ReadInfoFileName();
	//if (bForceRegister || (!ReadInfoFileName() && bProcessIfNecessary))
	//{
	//	RegisterPicture();
	//	SaveRegisteringInfo();
	//}
}

/* ------------------------------------------------------------------- */

CRegisterEngine::CRegisterEngine()
{
	m_bSaveCalibrated = CAllStackingTasks::GetSaveCalibrated();
	m_IntermediateFileFormat = CAllStackingTasks::GetIntermediateFileFormat();
	m_bSaveCalibratedDebayered = CAllStackingTasks::GetSaveCalibratedDebayered();
}

bool CRegisterEngine::SaveCalibratedLightFrame(const CLightFrameInfo& lfi, std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase* pProgress, QString& strCalibratedFile)
{
	bool bResult = false;

	if (!lfi.filePath.empty() && static_cast<bool>(pBitmap))
	{
		const QFileInfo fileInfo(lfi.filePath);
		const QString strPath(fileInfo.path() + QDir::separator());
		const QString strBaseName(fileInfo.baseName());

		if ((m_IntermediateFileFormat == IFF_TIFF))
		{
			strCalibratedFile = strPath + strBaseName + ".cal.tif";
		}
		else
		{
			QString strFitsExt;
			GetFITSExtension(fileInfo.absoluteFilePath(), strFitsExt);
			strCalibratedFile = strPath + strBaseName + ".cal" + strFitsExt;
		}
		strCalibratedFile = QDir::toNativeSeparators(strCalibratedFile);

		std::shared_ptr<CMemoryBitmap> pOutBitmap;

		if (m_bSaveCalibratedDebayered)
		{
			// Debayer the image
			if (!DebayerPicture(pBitmap.get(), pOutBitmap, pProgress))
				pOutBitmap = pBitmap;
		}
		else
			pOutBitmap = pBitmap;

		// Check and remove super pixel settings
		CFATRANSFORMATION CFATransform = CFAT_NONE;
		CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(pOutBitmap.get());
		if (pCFABitmapInfo != nullptr)
		{
			CFATransform = pCFABitmapInfo->GetCFATransformation();
			if (CFATransform == CFAT_SUPERPIXEL)
				pCFABitmapInfo->UseBilinear(true);
		}

		if (pProgress)
		{
			const QString strText(QCoreApplication::translate("RegisterEngine", "Saving Calibrated image in %1", "IDS_SAVINGCALIBRATED").arg(strCalibratedFile));
			pProgress->Start2(strText, 0);
		}

		const QString description("Calibrated light frame");
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strCalibratedFile.toStdU16String().c_str(), pOutBitmap.get(), pProgress, description, lfi.m_lISOSpeed, lfi.m_lGain, lfi.m_fExposure, lfi.m_fAperture);
		else
			bResult = WriteFITS(strCalibratedFile.toStdU16String().c_str(), pOutBitmap.get(), pProgress, description, lfi.m_lISOSpeed, lfi.m_lGain, lfi.m_fExposure);

		if (CFATransform == CFAT_SUPERPIXEL)
			pCFABitmapInfo->UseSuperPixels(true);

		if (pProgress)
			pProgress->End2();
	}

	return bResult;
}


bool CRegisterEngine::RegisterLightFrames(CAllStackingTasks& tasks, bool bForce, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = true;
	int nrRegisteredPictures = 0;

	for (auto it = std::cbegin(tasks.m_vStacks); it != std::cend(tasks.m_vStacks); ++it)
		nrRegisteredPictures += it->m_pLightTask == nullptr ? 0 : static_cast<int>(it->m_pLightTask->m_vBitmaps.size());

	const QString strText = QCoreApplication::translate("RegisterEngine", "Registering pictures", "IDS_REGISTERING");

	if (pProgress != nullptr)
	{
		pProgress->Start1(strText, nrRegisteredPictures, true);
	}

	bResult = tasks.DoAllPreTasks(pProgress);

	// Do it again in case pretasks change the progress
	if (pProgress != nullptr)
		pProgress->Start1(strText, nrRegisteredPictures, true);

	//
	// Number of image being registered.  Starts at 1 - goes up to nrRegisteredPictures
	//
	int imageNumber = 1;
	int successfulRegisteredPictures = 0;

	for (auto it = std::cbegin(tasks.m_vStacks); it != std::cend(tasks.m_vStacks) && bResult; ++it)
	{
		if (it->m_pLightTask == nullptr)
			continue;

		CMasterFrames MasterFrames;
		MasterFrames.LoadMasters(std::addressof(*it), pProgress);

		const auto readTask = [&bitmaps = it->m_pLightTask->m_vBitmaps, bForce](const size_t bitmapNdx, ProgressBase* pTaskProgress)
			-> std::tuple<std::shared_ptr<CMemoryBitmap>, bool, std::unique_ptr<CLightFrameInfo>, std::unique_ptr<CBitmapInfo>>
		{
			if (bitmapNdx >= bitmaps.size())
				return std::make_tuple(std::shared_ptr<CMemoryBitmap>{}, false, std::unique_ptr<CLightFrameInfo>{}, std::unique_ptr<CBitmapInfo>{});

			const auto& bitmap{ bitmaps[bitmapNdx] };
			auto lfInfo = std::make_unique<CLightFrameInfo>();
			lfInfo->SetBitmap(bitmap.filePath);
			if (!bForce && lfInfo->IsRegistered())
				return std::make_tuple(std::shared_ptr<CMemoryBitmap>{}, false, std::unique_ptr<CLightFrameInfo>{}, std::unique_ptr<CBitmapInfo>{});

			auto bmpInfo = std::make_unique<CBitmapInfo>();
			if (!GetPictureInfo(lfInfo->filePath, *bmpInfo) || !bmpInfo->CanLoad())
				return std::make_tuple(std::shared_ptr<CMemoryBitmap>{}, false, std::unique_ptr<CLightFrameInfo>{}, std::unique_ptr<CBitmapInfo>{});

			std::shared_ptr<CMemoryBitmap> pBitmap;
			std::shared_ptr<QImage> pQImage;
			bool success = ::FetchPicture(lfInfo->filePath, pBitmap, lfInfo->m_PictureType == PICTURETYPE_FLATFRAME, pTaskProgress, pQImage);
			return std::make_tuple(std::move(pBitmap), success, std::move(lfInfo), std::move(bmpInfo));
		};

		auto future = std::async(std::launch::deferred, readTask, 0, pProgress);

		int numberOfRegisteredLightframes = 0;

		for (size_t j = 0; j < it->m_pLightTask->m_vBitmaps.size() && bResult; j++, imageNumber++)
		{
			ZTRACE_RUNTIME("Register file # %d: %s", successfulRegisteredPictures, it->m_pLightTask->m_vBitmaps[j].filePath.generic_u8string().c_str());

			auto [pBitmap, success, lfInfo, bmpInfo] = future.get();
			future = std::async(std::launch::async, readTask, j + 1, nullptr);

			if (pProgress != nullptr)
			{
				const QString strText1 = QCoreApplication::translate("RegisterEngine", "Registering %1 of %2", "IDS_REGISTERINGPICTURE").arg(imageNumber).arg(nrRegisteredPictures);
				pProgress->Progress1(strText1, (imageNumber - 1));
			}

			if (!success)
				continue;

			QString strDescription;
			bmpInfo->GetDescription(strDescription);
			QString strText2;
			if (bmpInfo->m_lNrChannels == 3)
				strText2 = QCoreApplication::translate("RegisterEngine", "Loading %1 bit/ch %2 light frame\n%3", "IDS_LOADRGBLIGHT").arg(bmpInfo->m_lBitsPerChannel).arg(strDescription).arg(lfInfo->filePath.c_str());
			else
				strText2 = QCoreApplication::translate("RegisterEngine", "Loading %1 bits gray %2 light frame\n%3", "IDS_LOADGRAYLIGHT").arg(bmpInfo->m_lBitsPerChannel).arg(strDescription).arg(lfInfo->filePath.c_str());
			if (pProgress != nullptr)
				pProgress->Start2(strText2, 0);

			// Apply offset, dark and flat to lightframe
			MasterFrames.ApplyAllMasters(pBitmap, nullptr, pProgress);

			QString strCalibratedFile;

			if (m_bSaveCalibrated && (it->m_pDarkTask != nullptr || it->m_pDarkFlatTask != nullptr || it->m_pFlatTask != nullptr || it->m_pOffsetTask != nullptr))
				SaveCalibratedLightFrame(*lfInfo, pBitmap, pProgress, strCalibratedFile);

			// Then register the light frame
			lfInfo->SetProgress(pProgress);
			lfInfo->RegisterPicture(pBitmap.get(), successfulRegisteredPictures++);
			lfInfo->SaveRegisteringInfo();

			++numberOfRegisteredLightframes;

			if (strCalibratedFile.length())
			{
				fs::path file{ strCalibratedFile.toStdU16String().c_str() };
				lfInfo->CRegisteredFrame::SaveRegisteringInfo(file.replace_extension(".info.txt"));
			}

			if (pProgress != nullptr)
			{
				pProgress->End2();
				bResult = !pProgress->IsCanceled();
			}
		}

		//
		// If at least one lightframe has been registered, then remove ALL stackinfo.txt files 
		// from that folder. This avoids alignment issues, because the stackinfo-file might 
		// contain invalid (not matching new registration info) alignment (=offset) parameters.
		//
		ZTRACE_RUNTIME("Number of actually registered lightframes in this stack = %d", numberOfRegisteredLightframes);
		if (numberOfRegisteredLightframes > 0)
		{
			for (const CFrameInfo& bitmap : it->m_pLightTask->m_vBitmaps)
			{
				fs::path toRemove{ bitmap.filePath };
				toRemove.replace_extension("stackinfo.txt");
				if (fs::exists(toRemove))
				{
					ZTRACE_RUNTIME("Removing stackinfo-file %s", toRemove.generic_u8string().c_str());
					fs::remove(toRemove);
				}
			}
		}
	} // Loop over tasks.

	// Clear stuff
	tasks.ClearCache();

	return bResult;
}
