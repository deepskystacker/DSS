#pragma once

#include "BitmapExt.h"
#include <vector>

class AvxHistogram
{
public:
	typedef std::vector<long> HistogramVectorType;
private:
	HistogramVectorType redHisto;
	HistogramVectorType greenHisto;
	HistogramVectorType blueHisto;
	CMemoryBitmap& inputBitmap;
	bool avxReady;
	bool allRunsSuccessful;
public:
	AvxHistogram() = delete;
	AvxHistogram(CMemoryBitmap& inputbm);
	AvxHistogram(const AvxHistogram&) = default;
	AvxHistogram(AvxHistogram&&) = delete;
	AvxHistogram& operator=(const AvxHistogram&) = delete;

	int calcHistogram(const size_t lineStart, const size_t lineEnd);
	int mergeHistograms(HistogramVectorType& red, HistogramVectorType& green, HistogramVectorType& blue);
	bool histogramSuccessful() const;
private:
	static constexpr size_t HistogramSize() { return std::numeric_limits<std::uint16_t>::max() + size_t{1}; }
	template <class T>
	int doCalcHistogram(const size_t lineStart, const size_t lineEnd);

	// Conflict detection: Number of equal elements + blocking mask.
	static std::tuple<__m256i, std::uint32_t> detectConflictsEpi32(const __m256i a) noexcept;
	static std::tuple<__m256i, std::uint32_t> detectConflictsEpi16(const __m256i a) noexcept;
};
