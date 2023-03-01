#ifndef __RUNNINGSTACKINGENGINE_H__
#define __RUNNINGSTACKINGENGINE_H__

#include "RegisterEngine.h"
#include "PixelTransform.h"
#include "BackgroundCalibration.h"

class CRunningStackingEngine
{
private:
	std::shared_ptr<CMemoryBitmap> m_pStackedBitmap;
	std::shared_ptr<CMemoryBitmap> m_pPublicBitmap;
	CBackgroundCalibration m_BackgroundCalibration;
	int m_lNrStacked;
	double m_fTotalExposure;
	CMatchingStars m_MatchingStars;

private:
	void CreatePublicBitmap();

public:
	CRunningStackingEngine();
	~CRunningStackingEngine() = default;

	bool ComputeOffset(CLightFrameInfo& lfi);
	bool AddImage(CLightFrameInfo& lfi, ProgressBase* pProgress);
	bool GetStackedImage(std::shared_ptr<CMemoryBitmap>& rpBitmap)
	{
		rpBitmap = m_pPublicBitmap;
		return static_cast<bool>(m_pPublicBitmap);
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

#endif
