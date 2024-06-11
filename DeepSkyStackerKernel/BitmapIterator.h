#pragma once
class CMemoryBitmap;
template <class Ptr, class T>
requires (
	(std::derived_from<std::remove_pointer_t<T>, CMemoryBitmap> || std::derived_from<std::remove_const_t<typename T::element_type>, CMemoryBitmap>)
	&& (std::is_copy_constructible_v<T>)
	&& (std::is_same_v<Ptr, void*> || std::is_same_v<Ptr, const void*>)
)
class BitmapIt
{
protected:
	T bitmap;
	Ptr pRed;
	Ptr pGreen;
	Ptr pBlue;
private:
	size_t elementSize;
public:
	BitmapIt(T p) :
		bitmap{ p }
	{
		Reset(0, 0);
	}

	BitmapIt(const BitmapIt&) = delete;

	void Reset(const size_t x, const size_t y)
	{
		bitmap->InitIterator(pRed, pGreen, pBlue, elementSize, x, y);
	}

	void GetPixel(double& r, double& g, double& b) const
	{
		const auto [vr, vg, vb] = bitmap->ConvertValue3(this->pRed, this->pGreen, this->pBlue);
		r = vr;
		g = vg;
		b = vb;
	}
	double GetPixel() const
	{
		return bitmap->ConvertValue1(this->pRed, this->pGreen, this->pBlue);
	}

	void SetPixel(const double r, const double g, const double b)
	{
		static_assert(std::is_same_v<Ptr, void*>);
		bitmap->ReceiveValue(this->pRed, this->pGreen, this->pBlue, r, g, b);
	}
	void SetPixel(const double gray)
	{
		static_assert(std::is_same_v<Ptr, void*>);
		bitmap->ReceiveValue(this->pRed, this->pGreen, this->pBlue, gray);
	}

	void operator++()
	{
		if constexpr (std::is_same_v<Ptr, const void*>)
		{
			pRed = static_cast<const std::uint8_t*>(pRed) + this->elementSize;
			pGreen = static_cast<const std::uint8_t*>(pGreen) + this->elementSize;
			pBlue = static_cast<const std::uint8_t*>(pBlue) + this->elementSize;
		}
		else
		{
			pRed = static_cast<std::uint8_t*>(pRed) + this->elementSize;
			pGreen = static_cast<std::uint8_t*>(pGreen) + this->elementSize;
			pBlue = static_cast<std::uint8_t*>(pBlue) + this->elementSize;
		}
	}
	void operator+=(const size_t n)
	{
		if constexpr (std::is_same_v<Ptr, const void*>)
		{
			pRed = static_cast<const std::uint8_t*>(pRed) + n * this->elementSize;
			pGreen = static_cast<const std::uint8_t*>(pGreen) + n * this->elementSize;
			pBlue = static_cast<const std::uint8_t*>(pBlue) + n * this->elementSize;
		}
		else
		{
			pRed = static_cast<std::uint8_t*>(pRed) + n * this->elementSize;
			pGreen = static_cast<std::uint8_t*>(pGreen) + n * this->elementSize;
			pBlue = static_cast<std::uint8_t*>(pBlue) + n * this->elementSize;
		}
	}
};

template <class T>
using BitmapIteratorConst = BitmapIt<const void*, T>;

template <class T>
using BitmapIterator = BitmapIt<void*, T>;
