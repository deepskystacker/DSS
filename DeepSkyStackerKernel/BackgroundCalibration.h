#pragma once
#include "DSSCommon.h"
#include "RationalInterpolation.h"
#include "LinearInterpolationh.h"

class CMemoryBitmap;
namespace DSS { class OldProgressBase; }
using namespace DSS;

class CBackgroundCalibration
{
public:
	bool				m_bInitOk;
	double				m_fTgtRedBk{ 0.0 },
						m_fTgtGreenBk{ 0.0 },
						m_fTgtBlueBk{ 0.0 };
	double				m_fSrcRedBk{ 0.0 },
						m_fSrcGreenBk{ 0.0 },
						m_fSrcBlueBk{ 0.0 };

	double				m_fSrcRedMax{ 0.0 },
						m_fSrcGreenMax{ 0.0 },
						m_fSrcBlueMax{ 0.0 };

	BACKGROUNDCALIBRATIONMODE			m_BackgroundCalibrationMode;
	BACKGROUNDCALIBRATIONINTERPOLATION	m_BackgroundInterpolation;
	RGBBACKGROUNDCALIBRATIONMETHOD		m_RGBBackgroundMethod;

	double				m_fMultiplier;

	CRationalInterpolation	m_riRed;
	CRationalInterpolation	m_riGreen;
	CRationalInterpolation	m_riBlue;

	CLinearInterpolation	m_liRed;
	CLinearInterpolation	m_liGreen;
	CLinearInterpolation	m_liBlue;

public:
	CBackgroundCalibration();

	CBackgroundCalibration(const CBackgroundCalibration& bc) = default;

	virtual ~CBackgroundCalibration() = default;

	CBackgroundCalibration& operator=(const CBackgroundCalibration& bc) = default;

	void SetMultiplier(double fMultiplier)
	{
		m_fMultiplier = fMultiplier;
	}

	void SetMode(BACKGROUNDCALIBRATIONMODE BackgroundCalibrationMode,
				BACKGROUNDCALIBRATIONINTERPOLATION BackgroundInterpolation,
				RGBBACKGROUNDCALIBRATIONMETHOD RGBBackgroundMethod)
	{
		m_BackgroundCalibrationMode	= BackgroundCalibrationMode;
		m_BackgroundInterpolation	= BackgroundInterpolation;
		m_RGBBackgroundMethod		= RGBBackgroundMethod;
	}
	void ComputeBackgroundCalibration(const CMemoryBitmap* pBitmap, const char8_t* pFileName, bool bFirst, OldProgressBase* pProgress);

	template <class T>
	void ApplyCalibration(T& fRed, T& fGreen, T& fBlue) const
	{
		static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>);

		if (m_BackgroundInterpolation == BCI_RATIONAL)
		{
			fRed = m_riRed.Interpolate(fRed);
			fGreen = m_riGreen.Interpolate(fGreen);
			fBlue = m_riBlue.Interpolate(fBlue);
		}
		else
		{
			fRed = m_liRed.Interpolate(fRed);
			fGreen = m_liGreen.Interpolate(fGreen);
			fBlue = m_liBlue.Interpolate(fBlue);
		}
	}

private:
	static constexpr size_t HistogramSize() { return std::numeric_limits<std::uint16_t>::max() + size_t{ 1 }; }
	void ompCalcHistogram(const CMemoryBitmap* pBitmap, OldProgressBase* pProgress, std::vector<int>& redHisto, std::vector<int>& greenHisto, std::vector<int>& blueHisto) const;
};

// ****************************************************************************************************
// ****************************************************************************************************

#include <ranges>
#include <vector>
#include <memory>
#include <tuple>
#include <any>


struct NoneParams {
};
struct OffsetParams {
	double offset{ 0 }; // Must be added to the (red/green/blue) pixel values.
	double minValue{ 0 };
	double maxValue{ 0 };
};
struct LinearParams {
	double median{ 0 };
	double a0{ 0 };
	double a1{ 0 };
	double b0{ 0 };
	double b1{ 0 };
};
struct RationalParams {
	double a{ 0 };
	double b{ 0 };
	double c{ 1 };
	double minValue{ 0 };
	double maxValue{ 0 };
};
using NoneModel = std::tuple<NoneParams, NoneParams, NoneParams>;
using OffsetModel = std::tuple<OffsetParams, OffsetParams, OffsetParams>;
using LinearModel = std::tuple<LinearParams, LinearParams, LinearParams>;
using RationalModel = std::tuple<RationalParams, RationalParams, RationalParams>;

using ModelRef = std::variant<
	std::reference_wrapper<const NoneModel>,
	std::reference_wrapper<const OffsetModel>,
	std::reference_wrapper<const LinearModel>,
	std::reference_wrapper<const RationalModel>
>;


// ---------------------------------------------------------
// Interface for Background Calibrators
// ---------------------------------------------------------

class BackgroundCalibrationInterface
{
public:
	enum class Interpolation { Linear = 0, Rational, Offset };
	enum class Mode { None = 0, PerChannel, RGB };

	template <int Mult>
		requires (Mult == 1 || Mult == 256)
	static std::shared_ptr<BackgroundCalibrationInterface> makeBackgroundCalibrator(const BACKGROUNDCALIBRATIONMODE mode, const int bitsPerSample, const bool integral);

	virtual ~BackgroundCalibrationInterface() = default;

	virtual std::tuple<double, double, double> calibratePixel(const double r, const double g, const double b) const = 0;
	virtual double calculateModelParameters(CMemoryBitmap const&, const bool calcReference) = 0;
	virtual ModelRef model() const = 0;
protected:
	virtual void initializeModel(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax) = 0;
};

// ---------------------------------------------------------
// Everything that is common to all Background Calibrators
// ---------------------------------------------------------

template <int Mult, typename T>
class BackgroundCalibrationCommon : public BackgroundCalibrationInterface
{
protected:
	using Mode = BackgroundCalibrationInterface::Mode;
	enum class RgbMethod { Minimum = 0, Median, Maximum };

	static inline constexpr size_t HistogramSize = std::numeric_limits<std::uint16_t>::max() + size_t{ 1 };
	static inline constexpr double Multiplier = static_cast<double>(Mult);

	double referenceBackgroundRed{ 0 };
	double referenceBackgroundGreen{ 0 };
	double referenceBackgroundBlue{ 0 };
	Mode mode{ Mode::None };
	RgbMethod rgbMethod{ RgbMethod::Median };
protected:
	explicit BackgroundCalibrationCommon(const Mode m);
	virtual ~BackgroundCalibrationCommon() = default;
	std::tuple<std::vector<int>, std::vector<int>, std::vector<int>> calcHistogram(CMemoryBitmap const&);
	std::pair<double, double> findMedianAndMax(std::span<const int> histo, const size_t halfNumberOfPixels);
	void calculateReferenceParameters(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax);
public:
	virtual double calculateModelParameters(CMemoryBitmap const&, const bool calcReference) override;
};

// ---------------------------------------------------------
// The 'None' Calibrator
// ---------------------------------------------------------

template <int Mult, typename T>
class BackgroundCalibrationNone : public BackgroundCalibrationCommon<Mult, T>
{
	using Mode = BackgroundCalibrationInterface::Mode;
private:
	NoneModel modelParams{};
public:
	explicit BackgroundCalibrationNone() : BackgroundCalibrationCommon<Mult, T>{ Mode::None } {}
	virtual ~BackgroundCalibrationNone() = default;
	virtual void initializeModel(const double, const double, const double, const double, const double, const double) override {}
	virtual std::tuple<double, double, double> calibratePixel(const double r, const double g, const double b) const override { return { r, g, b }; }
	virtual ModelRef model() const override { return std::cref(modelParams); }
};

// ---------------------------------------------------------
// Offset only Calibrator
// ---------------------------------------------------------

template <int Mult, typename T>
class BackgroundCalibrationOffset : public BackgroundCalibrationCommon<Mult, T>
{
	using Mode = BackgroundCalibrationInterface::Mode;
private:
	OffsetModel modelParams{};
public:
	explicit BackgroundCalibrationOffset(const Mode m);
	virtual ~BackgroundCalibrationOffset() = default;
	virtual void initializeModel(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax) override;
	virtual std::tuple<double, double, double> calibratePixel(const double r, const double g, const double b) const override;
	virtual ModelRef model() const override { return std::cref(modelParams); }
};

// ---------------------------------------------------------
// Linear Calibrator
// ---------------------------------------------------------

template <int Mult, typename T>
class BackgroundCalibrationLinear : public BackgroundCalibrationCommon<Mult, T>
{
	using Mode = BackgroundCalibrationInterface::Mode;
private:
	LinearModel modelParams{};
public:
	explicit BackgroundCalibrationLinear(const Mode m);
	virtual ~BackgroundCalibrationLinear() = default;
	virtual void initializeModel(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax) override;
	virtual std::tuple<double, double, double> calibratePixel(const double r, const double g, const double b) const override;
	virtual ModelRef model() const override { return std::cref(modelParams); }
};

// ---------------------------------------------------------
// Rational Calibrator
// ---------------------------------------------------------

template <int Mult, typename T>
class BackgroundCalibrationRational : public BackgroundCalibrationCommon<Mult, T>
{
	using Mode = BackgroundCalibrationInterface::Mode;
private:
	RationalModel modelParams{};
public:
	explicit BackgroundCalibrationRational(const Mode m);
	virtual ~BackgroundCalibrationRational() = default;

	template <size_t Ndx>
	void resetModel(const double x0, const double x1, const double x2, const double y0, const double y1, const double y2);
	
	virtual void initializeModel(const double redMedian, const double redMax, const double greenMedian, const double greenMax, const double blueMedian, const double blueMax) override;
	virtual std::tuple<double, double, double> calibratePixel(const double r, const double g, const double b) const override;
	virtual ModelRef model() const override { return std::cref(modelParams); }
};
