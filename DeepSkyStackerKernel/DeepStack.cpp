#include "pch.h"
#include "DeepStack.h"
#include "histogram.h"
#include "ztrace.h"
#include "Multitask.h"
#include "DSSProgress.h"

using namespace DSS;

void DeepStack::computeHistogram(RGBHistogram & Histo)
{
	double fMax = 0;
	const size_t width = GetWidth();
	const int height = GetHeight();
	const int nrEnabledThreads = Multitask::GetNrProcessors(); // Returns 1 if multithreading disabled by user, otherwise # HW threads
	float maxValue = 0;
	const float scalingFactor = 255.0f / static_cast<float>(stackedBitmap.GetNrStackedFrames());
	const auto& redPixels = stackedBitmap.getRedPixels();
	const auto& greenPixels = stackedBitmap.getGreenPixels();
	const auto& bluePixels = stackedBitmap.getBluePixels();

	Histo.clear();

#pragma omp parallel default(shared) firstprivate(maxValue) shared(fMax, redPixels, greenPixels, bluePixels) if(nrEnabledThreads - 1)
	{
#pragma omp for schedule(guided, 50)
		for (int row = 0; row < height; ++row)
		{
			size_t ndx = row * width;
			if (stackedBitmap.IsMonochrome())
			{
				for (size_t col = 0; col < width; ++col, ++ndx)
					maxValue = std::max(maxValue, redPixels[ndx]);
			}
			else
			{
				for (size_t col = 0; col < width; ++col, ++ndx)
				{
					const float red = redPixels[ndx];
					const float green = greenPixels[ndx];
					const float blue = bluePixels[ndx];
					maxValue = std::max(maxValue, std::max(red, std::max(green, blue)));
				};
			};
		};

#pragma omp critical(OrigHistoCalcOmpCrit)
		fMax = std::max(fMax, static_cast<double>(maxValue * scalingFactor));
	}

	Histo.SetSize(fMax, static_cast<size_t>(65535));

	if (!stackedBitmap.IsMonochrome())
	{
#pragma omp parallel sections default(shared) shared(Histo, redPixels, greenPixels, bluePixels) if(nrEnabledThreads - 1)
		{
#pragma omp section
			for (const auto& color : redPixels)
				Histo.GetRedHistogram().AddValue(static_cast<double>(color * scalingFactor));
#pragma omp section
			for (const auto& color : greenPixels)
				Histo.GetGreenHistogram().AddValue(static_cast<double>(color * scalingFactor));
#pragma omp section
			for (const auto& color: bluePixels)
				Histo.GetBlueHistogram().AddValue(static_cast<double>(color * scalingFactor));
		}
	}
	else
	{
		for (const auto& color: redPixels)
			Histo.GetRedHistogram().AddValue(color * scalingFactor);

		Histo.GetGreenHistogram() = Histo.GetRedHistogram();
		Histo.GetBlueHistogram() = Histo.GetRedHistogram();
	}
};

/* ------------------------------------------------------------------- */

void DeepStack::computeDisplayHistogram(RGBHistogram& displayHisto)
{
	if (!histogram.IsInitialized())
		computeHistogram(histogram);

	const auto& redHistogram = histogram.GetRedHistogram();
	const auto& greenHistogram = histogram.GetGreenHistogram();
	const auto& blueHistogram = histogram.GetBlueHistogram();

	bool monochrome = stackedBitmap.IsMonochrome();

	double redCount;
	double greenCount;
	double blueCount;

	for (size_t i = 0; i < histogram.GetSize(); i++)
	{

		redCount = redHistogram.componentValue(i);
		if (monochrome)
		{
			greenCount = blueCount = redCount;
		}
		else
		{
			greenCount = greenHistogram.componentValue(i);
			blueCount = blueHistogram.componentValue(i);
		}

		displayHisto.GetRedHistogram().AddValue(redCount, static_cast<size_t>(redHistogram.GetValue(i)));
		displayHisto.GetGreenHistogram().AddValue(greenCount, static_cast<size_t>(greenHistogram.GetValue(i)));
		displayHisto.GetBlueHistogram().AddValue(blueCount, static_cast<size_t>(blueHistogram.GetValue(i)));
	}
}

/* ------------------------------------------------------------------- */

bool DeepStack::LoadStackedInfo(const fs::path& file)
{
	ZFUNCTRACE_RUNTIME();
	bool				bResult;

	bResult = stackedBitmap.Load(file, m_pProgress);

	if (bResult)
	{
		computeHistogram(histogram);
	}


	return bResult;
};

/* ------------------------------------------------------------------- */
