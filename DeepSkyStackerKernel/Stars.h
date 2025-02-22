#pragma once
#include "dssrect.h"
#include "DSSTools.h"


class CStar
{
public:
	DSSRect			m_rcStar{};
	double			m_fIntensity{ 0.0 };
	double			m_fPercentage{ 0.0 };
	double			m_fCircularity{ 0.0 };
	double			m_fQuality{ 0.0 };
	double			m_fMeanRadius{ 0.0 };
	double			m_fX{ 0.0 };
	double			m_fY{ 0.0 };
//	bool			m_bUsed{ false }; // MT, Aug. 2024: this bool is not used anywhere.
	bool			m_bAdded{ false };
	bool			m_bRemoved{ false };
	double			m_fLargeMajorAxis{ 0.0 };
	double			m_fSmallMajorAxis{ 0.0 };
	double			m_fLargeMinorAxis{ 0.0 };
	double			m_fSmallMinorAxis{ 0.0 };
	double			m_fMajorAxisAngle{ 0.0 };

public:
	CStar() = default;
	explicit constexpr CStar(const double x, const double y) : m_fX{ x }, m_fY{ y }
	{}
	CStar(const CStar&) = default;
	CStar& operator=(const CStar&) = default;
	CStar(CStar&&) = default;
	~CStar() = default;

	friend constexpr auto operator<=>(const CStar& lhs, const CStar& rhs)
	{
		if (auto cmp = lhs.m_fX <=> rhs.m_fX; cmp != 0)
			return cmp;
		return lhs.m_fY <=> rhs.m_fY;
	}
	// Two stars are equal if they are at the same position
	friend constexpr auto operator==(const CStar& lhs, const CStar& rhs)
	{
		return operator<=>(lhs, rhs) == 0;
	}

private:
	bool inRadius(const double lx, const double ly, const double rx, const double ry) const noexcept
	{
		return Distance(lx, ly, rx, ry) <= m_fMeanRadius * (2.35 / 1.5);
	}

public:
	bool IsInRadius(const QPoint& pt) const noexcept
	{
		return inRadius(m_fX, m_fY, pt.x(), pt.y());
	}

	bool IsInRadius(const QPointF & pt) const noexcept
	{
		return inRadius(m_fX, m_fY, pt.x(), pt.y());
	}

	bool IsInRadius(double fX, double fY) const noexcept
	{
		return inRadius(m_fX, m_fY, fX, fY);
	}

	bool IsValid() const
	{
		return (m_fX > 0 && m_fY > 0 && m_fQuality > 0 && m_fIntensity > 0 && m_fMeanRadius > 0);
	}
};

typedef std::vector<CStar>		STARVECTOR;
typedef STARVECTOR::iterator	STARITERATOR;
typedef std::set<CStar>			STARSET;
typedef STARSET::iterator		STARSETITERATOR;

constexpr bool CompareStarLuminancy(const CStar& ms1, const CStar& ms2)
{
	if (ms1.m_fIntensity > ms2.m_fIntensity)
		return true;
	if (ms1.m_fIntensity < ms2.m_fIntensity)
		return false;
	return (ms1.m_fMeanRadius > ms2.m_fMeanRadius);
}

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
	auto it = std::ranges::lower_bound(vStars, CStar{ fX - distanceRange, 0 });
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
