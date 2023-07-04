#include <stdafx.h>
#include "DeepStack.h"
#include "Histogram.h"
#include "Ztrace.h"
#include "Multitask.h"
#include "DSSProgress.h"

using namespace DSS;

void CDeepStack::ComputeOriginalHistogram(CRGBHistogram & Histo)
{
	ZFUNCTRACE_RUNTIME();
	double fMax = 0;
	const size_t width = GetWidth();
	const int height = GetHeight();
	const int nrEnabledThreads = CMultitask::GetNrProcessors(); // Returns 1 if multithreading disabled by user, otherwise # HW threads
	float maxValue = 0;
	const float scalingFactor = 255.0f / m_StackedBitmap.GetNrStackedFrames();
	const auto& redPixels = m_StackedBitmap.getRedPixels();
	const auto& greenPixels = m_StackedBitmap.getGreenPixels();
	const auto& bluePixels = m_StackedBitmap.getBluePixels();

	Histo.Clear();

#pragma omp parallel default(none) firstprivate(maxValue) shared(fMax, redPixels, greenPixels, bluePixels) if(nrEnabledThreads - 1)
	{
#pragma omp for schedule(guided, 50)
		for (int row = 0; row < height; ++row)
		{
			size_t ndx = row * width;
			if (m_StackedBitmap.IsMonochrome())
			{
				for (size_t col = 0; col < width; ++col, ++ndx)
					maxValue = std::max(maxValue, redPixels[ndx]);
			}
			else
			{
				for (size_t col = 0; col < width; ++col, ++ndx)
				{
					const float red = redPixels[ndx];
					const float green = greenPixels[ndx];
					const float blue = bluePixels[ndx];
					maxValue = std::max(maxValue, std::max(red, std::max(green, blue)));
				};
			};
		};

#pragma omp critical(OrigHistoCalcOmpCrit)
		fMax = std::max(fMax, static_cast<double>(maxValue * scalingFactor));
	}

	Histo.SetSize(fMax, 65535);

	if (!m_StackedBitmap.IsMonochrome())
	{
#pragma omp parallel sections default(none) shared(Histo, redPixels, greenPixels, bluePixels) if(nrEnabledThreads - 1)
		{
#pragma omp section
			for (const auto& color: redPixels)
				Histo.GetRedHistogram().AddValue(color * scalingFactor);
#pragma omp section
			for (const auto& color: greenPixels)
				Histo.GetGreenHistogram().AddValue(color * scalingFactor);
#pragma omp section
			for (const auto& color: bluePixels)
				Histo.GetBlueHistogram().AddValue(color * scalingFactor);
		}
	}
	else
	{
		for (const auto& color: redPixels)
			Histo.GetRedHistogram().AddValue(color * scalingFactor);
		Histo.GetGreenHistogram() = Histo.GetRedHistogram();
		Histo.GetBlueHistogram() = Histo.GetRedHistogram();
	}
};

/* ------------------------------------------------------------------- */

void CDeepStack::AdjustHistogram(CRGBHistogram & srcHisto, CRGBHistogram & tgtHisto, const CRGBHistogramAdjust & HistogramAdjust)
{
	ZFUNCTRACE_RUNTIME();
	tgtHisto.Clear();
	bool				bMonochrome;

	bMonochrome = m_StackedBitmap.IsMonochrome();

	for (int i = 0;i<srcHisto.GetSize();i++)
	{
		double			fRed,
						fGreen,
						fBlue;

		fRed	= srcHisto.GetRedHistogram().GetComponentValue(i);

		if (!bMonochrome)
		{
			fGreen	= srcHisto.GetGreenHistogram().GetComponentValue(i);
			fBlue	= srcHisto.GetBlueHistogram().GetComponentValue(i);
			HistogramAdjust.Adjust(fRed, fGreen, fBlue);
		}
		else
		{
			fGreen = fBlue = fRed;
			HistogramAdjust.Adjust(fRed, fGreen, fBlue);
			fGreen = fBlue = fRed;
		};

		tgtHisto.GetRedHistogram().AddValue(fRed, srcHisto.GetRedHistogram().GetValue(i));
		tgtHisto.GetGreenHistogram().AddValue(fGreen, srcHisto.GetGreenHistogram().GetValue(i));
		tgtHisto.GetBlueHistogram().AddValue(fBlue, srcHisto.GetBlueHistogram().GetValue(i));
	};
};

/* ------------------------------------------------------------------- */

bool CDeepStack::LoadStackedInfo(LPCTSTR szStackedInfoFile)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult;

	bResult = m_StackedBitmap.Load(szStackedInfoFile, m_pProgress);

	if (bResult)
		ComputeOriginalHistogram(m_OriginalHisto);

	return bResult;
};

/* ------------------------------------------------------------------- */
