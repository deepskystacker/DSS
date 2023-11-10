#pragma once
#include "GrayBitmap.h"

/* ------------------------------------------------------------------- */

class CBloomInfo
{
public :
	QPoint			m_ptRef;
	double				m_fBloom;
	double				m_fRadius;
	double				m_fAngle;

private:
	void	CopyFrom(const CBloomInfo & right)
	{
		m_ptRef		= right.m_ptRef;
		m_fBloom	= right.m_fBloom;
		m_fRadius	= right.m_fRadius;
		m_fAngle	= right.m_fAngle;
	};

public:
	CBloomInfo()
	{
        m_fRadius = 0;
        m_fBloom = 0;
        m_fAngle = 0;
	};
	~CBloomInfo()
	{
	};

	CBloomInfo(const CBloomInfo & right)
	{
		CopyFrom(right);
	};

	CBloomInfo & operator = (const CBloomInfo & right)
	{
		CopyFrom(right);
		return (*this);
	};
};

typedef std::vector<CBloomInfo>			BLOOMINFOVECTOR;

/* ------------------------------------------------------------------- */

class CBloomedStar
{
public :
	QPointF			m_ptStar;
	std::vector<QPoint> m_vBloomed;
	BLOOMINFOVECTOR		m_vBlooms;
	double				m_fRadius;
	double				m_fBloom;


private:
	void	CopyFrom(const CBloomedStar & right)
	{
		m_ptStar	= right.m_ptStar;
		m_vBloomed  = right.m_vBloomed;
		m_vBlooms	= right.m_vBlooms;
		m_fRadius   = right.m_fRadius;
		m_fBloom	= right.m_fBloom;
	};

public:
	CBloomedStar()
	{
        m_fRadius = 0;
        m_fBloom = 0;
	};
	~CBloomedStar()
	{
	};

	CBloomedStar(const CBloomedStar & right)
	{
		CopyFrom(right);
	};

	CBloomedStar & operator = (const CBloomedStar & right)
	{
		CopyFrom(right);
		return (*this);
	};

	bool operator<(const CBloomedStar&) const
	{
		return true;
	}
};

typedef std::vector<CBloomedStar>		BLOOMEDSTARVECTOR;

/* ------------------------------------------------------------------- */

class CBloomedStarGradient
{
public :
	QPointF			ptStar;
	double				fdX,
						fdY;
	double				fNW,
						fSW,
						fNE,
						fSE;
	double				fGradient;
	double				fPercentGradient;


private:
	void	CopyFrom(const CBloomedStarGradient & right)
	{
		ptStar	= right.ptStar	;
		fdX		= right.fdX		;
		fdY		= right.fdY		;
		fNW		= right.fNW		;
		fSW		= right.fSW		;
		fNE		= right.fNE		;
		fSE		= right.fSE		;
		fGradient = right.fGradient;
		fPercentGradient = right.fPercentGradient;
	};

public:
	CBloomedStarGradient()
	{
		fdX = 0;
		fdY = 0;
		fGradient = 0;
		fPercentGradient = 0;
        fNW = 0;
        fSW = 0;
        fNE = 0;
        fSE = 0;
	};
	~CBloomedStarGradient()
	{
	};

	CBloomedStarGradient(const CBloomedStarGradient & right)
	{
		CopyFrom(right);
	};

	CBloomedStarGradient & operator = (const CBloomedStarGradient & right)
	{
		CopyFrom(right);
		return (*this);
	};
};

/* ------------------------------------------------------------------- */
namespace DSS { class ProgressBase; }
class CMemoryBitmap;
class CDeBloom
{
private:
	double m_fBloomThreshold;
	BLOOMEDSTARVECTOR m_vBloomedStars;
	int m_lWidth;
	int m_lHeight;
	std::shared_ptr<C8BitGrayBitmap> m_pMask;
	DSS::ProgressBase* m_pProgress;
	double m_fBackground;

	bool	IsLeftEdge(CMemoryBitmap * pBitmap, int x, int y);
	bool	IsRightEdge(CMemoryBitmap * pBitmap, int x, int y);
	bool	IsTopEdge(CMemoryBitmap * pBitmap, int x, int y);
	bool	IsBottomEdge(CMemoryBitmap * pBitmap, int x, int y);

	void	ComputeStarCenter(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs);

	void	ExpandBloomedArea(CMemoryBitmap* pBitmap, C8BitGrayBitmap * pMask, int x, int y);
	std::shared_ptr<C8BitGrayBitmap> CreateMask(CMemoryBitmap* pBitmap);

	void	AddStar(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs);
	double	ComputeValue(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, int x, int y, bool & bDone);
	void	DeBloom(CMemoryBitmap * pBitmap, std::shared_ptr<C8BitGrayBitmap> pMask);
	void    SmoothMaskBorders(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask);
	void	MarkBloomBorder(CMemoryBitmap * pMask, int x, int y, std::vector<QPointF> & vBorders);
	void	MarkBorderAsBloomed(CMemoryBitmap * pMask, int x, int y, std::vector<QPoint> & vBloomed);

	double	ComputeBackgroundValue(CMemoryBitmap * pBitmap);
	double	ComputeStarGradient(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStarGradient & bsg, double fRadius);
	void	RefineStarCenter(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs);
	void	RefineStarCenter2(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs);

public:
	CDeBloom()
	{
		m_fBloomThreshold = 0.85;
		m_fBackground = 0;
        m_lWidth = 0;
        m_lHeight = 0;
        m_pProgress = nullptr;
	};

	virtual ~CDeBloom() {};

	void CreateBloomMask(CMemoryBitmap* pBitmap, DSS::ProgressBase* pProgress);
	void DeBloomImage(CMemoryBitmap* pBitmap, DSS::ProgressBase* pProgress);
};
