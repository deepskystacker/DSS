#ifndef __DARKFRAME_H__
#define __DARKFRAME_H__

#include "BitmapExt.h"
#include "StackingTasks.h"
#include <algorithm>
#include <set>
#include "Stars.h"

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

private :
	void	CopyFrom(const CDarkAmpGlowParameters & dagp)
	{
		m_rcHotest			= dagp.m_rcHotest;
		m_vrcColdest		= dagp.m_vrcColdest;
		m_fMedianHotest		= dagp.m_fMedianHotest;
		m_vMedianColdest	= dagp.m_vMedianColdest;
		m_fGrayValue		= dagp.m_fGrayValue;
		m_lColdestIndice	= dagp.m_lColdestIndice;
	};

	DSSRect getRectAroundPoint(int lWidth, int lHeight, int lSize, const CHotPixel & px)
	{
		DSSRect rc{ QPoint {std::max(0, px.m_lX - lSize), std::max(0, px.m_lY - lSize)},
				QPoint {std::min(lWidth - 1, px.m_lX + lSize), std::min(lHeight - 1, px.m_lY + lSize) } };

		return rc;

	};

	void	GetBorderRects(int lWidth, int lHeight, std::vector<DSSRect> & vRects)
	{
		const int lSize = std::min(50, std::min(lWidth / 10, lHeight / 10)) / 2;
		DSSRect rc;

		// Left side
		rc.setCoords(0, 0, 2*lSize-1, 2*lSize-1);
		vRects.push_back(rc);
		rc.setCoords(0, lHeight/2-lSize, 2*lSize, lHeight/2+lSize-1);
		vRects.push_back(rc);
		rc.setCoords(0, lHeight-2*lSize, 2*lSize, lHeight-1);
		vRects.push_back(rc);

		// Right side
		rc.setCoords(lWidth-2*lSize, 0, lWidth-1, 2*lSize-1);
		vRects.push_back(rc);
		rc.setCoords(lWidth-2*lSize, lHeight/2-lSize, lWidth-1, lHeight/2+lSize-1);
		vRects.push_back(rc);
		rc.setCoords(lWidth-2*lSize, lHeight-2*lSize, lWidth-1, lHeight-1);
		vRects.push_back(rc);

		// Middle top/bottom
		rc.setCoords(lWidth/2-lSize, 0, lWidth/2+lSize-1, 2*lSize-1);
		vRects.push_back(rc);
		rc.setCoords(lWidth/2-lSize, lHeight-2*lSize, lWidth/2+lSize-1, lHeight-1);
		vRects.push_back(rc);
	};

	double computeMedianValueInRect(CMemoryBitmap* pBitmap, const DSSRect& rc);

public :
	CDarkAmpGlowParameters()
    {
        m_fMedianHotest = 0;
        m_fGrayValue = 0;
        m_lColdestIndice = 0;
    }
	CDarkAmpGlowParameters(const CDarkAmpGlowParameters & dagp)
	{
		CopyFrom(dagp);
	};

	CDarkAmpGlowParameters & operator = (const CDarkAmpGlowParameters & dagp)
	{
		CopyFrom(dagp);
		return (*this);
	};

	virtual ~CDarkAmpGlowParameters() {};

	void	ComputeParametersFromPoints(CMemoryBitmap * pBitmap);
	void FindPointsAndComputeParameters(CMemoryBitmap* pBitmap);
	void	ComputeParametersFromIndice(int lIndice)
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

	void Reset(std::shared_ptr<CMemoryBitmap> pMaster)
	{
		m_bDarkOptimization		= CAllStackingTasks::GetDarkOptimization();
		m_bHotPixelsDetection	= CAllStackingTasks::GetHotPixelsDetection();
		m_bBadLinesDetection	= CAllStackingTasks::GetBadLinesDetection();
		m_fDarkFactor			= CAllStackingTasks::GetDarkFactor();
		m_bHotPixelDetected		= false;
		m_pMasterDark = pMaster;
		m_vHotPixels.clear();
	}


	void	FillExcludedPixelList(STARVECTOR * pStars, EXCLUDEDPIXELVECTOR & vExcludedPixels);
	void	GetValidNeighbors(int lX, int lY, HOTPIXELVECTOR & vPixels, int lRadius, BAYERCOLOR BayerColor = BAYER_UNKNOWN);

protected :
	void	ComputeOptimalDistributionRatio(CMemoryBitmap * pBitmap, CMemoryBitmap * pDark, double & fRatio, CDSSProgress * pProgress);

	void	ComputeDarkFactorFromMedian(CMemoryBitmap * pBitmap, double & fHotDark, double & fAmpGlow, CDSSProgress * pProgress);
	void	ComputeDarkFactor(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor, CDSSProgress * pProgress);
	void	ComputeDarkFactorFromHotPixels(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor);
	void	RemoveContiguousHotPixels(bool bCFA);
	void	FindHotPixels(CDSSProgress * pProgress);
	void	FindBadVerticalLines(CDSSProgress * pProgress);

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

	bool Subtract(std::shared_ptr<CMemoryBitmap> pTarget, CDSSProgress * pProgress = nullptr);
	void InterpolateHotPixels(std::shared_ptr<CMemoryBitmap> pBitmap, CDSSProgress * pProgress = nullptr);

	bool IsOk() const
	{
//#		return (m_pMasterDark != nullptr);
		return static_cast<bool>(this->m_pMasterDark);
	}
};

#endif // __DARKFRAME_H__
