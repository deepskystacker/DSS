#pragma once

#if !defined(NDEBUG)
#include <boost/container/vector.hpp>
namespace bc = boost::container;
#endif
#include "DSSTools.h"
#include "BilinearParameters.h"

#pragma pack(push, STARTRIANGLE, 1)

/* ------------------------------------------------------------------- */

class CStarTriangle final
{
public:
	float m_fX{ 0 };
	float m_fY{ 0 };
	std::uint8_t m_Star1{ 0 };
	std::uint8_t m_Star2{ 0 };
	std::uint8_t m_Star3{ 0 };

public:
	CStarTriangle() = default;

	explicit constexpr CStarTriangle(const size_t Star1, const size_t Star2, const size_t Star3, float fX, float fY) noexcept :
		m_fX{ fX },
		m_fY{ fY },
		m_Star1{ static_cast<decltype(m_Star1)>(Star1) },
		m_Star2{ static_cast<decltype(m_Star2)>(Star2) },
		m_Star3{ static_cast<decltype(m_Star3)>(Star3) }
	{}

	CStarTriangle(const CStarTriangle&) = default;
	CStarTriangle& operator=(const CStarTriangle&) = default;
	CStarTriangle(CStarTriangle&&) = default;
	~CStarTriangle() = default;

	constexpr friend bool operator==(CStarTriangle const& lhs, CStarTriangle const& rhs) noexcept
	{
		return lhs.m_fX == rhs.m_fX;
	}
	constexpr friend auto operator<=>(CStarTriangle const& lhs, CStarTriangle const& rhs) noexcept
	{
		return lhs.m_fX <=> rhs.m_fX;
	}
};

/* ------------------------------------------------------------------- */

#pragma pack(pop, STARTRIANGLE)

using STARTRIANGLEVECTOR = std::vector<CStarTriangle>;

/* ------------------------------------------------------------------- */

typedef enum BITMAPCORNER
{
	BC_TOPLEFT		= 0,
	BC_TOPRIGHT		= 1,
	BC_BOTTOMLEFT	= 2,
	BC_BOTTOMRIGHT	= 3
} BITMAPCORNER;


/* ------------------------------------------------------------------- */

class CStarDist final
{
public:
	std::uint8_t m_Star1;
	std::uint8_t m_Star2;
	float m_fDistance;

public:
	explicit constexpr CStarDist(const size_t Star1, const size_t Star2, const float d) noexcept
	{
		if (Star1 < Star2)
		{
			m_Star1 = static_cast<decltype(m_Star1)>(Star1);
			m_Star2 = static_cast<decltype(m_Star2)>(Star2);
		}
		else
		{
			m_Star1 = static_cast<decltype(m_Star1)>(Star2);
			m_Star2 = static_cast<decltype(m_Star2)>(Star1);
		}

		m_fDistance = d;
	}

	explicit constexpr CStarDist(const size_t star1, const size_t star2) noexcept : CStarDist(star1, star2, 0.0f) {}

	CStarDist(const CStarDist&) = default;
	CStarDist(CStarDist&&) = default;
	CStarDist& operator=(const CStarDist&) = default;
	~CStarDist() = default;

	constexpr friend bool operator==(CStarDist const& lhs, CStarDist const& rhs) noexcept
	{
		return lhs.m_Star1 == rhs.m_Star1 && lhs.m_Star2 == rhs.m_Star2;
	}

	constexpr friend auto operator<=>(CStarDist const& lhs, CStarDist const& rhs) noexcept
	{
		const auto cmp = lhs.m_Star1 <=> rhs.m_Star1;
		return cmp != 0 ? cmp : lhs.m_Star2 <=> rhs.m_Star2;
//		return lhs.m_Star1 == rhs.m_Star1 ? (lhs.m_Star2 <=> rhs.m_Star2) : (lhs.m_Star1 <=> rhs.m_Star1);
	}
};

using STARDISTVECTOR = std::vector<CStarDist>;

constexpr int VPFLAG_ACTIVE				= 0x00000001;
constexpr int VPFLAG_CORNER_TOPLEFT		= 0x00000010;
constexpr int VPFLAG_CORNER_TOPRIGHT	= 0x00000020;
constexpr int VPFLAG_CORNER_BOTTOMLEFT  = 0x00000040;
constexpr int VPFLAG_CORNER_BOTTOMRIGHT	= 0x00000080;
constexpr int VPFLAG_CORNER_MASK		= 0x000000F0;
constexpr int VPFLAG_USED				= 0x00000100;
constexpr int VPFLAG_LOCKED				= 0x00000200;
constexpr int VPFLAG_POSSIBLE			= 0x00000400;

class VotingPair final
{
public:
	std::int8_t	m_RefStar{ 0 };
	std::int8_t m_TgtStar{ 0 };
	int m_lNrVotes{ 0 };
	int m_Flags{ VPFLAG_ACTIVE };

public:
	VotingPair() = default;

	constexpr explicit VotingPair(const int RefStar, const int TgtStar) noexcept :
		m_RefStar{ static_cast<decltype(m_RefStar)>(RefStar)},
		m_TgtStar{ static_cast<decltype(m_TgtStar)>(TgtStar) },
		m_lNrVotes{ 0 },
		m_Flags{ VPFLAG_ACTIVE }
	{}

	VotingPair(const VotingPair&) = default;

	VotingPair& operator=(const VotingPair&) = default;

	~VotingPair() = default;


	bool IsActive() const
	{
		return (m_Flags & VPFLAG_ACTIVE);
	}

	void SetActive(bool bActive)
	{
		if (bActive)
			m_Flags |= VPFLAG_ACTIVE;
		else
			m_Flags &= ~VPFLAG_ACTIVE;
	}

	bool IsCorner() const
	{
		return (m_Flags & VPFLAG_CORNER_MASK);
	}

	bool IsUsed() const
	{
		return (m_Flags & VPFLAG_USED);
	}

	void SetUsed(bool bUsed)
	{
		if (bUsed)
			m_Flags |= VPFLAG_USED;
		else
			m_Flags &= ~VPFLAG_USED;
	}

	bool IsLocked() const
	{
		return (m_Flags & VPFLAG_LOCKED);
	}

	void SetLocked(bool bLocked)
	{
		if (bLocked)
			m_Flags |= VPFLAG_LOCKED;
		else
			m_Flags &= ~VPFLAG_LOCKED;
	}

	bool IsPossible() const
	{
		return (m_Flags & VPFLAG_POSSIBLE);
	}

	void SetPossible(bool bPossible)
	{
		if (bPossible)
			m_Flags |= VPFLAG_POSSIBLE;
		else
			m_Flags &= ~VPFLAG_POSSIBLE;
	}

	constexpr friend bool operator==(VotingPair const& lhs, VotingPair const& rhs) noexcept
	{
		return lhs.m_lNrVotes == rhs.m_lNrVotes;
	}

	constexpr friend auto operator<=>(VotingPair const& lhs, VotingPair const& rhs) noexcept
	{
		return lhs.m_lNrVotes <=> rhs.m_lNrVotes;
	}
};

using VOTINGPAIRVECTOR = std::vector<VotingPair>;
using QPointFVector = std::vector<QPointF>;

class CBilinearParameters;

class CMatchingStars final
{
private:
	QPointFVector m_vRefStars;
	QPointFVector m_vTgtStars;
	QPointFVector m_vRefCorners;
	QPointFVector m_vTgtCorners;
	STARTRIANGLEVECTOR m_vRefTriangles;
	STARTRIANGLEVECTOR m_vTgtTriangles;
	std::vector<int> m_vRefStarIndices;
//	std::vector<int> m_vTgtStarIndices;
	STARDISTVECTOR m_vRefStarDistances;
//	STARDISTVECTOR m_vTgtStarDistances;
	VOTINGPAIRVECTOR m_vVotedPairs;
	int m_lWidth{ 0 };
	int m_lHeight{ 0 };

private:
	QPointF& RefStar(const VotingPair& vp)
	{
		return vp.IsCorner() ? m_vRefCorners[vp.m_RefStar] : m_vRefStars[vp.m_RefStar];
	}

	QPointF& TgtStar(const VotingPair& vp)
	{
		return vp.IsCorner() ? m_vTgtCorners[vp.m_TgtStar] : m_vTgtStars[vp.m_TgtStar];
	}

	void InitVotingGrid(VOTINGPAIRVECTOR& vVotingPairs);
//	void AdjustVoting(const VOTINGPAIRVECTOR& vInVotingPairs, VOTINGPAIRVECTOR& vOutVotingPairs, int lNrTgtStars);
	STARDISTVECTOR ComputeStarDistances(const QPointFVector& vStars);
	void ComputeTriangles(const QPointFVector& vStars, STARTRIANGLEVECTOR& vTriangles);
	template <typename... DistanceVector>
	std::pair<double, size_t> ComputeDistanceBetweenStars(const VOTINGPAIRVECTOR& vVotingPairs, const CBilinearParameters& projection, DistanceVector&... distances);
	bool ComputeCoordinatesTransformation(VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, const TRANSFORMATIONTYPE TType);
	bool ComputeTransformation(std::span<const VotingPair> vVotingPairs, CBilinearParameters& BilinearParameters, const TRANSFORMATIONTYPE TType);
	bool ComputeSigmaClippingTransformation(const VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, const TRANSFORMATIONTYPE TType);
//	bool ComputeMedianTransformation(const VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, TRANSFORMATIONTYPE TType);
	bool ComputeMatchingTriangleTransformation(CBilinearParameters& BilinearParameters);
	bool ComputeLargeTriangleTransformation(CBilinearParameters& BilinearParameters);
	void AdjustSize();

public:
	CMatchingStars() = default;
	explicit constexpr CMatchingStars(const int width, const int height) : m_lWidth{ width }, m_lHeight{ height }
	{}
	~CMatchingStars() = default;

	void AddReferenceStar(double fX, double fY)
	{
		m_vRefStars.emplace_back(fX, fY);
	}

	void AddTargetedStar(double fX, double fY)
	{
		m_vTgtStars.emplace_back(fX, fY);
	}

	bool IsReferenceSet() const
	{
		return !m_vRefStars.empty();
	}

	void ClearReference()
	{
		m_vRefStars.clear();
		m_vRefTriangles.clear();
		m_vRefCorners.clear();
		m_vRefStarDistances.clear();
		m_vRefStarIndices.clear();
	}

	void ClearTarget()
	{
		m_vTgtStars.clear();
		m_vTgtTriangles.clear();
		m_vTgtCorners.clear();
//		m_vTgtStarDistances.clear();
//		m_vTgtStarIndices.clear();
	}

	void SetSizes(const int lWidth, const int lHeight)
	{
		m_lWidth = lWidth;
		m_lHeight = lHeight;
	}

	bool ComputeCoordinateTransformation(CBilinearParameters& BilinearParameters);

	VOTINGPAIRVECTOR GetVotedPairsCopy() const
	{
		return m_vVotedPairs;
	}
};
