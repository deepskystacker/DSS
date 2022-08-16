#ifndef __ENTROPYINFO_H__
#define __ENTROPYINFO_H__

#include <memory>
#include "DSSTools.h"
#include "BitmapBase.h"
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
        m_fRedEntropy = 0;
        m_fGreenEntropy = 0;
        m_fBlueEntropy = 0;
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
private:
	std::shared_ptr<const CMemoryBitmap> m_pBitmap;
	int m_lWindowSize;
	int m_lNrPixels;
	int m_lNrSquaresX;
	int m_lNrSquaresY;
	std::vector<float> m_vRedEntropies;
	std::vector<float> m_vGreenEntropies;
	std::vector<float> m_vBlueEntropies;
	CDSSProgress* m_pProgress;

private:
	void InitSquareEntropies();
	void ComputeEntropies(int lMinX, int lMinY, int lMaxX, int lMaxY, double & fRedEntropy, double & fGreenEntropy, double & fBlueEntropy);
	void GetSquareCenter(int lX, int lY, CPointExt & ptCenter)
	{
		ptCenter.X = lX * (m_lWindowSize * 2 + 1) + m_lWindowSize;
		ptCenter.Y = lY * (m_lWindowSize * 2 + 1) + m_lWindowSize;
	}

	void AddSquare(CEntropySquare& Square, int lX, int lY)
	{
		GetSquareCenter(lX, lY, Square.m_ptCenter);
		Square.m_fRedEntropy	= m_vRedEntropies[lX + lY * m_lNrSquaresX];
		Square.m_fGreenEntropy	= m_vGreenEntropies[lX + lY * m_lNrSquaresX];
		Square.m_fBlueEntropy	= m_vBlueEntropies[lX + lY * m_lNrSquaresX];
	}

public:
    CEntropyInfo() :
		m_pProgress{ nullptr },
		m_lWindowSize{ 0 },
		m_lNrPixels{ 0 },
		m_lNrSquaresX{ 0 },
		m_lNrSquaresY{ 0 }
	{}

	virtual ~CEntropyInfo()
	{}

	const float* redEntropyData() const { return m_vRedEntropies.data(); }
	const float* greenEntropyData() const { return m_vGreenEntropies.data(); }
	const float* blueEntropyData() const { return m_vBlueEntropies.data(); }
	const int nrSquaresX() const { return m_lNrSquaresX; }
	const int nrSquaresY() const { return m_lNrSquaresY; }
	const int windowSize() const { return m_lWindowSize; }

	void Init(std::shared_ptr<const CMemoryBitmap> pBitmap, int lWindowSize = 10, CDSSProgress* pProgress = nullptr)
	{
		m_pBitmap = pBitmap;
		m_lWindowSize = lWindowSize;
		m_pProgress   = pProgress;
		InitSquareEntropies();
	}

	void GetPixel(int x, int y, double& fRedEntropy, double& fGreenEntropy, double& fBlueEntropy, COLORREF16& crResult)
	{
		int lSquareX, lSquareY;

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
		}

		fRedEntropy		/= fTotalWeight;
		fGreenEntropy	/= fTotalWeight;
		fBlueEntropy	/= fTotalWeight;
	}
};

#endif
