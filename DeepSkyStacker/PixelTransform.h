#ifndef __PIXELTRANSFORM_H__
#define __PIXELTRANSFORM_H__

#include "DSSTools.h"

class CPixelTransform
{
public :
	CBilinearParameters		m_BilinearParameters;
	double					m_fXShift,
							m_fYShift;
	int					m_lPixelSizeMultiplier;
	bool					m_bUseCometShift;
	double					m_fXCometShift,
							m_fYCometShift;

private :
	void	CopyFrom(const CPixelTransform & pt)
	{
		m_BilinearParameters	= pt.m_BilinearParameters;
		m_fXShift				= pt.m_fXShift;
		m_fYShift				= pt.m_fYShift;
		m_lPixelSizeMultiplier	= pt.m_lPixelSizeMultiplier;
		m_bUseCometShift		= pt.m_bUseCometShift;
		m_fXCometShift			= pt.m_fXCometShift;
		m_fYCometShift			= pt.m_fYCometShift;
	};

public :
	CPixelTransform()
	{
		m_fXShift = m_fYShift = 0;
		m_fXCometShift = m_fYCometShift = 0;
		m_lPixelSizeMultiplier = 1;
		m_bUseCometShift = false;
	};

	CPixelTransform(const CBilinearParameters & transform)
	{
		m_BilinearParameters = transform;
		m_fXShift = m_fYShift = 0;
		m_fXCometShift = m_fYCometShift = 0;
		m_lPixelSizeMultiplier = 1;
        m_bUseCometShift = false;
	};

	void	SetShift(double fXShift, double fYShift)
	{
		m_fXShift = fXShift;
		m_fYShift = fYShift;
	};

	void	ComputeCometShift(double fXOrgComet, double fYOrgComet, double fXTgtComet, double fYTgtComet, bool bDoNotUse, bool bAlreadyTransformed)
	{
		CPointExt		ptComet(fXTgtComet, fYTgtComet);

		if (!bAlreadyTransformed)
			ptComet = m_BilinearParameters.Transform(ptComet);

		m_fXCometShift = fXOrgComet - ptComet.X;
		m_fYCometShift = fYOrgComet - ptComet.Y;
		m_bUseCometShift = !bDoNotUse;
	};

	void	SetPixelSizeMultiplier(int lPixelSizeMultiplier)
	{
		m_lPixelSizeMultiplier = lPixelSizeMultiplier;
	};

	virtual ~CPixelTransform() {};

	CPixelTransform(const CPixelTransform & pt)
	{
		CopyFrom(pt);
	};

	CPixelTransform & operator = (const CPixelTransform & pt)
	{
		CopyFrom(pt);
		return (*this);
	};

	CPointExt Transform(const CPointExt & pp) const
	{
		// First rotate
		CPointExt		ppResult = pp;

		ppResult = m_BilinearParameters.Transform(ppResult);
		ppResult.X *= m_lPixelSizeMultiplier;
		ppResult.Y *= m_lPixelSizeMultiplier;

		ppResult.X += m_fXShift;
		ppResult.Y += m_fYShift;

		if (m_bUseCometShift)
		{
			ppResult.X += m_fXCometShift;
			ppResult.Y += m_fYCometShift;
		};

		return ppResult;
	};

	QPointF transform(const QPointF& pp) const
	{
		// First rotate
		QPointF	ppResult = pp;
		qreal& x = ppResult.rx();
		qreal& y = ppResult.ry();


		ppResult = m_BilinearParameters.transform(ppResult);
		x *= m_lPixelSizeMultiplier;
		y *= m_lPixelSizeMultiplier;

		x += m_fXShift;
		y += m_fYShift;

		if (m_bUseCometShift)
		{
			x += m_fXCometShift;
			y += m_fYCometShift;
		};

		return ppResult;
	};
};

typedef std::vector<CPixelTransform>		PIXELTRANSFORMVECTOR;

/* ------------------------------------------------------------------- */

class CPixelDispatch
{
public :
	int			m_lX,
					m_lY;
	double			m_fPercentage;

private :
	void	CopyFrom(const CPixelDispatch & pd)
	{
		m_lX = pd.m_lX;
		m_lY = pd.m_lY;
		m_fPercentage = pd.m_fPercentage;
	};

public :
	CPixelDispatch(int lX = 0, int lY = 0, double fPercentage = 0.0)
	{
		m_lX = lX;
		m_lY = lY;
		m_fPercentage = fPercentage;
	};

	CPixelDispatch(const CPixelDispatch & pd)
	{
		CopyFrom(pd);
	};

	CPixelDispatch & operator = (const CPixelDispatch & pd)
	{
		CopyFrom(pd);
		return (*this);
	};

	virtual ~CPixelDispatch() {};
};

typedef std::vector<CPixelDispatch>			PIXELDISPATCHVECTOR;

inline void ComputePixelDispatch(const CPointExt & pt, PIXELDISPATCHVECTOR & vPixels)
{
	int			lX,
					lY;
	double			fRemainX,
					fRemainY;

	lX = floor(pt.X);
	lY = floor(pt.Y);
	fRemainX = pt.X - lX;
	fRemainY = pt.Y - lY;

	if ((pt.X == lX) && (pt.Y == lY))
	{
		// Only one pixel with all the luminosity
		vPixels.emplace_back(lX, lY, 1.0);
	}
	else if (pt.X == lX)
	{
		// 2 pixels
		vPixels.emplace_back(lX, lY, 1.0 - fRemainY);
		vPixels.emplace_back(lX, lY+1, fRemainY);
	}
	else if (pt.Y == lY)
	{
		// 2 pixels
		vPixels.emplace_back(lX, lY, 1.0 - fRemainX);
		vPixels.emplace_back(lX+1, lY, fRemainX);
	}
	else
	{
		// 4 pixels
		vPixels.emplace_back(lX, lY, (1.0 - fRemainX)*(1.0-fRemainY));
		vPixels.emplace_back(lX+1, lY, fRemainX * (1.0 - fRemainY));
		vPixels.emplace_back(lX, lY+1, (1.0 - fRemainX)*fRemainY);
		vPixels.emplace_back(lX+1, lY+1, fRemainX*fRemainY);
	};
};

// Compute pixel dispatch with larger oversampling
//	1 = pt(0, 0) -> 1 pixel
//  2 = pt(-0.5, -0.5) - pt(-0.5, 0.5) - pt(0.5, -0.5) - pt (0.5, 0.5) - 4 pixels
//  3 = pt(-1, -1) - ... - pt (0, 0) - ... - pt (1, 1) - 9 pixels
//  4 = pt(-1.5, -1.5) - ... - pt(1.5, 1.5) - 16 pixels
inline void ComputePixelDispatch(const CPointExt & pt, int lPixelSize, PIXELDISPATCHVECTOR & vPixels)
{
	double			fStart = (double)(lPixelSize-1)/2.0;

	for (double i = -fStart;i<=fStart;i++)
		for (double j = -fStart;j<=fStart;j++)
		{
			CPointExt		pt2 = pt;
			pt2.X += i;
			pt2.Y += j;
			ComputePixelDispatch(pt2, vPixels);
		}
};


#endif // __PIXELTRANSFORM_H__