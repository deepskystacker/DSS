#pragma once

#pragma pack(push, HDCOLORREFT, 1)

template <typename TType>
class COLORREFT
{
public:
	TType		red;
	TType		green;
	TType		blue;

private:
	void	CopyFrom(const COLORREFT<TType>& cr)
	{
		red = cr.red;
		green = cr.green;
		blue = cr.blue;
	};

public:
	COLORREFT(TType r = 0, TType g = 0, TType b = 0)
	{
		red = r;
		green = g;
		blue = b;
	};

	COLORREFT(const COLORREFT<TType>& cr)
	{
		CopyFrom(cr);
	};

	COLORREFT<TType>& operator = (const COLORREFT<TType>& cr)
	{
		CopyFrom(cr);
		return *this;
	};

};

typedef COLORREFT<std::uint8_t> COLORREF8;
typedef COLORREFT<std::uint16_t> COLORREF16;
typedef COLORREFT<std::uint32_t> COLORREF32;
typedef COLORREFT<float> COLORREF32F;

#pragma pack(pop, HDCOLORREFT)

