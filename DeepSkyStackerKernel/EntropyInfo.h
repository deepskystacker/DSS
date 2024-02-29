#pragma once
#include "ColorRef.h"

namespace DSS { class ProgressBase; }

/* ------------------------------------------------------------------- */

class CEntropySquare
{
public :
	QPointF						m_ptCenter;
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

	CEntropySquare(const QPointF & pt, double fRedEntropy, double fGreenEntropy, double fBlueEntropy)
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

class CMemoryBitmap;
class CEntropyInfo
{
protected:
	std::shared_ptr<CMemoryBitmap> m_pBitmap{};
	int m_lWindowSize{ 0 };
	int m_lNrPixels{ 0 };
	int m_lNrSquaresX{ 0 };
	int m_lNrSquaresY{ 0 };
	std::vector<float> m_vRedEntropies{};
	std::vector<float> m_vGreenEntropies{};
	std::vector<float> m_vBlueEntropies{};
	DSS::ProgressBase* m_pProgress{ nullptr };

private:
	virtual void InitSquareEntropies();
	void ComputeEntropies(int lMinX, int lMinY, int lMaxX, int lMaxY, double& fRedEntropy, double& fGreenEntropy, double& fBlueEntropy);
	QPointF GetSquareCenter(int lX, int lY) const
	{
		return QPointF{
			static_cast<qreal>(lX * (m_lWindowSize * 2 + 1) + m_lWindowSize),
			static_cast<qreal>(lY * (m_lWindowSize * 2 + 1) + m_lWindowSize)
		};
	}

	void AddSquare(CEntropySquare& Square, int lX, int lY) const
	{
		Square.m_ptCenter = GetSquareCenter(lX, lY);
		Square.m_fRedEntropy	= m_vRedEntropies[lX + lY * m_lNrSquaresX];
		Square.m_fGreenEntropy	= m_vGreenEntropies[lX + lY * m_lNrSquaresX];
		Square.m_fBlueEntropy	= m_vBlueEntropies[lX + lY * m_lNrSquaresX];
	}

public:
	CEntropyInfo() = default;
	CEntropyInfo(const CEntropyInfo&) = delete;
	CEntropyInfo(CEntropyInfo&&) = delete;
	CEntropyInfo& operator=(const CEntropyInfo&) = delete;
	CEntropyInfo& operator=(CEntropyInfo&&) = delete;
	~CEntropyInfo() = default;

	const float* redEntropyData() const { return m_vRedEntropies.data(); }
	const float* greenEntropyData() const { return m_vGreenEntropies.data(); }
	const float* blueEntropyData() const { return m_vBlueEntropies.data(); }
	const int nrSquaresX() const { return m_lNrSquaresX; }
	const int nrSquaresY() const { return m_lNrSquaresY; }
	const int windowSize() const { return m_lWindowSize; }

	void Init(std::shared_ptr<CMemoryBitmap> pBitmap, int lWindowSize = 10, DSS::ProgressBase* pProgress = nullptr);
	void GetPixel(int x, int y, double& fRedEntropy, double& fGreenEntropy, double& fBlueEntropy, COLORREF16& crResult) const;
};

