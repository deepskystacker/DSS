#pragma once
#include "dssbase.h"
#include "zexcbase.h"

/* ------------------------------------------------------------------- */

namespace DSS
{
	class Histogram
	{
	private:
		std::vector<size_t>		values;
		size_t	intMax;
		double		absoluteMax;
		double		maximum;
		double		minimum;
		double		step;
		double		sum;
		double		sumOfSquares;
		size_t		valueCount;
		bool		initialised;

	public:
		Histogram()
		{
			sum = 0;
			sumOfSquares = 0;
			valueCount = 0;
			intMax = 0;
			maximum = 0;
			minimum = -1;
			initialised = false;
			absoluteMax = 0;
			step = 0;
		}

		Histogram& operator=(const Histogram&) = default;

		virtual ~Histogram() {}

		void	init()
		{
			initialised = false;
			clear();
			const double numberOfSteps = absoluteMax / step;
			size_t size = std::isfinite(numberOfSteps) ? (static_cast<size_t>(numberOfSteps) + 1) : 1;

			values.resize(size);

			initialised = true;
		}
		void init(const size_t size)
		{
			initialised = false;
			clear();
			values.resize(size);
			initialised = true;
		}

		void	clear()
		{
			values.clear();
			sum = 0;
			sumOfSquares = 0;
			valueCount = 0;
			intMax = 0;
			maximum = 0;
			minimum = -1;

			if (initialised)
			{
				double size = absoluteMax / step + 1;
				values.resize(static_cast<size_t>(size));
			};
		}

		void	SetSize(double fMax, double fStep)
		{
			absoluteMax = fMax;
			step = fStep;

			init();
		}

		void	SetSize(double fMax, size_t size)
		{
			absoluteMax = fMax;
			step = fMax == 0.0 ? std::numeric_limits<double>::min() : (fMax / (size - 1));

			init(size);
		}

		size_t	GetSize()
		{
			return values.size();
		}

		void	AddValue(double fValue, size_t number = 1)
		{
			size_t stepNum = static_cast<int>((fValue / step));

			if (stepNum < values.size())
			{
				values[stepNum] += number;
				valueCount += number;
				sumOfSquares += (fValue * fValue) * number;
				sum += fValue * number;
				intMax = std::max(intMax, values[stepNum]);

				maximum = std::max(maximum, fValue);
				if (minimum < 0)
					minimum = fValue;
				else
					minimum = min(minimum, fValue);
			};
		}

		void	AddValues(const Histogram& Histogram)
		{
			for (int i = 0; i < Histogram.values.size(); i++)
			{
				if (Histogram.values[i])
					AddValue(i * step, Histogram.values[i]);
			};
		}

		size_t	GetNrValues()
		{
			return values.size();
		}

		size_t	GetValue(double fValue) const
		{
			return values[static_cast<size_t>(fValue / step)];
		}

		double GetValue(size_t i) const
		{
			return static_cast<double>(values[i]);
		}

		double	componentValue(size_t index) const
		{
			return static_cast<double>(index * step);
		}

		double	GetAverage()
		{
			double		fResult = 0;

			if (valueCount)
				fResult = sum / valueCount;

			return fResult;
		}

		double	GetMin()
		{
			return minimum;
		}

		double	GetMax()
		{
			return maximum;
		}

		double	GetStdDeviation()
		{
			double		fResult = 0;

			if (valueCount)
				fResult = sqrt(sumOfSquares / valueCount - pow(sum / valueCount, 2));

			return fResult;
		}

		double	GetMedian()
		{
			double		fResult = 0;

			if (valueCount)
			{
				size_t lCount = 0;
				int		i = 0;

				while ((lCount + values[i]) <= static_cast<unsigned int>(valueCount / 2))
				{
					lCount += values[i];
					i++;
				};
				// The median is i
				fResult = i * step;
			};

			return fResult;
		}

		size_t GetMaximumNrValues()
		{
			return intMax;
		}
	};


	/* ------------------------------------------------------------------- */

	class RGBHistogram
	{
	private:

		Histogram				redHistogram;
		Histogram				greenHistogram;
		Histogram				blueHistogram;

	public:
		RGBHistogram() {}
		virtual ~RGBHistogram() {}

		void	clear()
		{
			redHistogram.clear();
			greenHistogram.clear();
			blueHistogram.clear();
		}

		bool	IsInitialized()
		{
			return redHistogram.GetSize() && greenHistogram.GetSize() && blueHistogram.GetSize();
		}

		size_t GetSize()
		{
			return redHistogram.GetSize();
		}

		void	SetSize(double fMax, double fStep)
		{
			redHistogram.SetSize(fMax, fStep);
			greenHistogram.SetSize(fMax, fStep);
			blueHistogram.SetSize(fMax, fStep);
		}

		void	SetSize(double fMax, size_t lNrValues)
		{
			redHistogram.SetSize(fMax, lNrValues);
			greenHistogram.SetSize(fMax, lNrValues);
			blueHistogram.SetSize(fMax, lNrValues);
		}

		void	AddValues(double fRed, double fGreen, double fBlue)
		{
			redHistogram.AddValue(fRed);
			greenHistogram.AddValue(fGreen);
			blueHistogram.AddValue(fBlue);
		}

		void	AddValues(const RGBHistogram& RGBHistogram)
		{
			redHistogram.AddValues(RGBHistogram.redHistogram);
			greenHistogram.AddValues(RGBHistogram.greenHistogram);
			blueHistogram.AddValues(RGBHistogram.blueHistogram);
		}

		void	GetValues(size_t index, double& lNrReds, double& lNrGreens, double& lNrBlues)
		{
			lNrReds = redHistogram.GetValue(index);
			lNrGreens = greenHistogram.GetValue(index);
			lNrBlues = blueHistogram.GetValue(index);
		}

		Histogram& GetRedHistogram()
		{
			return redHistogram;
		}

		Histogram& GetGreenHistogram()
		{
			return greenHistogram;
		}

		Histogram& GetBlueHistogram()
		{
			return blueHistogram;
		}
	};
} // namespace DSS
