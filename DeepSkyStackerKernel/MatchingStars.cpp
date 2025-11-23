#include "pch.h"
#include "MatchingStars.h"
#include "zexcept.h"

#define _NO_EXCEPTION
#include "matrix.h"
#include "Workspace.h"

constexpr int MINPAIRSTOBISQUARED = 25;
constexpr int MINPAIRSTOBICUBIC	= 40;
constexpr double TriangleMaxRatio = 0.9;


namespace {
	TRANSFORMATIONTYPE GetTransformationType(const size_t lNrVotingPairs)
	{
		std::uint32_t dwAlignmentTransformation = 2;
		TRANSFORMATIONTYPE		TTResult = TT_BILINEAR;

		dwAlignmentTransformation = Workspace{}.value("Stacking/AlignmentTransformation", (uint)2).toUInt();

		if (dwAlignmentTransformation > TT_LAST)
			dwAlignmentTransformation = 0;

		if (dwAlignmentTransformation == 0)
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

		}

		return TTResult;
	}

	TRANSFORMATIONTYPE GetTransformationType()
	{
		return GetTransformationType(99999); // 99999 -> just take a VERY large number to force finding the "highest" transformation or NONE.
	}

	bool AreCornersLocked()
	{
		return Workspace{}.value("Stacking/LockCorners", true).toBool();
	}
}


STARDISTVECTOR CMatchingStars::ComputeStarDistances(const QPointFVector& vStars)
{
	double fMaxDistance = 0;
	STARDISTVECTOR vStarDist;

	vStarDist.reserve((vStars.size() * (vStars.size() - 1)) / 2);

	for (const size_t i : std::views::iota(size_t{ 0 }, vStars.size()))
	{
		for (const size_t j : std::views::iota(i + 1, vStars.size()))
		{
			const double fDistance = Distance(vStars[i].x(), vStars[i].y(), vStars[j].x(), vStars[j].y());
			fMaxDistance = std::max(fDistance, fMaxDistance);

			vStarDist.emplace_back(i, j, fDistance);
		}
	}

	static_assert(CStarDist{ 1, 2 } < CStarDist{ 2, 2 } && CStarDist{ 1, 1 } < CStarDist{ 1, 2 }
		&& !(CStarDist{ 2, 6 } < CStarDist{ 1, 6 }) && !(CStarDist{ 2, 6 } < CStarDist{ 2, 5 }) && !(CStarDist{ 6, 6 } < CStarDist{ 6, 6 })
	);

	std::ranges::sort(vStarDist, std::less{});
	return vStarDist;
}


void CMatchingStars::ComputeTriangles(const QPointFVector& vStars, STARTRIANGLEVECTOR& vTriangles)
{
	ZFUNCTRACE_RUNTIME();

	const STARDISTVECTOR vStarDist = ComputeStarDistances(vStars);

	for (const size_t i : std::views::iota(size_t{ 0 }, vStars.size()))
	{
		for (const size_t j : std::views::iota(i + 1, vStars.size()))
		{
			std::array<float, 3> vDistances;
			auto it = std::lower_bound(vStarDist.cbegin(), vStarDist.cend(), CStarDist(i, j));
			vDistances[0] = it->m_fDistance;

			for (const size_t k : std::views::iota(j + 1, vStars.size()))
			{
				it = std::lower_bound(vStarDist.cbegin(), vStarDist.cend(), CStarDist(j, k));
				vDistances[1] = it->m_fDistance;
				it = std::lower_bound(vStarDist.cbegin(), vStarDist.cend(), CStarDist(i, k));
				vDistances[2] = it->m_fDistance;

				std::ranges::sort(vDistances);

				if (vDistances[2] > 0)
				{
					const float fX = vDistances[1] / vDistances[2];
					const float fY = vDistances[0] / vDistances[2];

					// Filter
					if (fX < TriangleMaxRatio)
					{
						// Add to the triangle list
						vTriangles.emplace_back(i, j, k, fX, fY);
					}
				}
			}
		}
	}

	std::ranges::sort(vTriangles, std::less{});
}


void CMatchingStars::InitVotingGrid(VOTINGPAIRVECTOR& vVotingPairs)
{
	vVotingPairs.clear();
	vVotingPairs.reserve(m_vRefStars.size() * m_vTgtStars.size());

	for (const int i : std::views::iota(0, static_cast<int>(m_vRefStars.size())))
	{
		for (const int j : std::views::iota(0, static_cast<int>(m_vTgtStars.size())))
		{
			vVotingPairs.emplace_back(i, j);
		}
	}
}


void AddVote(const std::uint8_t RefStar, const std::uint8_t TgtStar, VOTINGPAIRVECTOR& vVotingPairs, const size_t lNrTgtStars)
{
	const size_t offset = RefStar * lNrTgtStars + TgtStar;
	vVotingPairs[offset].m_lNrVotes++;
}

void AddAllVotes(const STARTRIANGLEVECTOR::const_iterator itRef, const STARTRIANGLEVECTOR::const_iterator itTgt, VOTINGPAIRVECTOR& vVotingPairs, const size_t nrTargetStars)
{
	AddVote(itRef->m_Star1, itTgt->m_Star1, vVotingPairs, nrTargetStars);
	AddVote(itRef->m_Star1, itTgt->m_Star2, vVotingPairs, nrTargetStars);
	AddVote(itRef->m_Star1, itTgt->m_Star3, vVotingPairs, nrTargetStars);
	AddVote(itRef->m_Star2, itTgt->m_Star1, vVotingPairs, nrTargetStars);
	AddVote(itRef->m_Star2, itTgt->m_Star2, vVotingPairs, nrTargetStars);
	AddVote(itRef->m_Star2, itTgt->m_Star3, vVotingPairs, nrTargetStars);
	AddVote(itRef->m_Star3, itTgt->m_Star1, vVotingPairs, nrTargetStars);
	AddVote(itRef->m_Star3, itTgt->m_Star2, vVotingPairs, nrTargetStars);
	AddVote(itRef->m_Star3, itTgt->m_Star3, vVotingPairs, nrTargetStars);
}


//void CMatchingStars::AdjustVoting(const VOTINGPAIRVECTOR & vInVotingPairs, VOTINGPAIRVECTOR & vOutVotingPairs, int lNrTgtStars)
//{
//	int			i, j;
//
//	for (i = 0;i<vInVotingPairs.size();i++)
//	{
//		int		lMaxVotes1 = 0,
//					lMaxVotes2 = 0;
//
//		// compute max votes for the same reference star
//		for (j = (vInVotingPairs[i].m_RefStar)*lNrTgtStars;j<(vInVotingPairs[i].m_RefStar+1)*lNrTgtStars;j++)
//		{
//			ZASSERT(vInVotingPairs[j].m_RefStar == vInVotingPairs[i].m_RefStar);
//			if (vInVotingPairs[j].m_TgtStar != vInVotingPairs[i].m_TgtStar)
//				lMaxVotes1 = max(lMaxVotes1, vInVotingPairs[j].m_lNrVotes);
//		}
//
//		// compute max votes for the same target star
//		for (j = vInVotingPairs[i].m_TgtStar;j<vInVotingPairs.size();j+=lNrTgtStars)
//		{
//			ZASSERT(vInVotingPairs[j].m_TgtStar == vInVotingPairs[i].m_TgtStar);
//			if (vInVotingPairs[j].m_RefStar != vInVotingPairs[i].m_RefStar)
//				lMaxVotes2 = max(lMaxVotes2, vInVotingPairs[j].m_lNrVotes);
//		}
//
//		vOutVotingPairs[i].m_lNrVotes = vInVotingPairs[i].m_lNrVotes - max(lMaxVotes1, lMaxVotes2);
//		if (vOutVotingPairs[i].m_lNrVotes<0)
//			vOutVotingPairs[i].m_lNrVotes = 0;
//	}
//}


using DMATRIX = math::matrix<double>;

bool CMatchingStars::ComputeTransformation(std::span<const VotingPair> vVotingPairs, CBilinearParameters& BilinearParameters, const TRANSFORMATIONTYPE TType)
{
	bool bResult = false;
	const double fXWidth = m_lWidth;
	const double fYWidth = m_lHeight;

	BilinearParameters.fXWidth = fXWidth;
	BilinearParameters.fYWidth = fYWidth;

	constexpr size_t Zero = size_t{ 0 };

	if (TType == TT_BICUBIC)
	{
		DMATRIX M(vVotingPairs.size(), 16);
		DMATRIX X(vVotingPairs.size(), 1);
		DMATRIX Y(vVotingPairs.size(), 1);

		for (const size_t i : std::views::iota(Zero, vVotingPairs.size()))
		{
			const QPointF& Star = RefStar(vVotingPairs[i]);
			X(i, 0) = Star.x() / fXWidth;
			Y(i, 0) = Star.y() / fYWidth;
		}

		for (const size_t i : std::views::iota(Zero, vVotingPairs.size()))
		{
			const QPointF& Star = TgtStar(vVotingPairs[i]);
#pragma warning (suppress:4456)
			const double X = Star.x() / fXWidth;
			const double X2 = X * X;
			const double X3 = X * X * X;
#pragma warning (suppress:4456)
			const double Y = Star.y() / fYWidth;
			const double Y2 = Y * Y;
			const double Y3 = Y * Y * Y;

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
		}

		const DMATRIX MT = ~M;
		DMATRIX TM = MT * M;

		try
		{
			if (!TM.IsSingular())
			{
				const DMATRIX A = !TM * MT * X;
				const DMATRIX B = !TM * MT * Y;

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
			}
		}
		catch(math::matrix_error const&)
		{
			bResult = false;
		}
	}
	else if (TType == TT_BISQUARED)
	{
		DMATRIX M(vVotingPairs.size(), 9);
		DMATRIX X(vVotingPairs.size(), 1);
		DMATRIX Y(vVotingPairs.size(), 1);

		for (const size_t i : std::views::iota(Zero, vVotingPairs.size()))
		{
			const QPointF& Star = RefStar(vVotingPairs[i]);
			X(i, 0) = Star.x() / fXWidth;
			Y(i, 0) = Star.y() / fYWidth;
		}

		for (const size_t i : std::views::iota(Zero, vVotingPairs.size()))
		{
			const QPointF& Star = TgtStar(vVotingPairs[i]);

#pragma warning (suppress:4456)
			const double X = Star.x() / fXWidth;
			const double X2 = X * X;
#pragma warning (suppress:4456)
			const double Y = Star.y() / fYWidth;
			const double Y2 = Y * Y;

			M(i, 0) = 1;
			M(i, 1) = X;
			M(i, 2) = Y;
			M(i, 3) = X*Y;
			M(i, 4) = X2;
			M(i, 5) = Y2;
			M(i, 6) = X2 * Y;
			M(i, 7) = X * Y2;
			M(i, 8) = X2 * Y2;
		}

		const DMATRIX MT = ~M;
		DMATRIX TM = MT * M;

		try
		{
			if (!TM.IsSingular())
			{
				const DMATRIX A = !TM * MT * X;
				const DMATRIX B = !TM * MT * Y;

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
			}
		}
		catch(math::matrix_error const&)
		{
			bResult = false;
		}
	}
	else
	{
		DMATRIX M(vVotingPairs.size(), 4);
		DMATRIX X(vVotingPairs.size(), 1);
		DMATRIX Y(vVotingPairs.size(), 1);

		for (const size_t i : std::views::iota(Zero, vVotingPairs.size()))
		{
			const QPointF& Star = RefStar(vVotingPairs[i]);
			X(i, 0) = Star.x() / fXWidth;
			Y(i, 0) = Star.y() / fYWidth;
		}

		for (const size_t i : std::views::iota(Zero, vVotingPairs.size()))
		{
			const QPointF& Star = TgtStar(vVotingPairs[i]);
#pragma warning (suppress:4456)
			const double X = Star.x() / fXWidth;
#pragma warning (suppress:4456) 
			const double Y = Star.y() / fYWidth;

			M(i, 0) = 1;
			M(i, 1) = X;
			M(i, 2) = Y;
			M(i, 3) = X * Y;
		}

		const DMATRIX MT = ~M;
		DMATRIX TM = MT * M;

		try
		{
			if (!TM.IsSingular())
			{
				const DMATRIX A = !TM * MT * X;
				const DMATRIX B = !TM * MT * Y;

				BilinearParameters.a0 = A(0, 0);
				BilinearParameters.a1 = A(1, 0);
				BilinearParameters.a2 = A(2, 0);
				BilinearParameters.a3 = A(3, 0);
				BilinearParameters.b0 = B(0, 0);
				BilinearParameters.b1 = B(1, 0);
				BilinearParameters.b2 = B(2, 0);
				BilinearParameters.b3 = B(3, 0);

				bResult = true;
			}
		}
		catch(math::matrix_error const&)
		{
			bResult = false;
		}
	}

	return bResult;
}

template <typename... DistanceVector>
std::pair<double, size_t> CMatchingStars::ComputeDistanceBetweenStars(const VOTINGPAIRVECTOR& vTestedPairs, const CBilinearParameters& projection, DistanceVector&... distances)
{
	double maxDistance = 0.0;
	size_t maxDistanceIndex = 0;
	auto vdistance = std::tie(distances...);

	// Compute the distance between the stars
	for (size_t i = 0; const auto& testedPair : vTestedPairs)
	{
		if (!testedPair.IsCorner())
		{
			const double distance = Distance(
				projection.transform(TgtStar(testedPair)), 
				RefStar(testedPair)
			);

			if constexpr (sizeof...(DistanceVector) == 1)
			{
				std::get<0>(vdistance).push_back(distance);
			}

			if (distance > maxDistance)
			{
				maxDistance = distance;
				maxDistanceIndex = i;
			}
		}
		++i;
	}

	return { maxDistance, maxDistanceIndex };
}


bool CMatchingStars::ComputeCoordinatesTransformation(VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, const TRANSFORMATIONTYPE MaxTType)
{
	bool bResult = false;
	bool bEnd = false;
	TRANSFORMATIONTYPE TType = TT_BILINEAR;
	TRANSFORMATIONTYPE OkTType = TT_LINEAR;
	
	CBilinearParameters OkTransformation;

	std::vector<int> vAddedPairs;
	std::vector<int> vOkAddedPairs;
	VOTINGPAIRVECTOR vPairs = vVotingPairs;
	VOTINGPAIRVECTOR vTestedPairs;
	VOTINGPAIRVECTOR vOkPairs;

	const size_t nrExtraPairs = !vVotingPairs.empty() && vVotingPairs[0].IsCorner() ? 4 : 0;

	while (!bEnd && !bResult)
	{
		const size_t nrPairs = nrExtraPairs + (TType == TT_BICUBIC ? 32 : (TType == TT_BISQUARED ? 18 : 8));

		// Get the top pairs
		vAddedPairs.clear();
		vTestedPairs.clear();
		// First add the locked pairs
		for (int i = 0; const auto& pair : vPairs)
		{
			if (pair.IsActive() && pair.IsLocked())
			{
				vTestedPairs.push_back(pair);
				vAddedPairs.push_back(i);
			}
			++i;
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
			CBilinearParameters projection;

			if (ComputeTransformation(vTestedPairs, projection, TType))
			{
				std::vector<double> vDistances;
				const auto [fMaxDistance, maxDistanceIndex] = ComputeDistanceBetweenStars(vTestedPairs, projection, vDistances);
/*
				// Compute the distance between the stars
				for (size_t i = 0; i < vTestedPairs.size(); i++)
				{
					const QPointF ptProjected = projection.transform(TgtStar(vTestedPairs[i]));
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
*/
				// If one star is far from the spot - deactivate the pair
				if (fMaxDistance > 3)
				{
					int lDeactivatedIndice = 0;
					bool bOneDeactivated = false;

					const double fAverage = Average(vDistances);
					const double fSigma = Sigma(vDistances);

					for (size_t i = 0; i < vDistances.size(); i++)
					{
						if (std::abs(vDistances[i] - fAverage) > 2 * fSigma)
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
				else // fMaxDistance <= 3 here.
				{
					OkTransformation = projection;
					vOkPairs = vTestedPairs;
					vOkAddedPairs = vAddedPairs;
					OkTType = TType;
					bResult = (TType == MaxTType);

					if (TType < MaxTType)
					{
						TType = getNextHigherTransformationType(TType);

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
						for (const size_t index : vAddedPairs)
							vPairs[index].SetLocked(true);
					}
				}
			}
			else // ComputeTransformation(vTestedPairs, projection, TType) returned false here.
			{
				// Remove the last pair of the selected pairs
				vPairs[vAddedPairs[nrPairs - 1]].SetActive(false);
			}
		}
		else
			bEnd = true;
	}

	if (!vOkPairs.empty())
		bResult = true;

	if (bResult)
	{
		// Try to add other pairs to refine the transformation
		bEnd = false;
		int lNrFails = 0;

		BilinearParameters = OkTransformation;

		vTestedPairs = vOkPairs;
		vAddedPairs = vOkAddedPairs;
		TType = OkTType;

		for (const auto index : vAddedPairs)
			vVotingPairs[index].SetUsed(true);

		while (!bEnd)
		{
			bool bTransformOk = false;
			int lAddedPair = -1;
			VOTINGPAIRVECTOR vTempPairs = vTestedPairs;

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
				CBilinearParameters projection;
				if (ComputeTransformation(vTempPairs, projection, TType))
				{
					const double maxDistance = ComputeDistanceBetweenStars(vTempPairs, projection).first;
					if (maxDistance <= 2)
					{
						vTestedPairs = vTempPairs;
						BilinearParameters = projection;
						vAddedPairs.push_back(lAddedPair);
						bTransformOk = true;
					}
					else
						vVotingPairs[lAddedPair].SetActive(false);
				}

				if (!bTransformOk)
				{
					lNrFails++;
					if (lNrFails > 3)
						bEnd = true;
				}
			}
			else
				bEnd = true;
		}
	}

	if (bResult)
		vVotingPairs = vTestedPairs;

	return bResult;
}


bool CMatchingStars::ComputeSigmaClippingTransformation(const VOTINGPAIRVECTOR& vVotingPairs, CBilinearParameters& BilinearParameters, const TRANSFORMATIONTYPE TType)
{
	bool bResult = false;
	VOTINGPAIRVECTOR vPairs = vVotingPairs;

	if (AreCornersLocked())
	{
		CBilinearParameters BaseTransformation;

		// First try to compute a robust bilinear transformation
		bResult = ComputeCoordinatesTransformation(vPairs, BaseTransformation, TT_BILINEAR);
		if (bResult)
		{
			// Use the transformation as pure linear
			// BaseTransformation.a3 = BaseTransformation.b3 = 0.0;
			// Add the four corners as pseudo transformed stars to the list with high voting values

			m_vRefCorners.clear();
			m_vTgtCorners.clear();

			m_vTgtCorners.push_back(QPointF(0, 0));
			m_vTgtCorners.push_back(QPointF(m_lWidth - 1, 0));
			m_vTgtCorners.push_back(QPointF(0, m_lHeight - 1));
			m_vTgtCorners.push_back(QPointF(m_lWidth - 1, m_lHeight - 1));

			for (const QPointF& targetCorner : m_vTgtCorners)
			{
				m_vRefCorners.push_back(BaseTransformation.transform(targetCorner));
			}

			// Then add the pairs to the list
			vPairs = vVotingPairs;
			VotingPair vp;

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
			std::ranges::sort(vPairs, std::greater{});
			bResult = ComputeCoordinatesTransformation(vPairs, BilinearParameters, TType);

			// Remove inactive and corners from the resulting pairs
			VOTINGPAIRVECTOR vOutPairs;
			for (const auto& pair : vPairs)
			{
				if (pair.IsActive() && !pair.IsCorner())
					vOutPairs.push_back(pair);
			}

			vPairs = vOutPairs;
		}
	}
	else
		bResult = ComputeCoordinatesTransformation(vPairs, BilinearParameters, TType);

	if (bResult)
		m_vVotedPairs = vPairs;

	return bResult;
}

/*
bool CMatchingStars::ComputeMedianTransformation(const VOTINGPAIRVECTOR & vVotingPairs, CBilinearParameters & BilinearParameters, TRANSFORMATIONTYPE TType)
{
	bool bResult = false;
	std::vector<CBilinearParameters> vBilinears;

	for (size_t i = 0; i < vVotingPairs.size(); i++)
	{
		for (size_t j = i + 1; j < vVotingPairs.size(); j++)
		{
			for (size_t k = j + 1; k < vVotingPairs.size(); k++)
			{
				for (size_t l = k + 1; l < vVotingPairs.size(); l++)
				{
					const std::array<const VotingPair, 4> vPairs = { vVotingPairs[i], vVotingPairs[j], vVotingPairs[k], vVotingPairs[l] };
					CBilinearParameters projection;
					if (ComputeTransformation(vPairs, projection, TType))
						vBilinears.push_back(std::move(projection));
				}
			}
		}
	}

	// Compute the median transformation
	if (!vBilinears.empty())
	{
		std::vector<double> vA0, vA1, vA2, vA3;
		std::vector<double> vB0, vB1, vB2, vB3;
		std::vector<double> vA4, vA5, vA6, vA7, vA8;
		std::vector<double> vB4, vB5, vB6, vB7, vB8;
		std::vector<double> vA9, vA10, vA11, vA12, vA13, vA14, vA15;
		std::vector<double> vB9, vB10, vB11, vB12, vB13, vB14, vB15;

		for (const auto& projection : vBilinears)
		{
			vA0.push_back(projection.a0);
			vA1.push_back(projection.a1);
			vA2.push_back(projection.a2);
			vA3.push_back(projection.a3);
			vB0.push_back(projection.b0);
			vB1.push_back(projection.b1);
			vB2.push_back(projection.b2);
			vB3.push_back(projection.b3);

			if (TType == TT_BISQUARED || TType == TT_BICUBIC)
			{
				vA4.push_back(projection.a4);
				vA5.push_back(projection.a5);
				vA6.push_back(projection.a6);
				vA7.push_back(projection.a7);
				vA8.push_back(projection.a8);
				vB4.push_back(projection.b4);
				vB5.push_back(projection.b5);
				vB6.push_back(projection.b6);
				vB7.push_back(projection.b7);
				vB8.push_back(projection.b8);
			}

			if (TType == TT_BICUBIC)
			{
				vA9.push_back(projection.a9);
				vA10.push_back(projection.a10);
				vA11.push_back(projection.a11);
				vA12.push_back(projection.a12);
				vA13.push_back(projection.a13);
				vA14.push_back(projection.a14);
				vA15.push_back(projection.a15);
				vB9.push_back(projection.b9);
				vB10.push_back(projection.b10);
				vB11.push_back(projection.b11);
				vB12.push_back(projection.b12);
				vB13.push_back(projection.b13);
				vB14.push_back(projection.b14);
				vB15.push_back(projection.b15);
			}
		}

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
		}

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
		}

		bResult = true;
	}

	return bResult;
}
*/

bool CMatchingStars::ComputeMatchingTriangleTransformation(CBilinearParameters & BilinearParameters)
{
	constexpr float TRIANGLETOLERANCE = 0.002f;
	bool bResult = false;

	// First compute the triangles for reference and target
	if (m_vRefTriangles.empty())
		ComputeTriangles(m_vRefStars, m_vRefTriangles);

	ComputeTriangles(m_vTgtStars, m_vTgtTriangles);

	// Then match the triangle filling the voting grid in the process
	// At this point the triangles vectors are sorted along the X axis
	bool bEnd = false;
	VOTINGPAIRVECTOR vVotingPairs;

	InitVotingGrid(vVotingPairs);

	auto itLastUsedRef = m_vRefTriangles.cbegin();

	for (auto itTgt = m_vTgtTriangles.cbegin(); itTgt != m_vTgtTriangles.cend() && !bEnd; ++itTgt)
	{
		while (itLastUsedRef != m_vRefTriangles.cend() && (itTgt->m_fX > itLastUsedRef->m_fX + TRIANGLETOLERANCE))
		{
			++itLastUsedRef;
		}

		if (itLastUsedRef == m_vRefTriangles.cend())
			bEnd = true;
		else
		{
			// At this point (*itLastUsedRef).m_fX is less than (*itTgt).m_fX
			auto itRef = itLastUsedRef;
			while (itRef != m_vRefTriangles.cend() && (itRef->m_fX < itTgt->m_fX + TRIANGLETOLERANCE))
			{
				// Check real distance between triangles
				const float fDistance = Distance(itRef->m_fX, itRef->m_fY, itTgt->m_fX, itTgt->m_fY);
				if (fDistance <= TRIANGLETOLERANCE)
				{
					// Vote for the all the pairs
					AddAllVotes(itRef, itTgt, vVotingPairs, m_vTgtStars.size());
				}
				++itRef;
			}
		}
	}

/*	InitVotingGrid(vOutputVotingPairs);
	AdjustVoting(vVotingPairs, vOutputVotingPairs, m_vTgtStars.size());
	std::ranges::sort(vOutputVotingPairs.begin(), vOutputVotingPairs.end());
	vVotingPairs = vOutputVotingPairs;
*/

	std::ranges::sort(vVotingPairs, std::greater{});

	// At this point voting pairs are ordered descending
	// Then eliminate false matches and get transformations parameters
	if (vVotingPairs.size() >= m_vTgtStars.size())
	{
		int lMinNrVotes = vVotingPairs[m_vTgtStars.size() * 2 - 1].m_lNrVotes;
		if (lMinNrVotes == 0)
			lMinNrVotes = 1;

		int lCut = 0;
		while (vVotingPairs[lCut].m_lNrVotes >= lMinNrVotes)
			lCut++;
		vVotingPairs.resize(lCut);

		const TRANSFORMATIONTYPE TType = GetTransformationType(vVotingPairs.size());

		bResult = ComputeSigmaClippingTransformation(vVotingPairs, BilinearParameters, TType);

		if (bResult && (TType == TT_LINEAR))
		{
			// This is a pure linear function -- Alter coefficients
			BilinearParameters.a3 = 0;
			BilinearParameters.b3 = 0;
		}
	}

	return bResult;
}


bool CMatchingStars::ComputeLargeTriangleTransformation(CBilinearParameters& BilinearParameters)
{
	constexpr double MAXSTARDISTANCEDELTA = 2.0;

	constexpr auto createIota = [](const size_t size) -> std::vector<int>
	{
		const auto iota = std::views::iota(0, static_cast<int>(size));
		return std::vector<int>(iota.begin(), iota.end());
	};

	// Compute patterns
	if (m_vRefStarDistances.empty())
	{
		m_vRefStarDistances = ComputeStarDistances(m_vRefStars);
		m_vRefStarIndices = createIota(m_vRefStarDistances.size());
	}

	const STARDISTVECTOR targetStarDistances = ComputeStarDistances(m_vTgtStars);
	std::vector<int> targetStarIndices = createIota(targetStarDistances.size());

//	for (const size_t i : std::views::iota(size_t{ 0 }, targetStarDistances.size()))
//		m_vTgtStarIndices.push_back(static_cast<int>(i));

	std::ranges::sort(m_vRefStarIndices, std::greater{}, [this](const int starIndex) { return m_vRefStarDistances[starIndex].m_fDistance; });
	std::ranges::sort(targetStarIndices, std::greater{}, [&targetStarDistances](const int starIndex) { return targetStarDistances[starIndex].m_fDistance; });

	VOTINGPAIRVECTOR vVotingPairs;
	InitVotingGrid(vVotingPairs);

	const auto TargetStar = [&targetStarDistances, &targetStarIndices](const size_t ndx) { return targetStarDistances[targetStarIndices[ndx]]; };
	const auto ReferenceStar = [&dist = m_vRefStarDistances, &ind = m_vRefStarIndices](const size_t ndx) { return dist[ind[ndx]]; };

	const auto getRefStarDistance = [this, b = m_vRefStarDistances.data(), e = m_vRefStarDistances.data() + m_vRefStarDistances.size()](
		const int star1, const int star2) -> float
	{
#if !defined(NDEBUG) // Accelerate the search in debug mode by using raw pointers and avoiding iterators.
		const auto it = std::lower_bound(b, e, CStarDist(star1, star2));
		return it == e ? 0.0 : it->m_fDistance;
#else
		const auto it = std::lower_bound(m_vRefStarDistances.cbegin(), m_vRefStarDistances.cend(), CStarDist(star1, star2));
		return it == m_vRefStarDistances.cend() ? 0.0 : it->m_fDistance;
#endif
	};


	for (size_t i = 0, j = 0; i < targetStarDistances.size() && j < m_vRefStarDistances.size();)
	{
		if (std::fabs(TargetStar(i).m_fDistance - ReferenceStar(j).m_fDistance) <= static_cast<decltype(CStarDist::m_fDistance)>(MAXSTARDISTANCEDELTA))
		{
			// These are within 2 pixels ... find all the others stars
			// using the same stars in Target and check if the distances
			// are the same in the reference.
			// If it is the case, cast a vote for each potential pair

// not used		const double fRefDistance12 = ReferenceStar(j).m_fDistance;
			const double fTgtDistance12 = TargetStar(i).m_fDistance;

			const int lRefStar1 = ReferenceStar(j).m_Star1;
			const int lRefStar2 = ReferenceStar(j).m_Star2;

			const int lTgtStar1 = TargetStar(i).m_Star1;
			const int lTgtStar2 = TargetStar(i).m_Star2;

			for (int lTgtStar3 = 0; lTgtStar3 < m_vTgtStars.size(); lTgtStar3++)
			{
				if ((lTgtStar3 != lTgtStar1) && (lTgtStar3 != lTgtStar2))
				{
					auto it = std::lower_bound(targetStarDistances.cbegin(), targetStarDistances.cend(), CStarDist(lTgtStar1, lTgtStar3));
					const double fTgtDistance13 = it == targetStarDistances.cend() ? 0.0 : it->m_fDistance;

					it = std::lower_bound(targetStarDistances.cbegin(), targetStarDistances.cend(), CStarDist(lTgtStar2, lTgtStar3));
					const double fTgtDistance23 = it == targetStarDistances.cend() ? 0.0 : it->m_fDistance;

					const double fRatio = std::max(fTgtDistance13, fTgtDistance23) / fTgtDistance12;
					// Filter triangle because :
					// Larger triangle are already used
					// TriangleMaxRatio (usually 0.9) avoids many useless triangles with two big sides and one small side
					if (fRatio < TriangleMaxRatio)
					{
						// Search a star from reference such as
						// distance from 1 to 3 is near fTgtDistance13
						// distance from 2 to 3 is near fTgtDistance23
						for (int lRefStar3 = 0; lRefStar3 < m_vRefStars.size(); lRefStar3++)
						{
							if ((lRefStar3 != lRefStar1) && (lRefStar3 != lRefStar2))
							{
//								it = std::lower_bound(m_vRefStarDistances.cbegin(), m_vRefStarDistances.cend(), CStarDist(lRefStar1, lRefStar3));
//								const double fRefDistance13 = it == m_vRefStarDistances.cend() ? 0.0 : it->m_fDistance;
//								it = std::lower_bound(m_vRefStarDistances.cbegin(), m_vRefStarDistances.cend(), CStarDist(lRefStar2, lRefStar3));
//								const double fRefDistance23 = it == m_vRefStarDistances.cend() ? 0.0 : it->m_fDistance;

								const double fRefDistance13 = getRefStarDistance(lRefStar1, lRefStar3);
								const double fRefDistance23 = getRefStarDistance(lRefStar2, lRefStar3);

								if (std::abs(fRefDistance13 - fTgtDistance13) < MAXSTARDISTANCEDELTA && std::fabs(fRefDistance23 - fTgtDistance23) < MAXSTARDISTANCEDELTA)
								{
									// Cast votes for stars
									AddVote(lRefStar1, lTgtStar1, vVotingPairs, m_vTgtStars.size());
									AddVote(lRefStar2, lTgtStar2, vVotingPairs, m_vTgtStars.size());
									AddVote(lRefStar3, lTgtStar3, vVotingPairs, m_vTgtStars.size());
								}
								else if (std::abs(fRefDistance23 - fTgtDistance13) < MAXSTARDISTANCEDELTA && std::fabs(fRefDistance13 - fTgtDistance23) < MAXSTARDISTANCEDELTA)
								{
									// Cast votes for stars
									AddVote(lRefStar1, lTgtStar2, vVotingPairs, m_vTgtStars.size());
									AddVote(lRefStar2, lTgtStar1, vVotingPairs, m_vTgtStars.size());
									AddVote(lRefStar3, lTgtStar3, vVotingPairs, m_vTgtStars.size());
								}
							}
						}
					}
				}
			}
		}

		if (TargetStar(i).m_fDistance < ReferenceStar(j).m_fDistance)
			++j;
		else
			++i;
	}

	// Resolve votes
	std::ranges::sort(vVotingPairs, std::greater{});

	// At this point voting pairs are ordered descending
	// Then eliminate false matches and get transformations parameters
	bool bResult = false;
	if (vVotingPairs.size() >= m_vTgtStars.size())
	{
		int lMinNrVotes = vVotingPairs[m_vTgtStars.size() * 2 - 1].m_lNrVotes;
		if (lMinNrVotes == 0)
			lMinNrVotes = 1;

		size_t lCut = 0;
		while (vVotingPairs[lCut].m_lNrVotes >= lMinNrVotes)
			lCut++;
		vVotingPairs.resize(lCut + 1);

		const TRANSFORMATIONTYPE TType = GetTransformationType(vVotingPairs.size());

		bResult = ComputeSigmaClippingTransformation(vVotingPairs, BilinearParameters, TType);

		if (bResult && (TType == TT_LINEAR))
		{
			// This is a pure linear function -- Alter coefficients
			BilinearParameters.a3 = 0;
			BilinearParameters.b3 = 0;
		}
	}

	return bResult;
}


void CMatchingStars::AdjustSize()
{
	// if all the stars are in the top/left corner divide the sizes by two
	bool				bAllInTopLeft = true;

	for (int i = 0;(i<m_vTgtStars.size()) && bAllInTopLeft;i++)
	{
		if ((m_vTgtStars[i].x() > m_lWidth/2) || (m_vTgtStars[i].y() > m_lHeight/2))
			bAllInTopLeft = false;
	}

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
		}
		if (bOutside)
		{
			m_lWidth  /= 2;
			m_lHeight /= 2;
		}
	}
}


bool CMatchingStars::ComputeCoordinateTransformation(CBilinearParameters& BilinearParameters)
{
	if (GetTransformationType() == TT_NONE)
		return true;

	//AdjustSize();
	if (m_vRefStars.size() >= 8 && m_vTgtStars.size() >= 8)
	{
		bool bResult = ComputeLargeTriangleTransformation(BilinearParameters);
		if (!bResult)
			bResult = ComputeMatchingTriangleTransformation(BilinearParameters);
		return bResult;
	}
	return false;
}
