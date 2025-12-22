#pragma once
#include "BackgroundCalibration.h"
#include "MatchingStars.h"

namespace DSS { class OldProgressBase; }

class CMemoryBitmap;
class CLightFrameInfo;

class CRunningStackingEngine
{
private:
	std::shared_ptr<CMemoryBitmap> m_pStackedBitmap{};
	std::shared_ptr<CMemoryBitmap> m_pPublicBitmap{};
	std::unique_ptr<BackgroundCalibrator> backgroundCalibration{};
	int m_lNrStacked{ 0 };
	double m_fTotalExposure{ 0 };
	CMatchingStars m_MatchingStars{};

private:
	void CreatePublicBitmap();

public:
	CRunningStackingEngine() = default;
	~CRunningStackingEngine() = default;

	bool ComputeOffset(CLightFrameInfo& lfi);
	bool AddImage(CLightFrameInfo& lfi, DSS::OldProgressBase* pProgress);
	std::shared_ptr<CMemoryBitmap> getStackedImage()
	{
		return m_pPublicBitmap;
	}

	int	GetNrStackedImages() const
	{
		return m_lNrStacked;
	}

	double GetTotalExposure() const
	{
		return m_fTotalExposure;
	}

	void Clear()
	{
		m_pStackedBitmap.reset();
		m_pPublicBitmap.reset();
		m_lNrStacked = 0;
		m_fTotalExposure = 0;
	}
};

