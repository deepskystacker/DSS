#pragma once

#if !defined(NDEBUG)
#include <boost/container/vector.hpp>
namespace bc = boost::container;
#endif
#include "DSSTools.h"
#include "BilinearParameters.h"

#pragma pack(push, STARTRIANGLE, 1)

/* ------------------------------------------------------------------- */

class CStarTriangle
{
public :
	float			m_fX,
					m_fY;
	std::uint8_t	m_Star1;
	std::uint8_t	m_Star2;
	std::uint8_t	m_Star3;

private :
	void	CopyFrom(const CStarTriangle & st)
	{
		m_fX	= st.m_fX;
		m_fY	= st.m_fY;
		m_Star1 = st.m_Star1;
		m_Star2 = st.m_Star2;
		m_Star3 = st.m_Star3;
	};

public :
    CStarTriangle()
    {
        m_fX = 0;
        m_fY = 0;
        m_Star1 = 0;
        m_Star2 = 0;
        m_Star3 = 0;
    }

	CStarTriangle(std::uint8_t Star1, std::uint8_t Star2, std::uint8_t Star3, float fX, float fY)
	{
		m_Star1 = Star1;
		m_Star2 = Star2;
		m_Star3 = Star3;
		m_fX	= fX;
		m_fY	= fY;
	};

	CStarTriangle(const CStarTriangle & st)
	{
		CopyFrom(st);
	};

	const CStarTriangle & operator = (const CStarTriangle & st)
	{
		CopyFrom(st);
		return (*this);
	};

	virtual ~CStarTriangle()
	{
	};

	bool operator < (const CStarTriangle & st) const
	{
		return m_fX < st.m_fX;
	};
};

/* ------------------------------------------------------------------- */

#pragma pack(pop, STARTRIANGLE)

typedef std::vector<CStarTriangle>		STARTRIANGLEVECTOR;
typedef STARTRIANGLEVECTOR::iterator	STARTRIANGLEITERATOR;

/* ------------------------------------------------------------------- */

typedef enum BITMAPCORNER
{
	BC_TOPLEFT		= 0,
	BC_TOPRIGHT		= 1,
	BC_BOTTOMLEFT	= 2,
	BC_BOTTOMRIGHT	= 3
}BITMAPCORNER;

class CStarCornerDist
{
public :
	std::uint8_t	m_Star;
	float			m_fCornerDist;
	BITMAPCORNER	m_Corner;

private :
	void	CopyFrom(const CStarCornerDist & scd)
	{
		m_Star		  = scd.m_Star;
		m_fCornerDist = scd.m_fCornerDist;
		m_Corner	  = scd.m_Corner;
	};

public :
	CStarCornerDist()
	{
		m_Star = 0;
		m_fCornerDist = 0;
		m_Corner      = BC_TOPLEFT;
	};

	CStarCornerDist(const CStarCornerDist & scd)
	{
		CopyFrom(scd);
	};

	const CStarCornerDist & operator = (const CStarCornerDist & scd)
	{
		CopyFrom(scd);
		return (*this);
	};

	bool operator < (const CStarCornerDist & scd) const
	{
		return m_fCornerDist < scd.m_fCornerDist;
	};
};

typedef std::vector<CStarCornerDist>			STARCORNERDISTVECTOR;

/* ------------------------------------------------------------------- */

class CStarDist
{
public :
	std::uint8_t	m_Star1;
	std::uint8_t	m_Star2;
	float			m_fDistance;

private :
	void	CopyFrom(const CStarDist & sd)
	{
		m_Star1 = sd.m_Star1;
		m_Star2 = sd.m_Star2;
		m_fDistance = sd.m_fDistance;
	};

public :
	CStarDist(std::uint8_t Star1, std::uint8_t Star2, float fDistance = 0.0)
	{
		if (Star1 < Star2)
		{
			m_Star1 = Star1;
			m_Star2 = Star2;
		}
		else
		{
			m_Star1 = Star2;
			m_Star2 = Star1;
		};

		m_fDistance = fDistance;
	};

	CStarDist(const CStarDist & sd)
	{
		CopyFrom(sd);
	};

	virtual ~CStarDist()
	{
	};

	const CStarDist & operator = (const CStarDist & sd)
	{
		CopyFrom(sd);
		return (*this);
	};

	bool operator < (const CStarDist & sd) const
	{
		if (m_Star1 < sd.m_Star1)
			return true;
		else if (m_Star1 > sd.m_Star1)
			return false;
		else
			return (m_Star2 < sd.m_Star2);
	};
};

#if !defined(NDEBUG)
typedef bc::vector<CStarDist>			STARDISTVECTOR;
#else
typedef std::vector<CStarDist>			STARDISTVECTOR;
#endif
typedef STARDISTVECTOR::iterator		STARDISTITERATOR;

/* ------------------------------------------------------------------- */

constexpr int VPFLAG_ACTIVE				= 0x00000001;
constexpr int VPFLAG_CORNER_TOPLEFT		= 0x00000010;
constexpr int VPFLAG_CORNER_TOPRIGHT	= 0x00000020;
constexpr int VPFLAG_CORNER_BOTTOMLEFT  = 0x00000040;
constexpr int VPFLAG_CORNER_BOTTOMRIGHT	= 0x00000080;
constexpr int VPFLAG_CORNER_MASK		= 0x000000F0;
constexpr int VPFLAG_USED				= 0x00000100;
constexpr int VPFLAG_LOCKED				= 0x00000200;
constexpr int VPFLAG_POSSIBLE			= 0x00000400;

class CVotingPair
{
public :
	std::int8_t	m_RefStar,
				m_TgtStar;
	int					m_lNrVotes;
	int					m_Flags;

private :
	void	CopyFrom(const CVotingPair & vp)
	{
		m_RefStar	= vp.m_RefStar;
		m_TgtStar	= vp.m_TgtStar;
		m_lNrVotes	= vp.m_lNrVotes;
		m_Flags     = vp.m_Flags;
	};

public :
	CVotingPair(std::uint8_t RefStar = 0, std::uint8_t TgtStar = 0)
	{
		m_RefStar	= RefStar;
		m_TgtStar	= TgtStar;
		m_lNrVotes	= 0;
		m_Flags     = VPFLAG_ACTIVE;
	};

	CVotingPair(const CVotingPair & vp)
	{
		CopyFrom(vp);
	};

	bool	IsActive() const
	{
		return (m_Flags & VPFLAG_ACTIVE);
	};

	void	SetActive(bool bActive)
	{
		if (bActive)
			m_Flags |= VPFLAG_ACTIVE;
		else
			m_Flags &=~VPFLAG_ACTIVE;
	};

	bool	IsCorner() const
	{
		return (m_Flags & VPFLAG_CORNER_MASK);
	};

	bool	IsUsed() const
	{
		return (m_Flags & VPFLAG_USED);
	};

	void	SetUsed(bool bUsed)
	{
		if (bUsed)
			m_Flags |= VPFLAG_USED;
		else
			m_Flags &=~VPFLAG_USED;
	};

	bool	IsLocked() const
	{
		return (m_Flags & VPFLAG_LOCKED);
	};

	void	SetLocked(bool bLocked)
	{
		if (bLocked)
			m_Flags |= VPFLAG_LOCKED;
		else
			m_Flags &=~VPFLAG_LOCKED;
	};

	bool	IsPossible() const
	{
		return (m_Flags & VPFLAG_POSSIBLE);
	};

	void	SetPossible(bool bPossible)
	{
		if (bPossible)
			m_Flags |= VPFLAG_POSSIBLE;
		else
			m_Flags &=~VPFLAG_POSSIBLE;
	};

	const CVotingPair & operator = (const CVotingPair & vp)
	{
		CopyFrom(vp);
		return (*this);
	};

	virtual ~CVotingPair()
	{
	};

	bool operator<(const CVotingPair& vp) const
	{
		return m_lNrVotes > vp.m_lNrVotes; // Reverse sort
	}
};

typedef std::vector<CVotingPair>		VOTINGPAIRVECTOR;
typedef VOTINGPAIRVECTOR::iterator		VOTINGPAIRITERATOR;

/* ------------------------------------------------------------------- */
class CBilinearParameters;
class CMatchingStars final
{
private:
	POINTFVECTOR m_vRefStars;
	POINTFVECTOR m_vTgtStars;
	POINTFVECTOR m_vRefCorners;
	POINTFVECTOR m_vTgtCorners;
	STARTRIANGLEVECTOR m_vRefTriangles;
	STARTRIANGLEVECTOR m_vTgtTriangles;
	std::vector<int> m_vRefStarIndices;
	std::vector<int> m_vTgtStarIndices;
	STARDISTVECTOR m_vRefStarDistances;
	STARDISTVECTOR m_vTgtStarDistances;
	VOTINGPAIRVECTOR m_vVotedPairs;
	int m_lWidth{ 0 };
	int m_lHeight{ 0 };

private:
	QPointF& RefStar(const CVotingPair& vp)
	{
		return vp.IsCorner() ? m_vRefCorners[vp.m_RefStar] : m_vRefStars[vp.m_RefStar];
	}

	QPointF& TgtStar(const CVotingPair& vp)
	{
		return vp.IsCorner() ? m_vTgtCorners[vp.m_TgtStar] : m_vTgtStars[vp.m_TgtStar];
	}

	void InitVotingGrid(VOTINGPAIRVECTOR& vVotingPairs);
	void AdjustVoting(const VOTINGPAIRVECTOR& vInVotingPairs, VOTINGPAIRVECTOR& vOutVotingPairs, int lNrTgtStars);
	void ComputeStarDistances(const POINTFVECTOR& vStars, STARDISTVECTOR& vStarDist);
	void ComputeTriangles(const POINTFVECTOR& vStars, STARTRIANGLEVECTOR& vTriangles);
	double ValidateTransformation(const VOTINGPAIRVECTOR& vVotingPairs, const CBilinearParameters& BilinearParameters);
	bool ComputeCoordinatesTransformation(VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, TRANSFORMATIONTYPE TType);
	bool ComputeTransformation(const VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, TRANSFORMATIONTYPE TType);
	bool ComputeSigmaClippingTransformation(const VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, TRANSFORMATIONTYPE TType);
	bool ComputeMedianTransformation(const VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, TRANSFORMATIONTYPE TType);
	bool ComputeMatchingTriangleTransformation(CBilinearParameters& BilinearParameters);
	bool ComputeLargeTriangleTransformation(CBilinearParameters& BilinearParameters);
	void AdjustSize();

public:
	CMatchingStars() = default;
	explicit CMatchingStars(const int width, const int height) : m_lWidth{ width }, m_lHeight{ height }
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

	bool IsReferenceSet()
	{
		return (m_vRefStars.size() > 0);
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
		m_vTgtStarDistances.clear();
		m_vTgtStarIndices.clear();
	}

	void SetSizes(int lWidth, int lHeight)
	{
		m_lWidth	= lWidth;
		m_lHeight	= lHeight;
	}

	bool ComputeCoordinateTransformation(CBilinearParameters& BilinearParameters);
	void GetVotedPairs(VOTINGPAIRVECTOR& vPairs)
	{
		vPairs = m_vVotedPairs;
	}
};
