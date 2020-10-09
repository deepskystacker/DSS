#pragma once

template <typename>
class CInternalMedianFilterEngineT;

#if defined(AVX_INTRINSICS) && defined(_M_X64)

template <class T>
class AvxImageFilter
{
private:
	CInternalMedianFilterEngineT<double>* filterEngine;
public:
	AvxImageFilter() = delete;
	AvxImageFilter(CInternalMedianFilterEngineT<T>* filEng);
	AvxImageFilter(const AvxImageFilter&) = delete;
	AvxImageFilter(AvxImageFilter&&) = delete;
	AvxImageFilter& operator=(const AvxImageFilter&) = delete;

	int filter(const size_t lineStart, const size_t lineEnd);
};

#else

template <class T>
class AvxImageFilter
{
public:
	AvxImageFilter(CInternalMedianFilterEngineT<T>*) {}
	int filter(const size_t, const size_t)
	{
		return 1;
	}
};
#endif
