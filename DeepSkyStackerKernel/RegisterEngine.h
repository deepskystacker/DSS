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

class CRegisterInfo
{
public :
	int			m_lNrStars;
	double			m_fMinLuminancy;
	double			m_fOverallQuality;

private :
	void	CopyFrom(const CRegisterInfo & ri)
	{
		m_lNrStars		= ri.m_lNrStars;
		m_fMinLuminancy = ri.m_fMinLuminancy;
		m_fOverallQuality = ri.m_fOverallQuality;
	};

public :
	CRegisterInfo()
	{
		m_lNrStars			= 0;
		m_fMinLuminancy		= 0;
		m_fOverallQuality	= 0;
	};

	CRegisterInfo(const CRegisterInfo & ri)
	{
		CopyFrom(ri);
	};

	virtual ~CRegisterInfo()
	{
	};

	const CRegisterInfo & operator = (const CRegisterInfo & ri)
	{
		CopyFrom(ri);
		return (*this);
	};

	bool operator < (const CRegisterInfo & ri) const
	{
		return m_fMinLuminancy < ri.m_fMinLuminancy;
	};
};

typedef std::vector<CRegisterInfo>		REGISTERINFOVECTOR;

/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */

class CRegisteredFrame
{
public :
	STARVECTOR		m_vStars;
	double			m_fRoundnessTolerance;
	double			m_fMinLuminancy;
	bool			m_bApplyMedianFilter;
	double			m_fBackground;
	double			m_fOverallQuality;
	double			m_fFWHM;
	bool			m_bInfoOk;
	bool			m_bComet;
	double			m_fXComet,
					m_fYComet;
	CSkyBackground	m_SkyBackground;

protected :
	void	CopyFrom(const CRegisteredFrame & rf)
	{
		m_vStars				= rf.m_vStars;
		m_fRoundnessTolerance	= rf.m_fRoundnessTolerance;
		m_fMinLuminancy			= rf.m_fMinLuminancy;
		m_bApplyMedianFilter	= rf.m_bApplyMedianFilter;
		m_fBackground			= rf.m_fBackground;
		m_fOverallQuality		= rf.m_fOverallQuality;
		m_fFWHM					= rf.m_fFWHM;
		m_bComet				= rf.m_bComet;
		m_fXComet				= rf.m_fXComet;
		m_fYComet				= rf.m_fYComet;
		m_bInfoOk				= rf.m_bInfoOk;
		m_SkyBackground			= rf.m_SkyBackground;
	};

	void	Reset();
	bool FindStarShape(CMemoryBitmap* pBitmap, CStar& star);

	void	ComputeOverallQuality()
	{
		m_fOverallQuality = 0.0;
		for (STARVECTOR::size_type i = 0;i<m_vStars.size();i++)
			m_fOverallQuality += m_vStars[i].m_fQuality;
	};

public :
	CRegisteredFrame()
	{
		Reset();
	};

	virtual ~CRegisteredFrame()
	{
	};

	void	SetDetectionThreshold(double fMinLuminancy)
	{
		m_fMinLuminancy = fMinLuminancy;
	};

	void	SetRoundnessTolerance(double fTolerance)
	{
		m_fRoundnessTolerance = fTolerance;
	};

	STARVECTOR GetStars() const
	{
		return m_vStars;
	}

	void	SetStars(const STARVECTOR & vStars)
	{
		m_vStars = vStars;
		ComputeOverallQuality();
		ComputeFWHM();
	};

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

	bool ComputeStarCenter(CMemoryBitmap* pBitmap, double& fX, double& fY, double& fRadius);
	size_t RegisterSubRect(CMemoryBitmap* pBitmap, const DSSRect& rc, STARSET& stars);

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

	void SetBitmap(fs::path path, bool bProcessIfNecessary = true, bool bForceRegister = false);

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

	void RegisterPicture(CMemoryBitmap* pBitmap);
	void RegisterPicture(const fs::path& bitmap, double fMinLuminancy = 0.10, bool bRemoveHotPixels = true, bool bApplyMedianFilter = false, DSS::ProgressBase* pProgress = nullptr);
	void SaveRegisteringInfo();

private:
	bool ReadInfoFileName();
	void RegisterPicture();
	void RegisterPicture(CGrayBitmap& Bitmap);
	double ComputeMedianValue(CGrayBitmap& Bitmap);
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

