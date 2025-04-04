#pragma once

#include "DSSTools.h"
#include "BilinearParameters.h"

class CPixelTransform
{
public:
	CBilinearParameters		m_BilinearParameters;
	double					m_fXShift,
		m_fYShift;
	int					m_lPixelSizeMultiplier;
	bool					m_bUseCometShift;
	double					m_fXCometShift,
		m_fYCometShift;

private:
	void	CopyFrom(const CPixelTransform& pt)
	{
		m_BilinearParameters = pt.m_BilinearParameters;
		m_fXShift = pt.m_fXShift;
		m_fYShift = pt.m_fYShift;
		m_lPixelSizeMultiplier = pt.m_lPixelSizeMultiplier;
		m_bUseCometShift = pt.m_bUseCometShift;
		m_fXCometShift = pt.m_fXCometShift;
		m_fYCometShift = pt.m_fYCometShift;
	};

public:
	CPixelTransform()
	{
		m_fXShift = m_fYShift = 0;
		m_fXCometShift = m_fYCometShift = 0;
		m_lPixelSizeMultiplier = 1;
		m_bUseCometShift = false;
	};

	CPixelTransform(const CBilinearParameters& transform)
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
		QPointF	ptComet(fXTgtComet, fYTgtComet);

		if (!bAlreadyTransformed)
			ptComet = m_BilinearParameters.transform(ptComet);

		m_fXCometShift = fXOrgComet - ptComet.x();
		m_fYCometShift = fYOrgComet - ptComet.y();
		m_bUseCometShift = !bDoNotUse;
	};

	void	SetPixelSizeMultiplier(int lPixelSizeMultiplier)
	{
		m_lPixelSizeMultiplier = lPixelSizeMultiplier;
	};

	virtual ~CPixelTransform() {};

	CPixelTransform(const CPixelTransform& pt)
	{
		CopyFrom(pt);
	};

	CPixelTransform& operator = (const CPixelTransform& pt)
	{
		CopyFrom(pt);
		return (*this);
	};

	QPointF transform(const QPointF& pp) const
	{
		// First rotate
//		QPointF	ppResult = pp;
//		qreal& x = ppResult.rx();
//		qreal& y = ppResult.ry();

		QPointF ppResult = m_BilinearParameters.transform(pp);
		ppResult *= static_cast<qreal>(m_lPixelSizeMultiplier);
		//		x *= m_lPixelSizeMultiplier;
		//		y *= m_lPixelSizeMultiplier;

		ppResult += QPointF{ m_fXShift, m_fYShift };
		//		x += m_fXShift;
		//		y += m_fYShift;

		if (m_bUseCometShift)
		{
			ppResult += QPointF{ m_fXCometShift, m_fYCometShift };
			//			x += m_fXCometShift;
			//			y += m_fYCometShift;
		}

		return ppResult;
	}
};

typedef std::vector<CPixelTransform> PIXELTRANSFORMVECTOR;

/* ------------------------------------------------------------------- */

class CPixelDispatch final
{
public:
	int m_lX{ 0 };
	int m_lY{ 0 };
	double m_fPercentage{ 0.0 };

public:
	CPixelDispatch() = default;
	explicit CPixelDispatch(const int lX, const int lY, const double fPercentage) :
		m_lX{ lX },
		m_lY{ lY },
		m_fPercentage{ fPercentage }
	{}
	CPixelDispatch(const CPixelDispatch&) = default;
	CPixelDispatch& operator=(const CPixelDispatch&) = default;
	CPixelDispatch(CPixelDispatch&&) = default;
	~CPixelDispatch() = default;
};

using PIXELDISPATCHVECTOR = std::vector<CPixelDispatch>;

inline void ComputePixelDispatch(const QPointF& pt, PIXELDISPATCHVECTOR& vPixels)
{
	const int lX = static_cast<int>(floor(pt.x()));
	const int lY = static_cast<int>(floor(pt.y()));
	const double fRemainX = pt.x() - lX;
	const double fRemainY = pt.y() - lY;

	if ((pt.x() == lX) && (pt.y() == lY))
	{
		// Only one pixel with all the luminosity
		vPixels.emplace_back(lX, lY, 1.0);
	}
	else if (pt.x() == lX)
	{
		// 2 pixels
		vPixels.emplace_back(lX, lY, 1.0 - fRemainY);
		vPixels.emplace_back(lX, lY + 1, fRemainY);
	}
	else if (pt.y() == lY)
	{
		// 2 pixels
		vPixels.emplace_back(lX, lY, 1.0 - fRemainX);
		vPixels.emplace_back(lX + 1, lY, fRemainX);
	}
	else
	{
		// 4 pixels
		vPixels.emplace_back(lX, lY, (1.0 - fRemainX) * (1.0 - fRemainY));
		vPixels.emplace_back(lX + 1, lY, fRemainX * (1.0 - fRemainY));
		vPixels.emplace_back(lX, lY + 1, (1.0 - fRemainX) * fRemainY);
		vPixels.emplace_back(lX + 1, lY + 1, fRemainX * fRemainY);
	}
}

// Compute pixel dispatch with larger oversampling
//	1 = pt(0, 0) -> 1 pixel
//  2 = pt(-0.5, -0.5) - pt(-0.5, 0.5) - pt(0.5, -0.5) - pt (0.5, 0.5) - 4 pixels
//  3 = pt(-1, -1) - ... - pt (0, 0) - ... - pt (1, 1) - 9 pixels
//  4 = pt(-1.5, -1.5) - ... - pt(1.5, 1.5) - 16 pixels
inline void ComputePixelDispatch(const QPointF& pt, const int lPixelSize, PIXELDISPATCHVECTOR& vPixels)
{
	const double fStart = static_cast<double>(lPixelSize - 1) / 2.0;

	for (double i = -fStart; i <= fStart; i++)
		for (double j = -fStart; j <= fStart; j++)
		{
			ComputePixelDispatch(pt + QPointF{ i, j }, vPixels);
		}
}

inline std::array<double, 4> ComputeAll4PixelDispatches(const QPointF& pt, std::span<int, 4> xcoords, std::span<int, 4> ycoords)
{
	const double xf = std::floor(pt.x());
	const double yf = std::floor(pt.y());
	const int xfi = static_cast<int>(xf);
	const int yfi = static_cast<int>(yf);
	xcoords[0] = xfi; xcoords[1] = xfi + 1; xcoords[2] = xfi;     xcoords[3] = xfi + 1;
	ycoords[0] = yfi; ycoords[1] = yfi;     ycoords[2] = yfi + 1; ycoords[3] = yfi + 1;

	return {
		(1.0 - pt.x() + xf) * (1.0 - pt.y() + yf),
		(pt.x() - xf) * (1.0 - pt.y() + yf),
		(1.0 - pt.x() + xf) * (pt.y() - yf),
		(pt.x() - xf) * (pt.y() - yf)
	};
}
