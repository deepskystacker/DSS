#pragma once

class	CDynamicStats
{
public:
	int			m_lNrValues;
	double			m_fSum;
	double			m_fPowSum;
	double			m_fMin,
		m_fMax;

public:
	CDynamicStats()
	{
		m_lNrValues = 0;
		m_fSum = 0;
		m_fPowSum = 0;
		m_fMin = 0;
		m_fMax = 0;
	};
	virtual ~CDynamicStats() {};

	void AddValue(double fValue, int lNrValues = 1) noexcept
	{
		if (!m_lNrValues)
		{
			m_fMin = m_fMax = fValue;
		}
		else
		{
			m_fMin = std::min(m_fMin, fValue);
			m_fMax = std::max(m_fMax, fValue);
		};
		m_lNrValues += lNrValues;
		m_fPowSum += (fValue * fValue) * lNrValues;
		m_fSum += fValue * lNrValues;
	};

	void RemoveValue(double fValue, int lNrValues = 1) noexcept
	{
		m_lNrValues -= lNrValues;
		m_fPowSum -= (fValue * fValue) * lNrValues;
		m_fSum -= fValue * lNrValues;
	};

	double	Average() noexcept
	{
		if (m_lNrValues)
			return m_fSum / (double)m_lNrValues;
		else
			return 0;
	};

	double Sigma() noexcept
	{
		if (m_lNrValues)
			return sqrt(m_fPowSum / m_lNrValues - pow(m_fSum / m_lNrValues, 2));
		else
			return 0;
	};

	double Min() noexcept
	{
		return m_fMin;
	};

	double Max() noexcept
	{
		return m_fMax;
	};
};

template <class T> inline
void	FillDynamicStat(const std::vector<T>& vValues, CDynamicStats& DynStats)
{
	for (int i = 0; i < vValues.size(); i++)
		DynStats.AddValue(vValues[i]);
};