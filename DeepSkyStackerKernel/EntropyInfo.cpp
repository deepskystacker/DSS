#include <stdafx.h>
#include "EntropyInfo.h"
#include "DSSProgress.h"
#include "avx_entropy.h"
#include "Ztrace.h"
#include "MemoryBitmap.h"
#include "DSSTools.h"

using namespace DSS;

void CEntropyInfo::InitSquareEntropies() // virtual
{
	ZFUNCTRACE_RUNTIME();

	int lSquareSize = m_lWindowSize * 2 + 1;

	m_lNrSquaresX = m_pBitmap->Width() / lSquareSize;
	m_lNrSquaresY = m_pBitmap->Height() / lSquareSize;
	if (m_pBitmap->RealWidth() % lSquareSize)
		m_lNrSquaresX++;
	if (m_pBitmap->RealHeight() % lSquareSize)
		m_lNrSquaresY++;

	m_vRedEntropies.resize(m_lNrSquaresX*m_lNrSquaresY);
	m_vGreenEntropies.resize(m_lNrSquaresX*m_lNrSquaresY);
	m_vBlueEntropies.resize(m_lNrSquaresX*m_lNrSquaresY);

	if (m_pProgress != nullptr)
		m_pProgress->Start2(m_lNrSquaresX);

	AvxEntropy avxEntropy(*m_pBitmap, *this, nullptr);
	if (avxEntropy.calcEntropies(lSquareSize, m_lNrSquaresX, m_lNrSquaresY, m_vRedEntropies, m_vGreenEntropies, m_vBlueEntropies) != 0)
	{
		for (int i = 0; i < m_lNrSquaresX; i++)
		{
			const int lMinX = i * lSquareSize;
			const int lMaxX = std::min((i + 1) * lSquareSize - 1, m_pBitmap->Width() - 1);

			for (int j = 0; j < m_lNrSquaresY; j++)
			{
				const int lMinY = j * lSquareSize;
				const int lMaxY = std::min((j + 1) * lSquareSize - 1, m_pBitmap->Height() - 1);

				// Compute the entropy for this square
				double fRedEntropy;
				double fGreenEntropy;
				double fBlueEntropy;
				ComputeEntropies(lMinX, lMinY, lMaxX, lMaxY, fRedEntropy, fGreenEntropy, fBlueEntropy);

				m_vRedEntropies[i + j * m_lNrSquaresX] = fRedEntropy;
				m_vGreenEntropies[i + j * m_lNrSquaresX] = fGreenEntropy;
				m_vBlueEntropies[i + j * m_lNrSquaresX] = fBlueEntropy;
			}

			if (m_pProgress != nullptr && i % m_lWindowSize == 0)
				m_pProgress->Progress2(1 + i);
		}
	}

	if (m_pProgress != nullptr)
		m_pProgress->End2();
}

/* ------------------------------------------------------------------- */

void CEntropyInfo::ComputeEntropies(int lMinX, int lMinY, int lMaxX, int lMaxY, double & fRedEntropy, double & fGreenEntropy, double & fBlueEntropy)
{
	std::vector<std::uint16_t> vRedHisto;
	std::vector<std::uint16_t> vGreenHisto;
	std::vector<std::uint16_t> vBlueHisto;

	fRedEntropy = 0.0;
	fGreenEntropy = 0.0;
	fBlueEntropy = 0.0;

	vRedHisto.resize((int)MAXWORD+1);
	vGreenHisto.resize((int)MAXWORD+1);
	vBlueHisto.resize((int)MAXWORD+1);

	COLORREF16		crColor;
	for (int i = lMinX;i<=lMaxX;i++)
	{
		for (int j = lMinY;j<=lMaxY;j++)
		{
			m_pBitmap->GetPixel16(i, j, crColor);
			vRedHisto[crColor.red]++;
			vGreenHisto[crColor.green]++;
			vBlueHisto[crColor.blue]++;
		};
	};

	const double lNrPixels = static_cast<double>(lMaxX - lMinX + 1) * static_cast<double>(lMaxY - lMinY + 1);

	for (int i = lMinX;i<=lMaxX;i++)
	{
		for (int j = lMinY;j<=lMaxY;j++)
		{
			m_pBitmap->GetPixel16(i, j, crColor);

			const double qRed = static_cast<double>(vRedHisto[crColor.red]) / lNrPixels;
			const double qGreen = static_cast<double>(vGreenHisto[crColor.green]) / lNrPixels;
			const double qBlue = static_cast<double>(vBlueHisto[crColor.blue]) / lNrPixels;

			fRedEntropy += -qRed * log(qRed)/log(2.0);
			fGreenEntropy += -qGreen * log(qGreen)/log(2.0);
			fBlueEntropy += -qBlue * log(qBlue)/log(2.0);
		};
	};
}

void CEntropyInfo::Init(std::shared_ptr<CMemoryBitmap> pBitmap, int lWindowSize /* = 10 */, DSS::ProgressBase* pProgress /* = nullptr */)
{
	m_pBitmap = pBitmap;
	m_lWindowSize = lWindowSize;
	m_pProgress = pProgress;
	InitSquareEntropies();
}

void CEntropyInfo::GetPixel(int x, int y, double& fRedEntropy, double& fGreenEntropy, double& fBlueEntropy, COLORREF16& crResult)
{
	int lSquareX, lSquareY;

	m_pBitmap->GetPixel16(x, y, crResult);

	lSquareX = x / (m_lWindowSize * 2 + 1);
	lSquareY = y / (m_lWindowSize * 2 + 1);

	CEntropySquare Squares[3];
	size_t sizeSquares = 0;

	QPointF ptCenter = GetSquareCenter(lSquareX, lSquareY);
	AddSquare(Squares[sizeSquares++], lSquareX, lSquareY);
	if (ptCenter.x() > x)
	{
		if (lSquareX > 0)
			AddSquare(Squares[sizeSquares++], lSquareX - 1, lSquareY);
	}
	else if (ptCenter.x() < x)
	{
		if (lSquareX < m_lNrSquaresX - 1)
			AddSquare(Squares[sizeSquares++], lSquareX + 1, lSquareY);
	};

	if (ptCenter.y() > y)
	{
		if (lSquareY > 0)
			AddSquare(Squares[sizeSquares++], lSquareX, lSquareY - 1);
	}
	else if (ptCenter.y() < y)
	{
		if (lSquareY < m_lNrSquaresY - 1)
			AddSquare(Squares[sizeSquares++], lSquareX, lSquareY + 1);
	};

	// Compute the gradient entropy from the nearby squares
	fRedEntropy = 0.0;
	fGreenEntropy = 0.0;
	fBlueEntropy = 0.0;
	QPointF			ptPixel(x, y);
	double				fTotalWeight = 0.0;

	for (size_t i = 0; i < sizeSquares; i++)
	{
		double		fDistance;
		double		fWeight = 1.0;

		fDistance = Distance(ptPixel, Squares[i].m_ptCenter);
		if (fDistance > 0)
			fWeight = 1.0 / fDistance;

		fRedEntropy += fWeight * Squares[i].m_fRedEntropy;
		fGreenEntropy += fWeight * Squares[i].m_fGreenEntropy;
		fBlueEntropy += fWeight * Squares[i].m_fBlueEntropy;

		fTotalWeight += fWeight;
	}

	fRedEntropy /= fTotalWeight;
	fGreenEntropy /= fTotalWeight;
	fBlueEntropy /= fTotalWeight;
}