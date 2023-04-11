#pragma once

template <typename>
class CInternalMedianFilterEngineT;

template <class T>
class AvxImageFilter
{
private:
	CInternalMedianFilterEngineT<double>* filterEngine;
public:
	AvxImageFilter() = delete;
	AvxImageFilter(CInternalMedianFilterEngineT<T>* filEng);
	AvxImageFilter(const AvxImageFilter&) = default;
	AvxImageFilter(AvxImageFilter&&) = delete;
	AvxImageFilter& operator=(const AvxImageFilter&) = delete;

	int filter(const size_t lineStart, const size_t lineEnd);
};
