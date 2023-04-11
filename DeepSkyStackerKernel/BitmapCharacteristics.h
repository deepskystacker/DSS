#pragma once

class CBitmapCharacteristics
{
public:
	int m_lNrChannels;
	int m_lBitsPerPixel;
	bool m_bFloat;
	std::uint32_t m_dwWidth, m_dwHeight;

private:
	void	CopyFrom(const CBitmapCharacteristics& bc)
	{
		m_lNrChannels = bc.m_lNrChannels;
		m_lBitsPerPixel = bc.m_lBitsPerPixel;
		m_bFloat = bc.m_bFloat;
		m_dwWidth = bc.m_dwWidth;
		m_dwHeight = bc.m_dwHeight;
	};

public:
	CBitmapCharacteristics() :
		m_lNrChannels{ 0 },
		m_lBitsPerPixel{ 0 },
		m_bFloat{ false },
		m_dwWidth{ 0 },
		m_dwHeight{ 0 }
	{}
	~CBitmapCharacteristics() {};

	CBitmapCharacteristics(const CBitmapCharacteristics& bc)
	{
		CopyFrom(bc);
	};

	CBitmapCharacteristics& operator= (const CBitmapCharacteristics& bc)
	{
		CopyFrom(bc);
		return *this;
	};
};