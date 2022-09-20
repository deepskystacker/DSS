#pragma once

class CSkyBackground
{
public:
	double				m_fLight;
	double				m_fRed,
		m_fGreen,
		m_fBlue;

private:
	void	CopyFrom(const CSkyBackground& right)
	{
		m_fLight = right.m_fLight;
		m_fRed = right.m_fRed;
		m_fGreen = right.m_fGreen;
		m_fBlue = right.m_fBlue;
	};

public:
	CSkyBackground()
	{
		m_fLight = 0;
		m_fRed = m_fGreen = m_fBlue = 0;
	};
	~CSkyBackground()
	{
	};

	CSkyBackground(const CSkyBackground& right)
	{
		CopyFrom(right);
	};

	CSkyBackground& operator = (const CSkyBackground& right)
	{
		CopyFrom(right);
		return (*this);
	};

	bool operator < (const CSkyBackground& right) const
	{
		return m_fLight < right.m_fLight;
	};

	void	Reset()
	{
		m_fLight = 0;
		m_fRed = m_fGreen = m_fBlue = 0;
	};
};
