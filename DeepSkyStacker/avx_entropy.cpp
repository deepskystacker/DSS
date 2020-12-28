#include "StdAfx.h" 
#include "avx_entropy.h" 
#include "avx.h" 
#include "avx_cfa.h" 
#include "avx_histogram.h" 
#include <immintrin.h> 
#include <omp.h> 

AvxEntropy::AvxEntropy(CMemoryBitmap& inputbm) :
	inputBitmap{ inputbm },
	avxReady{ AvxSupport::checkSimdAvailability() }
{}

int AvxEntropy::calcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies)
{
	if (!avxReady)
		return 1;
	if (doCalcEntropies<WORD>(squareSize, nSquaresX, nSquaresY, redEntropies, greenEntropies, blueEntropies) == 0)
		return 0;
	if (doCalcEntropies<unsigned long>(squareSize, nSquaresX, nSquaresY, redEntropies, greenEntropies, blueEntropies) == 0)
		return 0;
	if (doCalcEntropies<float>(squareSize, nSquaresX, nSquaresY, redEntropies, greenEntropies, blueEntropies) == 0)
		return 0;

	return 1;
}

template <class T>
int AvxEntropy::doCalcEntropies(const int squareSize, const int nSquaresX, const int nSquaresY, EntropyVectorType& redEntropies, EntropyVectorType& greenEntropies, EntropyVectorType& blueEntropies)
{
	// Check input bitmap. 
	const AvxSupport avxInputSupport{ inputBitmap };
	if (!avxInputSupport.isColorBitmapOfType<T>() && !avxInputSupport.isMonochromeBitmapOfType<T>()) // Monochrome includes CFA 
		return 1;

	constexpr int vectorLen = 16;
	const int width = inputBitmap.Width();
	const int height = inputBitmap.Height();

	const auto getDistribution = [](const auto& histogram, T value) -> float
	{
		if constexpr (std::is_integral<T>::value && sizeof(T) == 4) // 32 bit integral type 
			value >>= 16;
		return static_cast<float>(histogram[std::min(static_cast<size_t>(value), size_t{ USHORT_MAX })]);
	};

	const auto calcEntropyOfSquare = [squareSize, width, height, vectorLen, &getDistribution](const int col, const int row, const T* const pColor, auto& histogram) -> EntropyVectorType::value_type
	{
		const int xmin = col * squareSize;
		const int xmax = std::min(xmin + squareSize, width);
		const int nx = xmax - xmin;
		const int ymin = row * squareSize;
		const int ymax = std::min(ymin + squareSize, height);
		const int nrVectors = nx / vectorLen;
		memset(&histogram[0], 0, histogram.size() * sizeof(histogram[0]));

		for (int y = ymin; y < ymax; ++y)
		{
			const T* p = pColor + y * width + xmin;
			for (int n = 0; n < nrVectors; ++n, p += vectorLen)
			{
				const auto [lo, hi] = AvxSupport::read16PackedInt(p);
				AvxHistogram::calcHistoOfVectorEpi32(lo, histogram);
				AvxHistogram::calcHistoOfVectorEpi32(hi, histogram);
			}
			for (int x = xmin + nrVectors * vectorLen; x < xmax; ++x, ++p)
				AvxHistogram::addToHisto(histogram, *p);
		}

		const float N = static_cast<float>(nx * (ymax - ymin));
		const float lnN = std::log(N);
		const float f = 1.0f / (N * std::log(2.0f));
		float entropy = 0.0f;

		for (int y = ymin; y < ymax; ++y)
		{
			const T* p = pColor + y * width + xmin;
			for (int x = xmin; x < xmax; ++x, ++p)
			{
				const float d = getDistribution(histogram, *p);
				entropy += f * d * (lnN - std::log(d));
			}
		}

		return entropy;
	};

	const auto calcEntropy = [nSquaresX, nSquaresY, &calcEntropyOfSquare](const T* const pColor, EntropyVectorType& entropyVector) -> void
	{
		const int nrEnabledThreads = CMultitask::GetNrProcessors(false); // Returns 1 if multithreading disabled by user, otherwise # HW threads 
		constexpr size_t HistoSize = std::numeric_limits<std::uint16_t>::max() + size_t{ 1 };
		std::vector<int> histogram(HistoSize, 0);

#pragma omp parallel for default(none) firstprivate(histogram) schedule(dynamic, 10) if(nrEnabledThreads - 1) 
		for (int y = 0; y < nSquaresY; ++y)
		{
			for (int x = 0, ndx = y * nSquaresX; x < nSquaresX; ++x, ++ndx)
			{
				entropyVector[ndx] = calcEntropyOfSquare(x, y, pColor, histogram);
			}
		}
	};

	const bool isCFA = avxInputSupport.isMonochromeCfaBitmapOfType<T>();

	if (avxInputSupport.isColorBitmapOfType<T>() || isCFA)
	{
		AvxCfaProcessing avxCfa{ 0, 0, inputBitmap };
		if (isCFA)
		{
			const size_t lineEnd = inputBitmap.Height();
			avxCfa.init(0, lineEnd);
			avxCfa.interpolate(0, lineEnd, 1);
		}

		const T* pRedPixels = isCFA ? avxCfa.redCfaLine<T>(0) : &avxInputSupport.redPixels<T>().at(0);
		const T* pGreenPixels = isCFA ? avxCfa.greenCfaLine<T>(0) : &avxInputSupport.greenPixels<T>().at(0);
		const T* pBluePixels = isCFA ? avxCfa.blueCfaLine<T>(0) : &avxInputSupport.bluePixels<T>().at(0);

		calcEntropy(pRedPixels, redEntropies);
		calcEntropy(pGreenPixels, greenEntropies);
		calcEntropy(pBluePixels, blueEntropies);

		return 0;
	}

	if (avxInputSupport.isMonochromeBitmapOfType<T>())
	{
		const T* pGrayPixels = &avxInputSupport.grayPixels<T>().at(0);
		calcEntropy(pGrayPixels, redEntropies);

		memcpy(&greenEntropies[0], &redEntropies[0], redEntropies.size() * sizeof(EntropyVectorType::value_type));
		memcpy(&blueEntropies[0], &redEntropies[0], redEntropies.size() * sizeof(EntropyVectorType::value_type));

		return 0;
	}

	return 1;
}
