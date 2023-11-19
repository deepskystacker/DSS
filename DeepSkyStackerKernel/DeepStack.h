#pragma once
#include "StackedBitmap.h"
#include "BitmapExt.h"

namespace DSS { class ProgressBase; }
class CDeepStack
{
private :
	CStackedBitmap m_StackedBitmap;
	CRGBHistogram m_OriginalHisto;
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
		m_OriginalHisto.Clear();
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
	void	ComputeOriginalHistogram(CRGBHistogram & Histo);
	void	AdjustHistogram(CRGBHistogram & srcHisto, CRGBHistogram & tgtHisto, const CRGBHistogramAdjust & HistogramAdjust);

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

	HBITMAP PartialProcess(RECT rcProcess, const CBezierAdjust & BezierAdjust, const CRGBHistogramAdjust & HistogramAdjust)
	{
		if (m_Bitmap.IsEmpty())
			m_Bitmap.Create(GetWidth(), GetHeight());

		m_StackedBitmap.SetBezierAdjust(BezierAdjust);
		m_StackedBitmap.SetHistogramAdjust(HistogramAdjust);
		return m_StackedBitmap.GetHBitmap(m_Bitmap, &rcProcess);
	};

	CStackedBitmap& GetStackedBitmap()
	{
		return m_StackedBitmap;
	}

	C32BitsBitmap& GetBitmap()
	{
		return m_Bitmap;
	}

	void AdjustOriginalHistogram(CRGBHistogram & Histo, const CRGBHistogramAdjust & HistogramAdjust)
	{
		if (!m_OriginalHisto.IsInitialized())
			ComputeOriginalHistogram(m_OriginalHisto);

		AdjustHistogram(m_OriginalHisto, Histo, HistogramAdjust);
	};

	CRGBHistogram & GetOriginalHistogram()
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
