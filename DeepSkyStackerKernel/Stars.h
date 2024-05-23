#pragma once
#include "dssrect.h"
#include "DSSTools.h"


class CStar
{
public :
	DSSRect			m_rcStar;
	double			m_fIntensity;
	double			m_fPercentage;
	double			m_fDeltaRadius;
	double			m_fQuality;
	double			m_fMeanRadius;
	double			m_fX, m_fY;
	bool			m_bUsed;
	bool			m_bAdded;
	bool			m_bRemoved;
	double			m_fLargeMajorAxis;
	double			m_fSmallMajorAxis;
	double			m_fLargeMinorAxis;
	double			m_fSmallMinorAxis;
	double			m_fMajorAxisAngle;

private :
	void CopyFrom(const CStar & ms)
	{
		m_rcStar		= ms.m_rcStar;
		m_fIntensity	= ms.m_fIntensity;
		m_fPercentage	= ms.m_fPercentage;
		m_fDeltaRadius	= ms.m_fDeltaRadius;
		m_fQuality		= ms.m_fQuality;
		m_fMeanRadius	= ms.m_fMeanRadius;
		m_fX			= ms.m_fX;
		m_fY			= ms.m_fY;
		m_bUsed			= ms.m_bUsed;
		m_bAdded		= ms.m_bAdded;
		m_bRemoved		= ms.m_bRemoved;
		m_fLargeMajorAxis = ms.m_fLargeMajorAxis;
		m_fSmallMajorAxis = ms.m_fSmallMajorAxis;
		m_fLargeMinorAxis = ms.m_fLargeMinorAxis;
		m_fSmallMinorAxis = ms.m_fSmallMinorAxis;
		m_fMajorAxisAngle = ms.m_fMajorAxisAngle;
	};

public :
	CStar()
	{
		m_fX	   = 0;
		m_fY	   = 0;
		m_bUsed    = false;
		m_bAdded   = false;
		m_bRemoved = false;
		m_fLargeMajorAxis = 0;
		m_fSmallMajorAxis = 0;
		m_fLargeMinorAxis = 0;
		m_fSmallMinorAxis = 0;
		m_fMajorAxisAngle = 0;
		m_fIntensity	  = 0.0;
		m_fPercentage	  = 0.0;
		m_fQuality		  = 0.0;
		m_fMeanRadius	  = 0.0;
		m_fDeltaRadius	  = 0.0;
		m_rcStar.setEmpty();
	};
	~CStar() {};

	CStar(const CStar & ms)
	{
		CopyFrom(ms);
	};

	CStar(double fX, double fY)
	{
		m_fX = fX;
		m_fY = fY;
		m_bUsed    = false;
		m_bAdded   = false;
		m_bRemoved = false;
		m_fLargeMajorAxis = 0;
		m_fSmallMajorAxis = 0;
		m_fLargeMinorAxis = 0;
		m_fSmallMinorAxis = 0;
		m_fMajorAxisAngle = 0;
		m_fIntensity	  = 0.0;
		m_fPercentage	  = 0.0;
		m_fQuality		  = 0.0;
		m_fMeanRadius	  = 0.0;
		m_fDeltaRadius	  = 0.0;
		m_rcStar.setEmpty();
	};

	CStar & operator = (const CStar & ms)
	{
		CopyFrom(ms);
		return (*this);
	};

	bool operator < (const CStar & ms) const
	{
		if (m_fX < ms.m_fX)
			return true;
		else if (m_fX > ms.m_fX)
			return false;
		else
			return (m_fY < ms.m_fY);
	};

	private:
	bool inRadius(const double lx, const double ly, const double rx, const double ry) const noexcept
	{
		return Distance(lx, ly, rx, ry) <= m_fMeanRadius * (2.35 / 1.5);
	};

	public:
	bool IsInRadius(const QPoint& pt) const noexcept
	{
		return inRadius(m_fX, m_fY, pt.x(), pt.y());
	};

	bool IsInRadius(const QPointF & pt) const noexcept
	{
		return inRadius(m_fX, m_fY, pt.x(), pt.y());
	};

	bool IsInRadius(double fX, double fY) const noexcept
	{
		return inRadius(m_fX, m_fY, fX, fY);
	};

	bool	IsValid()
	{
		bool		bResult = false;

		if (m_fX > 0 && m_fY > 0 && m_fQuality > 0 && m_fIntensity > 0 && m_fMeanRadius > 0)
			bResult = true;

		return bResult;
	}
};

inline bool CompareStarLuminancy (const CStar & ms1, const CStar  & ms2)
{
	if (ms1.m_fIntensity > ms2.m_fIntensity)
		return true;
	else if (ms1.m_fIntensity < ms2.m_fIntensity)
		return false;
	else
		return (ms1.m_fMeanRadius > ms2.m_fMeanRadius);
};

typedef std::vector<CStar>		STARVECTOR;
typedef STARVECTOR::iterator	STARITERATOR;
typedef std::set<CStar>			STARSET;
typedef STARSET::iterator		STARSETITERATOR;

// Returns the index of the nearest star in the star-vector.
inline int FindNearestStar(const double fX, const double fY, const STARVECTOR& vStars, bool& bIn, double& fDistance)
{
	int lResult = -1;
	double minDistanceSqr = std::numeric_limits<double>::max();
	bIn = false;

	for (int i = 0; const auto& star : vStars)
	{
		if (!star.m_bRemoved)
		{
			const double dx = star.m_fX - fX;
			const double dy = star.m_fY - fY;
			const double testDistanceSqr = dx * dx + dy * dy;

			if (testDistanceSqr < minDistanceSqr)
			{
				minDistanceSqr = testDistanceSqr;
				lResult = i;
				bIn = star.IsInRadius(QPointF{ fX, fY });
			}
		}
		++i;
	}

	fDistance = lResult >= 0 ? std::sqrt(minDistanceSqr) : -1.0;

	return lResult;
}

inline int FindNearestStarWithinDistance(const double fX, const double fY, const STARVECTOR& vStars, bool& bIn, double& fDistance)
{
	const double distanceRange = fDistance;
	int lResult = -1;
	double minDistanceSqr = std::numeric_limits<double>::max();
	bIn = false;

	// First star right of fx - distanceRange.
	auto it = std::ranges::lower_bound(vStars, CStar{ fX - distanceRange, 0 }, std::less{});
	while (it != std::cend(vStars) && it->m_fX <= fX + distanceRange) // While star is left of fx + distanceRange
	{
		if (!it->m_bRemoved && std::abs(it->m_fY - fY) <= distanceRange) // y-distance smaller than distanceRange ?
		{
			const double dx = it->m_fX - fX;
			const double dy = it->m_fY - fY;
			const double testDistanceSqr = dx * dx + dy * dy;

			if (testDistanceSqr < minDistanceSqr)
			{
				minDistanceSqr = testDistanceSqr;
				lResult = std::distance(vStars.cbegin(), it);
				bIn = it->IsInRadius(QPointF{ fX, fY });
			}
		}
		++it;
	}

	fDistance = lResult >= 0 ? std::sqrt(minDistanceSqr) : -1.0;

	return lResult;
}
