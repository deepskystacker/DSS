#include "stdafx.h"
#include "ColorMultiBitmap.h"
#include "BitmapCharacteristics.h"
#include "ColorBitmap.h"
#include "BitmapBase.h"
#include "zexcbase.h"
#include "DSSTools.h"

template <typename TType, typename TTypeOutput>
std::shared_ptr<CMemoryBitmap> CColorMultiBitmapT<TType, TTypeOutput>::CreateNewMemoryBitmap() const
{
	if (static_cast<bool>(m_pBitmapModel))
	{
		CBitmapCharacteristics bc;
		m_pBitmapModel->GetCharacteristics(bc);
		if (bc.m_lNrChannels == 3)
			return m_pBitmapModel->Clone(true);
		else
		{
			bc.m_lNrChannels = 3;
			return CreateBitmap(bc);
		}
	}
	else
		return std::make_shared<CColorBitmapT<TType>>();
}

template <typename TType, typename TTypeOutput>
std::shared_ptr<CMemoryBitmap> CColorMultiBitmapT<TType, TTypeOutput>::CreateOutputMemoryBitmap() const
{
	return std::make_shared<CColorBitmapT<TTypeOutput>>();
}

template <typename TType, typename TTypeOutput>
bool CColorMultiBitmapT<TType, TTypeOutput>::SetScanLines(CMemoryBitmap* pBitmap, int lLine, const std::vector<void*>& vScanLines)
{
	bool bResult = false;
	// Each scan line consist of lWidth TType values
	TTypeOutput* pRedCurrentValue;
	TTypeOutput* pGreenCurrentValue;
	TTypeOutput* pBlueCurrentValue;
	std::vector<TType> vRedValues;
	std::vector<TType> vGreenValues;
	std::vector<TType> vBlueValues;
	std::vector<TType> vWorkingBuffer1;
	std::vector<TType> vWorkingBuffer2;
	std::vector<double> vdWork1;			// Used for AutoAdaptiveWeightedAverage
	std::vector<double> vdWork2;			// Used for AutoAdaptiveWeightedAverage
	double fMaximum = pBitmap->GetMaximumValue();
	const int lWidth = pBitmap->RealWidth();

	std::vector<TTypeOutput> outputScanBuffer;
	try {
		outputScanBuffer.resize(lWidth * 3);
	}
	catch (...) {
		ZOutOfMemory e("Could not allocate storage for output scanline");
		ZTHROW(e);
	}

	pRedCurrentValue = outputScanBuffer.data();
	pGreenCurrentValue = pRedCurrentValue + lWidth;
	pBlueCurrentValue = pGreenCurrentValue + lWidth;

	vRedValues.reserve(vScanLines.size());
	vGreenValues.reserve(vScanLines.size());
	vBlueValues.reserve(vScanLines.size());
	vWorkingBuffer1.reserve(vScanLines.size());
	vWorkingBuffer2.reserve(vScanLines.size());
	vdWork1.reserve(vScanLines.size());
	vdWork2.reserve(vScanLines.size());

	for (int i = 0; i < lWidth; i++)
	{
		TType* pRedValue;
		TType* pGreenValue;
		TType* pBlueValue;

		vRedValues.resize(0);
		vGreenValues.resize(0);
		vBlueValues.resize(0);
		for (auto p : vScanLines)
		{
			pRedValue = static_cast<TType*>(p) + i;
			pGreenValue = pRedValue + lWidth;
			pBlueValue = pGreenValue + lWidth;

			if (*pRedValue || !m_vImageOrder.empty())	// Remove 0
				vRedValues.push_back(*pRedValue);
			if (*pGreenValue || !m_vImageOrder.empty())	// Remove 0
				vGreenValues.push_back(*pGreenValue);
			if (*pBlueValue || !m_vImageOrder.empty())	// Remove 0
				vBlueValues.push_back(*pBlueValue);
		}

		if (m_bHomogenization)
		{
			//	if ((i==843) && (lLine==934))
			//		DebugBreak();

			if (static_cast<bool>(m_pHomBitmap))
			{
				double fAverage, fSigma;
				double fRed, fGreen, fBlue;

				fSigma = Sigma2(vRedValues, fAverage);
				fRed = fSigma / std::max(1.0, fAverage) * 256.0;
				fSigma = Sigma2(vGreenValues, fAverage);
				fGreen = fSigma / std::max(1.0, fAverage) * 256.0;
				fSigma = Sigma2(vBlueValues, fAverage);
				fBlue = fSigma / std::max(1.0, fAverage) * 256.0;

				m_pHomBitmap->SetPixel(i, lLine, fRed, fGreen, fBlue);
			}

			if (!m_vImageOrder.empty())
			{
				std::vector<TType> vAuxRedValues;
				std::vector<TType> vAuxGreenValues;
				std::vector<TType> vAuxBlueValues;
				// Change the order to respect the order of the images
				std::swap(vAuxRedValues, vRedValues);
				std::swap(vAuxGreenValues, vGreenValues);
				std::swap(vAuxBlueValues, vBlueValues);
				for (const size_t imgOrder : m_vImageOrder)
				{
					if (vAuxRedValues[imgOrder] || vAuxGreenValues[imgOrder] || vAuxBlueValues[imgOrder])
					{
						vRedValues.push_back(vAuxRedValues[imgOrder]);
						vGreenValues.push_back(vAuxGreenValues[imgOrder]);
						vBlueValues.push_back(vAuxBlueValues[imgOrder]);
					}
				}

				Homogenize(vRedValues, fMaximum);
				Homogenize(vGreenValues, fMaximum);
				Homogenize(vBlueValues, fMaximum);
			}
			else
			{
				Homogenize(vRedValues, fMaximum);
				Homogenize(vGreenValues, fMaximum);
				Homogenize(vBlueValues, fMaximum);
			}

			if (vRedValues.empty() || vGreenValues.empty() || vBlueValues.empty())
			{
				vRedValues.resize(0);
				vGreenValues.resize(0);
				vBlueValues.resize(0);
			}
		}
		else
		{
			if constexpr (sizeof(TType) == 4 && std::is_integral<TType>::value)
			{

				for (auto& x : vRedValues) x >>= 16;
				for (auto& x : vGreenValues) x >>= 16;
				for (auto& x : vBlueValues) x >>= 16;
			}
		}

		// Process the value
		if (m_Method == MBP_MEDIAN)
		{
			*pRedCurrentValue = Median(vRedValues);
			*pGreenCurrentValue = Median(vGreenValues);
			*pBlueCurrentValue = Median(vBlueValues);
		}
		else if (m_Method == MBP_AVERAGE)
		{
			*pRedCurrentValue = Average(vRedValues);
			*pGreenCurrentValue = Average(vGreenValues);
			*pBlueCurrentValue = Average(vBlueValues);
		}
		else if (m_Method == MBP_MAXIMUM)
		{
			*pRedCurrentValue = Maximum(vRedValues);
			*pGreenCurrentValue = Maximum(vGreenValues);
			*pBlueCurrentValue = Maximum(vBlueValues);
		}
		else if (m_Method == MBP_SIGMACLIP)
		{
			*pRedCurrentValue = KappaSigmaClip(vRedValues, m_fKappa, m_lNrIterations, vWorkingBuffer1);
			*pGreenCurrentValue = KappaSigmaClip(vGreenValues, m_fKappa, m_lNrIterations, vWorkingBuffer1);
			*pBlueCurrentValue = KappaSigmaClip(vBlueValues, m_fKappa, m_lNrIterations, vWorkingBuffer1);
		}
		else if (m_Method == MBP_MEDIANSIGMACLIP)
		{
			*pRedCurrentValue = MedianKappaSigmaClip(vRedValues, m_fKappa, m_lNrIterations, vWorkingBuffer1, vWorkingBuffer2);
			*pGreenCurrentValue = MedianKappaSigmaClip(vGreenValues, m_fKappa, m_lNrIterations, vWorkingBuffer1, vWorkingBuffer2);
			*pBlueCurrentValue = MedianKappaSigmaClip(vBlueValues, m_fKappa, m_lNrIterations, vWorkingBuffer1, vWorkingBuffer2);
		}
		else if (m_Method == MBP_AUTOADAPTIVE)
		{
			*pRedCurrentValue = AutoAdaptiveWeightedAverage(vRedValues, m_lNrIterations, vdWork1);
			*pGreenCurrentValue = AutoAdaptiveWeightedAverage(vGreenValues, m_lNrIterations, vdWork1);
			*pBlueCurrentValue = AutoAdaptiveWeightedAverage(vBlueValues, m_lNrIterations, vdWork1);
		};

		pRedCurrentValue++;
		pGreenCurrentValue++;
		pBlueCurrentValue++;
	};

	pBitmap->SetScanLine(lLine, outputScanBuffer.data());
	bResult = true;

	return bResult;
}

template class CColorMultiBitmapT<std::uint8_t>;
template class CColorMultiBitmapT<std::uint16_t>;
template class CColorMultiBitmapT<std::uint32_t>;
template class CColorMultiBitmapT<float>;
template class CColorMultiBitmapT<double>;

template class CColorMultiBitmapT<std::uint8_t, float>; 
template class CColorMultiBitmapT<std::uint16_t, float>;
template class CColorMultiBitmapT<std::uint32_t, float>;
template class CColorMultiBitmapT<double, float>;
