#include "stdafx.h"
#include "DarkFrame.h"
#include "ztrace.h"
#include "DSSProgress.h"
#include "MemoryBitmap.h"
#include "Filters.h"
#include "histogram.h"
#include "BitmapIterator.h"
#include "Multitask.h"
#include "CFABitmapInfo.h"
#include "StackingTasks.h"

using namespace DSS;

/* ------------------------------------------------------------------- */

class CValuePair
{
public:
	std::uint16_t m_wLightValue;
	std::uint16_t m_wDarkValue;
	int	m_lCount;

public:
	explicit CValuePair(const std::uint16_t lightValue = 0, const std::uint16_t darkValue = 0) noexcept :
		m_wLightValue{ lightValue },
		m_wDarkValue{ darkValue },
		m_lCount{ 1 }
	{
	}

	explicit CValuePair(double lightValue = 0, double darkValue = 0) noexcept :
		m_wLightValue{ static_cast<std::uint16_t>(lightValue) },
		m_wDarkValue{ static_cast<std::uint16_t>(darkValue) },
		m_lCount{ 1 }
	{
	}

	CValuePair(const CValuePair&) = default;

	CValuePair& operator =(const CValuePair&) = default;

	bool operator < (const CValuePair& rhs) const
	{
		if (m_wDarkValue == rhs.m_wDarkValue)
			return m_wLightValue > rhs.m_wLightValue;
		return m_wDarkValue > rhs.m_wDarkValue;
		//if (m_wDarkValue > rhs.m_wDarkValue)
		//	return true;
		//else if (m_wDarkValue < rhs.m_wDarkValue)
		//	return false;
		//else
		//	return (m_wLightValue > rhs.m_wLightValue);
	}
};

typedef std::vector<CValuePair>			VALUEPAIRVECTOR;
typedef std::set<CValuePair>			VALUEPAIRSET;
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
		int				lNrValues = 0;
		int				lNrThreshold;
		int				lNrAdded = 0;
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
	double				fMinEntropy = -1.0;
	double				fSelectedk = 0.0;
	constexpr int			MINNEGATIVE = 128;
	std::vector<double>	vValues;

	for (double k = 0.0; k <= 5.0; k += 0.01)
	{
		// Compute cumulated entropy
		std::vector<std::uint16_t> vHisto(MINNEGATIVE * 2 + 1);
		int lNrPixels = 0;

		for (auto it = sValuePairs.cbegin(); it != sValuePairs.cend(); ++it)
		{
			if (it->m_wLightValue + MINNEGATIVE >= static_cast<double>(it->m_wDarkValue) * k)
			{
				int lIndice = static_cast<double>(it->m_wLightValue) - static_cast<double>(it->m_wDarkValue) * k;
				lIndice /= (static_cast<int>(std::numeric_limits<std::uint16_t>::max()) + 1) / MINNEGATIVE;
				lIndice += MINNEGATIVE;

				if (lIndice >= 0)
				{
					vHisto[lIndice] += it->m_lCount;
					lNrPixels += it->m_lCount;
				}
			}
		}
		// Compute Entropy
		double					fEntropy = 0;

		for (int i = 0; i < vHisto.size(); i++)
		{
			if (vHisto[i])
			{
				double			fProbability;

				fProbability = (double)vHisto[i] / (double)lNrPixels;
				fEntropy += -fProbability * log(fProbability) / log(2.0);
			}
		}

		vValues.push_back(fEntropy);

		if ((fEntropy < fMinEntropy) || fSelectedk == 0)
		{
			fMinEntropy = fEntropy;
			fSelectedk = k;
		}
	}

	return fSelectedk;
};

/* ------------------------------------------------------------------- */

inline bool IsInStdDev(double fValue, double fMean, double fStdDev)
{
	ZFUNCTRACE_RUNTIME();
	return (fValue >= fMean-2*fStdDev) && (fValue <= fMean+2*fStdDev);
};
#if (0)
static double ComputeMinimumRMSFactor(VALUEPAIRSET & sValuePairs)
{
	ZFUNCTRACE_RUNTIME();
	double				fMinRMS = -1.0;
	double				fSelectedk	= 0.0;
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
		int					lNrValues = 0;

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
		int					lNrValues = 0;
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
						fMaxValue = std::max(fMaxValue, fValue);
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
	int					lNrValues = 0;

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
#endif
/* ------------------------------------------------------------------- */

class CSubSquare
{
public:
	DSSRect m_rcSquare{};
	double m_fMean{ 0.0 };
	double m_fStdDev{ 0.0 };

	CSubSquare(DSSRect&& r) :
		m_rcSquare{ std::move(r) },
		m_fMean{ 0.0 },
		m_fStdDev{ 0.0 }
	{}
	CSubSquare(const CSubSquare&) = default;
	CSubSquare(CSubSquare&&) = default;
	CSubSquare& operator=(const CSubSquare&) = default;
};

class CSubSquares
{
private:
	std::vector<CSubSquare>	m_vSubSquares{};
	int m_lNrPixels{ 0 };
public:
	explicit CSubSquares(const int lWidth, const int lHeight) :
		m_vSubSquares{},
		m_lNrPixels{ 0 }
	{
		int lSize = std::min(lWidth, lHeight);
		lSize = std::min(100, lSize / 10);

		for (int i = lSize; i < lWidth - 2 * lSize; i += lSize)
			for (int j = lSize; j < lHeight - 2 * lSize; j += lSize)
			{
				m_vSubSquares.emplace_back(CSubSquare{ DSSRect{ i, j, i + lSize, j + lSize } });
				m_lNrPixels += (lSize-1) * (lSize-1);
			}
	}

	const std::vector<CSubSquare>& GetSubSquares() const noexcept
	{
		return m_vSubSquares;
	}
	CSubSquare& GetSubSquare(const size_t k)
	{
		return m_vSubSquares[k];
	}

	int GetNrPixels() const noexcept
	{
		return m_lNrPixels;
	}

};

/* ------------------------------------------------------------------- */

void	CDarkFrame::FillExcludedPixelList(const STARVECTOR * pStars, EXCLUDEDPIXELVECTOR & vExcludedPixels)
{
	ZFUNCTRACE_RUNTIME();
	EXCLUDEDPIXELSET sExcludedPixels;

	vExcludedPixels.clear();
	// First add hot pixels if any
	for (const auto& hotPixel : m_vHotPixels)
	{
		sExcludedPixels.insert(CExcludedPixel{ hotPixel.m_lX, hotPixel.m_lY });
	}

	// Then add stars to the list
	if (pStars != nullptr)
	{
		for (size_t i = 0; i < pStars->size(); i++)
		{
			const DSSRect& rcStar = (*pStars)[i].m_rcStar;

			for (int x = rcStar.left; x < rcStar.right; x++) 
				for (int y = rcStar.top; y < rcStar.bottom; y++)
				{
					CExcludedPixel ep{ x, y };
					if (sExcludedPixels.count(ep) == 0)
						sExcludedPixels.insert(ep);
				}
		}
	}

	EXCLUDEDPIXELITERATOR		it;

	for (it = sExcludedPixels.begin(); it != sExcludedPixels.end(); it++)
		vExcludedPixels.push_back((*it));

	std::sort(vExcludedPixels.begin(), vExcludedPixels.end());
}

/* ------------------------------------------------------------------- */

void	CDarkFrame::ComputeOptimalDistributionRatio(CMemoryBitmap * pBitmap, CMemoryBitmap * pDark, double & fRatio, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	EXCLUDEDPIXELVECTOR		vExcludedPixels;
	int i, j;
	int lHeight = pBitmap->RealHeight();

	VALUEPAIRSET			sRedValuePairs;
	VALUEPAIRSET			sGreenValuePairs;
	VALUEPAIRSET			sBlueValuePairs;

	if (pProgress)
		pProgress->Start2(lHeight);


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
				}
			}
		}

		if (pProgress)
			pProgress->Progress2(j+1);
	}

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
			}
		}
	}

	fRatio = 1.0;

	while (fRatio>0.1)
	{
		VALUEPAIRITERATOR		it;
		CDynamicStats			RedStats,
								GreenStats,
								BlueStats;



		for (it = sRedValuePairs.begin();it != sRedValuePairs.end();it++)
			if ((*it).m_lCount && (*it).m_wLightValue && (*it).m_wDarkValue)
				RedStats.AddValue(std::max(0.0, (double)(*it).m_wLightValue - fRatio * (*it).m_wDarkValue), (*it).m_lCount);

		for (it = sGreenValuePairs.begin();it != sGreenValuePairs.end();it++)
			if ((*it).m_lCount && (*it).m_wLightValue && (*it).m_wDarkValue)
				GreenStats.AddValue(std::max(0.0, (double)(*it).m_wLightValue - fRatio * (*it).m_wDarkValue), (*it).m_lCount);

		for (it = sBlueValuePairs.begin();it != sBlueValuePairs.end();it++)
			if ((*it).m_lCount && (*it).m_wLightValue && (*it).m_wDarkValue)
				BlueStats.AddValue(std::max(0.0, (double)(*it).m_wLightValue - fRatio * (*it).m_wDarkValue), (*it).m_lCount);

		fRatio -= 0.1;
	}

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
				fprintf(hRed, "%d\t%d\t%d\n", (*it).m_wLightValue, (*it).m_wDarkValue, (*it).m_lCount);
		fclose(hRed);

		hGreen = fopen("E:\\GreenDistribution.txt", "wt");
		for (it = sGreenValuePairs.begin();it != sGreenValuePairs.end();it++)
			if ((*it).m_lCount)
				fprintf(hGreen, "%d\t%d\t%d\n", (*it).m_wLightValue, (*it).m_wDarkValue, (*it).m_lCount);
		fclose(hGreen);

		hBlue = fopen("E:\\BlueDistribution.txt", "wt");
		for (it = sBlueValuePairs.begin();it != sBlueValuePairs.end();it++)
			if ((*it).m_lCount)
				fprintf(hBlue, "%d\t%d\t%d\n", (*it).m_wLightValue, (*it).m_wDarkValue, (*it).m_lCount);
		fclose(hBlue);
	}
}


/* ------------------------------------------------------------------- */

void	CDarkFrame::ComputeDarkFactor(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor, ProgressBase * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	EXCLUDEDPIXELVECTOR		vExcludedPixels;

	FillExcludedPixelList(pStars, vExcludedPixels);

	fRedFactor = 1.0;
	fGreenFactor = 1.0;
	fBlueFactor = 1.0;

	ZTRACE_RUNTIME("Compute Dark coefficients");
	if (pProgress)
	{
		const QString strText(QCoreApplication::translate("DarkFrame", "Computing Dark Optimization parameters", "IDS_COMPUTINGDARKOPTIMIZATIONFACTOR"));
		pProgress->Start2(strText, 0);
	}

	int lBestSquare = -1;
	double fMinMean = 0.0;
	int lNrPixels = 0;
	CSubSquares subSquareGrid{ pBitmap->RealWidth(), pBitmap->RealHeight() };

	if (pProgress)
		pProgress->Start2(subSquareGrid.GetNrPixels());

	for (size_t k = 0;k < subSquareGrid.GetSubSquares().size(); k++)
	{
		CSubSquare&	sq = subSquareGrid.GetSubSquare(k);
		double fSum = 0.0;
		double fPowSum = 0.0;
		int lNrValues = 0;

		for (int i = sq.m_rcSquare.left; i < sq.m_rcSquare.right; i++)
			for (int j = sq.m_rcSquare.top; j < sq.m_rcSquare.bottom; j++)
			{
				double		fValue;

				pBitmap->GetPixel(i, j, fValue);
				fPowSum += fValue * fValue;
				fSum    += fValue;
				lNrValues++;
				lNrPixels++;
			}
		for (size_t i = 0; i < vExcludedPixels.size(); i++)
		{
			VALUEPAIRITERATOR		it;
			double					fValue;

			if (sq.m_rcSquare.contains(QPoint{ vExcludedPixels[i].X, vExcludedPixels[i].Y }))
			{
				pBitmap->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fValue);
				fPowSum -= fValue*fValue;
				fSum    -= fValue;
				lNrValues--;
			}
		}
		if (lNrValues)
		{
			sq.m_fMean = fSum / lNrValues;
			sq.m_fStdDev = sqrt(fPowSum / lNrValues - std::pow(fSum/lNrValues, 2));
		}

		if ((sq.m_fStdDev < fMinMean) || lBestSquare < 0)
		{
			lBestSquare = static_cast<int>(k);
			fMinMean    = sq.m_fStdDev;
		}

		if (pProgress)
			pProgress->Progress2(lNrPixels);
	}

	const CSubSquare subSquare = subSquareGrid.GetSubSquare(lBestSquare);

	// From now on we work only with the best square
	if (pBitmap->IsMonochrome() && !pBitmap->IsCFA())
	{
		VALUEPAIRSET		sValuePairs;

		for (int i = subSquare.m_rcSquare.left; i < subSquare.m_rcSquare.right; i++)
		{
			for (int j = subSquare.m_rcSquare.top; j < subSquare.m_rcSquare.bottom; j++)
			{
				double					fLight;
				double					fDark;

				pBitmap->GetPixel(i, j, fLight);
				m_pMasterDark->GetPixel(i, j, fDark);

				const CValuePair valuePair{ fLight * 256.0, fDark * 256.0 };
				VALUEPAIRITERATOR it = sValuePairs.find(valuePair);
				if (it != sValuePairs.end())
					(const_cast<CValuePair&>(*it)).m_lCount++;
				else
					sValuePairs.insert(valuePair);
			}
		}

		// Remove excluded pixels
		for (size_t i = 0; i < vExcludedPixels.size(); i++)
		{
			double					fLight;
			double					fDark;

			if (subSquare.m_rcSquare.contains(QPoint(vExcludedPixels[i].X, vExcludedPixels[i].Y)))
			{
				pBitmap->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fLight);
				m_pMasterDark->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fDark);

				CValuePair vp{ fLight * 256.0, fDark * 256.0 };

				VALUEPAIRITERATOR it = sValuePairs.find(vp);
				if (it != sValuePairs.end() && it->m_lCount>0)
					(const_cast<CValuePair&>(*it)).m_lCount++;
			};
		}

		fRedFactor = fGreenFactor = fBlueFactor = ComputeMinimumEntropyFactor(sValuePairs);
		ZTRACE_RUNTIME("Monochrome coefficient: %.2f", fRedFactor);
	}
	else if (pBitmap->IsMonochrome() && pBitmap->IsCFA())
	{
		VALUEPAIRSET		sRedValuePairs;
		VALUEPAIRSET		sGreenValuePairs;
		VALUEPAIRSET		sBlueValuePairs;

		for (int i = subSquare.m_rcSquare.left; i < subSquare.m_rcSquare.right; i++)
		{
			for (int j = subSquare.m_rcSquare.top; j < subSquare.m_rcSquare.bottom; j++)
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
						else
							sRedValuePairs.insert(vp);
						break;
					case BAYER_GREEN :
						it = sGreenValuePairs.find(vp);
						if (it != sGreenValuePairs.end())
							(const_cast<CValuePair&>(*it)).m_lCount++;
						else
							sGreenValuePairs.insert(vp);
						break;
					case BAYER_BLUE :
						it = sBlueValuePairs.find(vp);
						if (it != sBlueValuePairs.end())
							(const_cast<CValuePair&>(*it)).m_lCount++;
						else
							sBlueValuePairs.insert(vp);
						break;
					}
				}
			}
		}

		// Remove Hot pixels
		for (size_t i = 0; i < vExcludedPixels.size(); i++)
		{
			double					fLight;
			double					fDark;

			if (subSquare.m_rcSquare.contains(QPoint(vExcludedPixels[i].X, vExcludedPixels[i].Y)))
			{
				pBitmap->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fLight);
				m_pMasterDark->GetPixel(vExcludedPixels[i].X, vExcludedPixels[i].Y, fDark);

				{
					const CValuePair vp{ fLight * 256.0, fDark * 256.0 };


					const auto reduceCount = [vp](VALUEPAIRSET& valuePairs) {
						VALUEPAIRITERATOR it = valuePairs.find(vp);
						if (it != valuePairs.end() && it->m_lCount > 0)
							(const_cast<CValuePair&>(*it)).m_lCount--;
					};

					switch (pBitmap->GetBayerColor(vExcludedPixels[i].X, vExcludedPixels[i].Y))
					{
					case BAYER_RED:
						reduceCount(sRedValuePairs);
						break;
					case BAYER_GREEN:
						reduceCount(sGreenValuePairs);
						break;
					case BAYER_BLUE:
						reduceCount(sBlueValuePairs);
						break;
					}
				}
			}
		}

		// Remove anything that is not between [mean-sigma, mean+sigma]
		fRedFactor		= ComputeMinimumEntropyFactor(sRedValuePairs);

		fGreenFactor	= ComputeMinimumEntropyFactor(sGreenValuePairs);

		fBlueFactor		= ComputeMinimumEntropyFactor(sBlueValuePairs);

		ZTRACE_RUNTIME("RGB coefficients: Red = %.2f - Green = %.2f - Blue = %.2f", fRedFactor, fGreenFactor, fBlueFactor);
	}
	else
	{
		VALUEPAIRSET		sRedValuePairs;
		VALUEPAIRSET		sGreenValuePairs;
		VALUEPAIRSET		sBlueValuePairs;

		for (int i = subSquare.m_rcSquare.left; i < subSquare.m_rcSquare.right; i++)
		{
			for (int j = subSquare.m_rcSquare.top; j < subSquare.m_rcSquare.bottom; j++)
			{
				double fRedLight, fGreenLight, fBlueLight;
				double fRedDark, fGreenDark, fBlueDark;

				pBitmap->GetPixel(i, j, fRedLight, fGreenLight, fBlueLight);
				m_pMasterDark->GetPixel(i, j, fRedDark, fGreenDark, fBlueDark);

				const auto addValuePair = [](VALUEPAIRSET& valuePairs, const CValuePair& vp) {
					VALUEPAIRITERATOR it = valuePairs.find(vp);
					if (it != valuePairs.end())
						(const_cast<CValuePair&>(*it)).m_lCount++;
					else
						valuePairs.insert(vp);
				};

				addValuePair(sRedValuePairs, CValuePair{ fRedLight * 256.0, fRedDark * 256.0 });
				addValuePair(sGreenValuePairs, CValuePair{ fGreenLight * 256.0, fGreenDark * 256.0 });
				addValuePair(sBlueValuePairs, CValuePair{ fBlueLight * 256.0, fBlueDark * 256.0 });
			}
		}

		// Remove Hot pixels
		for (size_t i = 0; i < vExcludedPixels.size(); i++)
		{
			VALUEPAIRITERATOR		it;
			double					fRedLight, fGreenLight, fBlueLight;
			double					fRedDark, fGreenDark, fBlueDark;

			if (subSquare.m_rcSquare.contains(QPoint(vExcludedPixels[i].X, vExcludedPixels[i].Y)))
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
			}
		}

		fRedFactor		= ComputeMinimumEntropyFactor(sRedValuePairs);
		fGreenFactor	= ComputeMinimumEntropyFactor(sGreenValuePairs);
		fBlueFactor		= ComputeMinimumEntropyFactor(sBlueValuePairs);

		ZTRACE_RUNTIME("RGB coefficients: Red = %.2f - Green = %.2f - Blue = %.2f", fRedFactor, fGreenFactor, fBlueFactor);
	}
}

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
public:
	double					m_fHot;
	double					m_fMedian;
	IMAGEREGION				m_Region;

private:
	void	CopyFrom(const CHotCheckPixel& hcp)
	{
		m_fHot = hcp.m_fHot;
		m_fMedian = hcp.m_fMedian;
		m_Region = hcp.m_Region;
	}

public:
	CHotCheckPixel(double fHot, double fMedian, IMAGEREGION Region)
	{
		m_fHot = fHot;
		m_fMedian = fMedian;
		m_Region = Region;
	}
	~CHotCheckPixel() {}

	CHotCheckPixel(const CHotCheckPixel& hcp)
	{
		CopyFrom(hcp);
	}

	CHotCheckPixel& operator = (const CHotCheckPixel& hcp)
	{
		CopyFrom(hcp);

		return (*this);
	}

	bool operator < (const CHotCheckPixel& hcp) const
	{
		return hcp.m_fHot < m_fHot;
	}
};

/* ------------------------------------------------------------------- */

IMAGEREGION	GetPixelRegion(int lX, int lY, int lWidth, int lHeight)
{
	IMAGEREGION				Result = IR_NONE;
	int					lPosition;

	lPosition = (10 * std::max(1, std::min(3, lX * 3 / lWidth + 1)) + std::max(1, std::min(3, lY * 3 / lHeight + 1)));
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
	}

	return Result;
}


void CDarkFrameHotParameters::ComputeParameters(CMemoryBitmap* pBitmap, HOTPIXELVECTOR& vHotPixels)
{
	ZFUNCTRACE_RUNTIME();

	const int lWidth = pBitmap->RealWidth();
	const int lHeight = pBitmap->RealHeight();

	std::vector<CHotCheckPixel>	vHots;
	CMedianImageFilter Filter;

	Filter.SetBitmap(pBitmap);
	Filter.SetFilterSize(2);

	for (int i = 0;i<vHotPixels.size();i++)
	{
		// Compute the median around each hot pixel
		int			X = vHotPixels[i].m_lX,
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
			}
		}
		else
		{
			double			fMedianRed, fMedianGreen, fMedianBlue;
			double			fHotRed, fHotGreen, fHotBlue;

			Filter.ComputeMedianAt(X, Y, fMedianRed, fMedianGreen, fMedianBlue);
			pBitmap->GetPixel(X, Y, fHotRed, fHotGreen, fHotBlue);

			vHots.emplace_back((fHotRed+fHotGreen+fHotBlue)/3.0, (fMedianRed+fMedianGreen+fMedianBlue)/3.0, Region);
		}
	}

	int					lNrCovered = 0;
	double					fSumHot    = 0,
							fSumMedian = 0;
	std::uint32_t			dwCovered1 = 0,
							dwCovered2 = 0;

	std::sort(vHots.begin(), vHots.end());
	for (int i = 0;i<vHots.size() && (lNrCovered!=18);i++)
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
		}
	}

	if (lNrCovered)
	{
		fSumHot /= lNrCovered;
		fSumMedian /= lNrCovered;
	}

	m_fGrayValue = fSumHot - fSumMedian;
}

/* ------------------------------------------------------------------- */

void	CDarkAmpGlowParameters::ComputeParametersFromPoints(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	double			m_fMedianColdest = -1;

	m_fMedianHotest = computeMedianValueInRect(pBitmap, m_rcHotest);
	m_vMedianColdest.clear();

	for (int k = 0;k<m_vrcColdest.size();k++)
	{
		double		fValue;
		fValue = computeMedianValueInRect(pBitmap, m_vrcColdest[k]);
		m_vMedianColdest.push_back(fValue);
		if ((m_fMedianColdest<0) || (m_fMedianColdest>fValue))
		{
			m_fMedianColdest = fValue;
			m_lColdestIndice = k;
		}
	}

	m_fGrayValue = m_fMedianHotest-m_fMedianColdest;
}

/* ------------------------------------------------------------------- */

double CDarkAmpGlowParameters::computeMedianValueInRect(CMemoryBitmap* pBitmap, const DSSRect& rc)
{
	ZFUNCTRACE_RUNTIME();
	double				fResult = 0;
	RGBHistogram		rgbHistogram;
	bool				bMonochrome = pBitmap->IsMonochrome();
	bool				bCFA = pBitmap->IsCFA();

	rgbHistogram.SetSize(256.0, std::numeric_limits<std::uint16_t>::max() + 1);
	for (int i = rc.left; i < rc.right; i++)
	{
		for (int j = rc.top; j < rc.bottom; j++)
		{
			if (bCFA)
			{
				double				fGray;

				pBitmap->GetPixel(i, j, fGray);

				switch (pBitmap->GetBayerColor(i, j))
				{
				case BAYER_RED :
					rgbHistogram.GetRedHistogram().AddValue(fGray);
					break;
				case BAYER_GREEN :
					rgbHistogram.GetGreenHistogram().AddValue(fGray);
					break;
				case BAYER_BLUE :
					rgbHistogram.GetBlueHistogram().AddValue(fGray);
					break;
				}
			}
			else if (bMonochrome)
			{
				double				fGray;

				pBitmap->GetPixel(i, j, fGray);
				rgbHistogram.GetRedHistogram().AddValue(fGray);
			}
			else
			{
				double				fRed, fGreen, fBlue;

				pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
				rgbHistogram.AddValues(fRed, fGreen, fBlue);;
			}
		}
	}

	if (bMonochrome && !bCFA)
		fResult = rgbHistogram.GetRedHistogram().GetMedian();
	else
	{
		fResult = (rgbHistogram.GetRedHistogram().GetMedian()+
				   rgbHistogram.GetGreenHistogram().GetMedian()+
				   rgbHistogram.GetBlueHistogram().GetMedian())/3;
	}

	return fResult;
}

/* ------------------------------------------------------------------- */

void CDarkAmpGlowParameters::FindPointsAndComputeParameters(CMemoryBitmap* pBitmap)
{
	ZFUNCTRACE_RUNTIME();
	bool				bMonochrome = pBitmap->IsMonochrome();
	bool				bCFA = pBitmap->IsCFA();
	int				lWidth = pBitmap->RealWidth(),
						lHeight = pBitmap->RealHeight();

	std::vector<int>	vColumns;

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
	for (int j = 1;j<lHeight-1;j++)
	{
		for (int i = 0;i<vColumns.size();i++)
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
						}
						break;
					case BAYER_GREEN :
						if (fGray>fMaxGreen)
						{
							fMaxGreen = fGray;
							pxMaxGreen.SetPixel(vColumns[i], j);
						}
						break;
					case BAYER_BLUE :
						if (fGray>fMaxBlue)
						{
							fMaxBlue = fGray;
							pxMaxBlue.SetPixel(vColumns[i], j);
						}
						break;
					}
				}
				else
				{
					if (fGray>fMaxGray)
					{
						fMaxGray = fGray;
						pxMaxGray.SetPixel(vColumns[i], j);
					}
				}
			}
			else
			{
				double			fRed, fGreen, fBlue;

				pBitmap->GetPixel(vColumns[i], j, fRed, fGreen, fBlue);

				if (fRed>fMaxRed)
				{
					fMaxRed = fRed;
					pxMaxRed.SetPixel(vColumns[i], j);
				}
				if (fGreen>fMaxGreen)
				{
					fMaxGreen = fGreen;
					pxMaxGreen.SetPixel(vColumns[i], j);
				}
				if (fBlue>fMaxBlue)
				{
					fMaxBlue = fBlue;
					pxMaxBlue.SetPixel(vColumns[i], j);
				}
			}
		}
	}

	// Now compute the median value around each pixel
	if (bMonochrome && !bCFA)
	{
		m_rcHotest = getRectAroundPoint(lWidth, lHeight, 20, pxMaxGray);
		m_fMedianHotest = computeMedianValueInRect(pBitmap, m_rcHotest);
	}
	else
	{
		// Check all the points
#pragma warning (suppress:4456)
		double			fMaxRed, fMaxGreen, fMaxBlue;

		DSSRect			rcMaxRed,
						rcMaxGreen,
						rcMaxBlue;

		rcMaxRed = getRectAroundPoint(lWidth, lHeight, 20, pxMaxRed);
		fMaxRed = computeMedianValueInRect(pBitmap, rcMaxRed);
		rcMaxGreen = getRectAroundPoint(lWidth, lHeight, 20, pxMaxGreen);
		fMaxGreen = computeMedianValueInRect(pBitmap, rcMaxGreen);
		rcMaxBlue = getRectAroundPoint(lWidth, lHeight, 20, pxMaxBlue);
		fMaxBlue = computeMedianValueInRect(pBitmap, rcMaxBlue);

		double			fMax;

		fMax = std::max(fMaxRed, std::max(fMaxGreen, fMaxBlue));
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
		}
	}

	// Now find the coldest rectangle
	std::vector<DSSRect>	vRects;
	double					m_fMedianColdest = -1;

	getBorderRects(lWidth, lHeight, vRects);

	m_vrcColdest = vRects;
	m_vMedianColdest.clear();

	for (int k = 0;k<vRects.size();k++)
	{
		double			fValue;

		fValue = computeMedianValueInRect(pBitmap, vRects[k]);
		m_vMedianColdest.push_back(fValue);
		if ((m_fMedianColdest<0) || (m_fMedianColdest>fValue))
		{
			m_fMedianColdest = fValue;
			m_lColdestIndice = k;
		}
	}

	m_fGrayValue = m_fMedianHotest-m_fMedianColdest;
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CDarkFrame::ComputeDarkFactorFromMedian(CMemoryBitmap* pBitmap, double& fHotDark, double& fAmpGlow, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	if (!static_cast<bool>(m_pAmpGlow))
	{
		// First compute the median from the master dark
		// This is the ampglow...
		CMedianImageFilter Filter;
		Filter.SetFilterSize(2);

		if (pProgress != nullptr)
		{
			const QString strText(QCoreApplication::translate("DarkFrame", "Creating Median Filtered Image", "IDS_CREATINGMEDIANIMAGE"));
			pProgress->Start2(strText, 0);
		}

		m_pAmpGlow = Filter.ApplyFilter(m_pMasterDark.get(), pProgress);
		//WriteTIFF("E:\\AmpGlow.tif", m_pAmpGlow, pProgress, nullptr);

		m_pDarkCurrent = m_pMasterDark->Clone();
		// Then subtract this median from the master dark
		// This is the dark signal

		::Subtract(m_pDarkCurrent, m_pAmpGlow, pProgress);

		//WriteTIFF("E:\\DarkCurrent.tif", m_pDarkCurrent, pProgress, nullptr);

		// Compute the parameters from the ampglow and the dark current

		// First the dark current
		m_HotParameters.ComputeParameters(m_pMasterDark.get()/*m_pDarkCurrent*/, m_vHotPixels);

		// Then the Ampglow
		m_AmpglowParameters.FindPointsAndComputeParameters(m_pAmpGlow.get());
	}

	if (static_cast<bool>(m_pAmpGlow) && static_cast<bool>(m_pDarkCurrent))
	{
		// now this is the fun part
		// Compute ampglow matching and dark matching

		// First dark hot pixels
		CDarkFrameHotParameters	lightHotParameters;

		lightHotParameters.ComputeParameters(pBitmap, m_vHotPixels);

		// Then compare to compute the ratio
		if (m_HotParameters.m_fGrayValue != 0)
			fHotDark = lightHotParameters.m_fGrayValue / m_HotParameters.m_fGrayValue;
		else
			fHotDark = 1.0;

		// Then Ampglow
		CDarkAmpGlowParameters lightAmpGlowParameters(m_AmpglowParameters);
		lightAmpGlowParameters.ComputeParametersFromPoints(pBitmap);
		m_AmpglowParameters.ComputeParametersFromIndice(lightAmpGlowParameters.m_lColdestIndice);

		if (lightAmpGlowParameters.m_fGrayValue > 0 && m_AmpglowParameters.m_fGrayValue > 0)
			fAmpGlow = std::min(lightAmpGlowParameters.m_fGrayValue / m_AmpglowParameters.m_fGrayValue, 1.0);
		else
			fAmpGlow = 1.0;
	}
}

/* ------------------------------------------------------------------- */

void	CDarkFrame::ComputeDarkFactorFromHotPixels(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor)
{
	ZFUNCTRACE_RUNTIME();

	HOTPIXELVECTOR vHotPixels;

	fRedFactor	 = 1.0;
	fGreenFactor = 1.0;
	fBlueFactor	 = 1.0;

	if (m_vHotPixels.size())
	{
		// Remove hot pixels that are in stars
		if (pStars)
		{
			EXCLUDEDPIXELSET sStarPixels;

			for (size_t i = 0; i < pStars->size(); i++)
			{
				const DSSRect &rcStar = (*pStars)[i].m_rcStar;

				for (int x = rcStar.left; x < rcStar.right; x++)
					for (int y = rcStar.top; y < rcStar.bottom; y++)
					{
						const CExcludedPixel ep(x, y);

						if (0 == sStarPixels.count(ep))
							sStarPixels.insert(ep);
					}
			}

			for (size_t i = 0; i < m_vHotPixels.size(); i++)
			{
				const CExcludedPixel ep(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY);

				if (0 == sStarPixels.count(ep))
					vHotPixels.push_back(m_vHotPixels[i]);
			}
		}
		else
			vHotPixels = m_vHotPixels;

		// For each hot pixel compare the value in the light and in the dark
		if (pBitmap->IsMonochrome() && !pBitmap->IsCFA())
		{
			std::vector<double>	vRatios;

			for (size_t i = 0; i < vHotPixels.size(); i++)
			{
				double fLight,
					fDark;

				pBitmap->GetPixel(vHotPixels[i].m_lX, vHotPixels[i].m_lY, fLight);
				m_pMasterDark->GetPixel(vHotPixels[i].m_lX, vHotPixels[i].m_lY, fDark);

				if (fLight > 0 && fDark > 0)
				{
					double			fRatio;

					fRatio = fLight/fDark;
					vRatios.push_back(fRatio);
				}
			}
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

			for (size_t i = 0 ;i<vHotPixels.size();i++)
			{
				double fLight,
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
					}
				}
			}
			std::sort(vRedRatios.begin(), vRedRatios.end());
			std::sort(vGreenRatios.begin(), vGreenRatios.end());
			std::sort(vBlueRatios.begin(), vBlueRatios.end());

			fRedFactor = KappaSigmaClip(vRedRatios, 2.0, 5, vWork);
			fGreenFactor = KappaSigmaClip(vGreenRatios, 2.0, 5, vWork);
			fBlueFactor = KappaSigmaClip(vBlueRatios, 2.0, 5, vWork);
		}
	}
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void CDarkFrame::FindBadVerticalLines(ProgressBase*)
{
	ZFUNCTRACE_RUNTIME();
	bool				bMonochrome = m_pMasterDark->IsMonochrome();
	int				i, j;

	if (bMonochrome)
	{
		// Create an image holding the vertical averaged pixels
		int						lWidth = m_pMasterDark->RealWidth(),
									lHeight = m_pMasterDark->RealHeight();

		for (i = 1;i<lWidth-1;i++)
		{
			bool					bLineInProgress = false;
			bool					bLighterLine = false;
			int					lStartY = 0;
			int					lNrPixels = 0;
			int					lNrOutPixels = 0;
			int					lNrConsecutiveOutPixels = 0;

			for (j = 0;j<lHeight;j++)
			{
				double				fLeftValue,
									fValue,
									fRightValue;

				bool				bLighter,
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
							bLineInProgress = false;
							if (lNrPixels > 10)
							{
								for (int k = lStartY;k<lStartY+lNrPixels-lNrConsecutiveOutPixels;k++)
								{
									CHotPixel		hp(i, k);
									m_vHotPixels.push_back(hp);
								}
							}
						}
					}
				}
				else if (bLighter || bDarker)
				{
					lStartY = j;
					lNrPixels = 1;
					lNrOutPixels = 0;
					lNrConsecutiveOutPixels = 0;
					bLineInProgress = true;
					bLighterLine = bLighter;
				}
			}

			if (bLineInProgress)
			{
				if (lNrPixels > 10)
				{
					for (int k = lStartY;k<lStartY+lNrPixels-lNrConsecutiveOutPixels;k++)
					{
						CHotPixel		hp(i, k);
						m_vHotPixels.push_back(hp);
					}
				}
			}
		}
		m_bHotPixelDetected = true;
	}
}

/* ------------------------------------------------------------------- */

class CFindHotPixelTask1
{
public:
	RGBHistogram m_RGBHistogram;
	std::shared_ptr<CMemoryBitmap> m_pBitmap;
	ProgressBase* m_pProgress;

public:
	explicit CFindHotPixelTask1(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase* pProgress) :
		m_pBitmap{ pBitmap },
		m_pProgress{ pProgress }
	{
		m_RGBHistogram.SetSize(256.0, 65535);
	}

	~CFindHotPixelTask1() = default;

	void process();
};

template <class T> struct threadLocals {
	const T* bitmap;
	BitmapIteratorConst<const CMemoryBitmap*> PixelIt;
	RGBHistogram rgbHistogram;

	explicit threadLocals(const T* bm) : bitmap{ bm }, PixelIt{ bm }
	{
		rgbHistogram.SetSize(256.0, 65535);
	}
	threadLocals(const threadLocals& rhs) : bitmap{ rhs.bitmap }, PixelIt{ rhs.bitmap }
	{
		rgbHistogram.SetSize(256.0, 65535);
	}
};

void CFindHotPixelTask1::process()
{
	ZFUNCTRACE_RUNTIME();
	const int nrProcessors = CMultitask::GetNrProcessors();
	const int height = m_pBitmap->RealHeight();
	const int width = m_pBitmap->RealWidth();
	int progress = 0;

	threadLocals threadVars(m_pBitmap.get());

#pragma omp parallel default(shared) firstprivate(threadVars) if(nrProcessors > 1)
	{
#pragma omp for schedule(guided, 100)
		for (int row = 0; row < height; ++row)
		{
			if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
				m_pProgress->Progress2(progress += nrProcessors);

			threadVars.PixelIt.Reset(0, row);
			double r, g, b;

			for (int col = 0; col < width; ++col, ++threadVars.PixelIt)
			{
				threadVars.PixelIt.GetPixel(r, g, b); // GetPixel is virtual => works for monochrome bitmaps, too.
				threadVars.rgbHistogram.AddValues(r, g, b);
			}
		}

#pragma omp critical(OmpLockDarkFindHot)
		{
			m_RGBHistogram.AddValues(threadVars.rgbHistogram);
		}
	} // omp parallel
}


/* ------------------------------------------------------------------- */

void	CDarkFrame::RemoveContiguousHotPixels(bool bCFA)
{
	ZFUNCTRACE_RUNTIME();
	HOTPIXELVECTOR			vNewHotPixels;
	int					lStep = bCFA ? 2 : 1;
	int					lNrDiscarded = 0;

	for (int i = 0;i<m_vHotPixels.size();i++)
	{
		CHotPixel &			hp = m_vHotPixels[i];
		int				lNrHotNeighbors = 0;

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
	}

	if (lNrDiscarded)
		m_vHotPixels = vNewHotPixels;
}

/* ------------------------------------------------------------------- */

void CDarkFrame::FindHotPixels(ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	m_vHotPixels.clear();
	if (static_cast<bool>(m_pMasterDark))
	{
		RGBHistogram rgbHistogram;
		const bool bMonochrome = m_pMasterDark->IsMonochrome();

		if (pProgress)
		{
			const QString strText(QCoreApplication::translate("DarkFrame", "Detecting Hot Pixels (1/2)", "IDS_DETECTINGHOTPIXELS1"));
			pProgress->Start2(strText, m_pMasterDark->RealHeight());
		}

		CFindHotPixelTask1 hotPixelTask1{ m_pMasterDark, pProgress };
		hotPixelTask1.process();

		if (pProgress)
		{
			pProgress->End2();
			const QString strText(QCoreApplication::translate("DarkFrame", "Detecting Hot Pixels (2/2)", "IDS_DETECTINGHOTPIXELS2"));
			pProgress->Start2(strText, m_pMasterDark->RealHeight());
		}

		double fRedThreshold = hotPixelTask1.m_RGBHistogram.GetRedHistogram().GetMedian()+16.0 * hotPixelTask1.m_RGBHistogram.GetRedHistogram().GetStdDeviation();
		double fGreenThreshold = hotPixelTask1.m_RGBHistogram.GetGreenHistogram().GetMedian()+16.0 * hotPixelTask1.m_RGBHistogram.GetGreenHistogram().GetStdDeviation();
		double fBlueThreshold = hotPixelTask1.m_RGBHistogram.GetBlueHistogram().GetMedian()+16.0 * hotPixelTask1.m_RGBHistogram.GetBlueHistogram().GetStdDeviation();

		const int lWidth  = m_pMasterDark->RealWidth();
		const int lHeight = m_pMasterDark->RealHeight();
		BitmapIteratorConst<std::shared_ptr<const CMemoryBitmap>> pixelIt{ m_pMasterDark };


		for (int j = 0; j < lHeight; ++j)
		{
			if (bMonochrome)
			{
				for (int i = 0; i < lWidth; ++i, ++pixelIt)
					if (pixelIt.GetPixel() > fRedThreshold) // This is a hot pixel
						m_vHotPixels.emplace_back(CHotPixel{ i, j });
			}
			else
			{
				for (int i = 0; i < lWidth; ++i, ++pixelIt)
				{
					double fRed, fGreen, fBlue;
					pixelIt.GetPixel(fRed, fGreen, fBlue);
					if (fRed > fRedThreshold || fGreen > fGreenThreshold || fBlue > fBlueThreshold) // This is a hot pixel
						m_vHotPixels.emplace_back(CHotPixel{ i, j });
				}
			}
			if (pProgress)
				pProgress->Progress2(j+1);
		}
		if (pProgress)
			pProgress->End2();

		std::sort(m_vHotPixels.begin(), m_vHotPixels.end());
		RemoveContiguousHotPixels(m_pMasterDark->IsCFA());
	}

	m_bHotPixelDetected = true;
}

/* ------------------------------------------------------------------- */

void	CDarkFrame::GetValidNeighbors(int lX, int lY, HOTPIXELVECTOR & vPixels, int lRadius, BAYERCOLOR BayerColor)
{
	vPixels.clear();
	for (int i = std::max(0, lX - lRadius); i < std::min(m_pMasterDark->RealWidth(), 1+ lX + lRadius); i++)
	{
		for (int j = std::max(0, lY - lRadius); j < std::min(m_pMasterDark->RealHeight(), 1 + lY + lRadius); j++)
		{
			if ((i != lX) || (j != lY))
			{
				int				lWeight;
				bool				bAdd = false;

				lWeight = labs(1+lRadius-labs(lX - i)) + labs(1+lRadius-labs(lY - j));
				CHotPixel			hp(i, j, lWeight);

				if (!std::binary_search(m_vHotPixels.begin(), m_vHotPixels.end(), hp))
					bAdd = true;
				if ((BayerColor != BAYER_UNKNOWN) && (m_pMasterDark->GetBayerColor(i, j) != BayerColor))
					bAdd = false;

				if (bAdd)
					vPixels.push_back(hp);
			}
		}
	}
}

/* ------------------------------------------------------------------- */

void	CDarkFrame::InterpolateHotPixels(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase*)
{
	ZFUNCTRACE_RUNTIME();
	if (static_cast<bool>(pBitmap) && !m_vHotPixels.empty())
	{
		// First set hot pixels to 0
		for (size_t i = 0; i < m_vHotPixels.size(); i++)
			pBitmap->SetPixel(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, 0.0);

		// Then Interpolate Hot Pixels
		if (pBitmap->IsMonochrome())
		{
			// Check and remove super pixel settings
			CFATRANSFORMATION CFATransform = CFAT_NONE;

			CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(pBitmap.get());
			if (pCFABitmapInfo)
			{
				CFATransform = pCFABitmapInfo->GetCFATransformation();
				if (CFATransform == CFAT_SUPERPIXEL)
					pCFABitmapInfo->UseBilinear(true);
			}

			// Interpolate with neighbor pixels (level 1)
			const bool bCFA = pBitmap->IsCFA();


			for (size_t i = 0; i < m_vHotPixels.size(); i++)
			{
				HOTPIXELVECTOR vPixels;
				BAYERCOLOR BayerColor = BAYER_UNKNOWN;
				double fValue = 0.0;
				int lTotalWeight = 0;

				if (bCFA)
					BayerColor = pBitmap->GetBayerColor(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY);

				GetValidNeighbors(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, vPixels, bCFA ? 2 : 1, BayerColor);
				for (size_t j = 0; j < vPixels.size(); j++)
				{
					double fGray;

					pBitmap->GetPixel(vPixels[j].m_lX, vPixels[j].m_lY, fGray);
					fValue += fGray * vPixels[j].m_lWeight;
					lTotalWeight += vPixels[j].m_lWeight;
				}
				if (lTotalWeight)
					fValue /= lTotalWeight;
				pBitmap->SetPixel(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, fValue);

			}
			if (CFATransform == CFAT_SUPERPIXEL)
				pCFABitmapInfo->UseSuperPixels(true);
		}
		else
		{
			for (size_t i = 0; i < m_vHotPixels.size(); i++)
			{
				HOTPIXELVECTOR vPixels;
				double fRedValue	= 0.0, fGreenValue = 0.0, fBlueValue  = 0.0;
				int lTotalWeight = 0;

				GetValidNeighbors(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, vPixels, 1);
				for (size_t j = 0; j < vPixels.size(); j++)
				{
					double			fRed, fGreen, fBlue;

					pBitmap->GetPixel(vPixels[j].m_lX, vPixels[j].m_lY, fRed, fGreen, fBlue);
					fRedValue	+= fRed * vPixels[j].m_lWeight;
					fGreenValue += fGreen * vPixels[j].m_lWeight;
					fBlueValue  += fBlue * vPixels[j].m_lWeight;
					lTotalWeight += vPixels[j].m_lWeight;
				}
				if (lTotalWeight != 0)
				{
					fRedValue /= lTotalWeight;
					fGreenValue /= lTotalWeight;
					fBlueValue /= lTotalWeight;
				}
				pBitmap->SetPixel(m_vHotPixels[i].m_lX, m_vHotPixels[i].m_lY, fRedValue, fGreenValue, fBlueValue);
			}
		}
	}
}

/* ------------------------------------------------------------------- */

bool CDarkFrame::Subtract(std::shared_ptr<CMemoryBitmap> pTarget, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();

	if (m_pMasterDark && m_pMasterDark->IsOk())
	{

		if ((m_bHotPixelsDetection || m_bBadLinesDetection) && !m_bHotPixelDetected)
		{
			if (m_bHotPixelsDetection)
				FindHotPixels(pProgress);
			if (m_bBadLinesDetection)
				FindBadVerticalLines(pProgress);
		}

		if (m_bDarkOptimization)
		{
			double fHotDark = 1.0;
			double fAmpGlow = 1.0;

			//ComputeDistribution(pTarget, pStars);
			//ComputeDarkFactorFromHotPixels(pTarget, pStars, );
			//ComputeDarkFactor(pTarget, pStars, fRedDarkFactor, fGreenDarkFactor, fBlueDarkFactor, pProgress);
			if (pProgress != nullptr)
			{
				const QString strText(QCoreApplication::translate("DarkFrame", "Optimizing Dark Matching", "IDS_OPTIMIZINGDARKMATCHING"));
				pProgress->Start2(strText, 0);
			}
			ComputeDarkFactorFromMedian(pTarget.get(), fHotDark, fAmpGlow, pProgress);

			if (pProgress != nullptr)
			{
				const QString strText(QCoreApplication::translate("DarkFrame", "Subtracting Dark Frame", "IDS_SUBSTRACTINGDARK"));
				pProgress->Start2(strText, 0);
			}
			::Subtract(pTarget, m_pAmpGlow, pProgress, fAmpGlow, fAmpGlow, fAmpGlow);
			::Subtract(pTarget, m_pDarkCurrent, pProgress, fHotDark, fHotDark, fHotDark);
		}
		else
		{
			if (pProgress != nullptr)
			{
				const QString strText(QCoreApplication::translate("DarkFrame", "Subtracting Dark Frame", "IDS_SUBSTRACTINGDARK"));
				pProgress->Start2(strText, 0);
			}
			::Subtract(pTarget, m_pMasterDark, pProgress, m_fDarkFactor, m_fDarkFactor, m_fDarkFactor);
		}
	}

	return true;
}

void CDarkFrame::Reset(std::shared_ptr<CMemoryBitmap> pMaster)
{
	m_bDarkOptimization = CAllStackingTasks::GetDarkOptimization();
	m_bHotPixelsDetection = CAllStackingTasks::GetHotPixelsDetection();
	m_bBadLinesDetection = CAllStackingTasks::GetBadLinesDetection();
	m_fDarkFactor = CAllStackingTasks::GetDarkFactor();
	m_bHotPixelDetected = false;
	m_pMasterDark = pMaster;
	m_vHotPixels.clear();
}

