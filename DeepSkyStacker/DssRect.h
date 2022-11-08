#pragma once

class PixelBlock final
{
private:
	int leftEdge{ 0 }, topEdge{ 0 }, rightEdge{ 0 }, bottomEdge{ 0 };
public:
	PixelBlock() = default;
	explicit PixelBlock(const int l, const int t, const int r, const int b) :
		leftEdge{ l }, topEdge{ t }, rightEdge{ r }, bottomEdge{ b }
	{}
	template <class T>
	explicit PixelBlock(T&& topLeftPoint, T&& bottomRightPoint) :
		leftEdge{ topLeftPoint.x() }, topEdge{ topLeftPoint.y() }, rightEdge{ bottomRightPoint.x() }, bottomEdge{ bottomRightPoint.y() }
	{}

	PixelBlock(const PixelBlock&) = default;

	PixelBlock& operator=(const PixelBlock&) = default;

	int width() const noexcept
	{
		return rightEdge - leftEdge;
	}
	int height() const noexcept
	{
		return bottomEdge - topEdge;
	}

	int left() const noexcept { return leftEdge; }
	int top() const noexcept { return topEdge; }
	int right() const noexcept { return rightEdge; }
	int bottom() const noexcept { return bottomEdge; }

	void setEmpty() noexcept
	{
		leftEdge = 0;
		topEdge = 0;
		rightEdge = 0;
		bottomEdge = 0;
	}

	void setCoords(const int l, const int t, const int r, const int b) noexcept
	{
		this->operator=(PixelBlock{ l, t, r, b });
	}

	bool isEmpty() const noexcept
	{
		return leftEdge == rightEdge || topEdge == bottomEdge;
	}
};

using DSSRect = PixelBlock; // DSSRect is a misleading name for something that defines a block of pixels.
