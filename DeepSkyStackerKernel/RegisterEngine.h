#pragma once
#include "Stars.h"
#include "SkyBackground.h"
#include "FrameInfo.h"
#include "BilinearParameters.h"
#include "MatchingStars.h"
#include "DSSProgress.h"
#include "GrayBitmap.h"

namespace DSS { class ProgressBase; }

/* ------------------------------------------------------------------- */

class CRegisteredFrame
{
public:
	constexpr static double m_fRoundnessTolerance = 2.0;

	STARVECTOR		m_vStars;
	double			m_fMinLuminancy;
	bool			m_bApplyMedianFilter;
	double			m_fBackground;
	double			m_fOverallQuality;
	double			m_fFWHM;
	bool			m_bInfoOk;
	bool			m_bComet;
	double			m_fXComet;
	double			m_fYComet;
	CSkyBackground	m_SkyBackground;
protected:
	double usedDetectionThreshold{ 0.0 };

protected:
	void	Reset();
	bool FindStarShape(const CMemoryBitmap* pBitmap, CStar& star);

	void	ComputeOverallQuality()
	{
		m_fOverallQuality = 0.0;
		for (STARVECTOR::size_type i = 0;i<m_vStars.size();i++)
			m_fOverallQuality += m_vStars[i].m_fQuality;
	}

public:
	CRegisteredFrame()
	{
		Reset();
	}

	virtual ~CRegisteredFrame() = default;

	void	SetDetectionThreshold(double fMinLuminancy)
	{
		m_fMinLuminancy = fMinLuminancy;
	}

	STARVECTOR GetStars() const
	{
		return m_vStars;
	}

	void	SetStars(const STARVECTOR & vStars)
	{
		m_vStars = vStars;
		ComputeOverallQuality();
		ComputeFWHM();
	}

	void	ComputeFWHM()
	{
		std::vector<double>		vFWHM;

		// Compute FWHM
		m_fFWHM = 0.0;
		for (const auto& star : m_vStars)
			vFWHM.push_back(star.m_fMeanRadius * (2.35 / 1.5));

		if (!vFWHM.empty())
		{
			// m_fFWHM = Median(vFWHM);
			m_fFWHM = Average(vFWHM);
		}
	}

	bool IsRegistered() const
	{
		return m_bInfoOk;
	}

	size_t RegisterSubRect(const CGrayBitmap& inputBitmap, const double detectionThreshold, const DSSRect& rc, STARSET& stars, std::vector<double>& buffer);

	bool	SaveRegisteringInfo(const fs::path& szInfoFileName);
	bool	LoadRegisteringInfo(const fs::path& szInfoFileName);
};

/* ------------------------------------------------------------------- */

class CLightFrameInfo : public CFrameInfo,
                        public CRegisteredFrame
{
public:
	fs::path m_strInfoFileName;
	bool m_bStartingFrame;
	bool m_bTransformedCometPosition;

	CBilinearParameters m_BilinearParameters;
	VOTINGPAIRVECTOR m_vVotedPairs;

	double m_fXOffset;
	double m_fYOffset;
	double m_fAngle;

	bool m_bDisabled;
	DSS::ProgressBase* m_pProgress;

	bool m_bRemoveHotPixels;

private:
	void Reset();

public:
	CLightFrameInfo()
	{
		Reset();
	}

	CLightFrameInfo(const CLightFrameInfo&) = default;

	explicit CLightFrameInfo(const CFrameInfo& cbi)
	{
        Reset();
		CFrameInfo::CopyFrom(cbi);
	}

	explicit CLightFrameInfo(DSS::ProgressBase* const pPrg)
	{
		Reset();
		this->SetProgress(pPrg);
	}

	CLightFrameInfo& operator=(const CLightFrameInfo&) = default;

	CLightFrameInfo& operator=(const CFrameInfo& cbi)
	{
		CFrameInfo::operator=(cbi);
		return (*this);
	}

	void SetHotPixelRemoval(const bool bHotPixels)
	{
		m_bRemoveHotPixels = bHotPixels;
	}

	void SetProgress(DSS::ProgressBase* pProgress)
	{
		m_pProgress = pProgress;
	}

	void SetBitmap(fs::path path/*, bool bProcessIfNecessary, bool bForceRegister*/);

	bool operator<(const CLightFrameInfo& cbi) const
	{
		if (m_bStartingFrame)
			return true;
		else if (cbi.m_bStartingFrame)
			return false;
		else if (m_fOverallQuality > cbi.m_fOverallQuality)
			return true;
		else
			return false;
	}
public:
	void RegisterPicture(CMemoryBitmap* pBitmap, const int bitmapIndex);
	void RegisterPicture(const fs::path& bitmap, double fMinLuminancy, bool bRemoveHotPixels, bool bApplyMedianFilter, DSS::ProgressBase* pProgress);
	void SaveRegisteringInfo();

private:
	bool ReadInfoFileName();
	double RegisterPicture(const CGrayBitmap& Bitmap, const double threshold, const bool optimizeThreshold);
	double ComputeMedianValue(const CGrayBitmap& Bitmap);
	bool ComputeStarShifts(CMemoryBitmap * pBitmap, CStar & star, double & fRedXShift, double & fRedYShift, double & fBlueXShift, double & fBlueYShift);
	std::shared_ptr<CGrayBitmap> ComputeLuminanceBitmap(CMemoryBitmap* pBitmap);
};

/* ------------------------------------------------------------------- */

class CScoredLightFrame
{
public:
	std::uint32_t	m_dwIndice;
	double			m_fScore;

private:
	void CopyFrom(const CScoredLightFrame& slf)
	{
		m_dwIndice = slf.m_dwIndice;
		m_fScore   = slf.m_fScore;
	}

public:
	CScoredLightFrame(std::uint32_t dwIndice, double fScore) :
		m_dwIndice{ dwIndice },
		m_fScore{ fScore }
	{}

	CScoredLightFrame(const CScoredLightFrame& slf)
	{
		CopyFrom(slf);
	}

	const CScoredLightFrame& operator=(const CScoredLightFrame& slf)
	{
		CopyFrom(slf);
		return (*this);
	}

	bool operator<(const CScoredLightFrame& slf) const
	{
		return (m_fScore > slf.m_fScore);
	}
};

namespace DSS
{
	class ScoredLightFrame
	{
	public:
		std::uint16_t	group;
		std::uint32_t	index;
		double			score;


		//private:
		//	void CopyFrom(const CScoredLightFrame& slf)
		//	{
		//		m_dwIndice = slf.m_dwIndice;
		//		m_fScore   = slf.m_fScore;
		//	}

	public:
		ScoredLightFrame(std::uint16_t id, std::uint32_t ndx, double value) :
			group{ id },
			index{ ndx },
			score{ value }

		{}

		ScoredLightFrame(const ScoredLightFrame& rhs) = default;
		ScoredLightFrame(ScoredLightFrame&& rhs) = default;

		ScoredLightFrame& operator=(const ScoredLightFrame& rhs) = default;
		ScoredLightFrame& operator=(ScoredLightFrame&& rhs) = default;

		/// <summary>
		/// Implement operator < for std::sort.  Note the what is
		/// actually wanted is a reverse sort so we use > for the
		/// operator.
		/// </summary>
		/// <param name="rhs" >The comparand</param>
		/// <returns>true if greater than comparand </returns>
		bool operator<(const ScoredLightFrame& rhs) const
		{
			return (score > rhs.score);
		}
	};

}

/* ------------------------------------------------------------------- */

typedef std::vector<CLightFrameInfo>	LIGHTFRAMEINFOVECTOR;

/* ------------------------------------------------------------------- */
class CAllStackingTasks;
class CRegisterEngine
{
private :
	bool						m_bSaveCalibrated;
	INTERMEDIATEFILEFORMAT		m_IntermediateFileFormat;
	bool						m_bSaveCalibratedDebayered;

private :
	bool SaveCalibratedLightFrame(const CLightFrameInfo& lfi, std::shared_ptr<CMemoryBitmap> pBitmap, DSS::ProgressBase* pProgress, QString& strCalibratedFile);

public :
	CRegisterEngine();
	virtual ~CRegisterEngine() = default;

	void OverrideIntermediateFileFormat(INTERMEDIATEFILEFORMAT fmt) { m_IntermediateFileFormat = fmt; }
	bool	RegisterLightFrames(CAllStackingTasks & tasks, bool bForceRegister, DSS::ProgressBase* pProgress);
};

/* ------------------------------------------------------------------- */

