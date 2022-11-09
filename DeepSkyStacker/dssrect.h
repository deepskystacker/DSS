#pragma once

class DSSRect final
{

public:
	int    left;
	int    top;
	int    right;
	int    bottom;

public:
	DSSRect(int l = 0, int t = 0, int r = 0, int b = 0)
	{
		left = std::min(l, r);
		top = std::min(t, b);
		right = std::max(l, r);
		bottom = std::max(t, b);
	};

	DSSRect(const DSSRect& rc)
	{
		left = rc.left;
		right = rc.right;
		top = rc.top;
		bottom = rc.bottom;
	};

	const DSSRect& operator = (const DSSRect& rc)
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
		return (pt.x() >= left) && (pt.x() < right) &&
			(pt.y() >= top) && (pt.y() < bottom);
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
