#ifndef __DARKFRAME_H__
#define __DARKFRAME_H__

#include "BitmapExt.h"
#include "StackingTasks.h"
#include <algorithm>
#include <set>
#include "Stars.h"

/* ------------------------------------------------------------------- */

class	CHotPixel
{
public :
	LONG				m_lX,
						m_lY;
	LONG				m_lWeight;

private :
	void	CopyFrom(const CHotPixel & hp)
	{
		m_lX = hp.m_lX;
		m_lY = hp.m_lY;
		m_lWeight = hp.m_lWeight;
	};

public :
	CHotPixel(LONG X = 0, LONG Y = 0, LONG lWeight = 1)
	{
		m_lX = X;
		m_lY = Y;
		m_lWeight = lWeight;
	};

	CHotPixel(const CHotPixel & hp)
	{
		CopyFrom(hp);
	};

	const CHotPixel & operator = (const CHotPixel & hp)
	{
		CopyFrom(hp);
		return (*this);
	};

	virtual ~CHotPixel()
	{
	};

	bool operator < (const CHotPixel & hp) const
	{
		if (m_lX < hp.m_lX)
			return true;
		else if (m_lX > hp.m_lX)
			return false;
		else
			return (m_lY < hp.m_lY);
	};

	void	SetPixel(LONG lX, LONG lY)
	{
		m_lX = lX;
		m_lY = lY;
	};
};

typedef std::vector<CHotPixel>		HOTPIXELVECTOR;
typedef HOTPIXELVECTOR::iterator	HOTPIXELITERATOR;

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
	LONG			X;
	LONG			Y;

private :
	void	CopyFrom(const CExcludedPixel & ep)
	{
		X = ep.X;
		Y = ep.Y;
	};

public :
	CExcludedPixel(LONG lX = 0, LONG lY = 0)
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
public :
	double						m_fGrayValue;

public :
	CDarkFrameHotParameters() {};
	virtual ~CDarkFrameHotParameters() {};

	void	ComputeParameters(CMemoryBitmap * pBitmap, HOTPIXELVECTOR & vHotPixels);
};

/* ------------------------------------------------------------------- */

class CDarkAmpGlowParameters
{
public :
	CRect						m_rcHotest;
	std::vector<CRect>			m_vrcColdest;
	std::vector<double>			m_vMedianColdest;
	double						m_fMedianHotest;
	double						m_fGrayValue;
	LONG						m_lColdestIndice;

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

	void	GetRectAroundPoint(LONG lWidth, LONG lHeight, LONG lSize, const CHotPixel & px, CRect & rc)
	{
		rc.left		= max(0L, px.m_lX-lSize);
		rc.right	= min(lWidth-1, px.m_lX+lSize);
		rc.top		= max(0L, px.m_lY-lSize);
		rc.bottom	= min(lHeight-1, px.m_lY+lSize);
	};

	void	GetBorderRects(LONG lWidth, LONG lHeight, std::vector<CRect> & vRects)
	{
		LONG		lSize = min(50L, min(lWidth/10, lHeight/10))/2;

		CRect		rc;

		// Left side
		rc.SetRect(0, 0, 2*lSize-1, 2*lSize-1);
		vRects.push_back(rc);
		rc.SetRect(0, lHeight/2-lSize, 2*lSize, lHeight/2+lSize-1);
		vRects.push_back(rc);
		rc.SetRect(0, lHeight-2*lSize, 2*lSize, lHeight-1);
		vRects.push_back(rc);

		// Right side
		rc.SetRect(lWidth-2*lSize, 0, lWidth-1, 2*lSize-1);
		vRects.push_back(rc);
		rc.SetRect(lWidth-2*lSize, lHeight/2-lSize, lWidth-1, lHeight/2+lSize-1);
		vRects.push_back(rc);
		rc.SetRect(lWidth-2*lSize, lHeight-2*lSize, lWidth-1, lHeight-1);
		vRects.push_back(rc);

		// Middle top/bottom
		rc.SetRect(lWidth/2-lSize, 0, lWidth/2+lSize-1, 2*lSize-1);
		vRects.push_back(rc);
		rc.SetRect(lWidth/2-lSize, lHeight-2*lSize, lWidth/2+lSize-1, lHeight-1);
		vRects.push_back(rc);
	};

	double	ComputeMedianValueInRect(CMemoryBitmap * pBitmap, CRect & rc);

public :
	CDarkAmpGlowParameters() {};
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
	void	FindPointsAndComputeParameters(CMemoryBitmap * pBitmap);
	void	ComputeParametersFromIndice(LONG lIndice)
	{
		m_fGrayValue = m_fMedianHotest - m_vMedianColdest[lIndice];
	};
};

/* ------------------------------------------------------------------- */

class CDarkFrame
{
private :
	BOOL						m_bDarkOptimization;
	BOOL						m_bHotPixelsDetection;
	BOOL						m_bHotPixelDetected;
	BOOL						m_bBadLinesDetection;
	double						m_fDarkFactor;
	CSmartPtr<CMemoryBitmap>	m_pMasterDark;
	CSmartPtr<CMemoryBitmap>	m_pAmpGlow;
	CSmartPtr<CMemoryBitmap>	m_pDarkCurrent;
	HOTPIXELVECTOR				m_vHotPixels;
	EXCLUDEDPIXELVECTOR			m_vExcludedPixels;

	CDarkFrameHotParameters		m_HotParameters;
	CDarkAmpGlowParameters		m_AmpglowParameters;

	void	Reset()
	{
		m_bDarkOptimization		= CAllStackingTasks::GetDarkOptimization();
		m_bHotPixelsDetection	= CAllStackingTasks::GetHotPixelsDetection();
		m_bBadLinesDetection	= CAllStackingTasks::GetBadLinesDetection();
		m_fDarkFactor			= CAllStackingTasks::GetDarkFactor();
		m_bHotPixelDetected		= FALSE;
		m_pMasterDark.Release();
		m_vHotPixels.clear();
	};


	void	FillExcludedPixelList(STARVECTOR * pStars, EXCLUDEDPIXELVECTOR & vExcludedPixels);
	void	GetValidNeighbors(LONG lX, LONG lY, HOTPIXELVECTOR & vPixels, LONG lRadius, BAYERCOLOR BayerColor = BAYER_UNKNOWN);

protected :
	void	ComputeOptimalDistributionRatio(CMemoryBitmap * pBitmap, CMemoryBitmap * pDark, double & fRatio, CDSSProgress * pProgress);

	void	ComputeDarkFactorFromMedian(CMemoryBitmap * pBitmap, double & fHotDark, double & fAmpGlow, CDSSProgress * pProgress);
	void	ComputeDarkFactor(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor, CDSSProgress * pProgress);
	void	ComputeDarkFactorFromHotPixels(CMemoryBitmap * pBitmap, STARVECTOR * pStars, double & fRedFactor, double & fGreenFactor, double & fBlueFactor);
	void	RemoveContiguousHotPixels(BOOL bCFA);
	void	FindHotPixels(CDSSProgress * pProgress);
	void	FindBadVerticalLines(CDSSProgress * pProgress);

public :
	CDarkFrame(CMemoryBitmap * pMasterDark = NULL)
	{
		Reset();
		m_pMasterDark = pMasterDark;
	};

	virtual ~CDarkFrame()
	{
	};

	void	SetMasterDark(CMemoryBitmap * pMasterDark)
	{
		Reset();
		m_pMasterDark = pMasterDark;
	};

	BOOL	Subtract(CMemoryBitmap * pTarget, CDSSProgress * pProgress = NULL);

	void	InterpolateHotPixels(CMemoryBitmap * pBitmap, CDSSProgress * pProgress = NULL);

	BOOL	IsOk()
	{
		return (m_pMasterDark != NULL);
	};
};

#endif // __DARKFRAME_H__
