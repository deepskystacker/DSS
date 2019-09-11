#ifndef __ENTROPYINFO_H__
#define __ENTROPYINFO_H__

#include "DSSTools.h"
#include "BitmapExt.h"
#include "zexcept.h"

/* ------------------------------------------------------------------- */

class CEntropySquare
{
public :
	CPointExt					m_ptCenter;
	double						m_fRedEntropy;
	double						m_fGreenEntropy;
	double						m_fBlueEntropy;

private :
	void	CopyFrom(const CEntropySquare & es)
	{
		m_ptCenter		= es.m_ptCenter;
		m_fRedEntropy	= es.m_fRedEntropy;
		m_fGreenEntropy = es.m_fGreenEntropy;
		m_fBlueEntropy  = es.m_fBlueEntropy;
	};

public :
	CEntropySquare()
	{
	};

	CEntropySquare(const CPointExt & pt, double fRedEntropy, double fGreenEntropy, double fBlueEntropy)
	{
		m_ptCenter = pt;
		m_fRedEntropy	= fRedEntropy;
		m_fGreenEntropy = fGreenEntropy;
		m_fBlueEntropy	= fBlueEntropy;
	};

	CEntropySquare(const CEntropySquare & es)
	{
		CopyFrom(es);
	};

	virtual ~CEntropySquare()
	{
	};

	const CEntropySquare & operator = (const CEntropySquare & es)
	{
		CopyFrom(es);
		return (*this);
	};
};

class CEntropyInfo
{
private :
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	LONG						m_lWindowSize;
	LONG						m_lNrPixels;
	LONG						m_lNrSquaresX,
								m_lNrSquaresY;
	std::vector<float>			m_vRedEntropies;
	std::vector<float>			m_vGreenEntropies;
	std::vector<float>			m_vBlueEntropies;
	CDSSProgress *				m_pProgress;

private :
	void	InitSquareEntropies();
	void	ComputeEntropies(LONG lMinX, LONG lMinY, LONG lMaxX, LONG lMaxY, double & fRedEntropy, double & fGreenEntropy, double & fBlueEntropy);
	void	GetSquareCenter(LONG lX, LONG lY, CPointExt & ptCenter)
	{
		ptCenter.X = lX * (m_lWindowSize * 2 + 1) + m_lWindowSize;
		ptCenter.Y = lY * (m_lWindowSize * 2 + 1) + m_lWindowSize;
	};

	void	AddSquare(CEntropySquare &Square, LONG lX, LONG lY)
	{
		GetSquareCenter(lX, lY, Square.m_ptCenter);
		Square.m_fRedEntropy	= m_vRedEntropies[lX + lY * m_lNrSquaresX];
		Square.m_fGreenEntropy	= m_vGreenEntropies[lX + lY * m_lNrSquaresX];
		Square.m_fBlueEntropy	= m_vBlueEntropies[lX + lY * m_lNrSquaresX];
	};

public :
	CEntropyInfo()
	{
		m_pProgress = NULL;
	};

	virtual ~CEntropyInfo()
	{
	};

	void	Init(CMemoryBitmap * pBitmap, LONG lWindowSize = 10, CDSSProgress * pProgress = NULL)
	{
		m_pBitmap.Attach(pBitmap);
		m_lWindowSize = lWindowSize;
		m_pProgress   = pProgress;
		InitSquareEntropies();
	};

	void	GetPixel(LONG x, LONG y, double & fRedEntropy, double & fGreenEntropy, double & fBlueEntropy, COLORREF16 & crResult)
	{
		LONG			lSquareX,
						lSquareY;

		m_pBitmap->GetPixel16(x, y, crResult);

		lSquareX = x / (m_lWindowSize * 2 + 1);
		lSquareY = y / (m_lWindowSize * 2 + 1);

		CPointExt			ptCenter;
		CEntropySquare		Squares[3];
		size_t				sizeSquares = 0;

		GetSquareCenter(lSquareX, lSquareY, ptCenter);
		AddSquare(Squares[sizeSquares++], lSquareX, lSquareY);
		if (ptCenter.X > x)
		{
			if (lSquareX > 0)
				AddSquare(Squares[sizeSquares++], lSquareX-1, lSquareY);
		}
		else if (ptCenter.X < x)
		{
			if (lSquareX < m_lNrSquaresX - 1)
				AddSquare(Squares[sizeSquares++], lSquareX+1, lSquareY);
		};

		if (ptCenter.Y > y)
		{
			if (lSquareY > 0)
				AddSquare(Squares[sizeSquares++], lSquareX, lSquareY-1);
		}
		else if (ptCenter.Y < y)
		{
			if (lSquareY < m_lNrSquaresY - 1)
				AddSquare(Squares[sizeSquares++], lSquareX, lSquareY+1);
		};

		// Compute the gradient entropy from the nearby squares
		fRedEntropy		= 0.0;
		fGreenEntropy	= 0.0;
		fBlueEntropy	= 0.0;
		CPointExt			ptPixel(x, y);
		double				fTotalWeight = 0.0;

		for (size_t i = 0; i < sizeSquares; i++)
		{
			double		fDistance;
			double		fWeight = 1.0;

			fDistance = Distance(ptPixel, Squares[i].m_ptCenter);
			if (fDistance > 0)
				fWeight = 1.0/fDistance;

			fRedEntropy		+= fWeight * Squares[i].m_fRedEntropy;
			fGreenEntropy	+= fWeight * Squares[i].m_fGreenEntropy;
			fBlueEntropy	+= fWeight * Squares[i].m_fBlueEntropy;

			fTotalWeight += fWeight;
		};

		fRedEntropy		/= fTotalWeight;
		fGreenEntropy	/= fTotalWeight;
		fBlueEntropy	/= fTotalWeight;
	};
};

#endif // __ENTROPYINFO_H__