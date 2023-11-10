#pragma once

class DSSRect final
{

public:
	int    left;
	int    top;
	int    right;
	int    bottom;

public:
	DSSRect(const int l  = 0, const int t = 0, const int r = 0, const int b = 0) noexcept :
		left{ l },
		top{ t },
		right{ r },
		bottom{ b }
	{} 

	DSSRect(const DSSRect& rc) noexcept
	{
		left = rc.left;
		right = rc.right;
		top = rc.top;
		bottom = rc.bottom;
	};

	const DSSRect& operator = (const DSSRect& rc) noexcept
	{
		left = rc.left;
		right = rc.right;
		top = rc.top;
		bottom = rc.bottom;

		return (*this);
	};

	int	width() const
	{
		return right - left;
	};

	int	height() const
	{
		return bottom - top;
	};

	~DSSRect()
	{
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
