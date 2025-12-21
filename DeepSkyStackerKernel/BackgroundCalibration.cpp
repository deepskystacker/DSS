#include "pch.h"
#include "BackgroundCalibration.h"
#include "StackingTasks.h"
#include "avx_histogram.h"
#include "Multitask.h"
#include "MemoryBitmap.h"


// --------------------------
// Interface
// --------------------------

namespace {
	using BCI = BackgroundCalibrationInterface;
	using Interpolation = BCI::Interpolation;
	using Mode = BCI::Mode;
	using RgbMethod = BCI::RgbMethod;

	template <int Mult, typename Type, typename... OtherTypes>
		requires (
			(std::same_as<Type, std::uint8_t> || std::same_as<Type, std::uint16_t> || std::same_as<Type, std::uint32_t> || std::same_as<Type, float> || std::same_as<Type, double>)
			&& (Mult == 1 || Mult == 256) // we only support those 2 multipliers
		)
	std::shared_ptr<BCI> createTyped(const Interpolation interpolationMethod, const Mode mode, const RgbMethod rgbm, const int bitsPerSample, const bool isIntegral)
	{
		if (bitsPerSample == sizeof(Type) * 8 && isIntegral == std::integral<Type>)
		{
			if (mode == Mode::None)
				return std::make_shared<BackgroundCalibrationNone<Mult, Type>>();
			switch (interpolationMethod)
			{
			case Interpolation::Linear: return std::make_shared<BackgroundCalibrationLinear<Mult, Type>>(mode, rgbm);
			case Interpolation::Rational: return std::make_shared<BackgroundCalibrationRational<Mult, Type>>(mode, rgbm);
			case Interpolation::Offset: return std::make_shared<BackgroundCalibrationOffset<Mult, Type>>(mode, rgbm);
			default: return {};
			}
		}
		if constexpr (sizeof...(OtherTypes) != 0)
			return createTyped<Mult, OtherTypes...>(interpolationMethod, mode, rgbm, bitsPerSample, isIntegral);
		else
			return {};
	}
}

template <int Mult>
	requires (Mult == 1 || Mult == 256)
std::shared_ptr<BackgroundCalibrationInterface> BackgroundCalibrationInterface::makeBackgroundCalibrator(const BACKGROUNDCALIBRATIONINTERPOLATION interpolation,
	const BACKGROUNDCALIBRATIONMODE bcmd, const RGBBACKGROUNDCALIBRATIONMETHOD rgbme, const int bitsPerSample, const bool integral)
{
	Interpolation intMethod = Interpolation::Linear;
//	switch (CAllStackingTasks::GetBackgroundCalibrationInterpolation())
	switch (interpolation)
	{
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_LINEAR: break;
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_RATIONAL:
		intMethod = Interpolation::Rational; break;
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_OFFSET:
		intMethod = Interpolation::Offset; break;
	default: break;
	}

	const Mode mode = bcmd == BCM_RGB ? Mode::RGB : (bcmd == BCM_PERCHANNEL ? Mode::PerChannel : Mode::None);
	const RgbMethod rgbMethod = rgbme == RBCM_MINIMUM ? RgbMethod::Minimum : (rgbme == RBCM_MIDDLE ? RgbMethod::Median : RgbMethod::Maximum);

	return createTyped<Mult, std::uint8_t, std::uint16_t, std::uint32_t, float, double>(intMethod, mode, rgbMethod, bitsPerSample, integral);
}

// Explicit template instantiations
// --------------------------------
template
std::shared_ptr<BackgroundCalibrationInterface> BackgroundCalibrationInterface::makeBackgroundCalibrator<1>(
	const BACKGROUNDCALIBRATIONINTERPOLATION, const BACKGROUNDCALIBRATIONMODE, const RGBBACKGROUNDCALIBRATIONMETHOD, const int, const bool);
template
std::shared_ptr<BackgroundCalibrationInterface> BackgroundCalibrationInterface::makeBackgroundCalibrator<256>(
	const BACKGROUNDCALIBRATIONINTERPOLATION, const BACKGROUNDCALIBRATIONMODE, const RGBBACKGROUNDCALIBRATIONMETHOD, const int, const bool);

// -----------------------------
// Background Calibration Common 
// -----------------------------

template <int Mult, typename T>
BackgroundCalibrationCommon<Mult, T>::BackgroundCalibrationCommon(const Mode m, const RgbMethod rgbme) : mode{ m }, rgbMethod{ rgbme }
{
//	const auto md = CAllStackingTasks::GetBackgroundCalibrationMode();
//	this->mode = md == BCM_RGB ? Mode::RGB : (md == BCM_PERCHANNEL ? Mode::PerChannel : Mode::None);
//	const auto me = CAllStackingTasks::GetRGBBackgroundCalibrationMethod();
//	this->rgbMethod = me == RBCM_MINIMUM ? RgbMethod::Minimum : (me == RBCM_MIDDLE ? RgbMethod::Median : RgbMethod::Maximum);
}

template <int Mult, typename T>
std::tuple<std::vector<int>, std::vector<int>, std::vector<int>> BackgroundCalibrationCommon<Mult, T>::calcHistogram(CMemoryBitmap const& bitmap)
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
			avxHistogram.calcHistogram(startNdx, endNdx, Multiplier);
		}
#pragma omp critical(OmpLockHistoMerge)
		{
			avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto);
		}
	} // omp parallel

	return { redHisto, greenHisto, blueHisto };
}

template <int Mult, typename T>
std::pair<double, double> BackgroundCalibrationCommon<Mult, T>::findMedianAndMax(std::span<const int> histo, const size_t halfNumberOfPixels)
{
	size_t index = 0;
	for (size_t nrValues = 0; nrValues < halfNumberOfPixels;)
		nrValues += histo[index++];
	const double median = static_cast<double>(index) / Multiplier;

	double maximum = 0;
	index = HistogramSize - 1;
	for (auto it = histo.rbegin(); it != histo.rend(); ++it, --index)
	{
		if (*it != 0)
		{
			maximum = static_cast<double>(index);
			break;
		}
	}

	return { median, maximum };
}

template <int Mult, typename T>
void BackgroundCalibrationCommon<Mult, T>::calculateReferenceParameters(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax)
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

		this->referenceBackgroundRed = std::min(redMax, targetBackground);
		this->referenceBackgroundGreen = std::min(greenMax, targetBackground);
		this->referenceBackgroundBlue = std::min(blueMax, targetBackground);
	}
	else // per channel or even None
	{
		this->referenceBackgroundRed = redMedian;
		this->referenceBackgroundGreen = greenMedian;
		this->referenceBackgroundBlue = blueMedian;
	}
	ZTRACE_RUNTIME(std::format("Reference Background: red = {:.2f}, green = {:.2f}, blue = {:.2f}",
		referenceBackgroundRed / 256.0, referenceBackgroundGreen / 256.0, referenceBackgroundBlue / 256.0)
	);
}

// Initialize the calibration model (offset, linear, ...).
// Returns the median value of the red channel.
template <int Mult, typename T>
double BackgroundCalibrationCommon<Mult, T>::calculateModelParameters(CMemoryBitmap const& bitmap, const bool calcReference, const char8_t* pFileName)
{
	if (this->mode == Mode::None)
		return 0;

	const size_t halfNumberOfPixels = static_cast<size_t>(bitmap.Width()) * static_cast<size_t>(bitmap.Height()) / 2;

	const auto [redHisto, greenHisto, blueHisto] = calcHistogram(bitmap);
	const auto [redMedian, redMaximum] = findMedianAndMax(redHisto, halfNumberOfPixels);
	const auto [greenMedian, greenMaximum] = findMedianAndMax(greenHisto, halfNumberOfPixels);
	const auto [blueMedian, blueMaximum] = findMedianAndMax(blueHisto, halfNumberOfPixels);

	ZTRACE_RUNTIME(std::format("Background Calibration median parameters: red = {:.2f}, green = {:.2f}, blue = {:.2f}", redMedian / 256.0, greenMedian / 256.0, blueMedian / 256.0));

	if (calcReference)
	{
		const auto calibratorName = std::visit([](auto const& variantRef) {
			using ModelType = std::remove_cvref_t<decltype(variantRef.get())>;
			return std::tuple_element_t<3, ModelType>::name;
		}, this->model());

		ZTRACE_RUNTIME("Reference frame: %s | Calibrator Algorithm = %s, Mode = %s, RGB-Point = %s",
			pFileName == nullptr ? u8"-" : pFileName,
			calibratorName.data(),
			mode == Mode::PerChannel ? "Per Channel" : (mode == Mode::RGB ? "RGB" : "None"),
			mode == Mode::RGB ? (rgbMethod == RgbMethod::Minimum ? "Min" : (rgbMethod == RgbMethod::Median ? "Median" : "Max")) : "-"
		);

		calculateReferenceParameters(redMedian, redMaximum, greenMedian, greenMaximum, blueMedian, blueMaximum);
	}

	initializeModel(redMedian, redMaximum, greenMedian, greenMaximum, blueMedian, blueMaximum);

	return redMedian;
}

// ------------------------------
// Offset Calibrator
// ------------------------------

template <int Mult, typename T>
BackgroundCalibrationOffset<Mult, T>::BackgroundCalibrationOffset(const Mode m, const RgbMethod rgbme) : BackgroundCalibrationCommon<Mult, T>{ m, rgbme } {}


template <int Mult, typename T>
void BackgroundCalibrationOffset<Mult, T>::initializeModel(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax)
{
	constexpr auto Initialize = [](OffsetParams& model, double x0, double x1, double x2, double, double y1, double)
	{
		model.offset = y1 - x1;
		const auto [mn, mx] = std::ranges::minmax({ x0, x1, x2 });
		model.minValue = mn;
		model.maxValue = mx;
		if (mn != 0)
			throw "Offset calibrator should have a minimum value of zero";
	};

	Initialize(std::get<0>(modelParams), 0, redMedian, redMax, 0, this->referenceBackgroundRed, redMax);
	Initialize(std::get<1>(modelParams), 0, greenMedian, greenMax, 0, this->referenceBackgroundGreen, greenMax);
	Initialize(std::get<2>(modelParams), 0, blueMedian, blueMax, 0, this->referenceBackgroundBlue, blueMax);
}

template <int Mult, typename T>
std::tuple<double, double, double> BackgroundCalibrationOffset<Mult, T>::calibratePixel(const double r, const double g, const double b) const
{
	constexpr auto Calibrate = [](const OffsetParams& model, const double x)
	{
		// std::clamp(x + model.offset, model.minValue, model.maxValue);
		return std::abs(std::min(x + model.offset, model.maxValue));
	};

	return this->mode == Mode::None
		? std::make_tuple(r, g, b)
		: std::make_tuple(Calibrate(std::get<0>(modelParams), r), Calibrate(std::get<1>(modelParams), g), Calibrate(std::get<2>(modelParams), b));
}

// ------------------------------
// Linear Calibrator
// ------------------------------

template <int Mult, typename T>
BackgroundCalibrationLinear<Mult, T>::BackgroundCalibrationLinear(const Mode m, const RgbMethod rgbme) : BackgroundCalibrationCommon<Mult, T>{ m, rgbme } {}

template <int Mult, typename T>
void BackgroundCalibrationLinear<Mult, T>::initializeModel(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax)
{
	constexpr auto Initialize = [](LinearParams& model, double x0, double x1, double x2, double y0, double y1, double y2)
	{
		model.median = x1;
		model.a0 = (x0 < x1) ? (y0 - y1) / (x0 - x1) : 0.0;
		model.a1 = (x1 < x2) ? (y1 - y2) / (x1 - x2) : 0.0;
		model.b0 = y0 - model.a0 * x0;
		model.b1 = y1 - model.a1 * x1;
	};

	Initialize(std::get<0>(modelParams), 0, redMedian, redMax, 0, this->referenceBackgroundRed, redMax);
	Initialize(std::get<1>(modelParams), 0, greenMedian, greenMax, 0, this->referenceBackgroundGreen, greenMax);
	Initialize(std::get<2>(modelParams), 0, blueMedian, blueMax, 0, this->referenceBackgroundBlue, blueMax);
}

template <int Mult, typename T>
std::tuple<double, double, double> BackgroundCalibrationLinear<Mult, T>::calibratePixel(const double r, const double g, const double b) const
{
	constexpr auto Calibrate = [](const LinearParams& model, const double x)
	{
		return (x < model.median) ? (model.a0 * x + model.b0) : (model.a1 * x + model.b1);
	};

	return this->mode == Mode::None
		? std::make_tuple(r, g, b)
		: std::make_tuple(Calibrate(std::get<0>(modelParams), r), Calibrate(std::get<1>(modelParams), g), Calibrate(std::get<2>(modelParams), b));
}

// ---------------------------------------------------------
// Rational Calibrator
// ---------------------------------------------------------

template <int Mult, typename T>
BackgroundCalibrationRational<Mult, T>::BackgroundCalibrationRational(const Mode m, const RgbMethod rgbme) : BackgroundCalibrationCommon<Mult, T>{ m, rgbme } {}


template <int Mult, typename T>
template <size_t Ndx>
void BackgroundCalibrationRational<Mult, T>::resetModel(const double x0, const double x1, const double x2, const double y0, const double y1, const double y2)
{
	constexpr auto Initialize = [](RationalParams& model, double x0, double x1, double x2, double y0, double y1, double y2)
	{
		const double t1 = ((x0 * y0 - x1 * y1) * (y0 - y2) - (x0 * y0 - x2 * y2) * (y0 - y1));
		const double t2 = ((x0 - x1) * (y0 - y2) - (x0 - x2) * (y0 - y1));
		const double t3 = (y0 - y1);

		model.b = t1 != 0 ? t2 / t1 : 0.0;
		model.c = t3 != 0 ? ((x0 - x1) - model.b * (x0 * y0 - x1 * y1)) / t3 : 0.0;
		model.a = (model.b * x0 + model.c) * y0 - x0;
		const auto [mn, mx] = std::ranges::minmax({ y0, y1, y2 });
		model.minValue = mn;
		model.maxValue = mx;
	};

	Initialize(std::get<Ndx>(modelParams), x0, x1, x2, y0, y1, y2);
}

template <int Mult, typename T>
void BackgroundCalibrationRational<Mult, T>::initializeModel(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax)
{
	this->resetModel<0>(0, redMedian, redMax, 0, this->referenceBackgroundRed, redMax);
	this->resetModel<1>(0, greenMedian, greenMax, 0, this->referenceBackgroundGreen, greenMax);
	this->resetModel<2>(0, blueMedian, blueMax, 0, this->referenceBackgroundBlue, blueMax);
}

template <int Mult, typename T>
std::tuple<double, double, double> BackgroundCalibrationRational<Mult, T>::calibratePixel(const double r, const double g, const double b) const
{
	constexpr auto Calibrate = [](const RationalParams& model, const double x)
	{
		return (model.b != 0 || model.c != 0)
			? std::clamp((x + model.a) / (model.b * x + model.c), model.minValue, model.maxValue)
			: std::clamp((x + model.a), model.minValue, model.maxValue);
	};

	return this->mode == Mode::None
		? std::make_tuple(r, g, b)
		: std::make_tuple(Calibrate(std::get<0>(modelParams), r), Calibrate(std::get<1>(modelParams), g), Calibrate(std::get<2>(modelParams), b));
}

// Explicit template instantiation for the types we need.
// ------------------------------------------------------
template void BackgroundCalibrationRational<1, double>::resetModel<0ul>(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax);
template BackgroundCalibrationRational<1, double>::BackgroundCalibrationRational(const Mode, const RgbMethod);


// ******************************************************************************************************************************
// ******************************************************************************************************************************
// ******************************************************************************************************************************
// ******************************************************************************************************************************


using RT = std::tuple<double, double, double>;

struct ReferenceBackground
{
	double red{ 0 };
	double green{ 0 };
	double blue{ 0 };
};

struct NonePara {
	void initialize(const double, const double, const double, const double, const double, const double) {}
};
struct NonModel {
	NonePara redParams;
	NonePara greenParams;
	NonePara blueParams;
	static inline constexpr std::string_view name = "None";

	RT calibrate(const double r, const double g, const double b) const
	{
		return { r, g, b };
	}
};
// ------------------------

struct OffPara : public OffsetParams
{
	void initialize(const double x0, const double x1, const double x2, const double, const double y1, const double)
	{
		offset = y1 - x1;
		const auto [mn, mx] = std::ranges::minmax({ x0, x1, x2 });
		minValue = mn;
		maxValue = mx;
	}
};

struct OffModel
{
	OffPara redParams;
	OffPara greenParams;
	OffPara blueParams;
	static inline constexpr std::string_view name = "Offset";

	RT calibrate(const double r, const double g, const double b) const
	{
		constexpr auto Calibrate = [](OffPara const& params, const double x) -> double
		{
			// std::clamp(x + model.offset, model.minValue, model.maxValue);
			return std::abs(std::min(x + params.offset, params.maxValue));
		};

		return false // this->mode == Mode::None
			? std::make_tuple(r, g, b)
			: std::make_tuple(Calibrate(redParams, r), Calibrate(greenParams, g), Calibrate(blueParams, b));
	}
};
// ------------------------

struct LinPara : public LinearParams
{
	void initialize(const double x0, const double x1, const double x2, const double y0, const double y1, const double y2)
	{
		median = x1;
		a0 = (x0 < x1) ? (y0 - y1) / (x0 - x1) : 0.0;
		a1 = (x1 < x2) ? (y1 - y2) / (x1 - x2) : 0.0;
		b0 = y0 - a0 * x0;
		b1 = y1 - a1 * x1;
	}
};

struct LinModel
{
	LinPara redParams;
	LinPara greenParams;
	LinPara blueParams;
	static inline constexpr std::string_view name = "Linear";

	RT calibrate(const double r, const double g, const double b) const
	{
		constexpr auto Calibrate = [](LinPara const& params, const double x) -> double
		{
			return (x < params.median) ? (params.a0 * x + params.b0) : (params.a1 * x + params.b1);
		};

		return false // this->mode == Mode::None
			? std::make_tuple(r, g, b)
			: std::make_tuple(Calibrate(redParams, r), Calibrate(greenParams, g), Calibrate(blueParams, b));
	}
};
// ------------------------

struct RatioPara : public RationalParams
{
	void initialize(const double x0, const double x1, const double x2, const double y0, const double y1, const double y2)
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
};

struct RatioModel
{
	RatioPara redParams;
	RatioPara greenParams;
	RatioPara blueParams;
	static inline constexpr std::string_view name = "Rational";

	RT calibrate(const double r, const double g, const double b) const
	{
		constexpr auto Calibrate = [](RatioPara const& params, const double x) -> double
		{
			return (params.b != 0 || params.c != 0)
				? std::clamp((x + params.a) / (params.b * x + params.c), params.minValue, params.maxValue)
				: std::clamp((x + params.a), params.minValue, params.maxValue);
		};

		return false // this->mode == Mode::None
			? std::make_tuple(r, g, b)
			: std::make_tuple(Calibrate(redParams, r), Calibrate(greenParams, g), Calibrate(blueParams, b));
	}
};
// ------------------------

template<typename T>
concept Calibrator = requires(T t, double a, double b, double c, double d, double e, double f) {
	{ t.calibrate(a, b, c) } -> std::convertible_to<RT>;
	{ t.redParams.initialize(a, b, c, d, e, f) } -> std::same_as<void>;
	{ t.greenParams.initialize(a, b, c, d, e, f) } -> std::same_as<void>;
	{ t.blueParams.initialize(a, b, c, d, e, f) } -> std::same_as<void>;
	t.name;
};

template <Calibrator... Cals>
struct BackgrCalib
{
	static inline constexpr size_t HistogramSize = std::numeric_limits<std::uint16_t>::max() + size_t{ 1 };
//	static inline constexpr int Mult = 1;
//	static inline constexpr double Multiplier = static_cast<double>(Mult);

	std::variant<Cals...> calibrator{};

	ReferenceBackground referenceBackground{};
	const double multiplier{ 1.0 };

	const Mode mode{ Mode::None };
	const RgbMethod rgbMethod{ RgbMethod::Median };


	template <Calibrator C>
	constexpr BackgrCalib(C c, const double multr, const Mode md, const RgbMethod rgbm) :
		calibrator{ std::move(c) },
		multiplier{ multr },
		mode { md },
		rgbMethod{ rgbm }
	{}

	~BackgrCalib() = default;
	BackgrCalib(BackgrCalib const&) = delete;
	BackgrCalib(BackgrCalib&&) = default;


	RT calibratePixel(const double r, const double g, const double b) const
	{
		return std::visit([r, g, b](const auto& c)
			{
				return c.calibrate(r, g, b);
			},
			calibrator
		);
	}

	constexpr auto& getCalibratorVariant() const {
		return calibrator;
	}
private:
	std::tuple<std::vector<int>, std::vector<int>, std::vector<int>> calcHistogram(CMemoryBitmap const& bitmap)
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

	std::pair<double, double> findMedianAndMax(std::span<const int> histo, const size_t halfNumberOfPixels)
	{
		size_t index = 0;
		for (size_t nrValues = 0; nrValues < halfNumberOfPixels;)
			nrValues += histo[index++];
		const double median = static_cast<double>(index) / multiplier;

		double maximum = 0;
		index = HistogramSize - 1;
		for (auto it = histo.rbegin(); it != histo.rend(); ++it, --index)
		{
			if (*it != 0)
			{
				maximum = static_cast<double>(index);
				break;
			}
		}

		return { median, maximum };
	}

	void calculateReferenceParameters(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax)
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
		else // per channel or even None
		{
			this->referenceBackground.red = redMedian;
			this->referenceBackground.green = greenMedian;
			this->referenceBackground.blue = blueMedian;
		}
		ZTRACE_RUNTIME(std::format("Reference Background: red = {:.2f}, green = {:.2f}, blue = {:.2f}",
			referenceBackground.red / 256.0, referenceBackground.green / 256.0, referenceBackground.blue / 256.0)
		);
	}
public:
	double calculateModelParameters(CMemoryBitmap const& bitmap, const bool calcReference, const char8_t* pFileName)
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
				mode == Mode::PerChannel ? "Per Channel" : (mode == Mode::RGB ? "RGB" : "None"),
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
};

using BackgroundCali = BackgrCalib<NonModel, OffModel, LinModel, RatioModel>;

BackgroundCali makeBackgrCal(const BACKGROUNDCALIBRATIONINTERPOLATION interpolation, const BACKGROUNDCALIBRATIONMODE bcmd, const RGBBACKGROUNDCALIBRATIONMETHOD rgbme)
{
	const Mode mode = bcmd == BCM_RGB ? Mode::RGB : (bcmd == BCM_PERCHANNEL ? Mode::PerChannel : Mode::None);
	const RgbMethod rgbMethod = rgbme == RBCM_MINIMUM ? RgbMethod::Minimum : (rgbme == RBCM_MIDDLE ? RgbMethod::Median : RgbMethod::Maximum);

	if (mode == Mode::None)
		return { NonModel{}, 1.0, mode, rgbMethod };
	switch (interpolation)
	{
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_LINEAR: return { LinModel{}, 1.0, mode, rgbMethod };
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_RATIONAL: return { RatioModel{}, 1.0, mode, rgbMethod };
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_OFFSET: return { OffModel{}, 1.0, mode, rgbMethod };
	default: return { NonModel{}, 1.0, mode, rgbMethod };
	}
	return { NonModel{}, 1.0, mode, rgbMethod };
}

//BackgroundCali backNo{ NonModel{}, Mode::None, RgbMethod::Median };
auto backNo = makeBackgrCal(BACKGROUNDCALIBRATIONINTERPOLATION::BCI_LINEAR, BACKGROUNDCALIBRATIONMODE::BCM_NONE, RGBBACKGROUNDCALIBRATIONMETHOD::RBCM_MIDDLE);
BackgroundCali backCO{ OffModel{}, 1.0, Mode::PerChannel, RgbMethod::Median };
BackgroundCali backCL{ LinModel{}, 1.0, Mode::RGB, RgbMethod::Minimum };
BackgroundCali backCR{ RatioModel{}, 256.0, Mode::RGB, RgbMethod::Maximum };
std::array<BackgroundCali, 4> backArr = { std::move(backNo), std::move(backCO), std::move(backCL), std::move(backCR) };

void f(CMemoryBitmap const& b)
{
	backArr[0].calculateModelParameters(b, false, nullptr);
	backArr[1].calculateModelParameters(b, true, nullptr);
	backArr[2].calculateModelParameters(b, true, nullptr);
	backArr[1].calibratePixel(3, 4, 5);

	char const *const nnn = std::visit([](auto const& d)
		{
			using D = std::decay_t<decltype(d)>;
			if constexpr (std::same_as<D, NonModel>)
				return "1";
			else if constexpr (std::same_as<D, OffModel>)
				return d.redParams.offset > 0 ? "2+" : "2-";
			else if constexpr (std::same_as<D, LinModel>)
				return d.greenParams.a1 < 10 ? "3A" : "3b";
			else if constexpr (std::same_as<D, RatioModel>)
				return d.blueParams.c < 100 ? "4c" : "4x";
			else
				return "---";
		},
		backArr[3].getCalibratorVariant()
	);

	printf("%s", nnn);
}


//class BackCal
//{
//public:
//	using RT = std::tuple<double, double, double>;
//	using FuncSig = std::function<double(double)>;
//
//	FuncSig calibrator;
//
//	BackCal(std::invocable<double> auto&& f) : calibrator{ std::forward<decltype(f)>(f)} {}
//	BackCal(const BackCal&) = delete;
//	~BackCal() = default;
//
//	RT calibratePixel(const double r, const double g, const double b) {
//		return { calibrator(r), calibrator(g), calibrator(b) };
//	}
//};
//
//struct LinCal
//{
//	double median{ 0 };
//	double a0{ 0 };
//	double a1{ 0 };
//	double b0{ 0 };
//	double b1{ 0 };
//
//	double operator()(const double x) const noexcept
//	{
//		return (x < median) ? (a0 * x + b0) : (a1 * x + b1);
//	}
//};
//
//BackCal glob_bc(LinCal{});
