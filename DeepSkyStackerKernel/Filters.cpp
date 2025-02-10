#include "stdafx.h"
#include "Filters.h"
#include "ztrace.h"
#include "MemoryBitmap.h"
#include "DSSProgress.h"

using namespace DSS;

/* ------------------------------------------------------------------- */

inline double	GetMedianPosition(std::vector<double> & vValues, double fValue)
{
	double			fResult = 0;
	double			fMedian;
	double			fdValue;

	fMedian = Median(vValues);
	fdValue = fValue-fMedian;

	if (fdValue>0)
	{
		fResult = fdValue/fValue;
	}
	else if (fdValue<0)
	{
		fResult = fdValue/fMedian;
	};

	return fResult;
};

/* ------------------------------------------------------------------- */

void CExtendedMedianImageFilter::AnalyzeImage(const CMemoryBitmap * pInBitmap, bool bComputeThresholds)
{
	ZFUNCTRACE_RUNTIME();
	int				lWidth = pInBitmap->Width(),
						lHeight = pInBitmap->Height();
	bool				bMonochrome = pInBitmap->IsMonochrome();

	std::vector<double>		vGrays,
							vReds,
							vGreens,
							vBlues;

	vGrays.resize(8);
	vReds.resize(8);
	vGreens.resize(8);
	vBlues.resize(8);

	std::vector<int>		vHotPixels,
							vColdPixels;

	vHotPixels.resize(1001);
	vColdPixels.resize(1001);

	for (int j= 1;j<lHeight-1;j++)
	{
		for (int i = 1;i<lWidth-1;i++)
		{
			double				fPosition = 0;
			if (bMonochrome)
			{
				double			fGray;

				pInBitmap->GetPixel(i-1, j-1, vGrays[0]);
				pInBitmap->GetPixel(i,   j-1, vGrays[1]);
				pInBitmap->GetPixel(i+1, j-1, vGrays[2]);
				pInBitmap->GetPixel(i-1, j,   vGrays[3]);
				pInBitmap->GetPixel(i+1, j,   vGrays[4]);
				pInBitmap->GetPixel(i-1, j+1, vGrays[5]);
				pInBitmap->GetPixel(i,   j+1, vGrays[6]);
				pInBitmap->GetPixel(i+1, j+1, vGrays[7]);

				pInBitmap->GetPixel(i, j, fGray);
				fPosition = GetMedianPosition(vGrays, fGray);
			}
			else
			{
				double				fRed, fGreen, fBlue;
				double				fPositionRed, fPositionGreen, fPositionBlue;

				pInBitmap->GetPixel(i-1, j-1, vReds[0], vGreens[0], vBlues[0]);
				pInBitmap->GetPixel(i,   j-1, vReds[1], vGreens[1], vBlues[1]);
				pInBitmap->GetPixel(i+1, j-1, vReds[2], vGreens[2], vBlues[2]);
				pInBitmap->GetPixel(i-1, j,   vReds[3], vGreens[3], vBlues[3]);
				pInBitmap->GetPixel(i+1, j,   vReds[4], vGreens[4], vBlues[4]);
				pInBitmap->GetPixel(i-1, j+1, vReds[5], vGreens[5], vBlues[5]);
				pInBitmap->GetPixel(i,   j+1, vReds[6], vGreens[6], vBlues[6]);
				pInBitmap->GetPixel(i+1, j+1, vReds[7], vGreens[7], vBlues[7]);

				pInBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
				fPositionRed   = GetMedianPosition(vReds, fRed);
				fPositionGreen = GetMedianPosition(vGreens, fGreen);
				fPositionBlue  = GetMedianPosition(vBlues, fBlue);
				fPosition = (fPositionRed+fPositionGreen+fPositionBlue)/3.0;
			};
			if (bComputeThresholds)
			{
				if (fPosition>0)
					vHotPixels[min(fPosition*1000, 1000.0)]++;
				else if (fPosition<0)
					vColdPixels[min(-fPosition*1000, 1000.0)]++;
			}
			else
			{
				if ((fPosition>m_fHotThreshold) || // Hot pixel
					(-fPosition>m_fColdThreshold)) // Cold pixel
					m_vExcludedPixels.emplace_back(i, j);
			};
		};
	};

	if (bComputeThresholds)
	{
		int	lNrPixels = lWidth*lHeight;
		int	lNrRejectedHotPixels = lNrPixels * m_fRejectHotThreshold;
		int	lNrRejectedColdPixels = lNrPixels * m_fRejectColdThreshold;

		int	lThreshold;

		lThreshold = 1000;
		while ((lNrRejectedHotPixels>0) && (lThreshold>0))
		{
			lNrRejectedHotPixels-=vHotPixels[lThreshold];
			lThreshold--;
		};
		m_fHotThreshold = (double)lThreshold/1000.0;

		lThreshold = 1000;
		while ((lNrRejectedColdPixels>0) && (lThreshold>0))
		{
			lNrRejectedColdPixels-=vColdPixels[lThreshold];
			lThreshold--;
		};
		m_fColdThreshold = (double)lThreshold/1000.0;
	};
};

/* ------------------------------------------------------------------- */

inline void	CheckPixel(int X, int Y, EXCLUDEDPIXELVECTOR &vExcluded, EXCLUDEDPIXELVECTOR & vOkPixels)
{
	CExcludedPixel		TestPixel(X, Y);

	if (!binary_search(vExcluded.begin(), vExcluded.end(), TestPixel))
		vOkPixels.push_back(TestPixel);
};


/* ------------------------------------------------------------------- */

void CExtendedMedianImageFilter::ApplyFilterInternal(const CMemoryBitmap* pInBitmap, CMemoryBitmap* pOutBitmap, ProgressBase*)
{
	ZFUNCTRACE_RUNTIME();
	EXCLUDEDPIXELVECTOR vExcluded = m_vExcludedPixels;
	int lNrOkNeighbors = 8;
	const bool bMonochrome = pInBitmap->IsMonochrome();

	std::sort(vExcluded.begin(), vExcluded.end());

	while (!vExcluded.empty() && lNrOkNeighbors != 0)
	{
		// Check that at least 8 pixels are ok
		EXCLUDEDPIXELVECTOR vAuxExcluded;

		for (size_t k = 0; k < vExcluded.size(); k++)
		{
			// int lNrNeighbors = 0;
			CExcludedPixel& Pixel = vExcluded[k];
			CExcludedPixel TestPixel;
			EXCLUDEDPIXELVECTOR vOkPixels;

			CheckPixel(Pixel.X-1, Pixel.Y-1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X-1, Pixel.Y  , vExcluded, vOkPixels);
			CheckPixel(Pixel.X-1, Pixel.Y+1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X,   Pixel.Y-1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X,   Pixel.Y+1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X+1, Pixel.Y-1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X+1, Pixel.Y  , vExcluded, vOkPixels);
			CheckPixel(Pixel.X+1, Pixel.Y-1, vExcluded, vOkPixels);

			if (vOkPixels.size() >= lNrOkNeighbors)
			{
				// Interpolate with the good values
				double			fSumGray = 0,
								fSumRed  = 0,
								fSumGreen= 0,
								fSumBlue = 0;

				for (int l = 0;l<vOkPixels.size();l++)
				{
					CExcludedPixel	&	OkPixel = vOkPixels[l];

					if (bMonochrome)
					{
						double	fGray;

						pOutBitmap->GetPixel(OkPixel.X, OkPixel.Y, fGray);
						fSumGray += fGray;
					}
					else
					{
						double	fRed, fGreen, fBlue;

						pOutBitmap->GetPixel(OkPixel.X, OkPixel.Y, fRed, fGreen, fBlue);
						fSumRed		+= fRed;
						fSumGreen	+= fGreen;
						fSumBlue	+= fBlue;
					};
				};

				if (bMonochrome)
				{
					double		fAverage = fSumGray/vOkPixels.size();
					pOutBitmap->SetPixel(Pixel.X, Pixel.Y, fAverage);
				}
				else
				{
					double		fAverageRed   = fSumRed/vOkPixels.size();
					double		fAverageGreen = fSumGreen/vOkPixels.size();
					double		fAverageBlue  = fSumBlue/vOkPixels.size();

					pOutBitmap->SetPixel(Pixel.X, Pixel.Y, fAverageRed, fAverageGreen, fAverageBlue);
				}
			}
			else
				vAuxExcluded.push_back(Pixel);
		}

		if (vExcluded.size() == vAuxExcluded.size())
			lNrOkNeighbors--;
		vExcluded = vAuxExcluded;
	}
}

/* ------------------------------------------------------------------- */

std::shared_ptr<CMemoryBitmap> CExtendedMedianImageFilter::ApplyFilter(const CMemoryBitmap* pInBitmap, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	//
	// Note that if m_bUseRejectThreshold is set it is INTENTIONAL that analyseImage is called TWICE.
	// The first run sets m_fHotThreshold and m_fColdThreshold which are used in the second invocation.
	//
	if (m_bUseRejectThreshold)
		AnalyzeImage(pInBitmap, true);
	AnalyzeImage(pInBitmap, false);

	std::shared_ptr<CMemoryBitmap> pOutBitmap{ pInBitmap->Clone() };
	ApplyFilterInternal(pInBitmap, pOutBitmap.get(), pProgress);

	return pOutBitmap;
}

/* ------------------------------------------------------------------- */

void	CMedianImageFilter::ComputeMedianAt(int x, int y, double & fGrayValue, BAYERCOLOR BayerColor)
{
	std::vector<double>			vValues;

	m_vGrayPixelValues.resize(0);
	if (m_bCFA && BayerColor != BAYER_UNKNOWN)
	{
		for (int i = std::max(0, x - 2 * m_lFilterSize); i < std::min(m_lWidth - 1,  x + 2 * m_lFilterSize); i++)
		{
			for (int j = std::max(0, y - 2 * m_lFilterSize); j < std::min(m_lHeight - 1, y + 2 * m_lFilterSize); j++)
			{
				if (m_pInBitmap->GetBayerColor(i, j) == BayerColor)
				{
					double			fGray;

					m_pInBitmap->GetPixel(i, j, fGray);
					m_vGrayPixelValues.push_back(fGray);
				};
			};
		};
		fGrayValue = Median(m_vGrayPixelValues);
	}
	else
	{
		for (int i = std::max(0, x - m_lFilterSize); i < std::min(m_lWidth - 1, x + m_lFilterSize); i++)
		{
			for (int j = std::max(0, y - m_lFilterSize); j < std::min(m_lHeight - 1, y + m_lFilterSize); j++)
			{
				double			fGray;

				m_pInBitmap->GetPixel(i, j, fGray);
				m_vGrayPixelValues.push_back(fGray);
			};
		};
		fGrayValue = Median(m_vGrayPixelValues);
	};
};

/* ------------------------------------------------------------------- */

void	CMedianImageFilter::ComputeMedianAt(int x, int y, double & fRedValue, double & fGreenValue, double & fBlueValue)
{
	m_vRedPixelValues.resize(0);
	m_vGreenPixelValues.resize(0);
	m_vBluePixelValues.resize(0);

	for (int i = std::max(0, x - m_lFilterSize); i < std::min(m_lWidth - 1, x + m_lFilterSize); i++)
	{
		for (int j = std::max(0, y - m_lFilterSize); j < std::min(m_lHeight - 1, y + m_lFilterSize); j++)
		{
			double				fRed, fGreen, fBlue;

			m_pInBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
			m_vRedPixelValues.push_back(fRed);
			m_vGreenPixelValues.push_back(fGreen);
			m_vBluePixelValues.push_back(fBlue);
		};
	};
	fRedValue	= Median(m_vRedPixelValues);
	fGreenValue = Median(m_vGreenPixelValues);
	fBlueValue	= Median(m_vBluePixelValues);
};

/* ------------------------------------------------------------------- */

std::shared_ptr<CMemoryBitmap> CMedianImageFilter::ApplyFilter(const CMemoryBitmap* pInBitmap, ProgressBase* pProgress)
{
	return GetFilteredImage(pInBitmap, m_lFilterSize, pProgress);
}

/* ------------------------------------------------------------------- */

void	CDirectionalImageFilter::GetValuesAlongAngle(const CMemoryBitmap* pInBitmap, int x, int y, double fAngle, std::vector<double>& vValues)
{
	for (int l = -m_lSize;l<=m_lSize;l++)
	{
		int				i, j;

		i = (double)x + (double)l * cos(fAngle) + 0.5;
		j = (double)y + (double)l * sin(fAngle) + 0.5;

		double				fValue;

		pInBitmap->GetPixel(i, j, fValue);

		if (fValue)
			vValues.push_back(fValue);
	}
}

/* ------------------------------------------------------------------- */

void	CDirectionalImageFilter::GetValuesAlongAngle(const CMemoryBitmap* pInBitmap, int x, int y, double fAngle, std::vector<double>& vRedValues, std::vector<double>& vGreenValues, std::vector<double>& vBlueValues)
{
	for (int l = -m_lSize;l<=m_lSize;l++)
	{
		int				i, j;

		i = (double)x + (double)l * cos(fAngle) + 0.5;
		j = (double)y + (double)l * sin(fAngle) + 0.5;

		double				fRed, fGreen, fBlue;

		pInBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

		if (fRed)
			vRedValues.push_back(fRed);
		if (fGreen)
			vGreenValues.push_back(fGreen);
		if (fBlue)
			vBlueValues.push_back(fBlue);
	}
}

/* ------------------------------------------------------------------- */

void CDirectionalImageFilter::InitFilterMatrix(CFilterMatrix & fm)
{
	fm.Init(m_lSize);

};

/* ------------------------------------------------------------------- */

std::shared_ptr<CMemoryBitmap> CDirectionalImageFilter::ApplyFilter(const CMemoryBitmap* pInBitmap, ProgressBase* pProgress)
{
	if (pInBitmap == nullptr)
		return std::shared_ptr<CMemoryBitmap>{};

	const int lWidth = pInBitmap->Width();
	const int lHeight = pInBitmap->Height();
	const bool monochrome = pInBitmap->IsMonochrome();

	if (pProgress)
		pProgress->Start2(lWidth);

	std::shared_ptr<CMemoryBitmap> pOutBitmap{ pInBitmap->Clone() };

	for (int i = 0; i < lWidth; i++)
	{
		for (int j = 0; j < lHeight; j++)
		{
			if (monochrome)
			{
				std::vector<double>		vValues;
				double					fValue,
										fMedian;

				pInBitmap->GetPixel(i, j, fValue);
				GetValuesAlongAngle(pInBitmap, i, j, m_fAngle, vValues);
				fMedian = Median(vValues);
				if (fValue > fMedian || !fValue)
					pOutBitmap->SetPixel(i, j, fMedian);
			}
			else
			{
				std::vector<double>		vRedValues;
				std::vector<double>		vGreenValues;
				std::vector<double>		vBlueValues;
				double					fRed,
										fGreen,
										fBlue,
										fRedMedian,
										fGreenMedian,
										fBlueMedian;

				pInBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
				GetValuesAlongAngle(pInBitmap, i, j, m_fAngle, vRedValues, vGreenValues, vBlueValues);
				fRedMedian		= Median(vRedValues);
				fGreenMedian	= Median(vGreenValues);
				fBlueMedian		= Median(vBlueValues);
				if (fRed > fRedMedian || !fRed)
					fRed = fRedMedian;
				if (fGreen > fGreenMedian || !fGreen)
					fGreen = fGreenMedian;
				if (fBlue > fBlueMedian || !fBlue)
					fBlue = fBlueMedian;

				pOutBitmap->SetPixel(i, j, fRed, fGreen, fBlue);
			}
		}

		if (pProgress)
			pProgress->Progress2(i+1);
	}

	if (pProgress)
		pProgress->End2();

	return pOutBitmap;
}

void CMedianImageFilter::SetBitmap(CMemoryBitmap* pBitmap)
{
	m_pInBitmap = pBitmap;
	if (pBitmap != nullptr)
	{
		m_bMonochrome = pBitmap->IsMonochrome();
		m_bCFA = pBitmap->IsCFA();
		m_lWidth = pBitmap->Width();
		m_lHeight = pBitmap->Height();
	}
}