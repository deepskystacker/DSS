#pragma once

#include <vector>
#include "cfa.h"
#include "ColorBitmap.h"

class AvxBitmapUtil
{
private:
	const CMemoryBitmap& bitmap;
	CMemoryBitmap* pBitmap;

	template <class T>
	auto* getColorPtr() { return dynamic_cast<CColorBitmapT<T>*>(pBitmap); }
	template <class T>
	auto* getGrayPtr() { return dynamic_cast<CGrayBitmapT<T>*>(pBitmap); }
	template <class T>
	const auto* getColorPtr() const { return dynamic_cast<const CColorBitmapT<T>*>(&bitmap); }
	template <class T>
	const auto* getGrayPtr() const { return dynamic_cast<const CGrayBitmapT<T>*>(&bitmap); }

	int getNrChannels() const;
public:
	explicit AvxBitmapUtil(CMemoryBitmap& b);
	explicit AvxBitmapUtil(const CMemoryBitmap& b);
	AvxBitmapUtil(const AvxBitmapUtil&) = delete;
	AvxBitmapUtil(AvxBitmapUtil&&) = delete;
	AvxBitmapUtil& operator=(const AvxBitmapUtil&) = delete;

	bool isColorBitmap() const;
	template <class T> bool isColorBitmapOfType() const;
	bool isMonochromeBitmap() const;
	template <class T> bool isMonochromeBitmapOfType() const;
	template <class T> bool isMonochromeCfaBitmapOfType() const;
	bool isColorBitmapOrCfa() const;

	CFATYPE getCfaType() const;

	template <class T>
	const std::vector<T>& redPixels() const { return getColorPtr<T>()->m_Red.m_vPixels; }
	template <class T>
	const std::vector<T>& greenPixels() const { return getColorPtr<T>()->m_Green.m_vPixels; }
	template <class T>
	const std::vector<T>& bluePixels() const { return getColorPtr<T>()->m_Blue.m_vPixels; }
	template <class T>
	const std::vector<T>& grayPixels() const { return getGrayPtr<T>()->m_vPixels; }

	template <class T>
	std::vector<T>& redPixels() { return getColorPtr<T>()->m_Red.m_vPixels; }
	template <class T>
	std::vector<T>& greenPixels() { return getColorPtr<T>()->m_Green.m_vPixels; }
	template <class T>
	std::vector<T>& bluePixels() { return getColorPtr<T>()->m_Blue.m_vPixels; }
	template <class T>
	std::vector<T>& grayPixels() { return getGrayPtr<T>()->m_vPixels; }

	const int width() const;

	template <class T>
	bool bitmapHasCorrectType() const;

	template <class ElementType, class VectorElementType>
	inline static size_t numberOfAvxVectors(const size_t width)
	{
		static_assert(sizeof(ElementType) == 1 || sizeof(ElementType) == 2 || sizeof(ElementType) == 4 || sizeof(ElementType) == 8);
		return width == 0 ? 0 : ((width - 1) * sizeof(ElementType)) / sizeof(VectorElementType) + 1;
	}
};
