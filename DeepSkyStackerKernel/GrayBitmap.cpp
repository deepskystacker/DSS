#include <stdafx.h>
#include <algorithm>
#include "GrayBitmap.h"
#include "DSSProgress.h"
#include "ColorHelpers.h"
#include "BitmapCharacteristics.h"
#include "Multitask.h"
#include "GreyMultiBitmap.h"
#include "MedianFilterEngine.h"
#include "ZExcept.h"

using namespace DSS;

template <typename T>
CGrayBitmapT<T>::CGrayBitmapT() :
	m_lWidth{ 0 },
	m_lHeight{ 0 }
{

}

template <typename T>
std::unique_ptr<CMemoryBitmap> CGrayBitmapT<T>::Clone(bool bEmpty/*=false*/) const
{
	auto pResult = std::make_unique<CGrayBitmapT<T>>();
	if (!bEmpty)
	{
		pResult->m_vPixels = m_vPixels;
		pResult->m_lWidth = m_lWidth;
		pResult->m_lHeight = m_lHeight;
	}
	ZASSERT(pResult->m_bWord == m_bWord); // Will be eliminated by the compiler, as both (static!) variables are referencing the identical memory location.
	ZASSERT(pResult->m_bFloat == m_bFloat);
	pResult->m_CFATransform = m_CFATransform;
	pResult->m_CFAType = m_CFAType;
	pResult->m_bCYMG = m_bCYMG;
	pResult->CopyFrom(*this);

	return pResult;
}

template <typename T>
BAYERCOLOR CGrayBitmapT<T>::GetBayerColor(int x, int y) const
{
	return ::GetBayerColor(x, y, m_CFAType, m_xBayerOffset, m_yBayerOffset);
}

template <typename T>
bool CGrayBitmapT<T>::InitInternals()
{
	const size_t nrPixels = static_cast<size_t>(m_lWidth) * static_cast<size_t>(m_lHeight);
	m_vPixels.clear();
	m_vPixels.resize(nrPixels);

	return true; // Otherwise m_vPixels.resize() would have thrown bad_alloc.
}

template <typename T>
inline void	CGrayBitmapT<T>::CheckXY(size_t x, size_t y) const
{
	ZASSERTSTATE(IsXYOk(x, y));
}

template <typename T>
T CGrayBitmapT<T>::GetPrimary(int x, int y, const COLORREF16& crColor) const
{
	switch (::GetBayerColor(x, y, m_CFAType, m_xBayerOffset, m_yBayerOffset))
	{
	case BAYER_RED:
		return crColor.red;
		break;
	case BAYER_GREEN:
		return crColor.green;
		break;
	case BAYER_BLUE:
		return crColor.blue;
		break;
	}

	return 0;
}

template <typename T>
double CGrayBitmapT<T>::GetPrimary(size_t x, size_t y, double fRed, double fGreen, double fBlue) const
{
	switch (::GetBayerColor(x, y, m_CFAType, m_xBayerOffset, m_yBayerOffset))
	{
	case BAYER_RED:
		return fRed;
		break;
	case BAYER_GREEN:
		return fGreen;
		break;
	case BAYER_BLUE:
		return fBlue;
		break;
	}

	return 0;
}

template <typename T>
double CGrayBitmapT<T>::InterpolateGreen(size_t x, size_t y, const T* pValue/*=nullptr*/) const
{
	double fResult = 0.0;
	int lNrValues = 0;

	if (!pValue)
		pValue = &m_vPixels[GetOffset(x, y)];

	if (x > 0)
	{
		fResult += *(pValue - 1);
		lNrValues++;
	}
	if (x < m_lWidth - 1)
	{
		fResult += *(pValue + 1);
		lNrValues++;
	}
	if (y > 0)
	{
		fResult += *(pValue - m_lWidth);
		lNrValues++;
	}
	if (y < m_lHeight - 1)
	{
		fResult += *(pValue + m_lWidth);
		lNrValues++;
	}

	return fResult / lNrValues;
}

template <typename T>
double CGrayBitmapT<T>::InterpolateBlue(size_t x, size_t y, const T* pValue/*=nullptr*/) const
{
	double fResult = 0.0;
	int lNrValues = 0;

	if (!pValue)
		pValue = &m_vPixels[GetOffset(x, y)];

	if (IsBayerBlueLine(y, m_CFAType, m_yBayerOffset))
	{
		// Pixel between 2 blue pixel (horizontaly)
		if (x > 0)
		{
			fResult += *(pValue - 1);
			lNrValues++;
		}
		if (x < m_lWidth - 1)
		{
			fResult += *(pValue + 1);
			lNrValues++;
		}
	}
	else if (IsBayerBlueColumn(x, m_CFAType, m_xBayerOffset))
	{
		// Pixel between 2 blue pixels (verticaly)
		if (y > 0)
		{
			fResult += *(pValue - m_lWidth);
			lNrValues++;
		}
		if (y < m_lHeight - 1)
		{
			fResult += *(pValue + m_lWidth);
			lNrValues++;
		}
	}
	else
	{
		// Use 4 pixels to interpolate
		if (x > 0 && y > 0)
		{
			fResult += *(pValue - 1 - m_lWidth);
			lNrValues++;
		}
		if ((x > 0) && (y < m_lHeight - 1))
		{
			fResult += *(pValue - 1 + m_lWidth);
			lNrValues++;
		}
		if ((x < m_lWidth - 1) && (y < m_lHeight - 1))
		{
			fResult += *(pValue + 1 + m_lWidth);
			lNrValues++;
		}
		if ((x < m_lWidth - 1) && (y > 0))
		{
			fResult += *(pValue + 1 - m_lWidth);
			lNrValues++;
		}
	}

	return fResult / lNrValues;
}

template <typename T>
double CGrayBitmapT<T>::InterpolateRed(size_t x, size_t y, const T* pValue/*=nullptr*/) const
{
	double fResult = 0.0;
	int lNrValues = 0;

	if (!pValue)
		pValue = &m_vPixels[GetOffset(x, y)];

	if (IsBayerRedLine(y, m_CFAType, m_yBayerOffset))
	{
		// Pixel between 2 blue pixel (horizontaly)
		if (x > 0)
		{
			fResult += *(pValue - 1);
			lNrValues++;
		}
		if (x < m_lWidth - 1)
		{
			fResult += *(pValue + 1);
			lNrValues++;
		}
	}
	else if (IsBayerRedColumn(x, m_CFAType, m_xBayerOffset))
	{
		// Pixel between 2 blue pixels (verticaly)
		if (y > 0)
		{
			fResult += *(pValue - m_lWidth);
			lNrValues++;
		}
		if (y < m_lHeight - 1)
		{
			fResult += *(pValue + m_lWidth);
			lNrValues++;
		}
	}
	else
	{
		// Use 4 pixels to interpolate
		if (x > 0 && y > 0)
		{
			fResult += *(pValue - 1 - m_lWidth);
			lNrValues++;
		}
		if ((x > 0) && (y < m_lHeight - 1))
		{
			fResult += *(pValue - 1 + m_lWidth);
			lNrValues++;
		}
		if ((x < m_lWidth - 1) && (y < m_lHeight - 1))
		{
			fResult += *(pValue + 1 + m_lWidth);
			lNrValues++;
		}
		if ((x < m_lWidth - 1) && (y > 0))
		{
			fResult += *(pValue + 1 - m_lWidth);
			lNrValues++;
		}
	}

	return fResult / lNrValues;
}

template <typename T>
void CGrayBitmapT<T>::InterpolateAll(double* pfValues, size_t x, size_t y) const
{
	size_t lIndice;
	size_t lNrValues[4] = { 0 };

	pfValues[0] = pfValues[1] = pfValues[2] = pfValues[3] = 0;

	for (size_t i = std::max(static_cast<size_t>(0), x - 1); i <= std::min(static_cast<size_t>(m_lWidth - 1), x + 1); i++)
		for (size_t j = std::max(static_cast<size_t>(0), y - 1); j <= std::min(static_cast<size_t>(m_lHeight - 1), y + 1); j++)
		{
			lIndice = CMYGZeroIndex(::GetBayerColor(i, j, m_CFAType, m_xBayerOffset, m_yBayerOffset));
			pfValues[lIndice] += m_vPixels[GetOffset(i, j)];
			lNrValues[lIndice] ++;
		}

	pfValues[0] /= std::max(static_cast<size_t>(1), lNrValues[0]);
	pfValues[1] /= std::max(static_cast<size_t>(1), lNrValues[1]);
	pfValues[2] /= std::max(static_cast<size_t>(1), lNrValues[2]);
	pfValues[3] /= std::max(static_cast<size_t>(1), lNrValues[3]);

	/*
			// It's used only for CYMG - so cut it down to the basic

			if (x==m_lWidth-1)
				x = m_lWidth-2;
			if (y==m_lHeight-1)
				y = m_lHeight-2;

			pfValues[0]  = pfValues[1]  = pfValues[2]  = pfValues[3]  = 0;

			for (int i = x;i<=x+1;i++)
				for (int j = y;j<=y+1;j++)
					pfValues[CMYGZeroIndex(::GetBayerColor(i, j, m_CFAType))]  = m_vPixels[GetOffset(i, j)];
	*/
}

template <typename T>
void CGrayBitmapT<T>::SetPixel(size_t i, size_t j, double fRed, double fGreen, double fBlue)
{
	if (m_CFATransform == CFAT_SUPERPIXEL)
	{
		SetPixel(i * 2, j * 2, fRed);
		SetPixel(i * 2, j * 2 + 1, fGreen);
		SetPixel(i * 2 + 1, j * 2, fGreen);
		SetPixel(i * 2 + 1, j * 2 + 1, fBlue);
	}
	else if (m_CFATransform == CFAT_NONE)
		SetPixel(i, j, fRed);
	else
		SetPixel(i, j, GetPrimary(i, j, fRed, fGreen, fBlue));
}

template <typename T>
void CGrayBitmapT<T>::SetPixel(size_t i, size_t j, double fGray)
{
	SetValue(i, j, std::clamp(fGray * m_fMultiplier, 0.0, clampValue));
}

template <typename T>
void CGrayBitmapT<T>::GetPixel(size_t i, size_t j, double& fRed, double& fGreen, double& fBlue) const
{
	CheckXY(i, j);
	fRed = fGreen = fBlue = 0.0;

	if (m_CFATransform == CFAT_SUPERPIXEL)
	{
		assert(m_bWord);
		if (IsXYOk((i - 1) * 2, (j - 1) * 2) && IsXYOk((i + 1) * 2 + 1, (j + 1) * 2 + 1))
		{
			const T* pValue = &(m_vPixels[GetOffset(i * 2, j * 2)]);

			switch (m_CFAType)
			{
			case CFATYPE_GRBG:
				fRed = (*(pValue + 1)) / m_fMultiplier;
				fGreen = ((*pValue) + (*(pValue + 1 + m_lWidth))) / 2.0 / m_fMultiplier;
				fBlue = (*(pValue + m_lWidth)) / m_fMultiplier;
				break;
			case CFATYPE_GBRG:
				fRed = (*(pValue + m_lWidth)) / m_fMultiplier;
				fGreen = ((*pValue) + (*(pValue + 1 + m_lWidth))) / 2.0 / m_fMultiplier;
				fBlue = (*(pValue + 1)) / m_fMultiplier;
				break;
			case CFATYPE_BGGR:
				fRed = (*(pValue + 1 + m_lWidth)) / m_fMultiplier;
				fGreen = ((*(pValue + m_lWidth)) + (*(pValue + 1))) / 2.0 / m_fMultiplier;
				fBlue = (*pValue) / m_fMultiplier;
				break;
			case CFATYPE_RGGB:
				fRed = (*pValue) / m_fMultiplier;
				fGreen = ((*(pValue + m_lWidth)) + (*(pValue + 1))) / 2.0 / m_fMultiplier;
				fBlue = (*(pValue + 1 + m_lWidth)) / m_fMultiplier;
				break;
			}
		}
	}
	else if (m_CFATransform == CFAT_RAWBAYER)
	{
		assert(m_bWord);
		const T* pValue = &(m_vPixels[GetOffset(i, j)]);

		switch (::GetBayerColor(i, j, m_CFAType, m_xBayerOffset, m_yBayerOffset))
		{
		case BAYER_RED:
			fRed = (*pValue) / m_fMultiplier;
			break;
		case BAYER_GREEN:
			fGreen = (*pValue) / m_fMultiplier;
			break;
		case BAYER_BLUE:
			fBlue = (*pValue) / m_fMultiplier;
			break;
		}
	}
	else if ((m_CFATransform == CFAT_BILINEAR) || (m_CFATransform == CFAT_AHD))
	{
		assert(m_bWord);
		if (m_bCYMG)
		{
			double fValue[4]; // Myself
			InterpolateAll(fValue, i, j);

			CYMGToRGB(fValue[CMYGZeroIndex(BAYER_CYAN)] / m_fMultiplier,
				fValue[CMYGZeroIndex(BAYER_YELLOW)] / m_fMultiplier,
				fValue[CMYGZeroIndex(BAYER_MAGENTA)] / m_fMultiplier,
				fValue[CMYGZeroIndex(BAYER_GREEN2)] / m_fMultiplier,
				fRed, fGreen, fBlue
			);
		}
		else
		{
			const T* pValue = &(m_vPixels[GetOffset(i, j)]);
			switch (::GetBayerColor(i, j, m_CFAType, m_xBayerOffset, m_yBayerOffset))
			{
			case BAYER_RED:
				fRed = (*pValue) / m_fMultiplier;
				fGreen = InterpolateGreen(i, j, pValue) / m_fMultiplier;
				fBlue = InterpolateBlue(i, j, pValue) / m_fMultiplier;
				break;
			case BAYER_GREEN:
				fRed = InterpolateRed(i, j, pValue) / m_fMultiplier;
				fGreen = (*pValue) / m_fMultiplier;
				fBlue = InterpolateBlue(i, j, pValue) / m_fMultiplier;
				break;
			case BAYER_BLUE:
				fRed = InterpolateRed(i, j, pValue) / m_fMultiplier;
				fGreen = InterpolateGreen(i, j, pValue) / m_fMultiplier;
				fBlue = (*pValue) / m_fMultiplier;
				break;
			}
		}

	}
	else if (m_CFATransform == CFAT_GRADIENT)
	{
	}
	else
	{
		fRed = fBlue = fGreen = m_vPixels[GetOffset(i, j)] / m_fMultiplier;
	}
}

template <typename T>
void CGrayBitmapT<T>::GetPixel(size_t i, size_t j, double& fGray) const
{
	GetValue(i, j, fGray);
	fGray /= m_fMultiplier;
}

template <typename T>
bool CGrayBitmapT<T>::GetScanLine(size_t j, void* pScanLine) const
{
	bool bResult = false;

	if (j < m_lHeight)
	{
		memcpy(pScanLine, &(m_vPixels[j * m_lWidth]), sizeof(T) * m_lWidth);
		bResult = true;
	}

	return bResult;
}

template <typename T>
bool CGrayBitmapT<T>::SetScanLine(size_t j, void* pScanLine)
{
	bool bResult = false;

	if (j < m_lHeight)
	{
		memcpy(&(m_vPixels[j * m_lWidth]), pScanLine, sizeof(T) * m_lWidth);
		bResult = true;
	}

	return bResult;
}

template <typename T>
std::shared_ptr<CMultiBitmap> CGrayBitmapT<T>::CreateEmptyMultiBitmap() const
{
	std::shared_ptr<CMultiBitmap> result;
	//
	// If the input bitmap is 16 bit or 32 bit integer want the output type to be float
	//
	//if (std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint16_t>)
	//	result = std::make_shared<CGrayMultiBitmapT<T,float>>();
	//else
	result = std::make_shared<CGrayMultiBitmapT<T>>();
	
	result->SetBitmapModel(this);
	return result;
}

template <typename T>
std::shared_ptr<CMedianFilterEngine> CGrayBitmapT<T>::GetMedianFilterEngine() const
{
	std::shared_ptr<CGrayMedianFilterEngineT<T>> pFilter = std::make_shared<CGrayMedianFilterEngineT<T>>();
	pFilter->SetInputBitmap(this);
	return pFilter;
}

template <typename T>
void CGrayBitmapT<T>::InitIterator(void*& pRed, void*& pGreen, void*& pBlue, size_t& elementSize, const size_t x, const size_t y)
{
	pRed = static_cast<void*>(this->m_vPixels.data() + GetOffset(x, y));
	pGreen = pRed;
	pBlue = pRed;
	elementSize = sizeof(T);
}

template <typename T>
void CGrayBitmapT<T>::InitIterator(const void*& pRed, const void*& pGreen, const void*& pBlue, size_t& elementSize, const size_t x, const size_t y) const
{
	pRed = static_cast<const void*>(this->m_vPixels.data() + GetOffset(x, y));
	pGreen = pRed;
	pBlue = pRed;
	elementSize = sizeof(T);
}

template <typename T>
void CGrayBitmapT<T>::GetCharacteristics(CBitmapCharacteristics& bc) const
{
	bc.m_bFloat = m_bFloat;
	bc.m_dwHeight = m_lHeight;
	bc.m_dwWidth = m_lWidth;
	bc.m_lNrChannels = 1;
	bc.m_lBitsPerPixel = BitPerSample();
}


#pragma warning( push )
#pragma warning( disable : 4189 ) // unreachable code from initial constexpr if statement.

template <typename T>
void CGrayBitmapT<T>::RemoveHotPixels(ProgressBase* pProgress)
{
	const int nrProcessors = CMultitask::GetNrProcessors();

	if (pProgress != nullptr)
	{
		const QString strText(QCoreApplication::translate("BitmapBase", "Detecting hot pixels", "IDS_REMOVINGHOTPIXELS"));
		pProgress->Start2(strText, m_lHeight);
	}

	const int height = this->Height();
	const int width = this->Width();
	std::vector<size_t> hotOffsets;
	std::vector<size_t> localHotOffsets;

#pragma omp parallel default(none) shared(hotOffsets) firstprivate(localHotOffsets) if(nrProcessors > 1)
	{
#pragma omp for schedule(dynamic, 50) nowait
		for (int row = 2; row < height - 2; ++row)
		{
			for (int column = 2; column < width - 2; ++column)
			{
				const size_t testOffset = this->GetOffset(column, row);
				const T testValue = this->m_vPixels[testOffset];
				constexpr double hotFactor = 4;

				if (testValue > hotFactor * m_vPixels[GetOffset(column - 1, row)]
					&& testValue > hotFactor * m_vPixels[GetOffset(column + 1, row)]
					&& testValue > hotFactor * m_vPixels[GetOffset(column, row + 1)]
					&& testValue > hotFactor * m_vPixels[GetOffset(column, row - 1)])
				{
					localHotOffsets.push_back(testOffset);
					++column;
				}
			}
		}
#pragma omp critical(OmpLockHotpixelRemove)
		{
			hotOffsets.insert(hotOffsets.end(), localHotOffsets.cbegin(), localHotOffsets.cend());
		}
	}

	for (const auto hotOffset : hotOffsets)
		this->m_vPixels[hotOffset] = 0;

	if (pProgress != nullptr)
		pProgress->End2();
}

#pragma warning( pop )

// Define these here so that we can simple headers. We only run with these types so this isn't too bad.
template class CGrayBitmapT<std::uint8_t>;
template class CGrayBitmapT<std::uint16_t>;
template class CGrayBitmapT<std::uint32_t>;
template class CGrayBitmapT<float>;
template class CGrayBitmapT<double>;

