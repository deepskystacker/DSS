#include <stdafx.h>
//#include "resource.h"
#include "BitmapBase.h"
#include "BackgroundCalibration.h"
#include "DSSProgress.h"
#include "StackingTasks.h"
#include "avx_histogram.h"
#include "Multitask.h"
#include "Ztrace.h"

CBackgroundCalibration::CBackgroundCalibration() :
	m_bInitOk{ false },
	m_fMultiplier{ 1.0 },
	m_BackgroundCalibrationMode{ CAllStackingTasks::GetBackgroundCalibrationMode() },
	m_BackgroundInterpolation{ CAllStackingTasks::GetBackgroundCalibrationInterpolation() },
	m_RGBBackgroundMethod{ CAllStackingTasks::GetRGBBackgroundCalibrationMethod() }
{}

void CBackgroundCalibration::ompCalcHistogram(CMemoryBitmap* pBitmap, ProgressBase* pProgress, std::vector<int>& redHisto, std::vector<int>& greenHisto, std::vector<int>& blueHisto) const
{
	AvxHistogram avxHistogram(*pBitmap);
	std::vector<int> redLocalHist(avxHistogram.isAvxReady() ? 0 : HistogramSize()); // Only allocate mem if AVX will not be used (see below).
	std::vector<int> greenLocalHist(avxHistogram.isAvxReady() ? 0 : HistogramSize());
	std::vector<int> blueLocalHist(avxHistogram.isAvxReady() ? 0 : HistogramSize());
	const int height = pBitmap->Height();
	const auto nrProcessors = CMultitask::GetNrProcessors();

#pragma omp parallel default(none) shared(redHisto, greenHisto, blueHisto) firstprivate(avxHistogram, redLocalHist, greenLocalHist, blueLocalHist) if(nrProcessors > 1)
	{
		constexpr int Bulksize = 10;
#pragma omp for schedule(guided, 50)
		for (int startNdx = 0; startNdx < height; startNdx += Bulksize)
		{
			const int endNdx = std::min(startNdx + Bulksize, height);
			if (avxHistogram.calcHistogram(startNdx, endNdx) != 0)
			{
				constexpr double Maxvalue = double{ std::numeric_limits<std::uint16_t>::max() };
				const double fMultiplier = m_fMultiplier * 256.0;

				if (redLocalHist.empty()) {
					redLocalHist.resize(HistogramSize(), 0);
					greenLocalHist.resize(HistogramSize(), 0);
					blueLocalHist.resize(HistogramSize(), 0);
				}

				if (pProgress != nullptr && omp_get_thread_num() == 0) // Only master thread
					pProgress->Progress2(startNdx);

				for (int j = startNdx; j < endNdx; ++j)
				{
					for (int i = 0, width = pBitmap->Width(); i < width; ++i)
					{
						COLORREF16 crColor;
						double fRed, fGreen, fBlue;
						pBitmap->GetPixel(i, j, fRed, fGreen, fBlue);
						fRed *= fMultiplier;
						fGreen *= fMultiplier;
						fBlue *= fMultiplier;

						crColor.red = std::min(fRed, Maxvalue);
						crColor.blue = std::min(fBlue, Maxvalue);
						crColor.green = std::min(fGreen, Maxvalue);

						redLocalHist[crColor.red]++;
						greenLocalHist[crColor.green]++;
						blueLocalHist[crColor.blue]++;
					}
				}
			}
		}

		int rval = 1;
#pragma omp critical(OmpLockHistoMerge)
		{
			if (avxHistogram.histogramSuccessful())
				rval = avxHistogram.mergeHistograms(redHisto, greenHisto, blueHisto);
			if (rval != 0)
				for (size_t i = 0; i < HistogramSize(); ++i)
				{
					redHisto[i] += redLocalHist[i];
					greenHisto[i] += greenLocalHist[i];
					blueHisto[i] += blueLocalHist[i];
				}
		}
	} // omp parallel
}

void CBackgroundCalibration::ComputeBackgroundCalibration(CMemoryBitmap* pBitmap, bool bFirst, ProgressBase* pProgress)
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
		if (m_BackgroundCalibrationMode == BCM_PERCHANNEL)
		{
			m_fTgtRedBk = m_fSrcRedBk;
			m_fTgtGreenBk = m_fSrcGreenBk;
			m_fTgtBlueBk = m_fSrcBlueBk;
		}
		else if (m_BackgroundCalibrationMode == BCM_RGB)
		{
			double fTgtBk;

			if (m_RGBBackgroundMethod == RBCM_MAXIMUM)
				fTgtBk = std::max(m_fSrcRedBk, std::max(m_fSrcGreenBk, m_fSrcBlueBk));
			else if (m_RGBBackgroundMethod == RBCM_MINIMUM)
				fTgtBk = std::min(m_fSrcRedBk, std::min(m_fSrcGreenBk, m_fSrcBlueBk));
			else
				fTgtBk = Median(m_fSrcRedBk, m_fSrcGreenBk, m_fSrcBlueBk);

			m_fTgtRedBk = m_fSrcRedMax > fTgtBk ? fTgtBk : m_fSrcRedBk;
			m_fTgtGreenBk = m_fSrcGreenMax > fTgtBk ? fTgtBk : m_fSrcGreenBk;
			m_fTgtBlueBk = m_fSrcBlueMax > fTgtBk ? fTgtBk : m_fSrcBlueBk;
		}
		ZTRACE_RUNTIME("Target Background : Red = %.2f - Green = %.2f - Blue = %.2f", m_fTgtRedBk/256.0, m_fTgtGreenBk/256.0, m_fTgtBlueBk/256.0);
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
