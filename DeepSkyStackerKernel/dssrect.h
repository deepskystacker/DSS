#pragma once

class DSSRect final
{

public:
	int    left;
	int    top;
	int    right;
	int    bottom;

public:
	constexpr DSSRect() :
		left{ 0 },
		top{ 0 },
		right{ 0 },
		bottom{ 0 }
	{}
	explicit constexpr DSSRect(const int l, const int t, const int r, const int b) noexcept :
		left{ l },
		top{ t },
		right{ r },
		bottom{ b }
	{} 

	DSSRect(const DSSRect&) noexcept = default;
	DSSRect& operator=(const DSSRect&) noexcept = default;
	~DSSRect() = default;

	int	width() const
	{
		return right - left;
	};

	int	height() const
	{
		return bottom - top;
	};

	bool	contains(const QPoint& pt) const
	{
		return (pt.x() >= left) && (pt.x() < right) &&
			(pt.y() >= top) && (pt.y() < bottom);
	};

	bool	contains(const QPointF& pt) const
	{
		return (pt.x() >= left) && (pt.x() <= static_cast<qreal>(right)-1) &&
			(pt.y() >= top) && (pt.y() <= static_cast<qreal>(bottom)-1);
	};

	void	setEmpty()
	{
		left = right = top = bottom = 0;
	};

	bool	isEmpty() const
	{
		return (left == right) || (top == bottom);
	};

	void	offsetRect(int dx, int dy)
	{
		left += dx;
		right += dx;
		top += dy;
		bottom += dy;
	};

	void	setCoords(int x1, int y1, int x2, int y2)
	{
		left = x1;		right = x2;
		top = y1;		bottom = y2;
	};
};
