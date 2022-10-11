
#include <stdafx.h>
#include "RegisterEngine.h"

#include "MasterFrames.h"
#include "BackgroundCalibration.h"
#include "PixelTransform.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "Filters.h"
#include "avx_luminance.h"

#if QT_VERSION < 0x060000
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <omp.h>

/* ------------------------------------------------------------------- */

class CStarAxisInfo
{
public :
	int					m_lAngle;
	double					m_fRadius;
	double					m_fSum;

private :
	void	CopyFrom(const CStarAxisInfo & ai)
	{
		m_lAngle	= ai.m_lAngle;
		m_fRadius	= ai.m_fRadius;
		m_fSum		= ai.m_fSum;
	};

public :
	CStarAxisInfo()
	{
        m_lAngle = 0;
        m_fRadius = 0;
        m_fSum = 0;
	};

	CStarAxisInfo(const CStarAxisInfo & ai)
	{
		CopyFrom(ai);
	};

	virtual ~CStarAxisInfo()
	{
	};

	const CStarAxisInfo & operator = (const CStarAxisInfo & ai)
	{
		CopyFrom(ai);
		return *this;
	};
};

inline	void NormalizeAngle(int & lAngle)
{
	while (lAngle >= 360)
		lAngle -= 360;
	while (lAngle < 0)
		lAngle += 360;
};

/* ------------------------------------------------------------------- */

bool CRegisteredFrame::FindStarShape(CMemoryBitmap* pBitmap, CStar& star)
{
	bool						bResult = false;
	std::vector<CStarAxisInfo>	vStarAxises;
	double						fMaxHalfRadius = 0.0;
	double						fMaxCumulated  = 0.0;
	int						lMaxHalfRadiusAngle = 0.0;
	int						lAngle;

	// Preallocate the vector for the inner loop.
	PIXELDISPATCHVECTOR		vPixels;
	vPixels.reserve(10);

	for (lAngle = 0;lAngle<360;lAngle+=10)
	{
		CStarAxisInfo			ai;
		double					fSquareSum = 0.0;
		double					fSum	   = 0.0;
		double					fNrValues  = 0.0;
		double					fStdDev	   = 0.0;

		ai.m_lAngle = lAngle;
		for (double fPos = 0.0;fPos<=star.m_fMeanRadius*2.0;fPos+=0.10)
		{
			double		fX = star.m_fX + cos(lAngle*M_PI/180.0)*fPos,
						fY = star.m_fY + sin(lAngle*M_PI/180.0)*fPos;
			double		fLuminance = 0;

			// Compute luminance at fX, fY
			vPixels.resize(0);
			ComputePixelDispatch(CPointExt(fX, fY), vPixels);

			for (int k = 0;k<vPixels.size();k++)
			{
				double				fValue;

				pBitmap->GetPixel(vPixels[k].m_lX, vPixels[k].m_lY, fValue);
				fLuminance += fValue * vPixels[k].m_fPercentage;
			};
			fSquareSum	+= pow(fPos, 2) * fLuminance * 2;
			fSum		+= fLuminance;
			fNrValues	+= fLuminance * 2;
		};

		if (fNrValues)
			fStdDev = sqrt(fSquareSum/fNrValues);
		ai.m_fRadius = fStdDev * 1.5;
		ai.m_fSum    = fSum;

		if (ai.m_fSum > fMaxCumulated)
		{
			fMaxCumulated		= ai.m_fSum;
			fMaxHalfRadius		= ai.m_fRadius;
			lMaxHalfRadiusAngle = ai.m_lAngle;
		};

		vStarAxises.push_back(ai);
	};

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
		};
	};

	bFound		 = false;
	lSearchAngle = lMaxHalfRadiusAngle + 90;
	NormalizeAngle(lSearchAngle);

	for (int i = 0;i<vStarAxises.size() && !bFound;i++)
	{
		if (vStarAxises[i].m_lAngle == lSearchAngle)
		{
			bFound = true;
			star.m_fLargeMinorAxis = vStarAxises[i].m_fRadius;
		};
	};

	bFound		 = false;
	lSearchAngle = lMaxHalfRadiusAngle + 210;
	NormalizeAngle(lSearchAngle);

	for (int i = 0;i<vStarAxises.size() && !bFound;i++)
	{
		if (vStarAxises[i].m_lAngle == lSearchAngle)
		{
			bFound = true;
			star.m_fSmallMinorAxis = vStarAxises[i].m_fRadius;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CRegisteredFrame::ComputeStarCenter(CMemoryBitmap* pBitmap, double& fX, double& fY, double& fRadius)
{
	bool				bResult = false;
	int				i, j;
	double				fSumX = 0,
						fSumY = 0;
	double				fNrValuesX = 0,
						fNrValuesY = 0;
	double				fAverageX = 0,
						fAverageY = 0;


	int				lNrLines = 0;
	for (j = fY-fRadius;j<=fY+fRadius;j++)
	{
		fSumX = 0;
		fNrValuesX = 0;
		for (i = fX-fRadius;i<=fX+fRadius;i++)
		{
			double			fValue;

			pBitmap->GetPixel(i, j, fValue);
			fSumX += fValue * i;
			fNrValuesX += fValue;
		};
		if (fNrValuesX)
		{
			lNrLines++;
			fAverageX += fSumX/fNrValuesX;
		};
	};
	fAverageX /= (double)lNrLines;

	int				lNrColumns = 0;
	for (j = fX-fRadius;j<=fX+fRadius;j++)
	{
		fSumY = 0;
		fNrValuesY = 0;
		for (i = fY-fRadius;i<=fY+fRadius;i++)
		{
			double			fValue;
			pBitmap->GetPixel(j, i, fValue);
			fSumY += fValue * i;
			fNrValuesY += fValue;
		};
		if (fNrValuesY)
		{
			lNrColumns++;
			fAverageY += fSumY/fNrValuesY;
		};
	};
	fAverageY /= (double)lNrColumns;

	fX = fAverageX;
	fY = fAverageY;

	// Then compute the radius
	double				fSquareSumX = 0;
	double				fStdDevX = 0;
	fSumX = 0;
	fNrValuesX = 0;
	for (i = fX-fRadius;i<=fX+fRadius;i++)
	{
		double			fValue;
		pBitmap->GetPixel(i, fY, fValue);
		fValue = std::max(0.0, fValue - m_fBackground);
		fSumX		+= fValue * i;
		fSquareSumX += pow(i - fX, 2)* fValue;
		fNrValuesX	+= fValue;
	};
	fStdDevX = sqrt(fSquareSumX /fNrValuesX);

	double				fSquareSumY = 0;
	double				fStdDevY = 0;
	fSumY = 0;
	fNrValuesY = 0;
	for (i = fY-fRadius;i<=fY+fRadius;i++)
	{
		double			fValue;
		pBitmap->GetPixel(fX, i, fValue);
		fValue = std::max(0.0, fValue - m_fBackground);
		fSumY		+= fValue * i;
		fSquareSumY += pow(i - fY, 2)*fValue;
		fNrValuesY	+= fValue;
	};
	fStdDevY = sqrt(fSquareSumY /fNrValuesY);

	// The radius is the average of the standard deviations
	fRadius = (fStdDevX + fStdDevY)/2.0*1.5;

	return fabs(fStdDevX - fStdDevY) < m_fRoundnessTolerance;
};

/* ------------------------------------------------------------------- */

class CPixelDirection
{
public :
	int				m_Ok;
	double				m_fIntensity;
	int				m_lXDir;
	int				m_lYDir;
	double				m_fRadius;
	int				m_lNrBrighterPixels;

private :
	void	CopyFrom(const CPixelDirection & pd)
	{
		m_Ok		 = pd.m_Ok;
		m_fIntensity = pd.m_fIntensity;
		m_lXDir		 = pd.m_lXDir;
		m_lYDir		 = pd.m_lYDir;
		m_fRadius	 = pd.m_fRadius;
		m_lNrBrighterPixels = pd.m_lNrBrighterPixels;
	};

public :
	CPixelDirection(int lXDir = 0, int lYDir = 0)
	{
		m_Ok = 2;
		m_fIntensity = 0;
		m_lXDir      = lXDir;
		m_lYDir      = lYDir;
		m_fRadius	 = 0;
		m_lNrBrighterPixels = 0;
	};

	CPixelDirection(const int xd, const int yd, const int ok, const double r) :
		m_Ok{ ok },
		m_fIntensity{ 0.0 },
		m_lXDir{ xd },
		m_lYDir{ yd },
		m_fRadius{ r },
		m_lNrBrighterPixels{ 0 }
	{}

	CPixelDirection(const CPixelDirection & pd)
	{
		CopyFrom(pd);
	};

	const CPixelDirection & operator = (const CPixelDirection & pd)
	{
		CopyFrom(pd);
		return (*this);
	};

	virtual ~CPixelDirection() {};
};

/* ------------------------------------------------------------------- */

class CPixelDirections
{
public :
	bool			bBrighterPixel;
	bool			bMainOk;
	double			fMaxRadius;
	double			m_fBackground;
	double			fIntensity;

public :
	CPixelDirections()
	{
		bBrighterPixel = false;
		bMainOk		   = true;
		fMaxRadius	   = 0;
        m_fBackground  = 0;
        fIntensity     = 0;
	};
	bool	FillPixelDirection(double fX, double fY, CGrayBitmap & Bitmap, std::vector<CPixelDirection> & vPixels)
	{
		int			k;

		bMainOk = true;
		fMaxRadius = 0;

		for (k = 0;k<8;k++)
		{
			vPixels[k].m_Ok = 2;
			vPixels[k].m_fRadius = 0;
		};

		for (int r = 1;(r<STARMAXSIZE) && (bMainOk) && !bBrighterPixel;r++)
		{
			for (k = 0;k<8;k++)
			{
				int		ldX = vPixels[k].m_lXDir*r;
				int		ldY = vPixels[k].m_lYDir*r;

				Bitmap.GetPixel(fX+ldX+0.5, fY+ldY+0.5, vPixels[k].m_fIntensity);
			};

			bMainOk = false;
			for (k = 0;(k<8) && !bBrighterPixel;k++)
			{
				if (vPixels[k].m_Ok)
				{
					if (vPixels[k].m_fIntensity-m_fBackground < 0.25 * (fIntensity - m_fBackground))
					{
						vPixels[k].m_fRadius = r;
						vPixels[k].m_Ok--;
						fMaxRadius = std::max(fMaxRadius, static_cast<double>(r));
					}
					else if (vPixels[k].m_fIntensity > fIntensity)
						bBrighterPixel = true;
				};

				if (vPixels[k].m_Ok)
					bMainOk = true;
			};
		};

		return fMaxRadius>2;
	};
};

/* ------------------------------------------------------------------- */

size_t CRegisteredFrame::RegisterSubRect(CMemoryBitmap* pBitmap, const CRect& rc, STARSET& stars)
{
	double				fMaxIntensity = 0;
	int				i, j, k;
	std::vector<int>	vHistogram;
	size_t nStars{ 0 };

	// Work with a local buffer. Copy the pixel values for the rect.
	const int width = (rc.right - rc.left);
	std::vector<double> values(width * (rc.bottom - rc.top));
	for (int j = rc.top, ndx = 0; j < rc.bottom; ++j)
		for (int i = rc.left; i < rc.right; ++i, ++ndx)		
			pBitmap->GetPixel(i, j, values[ndx]);
	const auto getValue = [&values, rc, width](const int x, const int y) -> double
	{
		return values[(y - rc.top) * width + x - rc.left];
	};

	// Read pixels from the memory bitmap
	// First find the top luminance
	if (m_fBackground == 0.0)
	{
		constexpr size_t Maxsize = size_t{ MAXWORD } + 1;
		constexpr double Maxvalue = double{ MAXWORD };
		vHistogram.resize(Maxsize);
		for (const auto value : values)
		{
			fMaxIntensity = std::max(fMaxIntensity, value);
			++vHistogram[std::min(value * 256.0, Maxvalue)];
		}

		const int lNrTotalValues = ((rc.Width() - 1) * (rc.Height() - 1)) / 2;
		int lNrValues = 0;
		int lIndice = 0;
		while (lNrValues < lNrTotalValues)
		{
			lNrValues += vHistogram[lIndice];
			++lIndice;
		};
		m_fBackground = static_cast<double>(lIndice) / 256.0 / 256.0;
	}
	else
		fMaxIntensity = *std::max_element(values.cbegin(), values.cend());

	if (fMaxIntensity >= m_fMinLuminancy + m_fBackground)
	{
		// Find how many wanabee stars are existing above 90% maximum luminance

		for (double fDeltaRadius = 0; fDeltaRadius < 4; ++fDeltaRadius)
		{
			for (j = rc.top; j < rc.bottom; j++)
			{
				for (i = rc.left; i < rc.right; i++)
				{
//					double			fIntensity;
//					pBitmap->GetPixel(i, j, fIntensity);
					const double fIntensity = getValue(i, j);

					if (fIntensity >= m_fMinLuminancy + m_fBackground)
					{
						// Check that this pixel is not already used in a wanabee star
						bool bNew = true;
						const POINT ptTest{ i, j };

						for (auto it = stars.lower_bound(CStar(ptTest.x - STARMAXSIZE, 0)); it != stars.end() && bNew; ++it)
						{
							if (it->IsInRadius(ptTest))
								bNew = false;
							else if (it->m_fX > ptTest.x + STARMAXSIZE)
								break;
						}

						if (bNew)
						{
							// Search around the point until intensity is divided by 2
							// STARMAXSIZE pixels radius max search
							std::vector<CPixelDirection> vPixels{ {0, -1, 2, 0.0}, {1, 0, 2, 0.0}, {0, 1, 2, 0.0}, {-1, 0, 2, 0.0}, {1, -1, 2, 0.0}, {1, 1, 2, 0.0}, {-1, 1, 2, 0.0}, {-1, -1, 2, 0.0} };

							bool			bBrighterPixel = false;
							bool			bMainOk = true;
							int			lMaxRadius = 0;
							int			lNrBrighterPixels = 0;

							for (int r = 1; r < STARMAXSIZE && bMainOk && !bBrighterPixel; r++)
							{
								for (auto& pixel : vPixels)
								{
									pBitmap->GetPixel(i + pixel.m_lXDir * r, j + pixel.m_lYDir * r, pixel.m_fIntensity);
								}

								bMainOk = false;
								for (k = 0; k < 8 && !bBrighterPixel; k++)
								{
									if (vPixels[k].m_Ok)
									{
										if (vPixels[k].m_fIntensity - m_fBackground < 0.25 * (fIntensity - m_fBackground))
										{
											vPixels[k].m_fRadius = r;
											--vPixels[k].m_Ok;
											lMaxRadius = std::max(lMaxRadius, r);
										}
										else if (vPixels[k].m_fIntensity > 1.05 * fIntensity)
											bBrighterPixel = true;
										else if (vPixels[k].m_fIntensity > fIntensity)
											++vPixels[k].m_lNrBrighterPixels;
									};

									if (vPixels[k].m_Ok)
										bMainOk = true;
									if (vPixels[k].m_lNrBrighterPixels > 2)
										bBrighterPixel = true;
								};
							};

							// Check the roundness of the wanabee star
							if (!bMainOk && !bBrighterPixel && (lMaxRadius > 2))
							{
								// Radiuses should be within fDeltaRadius pixels of each others
								//if (i>=1027 && i<=1035 && j>=2365 && j<=2372)
								//	DebugBreak();

								bool			bWanabeeStarOk = true;
								int			k1, k2;
								double			fMeanRadius1 = 0.0,
												fMeanRadius2 = 0.0;

								for (k1 = 0; (k1 < 4) && bWanabeeStarOk; k1++)
								{
									for (k2 = 0; (k2 < 4) && bWanabeeStarOk; k2++)
									{
										if ((k1 != k2) && labs(vPixels[k2].m_fRadius - vPixels[k1].m_fRadius) > fDeltaRadius)
											bWanabeeStarOk = false;
									};
								};
								for (k1 = 4; (k1 < 8) && bWanabeeStarOk; k1++)
								{
									for (k2 = 4; (k2 < 8) && bWanabeeStarOk; k2++)
									{
										if ((k1 != k2) && labs(vPixels[k2].m_fRadius - vPixels[k1].m_fRadius) > fDeltaRadius)
											bWanabeeStarOk = false;
									};
								};

								for (k1 = 0; k1 < 4; k1++)
									fMeanRadius1 += vPixels[k1].m_fRadius;
								fMeanRadius1 /= 4.0;
								for (k1 = 4; k1 < 8; k1++)
									fMeanRadius2 += vPixels[k1].m_fRadius;
								fMeanRadius2 /= 4.0;
								fMeanRadius2 *= sqrt(2.0);

								//if (fabs(fMeanRadius1 - fMeanRadius2) > fDeltaRadius - 1)
								//	bWanabeeStarOk = false;

								CRect			rcStar;
								int			lLeftRadius = 0;
								int			lRightRadius = 0;
								int			lTopRadius = 0;
								int			lBottomRadius = 0;

								for (k = 0; k < 8; k++)
								{
									if (vPixels[k].m_lXDir < 0)
										lLeftRadius = std::max(lLeftRadius, static_cast<int>(vPixels[k].m_fRadius));
									else if (vPixels[k].m_lXDir > 0)
										lRightRadius = std::max(lRightRadius, static_cast<int>(vPixels[k].m_fRadius));
									if (vPixels[k].m_lYDir < 0)
										lTopRadius = std::max(lTopRadius, static_cast<int>(vPixels[k].m_fRadius));
									else if (vPixels[k].m_lYDir > 0)
										lBottomRadius = std::max(lBottomRadius, static_cast<int>(vPixels[k].m_fRadius));
								};

								rcStar.left   = ptTest.x - lLeftRadius;
								rcStar.right  = ptTest.x + lRightRadius;
								rcStar.top	  = ptTest.y - lTopRadius;
								rcStar.bottom = ptTest.y + lBottomRadius;

								if (bWanabeeStarOk)
								{
									// Add the star
									CStar ms(ptTest.x, ptTest.y);

									ms.m_fIntensity	  = fIntensity;
									ms.m_rcStar		  = rcStar;
									ms.m_fPercentage  = 1.0;
									ms.m_fDeltaRadius = fDeltaRadius;
									ms.m_fMeanRadius  = (fMeanRadius1 + fMeanRadius2) / 2.0;

									// Compute the real position
									if (ComputeStarCenter(pBitmap, ms.m_fX, ms.m_fY, ms.m_fMeanRadius))
									{
										// Check last overlap condition
										{
											for (auto it = stars.lower_bound(CStar(ms.m_fX - ms.m_fMeanRadius * 2.35 / 1.5 - STARMAXSIZE, 0)); it != stars.end() && bWanabeeStarOk; ++it)
											{
												if (Distance(CPointExt(ms.m_fX, ms.m_fY), CPointExt(it->m_fX, it->m_fY)) < (ms.m_fMeanRadius + it->m_fMeanRadius) * 2.35 / 1.5)
													bWanabeeStarOk = false;
												else if (it->m_fX > ms.m_fX + ms.m_fMeanRadius * 2.35 / 1.5 + STARMAXSIZE)
													break;
											}
										};

										// Check comet intersection
										if (m_bComet)
										{
											if (ms.IsInRadius(m_fXComet, m_fYComet))
												bWanabeeStarOk = false;
										};

										if (bWanabeeStarOk)
										{
											ms.m_fQuality = (10 - fDeltaRadius) + fIntensity - ms.m_fMeanRadius;
											FindStarShape(pBitmap, ms);
											stars.insert(ms);
											++nStars;
										};
									};
								};
							};
						};
					};
				};
			};
		};
	};

	if  (vHistogram.size())
		m_fBackground = 0;

	return nStars;
};

/* ------------------------------------------------------------------- */

bool	CRegisteredFrame::SaveRegisteringInfo(LPCTSTR szInfoFileName)
{
	bool				bResult = false;
	FILE *				hFile;

	hFile = _tfopen(szInfoFileName, _T("wt+"));
	if (hFile)
	{
		fprintf(hFile, "OverallQuality = %.2f\n", m_fOverallQuality);
		//fprintf(hFile, "Width = %ld\n", m_lWidth);
		//fprintf(hFile, "Height = %ld\n", m_lHeight);
		fprintf(hFile, "RedXShift = 0.0\n");
		fprintf(hFile, "RedYShift = 0.0\n");
		fprintf(hFile, "BlueXShift = 0.0\n");
		fprintf(hFile, "BlueYShift = 0.0\n");
		if (m_bComet)
			fprintf(hFile, "Comet = %.2f, %.2f\n", m_fXComet, m_fYComet);
		fprintf(hFile, "SkyBackground = %.4f\n", m_SkyBackground.m_fLight);
		fprintf(hFile, "NrStars = %zu\n", m_vStars.size());
		for (int i = 0; i<m_vStars.size();i++)
		{
			fprintf(hFile, "Star# = %ld\n", i);
			fprintf(hFile, "Intensity = %.2f\n", m_vStars[i].m_fIntensity);
			fprintf(hFile, "Quality = %.2f\n",  m_vStars[i].m_fQuality);
			fprintf(hFile, "MeanRadius = %.2f\n",  m_vStars[i].m_fMeanRadius);
			fprintf(hFile, "Rect = %ld, %ld, %ld, %ld\n", m_vStars[i].m_rcStar.left, m_vStars[i].m_rcStar.top, m_vStars[i].m_rcStar.right, m_vStars[i].m_rcStar.bottom);
			fprintf(hFile, "Center = %.2f, %.2f\n", m_vStars[i].m_fX, m_vStars[i].m_fY);
			fprintf(hFile, "Axises = %.2f, %.2f, %.2f, %.2f, %.2f\n", m_vStars[i].m_fMajorAxisAngle,
																	  m_vStars[i].m_fLargeMajorAxis,
																	  m_vStars[i].m_fSmallMajorAxis,
																	  m_vStars[i].m_fLargeMinorAxis,
																	  m_vStars[i].m_fSmallMinorAxis);
		};
		fclose(hFile);
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

static bool GetNextValue(FILE * hFile, CString & strVariable, CString & strValue)
{
	bool				bResult = false;
	CHAR				szText[2000];
	CString				strText;
	int					nPos;

	strVariable.Empty();
	strValue.Empty();
	if (fgets(szText, sizeof(szText), hFile))
	{
		strText = CA2CTEX<sizeof(szText)>(szText, CP_UTF8);
		nPos = strText.Find(_T("="), 0); // Search = sign
		if (nPos >= 0)
		{
			strVariable = strText.Left(nPos-1);
			strValue = strText.Right(strText.GetLength()-nPos-1);
			strVariable.TrimLeft();
			strVariable.TrimRight();
			strValue.TrimLeft();
			strValue.TrimRight();
		}
		else
		{
			strVariable = strText;
			strVariable.TrimLeft();
			strVariable.TrimRight();
		};
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CRegisteredFrame::LoadRegisteringInfo(LPCTSTR szInfoFileName)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult = false;
	FILE *				hFile;

	// Try to open the file as a text file
	hFile = _tfopen(szInfoFileName, _T("rt"));
	if (hFile)
	{
		CString			strVariable;
		CString			strValue;
		int			lNrStars = 0;
		bool			bEnd = false;

		m_bComet = false;

		// Read overall quality
		while (!bEnd)
		{
			GetNextValue(hFile, strVariable, strValue);
			if (!strVariable.CompareNoCase(_T("OverallQuality")))
				m_fOverallQuality = _ttof(strValue);
			if (!strVariable.CompareNoCase(_T("Comet")))
			{
				// Parse value (X, Y)
				int			nPos;
				CString		strX, strY;

				// Get X
				nPos = strValue.Find(_T(","));
				strX = strValue.Left(nPos);
				strX.TrimLeft();
				strX.TrimRight();
				m_fXComet = _ttof(strX);

				strValue = strValue.Right(strValue.GetLength()-nPos-1);
				// Get Y
				strY = strValue;
				strY.TrimLeft();
				strY.TrimRight();
				m_fYComet = _ttof(strY);
				m_bComet = true;
			}
			/*else if (!strVariable.CompareNoCase(_T("Width")))
				m_lWidth = _ttol(strValue);
			else if (!strVariable.CompareNoCase(_T("Height")))
				m_lHeight = _ttol(strValue);
			else if (!strVariable.CompareNoCase(_T("RedXShift")))
				m_fRedXShift = _ttof(strValue);
			else if (!strVariable.CompareNoCase(_T("RedYShift")))
				m_fRedYShift = _ttof(strValue);
			else if (!strVariable.CompareNoCase(_T("BlueXShift")))
				m_fBlueXShift = _ttof(strValue);
			else if (!strVariable.CompareNoCase(_T("BlueYShift")))
				m_fBlueYShift = _ttof(strValue);*/
			else if (!strVariable.CompareNoCase(_T("SkyBackground")))
				m_SkyBackground.m_fLight = _ttof(strValue);
			else if (!strVariable.CompareNoCase(_T("NrStars")))
			{
				lNrStars = _ttol(strValue);
				bEnd = true;
			};
		};

		// Jump the first [Star#]
		GetNextValue(hFile, strVariable, strValue);
		bEnd = false;
		for (int i = 0;i<lNrStars && !bEnd;i++)
		{
			bool			bNextStar = false;
			CStar			ms;

			ms.m_fPercentage  = 0;
			ms.m_fDeltaRadius = 0;

			while (!bNextStar)
			{
				GetNextValue(hFile, strVariable, strValue);
				if (!strVariable.CompareNoCase(_T("Intensity")))
					ms.m_fIntensity = _ttof(strValue);
				else if (!strVariable.CompareNoCase(_T("Quality")))
					ms.m_fQuality = _ttof(strValue);
				else if (!strVariable.CompareNoCase(_T("MeanRadius")))
					ms.m_fMeanRadius = _ttof(strValue);
				else if (!strVariable.CompareNoCase(_T("Rect")))
				{
					// Parse value (left, top, right, bottom)
					int			nPos;
					CString		strCoord;

					// get Left
					nPos = strValue.Find(_T(","));
					strCoord = strValue.Left(nPos);
					strCoord.TrimLeft();
					strCoord.TrimRight();
					ms.m_rcStar.left = _ttol(strCoord);
					strValue = strValue.Right(strValue.GetLength()-nPos-1);
					// get Top
					nPos = strValue.Find(_T(","));
					strCoord = strValue.Left(nPos);
					strCoord.TrimLeft();
					strCoord.TrimRight();
					ms.m_rcStar.top = _ttol(strCoord);
					strValue = strValue.Right(strValue.GetLength()-nPos-1);
					// get Right
					nPos = strValue.Find(_T(","));
					strCoord = strValue.Left(nPos);
					strCoord.TrimLeft();
					strCoord.TrimRight();
					ms.m_rcStar.right = _ttol(strCoord);
					strValue = strValue.Right(strValue.GetLength()-nPos-1);
					// get Bottom
					strCoord = strValue;
					strCoord.TrimLeft();
					strCoord.TrimRight();
					ms.m_rcStar.bottom = _ttol(strCoord);
					strValue = strValue.Right(strValue.GetLength()-nPos-1);
				}
				else if (!strVariable.CompareNoCase(_T("Axises")))
				{
					// Parse value (left, top, right, bottom)
					int			nPos;
					CString		strParams;

					// get Angle
					nPos = strValue.Find(_T(","));
					strParams = strValue.Left(nPos);
					strParams.TrimLeft();
					strParams.TrimRight();
					ms.m_fMajorAxisAngle = _ttof(strParams);
					strValue = strValue.Right(strValue.GetLength()-nPos-1);
					// get Large Major
					nPos = strValue.Find(_T(","));
					strParams = strValue.Left(nPos);
					strParams.TrimLeft();
					strParams.TrimRight();
					ms.m_fLargeMajorAxis = _ttof(strParams);
					strValue = strValue.Right(strValue.GetLength()-nPos-1);
					// get Small Major
					nPos = strValue.Find(_T(","));
					strParams = strValue.Left(nPos);
					strParams.TrimLeft();
					strParams.TrimRight();
					ms.m_fSmallMajorAxis = _ttof(strParams);
					strValue = strValue.Right(strValue.GetLength()-nPos-1);
					// get Large Minor Axis
					nPos = strValue.Find(_T(","));
					strParams = strValue.Left(nPos);
					strParams.TrimLeft();
					strParams.TrimRight();
					ms.m_fLargeMinorAxis = _ttof(strParams);
					strValue = strValue.Right(strValue.GetLength()-nPos-1);
					// get Small Minor Axis
					strParams = strValue;
					strParams.TrimLeft();
					strParams.TrimRight();
					ms.m_fSmallMinorAxis = _ttof(strParams);
				}
				else if (!strVariable.CompareNoCase(_T("Center")))
				{
					// Parse value (X, Y)
					int			nPos;
					CString		strX, strY;

					// Get X
					nPos = strValue.Find(_T(","));
					strX = strValue.Left(nPos);
					strX.TrimLeft();
					strX.TrimRight();
					ms.m_fX = _ttof(strX);

					strValue = strValue.Right(strValue.GetLength()-nPos-1);
					// Get Y
					strY = strValue;
					strY.TrimLeft();
					strY.TrimRight();
					ms.m_fY = _ttof(strY);
				}
				else
				{
					bEnd = !strValue.GetLength();
					bNextStar = !strVariable.CompareNoCase(_T("Star#")) || bEnd;
				};
			};

			if (ms.IsValid())
				m_vStars.push_back(ms);
		};

		fclose(hFile);

		ComputeFWHM();

		m_bInfoOk = true;
		bResult = true;
	}
	else
		m_bInfoOk = false;

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

double	CLightFrameInfo::ComputeMedianValue(CGrayBitmap & Bitmap)
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

/* ------------------------------------------------------------------- */

void CLightFrameInfo::RegisterPicture(CGrayBitmap& Bitmap)
{
	ZFUNCTRACE_RUNTIME();
	// Try to find star by studying the variation of luminosity
	int lSubRectWidth;
	int lSubRectHeight;
	int lProgress = 0;

	// First computed median value
	m_fBackground = ComputeMedianValue(Bitmap);

	m_SkyBackground.m_fLight = m_fBackground;

	lSubRectWidth = STARMAXSIZE * 5;
	lSubRectHeight = STARMAXSIZE * 5;

	const int lNrSubRects = ((Bitmap.Width() - STARMAXSIZE * 2) / lSubRectWidth * 2) * ((Bitmap.Height() - STARMAXSIZE * 2) / lSubRectHeight * 2);

	if (m_pProgress != nullptr)
	{
		CString			strText;
		strText.Format(IDS_REGISTERINGNAME, (LPCTSTR)filePath.c_str());
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
	const int nrEnabledThreads = CMultitask::GetNrProcessors(); // Returns 1 if multithreading disabled by user, otherwise # HW threads

	STARSET stars1, stars2, stars3, stars4;
	std::atomic<int> nrSubrects{ 0 };
	std::atomic<size_t> nStars{ 0 };

	int masterCount{ 0 };
	const auto progress = [this, &nrSubrects, &nStars, &masterCount]() -> void
	{
		if (m_pProgress == nullptr)
			return;
		++nrSubrects;
		if (omp_get_thread_num() == 0 && (++masterCount % 25) == 0) // Only master thread
		{
			CString str;
			str.Format(IDS_REGISTERINGNAMEPLUSTARS, (LPCTSTR)filePath.c_str(), nStars.load());
			m_pProgress->Progress2(str, nrSubrects.load());
		}
	};

	const auto processDisjointArea = [this, StarMaxSize, &Bitmap, stepSize, rectSize, &progress, &nStars](const int yStart, const int yEnd, const int xStart, const int xEnd, STARSET& stars) -> void
	{
		const int rightmostColumn = static_cast<int>(Bitmap.Width()) - StarMaxSize;

		for (int rowNdx = yStart; rowNdx < yEnd; ++rowNdx)
		{
			const int top = StarMaxSize + rowNdx * stepSize;
			const int bottom = std::min(static_cast<int>(Bitmap.Height()) - StarMaxSize, top + rectSize);

			for (int colNdx = xStart; colNdx < xEnd; ++colNdx, progress())
				nStars += RegisterSubRect(&Bitmap, CRect(StarMaxSize + colNdx * stepSize, top, std::min(rightmostColumn, StarMaxSize + colNdx * stepSize + rectSize), bottom), stars);
		}
	};

#pragma omp parallel default(none) shared(stars1, stars2, stars3, stars4) if(nrEnabledThreads - 1)
{
#pragma omp sections
	{
		// Upper left area
#pragma omp section
		processDisjointArea(0, (nrSubrectsY - Separation) / 2, 0, (nrSubrectsX - Separation) / 2, stars1);
		// Upper right area
#pragma omp section
		processDisjointArea(0, (nrSubrectsY - Separation) / 2, (nrSubrectsX - Separation) / 2 + Separation, nrSubrectsX, stars2);
		// Lower left area
#pragma omp section
		processDisjointArea((nrSubrectsY - Separation) / 2 + Separation, nrSubrectsY, 0, (nrSubrectsX - Separation) / 2, stars3);
		// Lower right area
#pragma omp section
		processDisjointArea((nrSubrectsY - Separation) / 2 + Separation, nrSubrectsY, (nrSubrectsX - Separation) / 2 + Separation, nrSubrectsX, stars4);
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
		processDisjointArea(0, nrSubrectsY, (nrSubrectsX - Separation) / 2, (nrSubrectsX - Separation) / 2 + Separation, stars1);
		// Middle left
		processDisjointArea((nrSubrectsY - Separation) / 2, (nrSubrectsY - Separation) / 2 + Separation, 0, (nrSubrectsX - Separation) / 2, stars1);
		// Middle right
		processDisjointArea((nrSubrectsY - Separation) / 2, (nrSubrectsY - Separation) / 2 + Separation, (nrSubrectsX - Separation) / 2 + Separation, nrSubrectsX, stars1);

		m_vStars.assign(stars1.cbegin(), stars1.cend());
	}

#pragma omp sections
	{
#pragma omp section
		ComputeOverallQuality();
#pragma omp section
		ComputeFWHM();
	}
} // omp parallel

	if (m_pProgress)
		m_pProgress->End2();
}


class CComputeLuminanceTask
{
public:
	CGrayBitmap* m_pGrayBitmap;
	CMemoryBitmap* m_pBitmap;
	CDSSProgress* m_pProgress;

public:
	CComputeLuminanceTask(CMemoryBitmap* pBm, CGrayBitmap* pGb, CDSSProgress* pPrg) :
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

	if (m_pProgress != nullptr)
		m_pProgress->SetNrUsedProcessors(nrProcessors);

	AvxLuminance avxLuminance{ *m_pBitmap, *m_pGrayBitmap };

#pragma omp parallel for schedule(dynamic, 50) default(none) firstprivate(avxLuminance) if(nrProcessors > 1)
	for (int row = 0; row < height; row += lineBlockSize)
	{
		if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
			m_pProgress->Progress2(nullptr, progress += nrProcessors * lineBlockSize);

		const int endRow = std::min(row + lineBlockSize, height);
		if (avxLuminance.computeLuminanceBitmap(row, endRow) != 0)
		{
			processNonAvx(row, endRow);
		}
	}

	if (m_pProgress != nullptr)
		m_pProgress->SetNrUsedProcessors();
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


std::shared_ptr<CGrayBitmap> CLightFrameInfo::ComputeLuminanceBitmap(CMemoryBitmap* pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	m_lWidth = pBitmap->Width();
	m_lHeight = pBitmap->Height();

	if (m_bRemoveHotPixels)
		pBitmap->RemoveHotPixels(m_pProgress);

	// Try to find star by studying the variation of luminosity
	if (m_pProgress != nullptr)
	{
		CString strText;
		strText.Format(IDS_COMPUTINGLUMINANCE, (LPCTSTR)filePath.c_str());
		m_pProgress->Start2(strText, m_lHeight);
	}

	std::shared_ptr<CGrayBitmap> pGrayBitmap = std::make_shared<CGrayBitmap>();
	ZTRACE_RUNTIME("Creating Gray memory bitmap %p (luminance)", pGrayBitmap.get());
	pGrayBitmap->Init(pBitmap->Width(), pBitmap->Height());

	CComputeLuminanceTask{ pBitmap, pGrayBitmap.get(), m_pProgress }.process();

	if (m_bApplyMedianFilter)
	{
		std::shared_ptr<CGrayBitmap> pFiltered = std::dynamic_pointer_cast<CGrayBitmap>(CMedianImageFilter{}.ApplyFilter(pGrayBitmap.get(), m_pProgress));
		if (static_cast<bool>(pFiltered))
			return pFiltered;
		else
			throw std::runtime_error("ComputeLuminanceBitmap: Median Image Filter did not return a GrayBitmap.");
	}
	else
		return pGrayBitmap;
}

void CLightFrameInfo::RegisterPicture(CMemoryBitmap* pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	const std::shared_ptr<CGrayBitmap> pGrayBitmap = ComputeLuminanceBitmap(pBitmap);
	if (static_cast<bool>(pGrayBitmap))
		RegisterPicture(*pGrayBitmap);
}

bool CLightFrameInfo::ComputeStarShifts(CMemoryBitmap* pBitmap, CStar& star, double& fRedXShift, double& fRedYShift, double& fBlueXShift, double& fBlueYShift)
{
	// Compute star center for blue and red
	bool				bResult = false;
	int				i, j;
	double				fSumRedX = 0,
						fSumRedY = 0;
	double				fNrValuesRedX = 0,
						fNrValuesRedY = 0;
	double				fAverageRedX = 0,
						fAverageRedY = 0;
	double				fSumBlueX = 0,
						fSumBlueY = 0;
	double				fNrValuesBlueX = 0,
						fNrValuesBlueY = 0;
	double				fAverageBlueX = 0,
						fAverageBlueY = 0;


	int				lNrBlueLines = 0;
	int				lNrRedLines = 0;
	for (j = star.m_rcStar.top;j<=star.m_rcStar.bottom;j++)
	{
		fSumRedX = 0;
		fNrValuesRedX = 0;
		fSumBlueX = 0;
		fNrValuesBlueX = 0;
		for (i = star.m_rcStar.left;i<=star.m_rcStar.right;i++)
		{
			double			fRed, fGreen, fBlue;

			pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
			fSumRedX += fRed * i;
			fNrValuesRedX += fRed;
			fSumBlueX += fBlue * i;
			fNrValuesBlueX += fBlue;
		};
		if (fNrValuesRedX)
		{
			fAverageRedX += fSumRedX/fNrValuesRedX;
			lNrRedLines++;
		}
		if (fNrValuesBlueX)
		{
			fAverageBlueX += fSumBlueX/fNrValuesBlueX;
			lNrBlueLines++;
		};
	};
	if (lNrRedLines)
		fAverageRedX /= (double)lNrRedLines;
	if (lNrBlueLines)
		fAverageBlueX /= (double)lNrBlueLines;

	int				lNrRedColumns = 0;
	int				lNrBlueColumns = 0;
	for (j = star.m_rcStar.left;j<=star.m_rcStar.right;j++)
	{
		fSumRedY = 0;
		fNrValuesRedY = 0;
		fSumBlueY = 0;
		fNrValuesBlueY = 0;
		for (i = star.m_rcStar.top;i<=star.m_rcStar.bottom;i++)
		{
			double			fRed, fGreen, fBlue;

			pBitmap->GetPixel(j, i, fRed, fGreen, fBlue);
			fSumRedY += fRed * i;
			fNrValuesRedY += fRed;
			fSumBlueY += fBlue * i;
			fNrValuesBlueY += fBlue;
		};
		if (fNrValuesRedY)
		{
			fAverageRedY += fSumRedY/fNrValuesRedY;
			lNrRedColumns++;
		};
		if (fNrValuesBlueY)
		{
			fAverageBlueY += fSumBlueY/fNrValuesBlueY;
			lNrBlueColumns++;
		};
	};
	if (lNrRedColumns)
		fAverageRedY /= (double)lNrRedColumns;
	if (lNrBlueColumns)
		fAverageBlueY /= (double)lNrBlueColumns;

	fRedXShift = fAverageRedX - star.m_fX;
	fRedYShift = fAverageRedY - star.m_fY;
	fBlueXShift = fAverageBlueX - star.m_fX;
	fBlueYShift = fAverageBlueY - star.m_fY;

	bResult = lNrRedColumns && lNrRedLines && lNrBlueLines && lNrBlueColumns;

	return bResult;
};

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
/* ------------------------------------------------------------------- */

void CLightFrameInfo::RegisterPicture()
{
	ZFUNCTRACE_RUNTIME();
	CBitmapInfo			bmpInfo;
	bool				bLoaded;

	if (GetPictureInfo(filePath.c_str(), bmpInfo) && bmpInfo.CanLoad())
	{
		CString						strText;
		CString						strDescription;

		bmpInfo.GetDescription(strDescription);

		if (bmpInfo.m_lNrChannels == 3)
			strText.Format(IDS_LOADRGBPICTURE, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, filePath.c_str());
		else
			strText.Format(IDS_LOADGRAYPICTURE, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, filePath.c_str());

		if (m_pProgress != nullptr)
			m_pProgress->Start2(strText, 0);

		std::shared_ptr<CMemoryBitmap> pBitmap;
		bLoaded = ::FetchPicture(filePath.c_str(), pBitmap, m_pProgress);

		if (m_pProgress != nullptr)
			m_pProgress->End2();

		if (bLoaded)
		{
			RegisterPicture(pBitmap.get());
//			ComputeRedBlueShifting(pBitmap);
		}
	}
}

/* ------------------------------------------------------------------- */

void CLightFrameInfo::RegisterPicture(LPCTSTR szBitmap, double fMinLuminancy, bool bRemoveHotPixels, bool bApplyMedianFilter, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	Reset();
	filePath = fs::path(szBitmap);
	m_fMinLuminancy		= fMinLuminancy;
	m_fBackground		= 0.0;
	m_bRemoveHotPixels  = bRemoveHotPixels;
	m_bApplyMedianFilter= bApplyMedianFilter ? true : false;
	m_pProgress			= pProgress;

	RegisterPicture();

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

void CLightFrameInfo::SetBitmap(fs::path path, bool bProcessIfNecessary, bool bForceRegister)
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
	_tmakepath(szInfoName, szDrive, szDir, szFile, _T(".Info.txt"));

	m_strInfoFileName = szInfoName;

	if (bForceRegister || (!ReadInfoFileName() && bProcessIfNecessary))
	{
		RegisterPicture();
		SaveRegisteringInfo();
	}
}

/* ------------------------------------------------------------------- */

bool CRegisterEngine::SaveCalibratedLightFrame(CLightFrameInfo& lfi, std::shared_ptr<CMemoryBitmap> pBitmap, CDSSProgress* pProgress, CString& strCalibratedFile)
{
	bool bResult = false;

	if (!lfi.filePath.empty() != 0 && static_cast<bool>(pBitmap))
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(lfi.filePath.c_str(), szDrive, szDir, szName, nullptr);

		strOutputFile = szDrive;
		strOutputFile += szDir;
		strOutputFile += szName;
		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile += _T(".cal.tif");
		else
		{
			CString			strExt;

			GetFITSExtension(lfi.filePath, strExt);
			strOutputFile += ".cal"+strExt;
		}

		strCalibratedFile = strOutputFile;

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
			CString				strText;

			strText.Format(IDS_SAVINGCALIBRATED, strOutputFile);
			pProgress->Start2(strText, 0);
		}

		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pOutBitmap.get(), pProgress, _T("Calibrated light frame"), lfi.m_lISOSpeed, lfi.m_lGain, lfi.m_fExposure, lfi.m_fAperture);
		else
			bResult = WriteFITS(strOutputFile, pOutBitmap.get(), pProgress, _T("Calibrated light frame"), lfi.m_lISOSpeed, lfi.m_lGain, lfi.m_fExposure);

		if (CFATransform == CFAT_SUPERPIXEL)
			pCFABitmapInfo->UseSuperPixels(true);

		if (pProgress)
			pProgress->End2();
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CRegisterEngine::RegisterLightFrames(CAllStackingTasks& tasks, bool bForce, CDSSProgress* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool					bResult = true;
	CString					strText;
	int					lTotalRegistered = 0;

	for (size_t i = 0; i < tasks.m_vStacks.size(); i++)
	{
		CStackingInfo *		pStackingInfo = nullptr;

		if (tasks.m_vStacks[i].m_pLightTask)
			pStackingInfo = &(tasks.m_vStacks[i]);
		if (pStackingInfo)
			lTotalRegistered += (int)pStackingInfo->m_pLightTask->m_vBitmaps.size();
	};

	strText.LoadString(IDS_REGISTERING);
	if (pProgress)
		pProgress->Start(strText, lTotalRegistered, true);

	bResult = tasks.DoAllPreTasks(pProgress);

	// Do it again in case pretasks change the progress
	if (pProgress)
		pProgress->Start(strText, lTotalRegistered, true);

	for (size_t i = 0; i < tasks.m_vStacks.size() && bResult; i++)
	{
		CStackingInfo *		pStackingInfo = nullptr;

		if (tasks.m_vStacks[i].m_pLightTask)
			pStackingInfo = &(tasks.m_vStacks[i]);

		if (pStackingInfo)
		{
			CMasterFrames				MasterFrames;

			MasterFrames.LoadMasters(pStackingInfo, pProgress);

			for (size_t j = 0; j < pStackingInfo->m_pLightTask->m_vBitmaps.size() && bResult; j++)
			{
				// Register this bitmap
				CLightFrameInfo lfi;

				ZTRACE_RUNTIME("Register %s", pStackingInfo->m_pLightTask->m_vBitmaps[j].filePath.generic_string().c_str());

				lfi.SetProgress(pProgress);
				lfi.SetBitmap(pStackingInfo->m_pLightTask->m_vBitmaps[j].filePath.c_str(), false, false);

				if (pProgress)
				{
					strText.Format(IDS_REGISTERINGPICTURE, static_cast<int>(j+1), lTotalRegistered);
					pProgress->Progress1(strText, static_cast<int>(j));
				};

				if (bForce || !lfi.IsRegistered())
				{
					CBitmapInfo		bmpInfo;
					// Load the bitmap
					if (GetPictureInfo(lfi.filePath.c_str(), bmpInfo) && bmpInfo.CanLoad())
					{
						
						CString						strText;
						CString						strDescription;

						bmpInfo.GetDescription(strDescription);

						if (bmpInfo.m_lNrChannels==3)
							strText.Format(IDS_LOADRGBLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, lfi.filePath.c_str());
						else
							strText.Format(IDS_LOADGRAYLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, lfi.filePath.c_str());
						if (pProgress)
							pProgress->Start2(strText, 0);

						std::shared_ptr<CMemoryBitmap> pBitmap;
						if (::FetchPicture(lfi.filePath.c_str(), pBitmap, pProgress))
						{
							// Apply offset, dark and flat to lightframe
							MasterFrames.ApplyAllMasters(pBitmap, nullptr, pProgress);

							CString strCalibratedFile;

							if (m_bSaveCalibrated && (pStackingInfo->m_pDarkTask || pStackingInfo->m_pDarkFlatTask || pStackingInfo->m_pFlatTask || pStackingInfo->m_pOffsetTask))
								SaveCalibratedLightFrame(lfi, pBitmap, pProgress, strCalibratedFile);

							// Then register the light frame
							lfi.SetProgress(pProgress);
							lfi.RegisterPicture(pBitmap.get());
							lfi.SaveRegisteringInfo();

							if (strCalibratedFile.GetLength())
							{
								CString				strInfoFileName;
								TCHAR				szDrive[1+_MAX_DRIVE];
								TCHAR				szDir[1+_MAX_DIR];
								TCHAR				szFile[1+_MAX_FNAME];

								_tsplitpath(strCalibratedFile, szDrive, szDir, szFile, nullptr);
								strInfoFileName.Format(_T("%s%s%s%s"), szDrive, szDir, szFile, _T(".Info.txt"));
								lfi.CRegisteredFrame::SaveRegisteringInfo(strInfoFileName);
							};
						};

						if (pProgress)
						{
							pProgress->End2();
							bResult = !pProgress->IsCanceled();
						}
					}
				}
			}
		}
	}

	// Clear stuff
	tasks.ClearCache();

	return bResult;
}

/* ------------------------------------------------------------------- */
