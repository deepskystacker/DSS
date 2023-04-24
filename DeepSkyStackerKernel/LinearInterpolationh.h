#pragma once

class CLinearInterpolation
{
private:
	double				xm,
		a0, b0,
		a1, b1;

public:
	CLinearInterpolation() noexcept
	{
		xm = 0;
		a0 = b0 = a1 = b1 = 0;
	};

	CLinearInterpolation(const CLinearInterpolation&) = default;

	CLinearInterpolation(CLinearInterpolation&&) = default;

	CLinearInterpolation& operator = (const CLinearInterpolation&) = default;

	double	Interpolate(double x) const noexcept
	{
		if (x < xm)
			return a0 * x + b0;
		else
			return a1 * x + b1;
	};

	void Initialize(double x0, double x1, double x2, double y0, double y1, double y2) noexcept
	{
		xm = x1;
		if (x0 < x1)
			a0 = (y0 - y1) / (x0 - x1);
		else
			a0 = 0;

		b0 = y0 - a0 * x0;

		if (x1 < x2)
			a1 = (y1 - y2) / (x1 - x2);
		else
			a1 = 0;
		b1 = y1 - a1 * x1;
	};

	float getParameterXm() const noexcept { return static_cast<float>(this->xm); }
	float getParameterA0() const noexcept { return static_cast<float>(this->a0); }
	float getParameterA1() const noexcept { return static_cast<float>(this->a1); }
	float getParameterB0() const noexcept { return static_cast<float>(this->b0); }
	float getParameterB1() const noexcept { return static_cast<float>(this->b1); }
};