#pragma once

class CRationalInterpolation
{
private:
	double					a, b, c;
	double					fMin, fMax;

private:

public:
	CRationalInterpolation()
	{
		a = b = 0;
		c = 1;
		fMin = 0;
		fMax = 0;
	};

	CRationalInterpolation(const CRationalInterpolation&) = default;

	CRationalInterpolation& operator = (const CRationalInterpolation&) = default;

	double	Interpolate(double x) const noexcept
	{
		if (b || c)
			return std::max(std::min((x + a) / (b * x + c), fMax), fMin);
		else
			return std::max(std::min(x + a, fMax), fMin);
	};

	void Initialize(double x0, double x1, double x2, double y0, double y1, double y2) noexcept
	{
		double				t1 = ((x0 * y0 - x1 * y1) * (y0 - y2) - (x0 * y0 - x2 * y2) * (y0 - y1));
		double				t2 = ((x0 - x1) * (y0 - y2) - (x0 - x2) * (y0 - y1));
		double				t3 = (y0 - y1);

		if (t1)
			b = t2 / t1;
		else
			b = 0;
		if (t3)
			c = ((x0 - x1) - b * (x0 * y0 - x1 * y1)) / t3;
		else
			c = 0;
		a = (b * x0 + c) * y0 - x0;

		fMin = std::min(std::min(y0, y1), y2);
		fMax = std::max(std::max(y0, y1), y2);
	};

	float getParameterA() const noexcept { return static_cast<float>(this->a); }
	float getParameterB() const noexcept { return static_cast<float>(this->b); }
	float getParameterC() const noexcept { return static_cast<float>(this->c); }
	float getParameterMin() const noexcept { return static_cast<float>(this->fMin); }
	float getParameterMax() const noexcept { return static_cast<float>(this->fMax); }
};