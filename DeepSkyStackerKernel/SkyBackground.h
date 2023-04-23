#pragma once

class CSkyBackground
{
public:
	double m_fLight{ 0.0 };
	double m_fRed{ 0.0 };
	double m_fGreen{ 0.0 };
	double m_fBlue{ 0.0 };

	CSkyBackground() noexcept = default;
	CSkyBackground(const CSkyBackground&) noexcept = default;
	CSkyBackground& operator=(const CSkyBackground&) noexcept = default;

	bool operator<(const CSkyBackground& right) const noexcept
	{
		return m_fLight < right.m_fLight;
	}

	void Reset() noexcept
	{
		*this = CSkyBackground{};
	}
};