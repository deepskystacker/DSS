#include <stdafx.h>
#include "MatchingStars.h"
#include "ZExcept.h"

#define _NO_EXCEPTION
#include "Matrix.h"
#include "Workspace.h"

constexpr int		MINPAIRSTOBISQUARED = 25;
constexpr int		MINPAIRSTOBICUBIC	= 40;

/* ------------------------------------------------------------------- */

static TRANSFORMATIONTYPE	GetTransformationType(int lNrVotingPairs = 2000)
{
	std::uint32_t dwAlignmentTransformation = 2;
	TRANSFORMATIONTYPE		TTResult = TT_BILINEAR;
	Workspace				workspace;

	dwAlignmentTransformation = workspace.value("Stacking/AlignmentTransformation", (uint)2).toUInt();

	if (dwAlignmentTransformation > TT_LAST)
		dwAlignmentTransformation = 0;

	if (!dwAlignmentTransformation)
	{
		// Automatic - no forcing
		if (lNrVotingPairs >= MINPAIRSTOBICUBIC)
			TTResult = TT_BICUBIC;
		else if (lNrVotingPairs >= MINPAIRSTOBISQUARED)
			TTResult = TT_BISQUARED;
		else
			TTResult = TT_BILINEAR;
	}
	else
	{
		if (dwAlignmentTransformation == 5)
			TTResult = TT_NONE;
		else if (dwAlignmentTransformation == 4 && lNrVotingPairs >= MINPAIRSTOBICUBIC)
			TTResult = TT_BICUBIC;
		else if (dwAlignmentTransformation >= 3 && lNrVotingPairs >= MINPAIRSTOBISQUARED)
			TTResult = TT_BISQUARED;
		else if (dwAlignmentTransformation >= 2)
			TTResult = TT_BILINEAR;
		else
			TTResult = TT_LINEAR;

	};

	return TTResult;
};

/* ------------------------------------------------------------------- */

static bool AreCornersLocked()
{
	Workspace				workspace;

	return workspace.value("Stacking/LockCorners", true).toBool();
};

/* ------------------------------------------------------------------- */

void	CMatchingStars::ComputeStarDistances(const POINTFVECTOR & vStars, STARDISTVECTOR & vStarDist)
{
	int				i, j;

	double				fMaxDistance = 0;

	vStarDist.reserve(vStars.size() * (vStars.size()-1)/2);

	for (i = 0;i<vStars.size();i++)
	{
		for (j = i+1;j<vStars.size();j++)
		{
			double			fDistance;

			fDistance = Distance(vStars[i].x(), vStars[i].y(), vStars[j].x(), vStars[j].y());
			fMaxDistance = max(fDistance, fMaxDistance);

			vStarDist.emplace_back(i, j, fDistance);
		};
	};

	std::sort(vStarDist.begin(), vStarDist.end());
};

/* ------------------------------------------------------------------- */

void	CMatchingStars::ComputeTriangles(const POINTFVECTOR & vStars, STARTRIANGLEVECTOR & vTriangles)
{
	ZFUNCTRACE_RUNTIME();
	STARDISTVECTOR			vStarDist;
	int					i, j, k;
	STARDISTITERATOR		it;
	std::vector<float>		vDistances;

	ComputeStarDistances(vStars, vStarDist);
	vDistances.resize(3);

	for (i = 0;i<vStars.size();i++)
	{
		for (j = i+1;j<vStars.size();j++)
		{
			it = std::lower_bound(vStarDist.begin(), vStarDist.end(), CStarDist(i, j));
			vDistances[0] = (*it).m_fDistance;

			for (k = j+1;k<vStars.size();k++)
			{
				it = std::lower_bound(vStarDist.begin(), vStarDist.end(), CStarDist(j, k));
				vDistances[1] = (*it).m_fDistance;
				it = std::lower_bound(vStarDist.begin(), vStarDist.end(), CStarDist(i, k));
				vDistances[2] = (*it).m_fDistance;

				std::sort(vDistances.begin(), vDistances.end());

				if (vDistances[2] > 0)
				{
					float		fX, fY;

					fX = vDistances[1]/vDistances[2];
					fY = vDistances[0]/vDistances[2];

					// Filter
					if (fX < 0.9)
					{
						// Add to the triangle list
						vTriangles.push_back(CStarTriangle(i, j, k, fX, fY));
					};
				};
			};
		};
	};

	std::sort(vTriangles.begin(), vTriangles.end());
};

/* ------------------------------------------------------------------- */

void	CMatchingStars::InitVotingGrid(VOTINGPAIRVECTOR & vVotingPairs)
{
	vVotingPairs.clear();
	vVotingPairs.reserve(m_vRefStars.size() * m_vTgtStars.size());

	for (int i = 0;i<m_vRefStars.size();i++)
	{
		for (int j = 0;j<m_vTgtStars.size();j++)
		{
			vVotingPairs.push_back(CVotingPair(i, j));
		};
	};
};

/* ------------------------------------------------------------------- */

inline void	AddVote(std::uint8_t RefStar, std::uint8_t TgtStar, VOTINGPAIRVECTOR & vVotingPairs, int lNrTgtStars)
{
	int				lOffset = RefStar * lNrTgtStars + TgtStar;

	vVotingPairs[lOffset].m_lNrVotes++;
};

/* ------------------------------------------------------------------- */

void CMatchingStars::AdjustVoting(const VOTINGPAIRVECTOR & vInVotingPairs, VOTINGPAIRVECTOR & vOutVotingPairs, int lNrTgtStars)
{
	int			i, j;

	for (i = 0;i<vInVotingPairs.size();i++)
	{
		int		lMaxVotes1 = 0,
					lMaxVotes2 = 0;

		// compute max votes for the same reference star
		for (j = (vInVotingPairs[i].m_RefStar)*lNrTgtStars;j<(vInVotingPairs[i].m_RefStar+1)*lNrTgtStars;j++)
		{
			ZASSERT(vInVotingPairs[j].m_RefStar == vInVotingPairs[i].m_RefStar);
			if (vInVotingPairs[j].m_TgtStar != vInVotingPairs[i].m_TgtStar)
				lMaxVotes1 = max(lMaxVotes1, vInVotingPairs[j].m_lNrVotes);
		};

		// compute max votes for the same target star
		for (j = vInVotingPairs[i].m_TgtStar;j<vInVotingPairs.size();j+=lNrTgtStars)
		{
			ZASSERT(vInVotingPairs[j].m_TgtStar == vInVotingPairs[i].m_TgtStar);
			if (vInVotingPairs[j].m_RefStar != vInVotingPairs[i].m_RefStar)
				lMaxVotes2 = max(lMaxVotes2, vInVotingPairs[j].m_lNrVotes);
		};

		vOutVotingPairs[i].m_lNrVotes = vInVotingPairs[i].m_lNrVotes - max(lMaxVotes1, lMaxVotes2);
		if (vOutVotingPairs[i].m_lNrVotes<0)
			vOutVotingPairs[i].m_lNrVotes = 0;
	};
};

/* ------------------------------------------------------------------- */

typedef math::matrix<double>		DMATRIX;

bool CMatchingStars::ComputeTransformation(const VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE TType)
{
	bool				bResult = false;
	int				i;
	double				fXWidth = m_lWidth,
						fYWidth = m_lHeight;

	BilinearParameters.fXWidth = fXWidth;
	BilinearParameters.fYWidth = fYWidth;

	if (TType == TT_BICUBIC)
	{
		DMATRIX				M((int)vVotingPairs.size(), 16);
		DMATRIX				X((int)vVotingPairs.size(), 1);
		DMATRIX				Y((int)vVotingPairs.size(), 1);

		for (i = 0;i<vVotingPairs.size();i++)
		{
			QPointF &		Star = RefStar(vVotingPairs[i]);
			X(i, 0) = Star.x()/fXWidth;
			Y(i, 0) = Star.y()/fYWidth;
		};

		for (i = 0;i<vVotingPairs.size();i++)
		{
			QPointF &		Star = TgtStar(vVotingPairs[i]);
#pragma warning (suppress:4456)
			double			X = Star.x()/fXWidth;
			double			X2 = X * X;
			double			X3 = X * X * X;
#pragma warning (suppress:4456)
			double			Y = Star.y()/fYWidth;
			double			Y2 = Y * Y;
			double			Y3 = Y * Y * Y;

			M(i, 0) = 1;
			M(i, 1) = X;
			M(i, 2) = Y;
			M(i, 3) = X*Y;
			M(i, 4) = X2;
			M(i, 5) = Y2;
			M(i, 6) = X2*Y;
			M(i, 7) = X*Y2;
			M(i, 8) = X2*Y2;
			M(i, 9) = X3;
			M(i, 10) = Y3;
			M(i, 11) = X3*Y;
			M(i, 12) = X*Y3;
			M(i, 13) = X3*Y2;
			M(i, 14) = X2*Y3;
			M(i, 15) = X3*Y3;
		};

		DMATRIX				MT = ~M;
		DMATRIX				TM = MT * M;

		try
		{
			if (!TM.IsSingular())
			{
				DMATRIX				A = !TM * MT * X;
				DMATRIX				B = !TM * MT * Y;

				BilinearParameters.Type = TType;
				BilinearParameters.a0 = A(0, 0);
				BilinearParameters.a1 = A(1, 0);
				BilinearParameters.a2 = A(2, 0);
				BilinearParameters.a3 = A(3, 0);
				BilinearParameters.a4 = A(4, 0);
				BilinearParameters.a5 = A(5, 0);
				BilinearParameters.a6 = A(6, 0);
				BilinearParameters.a7 = A(7, 0);
				BilinearParameters.a8 = A(8, 0);
				BilinearParameters.a9 = A(9, 0);
				BilinearParameters.a10 = A(10, 0);
				BilinearParameters.a11 = A(11, 0);
				BilinearParameters.a12 = A(12, 0);
				BilinearParameters.a13 = A(13, 0);
				BilinearParameters.a14 = A(14, 0);
				BilinearParameters.a15 = A(15, 0);

				BilinearParameters.b0 = B(0, 0);
				BilinearParameters.b1 = B(1, 0);
				BilinearParameters.b2 = B(2, 0);
				BilinearParameters.b3 = B(3, 0);
				BilinearParameters.b4 = B(4, 0);
				BilinearParameters.b5 = B(5, 0);
				BilinearParameters.b6 = B(6, 0);
				BilinearParameters.b7 = B(7, 0);
				BilinearParameters.b8 = B(8, 0);
				BilinearParameters.b9 = B(9, 0);
				BilinearParameters.b10 = B(10, 0);
				BilinearParameters.b11 = B(11, 0);
				BilinearParameters.b12 = B(12, 0);
				BilinearParameters.b13 = B(13, 0);
				BilinearParameters.b14 = B(14, 0);
				BilinearParameters.b15 = B(15, 0);

				bResult = true;
			};
		}
		catch(math::matrix_error const&)
		{
			bResult = false;
		};
	}
	else if (TType == TT_BISQUARED)
	{
		DMATRIX				M(vVotingPairs.size(), 9);
		DMATRIX				X(vVotingPairs.size(), 1);
		DMATRIX				Y(vVotingPairs.size(), 1);

		for (i = 0;i<vVotingPairs.size();i++)
		{
			QPointF &		Star = RefStar(vVotingPairs[i]);

			X(i, 0) = Star.x()/fXWidth;
			Y(i, 0) = Star.y()/fYWidth;
		};

		for (i = 0;i<vVotingPairs.size();i++)
		{
			QPointF &		Star = TgtStar(vVotingPairs[i]);

#pragma warning (suppress:4456)
			double		X = Star.x()/fXWidth;
			double		X2 = X * X;
#pragma warning (suppress:4456)
			double		Y = Star.y()/fYWidth;
			double		Y2 = Y * Y;

			M(i, 0) = 1;
			M(i, 1) = X;
			M(i, 2) = Y;
			M(i, 3) = X*Y;
			M(i, 4) = X2;
			M(i, 5) = Y2;
			M(i, 6) = X2 * Y;
			M(i, 7) = X * Y2;
			M(i, 8) = X2 * Y2;
		};

		DMATRIX				MT = ~M;
		DMATRIX				TM = MT * M;

		try
		{
			if (!TM.IsSingular())
			{
				DMATRIX				A = !TM * MT * X;
				DMATRIX				B = !TM * MT * Y;

				BilinearParameters.Type = TType;
				BilinearParameters.a0 = A(0, 0);
				BilinearParameters.a1 = A(1, 0);
				BilinearParameters.a2 = A(2, 0);
				BilinearParameters.a3 = A(3, 0);
				BilinearParameters.a4 = A(4, 0);
				BilinearParameters.a5 = A(5, 0);
				BilinearParameters.a6 = A(6, 0);
				BilinearParameters.a7 = A(7, 0);
				BilinearParameters.a8 = A(8, 0);

				BilinearParameters.b0 = B(0, 0);
				BilinearParameters.b1 = B(1, 0);
				BilinearParameters.b2 = B(2, 0);
				BilinearParameters.b3 = B(3, 0);
				BilinearParameters.b4 = B(4, 0);
				BilinearParameters.b5 = B(5, 0);
				BilinearParameters.b6 = B(6, 0);
				BilinearParameters.b7 = B(7, 0);
				BilinearParameters.b8 = B(8, 0);

				bResult = true;
			};
		}
		catch(math::matrix_error const&)
		{
			bResult = false;
		};
	}
	else
	{
		DMATRIX				M(vVotingPairs.size(), 4);
		DMATRIX				X(vVotingPairs.size(), 1);
		DMATRIX				Y(vVotingPairs.size(), 1);

		for (i = 0;i<vVotingPairs.size();i++)
		{
			QPointF &		Star = RefStar(vVotingPairs[i]);

			X(i, 0) = Star.x()/fXWidth;
			Y(i, 0) = Star.y()/fYWidth;
		};

		for (i = 0;i<vVotingPairs.size();i++)
		{
			QPointF &		Star = TgtStar(vVotingPairs[i]);
#pragma warning (suppress:4456)
			double		X = Star.x()/fXWidth;
#pragma warning (suppress:4456)
			double		Y = Star.y()/fYWidth;

			M(i, 0) = 1;
			M(i, 1) = X;
			M(i, 2) = Y;
			M(i, 3) = X*Y;
		};

		DMATRIX				MT = ~M;
		DMATRIX				TM = MT * M;

		try
		{
			if (!TM.IsSingular())
			{
				DMATRIX				A = !TM * MT * X;
				DMATRIX				B = !TM * MT * Y;

				BilinearParameters.a0 = A(0, 0);
				BilinearParameters.a1 = A(1, 0);
				BilinearParameters.a2 = A(2, 0);
				BilinearParameters.a3 = A(3, 0);
				BilinearParameters.b0 = B(0, 0);
				BilinearParameters.b1 = B(1, 0);
				BilinearParameters.b2 = B(2, 0);
				BilinearParameters.b3 = B(3, 0);

				bResult = true;
			};
		}
		catch(math::matrix_error const&)
		{
			bResult = false;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

double CMatchingStars::ValidateTransformation(const VOTINGPAIRVECTOR & vTestedPairs, const CBilinearParameters & transform)
{
	double fResult = 0.0;

	// Compute the distance between the stars
	for (const auto& testedPair : vTestedPairs)
	{
		const QPointF ptProjected = transform.transform(TgtStar(testedPair));
		const double fDistance = Distance(ptProjected, RefStar(testedPair));

		if (!testedPair.IsCorner())
		{
			if (fDistance > fResult)
				fResult = fDistance;
		}
	}

	return fResult;
};

/* ------------------------------------------------------------------- */

bool CMatchingStars::ComputeCoordinatesTransformation(VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE MaxTType)
{
	bool								bResult = false;
	bool								bEnd = false;
	VOTINGPAIRVECTOR					vPairs;
	TRANSFORMATIONTYPE					TType = TT_BILINEAR;

	size_t								nrPairs;
	size_t								lNrExtraPairs = 0;
	std::vector<int>					vAddedPairs;
	VOTINGPAIRVECTOR					vTestedPairs;
	VOTINGPAIRVECTOR					vOkPairs;
	std::vector<int>					vOkAddedPairs;
	CBilinearParameters					OkTransformation;
	TRANSFORMATIONTYPE					OkTType{ TT_LINEAR };

	if (!vVotingPairs.empty() && vVotingPairs[0].IsCorner())
		lNrExtraPairs = 4;

	vPairs = vVotingPairs;

	while (!bEnd && !bResult)
	{
		if (TType == TT_BICUBIC)
			nrPairs = 32 + lNrExtraPairs;
		else if (TType == TT_BISQUARED)
			nrPairs = 18 + lNrExtraPairs;
		else
			nrPairs = 8 + lNrExtraPairs;

		// Get the top pairs
		vAddedPairs.clear();
		vTestedPairs.clear();
		// First add the locked pairs
		for (size_t i = 0; i < vPairs.size(); i++)
		{
			if (vPairs[i].IsActive() && vPairs[i].IsLocked())
			{
				vTestedPairs.push_back(vPairs[i]);
				vAddedPairs.push_back(static_cast<int>(i));
			}
		}

		// Then add the other pairs up to the limit
		for (size_t i = 0; i < vPairs.size() && vTestedPairs.size() < nrPairs; i++)
		{
			if (vPairs[i].IsActive() && !vPairs[i].IsLocked())
			{
				vTestedPairs.push_back(vPairs[i]);
				vAddedPairs.push_back(static_cast<int>(i));
			}
		}

		if (vTestedPairs.size() == nrPairs)
		{
			// Compute the transformation
			CBilinearParameters				transform;

			if (ComputeTransformation(vTestedPairs, transform, TType))
			{
				std::vector<double> vDistances;
				double fMaxDistance = 0.0;
				size_t maxDistanceIndex = 0;

				// Compute the distance between the stars
				for (size_t i = 0; i < vTestedPairs.size(); i++)
				{
					const QPointF ptProjected = transform.transform(TgtStar(vTestedPairs[i]));
					const double fDistance = Distance(ptProjected, RefStar(vTestedPairs[i]));

					if (!vTestedPairs[i].IsCorner())
					{
						vDistances.push_back(fDistance);
						if (fDistance > fMaxDistance)
						{
							fMaxDistance = fDistance;
							maxDistanceIndex = i;
						}
					}
				}

				// If one star is far from the spot - deactivate the pair
				if (fMaxDistance > 3)
				{
					int						lDeactivatedIndice;
					double						fAverage;
					double						fSigma;
					bool						bOneDeactivated = false;

					fAverage = Average(vDistances);
					fSigma = Sigma(vDistances);

					for (size_t i = 0; i < vDistances.size(); i++)
					{
						if (fabs(vDistances[i] - fAverage) > 2 * fSigma)
						{
							lDeactivatedIndice = vAddedPairs[i];
							if (vPairs[lDeactivatedIndice].IsCorner())
							{
								// Trouble here (corner no good)
							}
							else
							{
								vPairs[lDeactivatedIndice].SetActive(false);
								if (vDistances[i] < 7)
									vPairs[lDeactivatedIndice].SetPossible(true);
								bOneDeactivated = true;
							}
						}
					}

					if (!bOneDeactivated)
					{
						for (size_t i = 0; i < vDistances.size(); i++)
						{
							if (fabs(vDistances[i] - fAverage) > fSigma)
							{
								lDeactivatedIndice = vAddedPairs[i];
								if (vPairs[lDeactivatedIndice].IsCorner())
								{
									// Trouble here (corner no good)
								}
								else
								{
									vPairs[lDeactivatedIndice].SetActive(false);
									bOneDeactivated = true;
								}
							}
						}
					}
					if (!bOneDeactivated)
					{
						lDeactivatedIndice = vAddedPairs[maxDistanceIndex];
						if (vPairs[lDeactivatedIndice].IsCorner())
						{
							// Trouble here (corner no good)
						}
						else
						{
							vPairs[lDeactivatedIndice].SetActive(false);
						}
					}
				}
				else
				{
					OkTransformation = transform;
					vOkPairs = vTestedPairs;
					vOkAddedPairs = vAddedPairs;
					OkTType = TType;
					bResult = (TType == MaxTType);
					if (TType < MaxTType)
					{
						TType = (TRANSFORMATIONTYPE)(1 + (int)TType);
						// All the possible pairs are active again
						for (auto& votingPair : vPairs)
						{
							if (votingPair.IsPossible())
							{
								votingPair.SetActive(true);
								votingPair.SetPossible(false);
							}
						}

						// Lock the pairs
						for (size_t index : vAddedPairs)
							vPairs[index].SetLocked(true);
					}
				}
			}
			else
			{
				// Remove the last pair of the selected pairs
				vPairs[vAddedPairs[nrPairs - 1]].SetActive(false);
			}
		}
		else
			bEnd = true;
	}

	if (vOkPairs.size())
		bResult = true;

	if (bResult)
	{
		// Try to add other pairs to refine the transformation
		bEnd = false;
		CBilinearParameters		transform;
		VOTINGPAIRVECTOR		vTempPairs;
		int					lNrFails = 0;

		BilinearParameters = OkTransformation;

		vTestedPairs = vOkPairs;
		vAddedPairs = vOkAddedPairs;
		TType = OkTType;

		for (size_t index : vAddedPairs)
			vVotingPairs[index].SetUsed(true);

		while (!bEnd)
		{
			double			fMaxDistance;
			bool			bTransformOk = false;
			int				lAddedPair = -1;

			vTempPairs = vTestedPairs;
			for (size_t i = 0; i < vVotingPairs.size() && lAddedPair < 0; i++)
			{
				if (vVotingPairs[i].IsActive() && !vVotingPairs[i].IsUsed())
				{
					lAddedPair = static_cast<int>(i);
					vTempPairs.push_back(vVotingPairs[i]);
					vVotingPairs[lAddedPair].SetUsed(true);
				}
			}

			if (lAddedPair >= 0)
			{
				if (ComputeTransformation(vTempPairs, transform, TType))
				{
					fMaxDistance = ValidateTransformation(vTempPairs, transform);
					if (fMaxDistance <= 2)
					{
						vTestedPairs = vTempPairs;
						BilinearParameters = transform;
						vAddedPairs.push_back(lAddedPair);
						bTransformOk = true;
					}
					else
						vVotingPairs[lAddedPair].SetActive(false);
				};

				if (!bTransformOk)
				{
					lNrFails++;
					if (lNrFails > 3)
						bEnd = true;
				}
			}
			else
				bEnd = true;
		};
	};

	if (bResult)
		vVotingPairs = vTestedPairs;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CMatchingStars::ComputeSigmaClippingTransformation(const VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE TType)
{
	bool								bResult = false;
	VOTINGPAIRVECTOR					vPairs;

	vPairs = vVotingPairs;

	if (AreCornersLocked())
	{
		CBilinearParameters					BaseTransformation;

		// First try to compute a robust bilinear transformation
		bResult = ComputeCoordinatesTransformation(vPairs, BaseTransformation, TT_BILINEAR);
		if (bResult)
		{
			int					i;

			// Use the transformation as pure linear
			// BaseTransformation.a3 = BaseTransformation.b3 = 0.0;
			// Add the four corners as pseudo transformed stars to the list with high voting values

			m_vRefCorners.clear();
			m_vTgtCorners.clear();

			m_vTgtCorners.push_back(QPointF(0, 0));
			m_vTgtCorners.push_back(QPointF(m_lWidth-1, 0));
			m_vTgtCorners.push_back(QPointF(0, m_lHeight-1));
			m_vTgtCorners.push_back(QPointF(m_lWidth-1, m_lHeight-1));

			for (i = 0;i<m_vTgtCorners.size();i++)
			{
				QPointF			ptProjected;

				ptProjected = BaseTransformation.transform(m_vTgtCorners[i]);
				m_vRefCorners.push_back(ptProjected);
			};

			// Then add the pairs to the list
			vPairs = vVotingPairs;
			CVotingPair				vp;

			vp.m_Flags = VPFLAG_ACTIVE | VPFLAG_CORNER_TOPLEFT;
			vp.m_lNrVotes = 10000000;
			vp.m_RefStar = vp.m_TgtStar = 0;
			vPairs.push_back(vp);
			vp.m_Flags = VPFLAG_ACTIVE | VPFLAG_CORNER_TOPRIGHT;
			vp.m_RefStar = vp.m_TgtStar = 1;
			vPairs.push_back(vp);
			vp.m_Flags = VPFLAG_ACTIVE | VPFLAG_CORNER_BOTTOMLEFT;
			vp.m_RefStar = vp.m_TgtStar = 2;
			vPairs.push_back(vp);
			vp.m_Flags = VPFLAG_ACTIVE | VPFLAG_CORNER_BOTTOMRIGHT;
			vp.m_RefStar = vp.m_TgtStar = 3;
			vPairs.push_back(vp);

			// And compute the transformation with the four corners firmly set
			std::sort(vPairs.begin(), vPairs.end());
			bResult = ComputeCoordinatesTransformation(vPairs, BilinearParameters, TType);

			// Remove inactive and corners from the resulting pairs
			VOTINGPAIRVECTOR					vOutPairs;

			for (i = 0;i<vPairs.size(); i++)
			{
				if (vPairs[i].IsActive() && !vPairs[i].IsCorner())
					vOutPairs.push_back(vPairs[i]);
			};

			vPairs = vOutPairs;
		};
	}
	else
		bResult = ComputeCoordinatesTransformation(vPairs, BilinearParameters, TType);

	if (bResult)
		m_vVotedPairs = vPairs;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CMatchingStars::ComputeMedianTransformation(const VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE TType)
{
	bool								bResult = false;
	std::vector<CBilinearParameters>	vBilinears;
	int								i, j, k, l;

	for (i = 0;i<vVotingPairs.size();i++)
	{
		for (j = i+1;j<vVotingPairs.size();j++)
		{
			for (k = j+1;k<vVotingPairs.size();k++)
			{
				for (l = k+1;l<vVotingPairs.size();l++)
				{
					VOTINGPAIRVECTOR					vPairs;
					CBilinearParameters					params;

					vPairs.push_back(vVotingPairs[i]);
					vPairs.push_back(vVotingPairs[j]);
					vPairs.push_back(vVotingPairs[k]);
					vPairs.push_back(vVotingPairs[l]);

					if (ComputeTransformation(vPairs, params, TType))
						vBilinears.push_back(params);
				}
			};
		};
	};

	// Compute the median transformation
	if (vBilinears.size())
	{
		std::vector<double>					vA0, vA1, vA2, vA3;
		std::vector<double>					vB0, vB1, vB2, vB3;
		std::vector<double>					vA4, vA5, vA6, vA7, vA8;
		std::vector<double>					vB4, vB5, vB6, vB7, vB8;
		std::vector<double>					vA9, vA10, vA11, vA12, vA13, vA14, vA15;
		std::vector<double>					vB9, vB10, vB11, vB12, vB13, vB14, vB15;

		for (i = 0;i<vBilinears.size();i++)
		{
			vA0.push_back(vBilinears[i].a0);
			vA1.push_back(vBilinears[i].a1);
			vA2.push_back(vBilinears[i].a2);
			vA3.push_back(vBilinears[i].a3);
			vB0.push_back(vBilinears[i].b0);
			vB1.push_back(vBilinears[i].b1);
			vB2.push_back(vBilinears[i].b2);
			vB3.push_back(vBilinears[i].b3);

			if (TType == TT_BISQUARED || TType == TT_BICUBIC)
			{
				vA4.push_back(vBilinears[i].a4);
				vA5.push_back(vBilinears[i].a5);
				vA6.push_back(vBilinears[i].a6);
				vA7.push_back(vBilinears[i].a7);
				vA8.push_back(vBilinears[i].a8);
				vB4.push_back(vBilinears[i].b4);
				vB5.push_back(vBilinears[i].b5);
				vB6.push_back(vBilinears[i].b6);
				vB7.push_back(vBilinears[i].b7);
				vB8.push_back(vBilinears[i].b8);
			};
			if (TType == TT_BICUBIC)
			{
				vA9.push_back(vBilinears[i].a9);
				vA10.push_back(vBilinears[i].a10);
				vA11.push_back(vBilinears[i].a11);
				vA12.push_back(vBilinears[i].a12);
				vA13.push_back(vBilinears[i].a13);
				vA14.push_back(vBilinears[i].a14);
				vA15.push_back(vBilinears[i].a15);
				vB9.push_back(vBilinears[i].b9);
				vB10.push_back(vBilinears[i].b10);
				vB11.push_back(vBilinears[i].b11);
				vB12.push_back(vBilinears[i].b12);
				vB13.push_back(vBilinears[i].b13);
				vB14.push_back(vBilinears[i].b14);
				vB15.push_back(vBilinears[i].b15);
			};
		};

		BilinearParameters.a0 = Median(vA0);
		BilinearParameters.a1 = Median(vA1);
		BilinearParameters.a2 = Median(vA2);
		BilinearParameters.a3 = Median(vA3);
		BilinearParameters.b0 = Median(vB0);
		BilinearParameters.b1 = Median(vB1);
		BilinearParameters.b2 = Median(vB2);
		BilinearParameters.b3 = Median(vB3);

		if (TType == TT_BISQUARED || TType == TT_BICUBIC)
		{
			BilinearParameters.a4 = Median(vA4);
			BilinearParameters.a5 = Median(vA5);
			BilinearParameters.a6 = Median(vA6);
			BilinearParameters.a7 = Median(vA7);
			BilinearParameters.a8 = Median(vA8);
			BilinearParameters.b4 = Median(vB4);
			BilinearParameters.b5 = Median(vB5);
			BilinearParameters.b6 = Median(vB6);
			BilinearParameters.b7 = Median(vB7);
			BilinearParameters.b8 = Median(vB8);
		};
		if (TType == TT_BICUBIC)
		{
			BilinearParameters.a9 = Median(vA9);
			BilinearParameters.a10 = Median(vA10);
			BilinearParameters.a11 = Median(vA11);
			BilinearParameters.a12 = Median(vA12);
			BilinearParameters.a13 = Median(vA13);
			BilinearParameters.a14 = Median(vA14);
			BilinearParameters.a15 = Median(vA15);
			BilinearParameters.b9 = Median(vB9);
			BilinearParameters.b10 = Median(vB10);
			BilinearParameters.b11 = Median(vB11);
			BilinearParameters.b12 = Median(vB12);
			BilinearParameters.b13 = Median(vB13);
			BilinearParameters.b14 = Median(vB14);
			BilinearParameters.b15 = Median(vB15);
		};

		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

const	float					TRIANGLETOLERANCE = (float)0.002;

bool	CMatchingStars::ComputeMatchingTriangleTransformation(CBilinearParameters & BilinearParameters)
{
	bool				bResult = false;

	// First compute the triangles for reference and target
	if (!m_vRefTriangles.size())
		ComputeTriangles(m_vRefStars, m_vRefTriangles);

	ComputeTriangles(m_vTgtStars, m_vTgtTriangles);

	// Then match the triangle filling the voting grid in the process
	// At this point the triangles vectors are sorted along the X axis
	bool						bEnd = false;
	STARTRIANGLEITERATOR		itRef,
								itLastUsedRef,
								itTgt;
	VOTINGPAIRVECTOR			vVotingPairs,
								vOutputVotingPairs;

	InitVotingGrid(vVotingPairs);

	itLastUsedRef = m_vRefTriangles.begin();

	for (itTgt = m_vTgtTriangles.begin();(itTgt != m_vTgtTriangles.end()) && !bEnd;itTgt++)
	{
		while (itLastUsedRef != m_vRefTriangles.end() &&
			   (*itTgt).m_fX > (*itLastUsedRef).m_fX+TRIANGLETOLERANCE)
			   itLastUsedRef++;

		if (itLastUsedRef == m_vRefTriangles.end())
			bEnd = true;
		else
		{
			// At this point (*itLastUsedRef).m_fX is less than (*itTgt).m_fX
			itRef = itLastUsedRef;
			while ((itRef != m_vRefTriangles.end()) && ((*itRef).m_fX < (*itTgt).m_fX + TRIANGLETOLERANCE))
			{
				// Check real distance between triangles
				float			fDistance;

				fDistance = Distance((*itRef).m_fX, (*itRef).m_fY, (*itTgt).m_fX, (*itTgt).m_fY);
				if (fDistance <= TRIANGLETOLERANCE)
				{
					// Vote for the all the pairs
					AddVote((*itRef).m_Star1, (*itTgt).m_Star1, vVotingPairs, (int)m_vTgtStars.size());
					AddVote((*itRef).m_Star1, (*itTgt).m_Star2, vVotingPairs, (int)m_vTgtStars.size());
					AddVote((*itRef).m_Star1, (*itTgt).m_Star3, vVotingPairs, (int)m_vTgtStars.size());
					AddVote((*itRef).m_Star2, (*itTgt).m_Star1, vVotingPairs, (int)m_vTgtStars.size());
					AddVote((*itRef).m_Star2, (*itTgt).m_Star2, vVotingPairs, (int)m_vTgtStars.size());
					AddVote((*itRef).m_Star2, (*itTgt).m_Star3, vVotingPairs, (int)m_vTgtStars.size());
					AddVote((*itRef).m_Star3, (*itTgt).m_Star1, vVotingPairs, (int)m_vTgtStars.size());
					AddVote((*itRef).m_Star3, (*itTgt).m_Star2, vVotingPairs, (int)m_vTgtStars.size());
					AddVote((*itRef).m_Star3, (*itTgt).m_Star3, vVotingPairs, (int)m_vTgtStars.size());
				};
				itRef++;
			};
		};
	};

/*	InitVotingGrid(vOutputVotingPairs);
	AdjustVoting(vVotingPairs, vOutputVotingPairs, m_vTgtStars.size());
	std::sort(vOutputVotingPairs.begin(), vOutputVotingPairs.end());
	vVotingPairs = vOutputVotingPairs;*/

	std::sort(vVotingPairs.begin(), vVotingPairs.end());

	// At this point voting pairs are ordered descending
	// Then eliminate false matches and get transformations parameters
	if (vVotingPairs.size() >= m_vTgtStars.size())
	{
		int				lMinNrVotes;
		int				lCut = 0;
		TRANSFORMATIONTYPE	TType = TT_BILINEAR;

		lMinNrVotes = vVotingPairs[m_vTgtStars.size()*2-1].m_lNrVotes;
		if (lMinNrVotes == 0)
			lMinNrVotes = 1;
		while (vVotingPairs[lCut].m_lNrVotes >= lMinNrVotes)
			lCut++;
		vVotingPairs.resize(lCut);

		TType = GetTransformationType((int)vVotingPairs.size());

		bResult = ComputeSigmaClippingTransformation(vVotingPairs, BilinearParameters, TType);

		if (bResult && (TType == TT_LINEAR))
		{
			// This is a pure linear function -- Alter coefficients
			BilinearParameters.a3 = 0;
			BilinearParameters.b3 = 0;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

const double			MAXSTARDISTANCEDELTA = 2.0;

CComAutoCriticalSection	g_StarDistSection;
STARDISTVECTOR *		g_pvDists = nullptr;

inline bool CompareStarDistances (int lDist1, int lDist2)
{
	return (*g_pvDists)[lDist1].m_fDistance > (*g_pvDists)[lDist2].m_fDistance;
};

bool	CMatchingStars::ComputeLargeTriangleTransformation(CBilinearParameters & BilinearParameters)
{
	bool					bResult = false;
	int					i = 0,
							j = 0;

	// Compute patterns
	if (!m_vRefStarDistances.size())
	{
		ComputeStarDistances(m_vRefStars, m_vRefStarDistances);
		for (i = 0;i<m_vRefStarDistances.size();i++)
			m_vRefStarIndices.push_back(i);
	};

	ComputeStarDistances(m_vTgtStars, m_vTgtStarDistances);
	for (i = 0;i<m_vTgtStarDistances.size();i++)
		m_vTgtStarIndices.push_back(i);

	g_StarDistSection.Lock();
	g_pvDists = &m_vRefStarDistances;
	std::sort(m_vRefStarIndices.begin(), m_vRefStarIndices.end(), CompareStarDistances);

	g_pvDists = &m_vTgtStarDistances;
	std::sort(m_vTgtStarIndices.begin(), m_vTgtStarIndices.end(), CompareStarDistances);

	g_pvDists = nullptr;
	g_StarDistSection.Unlock();

	VOTINGPAIRVECTOR			vVotingPairs,
								vOutputVotingPairs;

	InitVotingGrid(vVotingPairs);
	i = j = 0;

	while (i<m_vTgtStarDistances.size() && j<m_vRefStarDistances.size())
	{
		if (fabs(m_vTgtStarDistances[m_vTgtStarIndices[i]].m_fDistance-m_vRefStarDistances[m_vRefStarIndices[j]].m_fDistance) <= MAXSTARDISTANCEDELTA)
		{
			// These are within 2 pixels ... find all the others stars
			// using the same stars in Target and check if the distances
			// are the same in the reference.
			// If it is the case, cast a vote for each potential pair
			int				lRefStar1,
								lRefStar2,
								lTgtStar1,
								lTgtStar2;
			double				fTgtDistance12,
								fRefDistance12;

			fRefDistance12 = m_vRefStarDistances[m_vRefStarIndices[j]].m_fDistance;
			fTgtDistance12 = m_vTgtStarDistances[m_vTgtStarIndices[i]].m_fDistance;

			lRefStar1 = m_vRefStarDistances[m_vRefStarIndices[j]].m_Star1;
			lRefStar2 = m_vRefStarDistances[m_vRefStarIndices[j]].m_Star2;

			lTgtStar1 = m_vTgtStarDistances[m_vTgtStarIndices[i]].m_Star1;
			lTgtStar2 = m_vTgtStarDistances[m_vTgtStarIndices[i]].m_Star2;

			for (int lTgtStar3 = 0;lTgtStar3 < m_vTgtStars.size();lTgtStar3++)
			{
				if ((lTgtStar3 != lTgtStar1) && (lTgtStar3 != lTgtStar2))
				{
					STARDISTITERATOR		it;
					double					fTgtDistance13 = 0.0;
					double					fTgtDistance23 = 0.0;
					double					fRatio;

					it = std::lower_bound(m_vTgtStarDistances.begin(), m_vTgtStarDistances.end(), CStarDist(lTgtStar1, lTgtStar3));
					if (it != m_vTgtStarDistances.end())
						fTgtDistance13 = (*it).m_fDistance;
					it = std::lower_bound(m_vTgtStarDistances.begin(), m_vTgtStarDistances.end(), CStarDist(lTgtStar2, lTgtStar3));
					if (it != m_vTgtStarDistances.end())
						fTgtDistance23 = (*it).m_fDistance;

					fRatio = max(fTgtDistance13, fTgtDistance23) / fTgtDistance12;
					// Filter triangle because :
					// Larger triangle are already used
					// 0.9 avoids many useless triangles with two big sides and one small side
					if (fRatio < 0.9)
					{
						// Search a star from reference such as
						// distance from 1 to 3 is near fTgtDistance13
						// distance from 2 to 3 is near fTgtDistance23
						for (int lRefStar3 = 0;lRefStar3 < m_vRefStars.size();lRefStar3++)
						{
							if ((lRefStar3 != lRefStar1) && (lRefStar3 != lRefStar2))
							{
								double		fRefDistance13 = 0.0;
								double		fRefDistance23 = 0.0;

								it = std::lower_bound(m_vRefStarDistances.begin(), m_vRefStarDistances.end(), CStarDist(lRefStar1, lRefStar3));
								if (it != m_vRefStarDistances.end())
									fRefDistance13 = (*it).m_fDistance;
								it = std::lower_bound(m_vRefStarDistances.begin(), m_vRefStarDistances.end(), CStarDist(lRefStar2, lRefStar3));
								if (it != m_vRefStarDistances.end())
									fRefDistance23 = (*it).m_fDistance;

								if ((fabs(fRefDistance13 - fTgtDistance13) < MAXSTARDISTANCEDELTA) &&
									(fabs(fRefDistance23 - fTgtDistance23) < MAXSTARDISTANCEDELTA))
								{
									// Cast votes for stars
									AddVote(lRefStar1, lTgtStar1, vVotingPairs, (int)m_vTgtStars.size());
									AddVote(lRefStar2, lTgtStar2, vVotingPairs, (int)m_vTgtStars.size());
									AddVote(lRefStar3, lTgtStar3, vVotingPairs, (int)m_vTgtStars.size());
								}
								else if ((fabs(fRefDistance23 - fTgtDistance13) < MAXSTARDISTANCEDELTA) &&
										 (fabs(fRefDistance13 - fTgtDistance23) < MAXSTARDISTANCEDELTA))
								{
									// Cast votes for stars
									AddVote(lRefStar1, lTgtStar2, vVotingPairs, (int)m_vTgtStars.size());
									AddVote(lRefStar2, lTgtStar1, vVotingPairs, (int)m_vTgtStars.size());
									AddVote(lRefStar3, lTgtStar3, vVotingPairs, (int)m_vTgtStars.size());
								};
							};
						};
					};
				};
			};
		};

		if (m_vTgtStarDistances[m_vTgtStarIndices[i]].m_fDistance<m_vRefStarDistances[m_vRefStarIndices[j]].m_fDistance)
			j++;
		else
			i++;
	};

	// Resolve votes
	std::sort(vVotingPairs.begin(), vVotingPairs.end());

	// At this point voting pairs are ordered descending
	// Then eliminate false matches and get transformations parameters
	if (vVotingPairs.size() >= m_vTgtStars.size())
	{
		int				lMinNrVotes;
		int				lCut = 0;
		TRANSFORMATIONTYPE	TType = TT_BILINEAR;

		lMinNrVotes = vVotingPairs[m_vTgtStars.size()*2-1].m_lNrVotes;
		if (lMinNrVotes == 0)
			lMinNrVotes = 1;
		while (vVotingPairs[lCut].m_lNrVotes >= lMinNrVotes)
			lCut++;
		vVotingPairs.resize(lCut+1);

		TType = GetTransformationType((int)vVotingPairs.size());

		bResult = ComputeSigmaClippingTransformation(vVotingPairs, BilinearParameters, TType);

		if (bResult && (TType == TT_LINEAR))
		{
			// This is a pure linear function -- Alter coefficients
			BilinearParameters.a3 = 0;
			BilinearParameters.b3 = 0;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CMatchingStars::AdjustSize()
{
	// if all the stars are in the top/left corner divide the sizes by two
	bool				bAllInTopLeft = true;

	for (int i = 0;(i<m_vTgtStars.size()) && bAllInTopLeft;i++)
	{
		if ((m_vTgtStars[i].x() > m_lWidth/2) || (m_vTgtStars[i].y() > m_lHeight/2))
			bAllInTopLeft = false;
	};

	if (bAllInTopLeft)
	{
		m_lWidth *= 2;
		m_lHeight *= 2;
	}
	else
	{
		// Check that stars are not outside the given sizes
		bool			bOutside = false;
		for (int i = 0;i<m_vTgtStars.size() && !bOutside;i++)
		{
			if ((m_vTgtStars[i].x() > m_lWidth) || (m_vTgtStars[i].y() > m_lHeight))
				bOutside = true;
		};
		if (bOutside)
		{
			m_lWidth  /= 2;
			m_lHeight /= 2;
		};
	};
};

/* ------------------------------------------------------------------- */

bool	CMatchingStars::ComputeCoordinateTransformation(CBilinearParameters & BilinearParameters)
{
	bool					bResult = false;

	if (GetTransformationType() != TT_NONE)
	{
		//AdjustSize();
		if (m_vRefStars.size()>=8 && m_vTgtStars.size()>=8)
		{
			bResult = ComputeLargeTriangleTransformation(BilinearParameters);
			if (!bResult)
				bResult = ComputeMatchingTriangleTransformation(BilinearParameters);
		};
	}
	else
	{
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

