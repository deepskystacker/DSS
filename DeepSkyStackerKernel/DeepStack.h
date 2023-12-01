#pragma once
#include "StackedBitmap.h"
#include "BitmapExt.h"

namespace DSS { class ProgressBase; }
class CDeepStack
{
private :
	DSS::StackedBitmap m_StackedBitmap;
	DSS::RGBHistogram m_OriginalHisto;
	C32BitsBitmap m_Bitmap;
	bool m_bNewStackedBitmap;
	DSS::ProgressBase* m_pProgress;

public :
	CDeepStack()
	{
		m_bNewStackedBitmap = false;
		m_pProgress			= nullptr;
	};
	virtual ~CDeepStack() {};

	void	Clear()
	{
		m_StackedBitmap.Clear();
		m_Bitmap.Free();
		m_OriginalHisto.clear();
		m_bNewStackedBitmap = false;
	};

	int	GetWidth()
	{
		return m_StackedBitmap.GetWidth();
	};

	int	GetHeight()
	{
		return m_StackedBitmap.GetHeight();
	};

private :
	void	ComputeOriginalHistogram(DSS::RGBHistogram & Histo);
	void	AdjustHistogram(DSS::RGBHistogram & srcHisto, DSS::RGBHistogram & tgtHisto, const DSS::RGBHistogramAdjust & histogramAdjust);

public :
	void	SetProgress(DSS::ProgressBase *	pProgress)
	{
		m_pProgress = pProgress;
	};

	bool	IsNewStackedBitmap(bool bReset = false)
	{
		bool			bResult = m_bNewStackedBitmap;

		if (bReset)
			m_bNewStackedBitmap = false;

		return bResult;
	};

	bool	LoadStackedInfo(const fs::path& file);

	HBITMAP PartialProcess(RECT rcProcess, const DSS::BezierAdjust & BezierAdjust, const DSS::RGBHistogramAdjust & histogramAdjust)
	{
		if (m_Bitmap.IsEmpty())
			m_Bitmap.Create(GetWidth(), GetHeight());

		m_StackedBitmap.SetBezierAdjust(BezierAdjust);
		m_StackedBitmap.SetHistogramAdjust(histogramAdjust);
		return m_StackedBitmap.GetHBitmap(m_Bitmap, &rcProcess);
	};

	DSS::StackedBitmap& GetStackedBitmap()
	{
		return m_StackedBitmap;
	}

	C32BitsBitmap& GetBitmap()
	{
		return m_Bitmap;
	}

	void AdjustOriginalHistogram(DSS::RGBHistogram & Histo, const DSS::RGBHistogramAdjust & histogramAdjust)
	{
		if (!m_OriginalHisto.IsInitialized())
			ComputeOriginalHistogram(m_OriginalHisto);

		AdjustHistogram(m_OriginalHisto, Histo, histogramAdjust);
	};

	DSS::RGBHistogram & GetOriginalHistogram()
	{
		if (!m_OriginalHisto.IsInitialized())
			ComputeOriginalHistogram(m_OriginalHisto);

		return m_OriginalHisto;
	};

	bool	IsLoaded()
	{
		return GetWidth() && GetHeight();
	};
};
