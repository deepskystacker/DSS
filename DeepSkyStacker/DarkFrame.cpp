#include <stdafx.h>
#include "DarkFrame.h"
#include "Registry.h"
#include "DSSProgress.h"
#include "Histogram.h"
#include <set>
#include <algorithm>
#include "Filters.h"

#include "TIFFUtil.h"

#define _USE_MATH_DEFINES
#include <math.h>

/* ------------------------------------------------------------------- */

class CValuePair
{
public :
	WORD		m_wLightValue;
	WORD		m_wDarkValue;
	LONG		m_lCount;

private :
	void	CopyFrom(const CValuePair & vp)
	{
		m_wLightValue = vp.m_wLightValue;
		m_wDarkValue  = vp.m_wDarkValue;
		m_lCount	  = vp.m_lCount;
	};

public :
	CValuePair(WORD wLightValue = 0, WORD wDarkValue = 0)
	{
		m_wLightValue = wLightValue;
		m_wDarkValue  = wDarkValue;
		m_lCount = 1;
	};

	CValuePair(const CValuePair & vp)
	{
		CopyFrom(vp);
	};

	const CValuePair & operator = (const CValuePair & vp)
	{
		CopyFrom(vp);
		return (*this);
	};

	virtual ~CValuePair()
	{
	};

	bool operator < (const CValuePair & vp) const
	{
		if (m_wDarkValue > vp.m_wDarkValue)
			return true;
		else if (m_wDarkValue < vp.m_wDarkValue)
			return false;
		else
			return (m_wLightValue > vp.m_wLightValue);
	};
};

typedef std::vector<CValuePair>			VALUEPAIRVECTOR;
typedef std::set<CValuePair>			VALUEPAIRSET;
//typedef std::vector<CValuePair>			VALUEPAIRSET;
typedef VALUEPAIRSET::iterator			VALUEPAIRITERATOR;

/* ------------------------------------------------------------------- */

class CSplittedValuePairs
{
public :
	std::vector<VALUEPAIRSET>			m_vsValuePairs;
	std::vector<double>					m_vRMSMultiplier;
	std::vector<double>					m_vEntropyMultiplier;

public :
	CSplittedValuePairs()
	{
	};

	virtual ~CSplittedValuePairs()
	{
	};

	void	SplitValuePairs(VALUEPAIRSET & sValuePairs)
	{
		ZFUNCTRACE_RUNTIME();
		LONG				lNrValues = 0;
		LONG				lNrThreshold;
		LONG				lNrAdded = 0;
		VALUEPAIRITERATOR	it;

		for (it = sValuePairs.begin();it != sValuePairs.end();it++)
			lNrValues += (*it).m_lCount;

		// Make 5 classes
		lNrThreshold = lNrValues/5;
		m_vsValuePairs.clear();
		m_vRMSMultiplier.clear();
		m_vEntropyMultiplier.clear();

		VALUEPAIRSET		sCurrentValuePairs;

		for (it = sValuePairs.begin();it != sValuePairs.end();it++)
		{
			sCurrentValuePairs.insert((*it));
			lNrAdded += (*it).m_lCount;
			if (lNrAdded>=lNrThreshold)
			{
				m_vsValuePairs.push_back(sCurrentValuePairs);
				sCurrentValuePairs.clear();
				lNrAdded = 0;
			};
		};

		// Add the last one
		if (lNrAdded > 0)
			m_vsValuePairs.push_back(sCurrentValuePairs);

		m_vRMSMultiplier.resize(m_vsValuePairs.size());
		m_vEntropyMultiplier.resize(m_vsValuePairs.size());
	};
};

/* ------------------------------------------------------------------- */

/*
Entropy of a gaussian variable :
H(X) = 1/2 (1+ log(2*PI*(sigma*sigma)))
*/

static double	ComputeMinimumEntropyFactor(VALUEPAIRSET & sValuePairs)
{
	ZFUNCTRACE_RUNTIME();
	LONG				lSize = (LONG)sValuePairs.size();
	double				fMinEntropy = -1.0;
	double				fSelectedk	= 0.0;
	const LONG			MINNEGATIVE = 128;
	std::vector<double>	vValues;

	for (double k = 0.00;k<=5.0;k+=0.01)
	{
		// Compute cumulated entropy
		VALUEPAIRITERATOR		it;
		std::vector<WORD>		vHisto;
		LONG					lNrPixels = 0;

		vHisto.resize((LONG)MINNEGATIVE*2+1);

		for (it = sValuePairs.begin(); it != sValuePairs.end();it++)
		{
			if ((*it).m_wLightValue + MINNEGATIVE >= (double)(*it).m_wDarkValue * k)
			{
				LONG			lIndice = (double)(*it).m_wLightValue - (double)(*it).m_wDarkValue * k;
				lIndice /= ((LONG)MAXWORD+1)/MINNEGATIVE;
				lIndice += MINNEGATIVE;

				if (lIndice >= 0)
				{
					vHisto[lIndice]+=(*it).m_lCount;
					lNrPixels+=(*it).m_lCount;
				};
			}
		};
		// Compute Entropy
		double					fEntropy = 0;

		for (LONG i = 0;i<vHisto.size();i++)
		{
			if (vHisto[i])
			{
				double			fProbability;

				fProbability = (double)vHisto[i]/(double)lNrPixels;
				fEntropy += -fProbability * log(fProbability)/log(2.0);
			};
		};

		vValues.push_back(fEntropy);

		if ((fEntropy < fMinEntropy) || fSelectedk == 0)
		{
			fMinEntropy = fEntropy;
			fSelectedk  = k;
		};
	};

	return fSelectedk;
};

/* ------------------------------------------------------------------- */

inline BOOL IsInStdDev(double fValue, double fMean, double fStdDev)
{
	ZFUNCTRACE_RUNTIME();
	return (fValue >= fMean-2*fStdDev) && (fValue <= fMean+2*fStdDev);
};

static double ComputeMinimumRMSFactor(VALUEPAIRSET & sValuePairs)
{
	ZFUNCTRACE_RUNTIME();
	LONG				lSize = (LONG)sValuePairs.size();
	double				fMinRMS = -1.0;
	double				fSelectedk	= 0.0;
	const LONG			MINNEGATIVE = MAXWORD;
	std::vector<double>	vValues;
	std::vector<double>	vEntropies;
	double				fDarkMean = 0.0;
	double				fDarkStdDev = 0.0;
	double				fLightMean = 0.0;
	double				fLightStdDev = 0.0;

	{
		VALUEPAIRITERATOR		it;
		double					fDarkSum = 0.0;
		double					fDarkPowSum = 0.0;
		double					fLightSum = 0.0;
		double					fLightPowSum = 0.0;
		LONG					lNrValues = 0;

		for (it = sValuePairs.begin();it != sValuePairs.end();it++)
		{
			fDarkSum += (*it).m_wDarkValue * (*it).m_lCount;
			fDarkPowSum += (*it).m_wDarkValue * (*it).m_wDarkValue * (*it).m_lCount;
			fLightSum += (*it).m_wLightValue * (*it).m_lCount;
			fLightPowSum += (*it).m_wLightValue * (*it).m_wLightValue * (*it).m_lCount;
			lNrValues += (*it).m_lCount;
		};

		if (lNrValues)
		{
			fDarkMean = fDarkSum/lNrValues;
			fDarkStdDev = sqrt(fDarkPowSum/lNrValues - pow(fDarkSum/lNrValues, 2));
			fLightMean = fLightSum/lNrValues;
			fLightStdDev = sqrt(fLightPowSum/lNrValues - pow(fLightSum/lNrValues, 2));
		};
	}

	for (double k = 0.00;k<=5.0;k+=0.01)
	{
		VALUEPAIRITERATOR		it;
		double					fPowSum = 0.0;
		double					fSum = 0.0;
		LONG					lNrValues = 0;
		double					fMaxValue = 0.0;

		for (it = sValuePairs.begin(); it != sValuePairs.end();it++)
		{
			//if ((*it).m_wLightValue >= (*it).m_wDarkValue )
			{
				{
					double			fValue = (double)(*it).m_wLightValue- ((double)(*it).m_wDarkValue)*k;

					//if (fValue >= 0)
					{
						lNrValues += (*it).m_lCount;
						fPowSum += (fValue*fValue)*(*it).m_lCount;
						fSum	+= fValue * (*it).m_lCount;
						fMaxValue = max(fMaxValue, fValue);
					};
				};
			}
		};
		// Compute Root Mean Square
		double					fRMS = 0;
		double					fVariance = 0;

		fVariance = fPowSum/lNrValues - pow(fSum/lNrValues, 2);
		fRMS = sqrt(fPowSum/lNrValues);

		fRMS = fRMS*fSum;

		vValues.push_back(fRMS);
		vEntropies.push_back(1.0/2.0*(1.0+log(2*M_PI*fVariance)));

		if ((fRMS < fMinRMS) || fSelectedk == 0)
		{
			fMinRMS = fRMS;
			fSelectedk  = k;
		};
	};

	return fSelectedk;
};

/* ------------------------------------------------------------------- */

static void	RetrictValues(VALUEPAIRSET & sValuePairs)
{
	ZFUNCTRACE_RUNTIME();
	VALUEPAIRITERATOR		it;
	double					fSum = 0.0;
	double					fPowSum = 0.0;
	LONG					lNrValues = 0;

	for (it = sValuePairs.begin(); it != sValuePairs.end();it++)
	{
		fSum		+= (*it).m_wDarkValue * (*it).m_lCount;
		fPowSum		+= (*it).m_wDarkValue * (*it).m_wDarkValue * (*it).m_lCount;
		lNrValues	+= (*it).m_lCount;
	};

	double					fStdDev;
	double					fMean;
	VALUEPAIRSET			sTempValuePairs;

	fMean   = fSum / (double)lNrValues;
	fStdDev = sqrt(fPowSum/(double)lNrValues - pow(fMean, 2));

	for (it = sValuePairs.begin();it != sValuePairs.end();it++)
	{
		if (((*it).m_wLightValue <= fMean+2*fStdDev) && ((*it).m_wLightValue >= fMean-2*fStdDev))
		{
			if ((*it).m_wDarkValue)
				sTempValuePairs.insert((*it));
		};
	};

	sValuePairs = sTempValuePairs;
};

/* ------------------------------------------------------------------- */

class CSubSquare
{
public :
	CRect					m_rcSquare;
	double					m_fMean;
	double					m_fStdDev;

private :
	void	CopyFrom(const CSubSquare & sq)
	{
		m_rcSquare	  = sq.m_rcSquare;
		m_fMean		  = sq.m_fMean;
		m_fStdDev	  = sq.m_fStdDev;
	};

public :
	CSubSquare()
	{
        m_fMean = 0;
        m_fStdDev = 0;
	};

	CSubSquare(const CSubSquare & sq)
	{
		CopyFrom(sq);
	};

	const CSubSquare & operator = (const CSubSquare & sq)
	{
		CopyFrom(sq);
		return (*this);
	};

	virtual ~CSubSquare()
	{
	};
};

/* ------------------------------------------------------------------- */

class CSubSquares
{
public :
	std::vector<CSubSquare>	m_vSubSquares;
	LONG					m_lNrPixels;

public :
	CSubSquares()
	{
		m_lNrPixels = 0;
	};

	virtual ~CSubSquares()
	{
	};

	void	Init(LONG lWidth, LONG lHeight)
	{
		LONG		lSize = min(lWidth, lHeight);

		lSize = min(100L, lSize/10);
		m_lNrPixels = 0;

		for (LONG i = lSize;i<lWidth-2*lSize;i+=lSize)
			for (LONG j = lSize;j<lHeight-2*lSize;j+=lSize)
		{
			CSubSquare			sq;

			sq.m_rcSquare.left		= i;
			sq.m_rcSquare.right		= i+lSize-1;
			sq.m_rcSquare.top		= j;
			sq.m_rcSquare.bottom	= j+lSize-1;

			m_vSubSquares.push_back(sq);

			m_lNrPixels += lSize*lSize;
		};
	};

	LONG	GetNrPixels()
	{
		return m_lNrPixels;
	};
};

/* ------------------------------------------------------------------- */

void	CDarkFrame::FillExcludedPixelList(STARVECTOR * pStars, EXCLUDEDPIXELVECTOR & vExcludedPixels)
{
	ZFUNCTRACE_RUNTIME();
	EXCLUDEDPIXELSET			sExcludedPixels;
	LONG						i;

	vExcludedPixels.clear();
	// First add hot pixels if any
	for (i = 0;i<m_vHotPixels.size();i++)
	{
		CExcludedPixel		ep(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY);

		sExcludedPixels.insert(ep);
	};

	// Then add stars to the list
	if (pStars)
	{
		for (i = 0;i<pStars->size();i++)
		{
			CRect &			rcStar = (*pStars)[i].m_rcStar;

			for (LONG x = rcStar.left;x<=rcStar.right;x++)
				for (LONG y = rcStar.top;y<=rcStar.bottom;y++)
				{
					CExcludedPixel	ep(x, y);

					if (sExcludedPixels.find(ep) == sExcludedPixels.end())
						sExcludedPixels.insert(ep);
				};
		};
	};

	EXCLUDEDPIXELITERATOR		it;

	for (it = sExcludedPixels.begin(); it != sExcludedPixels.end();it++)
		vExcludedPixels.push_back((*it));

	std::sort(vExcludedPixels.begin(), vExcludedPixels.end());
};

/* ------------------------------------------------------------------- */

void	CDarkFrame::ComputeOptimalDistributionRatio(CMemoryBitmap * pBitmap, CMemoryBitmap * pDark, double & fRatio, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	EXCLUDEDPIXELVECTOR		vExcludedPixels;
	LONG					i, j;
	LONG					lWidth = pBitmap->RealWidth(),
							lHeight = pBitmap->RealHeight();

	VALUEPAIRSET			sRedValuePairs;
	VALUEPAIRSET			sGreenValuePairs;
	VALUEPAIRSET			sBlueValuePairs;

	if (pProgress)
		pProgress->Start2(nullptr, lHeight);


	for (j = 0;j<pBitmap->RealHeight();j++)
	{
		for (i = 0;i<pBitmap->RealWidth();i++)
		{
			VALUEPAIRITERATOR		it;
			double					fLight;
			double					fDark;

			pBitmap->GetPixel(i, j, fLight);
			pDark->GetPixel(i, j, fDark);

			{
				CValuePair			vp(fLight*256.0, fDark * 256.0);

				switch (pBitmap->GetBayerColor(i, j))
				{
				case BAYER_RED :
					it = sRedValuePairs.find(vp);
					if (it != sRedValuePairs.end())
						(const_cast<CValuePair&>(*it)).m_lCount++; //MATD
						//(*it).m_lCount++;
					else
						sRedValuePairs.insert(vp);
					break;
				case BAYER_GREEN :
					it = sGreenValuePairs.find(vp);
					if (it != sGreenValuePairs.end())
						(const_cast<CValuePair&>(*it)).m_lCount++;//MATD
						//(*it).m_lCount++;
					else
						sGreenValuePairs.insert(vp);
					break;
				case BAYER_BLUE :
					it = sBlueValuePairs.find(vp);
					if (it != sBlueValuePairs.end())
						(const_cast<CValuePair&>(*it)).m_lCount++;//MATD - and more below
						//(*it).m_lCount++;
					else
						sBlueValuePairs.insert(vp);
					break;
				};
			}
		};

		if (pProgress)
			pProgress->Progress2(nullptr, j+1);
	};

	// Remove the hot pixels
	for (i = 0;i<m_vHotPixels.size();i++)
	{
		VALUEPAIRITERATOR		it;
		double					fLight;
		double					fDark;

		pBitmap->GetPixel(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, fLight);
		m_pMasterDark->GetPixel(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, fDark);

		{
			CValuePair			vp(fLight*256.0, fDark * 256.0);

			switch (pBitmap->GetBayerColor(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY))
			{
			case BAYER_RED :
				it = sRedValuePairs.find(vp);
				if (it != sRedValuePairs.end() && (*it).m_lCount>0)
					(const_cast<CValuePair&>(*it)).m_lCount--;
					//(*it).m_lCount--;
				break;
			case BAYER_GREEN :
				it = sGreenValuePairs.find(vp);
				if (it != sGreenValuePairs.end() && (*it).m_lCount>0)
					(const_cast<CValuePair&>(*it)).m_lCount--;
					//(*it).m_lCount--;
				break;
			case BAYER_BLUE :
				it = sBlueValuePairs.find(vp);
				if (it != sBlueValuePairs.end() && (*it).m_lCount>0)
					(const_cast<CValuePair&>(*it)).m_lCount--;
					//(*it).m_lCount--;
				break;
			};
		}
	};

	fRatio = 1.0;
	BOOL					bOk = FALSE;

	while ((fRatio>0.1) && !bOk)
	{
		VALUEPAIRITERATOR		it;
		CDynamicStats			RedStats,
								GreenStats,
								BlueStats;

		LONG					lNrNegativeReds = 0;
		LONG					lNrNegativeBlues = 0;
		LONG					lNrNegativeGreens = 0;


		for (it = sRedValuePairs.begin();it != sRedValuePairs.end();it++)
			if ((*it).m_lCount && (*it).m_wLightValue && (*it).m_wDarkValue)
				RedStats.AddValue(max(0.0, (double)(*it).m_wLightValue - fRatio * (*it).m_wDarkValue), (*it).m_lCount);

		for (it = sGreenValuePairs.begin();it != sGreenValuePairs.end();it++)
			if ((*it).m_lCount && (*it).m_wLightValue && (*it).m_wDarkValue)
				GreenStats.AddValue(max(0.0, (double)(*it).m_wLightValue - fRatio * (*it).m_wDarkValue), (*it).m_lCount);

		for (it = sBlueValuePairs.begin();it != sBlueValuePairs.end();it++)
			if ((*it).m_lCount && (*it).m_wLightValue && (*it).m_wDarkValue)
				BlueStats.AddValue(max(0.0, (double)(*it).m_wLightValue - fRatio * (*it).m_wDarkValue), (*it).m_lCount);

		double					fSigmaRed	= RedStats.Sigma(),
								fSigmaGreen = GreenStats.Sigma(),
								fSigmaBlue  = BlueStats.Sigma();
		double					fAvgRed		= RedStats.Max()-RedStats.Min(),
								fAvgGreen	= RedStats.Average(),
								fAvgBlue	= RedStats.Average();

		fRatio -= 0.1;
	};

	if (!bOk)
		fRatio = 0.1;

	if (pProgress)
		pProgress->End2();

	{
		VALUEPAIRITERATOR		it;

		FILE *					hRed;
		FILE *					hGreen;
		FILE *					hBlue;

		hRed = fopen("E:\\RedDistribution.txt", "wt");
		for (it = sRedValuePairs.begin();it != sRedValuePairs.end();it++)
			if ((*it).m_lCount)
				fprintf(hRed, "%d\t%d\t%ld\n", (*it).m_wLightValue, (*it).m_wDarkValue, (*it).m_lCount);
		fclose(hRed);

		hGreen = fopen("E:\\GreenDistribution.txt", "wt");
		for (it = sGreenValuePairs.begin();it != sGreenValuePairs.end();it++)
			if ((*it).m_lCount)
				fprintf(hGreen, "%d\t%d\t%ld\n", (*it).m_wLightValue, (*it).m_wDarkValue, (*it).m_lCount);
		fclose(hGreen);

		hBlue = fopen("E:\\BlueDistribution.txt", "wt");
		for (it = sBlueValuePairs.begin();it != sBlueValuePairs.end();it++)
			if ((*it).m_lCount)
				fprintf(hBlue, "%d\t%d\t%ld\n", (*it).m_wLightValue, (*it).m_wDarkValue, (*it).m_lCount);
		fclose(hBlue);
	};
};


/* ------------------------------------------------------------------- */

void	CDarkFrame::ComputeDarkFactor(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	EXCLUDEDPIXELVECTOR		vExcludedPixels;
	LONG					i, j, k;

	FillExcludedPixelList(pStars, vExcludedPixels);

	fRedFactor		= 1.0;
	fGreenFactor	= 1.0;
	fBlueFactor		= 1.0;

	ZTRACE_RUNTIME("Compute Dark coefficients");
	if (pProgress)
	{
		CString		strText;
		strText.LoadString(IDS_COMPUTINGDARKOPTIMIZATIONFACTOR);

		pProgress->Start2(strText, 0);
	};

	CSubSquares				SubSquares;
	LONG					lBestSquare = -1;
	double					fMinMean = 0.0;
	LONG					lNrPixels = 0;
	CSubSquare				SubSquare;

	SubSquares.Init(pBitmap->RealWidth(), pBitmap->RealHeight());

	if (pProgress)
		pProgress->Start2(nullptr, SubSquares.GetNrPixels());

	for (k = 0;k<SubSquares.m_vSubSquares.size();k++)
	{
		CSubSquare &		sq = SubSquares.m_vSubSquares[k];
		double				fSum = 0.0;
		double				fPowSum = 0.0;
		LONG				lNrValues = 0;

		for (i = sq.m_rcSquare.left;i<=sq.m_rcSquare.right;i++)
			for (j = sq.m_rcSquare.top;j<=sq.m_rcSquare.bottom;j++)
			{
				double		fValue;

				pBitmap->GetPixel(i, j, fValue);
				fPowSum += fValue * fValue;
				fSum    += fValue;
				lNrValues++;

				lNrPixels++;
			};
		for (i = 0;i<vExcludedPixels.size();i++)
		{
			VALUEPAIRITERATOR		it;
			double					fValue;

			if (sq.m_rcSquare.PtInRect(CPoint(vExcludedPixels[i].X, vExcludedPixels[i].Y)))
			{
				pBitmap->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fValue);
				fPowSum -= fValue*fValue;
				fSum    -= fValue;
				lNrValues--;
			};
		};
		if (lNrValues)
		{
			sq.m_fMean = fSum/lNrValues;
			sq.m_fStdDev = sqrt(fPowSum/lNrValues-pow(fSum/lNrValues, 2));
		};

		if ((sq.m_fStdDev < fMinMean) || lBestSquare < 0)
		{
			lBestSquare = k;
			fMinMean    = sq.m_fStdDev;
		};

		if (pProgress)
			pProgress->Progress2(nullptr, lNrPixels);
	};

	SubSquare = SubSquares.m_vSubSquares[lBestSquare];

	// From now on we work only with the best square
	if (pBitmap->IsMonochrome() && !pBitmap->IsCFA())
	{
		VALUEPAIRSET		sValuePairs;

		for (i = SubSquare.m_rcSquare.left;i<=SubSquare.m_rcSquare.right;i++)
		{
			for (j = SubSquare.m_rcSquare.top;j<SubSquare.m_rcSquare.bottom;j++)
			{
				VALUEPAIRITERATOR		it;
				double					fLight;
				double					fDark;

				pBitmap->GetPixel(i, j, fLight);
				m_pMasterDark->GetPixel(i, j, fDark);

				it = sValuePairs.find(CValuePair(fLight*256.0, fDark * 256.0));
				if (it != sValuePairs.end())
					(const_cast<CValuePair&>(*it)).m_lCount++;
					//(*it).m_lCount++;
				else
					sValuePairs.insert(CValuePair(fLight*256.0, fDark * 256.0));
			};
		};

		// Remove excluded pixels
		for (i = 0;i<vExcludedPixels.size();i++)
		{
			VALUEPAIRITERATOR		it;
			double					fLight;
			double					fDark;

			if (SubSquare.m_rcSquare.PtInRect(CPoint(vExcludedPixels[i].X, vExcludedPixels[i].Y)))
			{
				pBitmap->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fLight);
				m_pMasterDark->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fDark);

				CValuePair			vp(fLight*256.0, fDark * 256.0);

				it = sValuePairs.find(CValuePair(fLight*256.0, fDark * 256.0));
				if (it != sValuePairs.end() && (*it).m_lCount>0)
					(const_cast<CValuePair&>(*it)).m_lCount++;
					//(*it).m_lCount++;
			};
		};

		fRedFactor = fGreenFactor = fBlueFactor = ComputeMinimumEntropyFactor(sValuePairs);
		//fRedFactor = fGreenFactor = fBlueFactor = ComputeMinimumRMSFactor(sValuePairs);
		ZTRACE_RUNTIME("Monochrome coefficient: %.2f", fRedFactor);
	}
	else if (pBitmap->IsMonochrome() && pBitmap->IsCFA())
	{
		VALUEPAIRSET		sRedValuePairs;
		VALUEPAIRSET		sGreenValuePairs;
		VALUEPAIRSET		sBlueValuePairs;

		for (i = SubSquare.m_rcSquare.left;i<=SubSquare.m_rcSquare.right;i++)
		{
			for (j = SubSquare.m_rcSquare.top;j<SubSquare.m_rcSquare.bottom;j++)
			{
				VALUEPAIRITERATOR		it;
				double					fLight;
				double					fDark;

				pBitmap->GetPixel(i, j, fLight);
				m_pMasterDark->GetPixel(i, j, fDark);

				{
					CValuePair			vp(fLight*256.0, fDark * 256.0);

					switch (pBitmap->GetBayerColor(i, j))
					{
					case BAYER_RED :
						it = sRedValuePairs.find(vp);
						if (it != sRedValuePairs.end())
							(const_cast<CValuePair&>(*it)).m_lCount++;
							//(*it).m_lCount++;
						else
							sRedValuePairs.insert(vp);
						break;
					case BAYER_GREEN :
						it = sGreenValuePairs.find(vp);
						if (it != sGreenValuePairs.end())
							(const_cast<CValuePair&>(*it)).m_lCount++;
							//(*it).m_lCount++;
						else
							sGreenValuePairs.insert(vp);
						break;
					case BAYER_BLUE :
						it = sBlueValuePairs.find(vp);
						if (it != sBlueValuePairs.end())
							(const_cast<CValuePair&>(*it)).m_lCount++;
							//(*it).m_lCount++;
						else
							sBlueValuePairs.insert(vp);
						break;
					};
				}
			};
		};

		// Remove Hot pixels
		for (i = 0;i<vExcludedPixels.size();i++)
		{
			VALUEPAIRITERATOR		it;
			double					fLight;
			double					fDark;

			if (SubSquare.m_rcSquare.PtInRect(CPoint(vExcludedPixels[i].X, vExcludedPixels[i].Y)))
			{
				pBitmap->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fLight);
				m_pMasterDark->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fDark);

				{
					CValuePair			vp(fLight*256.0, fDark * 256.0);

					switch (pBitmap->GetBayerColor(vExcludedPixels[i].X, vExcludedPixels[i].Y))
					{
					case BAYER_RED :
						it = sRedValuePairs.find(vp);
						if (it != sRedValuePairs.end() && (*it).m_lCount>0)
							(const_cast<CValuePair&>(*it)).m_lCount--;
							//(*it).m_lCount--;
						break;
					case BAYER_GREEN :
						it = sGreenValuePairs.find(vp);
						if (it != sGreenValuePairs.end() && (*it).m_lCount>0)
							(const_cast<CValuePair&>(*it)).m_lCount--;
							//(*it).m_lCount--;
						break;
					case BAYER_BLUE :
						it = sBlueValuePairs.find(vp);
						if (it != sBlueValuePairs.end() && (*it).m_lCount>0)
							(const_cast<CValuePair&>(*it)).m_lCount--;
							//(*it).m_lCount--;
						break;
					};
				}
			};
		};

		// Remove anything that is not between [mean-sigma, mean+sigma]
		fRedFactor		= ComputeMinimumEntropyFactor(sRedValuePairs);
		//fRedFactor		= ComputeMinimumRMSFactor(sRedValuePairs);

		fGreenFactor	= ComputeMinimumEntropyFactor(sGreenValuePairs);
		//fGreenFactor	= ComputeMinimumRMSFactor(sGreenValuePairs);

		fBlueFactor		= ComputeMinimumEntropyFactor(sBlueValuePairs);
		//fBlueFactor		= ComputeMinimumRMSFactor(sBlueValuePairs);

		ZTRACE_RUNTIME("RGB coefficients: Red = %.2f - Green = %.2f - Blue = %.2f", fRedFactor, fGreenFactor, fBlueFactor);
	}
	else
	{
		VALUEPAIRSET		sRedValuePairs;
		VALUEPAIRSET		sGreenValuePairs;
		VALUEPAIRSET		sBlueValuePairs;

		for (i = SubSquare.m_rcSquare.left;i<=SubSquare.m_rcSquare.right;i++)
		{
			for (j = SubSquare.m_rcSquare.top;j<SubSquare.m_rcSquare.bottom;j++)
			{
				VALUEPAIRITERATOR		it;
				double					fRedLight, fGreenLight, fBlueLight;
				double					fRedDark, fGreenDark, fBlueDark;

				pBitmap->GetPixel(i, j, fRedLight, fGreenLight, fBlueLight);
				m_pMasterDark->GetPixel(i, j, fRedDark, fGreenDark, fBlueDark);

				it = sRedValuePairs.find(CValuePair(fRedLight*256.0, fRedDark * 256.0));
				if (it != sRedValuePairs.end())
					(const_cast<CValuePair&>(*it)).m_lCount++;
					//(*it).m_lCount++;
				else
					sRedValuePairs.insert(CValuePair(fRedLight*256.0, fRedDark * 256.0));

				it = sGreenValuePairs.find(CValuePair(fGreenLight*256.0, fGreenDark * 256.0));
				if (it != sGreenValuePairs.end())
					(const_cast<CValuePair&>(*it)).m_lCount++;
					//(*it).m_lCount++;
				else
					sGreenValuePairs.insert(CValuePair(fGreenLight*256.0, fGreenDark * 256.0));

				it = sBlueValuePairs.find(CValuePair(fBlueLight*256.0, fBlueDark * 256.0));
				if (it != sBlueValuePairs.end())
					(const_cast<CValuePair&>(*it)).m_lCount++;
					//(*it).m_lCount++;
				else
					sBlueValuePairs.insert(CValuePair(fBlueLight*256.0, fBlueDark * 256.0));
			};
		};


		// Remove Hot pixels
		for (i = 0;i<vExcludedPixels.size();i++)
		{
			VALUEPAIRITERATOR		it;
			double					fRedLight, fGreenLight, fBlueLight;
			double					fRedDark, fGreenDark, fBlueDark;

			if (SubSquare.m_rcSquare.PtInRect(CPoint(vExcludedPixels[i].X, vExcludedPixels[i].Y)))
			{
				pBitmap->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fRedLight, fGreenLight, fBlueLight);
				m_pMasterDark->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fRedDark, fGreenDark, fBlueDark);

				it = sRedValuePairs.find(CValuePair(fRedLight*256.0, fRedDark * 256.0));
				if (it != sRedValuePairs.end() && (*it).m_lCount>0)
					(const_cast<CValuePair&>(*it)).m_lCount--;
					//(*it).m_lCount--;

				it = sGreenValuePairs.find(CValuePair(fGreenLight*256.0, fGreenDark * 256.0));
				if (it != sGreenValuePairs.end() && (*it).m_lCount>0)
					(const_cast<CValuePair&>(*it)).m_lCount--;
					//(*it).m_lCount--;

				it = sBlueValuePairs.find(CValuePair(fBlueLight*256.0, fBlueDark * 256.0));
				if (it != sBlueValuePairs.end() && (*it).m_lCount>0)
					(const_cast<CValuePair&>(*it)).m_lCount--;
					//(*it).m_lCount--;
			};
		};

		fRedFactor		= ComputeMinimumEntropyFactor(sRedValuePairs);
		fGreenFactor	= ComputeMinimumEntropyFactor(sGreenValuePairs);
		fBlueFactor		= ComputeMinimumEntropyFactor(sBlueValuePairs);

		ZTRACE_RUNTIME("RGB coefficients: Red = %.2f - Green = %.2f - Blue = %.2f", fRedFactor, fGreenFactor, fBlueFactor);
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

typedef enum tagIMAGEREGION
{
	IR_NONE				= 0x0000,
	IR_TOPLEFT			= 0x0001,
	IR_TOPCENTER		= 0x0002,
	IR_TOPRIGHT			= 0x0004,
	IR_CENTERLEFT		= 0x0008,
	IR_CENTERCENTER		= 0x0010,
	IR_CENTERRIGHT		= 0x0020,
	IR_BOTTOMLEFT		= 0x0040,
	IR_BOTTOMCENTER		= 0x0080,
	IR_BOTTOMRIGHT		= 0x0100
}IMAGEREGION;

class CHotCheckPixel
{
public :
	double					m_fHot;
	double					m_fMedian;
	IMAGEREGION				m_Region;

private :
	void	CopyFrom(const CHotCheckPixel & hcp)
	{
		m_fHot = hcp.m_fHot;
		m_fMedian = hcp.m_fMedian;
		m_Region  = hcp.m_Region;
	};

public :
	CHotCheckPixel(double fHot, double fMedian, IMAGEREGION Region)
	{
		m_fHot		= fHot;
		m_fMedian	= fMedian;
		m_Region	= Region;
	};
	~CHotCheckPixel() {};

	CHotCheckPixel(const CHotCheckPixel & hcp)
	{
		CopyFrom(hcp);
	};

	CHotCheckPixel & operator = (const CHotCheckPixel & hcp)
	{
		CopyFrom(hcp);

		return (*this);
	};

	bool operator < (const CHotCheckPixel & hcp) const
	{
		return hcp.m_fHot<m_fHot;
	};
};

/* ------------------------------------------------------------------- */

IMAGEREGION	GetPixelRegion(LONG lX, LONG lY, LONG lWidth, LONG lHeight)
{
	IMAGEREGION				Result = IR_NONE;
	LONG					lPosition;

	lPosition = (10*max(1L, min(3L, lX*3/lWidth+1))+max(1L, min(3L, lY*3/lHeight+1)));
	switch (lPosition)
	{
	case 11 :
		Result = IR_TOPLEFT;
		break;
	case 12 :
		Result = IR_TOPCENTER;
		break;
	case 13 :
		Result = IR_TOPRIGHT;
		break;
	case 21 :
		Result = IR_CENTERLEFT;
		break;
	case 22 :
		Result = IR_CENTERCENTER;
		break;
	case 23 :
		Result = IR_CENTERRIGHT;
		break;
	case 31 :
		Result = IR_BOTTOMLEFT;
		break;
	case 32 :
		Result = IR_BOTTOMCENTER;
		break;
	case 33 :
		Result = IR_BOTTOMRIGHT;
		break;
	};

	return Result;
};

/* ------------------------------------------------------------------- */

void	CDarkFrameHotParameters::ComputeParameters(CMemoryBitmap * pBitmap, HOTPIXELVECTOR & vHotPixels)
{
	ZFUNCTRACE_RUNTIME();
	CMedianImageFilter		Filter;
	LONG					lWidth = pBitmap->RealWidth(),
							lHeight = pBitmap->RealHeight();

	std::vector<CHotCheckPixel>	vHots;

	Filter.SetBitmap(pBitmap);
	Filter.SetFilterSize(2);

	for (LONG i = 0;i<vHotPixels.size();i++)
	{
		// Compute the median around each hot pixel
		LONG			X = vHotPixels[i].m_lX,
						Y = vHotPixels[i].m_lY;
		IMAGEREGION		Region = GetPixelRegion(X, Y, lWidth, lHeight);

		if (Filter.IsMonochrome())
		{
			if (Filter.IsCFA())
			{
				BAYERCOLOR			BayerColor = pBitmap->GetBayerColor(X, Y);
				double				fMedian,
									fHot;

				Filter.ComputeMedianAt(X, Y, fMedian, BayerColor);
				pBitmap->GetPixel(X, Y, fHot);
				vHots.emplace_back(fHot, fMedian, Region);
			}
			else
			{
				double				fMedian,
									fHot;

				Filter.ComputeMedianAt(X, Y, fMedian);
				pBitmap->GetPixel(X, Y, fHot);

				vHots.emplace_back(fHot, fMedian, Region);
			};
		}
		else
		{
			double			fMedianRed, fMedianGreen, fMedianBlue;
			double			fHotRed, fHotGreen, fHotBlue;

			Filter.ComputeMedianAt(X, Y, fMedianRed, fMedianGreen, fMedianBlue);
			pBitmap->GetPixel(X, Y, fHotRed, fHotGreen, fHotBlue);

			vHots.emplace_back((fHotRed+fHotGreen+fHotBlue)/3.0, (fMedianRed+fMedianGreen+fMedianBlue)/3.0, Region);
		};
	};

	LONG					lNrCovered = 0;
	double					fSumHot    = 0,
							fSumMedian = 0;
	DWORD					dwCovered1 = 0,
							dwCovered2 = 0;

	std::sort(vHots.begin(), vHots.end());
	for (LONG i = 0;i<vHots.size() && (lNrCovered!=18);i++)
	{
		if (!(dwCovered1 & vHots[i].m_Region) ||
			!(dwCovered2 & vHots[i].m_Region))
		{
			fSumHot    += vHots[i].m_fHot;
			fSumMedian += vHots[i].m_fMedian;
			lNrCovered++;
			if (!(dwCovered1 & vHots[i].m_Region))
				dwCovered1 |= vHots[i].m_Region;
			else
				dwCovered2 |= vHots[i].m_Region;
		};
	};

	if (lNrCovered)
	{
		fSumHot /= lNrCovered;
		fSumMedian /= lNrCovered;
	};

	m_fGrayValue = fSumHot - fSumMedian;
};

/* ------------------------------------------------------------------- */

void	CDarkAmpGlowParameters::ComputeParametersFromPoints(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	CRect			rc;
	LONG			lWidth	= pBitmap->RealWidth(),
					lHeight = pBitmap->RealHeight();
	double			m_fMedianColdest = -1;

	m_fMedianHotest = ComputeMedianValueInRect(pBitmap, m_rcHotest);
	m_vMedianColdest.clear();

	for (LONG k = 0;k<m_vrcColdest.size();k++)
	{
		double		fValue;
		fValue = ComputeMedianValueInRect(pBitmap, m_vrcColdest[k]);
		m_vMedianColdest.push_back(fValue);
		if ((m_fMedianColdest<0) || (m_fMedianColdest>fValue))
		{
			m_fMedianColdest = fValue;
			m_lColdestIndice = k;
		};
	};

	m_fGrayValue = m_fMedianHotest-m_fMedianColdest;
};

/* ------------------------------------------------------------------- */

double	CDarkAmpGlowParameters::ComputeMedianValueInRect(CMemoryBitmap * pBitmap, CRect & rc)
{
	ZFUNCTRACE_RUNTIME();
	double				fResult = 0;
	CRGBHistogram		RGBHistogram;
	BOOL				bMonochrome = pBitmap->IsMonochrome();
	BOOL				bCFA = pBitmap->IsCFA();

	RGBHistogram.SetSize(256.0, (LONG)65536);
	for (LONG i = rc.left;i<=rc.right;i++)
	{
		for (LONG j = rc.top;j<=rc.bottom;j++)
		{
			if (bCFA)
			{
				double				fGray;

				pBitmap->GetPixel(i, j, fGray);

				switch (pBitmap->GetBayerColor(i, j))
				{
				case BAYER_RED :
					RGBHistogram.GetRedHistogram().AddValue(fGray);
					break;
				case BAYER_GREEN :
					RGBHistogram.GetGreenHistogram().AddValue(fGray);
					break;
				case BAYER_BLUE :
					RGBHistogram.GetBlueHistogram().AddValue(fGray);
					break;
				};
			}
			else if (bMonochrome)
			{
				double				fGray;

				pBitmap->GetPixel(i, j, fGray);
				RGBHistogram.GetRedHistogram().AddValue(fGray);
			}
			else
			{
				double				fRed, fGreen, fBlue;

				pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
				RGBHistogram.AddValues(fRed, fGreen, fBlue);;
			};
		};
	};

	if (bMonochrome && !bCFA)
		fResult = RGBHistogram.GetRedHistogram().GetMedian();
	else
	{
		fResult = (RGBHistogram.GetRedHistogram().GetMedian()+
				   RGBHistogram.GetGreenHistogram().GetMedian()+
				   RGBHistogram.GetBlueHistogram().GetMedian())/3;
	};

	return fResult;
};

/* ------------------------------------------------------------------- */

void	CDarkAmpGlowParameters::FindPointsAndComputeParameters(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bMonochrome = pBitmap->IsMonochrome();
	BOOL				bCFA = pBitmap->IsCFA();
	LONG				lWidth = pBitmap->RealWidth(),
						lHeight = pBitmap->RealHeight();

	std::vector<LONG>	vColumns;

	vColumns.reserve(6);

	vColumns.push_back(1);
	vColumns.push_back(2);
	vColumns.push_back(3);

	vColumns.push_back(lWidth-2);
	vColumns.push_back(lWidth-3);
	vColumns.push_back(lWidth-4);

	double				fMaxGray  = 0,
						fMaxRed   = 0,
						fMaxGreen = 0,
						fMaxBlue  = 0;

	CHotPixel			pxMaxGray,
						pxMaxRed,
						pxMaxGreen,
						pxMaxBlue;

	// Inspect the 3 columns on each side to find the brighest pixel (in each color)
	for (LONG j = 1;j<lHeight-1;j++)
	{
		for (LONG i = 0;i<vColumns.size();i++)
		{
			if (bMonochrome)
			{
				double			fGray;

				pBitmap->GetPixel(vColumns[i], j, fGray);
				if (bCFA)
				{
					switch (pBitmap->GetBayerColor(vColumns[i], j))
					{
					case BAYER_RED :
						if (fGray>fMaxRed)
						{
							fMaxRed = fGray;
							pxMaxRed.SetPixel(vColumns[i], j);
						};
						break;
					case BAYER_GREEN :
						if (fGray>fMaxGreen)
						{
							fMaxGreen = fGray;
							pxMaxGreen.SetPixel(vColumns[i], j);
						};
						break;
					case BAYER_BLUE :
						if (fGray>fMaxBlue)
						{
							fMaxBlue = fGray;
							pxMaxBlue.SetPixel(vColumns[i], j);
						};
						break;
					};
				}
				else
				{
					if (fGray>fMaxGray)
					{
						fMaxGray = fGray;
						pxMaxGray.SetPixel(vColumns[i], j);
					};
				};
			}
			else
			{
				double			fRed, fGreen, fBlue;

				pBitmap->GetPixel(vColumns[i], j, fRed, fGreen, fBlue);

				if (fRed>fMaxRed)
				{
					fMaxRed = fRed;
					pxMaxRed.SetPixel(vColumns[i], j);
				};
				if (fGreen>fMaxGreen)
				{
					fMaxGreen = fGreen;
					pxMaxGreen.SetPixel(vColumns[i], j);
				};
				if (fBlue>fMaxBlue)
				{
					fMaxBlue = fBlue;
					pxMaxBlue.SetPixel(vColumns[i], j);
				};
			};
		};
	};

	// Now compute the median value around each pixel
	if (bMonochrome && !bCFA)
	{
		GetRectAroundPoint(lWidth, lHeight, 20, pxMaxGray, m_rcHotest);
		m_fMedianHotest = ComputeMedianValueInRect(pBitmap, m_rcHotest);
	}
	else
	{
		// Check all the points
		double			fMaxRed,
						fMaxGreen,
						fMaxBlue;
		CRect			rcMaxRed,
						rcMaxGreen,
						rcMaxBlue;

		GetRectAroundPoint(lWidth, lHeight, 20, pxMaxRed, rcMaxRed);
		fMaxRed = ComputeMedianValueInRect(pBitmap, rcMaxRed);
		GetRectAroundPoint(lWidth, lHeight, 20, pxMaxGreen, rcMaxGreen);
		fMaxGreen = ComputeMedianValueInRect(pBitmap, rcMaxGreen);
		GetRectAroundPoint(lWidth, lHeight, 20, pxMaxBlue, rcMaxBlue);
		fMaxBlue = ComputeMedianValueInRect(pBitmap, rcMaxBlue);

		double			fMax;

		fMax = max(fMaxRed, max(fMaxGreen, fMaxBlue));
		if (fMax == fMaxRed)
		{
			m_rcHotest = rcMaxRed;
			m_fMedianHotest = fMaxRed;
		}
		else if (fMax == fMaxBlue)
		{
			m_rcHotest = rcMaxBlue;
			m_fMedianHotest = fMaxBlue;
		}
		else
		{
			m_rcHotest = rcMaxGreen;
			m_fMedianHotest = fMaxGreen;
		};
	};

	// Now find the coldest rectangle
	std::vector<CRect>		vRects;
	double					m_fMedianColdest = -1;

	GetBorderRects(lWidth, lHeight, vRects);

	m_vrcColdest = vRects;
	m_vMedianColdest.clear();

	for (LONG k = 0;k<vRects.size();k++)
	{
		double			fValue;

		fValue = ComputeMedianValueInRect(pBitmap, vRects[k]);
		m_vMedianColdest.push_back(fValue);
		if ((m_fMedianColdest<0) || (m_fMedianColdest>fValue))
		{
			m_fMedianColdest = fValue;
			m_lColdestIndice = k;
		};
	};

	m_fGrayValue = m_fMedianHotest-m_fMedianColdest;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	CDarkFrame::ComputeDarkFactorFromMedian(CMemoryBitmap * pBitmap, double & fHotDark, double & fAmpGlow, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	CString					strText;

	if (!m_pAmpGlow)
	{
		CMedianImageFilter		Filter;

		// First compute the median from the master dark
		// This is the ampglow...
		Filter.SetFilterSize(2);

		if (pProgress)
		{
			strText.LoadString(IDS_CREATINGMEDIANIMAGE);
			pProgress->Start2(strText, 0);
		};

		Filter.ApplyFilter(m_pMasterDark, &m_pAmpGlow, pProgress);
		//WriteTIFF("E:\\AmpGlow.tif", m_pAmpGlow, pProgress, nullptr);

		m_pDarkCurrent.Attach(m_pMasterDark->Clone());
		// Then subtract this median from the master dark
		// This is the dark signal

		::Subtract(m_pDarkCurrent, m_pAmpGlow, pProgress);

		//WriteTIFF("E:\\DarkCurrent.tif", m_pDarkCurrent, pProgress, nullptr);

		// Compute the parameters from the ampglow and the dark current

		// First the dark current
		m_HotParameters.ComputeParameters(m_pMasterDark/*m_pDarkCurrent*/, m_vHotPixels);

		// Then the Ampglow
		m_AmpglowParameters.FindPointsAndComputeParameters(m_pAmpGlow);
		//m_AmpglowParameters.ComputeParametersFromPoints(m_pMasterDark);//.FindPointsAndComputeParameters(m_pAmpGlow);
	};

	if (m_pAmpGlow && m_pDarkCurrent)
	{
		// now this is the fun part
		// Compute ampglow matching and dark matching

		// First dark hot pixels
		CDarkFrameHotParameters	LightHotParameters;

		LightHotParameters.ComputeParameters(pBitmap, m_vHotPixels);

		// Then compare to compute the ratio
		if (m_HotParameters.m_fGrayValue)
			fHotDark = LightHotParameters.m_fGrayValue/m_HotParameters.m_fGrayValue;
		else
			fHotDark = 1.0;

		// Then Ampglow
		CDarkAmpGlowParameters	LightAmpGlowParameters(m_AmpglowParameters);


		//CMedianImageFilter		Filter;

		// First compute the median from the master dark
		// This is the ampglow...
		/*CSmartPtr<CMemoryBitmap>	pBitmapGlow;
		Filter.SetFilterSize(2);
		Filter.ApplyFilter(pBitmap, &pBitmapGlow, pProgress);*/

		LightAmpGlowParameters.ComputeParametersFromPoints(pBitmap);
		m_AmpglowParameters.ComputeParametersFromIndice(LightAmpGlowParameters.m_lColdestIndice);

		if (LightAmpGlowParameters.m_fGrayValue>0 &&
			m_AmpglowParameters.m_fGrayValue>0)
			fAmpGlow = min(LightAmpGlowParameters.m_fGrayValue/m_AmpglowParameters.m_fGrayValue, 1.0);
		else
			fAmpGlow = 1.0;

		//fAmpGlow = min(fAmpGlow, fHotDark);
	};
};

/* ------------------------------------------------------------------- */

void	CDarkFrame::ComputeDarkFactorFromHotPixels(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor)
{
	ZFUNCTRACE_RUNTIME();

	HOTPIXELVECTOR				vHotPixels;
	LONG						i;

	fRedFactor	 = 1.0;
	fGreenFactor = 1.0;
	fBlueFactor	 = 1.0;

	if (m_vHotPixels.size())
	{
		// Remove hot pixels that are in stars
		if (pStars)
		{
			EXCLUDEDPIXELSET			sStarPixels;

			for (i = 0;i<pStars->size();i++)
			{
				CRect &			rcStar = (*pStars)[i].m_rcStar;

				for (LONG x = rcStar.left;x<=rcStar.right;x++)
					for (LONG y = rcStar.top;y<=rcStar.bottom;y++)
					{
						CExcludedPixel	ep(x, y);

						if (sStarPixels.find(ep) == sStarPixels.end())
							sStarPixels.insert(ep);
					};
			};

			for (i = 0;i<m_vHotPixels.size();i++)
			{
				CExcludedPixel		ep(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY);

				if (sStarPixels.find(ep) == sStarPixels.end())
					vHotPixels.push_back(m_vHotPixels[i]);
			};
		}
		else
			vHotPixels = m_vHotPixels;

		// For each hot pixel compare the value in the light and in the dark
		if (pBitmap->IsMonochrome() && !pBitmap->IsCFA())
		{
			std::vector<double>		vRatios;

			for (i = 0;i<vHotPixels.size();i++)
			{
				double				fLight,
									fDark;

				pBitmap->GetPixel(vHotPixels[i].m_lX, vHotPixels[i].m_lY, fLight);
				m_pMasterDark->GetPixel(vHotPixels[i].m_lX, vHotPixels[i].m_lY, fDark);

				if (fLight > 0 && fDark > 0)
				{
					double			fRatio;

					fRatio = fLight/fDark;
					vRatios.push_back(fRatio);
				};
			};
			std::sort(vRatios.begin(), vRatios.end());
			std::vector<double> vWork;
			vWork.reserve(vRatios.size());

			fRedFactor = KappaSigmaClip(vRatios, 2.0, 5, vWork);
			fGreenFactor = fBlueFactor = fRedFactor;
		}
		else if (pBitmap->IsMonochrome() && pBitmap->IsCFA())
		{
			std::vector<double>		vRedRatios;
			std::vector<double>		vGreenRatios;
			std::vector<double>		vBlueRatios;
			std::vector<double>		vWork;

			vWork.reserve(vHotPixels.size());

			for (i = 0;i<vHotPixels.size();i++)
			{
				double				fLight,
									fDark;

				pBitmap->GetPixel(vHotPixels[i].m_lX, vHotPixels[i].m_lY, fLight);
				m_pMasterDark->GetPixel(vHotPixels[i].m_lX, vHotPixels[i].m_lY, fDark);

				if (fLight > 0 && fDark > 0)
				{
					double			fRatio;

					fRatio = fLight/fDark;
					switch (pBitmap->GetBayerColor(vHotPixels[i].m_lX, vHotPixels[i].m_lY))
					{
					case BAYER_RED :
						vRedRatios.push_back(fRatio);
						break;
					case BAYER_GREEN :
						vGreenRatios.push_back(fRatio);
						break;
					case BAYER_BLUE :
						vBlueRatios.push_back(fRatio);
						break;
					};
				};
			};
			std::sort(vRedRatios.begin(), vRedRatios.end());
			std::sort(vGreenRatios.begin(), vGreenRatios.end());
			std::sort(vBlueRatios.begin(), vBlueRatios.end());

			fRedFactor = KappaSigmaClip(vRedRatios, 2.0, 5, vWork);
			fGreenFactor = KappaSigmaClip(vGreenRatios, 2.0, 5, vWork);
			fBlueFactor = KappaSigmaClip(vBlueRatios, 2.0, 5, vWork);
		}
		else
		{
		};
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CDarkFrame::FindBadVerticalLines(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bMonochrome = m_pMasterDark->IsMonochrome();
	LONG				i, j;

	if (bMonochrome)
	{
		// Create an image holding the vertical averaged pixels
		LONG						lWidth = m_pMasterDark->RealWidth(),
									lHeight = m_pMasterDark->RealHeight();

		for (i = 1;i<lWidth-1;i++)
		{
			BOOL					bLineInProgress = FALSE;
			BOOL					bLighterLine = FALSE;
			LONG					lStartY = 0;
			LONG					lNrPixels = 0;
			LONG					lNrOutPixels = 0;
			LONG					lNrConsecutiveOutPixels = 0;

			for (j = 0;j<lHeight;j++)
			{
				double				fLeftValue,
									fValue,
									fRightValue;

				BOOL				bLighter,
									bDarker;

				m_pMasterDark->GetPixel(i-1, j, fLeftValue);
				m_pMasterDark->GetPixel(i, j, fValue);
				m_pMasterDark->GetPixel(i+1, j, fRightValue);

				bLighter = (fValue > fLeftValue) && (fValue > fRightValue);
				bDarker  = (fValue < fLeftValue) && (fValue < fRightValue);

				if (bLineInProgress)
				{
					if ((bLighterLine && bLighter) ||(!bLighterLine && bDarker))
					{
						// Add a pixel
						lNrPixels++;
						lNrConsecutiveOutPixels = 0;
					}
					else
					{
						// This is a bad pixel
						lNrOutPixels++;
						lNrConsecutiveOutPixels++;
						// End the line ?
						if ((double)lNrOutPixels/(double)lNrPixels > 0.10)
						{
							// End the line - save it
							bLineInProgress = FALSE;
							if (lNrPixels > 10)
							{
								for (LONG k = lStartY;k<lStartY+lNrPixels-lNrConsecutiveOutPixels;k++)
								{
									CHotPixel		hp(i, k);
									m_vHotPixels.push_back(hp);
								};
							};
						};
					};
				}
				else if (bLighter || bDarker)
				{
					lStartY = j;
					lNrPixels = 1;
					lNrOutPixels = 0;
					lNrConsecutiveOutPixels = 0;
					bLineInProgress = TRUE;
					bLighterLine = bLighter;
				};
			};

			if (bLineInProgress)
			{
				if (lNrPixels > 10)
				{
					for (LONG k = lStartY;k<lStartY+lNrPixels-lNrConsecutiveOutPixels;k++)
					{
						CHotPixel		hp(i, k);
						m_vHotPixels.push_back(hp);
					};
				};
			};
		};
		m_bHotPixelDetected = TRUE;
	};
};

/* ------------------------------------------------------------------- */

class CFindHotPixelTask1 : public CMultitask
{
public :
	CRGBHistogram				m_RGBHistogram;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	BOOL						m_bMonochrome;
	CDSSProgress *				m_pProgress;

public :
	CFindHotPixelTask1()
    {
        m_bMonochrome = false;
        m_pProgress = nullptr;
    }
	virtual ~CFindHotPixelTask1() {};

	void	Init(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
	{
		m_pBitmap				 = pBitmap;
		m_pProgress				 = pProgress;
		m_bMonochrome			 = pBitmap->IsMonochrome();

		m_RGBHistogram.SetSize(256.0, (LONG)65535);
	};

	virtual BOOL	DoTask(HANDLE hEvent)
	{
		ZFUNCTRACE_RUNTIME();
		BOOL				bResult = TRUE;

		LONG				i, j;
		BOOL				bEnd = FALSE;
		MSG					msg;
		LONG				lWidth = m_pBitmap->RealWidth();

		CRGBHistogram		RGBHistogram;
		RGBHistogram.SetSize(256.0, (LONG)65535);

		PixelIterator		PixelIt;

		m_pBitmap->GetIterator(&PixelIt);

		// Create a message queue and signal the event
		PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
		SetEvent(hEvent);
		while (!bEnd && GetMessage(&msg, nullptr, 0, 0))
		{
			if (msg.message == WM_MT_PROCESS)
			{
				PixelIt->Reset(0, msg.wParam);
				for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
				{
					for (i = 0;i<lWidth;i++)
					{
						double			fRed, fGreen, fBlue, fGray;

						if (m_bMonochrome)
						{
							PixelIt->GetPixel(fGray);
							//m_pBitmap->GetPixel(i, j, fGray);
							RGBHistogram.AddValues(fGray, fGray, fGray);
						}
						else
						{
							PixelIt->GetPixel(fRed, fGreen, fBlue);
							//m_pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
							RGBHistogram.AddValues(fRed, fGreen, fBlue);
						};
						(*PixelIt)++;
					};
				};

				SetEvent(hEvent);
			}
			else if (msg.message == WM_MT_STOP)
				bEnd = TRUE;
		};

		m_CriticalSection.Lock();
		m_RGBHistogram.AddValues(RGBHistogram);
		m_CriticalSection.Unlock();
		return TRUE;
	};

	virtual BOOL	Process()
	{
		ZFUNCTRACE_RUNTIME();
		BOOL				bResult = TRUE;
		LONG				lHeight = m_pBitmap->RealHeight();
		LONG				i = 0;
		LONG				lStep;
		LONG				lRemaining;

		if (m_pProgress)
			m_pProgress->SetNrUsedProcessors(GetNrThreads());
		lStep		= max(1L, lHeight/50);
		lRemaining	= lHeight;

		while (i<lHeight)
		{
			LONG			lAdd = min(lStep, lRemaining);
			DWORD			dwThreadId;

			dwThreadId = GetAvailableThreadId();
			PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

			i			+=lAdd;
			lRemaining	-= lAdd;
			if (m_pProgress)
				m_pProgress->Progress2(nullptr, i);
		};

		CloseAllThreads();

		if (m_pProgress)
			m_pProgress->SetNrUsedProcessors();

		return bResult;
	};
};

/* ------------------------------------------------------------------- */

void	CDarkFrame::RemoveContiguousHotPixels(BOOL bCFA)
{
	ZFUNCTRACE_RUNTIME();
	HOTPIXELVECTOR			vNewHotPixels;
	LONG					lStep = bCFA ? 2 : 1;
	LONG					lNrDiscarded = 0;

	for (LONG i = 0;i<m_vHotPixels.size();i++)
	{
		CHotPixel &			hp = m_vHotPixels[i];
		LONG				lNrHotNeighbors = 0;

		if (std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), CHotPixel(hp.m_lX-lStep, hp.m_lY-lStep)))
			lNrHotNeighbors++;
		if (std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), CHotPixel(hp.m_lX-lStep, hp.m_lY)))
			lNrHotNeighbors++;
		if (std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), CHotPixel(hp.m_lX-lStep, hp.m_lY+lStep)))
			lNrHotNeighbors++;
		if (std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), CHotPixel(hp.m_lX, hp.m_lY-lStep)))
			lNrHotNeighbors++;
		if (std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), CHotPixel(hp.m_lX, hp.m_lY+lStep)))
			lNrHotNeighbors++;
		if (std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), CHotPixel(hp.m_lX+lStep, hp.m_lY-lStep)))
			lNrHotNeighbors++;
		if (std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), CHotPixel(hp.m_lX+lStep, hp.m_lY)))
			lNrHotNeighbors++;
		if (std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), CHotPixel(hp.m_lX+lStep, hp.m_lY-lStep)))
			lNrHotNeighbors++;

		if (lNrHotNeighbors<3)
			vNewHotPixels.push_back(hp);
		else
			lNrDiscarded++;
	};

	if (lNrDiscarded)
		m_vHotPixels = vNewHotPixels;
};

/* ------------------------------------------------------------------- */

void	CDarkFrame::FindHotPixels(CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	m_vHotPixels.clear();
	if (m_pMasterDark)
	{
		CRGBHistogram		RGBHistogram;
		LONG				i, j;
		BOOL				bMonochrome = m_pMasterDark->IsMonochrome();

		if (pProgress)
		{
			CString		strText;
			strText.LoadString(IDS_DETECTINGHOTPIXELS1);

			pProgress->Start2(strText, m_pMasterDark->RealHeight());
		};

		CFindHotPixelTask1	HotPixelTask1;

		HotPixelTask1.Init(m_pMasterDark, pProgress);
		HotPixelTask1.StartThreads();
		HotPixelTask1.Process();

		/*
		RGBHistogram.SetSize(256.0, (LONG)65535);
		for (j = 0;j<m_pMasterDark->RealHeight();j++)
		{
			for (i = 0;i<m_pMasterDark->Width();i++)
			{
				double			fRed, fGreen, fBlue, fGray;

				if (bMonochrome)
				{
					m_pMasterDark->GetPixel(i, j, fGray);
					RGBHistogram.AddValues(fGray, fGray, fGray);
				}
				else
				{
					m_pMasterDark->GetPixel(i, j, fRed, fGreen, fBlue);
					RGBHistogram.AddValues(fRed, fGreen, fBlue);
				};
			};
			if (pProgress)
				pProgress->Progress2(nullptr, j+1);
		};*/

		if (pProgress)
		{
			pProgress->End2();
			CString		strText;
			strText.LoadString(IDS_DETECTINGHOTPIXELS2);

			pProgress->Start2(strText, m_pMasterDark->RealHeight());
		};

		double				fRedThreshold,
							fGreenThreshold,
							fBlueThreshold;

		fRedThreshold = HotPixelTask1.m_RGBHistogram.GetRedHistogram().GetMedian()+16.0 * HotPixelTask1.m_RGBHistogram.GetRedHistogram().GetStdDeviation();
		fGreenThreshold = HotPixelTask1.m_RGBHistogram.GetGreenHistogram().GetMedian()+16.0 * HotPixelTask1.m_RGBHistogram.GetGreenHistogram().GetStdDeviation();
		fBlueThreshold = HotPixelTask1.m_RGBHistogram.GetBlueHistogram().GetMedian()+16.0 * HotPixelTask1.m_RGBHistogram.GetBlueHistogram().GetStdDeviation();

		LONG				lWidth  = m_pMasterDark->RealWidth();
		LONG				lHeight = m_pMasterDark->RealHeight();
		PixelIterator		PixelIt;

		m_pMasterDark->GetIterator(&PixelIt);
		for (j = 0;j<lHeight;j++)
		{
			for (i = 0;i<lWidth;i++)
			{
				double			fGray,
								fRed,
								fGreen,
								fBlue;
				BOOL			bHot = FALSE;

				if (bMonochrome)
				{
					PixelIt->GetPixel(fGray);
					//m_pMasterDark->GetPixel(i, j, fGray);
					bHot = (fGray > fRedThreshold);
				}
				else
				{
					PixelIt->GetPixel(fRed, fGreen, fBlue);
					//m_pMasterDark->GetPixel(i, j, fRed, fGreen, fBlue);
					bHot =	(fRed > fRedThreshold) ||
							(fGreen> fGreenThreshold) ||
							(fBlue > fBlueThreshold);
				};
				if (bHot)
				{
					// This is a hot pixel
					CHotPixel		hp(i, j);

					m_vHotPixels.push_back(hp);
				};

				(*PixelIt)++;
			};
			if (pProgress)
				pProgress->Progress2(nullptr, j+1);
		};

		if (pProgress)
			pProgress->End2();

		std::sort(m_vHotPixels.begin(), m_vHotPixels.end());
		RemoveContiguousHotPixels(m_pMasterDark->IsCFA());
	};

	m_bHotPixelDetected = TRUE;
};

/* ------------------------------------------------------------------- */

void	CDarkFrame::GetValidNeighbors(LONG lX, LONG lY, HOTPIXELVECTOR & vPixels, LONG lRadius, BAYERCOLOR BayerColor)
{
	vPixels.clear();
	for (LONG i = max(0L, lX-lRadius);i<=min(m_pMasterDark->RealWidth()-1,lX+lRadius);i++)
	{
		for (LONG j = max(0L, lY-lRadius);j<=min(m_pMasterDark->RealHeight()-1, lY+lRadius);j++)
		{
			if ((i != lX) || (j != lY))
			{
				LONG				lWeight;
				BOOL				bAdd = FALSE;

				lWeight = labs(1+lRadius-labs(lX - i)) + labs(1+lRadius-labs(lY - j));
				CHotPixel			hp(i, j, lWeight);

				if (!std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), hp))
					bAdd = TRUE;
				if ((BayerColor != BAYER_UNKNOWN) && (m_pMasterDark->GetBayerColor(i, j) != BayerColor))
					bAdd = FALSE;

				if (bAdd)
					vPixels.push_back(hp);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void	CDarkFrame::InterpolateHotPixels(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	if (pBitmap && m_vHotPixels.size())
	{
		LONG			i, j;

		// First set hot pixels to 0
		for (i = 0;i<m_vHotPixels.size();i++)
			pBitmap->SetPixel(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, 0.0);

		// Then Interpolate Hot Pixels
		if (pBitmap->IsMonochrome())
		{
			// Interpolate with neighbor pixels (level 1)
			BOOL				bCFA = pBitmap->IsCFA();


			for (i = 0;i<m_vHotPixels.size();i++)
			{
				HOTPIXELVECTOR		vPixels;
				BAYERCOLOR			BayerColor = BAYER_UNKNOWN;
				double				fValue = 0.0;
				LONG				lTotalWeight = 0;

				if (bCFA)
					BayerColor = pBitmap->GetBayerColor(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY);

				GetValidNeighbors(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, vPixels, bCFA ? 2 : 1, BayerColor);
				for (j = 0;j<vPixels.size();j++)
				{
					double			fGray;

					pBitmap->GetPixel(vPixels[j].m_lX, vPixels[j].m_lY, fGray);
					fValue += fGray * vPixels[j].m_lWeight;
					lTotalWeight += vPixels[j].m_lWeight;
				};
				if (lTotalWeight)
					fValue /= lTotalWeight;
				pBitmap->SetPixel(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, fValue);
			};
		}
		else
		{
			for (i = 0;i<m_vHotPixels.size();i++)
			{
				HOTPIXELVECTOR		vPixels;
				double				fRedValue	= 0.0,
									fGreenValue = 0.0,
									fBlueValue  = 0.0;
				LONG				lTotalWeight = 0;

				GetValidNeighbors(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, vPixels, 1);
				for (j = 0;j<vPixels.size();j++)
				{
					double			fRed, fGreen, fBlue;

					pBitmap->GetPixel(vPixels[j].m_lX, vPixels[j].m_lY, fRed, fGreen, fBlue);
					fRedValue	+= fRed * vPixels[j].m_lWeight;
					fGreenValue += fGreen * vPixels[j].m_lWeight;
					fBlueValue  += fBlue * vPixels[j].m_lWeight;
					lTotalWeight += vPixels[j].m_lWeight;
				};
				if (lTotalWeight)
				{
					fRedValue /= lTotalWeight;
					fGreenValue /= lTotalWeight;
					fBlueValue /= lTotalWeight;
				};
				pBitmap->SetPixel(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, fRedValue, fGreenValue, fBlueValue);
			};
		};
	};
}

/* ------------------------------------------------------------------- */

BOOL	CDarkFrame::Subtract(CMemoryBitmap * pTarget, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	if (m_pMasterDark && m_pMasterDark->IsOk())
	{
		CString			strText;

		double			fRedDarkFactor = 1.0,
						fGreenDarkFactor = 1.0,
						fBlueDarkFactor = 1.0;

		if ((m_bHotPixelsDetection || m_bBadLinesDetection) && !m_bHotPixelDetected)
		{
			if (m_bHotPixelsDetection)
				FindHotPixels(pProgress);
			if (m_bBadLinesDetection)
				FindBadVerticalLines(pProgress);
		};

		if (m_bDarkOptimization)
		{
			double				fHotDark = 1.0,
								fAmpGlow = 1.0;

			//ComputeDistribution(pTarget, pStars);
			//ComputeDarkFactorFromHotPixels(pTarget, pStars, );
			//ComputeDarkFactor(pTarget, pStars, fRedDarkFactor, fGreenDarkFactor, fBlueDarkFactor, pProgress);
			if (pProgress)
			{
				strText.LoadString(IDS_OPTIMIZINGDARKMATCHING);
				pProgress->Start2(strText, 0);
			};
			ComputeDarkFactorFromMedian(pTarget, fHotDark, fAmpGlow, pProgress);
			/*CSmartPtr<CMemoryBitmap>		pAmpGlow;
			CSmartPtr<CMemoryBitmap>		pDarkCurrent;

			pAmpGlow.Attach(m_pAmpGlow->Clone());
			Multiply(pAmpGlow, fAmpGlow, fAmpGlow, fAmpGlow, pProgress);*/

			if (pProgress)
			{
				strText.LoadString(IDS_SUBSTRACTINGDARK);
				pProgress->Start2(strText, 0);
			};
			::Subtract(pTarget, m_pAmpGlow, pProgress, fAmpGlow, fAmpGlow, fAmpGlow);
			::Subtract(pTarget, m_pDarkCurrent, pProgress, fHotDark, fHotDark, fHotDark);

			// Now check that if modified dark current is removed from
			// the target - there is no more negative pixels
			/*pDarkCurrent.Attach(m_pDarkCurrent->Clone());
			Multiply(pDarkCurrent, fHotDark, fHotDark, fHotDark, pProgress);

			ComputeOptimalDistributionRatio(pTarget, pDarkCurrent, fHotDark, pProgress);
			::Subtract(pTarget, pDarkCurrent, pProgress, fHotDark, fHotDark, fHotDark);*/
		}
		else if (m_fDarkFactor != 1.0)
		{
			if (pProgress)
			{
				strText.LoadString(IDS_SUBSTRACTINGDARK);
				pProgress->Start2(strText, 0);
			};
			::Subtract(pTarget, m_pMasterDark, pProgress, m_fDarkFactor, m_fDarkFactor, m_fDarkFactor);
		}
		else
		{
			if (pProgress)
			{
				strText.LoadString(IDS_SUBSTRACTINGDARK);
				pProgress->Start2(strText, 0);
			};
			::Subtract(pTarget, m_pMasterDark, pProgress);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
