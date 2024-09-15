
#pragma once
#include <zexcept.h>
#include <filesystem>
#include <QImage>

namespace fs = std::filesystem;

#include "StackedBitmap.h"
#include "BitmapExt.h"

namespace DSS { class ProgressBase; }
class CDeepStack
{
private :
	DSS::StackedBitmap m_StackedBitmap;
	DSS::RGBHistogram m_OriginalHisto;
	QVector<uchar> imageData_;
	std::unique_ptr<QImage> image_;
	bool m_bNewStackedBitmap;
	DSS::ProgressBase* m_pProgress;

public :
	CDeepStack() : 
		m_bNewStackedBitmap{false},
		m_pProgress{nullptr}
	{
	};
	virtual ~CDeepStack() {};

	void	reset()
	{
		m_StackedBitmap.Clear();
		image_.reset();
		imageData_.clear();
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

	void PartialProcess(DSSRect& rcProcess, const DSS::BezierAdjust& BezierAdjust, const DSS::RGBHistogramAdjust& histogramAdjust)
	{
		//
		// Initialise an empty QImage of the right size if necessary using a preallocated buffer (in imageData_)
		//
		if (nullptr == image_.get())
		{
			int width = GetWidth(); int height = GetHeight();
			qDebug() << "Creating image data storage: " << width << "*" << height;
			imageData_.resize(GetWidth() * GetHeight() * sizeof(QRgb));
			image_ = std::make_unique<QImage>(imageData_.data(), GetWidth(), GetHeight(), QImage::Format_RGB32);
		}

		m_StackedBitmap.SetBezierAdjust(BezierAdjust);
		m_StackedBitmap.SetHistogramAdjust(histogramAdjust);
		m_StackedBitmap.updateQImage(imageData_.data(), image_->bytesPerLine(), &rcProcess);
		
	}

	DSS::StackedBitmap& GetStackedBitmap()
	{
		return m_StackedBitmap;
	}

	const QImage& getImage() const
	{
		return *image_;
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
