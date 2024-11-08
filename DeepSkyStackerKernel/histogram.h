#pragma once

#include "zexcbase.h"
//#include "resource.h"

/* ------------------------------------------------------------------- */

inline double LinearAdjust(double fValue)
{
	ZASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double CubeRootAdjust(double fValue)
{
	ZASSERT(fValue >= 0 && fValue <= 1);
	fValue = pow(fValue, 1 / 3.0);
	ZASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double SquareRootAdjust(double fValue)
{
	ZASSERT(fValue >= 0 && fValue <= 1);
	fValue = pow(fValue, 1 / 2.0);
	ZASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double LogAdjust(double fValue)
{
	ZASSERT(fValue >= 0 && fValue <= 1);
	fValue = log(fValue * 1.7 + 1);
	ZASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double LogLogAdjust(double fValue)
{
	ZASSERT(fValue >= 0 && fValue <= 1);
	fValue = log(log(fValue * 1.7 + 1) * 1.7 + 1);
	ZASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double LogSquareRootAdjust(double fValue)
{
	ZASSERT(fValue >= 0 && fValue <= 1);
	fValue = log(pow(fValue, 1 / 2.0) * 1.7 + 1);
	ZASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

inline double AsinHAdjust(double fValue)
{
	ZASSERT(fValue >= 0 && fValue <= 1);
	fValue *= 1.15;
	fValue = log(fValue + sqrt(fValue * fValue + 1));
	ZASSERT(fValue >= 0 && fValue <= 1);
	return fValue;
};

namespace DSS
{
	enum class HistogramAdjustmentCurve : uint
	{
		//
		// HistogramAdjustmentCurve is 1 based not zero based for compatability 
		// with the MFC code.
		//
		Linear = 1,
		CubeRoot = 2,
		SquareRoot = 3,
		Log = 4,
		LogLog = 5,
		LogSquareRoot = 6,
		ASinH = 7
	};
}

inline QString HistoAdjustTypeText(DSS::HistogramAdjustmentCurve hat)
{
	switch (hat)
	{
	case DSS::HistogramAdjustmentCurve::Linear:
		return QCoreApplication::translate("Histogram", "Linear", "IDS_HAT_LINEAR");
		break;
	case DSS::HistogramAdjustmentCurve::CubeRoot:
		return QCoreApplication::translate("Histogram", "Cube Root", "IDS_HAT_CUBEROOT");
		break;
	case DSS::HistogramAdjustmentCurve::SquareRoot:
		return QCoreApplication::translate("Histogram", "Square Root", "IDS_HAT_SQUAREROOT");
		break;
	case DSS::HistogramAdjustmentCurve::Log:
		return QCoreApplication::translate("Histogram", "Logarithm", "IDS_HAT_LOG");
		break;
	case DSS::HistogramAdjustmentCurve::LogLog:
		return QCoreApplication::translate("Histogram", "Log(Log)", "IDS_HAT_LOGLOG");
		break;
	case DSS::HistogramAdjustmentCurve::LogSquareRoot:
		return QCoreApplication::translate("Histogram", "Log(Square Root)", "IDS_HAT_LOGSQUAREROOT");
		break;
	case DSS::HistogramAdjustmentCurve::ASinH:
		return QCoreApplication::translate("Histogram", "ASinH", "IDS_HAT_ASINH");
		break;
	};
	return "";
};


namespace DSS
{
	class HistogramAdjust
	{
		friend class RGBHistogramAdjust;

	private:
		double					minimum;
		double					maximum;
		double					shift;

		double					originalMinimum;
		double					originalMaximum;

		double					usedMinimum;
		double					usedMaximum;

		HistogramAdjustmentCurve adjustmentCurve;

		//
		// loadSettings and saveSettings are deliberately private and should only ever be
		// called from the same named member function in RGBHistogramAdjust (which is a 
		// friend so we can do that).
		//
		void loadSettings(const QString& groupName)
		{
			ZFUNCTRACE_RUNTIME();
			QSettings settings;
			settings.beginGroup(groupName);
			minimum = settings.value("Minimum").toDouble();
			maximum = settings.value("Maximum").toDouble();
			shift = settings.value("Shift").toDouble();
			originalMinimum = settings.value("OriginalMinimum").toDouble();
			originalMaximum = settings.value("OriginalMaximum").toDouble();
			usedMinimum = settings.value("UsedMinimum").toDouble();
			usedMaximum = settings.value("UsedMaximum").toDouble();
			adjustmentCurve = static_cast<HistogramAdjustmentCurve>(settings.value("AdjustmentCurve").toUInt());
		};

		void saveSettings(const QString& groupName) const
		{
			ZFUNCTRACE_RUNTIME();
			QSettings settings;
			settings.beginGroup(groupName);
			settings.setValue("Minimum", minimum);
			settings.setValue("Maximum", maximum);
			settings.setValue("Shift", shift);
			settings.setValue("OriginalMinimum", originalMinimum);
			settings.setValue("OriginalMaximum", originalMaximum);
			settings.setValue("UsedMinimum", usedMinimum);
			settings.setValue("UsedMaximum", usedMaximum);
			settings.setValue("AdjustmentCurve", static_cast<uint>(adjustmentCurve));
		};

		double	AdjustValue(double fValue) const
		{
			if (!std::isfinite(fValue))
				return 0;

			switch (adjustmentCurve)
			{
			case HistogramAdjustmentCurve::CubeRoot:
				return CubeRootAdjust(fValue);
				break;
			case HistogramAdjustmentCurve::SquareRoot:
				return SquareRootAdjust(fValue);
				break;
			case HistogramAdjustmentCurve::Log:
				return LogAdjust(fValue);
				break;
			case HistogramAdjustmentCurve::LogLog:
				return LogLogAdjust(fValue);
				break;
			case HistogramAdjustmentCurve::LogSquareRoot:
				return LogSquareRootAdjust(fValue);
				break;
			case HistogramAdjustmentCurve::ASinH:
				return AsinHAdjust(fValue);
				break;
			case HistogramAdjustmentCurve::Linear:
			default:
				return LinearAdjust(fValue);
				break;
			};
		};

		double	ExtractValue(const QString& szString, const QString& szVariable)
		{
			double fValue = 0.0;
			QString strVariable(szVariable + "=");
			qsizetype nPos = szString.indexOf(strVariable, 0);
			if (nPos >= 0)
			{
				qsizetype nStart = nPos + strVariable.length();
				qsizetype nEnd = szString.indexOf(";", nStart);
				if (nEnd < 0)
					nEnd = szString.indexOf("}", nStart);
				if (nEnd > nStart)
					fValue = szString.mid(nStart, nEnd - nStart).toFloat();
			}
			return fValue;
		}

	public:
		HistogramAdjust()
		{
			adjustmentCurve = HistogramAdjustmentCurve::LogSquareRoot;
			setOriginalValues(0.0, 65535.0);

			minimum = 0;
			maximum = 0;
			shift = 0;
		};

		virtual ~HistogramAdjust() {};

		double getOriginalMinimum() const { return originalMinimum; }
		double getOriginalMaximum() const { return originalMaximum; }
		double getUsedMinimum() const { return usedMinimum; }
		double getUsedMaximum() const { return usedMaximum; }


		void	reset()
		{
			adjustmentCurve = HistogramAdjustmentCurve::LogSquareRoot;
			setOriginalValues(0.0, 65535.0);
			SetNewValues(0.0, 65535.0, 0.0);
		};

		HistogramAdjust(const HistogramAdjust& ha) = default;

		HistogramAdjust& operator = (const HistogramAdjust& ha) = default;

		void	setOriginalValues(double min, double max)
		{
			originalMinimum = min;
			originalMaximum = max;
			usedMinimum = (originalMaximum - originalMinimum) * 0.05;
			usedMaximum = originalMaximum - usedMinimum;
		};

		void	SetNewValues(double fMin, double fMax, double fShift)
		{
			minimum = fMin;
			maximum = fMax;
			shift = fShift;
		};

		void	SetAdjustMethod(HistogramAdjustmentCurve hat)
		{
			adjustmentCurve = hat;
		};

		HistogramAdjustmentCurve GetAdjustMethod() const
		{
			return adjustmentCurve;
		};

		double	GetMin() const
		{
			return minimum;
		};

		double	GetMax() const
		{
			return maximum;
		};

		double	GetShift() const
		{
			return shift;
		};

		double	Adjust(double fValue) const
		{
			double		fResult;

			if (fValue < minimum)
				fResult = originalMinimum + fValue / max(1.0, minimum - originalMinimum) * (usedMinimum - originalMinimum);
			else if (fValue > maximum)
				fResult = originalMaximum - (fValue - maximum) / max(1.0, originalMaximum - maximum) * (originalMaximum - usedMaximum);
			else
				fResult = usedMinimum + AdjustValue((fValue - minimum) / max(1.0, (maximum - minimum))) * (usedMaximum - usedMinimum);

			// Then shift the value
			fResult = fResult + (usedMaximum - usedMinimum) * shift;
			if (fResult < originalMinimum)
				fResult = originalMinimum + fValue / max(1.0, minimum - originalMinimum) * (usedMinimum - originalMinimum);
			else if (fResult > originalMaximum)
				fResult = originalMaximum - (fValue - maximum) / max(1.0, originalMaximum - maximum) * (originalMaximum - usedMaximum);

			return fResult;
		};

		void	ToText(QString& strParameters) const
		{
			strParameters = QString("Min=%1;Max=%2;Shift=%3;MinOrg=%4;MaxOrg=%5;MinUsed=%6;MaxUsed=%7;HAT=%8;")
				.arg(minimum, 0, 'f', 2)
				.arg(maximum, 0, 'f', 2)
				.arg(shift, 0, 'f', 2)
				.arg(originalMinimum, 0, 'f', 2)
				.arg(originalMaximum, 0, 'f', 2)
				.arg(usedMinimum, 0, 'f', 2)
				.arg(usedMaximum, 0, 'f', 2)
				.arg(static_cast<int>(adjustmentCurve));
		};

		void	FromText(const QString& szParameters)
		{
			minimum = ExtractValue(szParameters, "Min");
			maximum = ExtractValue(szParameters, "Max");
			shift = ExtractValue(szParameters, "Shift");
			originalMinimum = ExtractValue(szParameters, "MinOrg");
			originalMaximum = ExtractValue(szParameters, "MaxOrg");
			usedMinimum = ExtractValue(szParameters, "MinUsed");
			usedMaximum = ExtractValue(szParameters, "MaxUsed");

			int				lValue;

			lValue = ExtractValue(szParameters, "HAT");
			adjustmentCurve = (HistogramAdjustmentCurve)lValue;
		};
	};

	/* ------------------------------------------------------------------- */

	class RGBHistogramAdjust
	{
	private:
		HistogramAdjust		redAdjustment;
		HistogramAdjust		greenAdjustment;
		HistogramAdjust		blueAdjustment;

		//void	CopyFrom(const RGBHistogramAdjust & ha)
		//{
		//	redAdjustment	= ha.redAdjustment;
		//	greenAdjustment	= ha.greenAdjustment;
		//	blueAdjustment	= ha.blueAdjustment;
		//};

		void	ExtractParameters(const QString& szParameters, const QString& szSub, HistogramAdjust& ha)
		{
			QString strSub(szSub + "{");
			qsizetype nPos = szParameters.indexOf(strSub);
			if (nPos >= 0)
			{
				qsizetype nStart = nPos + strSub.length();
				qsizetype nEnd = szParameters.indexOf("}", nStart);
				if (nEnd > nStart)
				{
					QString strValue(szParameters.mid(nStart, nEnd - nStart));
					ha.FromText(strValue);
				}
			}
		}

	public:
		RGBHistogramAdjust() {	};
		virtual ~RGBHistogramAdjust() {};

		void	reset()
		{
			redAdjustment.reset();
			greenAdjustment.reset();
			blueAdjustment.reset();
		};

		RGBHistogramAdjust(const RGBHistogramAdjust& ha) = default;
		//{
		//	CopyFrom(ha);
		//};

		RGBHistogramAdjust& operator = (const RGBHistogramAdjust& ha) = default;
		//{
		//	CopyFrom(ha);
		//	return (*this);
		//};

		void	Adjust(double& fRed, double& fGreen, double& fBlue) const
		{
			fRed = redAdjustment.Adjust(fRed);
			fGreen = greenAdjustment.Adjust(fGreen);
			fBlue = blueAdjustment.Adjust(fBlue);
		};

		const HistogramAdjust& GetRedAdjust() const
		{
			return redAdjustment;
		};

		HistogramAdjust& GetRedAdjust()
		{
			return redAdjustment;
		};

		const HistogramAdjust& GetGreenAdjust() const
		{
			return greenAdjustment;
		};

		HistogramAdjust& GetGreenAdjust()
		{
			return greenAdjustment;
		};

		const HistogramAdjust& GetBlueAdjust() const
		{
			return blueAdjustment;
		};

		HistogramAdjust& GetBlueAdjust()
		{
			return blueAdjustment;
		};

		void loadSettings(const QString& group)
		{
			const QString redGroup{ group + "/RedHistogramAdjust" };
			const QString greenGroup{ group + "/GreenHistogramAdjust" };
			const QString blueGroup{ group + "/BlueHistogramAdjust" };
			redAdjustment.loadSettings(redGroup);
			greenAdjustment.loadSettings(greenGroup);
			blueAdjustment.loadSettings(blueGroup);
		}

		void saveSettings(const QString& group) const
		{
			const QString redGroup{ group + "/RedHistogramAdjust" };
			const QString greenGroup{ group + "/GreenHistogramAdjust" };
			const QString blueGroup{ group + "/BlueHistogramAdjust" };
			redAdjustment.saveSettings(redGroup);
			greenAdjustment.saveSettings(greenGroup);
			blueAdjustment.saveSettings(blueGroup);
		}

		void	ToText(QString& strParameters) const
		{
			QString strRedParameters;
			QString strGreenParameters;
			QString strBlueParameters;

			redAdjustment.ToText(strRedParameters);
			greenAdjustment.ToText(strGreenParameters);
			blueAdjustment.ToText(strBlueParameters);

			strParameters = QString("RedAdjust{%1}GreenAdjust{%2}BlueAdjust{%3}")
				.arg(strRedParameters)
				.arg(strGreenParameters)
				.arg(strBlueParameters);
		};

		void	FromText(const QString& szParameters)
		{
			static const QString strRedAdjust("RedAdjust");
			static const QString strGreenAdjust("GreenAdjust");
			static const QString strBlueAdjust("BlueAdjust");
			ExtractParameters(szParameters, strRedAdjust, redAdjustment);
			ExtractParameters(szParameters, strGreenAdjust, greenAdjustment);
			ExtractParameters(szParameters, strBlueAdjust, blueAdjustment);
		};
	};

	/* ------------------------------------------------------------------- */

	class Histogram
	{
	private:
		std::vector<std::uint32_t>		values;
		uint32_t	intMax;
		double		absoluteMax;
		double		maximum;
		double		minimum;
		double		step;
		double		sum;
		double		sumOfSquares;
		int			valueCount;
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
		};

		Histogram& operator=(const Histogram&) = default;

		virtual ~Histogram() {};

		void	init()
		{
			int		lNrValues = 0;

			initialised = false;
			clear();
			const double numberOfSteps = absoluteMax / step;
			lNrValues = std::isfinite(numberOfSteps) ? (static_cast<int>(numberOfSteps) + 1) : 1;

			values.resize(lNrValues);

			initialised = true;
		};
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
				int		lNrValues;

				lNrValues = (int)(absoluteMax / step + 1);
				values.resize(lNrValues);
			};
		};

		void	SetSize(double fMax, double fStep)
		{
			absoluteMax = fMax;
			step = fStep;

			init();
		};

		void	SetSize(double fMax, int lNrValues)
		{
			absoluteMax = fMax;
			step = fMax == 0.0 ? std::numeric_limits<double>::min() : (fMax / (lNrValues - 1));

			init(lNrValues);
		};

		int	GetSize()
		{
			return (int)values.size();
		};

		void	AddValue(double fValue, int lNrValues = 1)
		{
			int		lNrStep;

			lNrStep = (int)(fValue / step);

			if (lNrStep < values.size())
			{
				values[lNrStep] += lNrValues;
				valueCount += lNrValues;
				sumOfSquares += (fValue * fValue) * lNrValues;
				sum += fValue * lNrValues;
				intMax = max(intMax, static_cast<uint32_t>(values[lNrStep]));

				maximum = max(maximum, fValue);
				if (minimum < 0)
					minimum = fValue;
				else
					minimum = min(minimum, fValue);
			};
		};

		void	AddValues(const Histogram& Histogram)
		{
			for (int i = 0; i < Histogram.values.size(); i++)
			{
				if (Histogram.values[i])
					AddValue(i * step, Histogram.values[i]);
			};
		};

		int	GetNrValues()
		{
			return (int)values.size();
		};

		int	GetValue(double fValue)
		{
			return values[(int)(fValue / step)];
		};

		int	GetValue(int lValue)
		{
			return values[lValue];
		};

		double	GetComponentValue(int lIndice)
		{
			return (double)lIndice * step;
		};

		double	GetAverage()
		{
			double		fResult = 0;

			if (valueCount)
				fResult = sum / valueCount;

			return fResult;
		};

		double	GetMin()
		{
			return minimum;
		};

		double	GetMax()
		{
			return maximum;
		};

		double	GetStdDeviation()
		{
			double		fResult = 0;

			if (valueCount)
				fResult = sqrt(sumOfSquares / valueCount - pow(sum / valueCount, 2));

			return fResult;
		};

		double	GetMedian()
		{
			double		fResult = 0;

			if (valueCount)
			{
				unsigned int		lCount = 0;
				int		i = 0;

				while ((lCount + values[i]) <= (unsigned int)(valueCount / 2))
				{
					lCount += values[i];
					i++;
				};
				// The median is i
				fResult = i * step;
			};

			return fResult;
		};

		int	GetMaximumNrValues()
		{
			return intMax;
		};
	};


	/* ------------------------------------------------------------------- */

	class RGBHistogram
	{
	private:

		Histogram				redHistogram;
		Histogram				greenHistogram;
		Histogram				blueHistogram;

	public:
		RGBHistogram() {};
		virtual ~RGBHistogram() {};

		void	clear()
		{
			redHistogram.clear();
			greenHistogram.clear();
			blueHistogram.clear();
		};

		bool	IsInitialized()
		{
			return redHistogram.GetSize() && greenHistogram.GetSize() && blueHistogram.GetSize();
		};

		int	GetSize()
		{
			return redHistogram.GetSize();
		};

		void	SetSize(double fMax, double fStep)
		{
			redHistogram.SetSize(fMax, fStep);
			greenHistogram.SetSize(fMax, fStep);
			blueHistogram.SetSize(fMax, fStep);
		};

		void	SetSize(double fMax, int lNrValues)
		{
			redHistogram.SetSize(fMax, lNrValues);
			greenHistogram.SetSize(fMax, lNrValues);
			blueHistogram.SetSize(fMax, lNrValues);
		};

		void	AddValues(double fRed, double fGreen, double fBlue)
		{
			redHistogram.AddValue(fRed);
			greenHistogram.AddValue(fGreen);
			blueHistogram.AddValue(fBlue);
		};

		void	AddValues(const RGBHistogram& RGBHistogram)
		{
			redHistogram.AddValues(RGBHistogram.redHistogram);
			greenHistogram.AddValues(RGBHistogram.greenHistogram);
			blueHistogram.AddValues(RGBHistogram.blueHistogram);
		};

		void	GetValues(int lValue, int& lNrReds, int& lNrGreens, int& lNrBlues)
		{
			lNrReds = redHistogram.GetValue(lValue);
			lNrGreens = greenHistogram.GetValue(lValue);
			lNrBlues = blueHistogram.GetValue(lValue);
		};

		Histogram& GetRedHistogram()
		{
			return redHistogram;
		};

		Histogram& GetGreenHistogram()
		{
			return greenHistogram;
		};

		Histogram& GetBlueHistogram()
		{
			return blueHistogram;
		};
	};
} // namespace DSS
