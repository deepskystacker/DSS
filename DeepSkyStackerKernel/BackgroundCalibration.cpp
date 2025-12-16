#include "pch.h"
//#include "resource.h"
#include "BitmapBase.h"
#include "BackgroundCalibration.h"
#include "DSSProgress.h"
#include "StackingTasks.h"
#include "avx_histogram.h"
#include "Multitask.h"
#include "MemoryBitmap.h"

CBackgroundCalibration::CBackgroundCalibration() :
	m_bInitOk{ false },
	m_fMultiplier{ 1.0 },
	m_BackgroundCalibrationMode{ CAllStackingTasks::GetBackgroundCalibrationMode() },
	m_BackgroundInterpolation{ CAllStackingTasks::GetBackgroundCalibrationInterpolation() },
	m_RGBBackgroundMethod{ CAllStackingTasks::GetRGBBackgroundCalibrationMethod() }
{}

void CBackgroundCalibration::ompCalcHistogram(const CMemoryBitmap* pBitmap, OldProgressBase* pProgress, std::vector<int>& redHisto, std::vector<int>& greenHisto, std::vector<int>& blueHisto) const
{
	AvxHistogram avxHistogram(*pBitmap);
//	std::vector<int> redLocalHist(avxHistogram.isAvxReady() ? 0 : HistogramSize()); // Only allocate mem if AVX will not be used (see below).
//	std::vector<int> greenLocalHist(avxHistogram.isAvxReady() ? 0 : HistogramSize());
//	std::vector<int> blueLocalHist(avxHistogram.isAvxReady() ? 0 : HistogramSize());
	const int height = pBitmap->Height();
	const auto nrProcessors = Multitask::GetNrProcessors();

#pragma omp parallel default(shared) shared(redHisto, greenHisto, blueHisto) firstprivate(avxHistogram/*, redLocalHist, greenLocalHist, blueLocalHist*/) if(nrProcessors > 1)
	{
		constexpr int Bulksize = 50;
#pragma omp for
		for (int startNdx = 0; startNdx < height; startNdx += Bulksize)
		{
			const int endNdx = std::min(startNdx + Bulksize, height);
			if (avxHistogram.calcHistogram(startNdx, endNdx, m_fMultiplier) != 0)
			{
				//constexpr double Maxvalue = double{std::numeric_limits<std::uint16_t>::max()};
				//const double fMultiplier = m_fMultiplier * 256.0;

				//if (redLocalHist.empty()) {
				//	redLocalHist.resize(HistogramSize(), 0);
				//	greenLocalHist.resize(HistogramSize(), 0);
				//	blueLocalHist.resize(HistogramSize(), 0);
				//}

				if (pProgress != nullptr && omp_get_thread_num() == 0) // Only master thread
					pProgress->Progress2(startNdx);

				//for (int j = startNdx; j < endNdx; ++j)
				//{
				//	for (int i = 0, width = pBitmap->Width(); i < width; ++i)
				//	{
				//		COLORREF16 crColor;
				//		double fRed, fGreen, fBlue;
				//		pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
				//		fRed *= fMultiplier;
				//		fGreen *= fMultiplier;
				//		fBlue *= fMultiplier;

				//		crColor.red = std::min(fRed, Maxvalue);
				//		crColor.blue = std::min(fBlue, Maxvalue);
				//		crColor.green = std::min(fGreen, Maxvalue);

				//		redLocalHist[crColor.red]++;
				//		greenLocalHist[crColor.green]++;
				//		blueLocalHist[crColor.blue]++;
				//	}
				//}
			}
		}

#pragma omp critical(OmpLockHistoMerge)
		{
			avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto);
		}
	} // omp parallel
}

void CBackgroundCalibration::ComputeBackgroundCalibration(const CMemoryBitmap* pBitmap, const char8_t* pFileName, bool bFirst, OldProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();
	m_fSrcRedMax = 0;
	m_fSrcGreenMax = 0;
	m_fSrcBlueMax = 0;
	const int height = pBitmap->Height();
	QString strText;

	if (pProgress != nullptr)
	{
		strText = QCoreApplication::translate("BackgroundCalibration", "Computing Background Calibration parameters", "IDS_COMPUTINGBACKGROUNDCALIBRATION");
		pProgress->Start2(strText, height);
	}

	std::vector<int> vRedHisto(HistogramSize());
	std::vector<int> vGreenHisto(HistogramSize());
	std::vector<int> vBlueHisto(HistogramSize());
	this->ompCalcHistogram(pBitmap, pProgress, vRedHisto, vGreenHisto, vBlueHisto);

	const auto findMax = [](const std::vector<int>& histo) -> double
	{
		size_t ndx = HistogramSize() - 1;
		for (auto it = histo.crbegin(); it != histo.crend(); ++it, --ndx)
			if (*it != 0)
				return static_cast<double>(ndx);
		return 0.0;
	};
	this->m_fSrcRedMax = findMax(vRedHisto);
	this->m_fSrcGreenMax = findMax(vGreenHisto);
	this->m_fSrcBlueMax = findMax(vBlueHisto);

	const auto findMedian = [multiplier = this->m_fMultiplier, nrTotalValues = (pBitmap->Width() * height) / 2](const std::vector<int>& histo) -> double
	{
		int nrValues = 0;
		int index = 0;
		while (nrValues < nrTotalValues)
			nrValues += histo[index++];
		return static_cast<double>(index) / multiplier;
	};
	m_fSrcRedBk = findMedian(vRedHisto);
	m_fSrcGreenBk = findMedian(vGreenHisto);
	m_fSrcBlueBk = findMedian(vBlueHisto);

	ZTRACE_RUNTIME("Background Calibration: Median Red = %.2f - Green = %.2f - Blue = %.2f", m_fSrcRedBk/256.0, m_fSrcGreenBk/256.0, m_fSrcBlueBk/256.0);

	if (bFirst)
	{
		const char* pStrMode = "None";
		const char* pStrRGB = "N/A";
		const char* pStrMethod = m_BackgroundInterpolation == BCI_LINEAR ? "Linear" : (m_BackgroundInterpolation == BCI_RATIONAL ? "Rational" : "Unknown");

		if (m_BackgroundCalibrationMode == BCM_PERCHANNEL)
		{
			m_fTgtRedBk = m_fSrcRedBk;
			m_fTgtGreenBk = m_fSrcGreenBk;
			m_fTgtBlueBk = m_fSrcBlueBk;
			pStrMode = "Per Channel";
		}
		else if (m_BackgroundCalibrationMode == BCM_RGB)
		{
			double fTgtBk;

			if (m_RGBBackgroundMethod == RBCM_MAXIMUM)
			{
				fTgtBk = std::ranges::max({ m_fSrcRedBk, m_fSrcGreenBk, m_fSrcBlueBk });
				pStrRGB = "Max";
			}
			else if (m_RGBBackgroundMethod == RBCM_MINIMUM)
			{
				fTgtBk = std::ranges::min({ m_fSrcRedBk, m_fSrcGreenBk, m_fSrcBlueBk });
				pStrRGB = "Min";
			}
			else
			{
				fTgtBk = Median(m_fSrcRedBk, m_fSrcGreenBk, m_fSrcBlueBk);
				pStrRGB = "Median";
			}

			m_fTgtRedBk = m_fSrcRedMax > fTgtBk ? fTgtBk : m_fSrcRedBk;
			m_fTgtGreenBk = m_fSrcGreenMax > fTgtBk ? fTgtBk : m_fSrcGreenBk;
			m_fTgtBlueBk = m_fSrcBlueMax > fTgtBk ? fTgtBk : m_fSrcBlueBk;

			pStrMode = "RGB";
		}
		ZTRACE_RUNTIME("Reference frame: %s | Method = %s, Mode = %s, RGB-Point = %s", pFileName == nullptr ? u8"-" : pFileName, pStrMethod, pStrMode, pStrRGB);
		ZTRACE_RUNTIME("Reference Background: Red = %.2f - Green = %.2f - Blue = %.2f", m_fTgtRedBk/256.0, m_fTgtGreenBk/256.0, m_fTgtBlueBk/256.0);
	}

	m_riRed.Initialize(0, m_fSrcRedBk, m_fSrcRedMax, 0, m_fTgtRedBk, m_fSrcRedMax);
	m_riGreen.Initialize(0, m_fSrcGreenBk, m_fSrcGreenMax, 0, m_fTgtGreenBk, m_fSrcGreenMax);
	m_riBlue.Initialize(0, m_fSrcBlueBk, m_fSrcBlueMax, 0, m_fTgtBlueBk, m_fSrcBlueMax);

	m_liRed.Initialize(0, m_fSrcRedBk, m_fSrcRedMax, 0, m_fTgtRedBk, m_fSrcRedMax);
	m_liGreen.Initialize(0, m_fSrcGreenBk, m_fSrcGreenMax, 0, m_fTgtGreenBk, m_fSrcGreenMax);
	m_liBlue.Initialize(0, m_fSrcBlueBk, m_fSrcBlueMax, 0, m_fTgtBlueBk, m_fSrcBlueMax);

	if (pProgress != nullptr)
		pProgress->End2();

	m_bInitOk = true;
}

// ****************************************************************************************************
// ****************************************************************************************************

// --------------------------
// Interface
// --------------------------

namespace {
	using BCI = BackgroundCalibrationInterface;
	using Interpolation = BCI::Interpolation;
	using Mode = BCI::Mode;

	template <int Mult, typename Type, typename... OtherTypes>
		requires (
			(std::same_as<Type, std::uint8_t> || std::same_as<Type, std::uint16_t> || std::same_as<Type, std::uint32_t> || std::same_as<Type, float> || std::same_as<Type, double>)
			&& (Mult == 1 || Mult == 256) // we only support those 2 multipliers
		)
	std::shared_ptr<BCI> createTyped(const Interpolation interpolationMethod, const Mode mode, const int bitsPerSample, const bool isIntegral)
	{
		if (bitsPerSample == sizeof(Type) * 8 && isIntegral == std::integral<Type>)
		{
			if (mode == Mode::None)
				return std::make_shared<BackgroundCalibrationNone<Mult, Type>>();
			switch (interpolationMethod)
			{
			case Interpolation::Linear: return std::make_shared<BackgroundCalibrationLinear<Mult, Type>>(mode);
			case Interpolation::Rational: return std::make_shared<BackgroundCalibrationRational<Mult, Type>>(mode);
			case Interpolation::Offset: return std::make_shared<BackgroundCalibrationOffset<Mult, Type>>(mode);
			default: return {};
			}
		}
		if constexpr (sizeof...(OtherTypes) != 0)
			return createTyped<Mult, OtherTypes...>(interpolationMethod, mode, bitsPerSample, isIntegral);
		else
			return {};
	}
}

template <int Mult>
	requires (Mult == 1 || Mult == 256)
std::shared_ptr<BackgroundCalibrationInterface> BackgroundCalibrationInterface::makeBackgroundCalibrator(const BACKGROUNDCALIBRATIONMODE bcmd, const int bitsPerSample, const bool integral)
{
	Interpolation intMethod = Interpolation::Linear;
	switch (CAllStackingTasks::GetBackgroundCalibrationInterpolation())
	{
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_LINEAR: break;
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_RATIONAL:
		intMethod = Interpolation::Rational; break;
	case BACKGROUNDCALIBRATIONINTERPOLATION::BCI_OFFSET:
		intMethod = Interpolation::Offset; break;
	default: break;
	}

	const Mode mode = bcmd == BCM_RGB ? Mode::RGB : (bcmd == BCM_PERCHANNEL ? Mode::PerChannel : Mode::None);

	return createTyped<Mult, std::uint8_t, std::uint16_t, std::uint32_t, float, double>(intMethod, mode, bitsPerSample, integral);
}

template
std::shared_ptr<BackgroundCalibrationInterface> BackgroundCalibrationInterface::makeBackgroundCalibrator<1>(const BACKGROUNDCALIBRATIONMODE bcmd, const int bitsPerSample, const bool integral);
template
std::shared_ptr<BackgroundCalibrationInterface> BackgroundCalibrationInterface::makeBackgroundCalibrator<256>(const BACKGROUNDCALIBRATIONMODE bcmd, const int bitsPerSample, const bool integral);

// -----------------------------
// Background Calibration Common 
// -----------------------------

template <int Mult, typename T>
BackgroundCalibrationCommon<Mult, T>::BackgroundCalibrationCommon(const Mode m) : mode{ m }
{
//	const auto md = CAllStackingTasks::GetBackgroundCalibrationMode();
//	this->mode = md == BCM_RGB ? Mode::RGB : (md == BCM_PERCHANNEL ? Mode::PerChannel : Mode::None);
	const auto me = CAllStackingTasks::GetRGBBackgroundCalibrationMethod();
	this->rgbMethod = me == RBCM_MINIMUM ? RgbMethod::Minimum : (me == RBCM_MIDDLE ? RgbMethod::Median : RgbMethod::Maximum);
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
	for (auto it = histo.crbegin(); it != histo.crend(); ++it, --index)
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
}

// Initialize the calibration model (offset, linear, ...).
// Returns the median value of the red channel.
template <int Mult, typename T>
double BackgroundCalibrationCommon<Mult, T>::calculateModelParameters(CMemoryBitmap const& bitmap, const bool calcReference)
{
	if (this->mode == Mode::None)
		return 0;

	const size_t halfNumberOfPixels = static_cast<size_t>(bitmap.Width()) * static_cast<size_t>(bitmap.Height()) / 2;

	const auto [redHisto, greenHisto, blueHisto] = calcHistogram(bitmap);
	const auto [redMedian, redMaximum] = findMedianAndMax(redHisto, halfNumberOfPixels);
	const auto [greenMedian, greenMaximum] = findMedianAndMax(greenHisto, halfNumberOfPixels);
	const auto [blueMedian, blueMaximum] = findMedianAndMax(blueHisto, halfNumberOfPixels);

	if (calcReference)
		calculateReferenceParameters(redMedian, redMaximum, greenMedian, greenMaximum, blueMedian, blueMaximum);

	initializeModel(redMedian, redMaximum, greenMedian, greenMaximum, blueMedian, blueMaximum);

	return redMedian;
}

// ------------------------------
// Offset Calibrator
// ------------------------------

template <int Mult, typename T>
BackgroundCalibrationOffset<Mult, T>::BackgroundCalibrationOffset(const Mode m) : BackgroundCalibrationCommon<Mult, T>{ m } {}


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
BackgroundCalibrationLinear<Mult, T>::BackgroundCalibrationLinear(const Mode m) : BackgroundCalibrationCommon<Mult, T>{ m } {}

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
BackgroundCalibrationRational<Mult, T>::BackgroundCalibrationRational(const Mode m) : BackgroundCalibrationCommon<Mult, T>{ m } {}


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
