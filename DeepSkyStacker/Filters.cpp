#include <stdafx.h>
#include "Filters.h"

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

void	CExtendedMedianImageFilter::AnalyzeImage(CMemoryBitmap * pInBitmap, BOOL bComputeThresholds)
{
	ZFUNCTRACE_RUNTIME();
	LONG				lWidth = pInBitmap->Width(),
						lHeight = pInBitmap->Height();
	BOOL				bMonochrome = pInBitmap->IsMonochrome();

	std::vector<double>		vGrays,
							vReds,
							vGreens,
							vBlues;

	vGrays.resize(8);
	vReds.resize(8);
	vGreens.resize(8);
	vBlues.resize(8);

	std::vector<LONG>		vHotPixels,
							vColdPixels;

	vHotPixels.resize(1001);
	vColdPixels.resize(1001);

	for (LONG j= 1;j<lHeight-1;j++)
	{
		for (LONG i = 1;i<lWidth-1;i++)
		{
			double				fPosition;
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
				double				fPosition;

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
		LONG	lNrPixels = lWidth*lHeight;
		LONG	lNrRejectedHotPixels = lNrPixels * m_fRejectHotThreshold;
		LONG	lNrRejectedColdPixels = lNrPixels * m_fRejectColdThreshold;

		LONG	lThreshold;

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

inline void	CheckPixel(LONG X, LONG Y, EXCLUDEDPIXELVECTOR &vExcluded, EXCLUDEDPIXELVECTOR & vOkPixels)
{
	CExcludedPixel		TestPixel(X, Y);

	if (!binary_search(vExcluded.begin(), vExcluded.end(), TestPixel))
		vOkPixels.push_back(TestPixel);
};


/* ------------------------------------------------------------------- */

void	CExtendedMedianImageFilter::ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap * pOutBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	EXCLUDEDPIXELVECTOR		vExcluded = m_vExcludedPixels;
	LONG					lNrOkNeighbors = 8;
	BOOL					bMonochrome = pInBitmap->IsMonochrome();

	std::sort(vExcluded.begin(), vExcluded.end());
	while (vExcluded.size() && lNrOkNeighbors)
	{
		// Check that at least 8 pixels are ok
		EXCLUDEDPIXELVECTOR		vAuxExcluded;

		for (LONG k = 0;k<vExcluded.size();k++)
		{
			LONG				lNrNeighbors = 0;
			CExcludedPixel &	Pixel = vExcluded[k];
			CExcludedPixel		TestPixel;
			EXCLUDEDPIXELVECTOR	vOkPixels;

			CheckPixel(Pixel.X-1, Pixel.Y-1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X-1, Pixel.Y  , vExcluded, vOkPixels);
			CheckPixel(Pixel.X-1, Pixel.Y+1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X,   Pixel.Y-1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X,   Pixel.Y+1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X+1, Pixel.Y-1, vExcluded, vOkPixels);
			CheckPixel(Pixel.X+1, Pixel.Y  , vExcluded, vOkPixels);
			CheckPixel(Pixel.X+1, Pixel.Y-1, vExcluded, vOkPixels);

			if (vOkPixels.size()>=lNrOkNeighbors)
			{
				// Interpolate with the good values
				double			fSumGray = 0,
								fSumRed  = 0,
								fSumGreen= 0,
								fSumBlue = 0;

				for (LONG l = 0;l<vOkPixels.size();l++)
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
				};
			}
			else
				vAuxExcluded.push_back(Pixel);
		};

		if (vExcluded.size() == vAuxExcluded.size())
			lNrOkNeighbors--;
		vExcluded = vAuxExcluded;
	};
};

/* ------------------------------------------------------------------- */

void	CExtendedMedianImageFilter::ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	if (m_bUseRejectThreshold)
		AnalyzeImage(pInBitmap, TRUE);
	AnalyzeImage(pInBitmap, FALSE);

	CSmartPtr<CMemoryBitmap>	pOutBitmap;

	pOutBitmap.Attach(pInBitmap->Clone());
	ApplyFilter(pInBitmap, pOutBitmap, pProgress);

	pOutBitmap.CopyTo(ppOutBitmap);
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	CMedianImageFilter::ComputeMedianAt(LONG x, LONG y, double & fGrayValue, BAYERCOLOR BayerColor)
{
	std::vector<double>			vValues;

	m_vGrayPixelValues.resize(0);
	if (m_bCFA && BayerColor != BAYER_UNKNOWN)
	{
		for (LONG i = max(0L, x-2*m_lFilterSize);i<min(m_lWidth-1, x+2*m_lFilterSize);i++)
		{
			for (LONG j = max(0L, y-2*m_lFilterSize);j<min(m_lHeight-1, y+2*m_lFilterSize);j++)
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
		for (LONG i = max(0L, x-m_lFilterSize);i<min(m_lWidth-1, x+m_lFilterSize);i++)
		{
			for (LONG j = max(0L, y-m_lFilterSize);j<min(m_lHeight-1, y+m_lFilterSize);j++)
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

void	CMedianImageFilter::ComputeMedianAt(LONG x, LONG y, double & fRedValue, double & fGreenValue, double & fBlueValue)
{
	m_vRedPixelValues.resize(0);
	m_vGreenPixelValues.resize(0);
	m_vBluePixelValues.resize(0);

	for (LONG i = max(0L, x-m_lFilterSize);i<min(m_lWidth-1, x+m_lFilterSize);i++)
	{
		for (LONG j = max(0L, y-m_lFilterSize);j<min(m_lHeight-1, y+m_lFilterSize);j++)
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

void	CMedianImageFilter::ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, CDSSProgress * pProgress)
{
	GetFilteredImage(pInBitmap, ppOutBitmap, m_lFilterSize, pProgress);
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	CDirectionalImageFilter::GetValuesAlongAngle(LONG x, LONG y, double fAngle, std::vector<double> & vValues)
{
	for (LONG l = -m_lSize;l<=m_lSize;l++)
	{
		LONG				i, j;

		i = (double)x + (double)l * cos(fAngle) + 0.5;
		j = (double)y + (double)l * sin(fAngle) + 0.5;

		double				fValue;

		m_pInBitmap->GetPixel(i, j, fValue);

		if (fValue)
			vValues.push_back(fValue);
	};
};

/* ------------------------------------------------------------------- */

void	CDirectionalImageFilter::GetValuesAlongAngle(LONG x, LONG y, double fAngle, std::vector<double> & vRedValues, std::vector<double> & vGreenValues, std::vector<double> & vBlueValues)
{
	for (LONG l = -m_lSize;l<=m_lSize;l++)
	{
		LONG				i, j;

		i = (double)x + (double)l * cos(fAngle) + 0.5;
		j = (double)y + (double)l * sin(fAngle) + 0.5;

		double				fRed, fGreen, fBlue;

		m_pInBitmap->GetPixel(i, j, fRed, fGreen, fBlue);

		if (fRed)
			vRedValues.push_back(fRed);
		if (fGreen)
			vGreenValues.push_back(fGreen);
		if (fBlue)
			vBlueValues.push_back(fBlue);
	};
};

/* ------------------------------------------------------------------- */

void CDirectionalImageFilter::InitFilterMatrix(CFilterMatrix & fm)
{
	fm.Init(m_lSize);

};

/* ------------------------------------------------------------------- */

void	CDirectionalImageFilter::ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, CDSSProgress * pProgress)
{
	CSmartPtr<CMemoryBitmap>		pOutBitmap;

	if (ppOutBitmap)
		*ppOutBitmap = NULL;

	if (pInBitmap)
	{
		LONG							lWidth = pInBitmap->Width(),
										lHeight = pInBitmap->Height();
		m_pInBitmap = pInBitmap;
		m_bMonochrome = pInBitmap->IsMonochrome();
		pOutBitmap.Attach(pInBitmap->Clone());

		if (pProgress)
			pProgress->Start2(NULL, lWidth);

		for (LONG i  =0;i<lWidth;i++)
		{
			for (LONG j = 0;j<lHeight;j++)
			{
				if (m_bMonochrome)
				{
					std::vector<double>		vValues;
					double					fValue,
											fMedian;

					m_pInBitmap->GetPixel(i, j, fValue);
					GetValuesAlongAngle(i, j, m_fAngle, vValues);
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

					m_pInBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
					GetValuesAlongAngle(i, j, m_fAngle, vRedValues, vGreenValues, vBlueValues);
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
				};
			};
			if (pProgress)
				pProgress->Progress2(NULL, i+1);
		};

		if (pProgress)
			pProgress->End2();

		pOutBitmap.CopyTo(ppOutBitmap);
	};
};

/* ------------------------------------------------------------------- */
