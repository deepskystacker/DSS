
#include <stdafx.h>
#include "RegisterEngine.h"
#include "Registry.h"
#include "MasterFrames.h"
#include "BackgroundCalibration.h"
#include "PixelTransform.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "Filters.h"

#define _USE_MATH_DEFINES
#include <math.h>

const LONG				STARMAXSIZE = 20;

/* ------------------------------------------------------------------- */

class CStarAxisInfo
{
public :
	LONG					m_lAngle;
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

inline	void NormalizeAngle(LONG & lAngle)
{
	while (lAngle >= 360)
		lAngle -= 360;
	while (lAngle < 0)
		lAngle += 360;
};

/* ------------------------------------------------------------------- */

BOOL	CRegisteredFrame::FindStarShape(CMemoryBitmap * pBitmap, CStar & star)
{
	BOOL						bResult = FALSE;
	std::vector<CStarAxisInfo>	vStarAxises;
	double						fMaxHalfRadius = 0.0;
	double						fMaxCumulated  = 0.0;
	LONG						lMaxHalfRadiusAngle = 0.0;
	LONG						lAngle;

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
			PIXELDISPATCHVECTOR		vPixels;

			ComputePixelDispatch(CPointExt(fX, fY), vPixels);

			for (LONG k = 0;k<vPixels.size();k++)
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

	LONG			lSearchAngle;
	BOOL			bFound = FALSE;

	lSearchAngle = lMaxHalfRadiusAngle + 180;
	NormalizeAngle(lSearchAngle);

	for (LONG i = 0;i<vStarAxises.size() && !bFound;i++)
	{
		if (vStarAxises[i].m_lAngle == lSearchAngle)
		{
			bFound = TRUE;
			star.m_fSmallMajorAxis = vStarAxises[i].m_fRadius;
		};
	};

	bFound		 = FALSE;
	lSearchAngle = lMaxHalfRadiusAngle + 90;
	NormalizeAngle(lSearchAngle);

	for (LONG i = 0;i<vStarAxises.size() && !bFound;i++)
	{
		if (vStarAxises[i].m_lAngle == lSearchAngle)
		{
			bFound = TRUE;
			star.m_fLargeMinorAxis = vStarAxises[i].m_fRadius;
		};
	};

	bFound		 = FALSE;
	lSearchAngle = lMaxHalfRadiusAngle + 210;
	NormalizeAngle(lSearchAngle);

	for (LONG i = 0;i<vStarAxises.size() && !bFound;i++)
	{
		if (vStarAxises[i].m_lAngle == lSearchAngle)
		{
			bFound = TRUE;
			star.m_fSmallMinorAxis = vStarAxises[i].m_fRadius;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CRegisteredFrame::ComputeStarCenter(CMemoryBitmap * pBitmap, double & fX, double & fY, double & fRadius)
{
	BOOL				bResult = FALSE;
	LONG				i, j;
	double				fSumX = 0,
						fSumY = 0;
	double				fNrValuesX = 0,
						fNrValuesY = 0;
	double				fAverageX = 0,
						fAverageY = 0;


	LONG				lNrLines = 0;
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

	LONG				lNrColumns = 0;
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
		fValue = max(0.0, fValue - m_fBackground);
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
		fValue = max(0.0, fValue - m_fBackground);
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
	LONG				m_Ok;
	double				m_fIntensity;
	LONG				m_lXDir;
	LONG				m_lYDir;
	double				m_fRadius;
	LONG				m_lNrBrighterPixels;

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
	CPixelDirection(LONG lXDir = 0, LONG lYDir = 0)
	{
		m_Ok = 2;
		m_fIntensity = 0;
		m_lXDir      = lXDir;
		m_lYDir      = lYDir;
		m_fRadius	 = 0;
		m_lNrBrighterPixels = 0;
	};

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
	BOOL			bBrighterPixel;
	BOOL			bMainOk;
	double			fMaxRadius;
	double			m_fBackground;
	double			fIntensity;

public :
	CPixelDirections() 
	{
		bBrighterPixel = FALSE;
		bMainOk		   = TRUE;
		fMaxRadius	   = 0;
	};
	BOOL	FillPixelDirection(double fX, double fY, CGrayBitmap & Bitmap, std::vector<CPixelDirection> & vPixels)
	{
		LONG			k;

		bMainOk = TRUE;
		fMaxRadius = 0;

		for (k = 0;k<8;k++)
		{
			vPixels[k].m_Ok = 2;
			vPixels[k].m_fRadius = 0;
		};

		for (LONG r = 1;(r<STARMAXSIZE) && (bMainOk) && !bBrighterPixel;r++)
		{
			for (k = 0;k<8;k++)
			{
				LONG		ldX = vPixels[k].m_lXDir*r;
				LONG		ldY = vPixels[k].m_lYDir*r;

				Bitmap.GetPixel(fX+ldX+0.5, fY+ldY+0.5, vPixels[k].m_fIntensity);
			};

			bMainOk = FALSE;
			for (k = 0;(k<8) && !bBrighterPixel;k++)
			{
				if (vPixels[k].m_Ok)
				{
					if (vPixels[k].m_fIntensity-m_fBackground < 0.25 * (fIntensity - m_fBackground))
					{
						vPixels[k].m_fRadius = r;
						vPixels[k].m_Ok--;
						fMaxRadius = max(fMaxRadius, r);
					}
					else if (vPixels[k].m_fIntensity > fIntensity)
						bBrighterPixel = TRUE;
				};

				if (vPixels[k].m_Ok)
					bMainOk = TRUE;
			};
		};

		return fMaxRadius>2;
	};
};

/* ------------------------------------------------------------------- */

void	CRegisteredFrame::RegisterSubRect(CMemoryBitmap * pBitmap, CRect & rc)
{
	double				fMaxIntensity = 0;
	LONG				i, j, k;
	std::vector<LONG>	vHistogram;

	if (!m_fBackground)
		vHistogram.resize((LONG)MAXWORD+1);
	// Read pixels from the memory bitmap
	// First find the top luminance
	for (i = rc.left;i<rc.right;i++)
	{
		for (j = rc.top;j<rc.bottom;j++)
		{
			double			fGray;
			pBitmap->GetPixel(i, j, fGray);
			fMaxIntensity = max(fGray, fMaxIntensity);

			if (!m_fBackground)
			{
				fGray *= 256.0;
				fGray = min(fGray, MAXWORD);

				vHistogram[fGray]++;
			};
		};
	};

	if (!m_fBackground)
	{
		LONG	lNrTotalValues = (rc.Width()-1)*(rc.Height()-1);
		lNrTotalValues /= 2;

		LONG	lNrValues = 0;
		LONG	lIndice   = 0;
		while (lNrValues < lNrTotalValues)
		{
			lNrValues += vHistogram[lIndice];
			lIndice++;
		};
		m_fBackground = (double)lIndice/256.0/256.0;
	};

	if (fMaxIntensity >= m_fMinLuminancy+m_fBackground)
	{
		// Find how many wanabee stars are existing above 90% maximum luminance
		std::vector<CPixelDirection>	vPixels;

		vPixels.reserve(16);

		for (double fDeltaRadius = 0;fDeltaRadius < 4;fDeltaRadius ++)
		{
			for (j = rc.top;j<rc.bottom;j++)
			{
				for (i = rc.left;i<rc.right;i++)
				{
					double			fIntensity;

					pBitmap->GetPixel(i, j, fIntensity);

					if (fIntensity >= m_fMinLuminancy+m_fBackground)
					{
						// Check that this pixel is not already used in a wanabee star
						BOOL		bNew = TRUE;
						POINT		ptTest;

						ptTest.x = i;	ptTest.y = j;

						{
							STARSETITERATOR	it;

							it = m_sStars.lower_bound(CStar(ptTest.x-STARMAXSIZE, 0));
							while (it != m_sStars.end() && bNew)
							{
								if ((*it).IsInRadius(ptTest))
									bNew = FALSE;
								else if ((*it).m_fX > ptTest.x + STARMAXSIZE)
									it = m_sStars.end();
								else
									it++;
							};
						};

						if (bNew)
						{
							// Search around the point until intensity is divided by 2
							// 20 pixels radius max search
							vPixels.resize(0);
							vPixels.push_back(CPixelDirection(0, -1));
							vPixels.push_back(CPixelDirection(1, 0));
							vPixels.push_back(CPixelDirection(0, 1));
							vPixels.push_back(CPixelDirection(-1, 0));
							vPixels.push_back(CPixelDirection(1, -1));
							vPixels.push_back(CPixelDirection(1, 1));
							vPixels.push_back(CPixelDirection(-1, 1));
							vPixels.push_back(CPixelDirection(-1, -1));

							BOOL			bBrighterPixel = FALSE;
							BOOL			bMainOk = TRUE;
							LONG			lMaxRadius = 0;
							LONG			lNrBrighterPixels = 0;

							for (k = 0;k<8;k++)
							{
								vPixels[k].m_Ok = 2;
								vPixels[k].m_fRadius = 0;
							};

							for (LONG r = 1;(r<STARMAXSIZE) && (bMainOk) && !bBrighterPixel;r++)
							{
								for (k = 0;k<8;k++)
								{
									LONG		ldX = vPixels[k].m_lXDir*r;
									LONG		ldY = vPixels[k].m_lYDir*r;

									pBitmap->GetPixel(i+ldX, j+ldY, vPixels[k].m_fIntensity);
								};

								bMainOk = FALSE;
								for (k = 0;(k<8) && !bBrighterPixel;k++)
								{
									if (vPixels[k].m_Ok)
									{
										if (vPixels[k].m_fIntensity-m_fBackground < 0.25 * (fIntensity - m_fBackground))
										{
											vPixels[k].m_fRadius = r;
											vPixels[k].m_Ok--;
											lMaxRadius = max(lMaxRadius, r);
										}
										else if (vPixels[k].m_fIntensity > 1.05*fIntensity)
											bBrighterPixel = TRUE;
										else if (vPixels[k].m_fIntensity > fIntensity)
											vPixels[k].m_lNrBrighterPixels++;
									};

									if (vPixels[k].m_Ok)
										bMainOk = TRUE;
									if (vPixels[k].m_lNrBrighterPixels>2)
										bBrighterPixel = TRUE;
								};
							};

							// Check the roundness of the wanabee star 
							if (!bMainOk && !bBrighterPixel && (lMaxRadius > 2))
							{
								// Radiuses should be within fDeltaRadius pixels of each others
								//if (i>=1027 && i<=1035 && j>=2365 && j<=2372)
								//	DebugBreak();

								BOOL			bWanabeeStarOk = TRUE;
								LONG			k1, k2;
								double			fMeanRadius1 = 0.0,
												fMeanRadius2 = 0.0;
								
								for (k1 = 0;(k1 <4) && bWanabeeStarOk;k1++)
								{
									for (k2 = 0;(k2 < 4) && bWanabeeStarOk;k2++)
									{
										if ((k1 != k2) && labs(vPixels[k2].m_fRadius-vPixels[k1].m_fRadius) > fDeltaRadius)
											bWanabeeStarOk = FALSE;
									};
								};
								for (k1 = 4;(k1 <8) && bWanabeeStarOk;k1++)
								{
									for (k2 = 4;(k2 < 8) && bWanabeeStarOk;k2++)
									{
										if ((k1 != k2) && labs(vPixels[k2].m_fRadius-vPixels[k1].m_fRadius) > fDeltaRadius)
											bWanabeeStarOk = FALSE;
									};
								};

								for (k1 = 0;k1 < 4;k1++)
									fMeanRadius1 += vPixels[k1].m_fRadius;
								fMeanRadius1 /= 4.0;
								for (k1 = 4;k1 < 8;k1++)
									fMeanRadius2 += vPixels[k1].m_fRadius;
								fMeanRadius2 /= 4.0;
								fMeanRadius2 *= sqrt(2.0);

								//if (fabs(fMeanRadius1 - fMeanRadius2) > fDeltaRadius - 1)
								//	bWanabeeStarOk = FALSE;

								CRect			rcStar;
								LONG			lLeftRadius = 0;
								LONG			lRightRadius = 0;
								LONG			lTopRadius = 0;
								LONG			lBottomRadius = 0;

								for (k = 0;k<8;k++)
								{
									if (vPixels[k].m_lXDir<0)
										lLeftRadius = max(lLeftRadius, vPixels[k].m_fRadius);
									else if (vPixels[k].m_lXDir>0)
										lRightRadius = max(lRightRadius, vPixels[k].m_fRadius);
									if (vPixels[k].m_lYDir<0)
										lTopRadius = max(lTopRadius, vPixels[k].m_fRadius);
									else if (vPixels[k].m_lYDir>0)
										lBottomRadius = max(lBottomRadius, vPixels[k].m_fRadius);
								};

								rcStar.left   = ptTest.x - lLeftRadius;
								rcStar.right  = ptTest.x + lRightRadius;
								rcStar.top	  = ptTest.y - lTopRadius;
								rcStar.bottom = ptTest.y + lBottomRadius;

								if (bWanabeeStarOk)
								{
									// Add the star
									CStar			ms;

									ms.m_fIntensity	  = fIntensity;
									ms.m_rcStar		  = rcStar;
									ms.m_fPercentage  = 1.0;
									ms.m_fDeltaRadius = fDeltaRadius;
									ms.m_fMeanRadius  = (fMeanRadius1 + fMeanRadius2) / 2.0;

									// Compute the real position
									ms.m_fX = ptTest.x;
									ms.m_fY = ptTest.y;
									if (ComputeStarCenter(pBitmap, ms.m_fX, ms.m_fY, ms.m_fMeanRadius))
									{
										// Check last overlap condition
										{
											STARSETITERATOR	it;

											it = m_sStars.lower_bound(CStar(ms.m_fX-ms.m_fMeanRadius*2.35/1.5-STARMAXSIZE, 0));
											while (it != m_sStars.end() && bWanabeeStarOk)
											{
												if (Distance(CPointExt(ms.m_fX, ms.m_fY), CPointExt((*it).m_fX, (*it).m_fY)) < (ms.m_fMeanRadius + (*it).m_fMeanRadius)*2.35/1.5)
													bWanabeeStarOk = FALSE;
												else if ((*it).m_fX > ms.m_fX + ms.m_fMeanRadius*2.35/1.5 + STARMAXSIZE)
													it = m_sStars.end();
												else
													it++;
											};
										};

										// Check comet intersection
										if (m_bComet)
										{
											if (ms.IsInRadius(m_fXComet, m_fYComet))
												bWanabeeStarOk = FALSE;
										};

										if (bWanabeeStarOk)
										{
											ms.m_fQuality	  = (10 - fDeltaRadius) + fIntensity - ms.m_fMeanRadius;

											FindStarShape(pBitmap, ms);
											m_vStars.push_back(ms);
											m_sStars.insert(ms);
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
};

/* ------------------------------------------------------------------- */

BOOL	CRegisteredFrame::SaveRegisteringInfo(LPCTSTR szInfoFileName)
{
	BOOL				bResult = FALSE;
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
		for (LONG i = 0; i<m_vStars.size();i++)
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
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

static BOOL GetNextValue(FILE * hFile, CString & strVariable, CString & strValue)
{
	BOOL				bResult = FALSE;
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
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CRegisteredFrame::LoadRegisteringInfo(LPCTSTR szInfoFileName)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
	FILE *				hFile;

	// Try to open the file as a text file
	hFile = _tfopen(szInfoFileName, _T("rt"));
	if (hFile)
	{
		CString			strVariable;
		CString			strValue;
		LONG			lNrStars = 0;
		BOOL			bEnd = FALSE;
		
		m_bComet = FALSE;

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
				m_bComet = TRUE;
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
				bEnd = TRUE;
			};
		};

		// Jump the first [Star#]
		GetNextValue(hFile, strVariable, strValue);
		bEnd = FALSE;
		for (LONG i = 0;i<lNrStars && !bEnd;i++)
		{
			BOOL			bNextStar = FALSE;
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

		m_bInfoOk = TRUE;
		bResult = TRUE;
	}
	else
		m_bInfoOk = FALSE;

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
	BackgroundCalibration.ComputeBackgroundCalibration(&Bitmap, TRUE, m_pProgress);
	fResult = BackgroundCalibration.m_fTgtRedBk/256.0;

	return fResult;
};

/* ------------------------------------------------------------------- */

void	CLightFrameInfo::RegisterPicture(CGrayBitmap & Bitmap)
{
	ZFUNCTRACE_RUNTIME();
	// Try to find star by studying the variation of luminosity
	LONG					lSubRectWidth,
							lSubRectHeight;
	LONG					lNrSubRects;
	LONG					i, j;
	LONG					lProgress = 0;

	// First computed median value
	m_fBackground = ComputeMedianValue(Bitmap);

	m_SkyBackground.m_fLight = m_fBackground;

	lSubRectWidth	= STARMAXSIZE * 5;
	lSubRectHeight	= STARMAXSIZE * 5;

	lNrSubRects		= ((Bitmap.Width()-STARMAXSIZE*2) / lSubRectWidth * 2) * ((Bitmap.Height()-STARMAXSIZE*2) / lSubRectHeight * 2);

	if (m_pProgress)
	{
		CString			strText;

		strText.Format(IDS_REGISTERINGNAME, (LPCTSTR)m_strFileName);
		m_pProgress->Start2(strText, lNrSubRects);
	};

	m_vStars.clear();
	m_sStars.clear();
	for (j = STARMAXSIZE;j<Bitmap.Height()-STARMAXSIZE;j+=lSubRectHeight/2)
	{
		for (i = STARMAXSIZE;i<Bitmap.Width()-STARMAXSIZE;i+=lSubRectWidth/2)
		{
			CRect			rcSubRect;

			rcSubRect.left = i;	rcSubRect.right  = min(Bitmap.Width()-STARMAXSIZE, i + lSubRectWidth);
			rcSubRect.top = j;	rcSubRect.bottom = min(Bitmap.Height()-STARMAXSIZE, j + lSubRectHeight);

			RegisterSubRect(&Bitmap, rcSubRect);

			lProgress++;
			if (m_pProgress)
			{
				CString			strText;

				strText.Format(IDS_REGISTERINGNAMEPLUSTARS, (LPCTSTR)m_strFileName, m_vStars.size());
				m_pProgress->Progress2(strText, lProgress);
			};
		};
	};
	m_sStars.clear();

	// Compute overall quality
	ComputeOverallQuality();

	// Compute FWHM
	ComputeFWHM();

	std::sort(m_vStars.begin(), m_vStars.end());

	if (m_pProgress)
		m_pProgress->End2();
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CComputeLuminanceTask : public CMultitask
{
public :
	CSmartPtr<CGrayBitmap>		m_pGrayBitmap;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CDSSProgress *				m_pProgress;

public :
	CComputeLuminanceTask()
	{
	};

	virtual ~CComputeLuminanceTask()
	{
	};

	void	Init(CMemoryBitmap * pBitmap, CGrayBitmap * pGrayBitmap, CDSSProgress * pProgress)
	{
		m_pBitmap				 = pBitmap;
		m_pGrayBitmap		     = pGrayBitmap;
		m_pProgress				 = pProgress;
	};

	virtual BOOL	DoTask(HANDLE hEvent);
	virtual BOOL	Process();
};

/* ------------------------------------------------------------------- */

BOOL	CComputeLuminanceTask::DoTask(HANDLE hEvent)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	LONG				i, j;
	BOOL				bEnd = FALSE;
	MSG					msg;
	LONG				lWidth = m_pBitmap->Width();

	// Create a message queue and signal the event
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(hEvent);
	while (!bEnd && GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_MT_PROCESS)
		{
			for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
			{
				for (i = 0;i<lWidth;i++)
				{
					COLORREF16			crColor;

					crColor = m_pBitmap->GetPixel16(i, j);
					m_pGrayBitmap->SetPixel(i, j, GetIntensity(crColor));
				};
			};

			SetEvent(hEvent);
		}
		else if (msg.message == WM_MT_STOP)
			bEnd = TRUE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL	CComputeLuminanceTask::Process()
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;
	LONG				lHeight = m_pBitmap->Height();
	LONG				i = 0;
	LONG				lStep;
	LONG				lRemaining;

	if (m_pProgress)
		m_pProgress->SetNrUsedProcessors(GetNrThreads());
	lStep		= max(1, lHeight/50);
	lRemaining	= lHeight;
	bResult = TRUE;
	while (i<lHeight)
	{
		LONG			lAdd = min(lStep, lRemaining);
		DWORD			dwThreadId;
		
		dwThreadId = GetAvailableThreadId();
		PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

		i			+=lAdd;
		lRemaining	-= lAdd;
		if (m_pProgress)
			m_pProgress->Progress2(NULL, i);
	};

	CloseAllThreads();

	if (m_pProgress)
		m_pProgress->SetNrUsedProcessors();

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CLightFrameInfo::ComputeLuminanceBitmap(CMemoryBitmap * pBitmap, CGrayBitmap ** ppGrayBitmap)
{
	ZFUNCTRACE_RUNTIME();
	CSmartPtr<CGrayBitmap>		pGrayBitmap;

	m_lWidth	= pBitmap->Width();
	m_lHeight	= pBitmap->Height();

	if (m_bRemoveHotPixels)
		pBitmap->RemoveHotPixels(m_pProgress);

	// Try to find star by studying the variation of luminosity
	if (m_pProgress)
	{
		CString			strText;

		strText.Format(IDS_COMPUTINGLUMINANCE, (LPCTSTR)m_strFileName);
		m_pProgress->Start2(strText, pBitmap->Height());
	};

	pGrayBitmap.Attach(new CGrayBitmap);
	ZTRACE_RUNTIME("Creating Gray memory bitmap %p (luminance)", pGrayBitmap.m_p);
	pGrayBitmap->Init(pBitmap->Width(), pBitmap->Height());

	CComputeLuminanceTask		ComputeLuminanceTask;

	ComputeLuminanceTask.Init(pBitmap, pGrayBitmap, m_pProgress);
	ComputeLuminanceTask.StartThreads();
	ComputeLuminanceTask.Process();

	if (m_bApplyMedianFilter)
	{
		CMedianImageFilter			filter;
		CSmartPtr<CMemoryBitmap>	pFiltered;
		CSmartPtr<CGrayBitmap>		pFilteredGray;

		filter.ApplyFilter(pGrayBitmap, &pFiltered, m_pProgress);

		pFilteredGray.Attach(dynamic_cast<CGrayBitmap *>(pFiltered.m_p));
		pFilteredGray.CopyTo(ppGrayBitmap);
	}
	else
		pGrayBitmap.CopyTo(ppGrayBitmap);
};

/* ------------------------------------------------------------------- */

void	CLightFrameInfo::RegisterPicture(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	CSmartPtr<CGrayBitmap>		pGrayBitmap;

	ComputeLuminanceBitmap(pBitmap, &pGrayBitmap);
	if (pGrayBitmap)
		RegisterPicture(*pGrayBitmap);
};

/* ------------------------------------------------------------------- */

BOOL CLightFrameInfo::ComputeStarShifts(CMemoryBitmap * pBitmap, CStar & star, double & fRedXShift, double & fRedYShift, double & fBlueXShift, double & fBlueYShift)
{
	// Compute star center for blue and red
	BOOL				bResult = FALSE;
	LONG				i, j;
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


	LONG				lNrBlueLines = 0;
	LONG				lNrRedLines = 0;
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

	LONG				lNrRedColumns = 0;
	LONG				lNrBlueColumns = 0;
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
	LONG				i = 0;
	LONG				lNrShifts = 0;

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
	BOOL				bLoaded;

	if (GetPictureInfo(m_strFileName, bmpInfo) && bmpInfo.CanLoad())
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;
		CString						strText;
		CString						strDescription;

		bmpInfo.GetDescription(strDescription);

		if (bmpInfo.m_lNrChannels==3)
			strText.Format(IDS_LOADRGBPICTURE, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)m_strFileName);
		else
			strText.Format(IDS_LOADGRAYPICTURE, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)m_strFileName);
		if (m_pProgress)
			m_pProgress->Start2(strText, 0);

		bLoaded = ::LoadPicture(m_strFileName, &pBitmap, m_pProgress);

		if (m_pProgress)
			m_pProgress->End2();

		if (bLoaded)
		{
			RegisterPicture(pBitmap);
//			ComputeRedBlueShifting(pBitmap);
		};
	};
};

/* ------------------------------------------------------------------- */

void CLightFrameInfo::RegisterPicture(LPCTSTR szBitmap, double fMinLuminancy, BOOL bRemoveHotPixels, BOOL bApplyMedianFilter, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	Reset();
	m_strFileName		= szBitmap;
	m_fMinLuminancy		= fMinLuminancy;
	m_fBackground		= 0.0;
	m_bRemoveHotPixels  = bRemoveHotPixels;
	m_bApplyMedianFilter= bApplyMedianFilter ? true : false;
	m_pProgress			= pProgress;

	RegisterPicture();

	m_pProgress = NULL;
};

/* ------------------------------------------------------------------- */

BOOL CLightFrameInfo::ReadInfoFileName()
{
	return LoadRegisteringInfo(m_strInfoFileName);
};

/* ------------------------------------------------------------------- */

void CLightFrameInfo::SaveRegisteringInfo()
{
	m_bInfoOk = CRegisteredFrame::SaveRegisteringInfo(m_strInfoFileName);
};

/* ------------------------------------------------------------------- */

void CLightFrameInfo::SetBitmap(LPCTSTR szBitmap, BOOL bProcessIfNecessary, BOOL bForceRegister)
{
	TCHAR				szDrive[1+_MAX_DRIVE];
	TCHAR				szDir[1+_MAX_DIR];
	TCHAR				szFile[1+_MAX_FNAME];
	TCHAR				szExt[1+_MAX_EXT];
	TCHAR				szInfoName[1+_MAX_PATH];

	Reset();
	m_bInfoOk = FALSE;
	m_strFileName = szBitmap;
	_tsplitpath(m_strFileName, szDrive, szDir, szFile, szExt);
	_tmakepath(szInfoName, szDrive, szDir, szFile, _T(".Info.txt"));

	m_strInfoFileName = szInfoName;

	if (bForceRegister || (!ReadInfoFileName() && bProcessIfNecessary))
	{
		RegisterPicture();
		SaveRegisteringInfo();
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL	CRegisterEngine::SaveCalibratedLightFrame(CLightFrameInfo & lfi, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, CString & strCalibratedFile)
{
	BOOL				bResult = FALSE;

	if (lfi.m_strFileName.GetLength() && pBitmap)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(lfi.m_strFileName, szDrive, szDir, szName, NULL);

		strOutputFile = szDrive;
		strOutputFile += szDir;
		strOutputFile += szName;
		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile += _T(".cal.tif");
		else
		{
			CString			strExt;

			GetFITSExtension(lfi.m_strFileName, strExt);
			strOutputFile += ".cal"+strExt;
		};

		strCalibratedFile = strOutputFile;

		CSmartPtr<CMemoryBitmap>		pOutBitmap;

		if (m_bSaveCalibratedDebayered)
		{
			// Debayer the image
			if (!DebayerPicture(pBitmap, &pOutBitmap, pProgress))
				pOutBitmap = pBitmap;
		}
		else
			pOutBitmap = pBitmap;

		// Check and remove super pixel settings
		CCFABitmapInfo *			pCFABitmapInfo;
		CFATRANSFORMATION			CFATransform = CFAT_NONE;

		pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pOutBitmap.m_p);
		if (pCFABitmapInfo)
		{
			CFATransform = pCFABitmapInfo->GetCFATransformation();
			if (CFATransform == CFAT_SUPERPIXEL)
				pCFABitmapInfo->UseBilinear(TRUE);
		};

		if (pProgress)
		{
			CString				strText;

			strText.Format(IDS_SAVINGCALIBRATED, strOutputFile);
			pProgress->Start2(strText, 0);
		};

		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pOutBitmap, pProgress, _T("Calibrated light frame"), lfi.m_lISOSpeed, lfi.m_fExposure);
		else
			bResult = WriteFITS(strOutputFile, pOutBitmap, pProgress, _T("Calibrated light frame"), lfi.m_lISOSpeed, lfi.m_fExposure);

		if ((CFATransform == CFAT_SUPERPIXEL) && pCFABitmapInfo)
			pCFABitmapInfo->UseSuperPixels(TRUE);

		if (pProgress)
			pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CRegisterEngine::RegisterLightFrames(CAllStackingTasks & tasks, BOOL bForce, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL					bResult = TRUE;
	LONG					i, j;
	CString					strText;
	LONG					lTotalRegistered = 0;
	LONG					lNrRegistered = 0;

	for (i = 0;i<tasks.m_vStacks.size();i++)
	{
		CStackingInfo *		pStackingInfo = NULL;

		if (tasks.m_vStacks[i].m_pLightTask)
			pStackingInfo = &(tasks.m_vStacks[i]);
		if (pStackingInfo)
			lTotalRegistered += (LONG)pStackingInfo->m_pLightTask->m_vBitmaps.size();
	};

	strText.LoadString(IDS_REGISTERING);
	if (pProgress)
		pProgress->Start(strText, lTotalRegistered, TRUE);

	bResult = tasks.DoAllPreTasks(pProgress);

	// Do it again in case pretasks change the progress
	if (pProgress)
		pProgress->Start(strText, lTotalRegistered, TRUE);

	for (i = 0;i<tasks.m_vStacks.size() && bResult;i++)
	{
		CStackingInfo *		pStackingInfo = NULL;

		if (tasks.m_vStacks[i].m_pLightTask)
			pStackingInfo = &(tasks.m_vStacks[i]);

		if (pStackingInfo)
		{
			CMasterFrames				MasterFrames;

			MasterFrames.LoadMasters(pStackingInfo, pProgress);

			for (j = 0;j<pStackingInfo->m_pLightTask->m_vBitmaps.size() && bResult;j++)
			{
				// Register this bitmap
				CLightFrameInfo		lfi;

				ZTRACE_RUNTIME("Register %s", (LPCTSTR)pStackingInfo->m_pLightTask->m_vBitmaps[j].m_strFileName);

				lfi.SetProgress(pProgress);
				lfi.SetBitmap(pStackingInfo->m_pLightTask->m_vBitmaps[j].m_strFileName, FALSE, FALSE);
				lNrRegistered++;
				
				if (pProgress)
				{
					strText.Format(IDS_REGISTERINGPICTURE, lNrRegistered, lTotalRegistered);
					pProgress->Progress1(strText, lNrRegistered);
				};

				if (bForce || !lfi.IsRegistered())
				{
					CBitmapInfo		bmpInfo;
					// Load the bitmap
					if (GetPictureInfo(lfi.m_strFileName, bmpInfo) && bmpInfo.CanLoad())
					{
						CSmartPtr<CMemoryBitmap>	pBitmap;
						CString						strText;
						CString						strDescription;

						bmpInfo.GetDescription(strDescription);

						if (bmpInfo.m_lNrChannels==3)
							strText.Format(IDS_LOADRGBLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)lfi.m_strFileName);
						else
							strText.Format(IDS_LOADGRAYLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, (LPCTSTR)lfi.m_strFileName);
						if (pProgress)
							pProgress->Start2(strText, 0);

						if (::LoadPicture(lfi.m_strFileName, &pBitmap, pProgress))
						{
							// Apply offset, dark and flat to lightframe
							MasterFrames.ApplyAllMasters(pBitmap, NULL, pProgress);

							CString				strCalibratedFile;

							if (m_bSaveCalibrated &&
								(pStackingInfo->m_pDarkTask || pStackingInfo->m_pDarkFlatTask ||
								pStackingInfo->m_pFlatTask || pStackingInfo->m_pOffsetTask))
								SaveCalibratedLightFrame(lfi, pBitmap, pProgress, strCalibratedFile);

							// Then register the light frame
							lfi.SetProgress(pProgress);
							lfi.RegisterPicture(pBitmap);
							lfi.SaveRegisteringInfo();

							if (strCalibratedFile.GetLength())
							{
								CString				strInfoFileName;
								TCHAR				szDrive[1+_MAX_DRIVE];
								TCHAR				szDir[1+_MAX_DIR];
								TCHAR				szFile[1+_MAX_FNAME];

								_tsplitpath(strCalibratedFile, szDrive, szDir, szFile, NULL);
								strInfoFileName.Format(_T("%s%s%s%s"), szDrive, szDir, szFile, _T(".Info.txt"));
								lfi.CRegisteredFrame::SaveRegisteringInfo(strInfoFileName);
							};
						};

						if (pProgress)
						{
							pProgress->End2();
							bResult = !pProgress->IsCanceled();
						};
					};			
				};
			};
		};
	};

	// Clear stuff
	tasks.ClearCache();

	return bResult;
};

/* ------------------------------------------------------------------- */
