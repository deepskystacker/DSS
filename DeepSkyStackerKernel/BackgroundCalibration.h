#pragma once
#include <ranges>
#include <vector>
#include <memory>
#include <tuple>
#include <string_view>
#include "DSSCommon.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------
// Instructions for implementing a new Background Calibration algorithm:
//
// 1. Create a new class for the needed parameters, and the 'initialize' member function (see below).
//
// 2. Create a new class for the model with redParams, greenParams, blueParams, static inline constexpr std::string_view name, and the 'calibrate' member function.
//
// 3. Add your new model to: 
// using BackgroundCalibrator = BackgroundCalibratorVariant<NoneModel, OffsetModel, LinearModel, RationalModel, YOUR_NEW_MODEL>;
//
// 4. Modify the factory function makeBackgroundCalibrator(), add how your new calibrator is created.
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------

class CMemoryBitmap;

enum class Interpolation { None, Linear, Rational, Offset };
enum class Mode { PerChannel, RGB };
enum class RgbMethod { Minimum, Median, Maximum };

using BcRT = std::tuple<double, double, double>;


struct ReferenceBackground
{
	double red{ 0 };
	double green{ 0 };
	double blue{ 0 };
};

struct NoneParams
{
	inline void initialize(const double, const double, const double, const double, const double, const double) {}
};
struct NoneModel
{
	NoneParams redParams{};
	NoneParams greenParams{};
	NoneParams blueParams{};
	static inline constexpr std::string_view name = "None";

	BcRT calibrate(const double r, const double g, const double b) const
	{
		return { r, g, b };
	}
};

struct OffsetParams
{
	double offset{ 0 }; // Must be added to the (red/green/blue) pixel values.
	double minValue{ 0 };
	double maxValue{ 0 };

	void initialize(const double x0, const double x1, const double x2, const double, const double y1, const double);
};

struct OffsetModel
{
	OffsetParams redParams{};
	OffsetParams greenParams{};
	OffsetParams blueParams{};
	static inline constexpr std::string_view name = "Offset";

	BcRT calibrate(const double r, const double g, const double b) const;
};

struct LinearParams
{
	double median{ 0 };
	double a0{ 0 };
	double a1{ 0 };
	double b0{ 0 };
	double b1{ 0 };

	void initialize(const double x0, const double x1, const double x2, const double y0, const double y1, const double y2);
};

struct LinearModel
{
	LinearParams redParams{};
	LinearParams greenParams{};
	LinearParams blueParams{};
	static inline constexpr std::string_view name = "Linear";

	BcRT calibrate(const double r, const double g, const double b) const;
};

struct RationalParams
{
	double a{ 0 };
	double b{ 0 };
	double c{ 1 };
	double minValue{ 0 };
	double maxValue{ 0 };

	void initialize(const double x0, const double x1, const double x2, const double y0, const double y1, const double y2);
};

struct RationalModel
{
	RationalParams redParams{};
	RationalParams greenParams{};
	RationalParams blueParams{};
	static inline constexpr std::string_view name = "Rational";

	BcRT calibrate(const double r, const double g, const double b) const;
};


template<typename T>
concept IsCalibrator = requires(T t, double a, double b, double c, double d, double e, double f)
{
	{ t.calibrate(a, b, c) } -> std::convertible_to<BcRT>;
	{ t.redParams.initialize(a, b, c, d, e, f) } -> std::same_as<void>;
	{ t.greenParams.initialize(a, b, c, d, e, f) } -> std::same_as<void>;
	{ t.blueParams.initialize(a, b, c, d, e, f) } -> std::same_as<void>;
	t.name;
};

template <IsCalibrator... Cals>
class BackgroundCalibratorVariant
{
private:
	static inline constexpr size_t HistogramSize = std::numeric_limits<std::uint16_t>::max() + size_t{ 1 };
	using TVariant = std::variant<Cals...>;

	TVariant calibrator{};

	ReferenceBackground referenceBackground{};
	const double multiplier{ 1.0 };
	const Mode mode{ Mode::PerChannel };
	const RgbMethod rgbMethod{ RgbMethod::Median };

public:
	template <IsCalibrator C>
	constexpr BackgroundCalibratorVariant(C c, const double multr, const Mode md, const RgbMethod rgbm) :
		calibrator{ std::move(c) },
		multiplier{ multr },
		mode{ md },
		rgbMethod{ rgbm }
	{}

	~BackgroundCalibratorVariant() = default;
	BackgroundCalibratorVariant(BackgroundCalibratorVariant const&) = delete;
	BackgroundCalibratorVariant(BackgroundCalibratorVariant&&) = default;


	inline BcRT calibratePixel(const double r, const double g, const double b) const
	{
		return std::visit([r, g, b](auto const& c)
			{
				return c.calibrate(r, g, b);
			},
			this->calibrator
		);
	}

	TVariant& getCalibratorVariant();
	TVariant const& getCalibratorVariant() const;

	double calculateModelParameters(CMemoryBitmap const& bitmap, const bool calcReference, const char8_t* pFileName);

private:
	std::tuple<std::vector<int>, std::vector<int>, std::vector<int>> calcHistogram(CMemoryBitmap const& bitmap);
	std::pair<double, double> findMedianAndMax(std::span<const int> const histo, const size_t halfNumberOfPixels);
	void calculateReferenceParameters(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax);
};

using BackgroundCalibrator = BackgroundCalibratorVariant<NoneModel, OffsetModel, LinearModel, RationalModel>;

BackgroundCalibrator makeBackgroundCalibrator(
	const BACKGROUNDCALIBRATIONINTERPOLATION interpolation, const BACKGROUNDCALIBRATIONMODE bcmd, const RGBBACKGROUNDCALIBRATIONMETHOD rgbme, const double multr);


extern template class BackgroundCalibratorVariant<NoneModel, OffsetModel, LinearModel, RationalModel>;
