#ifndef __REGISTERENGINE_H__
#define __REGISTERENGINE_H__

#include "Common.h"
#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "FrameInfo.h"
#include "Registry.h"
#include "StackingTasks.h"
#include "DSSTools.h"
#include "MatchingStars.h"
#include <set>
#include "Stars.h"
#include "Workspace.h"

/* ------------------------------------------------------------------- */

class CRegisterInfo
{
public :
	LONG			m_lNrStars;
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

class CSkyBackground
{
public :
	double				m_fLight;
	double				m_fRed,
						m_fGreen,
						m_fBlue;

private:
	void	CopyFrom(const CSkyBackground & right)
	{
		m_fLight	= right.m_fLight;
		m_fRed		= right.m_fRed;
		m_fGreen	= right.m_fGreen;
		m_fBlue		= right.m_fBlue;
	};

public:
	CSkyBackground()
	{
		m_fLight = 0;
		m_fRed = m_fGreen = m_fBlue = 0;
	};
	~CSkyBackground()
	{
	};

	CSkyBackground(const CSkyBackground & right)
	{
		CopyFrom(right);
	};

	CSkyBackground & operator = (const CSkyBackground & right)
	{
		CopyFrom(right);
		return (*this);
	};

	bool operator < (const CSkyBackground & right) const
	{
		return m_fLight < right.m_fLight;
	};

	void	Reset()
	{
		m_fLight = 0;
		m_fRed = m_fGreen = m_fBlue = 0;
	};
};

/* ------------------------------------------------------------------- */

class CRegisteredFrame
{
public :
	STARVECTOR		m_vStars;
	STARSET			m_sStars;
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
		m_sStars				= rf.m_sStars;
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

	void	Reset()
	{
		CWorkspace			workspace;
		DWORD				dwThreshold = 10;

		m_vStars.clear();
		m_sStars.clear();

		m_fRoundnessTolerance = 2.0;
		m_bInfoOk = false;

		m_bComet = false;
		m_fXComet = m_fYComet = -1;

		m_fMinLuminancy = workspace.value("Register/DetectionThreshold").toDouble() / 100.0;

		m_bApplyMedianFilter = workspace.value("Register/ApplyMedianFilter").toBool();
		m_fBackground = 0.0;

		m_SkyBackground.Reset();

        m_fOverallQuality = 0;
        m_fFWHM = 0;
	};

	bool	FindStarShape(CMemoryBitmap * pBitmap, CStar & star);

	void	ComputeOverallQuality()
	{
		m_fOverallQuality = 0.0;
		for (LONG i = 0;i<m_vStars.size();i++)
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

	void	GetStars(STARVECTOR & vStars)
	{
		vStars = m_vStars;
	};

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
		for (LONG i = 0;i<m_vStars.size();i++)
			vFWHM.push_back(m_vStars[i].m_fMeanRadius * 2.35/1.5);

		if (vFWHM.size())
		{
			// m_fFWHM = Median(vFWHM);
			m_fFWHM = Average(vFWHM);
		};
	};

	bool	IsRegistered()
	{
		return m_bInfoOk;
	};


//	void	RegisterPicture(CMemoryBitmap * pBitmap);
	bool	ComputeStarCenter(CMemoryBitmap * pBitmap, double & fX, double & fY, double & fRadius);
	void	RegisterSubRect(CMemoryBitmap * pBitmap, CRect & rc);

	bool	SaveRegisteringInfo(LPCTSTR szInfoFileName);
	bool	LoadRegisteringInfo(LPCTSTR szInfoFileName);
};

/* ------------------------------------------------------------------- */

class CLightFrameInfo : public CFrameInfo,
						public CRegisteredFrame
{
public :
	CString			m_strInfoFileName;
	bool			m_bStartingFrame;
	bool			m_bTransformedCometPosition;

	CBilinearParameters	m_BilinearParameters;
	VOTINGPAIRVECTOR	m_vVotedPairs;

	double			m_fXOffset,
					m_fYOffset;
	double			m_fAngle;

	bool			m_bDisabled;
	CDSSProgress *	m_pProgress;

	bool			m_bRemoveHotPixels;

private :
	void CopyFrom(const CLightFrameInfo & cbi)
	{
		CFrameInfo::CopyFrom(cbi);
		CRegisteredFrame::CopyFrom(cbi);

		m_bStartingFrame  = cbi.m_bStartingFrame;
		m_strInfoFileName = cbi.m_strInfoFileName;
		m_bTransformedCometPosition = cbi.m_bTransformedCometPosition;

		m_fXOffset		  = cbi.m_fXOffset;
		m_fYOffset		  = cbi.m_fYOffset;
		m_fAngle		  = cbi.m_fAngle;
		m_BilinearParameters = cbi.m_BilinearParameters;
		m_vVotedPairs	  = cbi.m_vVotedPairs;

		m_bDisabled		  = cbi.m_bDisabled;
		m_pProgress		  = cbi.m_pProgress;
		m_bRemoveHotPixels= cbi.m_bRemoveHotPixels;
	};

	void	Reset()
	{
		CFrameInfo::Reset();
		CRegisteredFrame::Reset();

		m_fXOffset = 0;
		m_fYOffset = 0;
		m_fAngle   = 0;
		m_bDisabled= false;
		m_pProgress= nullptr;
		m_bStartingFrame  = false;
		m_vVotedPairs.clear();

		m_bTransformedCometPosition = false;

		CWorkspace			workspace;
	
		m_bRemoveHotPixels = workspace.value("Register/DetectHotPixels", false).toBool();
	};

public :
	CLightFrameInfo()
	{
		Reset();
	};
	CLightFrameInfo(const CLightFrameInfo & cbi)
	{
		CopyFrom(cbi);
	};

	CLightFrameInfo(const CFrameInfo & cbi)
	{
        Reset();

		CFrameInfo::CopyFrom(cbi);
	};

	CLightFrameInfo & operator = (const CLightFrameInfo & cbi)
	{
		CopyFrom(cbi);
		return (*this);
	};

	CLightFrameInfo & operator = (const CFrameInfo & cbi)
	{
		CFrameInfo::CopyFrom(cbi);
		return (*this);
	};

	void	SetHotPixelRemoval(bool bHotPixels)
	{
		m_bRemoveHotPixels = bHotPixels;
	};

	void	SetProgress(CDSSProgress * pProgress)
	{
		m_pProgress = pProgress;
	};

	void	SetBitmap(LPCTSTR szBitmap, bool bProcessIfNecessary = true, bool bForceRegister = false);

	bool operator < (const CLightFrameInfo & cbi) const
	{
		if (m_bStartingFrame)
			return true;
		else if (cbi.m_bStartingFrame)
			return false;
		else if (m_fOverallQuality > cbi.m_fOverallQuality)
			return true;
		else
			return false;
	};

	void	RegisterPicture(CMemoryBitmap * pBitmap);
	void	RegisterPicture(LPCTSTR szBitmap, double fMinLuminancy = 0.10, bool bRemoveHotPixels = true, bool bApplyMedianFilter = false, CDSSProgress * pProgress = nullptr);
	void	SaveRegisteringInfo();

private :
	bool	ReadInfoFileName();
	void	RegisterPicture();
	double	ComputeMedianValue(CGrayBitmap & Bitmap);
	void	RegisterPicture(CGrayBitmap & Bitmap);
	bool	ComputeStarShifts(CMemoryBitmap * pBitmap, CStar & star, double & fRedXShift, double & fRedYShift, double & fBlueXShift, double & fBlueYShift);
	void	ComputeLuminanceBitmap(CMemoryBitmap * pBitmap, CGrayBitmap ** ppGrayBitmap);
};

/* ------------------------------------------------------------------- */

class CScoredLightFrame
{
public :
	DWORD				m_dwIndice;
	double				m_fScore;

private :
	void	CopyFrom(const CScoredLightFrame & slf)
	{
		m_dwIndice = slf.m_dwIndice;
		m_fScore   = slf.m_fScore;
	};

public :
	CScoredLightFrame(DWORD dwIndice, double fScore)
	{
		m_dwIndice = dwIndice;
		m_fScore   = fScore;
	};

	CScoredLightFrame(const CScoredLightFrame & slf)
	{
		CopyFrom(slf);
	};

	const CScoredLightFrame & operator = (const CScoredLightFrame & slf)
	{
		CopyFrom(slf);
		return (*this);
	};

	bool operator < (const CScoredLightFrame & slf) const
	{
		return (m_fScore > slf.m_fScore);
	};
};

/* ------------------------------------------------------------------- */

typedef std::vector<CLightFrameInfo>	LIGHTFRAMEINFOVECTOR;

/* ------------------------------------------------------------------- */

class CRegisterEngine
{
private :
	bool						m_bSaveCalibrated;
	INTERMEDIATEFILEFORMAT		m_IntermediateFileFormat;
	bool						m_bSaveCalibratedDebayered;

private :
	bool	SaveCalibratedLightFrame(CLightFrameInfo & lfi, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, CString & strCalibratedFile);

public :
	CRegisterEngine()
	{
		m_bSaveCalibrated			= CAllStackingTasks::GetSaveCalibrated();
		m_IntermediateFileFormat	= CAllStackingTasks::GetIntermediateFileFormat();
		m_bSaveCalibratedDebayered	= CAllStackingTasks::GetSaveCalibratedDebayered();
	};

	virtual ~CRegisterEngine()
	{
	};

	bool	RegisterLightFrames(CAllStackingTasks & tasks, bool bForceRegister, CDSSProgress * pProgress);
};

/* ------------------------------------------------------------------- */

#endif // __REGISTERENGINE_H__
