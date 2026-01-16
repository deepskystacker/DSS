#include "pch.h"
#include "BackgroundCalibration.h"
#include "StackingTasks.h"
#include "avx_histogram.h"
#include "Multitask.h"
#include "MemoryBitmap.h"


// How does the Background Calibrator work (example: offset calibrator):
//
// struct OffsetParams {} has 3 parameters offset, min, max, and the initialize() function with the 6 parameters for min/median/max input and min/median/max output.
// struct OffsetModel {} has 3 structs OffsetParams for red, green, blue, and the calibrate() function with the 3 parameters for the red-, green-, and blue color channel.
//   The return value of the calibrate function is type BcRT which is a tuple<double, double, double> for the 3 color channels.
//
// All calculations are done in the template class BackgroundCalibratorVariant.
// ----------------------------------------------------------------------------
//
// The interface simply is:
//   double calculateModelParameters(CMemoryBitmap const& bitmap, const bool calcReference, const char8_t* pFileName);
//   BcRT calibratePixel(const double r, const double g, const double b) const;
//
// calculateModelParameters() must be called by every light frame (parameter bitmap) to basically calculate the median values for R, G, and B of the bitmap.
// If it's the reference frame, set bool calcReference to true, and the BackgroundCalibratorVariant will save the median values as reference (=target) values.
//
// calibratePixel() will be called by every pixel of the light frame to transform the input R/G/B color values to the (calibrated) output values. 
//
// The class BackgroundCalibratorVariant has a data member calibrator, it's type is TVariant, which is a std::variant<Cals...>.
// Cals... is the variadic template parameter pack of the class, defining the different calibrator methods supported. 
// At the moment, this is: 
//   using BackgroundCalibrator = BackgroundCalibratorVariant<NoneModel, OffsetModel, LinearModel, RationalModel>;
//
// So the concrete BackgroundCalibrator is the instantiation of the BackgroundCalibratorVariant with one of the 4 supported calibrator methods (none, offset, linear, rational).
// This instantiation is done in the function:
//   BackgroundCalibrator makeBackgroundCalibrator(
//       const BACKGROUNDCALIBRATIONINTERPOLATION interpolation, const BACKGROUNDCALIBRATIONMODE bcmd, const RGBBACKGROUNDCALIBRATIONMETHOD rgbme, const double multr);
//
// BACKGROUNDCALIBRATIONINTERPOLATION, BACKGROUNDCALIBRATIONMODE, and RGBBACKGROUNDCALIBRATIONMETHOD are the enums that were used by DSS all the time for defining
//   the background calibration details (linear/rational/offset), (rgb/per-channel), (rgb min/middle/max).
//
// The polymorphism is done by means of the std::variant<...> calibrator. From the 4 possible options, one is concretely selected at runtime in the function makeBackgroundCalibrator().
// The dispatch to the concrete calibrate() function of the Model - e.g. OffsetModel::calibrate() - is done (as always with std::variant) by means of std::visit().
//   This is done in BackgroundCalibratorVariant::calibratePixel().
//
// std::variant is from a performance point of view almost optimal, the compiler can inline everything, the dispatch is a single internal switch call. 
//
// There's a separate implementation for SIMD (in avx.cpp).
//
// How to add a new background calibrator:
// ---------------------------------------
// (1) Define the Params struct (which parameters does the calibrator need?).
// (2) Define the Model struct (3 x params for R/G/B + function calibrate - how is the calibration done?).
// (3) Add the new Model to BackgroundCalibratorVariant<...>.
// (4) Add the new Model to makeBackgroundCalibrator() (when shall the new calibrator be used?).
//
// How the individual calibrators work:
// ------------------------------------
//
// 1) OffsetModel
//    Uses the parameters offset, minValue, maxValue. Offset = median[i] - median[reference].
//    Returns std::clamp(x + offset, minValue, maxValue).
//    So the function is linear around the median, but clamps above 0 or below max (depends if offset is smaller or larger than zero).
//
// 2) LinearModel
//    Uses the parameters median, a0, a1, b0, b1. a0, b0 are a linear function if x < median, a1, b1 are another linear function if x >= median.
//    Thus, if x == median[i] then median[reference] is returned, otherwise the values are linearly mapped to [0..median) and [median..max]. 
//    So it consists of 2 (different) linear parts (below and above the median).
//    Returns (x < median) ? (a0 * x + b0) : (a1 * x + b1)
//
// 3) RationalModel
//    Uses the parameters a, b, c, minValue, maxValue.
//    Basically (apart from special cases) returns std::clamp((x + a) / (b * x + c), minValue, maxValue).
//    a, b, c are determined such, that median[i] maps to median[reference]. Below and above that point, the function is smooth (differentiable), but (of course) non-linear.


void OffsetParams::initialize(const double x0, const double x1, const double x2, const double, const double y1, const double)
{
	offset = y1 - x1;
	const auto [mn, mx] = std::ranges::minmax({ x0, x1, x2 });
	minValue = mn;
	maxValue = mx;
}

BcRT OffsetModel::calibrate(const double r, const double g, const double b) const
{
	constexpr auto Calibrate = [](OffsetParams const& params, const double x) -> double
	{
		// std::clamp(x + model.offset, model.minValue, model.maxValue);
		return std::abs(std::min(x + params.offset, params.maxValue));
	};

	return std::make_tuple(Calibrate(redParams, r), Calibrate(greenParams, g), Calibrate(blueParams, b));
}

void LinearParams::initialize(const double x0, const double x1, const double x2, const double y0, const double y1, const double y2)
{
	median = x1;
	a0 = (x0 < x1) ? (y0 - y1) / (x0 - x1) : 0.0;
	a1 = (x1 < x2) ? (y1 - y2) / (x1 - x2) : 0.0;
	b0 = y0 - a0 * x0;
	b1 = y1 - a1 * x1;
}

BcRT LinearModel::calibrate(const double r, const double g, const double b) const
{
	constexpr auto Calibrate = [](LinearParams const& params, const double x) -> double
	{
		return (x < params.median) ? (params.a0 * x + params.b0) : (params.a1 * x + params.b1);
	};

	return std::make_tuple(Calibrate(redParams, r), Calibrate(greenParams, g), Calibrate(blueParams, b));
}

void RationalParams::initialize(const double x0, const double x1, const double x2, const double y0, const double y1, const double y2)
{
	const double t1 = ((x0 * y0 - x1 * y1) * (y0 - y2) - (x0 * y0 - x2 * y2) * (y0 - y1));
	const double t2 = ((x0 - x1) * (y0 - y2) - (x0 - x2) * (y0 - y1));
	const double t3 = (y0 - y1);

	b = t1 != 0 ? t2 / t1 : 0.0;
	c = t3 != 0 ? ((x0 - x1) - b * (x0 * y0 - x1 * y1)) / t3 : 0.0;
	a = (b * x0 + c) * y0 - x0;
	const auto [mn, mx] = std::ranges::minmax({ y0, y1, y2 });
	minValue = mn;
	maxValue = mx;
}

BcRT RationalModel::calibrate(const double r, const double g, const double b) const
{
	constexpr auto Calibrate = [](RationalParams const& params, const double x) -> double
	{
		return (params.b != 0 || params.c != 0)
			? std::clamp((x + params.a) / (params.b * x + params.c), params.minValue, params.maxValue)
			: std::clamp((x + params.a), params.minValue, params.maxValue);
	};

	return std::make_tuple(Calibrate(redParams, r), Calibrate(greenParams, g), Calibrate(blueParams, b));
}

template <IsCalibrator... Cals>
BackgroundCalibratorVariant<Cals...>::TVariant const& BackgroundCalibratorVariant<Cals...>::getCalibratorVariant() const
{
	return this->calibrator;
}

template <IsCalibrator... Cals>
BackgroundCalibratorVariant<Cals...>::TVariant& BackgroundCalibratorVariant<Cals...>::getCalibratorVariant()
{
	return this->calibrator;
}

template <IsCalibrator... Cals>
std::tuple<std::vector<int>, std::vector<int>, std::vector<int>> BackgroundCalibratorVariant<Cals...>::calcHistogram(CMemoryBitmap const& bitmap)
{
	AvxHistogram avxHistogram(bitmap);
	const int height = bitmap.Height();
	const auto nrProcessors = Multitask::GetNrProcessors();

	AvxHistogram::HistogramVectorType redHisto(HistogramSize, 0);
	AvxHistogram::HistogramVectorType greenHisto(HistogramSize, 0);
	AvxHistogram::HistogramVectorType blueHisto(HistogramSize, 0);

#pragma omp parallel default(shared) shared(redHisto, greenHisto, blueHisto) firstprivate(avxHistogram) if(nrProcessors > 1)
	{
		constexpr int Bulksize = 50;
#pragma omp for
		for (int startNdx = 0; startNdx < height; startNdx += Bulksize)
		{
			const int endNdx = std::min(startNdx + Bulksize, height);
			avxHistogram.calcHistogram(startNdx, endNdx, multiplier);
		}
#pragma omp critical(OmpLockHistoMerge)
		{
			avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto);
		}
	} // omp parallel

	return { redHisto, greenHisto, blueHisto };
}

template <IsCalibrator... Cals>
std::pair<double, double> BackgroundCalibratorVariant<Cals...>::findMedianAndMax(const std::span<const int> histo, const size_t halfNumberOfPixels)
{
	size_t index = 0;
	for (size_t nrValues = 0; nrValues < halfNumberOfPixels;)
		nrValues += histo[index++];
	const double median = static_cast<double>(index) / multiplier;

	double maximum = 0;
	index = HistogramSize - 1;
	for (auto it = std::crbegin(histo); it != std::crend(histo); ++it, --index)
	{
		if (*it != 0)
		{
			maximum = static_cast<double>(index);
			break;
		}
	}

	return { median, maximum };
}

template <IsCalibrator... Cals>
void BackgroundCalibratorVariant<Cals...>::calculateReferenceParameters(
	const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax)
{
	if (this->mode == Mode::RGB)
	{
		double targetBackground = 0;

		constexpr auto Median3 = [](double a, double b, double c) -> double
			{
				if (a > b) std::swap(a, b);
				if (b > c) std::swap(b, c);
				if (a > b) std::swap(a, b);
				return b;
			};

		if (this->rgbMethod == RgbMethod::Maximum)
		{
			targetBackground = std::ranges::max({ redMedian, greenMedian, blueMedian });
		}
		else if (this->rgbMethod == RgbMethod::Minimum)
		{
			targetBackground = std::ranges::min({ redMedian, greenMedian, blueMedian });
		}
		else
		{
			targetBackground = Median3(redMedian, greenMedian, blueMedian);
		}

		this->referenceBackground.red = std::min(redMax, targetBackground);
		this->referenceBackground.green = std::min(greenMax, targetBackground);
		this->referenceBackground.blue = std::min(blueMax, targetBackground);
	}
	else // per channel
	{
		this->referenceBackground.red = redMedian;
		this->referenceBackground.green = greenMedian;
		this->referenceBackground.blue = blueMedian;
	}
	ZTRACE_RUNTIME(std::format("Reference Background: red = {:.2f}, green = {:.2f}, blue = {:.2f}",
		referenceBackground.red / 256.0, referenceBackground.green / 256.0, referenceBackground.blue / 256.0)
	);
}

template <IsCalibrator... Cals>
double BackgroundCalibratorVariant<Cals...>::calculateModelParameters(CMemoryBitmap const& bitmap, const bool calcReference, const char8_t* pFileName)
{
	const size_t halfNumberOfPixels = static_cast<size_t>(bitmap.Width()) * static_cast<size_t>(bitmap.Height()) / 2;

	const auto [redHisto, greenHisto, blueHisto] = calcHistogram(bitmap);
	const auto [redMedian, redMaximum] = findMedianAndMax(redHisto, halfNumberOfPixels);
	const auto [greenMedian, greenMaximum] = findMedianAndMax(greenHisto, halfNumberOfPixels);
	const auto [blueMedian, blueMaximum] = findMedianAndMax(blueHisto, halfNumberOfPixels);

	ZTRACE_RUNTIME(std::format("Background Calibration median parameters: red = {:.2f}, green = {:.2f}, blue = {:.2f}", redMedian / 256.0, greenMedian / 256.0, blueMedian / 256.0));

	if (calcReference)
	{
		const auto calibratorName = std::visit([](auto const& vari)
			{
				using ModelType = std::remove_cvref_t<decltype(vari)>;
				return ModelType::name;
			},
			this->calibrator
		);

		ZTRACE_RUNTIME("Reference frame: %s | Calibrator Algorithm = %s, Mode = %s, RGB-Point = %s",
			pFileName == nullptr ? u8"-" : pFileName,
			calibratorName.data(),
			mode == Mode::PerChannel ? "Per Channel" : "RGB",
			mode == Mode::RGB ? (rgbMethod == RgbMethod::Minimum ? "Min" : (rgbMethod == RgbMethod::Median ? "Median" : "Max")) : "-"
		);

		calculateReferenceParameters(redMedian, redMaximum, greenMedian, greenMaximum, blueMedian, blueMaximum);
	}

	std::visit([this, redMedian, redMaximum, greenMedian, greenMaximum, blueMedian, blueMaximum](auto& c)
		{
			c.redParams.initialize(0, redMedian, redMaximum, 0, this->referenceBackground.red, redMaximum);
			c.greenParams.initialize(0, greenMedian, greenMaximum, 0, this->referenceBackground.green, greenMaximum);
			c.blueParams.initialize(0, blueMedian, blueMaximum, 0, this->referenceBackground.blue, blueMaximum);
		},
		calibrator
	);

	return redMedian;
}

BackgroundCalibrator makeBackgroundCalibrator(
	const BACKGROUNDCALIBRATIONINTERPOLATION interpolation, const BACKGROUNDCALIBRATIONMODE bcmd, const RGBBACKGROUNDCALIBRATIONMETHOD rgbme, const double multr)
{
	const Mode mode = bcmd == BCM_RGB ? Mode::RGB : Mode::PerChannel;
	const RgbMethod rgbMethod = rgbme == RBCM_MINIMUM ? RgbMethod::Minimum : (rgbme == RBCM_MIDDLE ? RgbMethod::Median : RgbMethod::Maximum);

	if (bcmd == BCM_NONE)
		return BackgroundCalibrator{ NoneModel{}, multr, mode, rgbMethod };

	switch (interpolation)
	{
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_LINEAR: return BackgroundCalibrator{ LinearModel{}, multr, mode, rgbMethod };
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_RATIONAL: return BackgroundCalibrator{ RationalModel{}, multr, mode, rgbMethod };
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_OFFSET: return BackgroundCalibrator{ OffsetModel{}, multr, mode, rgbMethod };
	default: return BackgroundCalibrator{ NoneModel{}, multr, mode, rgbMethod };
	}
}

// Explicit template instantiation
template class BackgroundCalibratorVariant<NoneModel, OffsetModel, LinearModel, RationalModel>;
