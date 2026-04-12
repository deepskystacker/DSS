
#pragma once
#include <zexcept.h>
#include <filesystem>
#include <QImage>

namespace fs = std::filesystem;

#include "StackedBitmap.h"
#include "BitmapExt.h"

namespace DSS { class OldProgressBase; }
class DeepStack
{
private :
	DSS::StackedBitmap stackedBitmap;
	DSS::RGBHistogram histogram;
	QVector<uchar> imageData_;
	std::unique_ptr<QImage> image_;
	DSS::OldProgressBase* m_pProgress;

	//
	// Copies of the values for the stretch parameters used to restore them on undo.
	//
	double asinhBeta_{ 0.0 };	// Asinh stretch value
	double asinhBP_{ 0.0 };		// Asinh black point value
	bool asinhHWLuminance_{ true };	// Whether to use human weighted luminance for asinh stretch
	bool preview_{ true };		// Whether to show a preview of the processed image

public :
	DeepStack() : 
		m_pProgress{nullptr}
	{
	}
	virtual ~DeepStack() {}

	DeepStack(const DeepStack& rhs)
	{
		stackedBitmap = rhs.stackedBitmap;
		imageData_.clear();
		m_pProgress = rhs.m_pProgress;
	}

	DeepStack& operator=(const DeepStack& rhs)
	{
		if (this != &rhs)
		{
			stackedBitmap = rhs.stackedBitmap;
			imageData_.clear();
			m_pProgress = rhs.m_pProgress;
		}
		return *this;
	}

	void	reset()
	{
		stackedBitmap.Clear();
		image_.reset();
		imageData_.clear();
		histogram.clear();
	}

	int	GetWidth()
	{
		return stackedBitmap.GetWidth();
	}

	int	GetHeight()
	{
		return stackedBitmap.GetHeight();
	}

	const auto& getHistogram() const
	{
		return histogram;
	}

	inline void	computeHistogram()
	{
		computeHistogram(histogram);
	}

	void computeDisplayHistogram(DSS::RGBHistogram& displayHisto);

private :
	void computeHistogram(DSS::RGBHistogram& Histo);

public :
	void	SetProgress(DSS::OldProgressBase *	pProgress)
	{
		m_pProgress = pProgress;
	}

	bool	LoadStackedInfo(const fs::path& file);

	void PartialProcess(DSSRect& rcProcess)
	{
		//
		// Initialise an empty QImage of the right size if necessary using a preallocated buffer (in imageData_)
		//
		if (nullptr == image_.get())
		{
			//int width = GetWidth(); int height = GetHeight();
			//qDebug() << "Creating image data storage: " << width << "*" << height;
			imageData_.resize(GetWidth() * GetHeight() * sizeof(QRgb));
			image_ = std::make_unique<QImage>(imageData_.data(), GetWidth(), GetHeight(), QImage::Format_RGB32);
		}

		stackedBitmap.updateQImage(imageData_.data(), image_->bytesPerLine(), rcProcess);
		
	}

	DSS::StackedBitmap& GetStackedBitmap()
	{
		return stackedBitmap;
	}

	const QImage& getImage() const
	{
		return *image_;
	}

	bool	IsLoaded()
	{
		return GetWidth() && GetHeight();
	}

	DeepStack& saveSettings(double beta, double bp, bool hwl, bool pv)
	{
		this->asinhBeta_ = beta;
		this->asinhBP_ = bp;
		this->asinhHWLuminance_ = hwl;
		this->preview_ = pv;
		return *this;
	}

	std::tuple<double, double, bool, bool> savedSettings() const
	{
		return std::make_tuple(asinhBeta_, asinhBP_, asinhHWLuminance_, preview_);
	}
};
