#include <stdafx.h>
#include "DeepStack.h"
#include <math.h>
#include <tiffio.h>
#include <algorithm>
#include "MatchingStars.h"

#include "LogFile.h"

#define _USE_MATH_DEFINES
#include <cmath>

/* ------------------------------------------------------------------- */

void CDeepStack::ComputeOriginalHistogram(CRGBHistogram & Histo)
{
	double				fMax = 0;
	LONG				i, j;

	Histo.Clear();

	for (i = 0;i<GetWidth();i++)
		for (j = 0;j<GetHeight();j++)
		{
			fMax = max(fMax, m_StackedBitmap.GetRedValue(i, j));
			fMax = max(fMax, m_StackedBitmap.GetGreenValue(i, j));
			fMax = max(fMax, m_StackedBitmap.GetBlueValue(i, j));
		};

	Histo.SetSize(fMax, (LONG)65535);

	for (i = 0;i<GetWidth();i++)
		for (j = 0;j<GetHeight();j++)
			Histo.AddValues(m_StackedBitmap.GetRedValue(i, j), 
							m_StackedBitmap.GetGreenValue(i, j), 
							m_StackedBitmap.GetBlueValue(i, j));
};

/* ------------------------------------------------------------------- */

void CDeepStack::AdjustHistogram(CRGBHistogram & srcHisto, CRGBHistogram & tgtHisto, const CRGBHistogramAdjust & HistogramAdjust)
{
	tgtHisto.Clear();
	BOOL				bMonochrome;

	bMonochrome = m_StackedBitmap.IsMonochrome();
	
	for (LONG i = 0;i<srcHisto.GetSize();i++)
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

void CDeepStack::SaveStackedInfo(LPCTSTR szStackedInfoFile, LPRECT pRect)
{
	m_StackedBitmap.SaveDSImage(szStackedInfoFile, pRect, m_pProgress);
};

/* ------------------------------------------------------------------- */

BOOL CDeepStack::LoadStackedInfo(LPCTSTR szStackedInfoFile)
{
	BOOL				bResult;
	
	bResult = m_StackedBitmap.Load(szStackedInfoFile, m_pProgress);

	if (bResult)
		ComputeOriginalHistogram(m_OriginalHisto);

	return bResult;
};

/* ------------------------------------------------------------------- */
