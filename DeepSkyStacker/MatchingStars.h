#ifndef __MATCHINGSTARS_H__
#define __MATCHINGSTARS_H__

#include "DSSTools.h"

#pragma pack(push, STARTRIANGLE, 1)

/* ------------------------------------------------------------------- */

class CStarTriangle
{
public :
	float			m_fX,
					m_fY;
	BYTE			m_Star1,
					m_Star2,
					m_Star3;

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
	};

	CStarTriangle(BYTE Star1, BYTE Star2, BYTE Star3, float fX, float fY)
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
	BYTE			m_Star;
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
	BYTE			m_Star1,
					m_Star2;
	float			m_fDistance;

private :
	void	CopyFrom(const CStarDist & sd)
	{
		m_Star1 = sd.m_Star1;
		m_Star2 = sd.m_Star2;
		m_fDistance = sd.m_fDistance;
	};

public :
	CStarDist(BYTE Star1, BYTE Star2, float fDistance = 0.0)
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

typedef std::vector<CStarDist>			STARDISTVECTOR;
typedef STARDISTVECTOR::iterator		STARDISTITERATOR;

/* ------------------------------------------------------------------- */

const	LONG			VPFLAG_ACTIVE			  = 0x00000001;
const	LONG			VPFLAG_CORNER_TOPLEFT	  = 0x00000010;
const	LONG			VPFLAG_CORNER_TOPRIGHT    = 0x00000020;
const	LONG			VPFLAG_CORNER_BOTTOMLEFT  = 0x00000040;
const	LONG			VPFLAG_CORNER_BOTTOMRIGHT = 0x00000080;
const	LONG			VPFLAG_CORNER_MASK		  = 0x000000F0;
const	LONG			VPFLAG_USED				  = 0x00000100;
const	LONG			VPFLAG_LOCKED			  = 0x00000200;
const	LONG			VPFLAG_POSSIBLE			  = 0x00000400;

class CVotingPair
{
public :
	__int8					m_RefStar,
							m_TgtStar;
	LONG					m_lNrVotes;
	LONG					m_Flags;

private :
	void	CopyFrom(const CVotingPair & vp)
	{
		m_RefStar	= vp.m_RefStar;
		m_TgtStar	= vp.m_TgtStar;
		m_lNrVotes	= vp.m_lNrVotes;
		m_Flags     = vp.m_Flags;
	};

public :
	CVotingPair(BYTE RefStar = 0, BYTE TgtStar = 0)
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

	BOOL	IsActive() const
	{
		return (m_Flags & VPFLAG_ACTIVE);
	};

	void	SetActive(BOOL bActive)
	{
		if (bActive)
			m_Flags |= VPFLAG_ACTIVE;
		else
			m_Flags &=~VPFLAG_ACTIVE;
	};

	BOOL	IsCorner() const
	{
		return (m_Flags & VPFLAG_CORNER_MASK);
	};

	BOOL	IsUsed() const
	{
		return (m_Flags & VPFLAG_USED);
	};

	void	SetUsed(BOOL bUsed)
	{
		if (bUsed)
			m_Flags |= VPFLAG_USED;
		else
			m_Flags &=~VPFLAG_USED;
	};

	BOOL	IsLocked() const
	{
		return (m_Flags & VPFLAG_LOCKED);
	};

	void	SetLocked(BOOL bLocked)
	{
		if (bLocked)
			m_Flags |= VPFLAG_LOCKED;
		else
			m_Flags &=~VPFLAG_LOCKED;
	};

	BOOL	IsPossible() const
	{
		return (m_Flags & VPFLAG_POSSIBLE);
	};

	void	SetPossible(BOOL bPossible)
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

	bool operator < (const CVotingPair & vp)
	{
		return m_lNrVotes > vp.m_lNrVotes; // Reverse sort
	};
};

typedef std::vector<CVotingPair>		VOTINGPAIRVECTOR;
typedef VOTINGPAIRVECTOR::iterator		VOTINGPAIRITERATOR;

/* ------------------------------------------------------------------- */

class CMatchingStars
{
private :
	POINTEXTVECTOR			m_vRefStars;
	POINTEXTVECTOR			m_vTgtStars;

	STARTRIANGLEVECTOR		m_vRefTriangles;
	STARTRIANGLEVECTOR		m_vTgtTriangles;

	std::vector<LONG>		m_vRefStarIndices;
	std::vector<LONG>		m_vTgtStarIndices;

	STARDISTVECTOR			m_vRefStarDistances;
	STARDISTVECTOR			m_vTgtStarDistances;

	VOTINGPAIRVECTOR		m_vVotedPairs;
	LONG					m_lWidth;
	LONG					m_lHeight;

	POINTEXTVECTOR			m_vRefCorners;
	POINTEXTVECTOR			m_vTgtCorners;

private :
	CPointExt & RefStar(const CVotingPair & vp)
	{
		if (vp.IsCorner())
			return m_vRefCorners[vp.m_RefStar];
		else
			return m_vRefStars[vp.m_RefStar];
	}

	CPointExt & TgtStar(const CVotingPair & vp)
	{
		if (vp.IsCorner())
			return m_vTgtCorners[vp.m_TgtStar];
		else
			return m_vTgtStars[vp.m_TgtStar];
	};

	void	InitVotingGrid(VOTINGPAIRVECTOR & vVotingPairs);
	void	AdjustVoting(const VOTINGPAIRVECTOR & vInVotingPairs, VOTINGPAIRVECTOR & vOutVotingPairs, LONG lNrTgtStars);
	void	ComputeStarDistances(const POINTEXTVECTOR & vStars, STARDISTVECTOR & vStarDist);
	void	ComputeTriangles(const POINTEXTVECTOR & vStars, STARTRIANGLEVECTOR & vTriangles);

	double	ValidateTransformation(const VOTINGPAIRVECTOR & vVotingPairs, const CBilinearParameters & BilinearParameters);
	BOOL	ComputeCoordinatesTransformation(VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE TType);
	BOOL	ComputeTransformation(const VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE TType);
	BOOL	ComputeSigmaClippingTransformation(const VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE TType);
	BOOL	ComputeMedianTransformation(const VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE TType);

	BOOL	ComputeMatchingTriangleTransformation(CBilinearParameters & BilinearParameters);
	BOOL	ComputeLargeTriangleTransformation(CBilinearParameters & BilinearParameters);

	void	AdjustSize();

public :
	CMatchingStars()
	{
		m_lWidth = 0;
		m_lHeight = 0;
	};

	virtual ~CMatchingStars()
	{
	};

	void	AddReferenceStar(double fX, double fY)
	{
		m_vRefStars.emplace_back(fX, fY);
	};

	void	AddTargetedStar(double fX, double fY)
	{
		m_vTgtStars.emplace_back(fX, fY);
	};

	BOOL	IsReferenceSet()
	{
		return (m_vRefStars.size() > 0);
	};

	void	ClearReference()
	{
		m_vRefStars.clear();
		m_vRefTriangles.clear();
		m_vRefCorners.clear();
		m_vRefStarDistances.clear();
		m_vRefStarIndices.clear();
	};

	void	ClearTarget()
	{
		m_vTgtStars.clear();
		m_vTgtTriangles.clear();
		m_vTgtCorners.clear();
		m_vTgtStarDistances.clear();
		m_vTgtStarIndices.clear();
	};

	void	SetSizes(LONG lWidth, LONG lHeight)
	{
		m_lWidth	= lWidth;
		m_lHeight	= lHeight;
	};

	BOOL	ComputeCoordinateTransformation(CBilinearParameters & BilinearParameters);
	void	GetVotedPairs(VOTINGPAIRVECTOR & vPairs)
	{
		vPairs = m_vVotedPairs;
	};
};

#endif // __MATCHINGSTARS_H__

