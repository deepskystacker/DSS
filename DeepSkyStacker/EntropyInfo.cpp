#include <stdafx.h>
#include "EntropyInfo.h"
#include "DSSProgress.h"
#include "avx_entropy.h"
/* ------------------------------------------------------------------- */

void CEntropyInfo::InitSquareEntropies()
{
	ZFUNCTRACE_RUNTIME();
	int		lSquareSize;

	lSquareSize = m_lWindowSize * 2 + 1;

	m_lNrSquaresX = m_pBitmap->Width() / lSquareSize;
	m_lNrSquaresY = m_pBitmap->Height() / lSquareSize;
	if (m_pBitmap->RealWidth() % lSquareSize)
		m_lNrSquaresX++;
	if (m_pBitmap->RealHeight() % lSquareSize)
		m_lNrSquaresY++;

	m_vRedEntropies.resize(m_lNrSquaresX*m_lNrSquaresY);
	m_vGreenEntropies.resize(m_lNrSquaresX*m_lNrSquaresY);
	m_vBlueEntropies.resize(m_lNrSquaresX*m_lNrSquaresY);

	if (m_pProgress)
		m_pProgress->Start2(m_lNrSquaresX);

	AvxEntropy avxEntropy(*m_pBitmap, *this, nullptr);
	if (avxEntropy.calcEntropies(lSquareSize, m_lNrSquaresX, m_lNrSquaresY, m_vRedEntropies, m_vGreenEntropies, m_vBlueEntropies) != 0)
	{
		for (int i = 0; i < m_lNrSquaresX; i++)
		{
			int			lMinX,
				lMaxX;

			lMinX = i * lSquareSize;
			lMaxX = std::min((i + 1) * lSquareSize - 1, m_pBitmap->Width() - 1);

			for (int j = 0; j < m_lNrSquaresY; j++)
			{
				int		lMinY,
					lMaxY;
				double		fRedEntropy,
					fGreenEntropy,
					fBlueEntropy;

				lMinY = j * lSquareSize;
				lMaxY = std::min((j + 1) * lSquareSize - 1, m_pBitmap->Height() - 1);
				// Compute the entropy for this square
				ComputeEntropies(lMinX, lMinY, lMaxX, lMaxY, fRedEntropy, fGreenEntropy, fBlueEntropy);

				m_vRedEntropies[i + j * m_lNrSquaresX] = fRedEntropy;
				m_vGreenEntropies[i + j * m_lNrSquaresX] = fGreenEntropy;
				m_vBlueEntropies[i + j * m_lNrSquaresX] = fBlueEntropy;
			};

			if (m_pProgress)
				if (0 == i % m_lWindowSize)
					m_pProgress->Progress2(1 + i);
		};
	}

	if (m_pProgress)
		m_pProgress->End2();
};

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
};
