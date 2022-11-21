#pragma once

class DSSRect final
{
private:
	int left{ 0 };
	int top{ 0 };
	int right{ 0 };
	int bottom{ 0 };

public:
	constexpr DSSRect() noexcept = default;

	explicit constexpr DSSRect(const int xleft, const int ytop, const int xright, const int ybottom) noexcept :
		left{ xleft },
		top{ ytop },
		right{ xright },
		bottom{ ybottom }
	{} 

	DSSRect(const DSSRect& rc) noexcept = default;
	DSSRect& operator=(const DSSRect& rc) noexcept = default;

	constexpr int getLeft() const noexcept { return left; }
	constexpr int getTop() const noexcept { return top; }
	constexpr int getRight() const noexcept { return right; }
	constexpr int getBottom() const noexcept { return bottom; }

	constexpr int width() const noexcept
	{
		return right - left;
	}
	constexpr int height() const noexcept
	{
		return bottom - top;
	}

	void setEmpty() noexcept
	{
		*this = DSSRect{};
	}

	constexpr bool isEmpty() const noexcept
	{
		return (left == right) || (top == bottom);
	}

	void offsetRect(const int dx, const int dy) noexcept
	{
		left += dx;
		right += dx;
		top += dy;
		bottom += dy;
	}

	void setCoords(const int xleft, const int ytop, const int xright, const int ybottom) noexcept
	{
		*this = DSSRect{ xleft, ytop, xright, ybottom };
	}
};
