#pragma once
#include "dssrect.h"
#include "Stars.h"
#include "Bayer.h"

namespace DSS { class ProgressBase; }

/* ------------------------------------------------------------------- */

class CHotPixel
{
public:
	int m_lX;
	int m_lY;
	int m_lWeight;

public :
	CHotPixel(int X = 0, int Y = 0, int lWeight = 1) :
		m_lX{ X },
		m_lY{ Y },
		m_lWeight{ lWeight }
	{}

	CHotPixel(const CHotPixel&) = default;
	CHotPixel& operator=(const CHotPixel&) = default;
	virtual ~CHotPixel() = default;

	bool operator<(const CHotPixel& hp) const
	{
		if (m_lX < hp.m_lX)
			return true;
		else if (m_lX > hp.m_lX)
			return false;
		else
			return (m_lY < hp.m_lY);
	}

	void SetPixel(int lX, int lY)
	{
		m_lX = lX;
		m_lY = lY;
	}
};

typedef std::vector<CHotPixel> HOTPIXELVECTOR;
typedef HOTPIXELVECTOR::iterator HOTPIXELITERATOR;

/* ------------------------------------------------------------------- */

class CDarkMultiplier
{
public :
	double						m_fMultiplier;
	double						m_fMin;
	double						m_fMax;

private :
	void	CopyFrom(const CDarkMultiplier & dm)
	{
		m_fMultiplier	= dm.m_fMultiplier;
		m_fMin			= dm.m_fMin;
		m_fMax			= dm.m_fMax;
	};

public :
	CDarkMultiplier()
	{
        m_fMax = 0;
        m_fMin = 0;
        m_fMultiplier = 0;
    };

	virtual ~CDarkMultiplier()
	{
	};
};

/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */

class CExcludedPixel
{
public :
	int			X;
	int			Y;

private :
	void	CopyFrom(const CExcludedPixel & ep)
	{
		X = ep.X;
		Y = ep.Y;
	};

public :
	CExcludedPixel(int lX = 0, int lY = 0)
	{
		X = lX;
		Y = lY;
	};

	CExcludedPixel(const CExcludedPixel & ep)
	{
		CopyFrom(ep);
	};

	virtual ~CExcludedPixel()
	{
	};

	const CExcludedPixel & operator = (const CExcludedPixel & ep)
	{
		CopyFrom(ep);
		return (*this);
	};

	bool operator < (const CExcludedPixel & ep) const
	{
		if (X < ep.X)
			return true;
		else if (X > ep.X)
			return false;
		else
			return (Y < ep.Y);
	};
};

typedef std::vector<CExcludedPixel>			EXCLUDEDPIXELVECTOR;
typedef std::set<CExcludedPixel>			EXCLUDEDPIXELSET;
typedef EXCLUDEDPIXELSET::iterator			EXCLUDEDPIXELITERATOR;

/* ------------------------------------------------------------------- */
class CMemoryBitmap;
class CDarkFrameHotParameters
{
public:
	double m_fGrayValue;

public:
	CDarkFrameHotParameters() : m_fGrayValue{ 0 } {};
	virtual ~CDarkFrameHotParameters() {};

	void ComputeParameters(CMemoryBitmap* pBitmap, HOTPIXELVECTOR& vHotPixels);
};

/* ------------------------------------------------------------------- */

class CDarkAmpGlowParameters
{
public :
	DSSRect						m_rcHotest;
	std::vector<DSSRect>		m_vrcColdest;
	std::vector<double>			m_vMedianColdest;
	double						m_fMedianHotest;
	double						m_fGrayValue;
	int						m_lColdestIndice;

	DSSRect getRectAroundPoint(int lWidth, int lHeight, int lSize, const CHotPixel & px)
	{
		DSSRect rc{ std::max(0, px.m_lX - lSize), std::max(0, px.m_lY - lSize),
				std::min(lWidth, px.m_lX + lSize), std::min(lHeight, px.m_lY + lSize) };

		return rc;
	};

	void getBorderRects(int lWidth, int lHeight, std::vector<DSSRect>& vRects)
	{
		const int lSize = std::min(50, std::min(lWidth / 10, lHeight / 10)) / 2;

		// Left side
		vRects.emplace_back(0, 0, 2 * lSize, 2 * lSize);
		vRects.emplace_back(0, lHeight / 2 - lSize, 2 * lSize, lHeight / 2 + lSize);
		vRects.emplace_back(0, lHeight - 2 * lSize, 2 * lSize, lHeight);

		// Right side
		vRects.emplace_back(lWidth - 2 * lSize, 0, lWidth, 2 * lSize);
		vRects.emplace_back(lWidth - 2 * lSize, lHeight / 2 - lSize, lWidth, lHeight / 2 + lSize);
		vRects.emplace_back(lWidth - 2 * lSize, lHeight - 2 * lSize, lWidth, lHeight);

		// Middle top/bottom
		vRects.emplace_back(lWidth / 2 - lSize, 0, lWidth / 2 + lSize, 2 * lSize);
		vRects.emplace_back(lWidth / 2 - lSize, lHeight - 2 * lSize, lWidth / 2 + lSize, lHeight);
	};

	double computeMedianValueInRect(CMemoryBitmap* pBitmap, const DSSRect& rc);

public :
	CDarkAmpGlowParameters() = default;
	CDarkAmpGlowParameters(const CDarkAmpGlowParameters&) = default;

	CDarkAmpGlowParameters& operator = (const CDarkAmpGlowParameters&) = default;

	virtual ~CDarkAmpGlowParameters() {};

	void ComputeParametersFromPoints(CMemoryBitmap * pBitmap);
	void FindPointsAndComputeParameters(CMemoryBitmap* pBitmap);
	void ComputeParametersFromIndice(int lIndice)
	{
		m_fGrayValue = m_fMedianHotest - m_vMedianColdest[lIndice];
	};
};

/* ------------------------------------------------------------------- */

class CDarkFrame
{
private :
	bool						m_bDarkOptimization;
	bool						m_bHotPixelsDetection;
	bool						m_bHotPixelDetected;
	bool						m_bBadLinesDetection;
	double						m_fDarkFactor;
	std::shared_ptr<CMemoryBitmap> m_pMasterDark;
	std::shared_ptr<CMemoryBitmap> m_pAmpGlow;
	std::shared_ptr<CMemoryBitmap> m_pDarkCurrent;
	HOTPIXELVECTOR				m_vHotPixels;
	EXCLUDEDPIXELVECTOR			m_vExcludedPixels;

	CDarkFrameHotParameters		m_HotParameters;
	CDarkAmpGlowParameters		m_AmpglowParameters;

	void Reset(std::shared_ptr<CMemoryBitmap> pMaster);


	void	FillExcludedPixelList(const STARVECTOR * pStars, EXCLUDEDPIXELVECTOR & vExcludedPixels);
	void	GetValidNeighbors(int lX, int lY, HOTPIXELVECTOR & vPixels, int lRadius, BAYERCOLOR BayerColor = BAYER_UNKNOWN);

protected :
	void	ComputeOptimalDistributionRatio(CMemoryBitmap * pBitmap, CMemoryBitmap * pDark, double & fRatio, DSS::ProgressBase * pProgress);

	void	ComputeDarkFactorFromMedian(CMemoryBitmap * pBitmap, double & fHotDark, double & fAmpGlow, DSS::ProgressBase * pProgress);
	void	ComputeDarkFactor(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor, DSS::ProgressBase * pProgress);
	void	ComputeDarkFactorFromHotPixels(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor);
	void	RemoveContiguousHotPixels(bool bCFA);
	void	FindHotPixels(DSS::ProgressBase* pProgress);
	void	FindBadVerticalLines(DSS::ProgressBase* pProgress);

public :
	CDarkFrame(std::shared_ptr<CMemoryBitmap> pMaster = std::shared_ptr<CMemoryBitmap>{})
	{
		Reset(pMaster);
	}

	virtual ~CDarkFrame()
	{}

	void SetMasterDark(std::shared_ptr<CMemoryBitmap> pMaster)
	{
		Reset(pMaster);
		//m_pMasterDark = pMaster; // pMaster.Addref(); m_pMasterDark.m_p->Release(); m_pMasterDark.m_p = pMaster;
	}

	bool Subtract(std::shared_ptr<CMemoryBitmap> pTarget, DSS::ProgressBase* pProgress = nullptr);
	void InterpolateHotPixels(std::shared_ptr<CMemoryBitmap> pBitmap, DSS::ProgressBase* pProgress = nullptr);

	bool IsOk() const
	{
//#		return (m_pMasterDark != nullptr);
		return static_cast<bool>(this->m_pMasterDark);
	}
};
