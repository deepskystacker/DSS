#pragma once

class CFlatPart
{
public:
	int			m_lStart,
		m_lEnd;
	double			m_fAverage;
	double			m_fAverageVariation;
	double			m_fAbsAverageVariation;

private:

public:
	CFlatPart() noexcept
	{
		m_lStart = -1;
		m_lEnd = -1;
		m_fAverage = 0;
		m_fAverageVariation = 0;
		m_fAbsAverageVariation = 0;
	};
	~CFlatPart() {};
	CFlatPart(const CFlatPart&) = default;

	CFlatPart& operator = (const CFlatPart&) = default;

	int	Length() const noexcept
	{
		return m_lEnd - m_lStart + 1;
	};

	double Score() const noexcept
	{
		if (m_fAbsAverageVariation)
			return (double)Length() / m_fAbsAverageVariation;//m_fAverageVariation;
		else
			return 0;
	};

	bool operator < (const CFlatPart& fp) const noexcept
	{
		return Score() > fp.Score();
		/*
		if (Length() > fp.Length())
			return true;
		else if (Length() < fp.Length())
			return false;
		else
			return m_fAverage < fp.m_fAverage;*/
	};
};