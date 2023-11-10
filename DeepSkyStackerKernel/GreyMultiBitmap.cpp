#include "stdafx.h"
#include "GreyMultiBitmap.h"
#include "GrayBitmap.h"
#include "BitmapCharacteristics.h"
#include "BitmapBase.h"
#include "ZExcBase.h"
#include "DSSTools.h"

template <typename TType, typename TTypeOutput = TType>
std::shared_ptr<CMemoryBitmap> CGrayMultiBitmapT<TType, TTypeOutput>::CreateNewMemoryBitmap() const
{
	std::shared_ptr<CMemoryBitmap> pBitmap;
	if (static_cast<bool>(m_pBitmapModel))
	{
		CBitmapCharacteristics bc;
		m_pBitmapModel->GetCharacteristics(bc);
		if (bc.m_lNrChannels == 1)
			return m_pBitmapModel->Clone(true);
		else
		{
			bc.m_lNrChannels = 1;
			return CreateBitmap(bc);
		}
	}
	return std::make_shared<CGrayBitmapT<TType>>();
}

template <typename TType, typename TTypeOutput = TType>
std::shared_ptr<CMemoryBitmap> CGrayMultiBitmapT<TType, TTypeOutput>::CreateOutputMemoryBitmap() const
{
	std::shared_ptr<CMemoryBitmap> pBitmap = std::make_shared<CGrayBitmapT<TTypeOutput>>();

	if (static_cast<bool>(pBitmap) && static_cast<bool>(m_pBitmapModel))
	{
		CCFABitmapInfo* pSrc;
		CCFABitmapInfo* pDst;

		pBitmap->SetCFA(m_pBitmapModel->IsCFA());
		pSrc = dynamic_cast<CCFABitmapInfo*>(m_pBitmapModel.get());
		pDst = dynamic_cast<CCFABitmapInfo*>(pBitmap.get());

		if (pSrc != nullptr && pDst != nullptr)
			pDst->InitFrom(pSrc);
	}

	return pBitmap;
}

template <typename TType, typename TTypeOutput = TType>
bool CGrayMultiBitmapT<TType, TTypeOutput>::SetScanLines(CMemoryBitmap* pBitmap, int lLine, const std::vector<void*>& vScanLines)
{
	bool bResult = false;
	// Each scan line consist of lWidth TType values
	std::vector<TType> vValues;
	std::vector<TType> vAuxValues;
	std::vector<TType> vWorkingBuffer1;
	std::vector<TType> vWorkingBuffer2;
	std::vector<double> vdWork1;			// Used for AutoAdaptiveWeightedAverage
	std::vector<double> vdWork2;			// Used for AutoAdaptiveWeightedAverage

	const double fMaximum = pBitmap->GetMaximumValue();
	const int lWidth = pBitmap->RealWidth();

	std::vector<TTypeOutput> outputScanBuffer;
	try {
		outputScanBuffer.resize(lWidth);
	}
	catch (...) {
		ZOutOfMemory e("Could not allocate storage for output scanline");
		ZTHROW(e);
	}
	TTypeOutput* pCurrentValue = outputScanBuffer.data();

	vValues.reserve(vScanLines.size());
	vWorkingBuffer1.reserve(vScanLines.size());
	vWorkingBuffer2.reserve(vScanLines.size());
	vdWork1.reserve(vScanLines.size());
	vdWork2.reserve(vScanLines.size());

	for (int i = 0; i < lWidth; i++)
	{
		TType* pValue;
		//double fWeight = 1.0;

		vValues.resize(0);
		for (size_t j = 0; j < vScanLines.size(); j++)
		{
			pValue = (TType*)vScanLines[j];
			pValue += i;

			if (*pValue || m_vImageOrder.size()) // Remove 0
				vValues.push_back(*pValue);
		};

		// Process the value
		if (m_bHomogenization)
		{
			if (static_cast<bool>(m_pHomBitmap))
			{
				double fAverage;
				const double fSigma = Sigma2(vValues, fAverage);
				m_pHomBitmap->SetPixel(i, lLine, fSigma / std::max(1.0, fAverage) * 256.0);
			}

			if (!m_vImageOrder.empty())
			{
				// Change the order to respect the order of the images
				vAuxValues = vValues;
				vValues.resize(0);
				for (size_t k = 0; k < m_vImageOrder.size(); k++)
					if (const auto auxVal = vAuxValues[m_vImageOrder[k]])
						vValues.push_back(auxVal);

				Homogenize(vValues, fMaximum);
			}
			else
			{
				Homogenize(vValues, fMaximum);
			};
		};

		if (m_Method == MBP_MEDIAN)
			*pCurrentValue = Median(vValues);
		else if (m_Method == MBP_AVERAGE)
			*pCurrentValue = Average(vValues);
		else if (m_Method == MBP_MAXIMUM)
			*pCurrentValue = Maximum(vValues);
		else if (m_Method == MBP_SIGMACLIP)
		{
			*pCurrentValue = KappaSigmaClip(vValues, m_fKappa, m_lNrIterations, vWorkingBuffer1);
		}
		else if (m_Method == MBP_MEDIANSIGMACLIP)
			*pCurrentValue = MedianKappaSigmaClip(vValues, m_fKappa, m_lNrIterations, vWorkingBuffer1, vWorkingBuffer2);
		else if (m_Method == MBP_AUTOADAPTIVE)
			*pCurrentValue = AutoAdaptiveWeightedAverage(vValues, m_lNrIterations, vdWork1);

		//if (m_bHomogenization)
		//	*pCurrentValue = fHomogenization*(double)(*pCurrentValue);
		pCurrentValue++;
	};

	pBitmap->SetScanLine(lLine, outputScanBuffer.data());
	bResult = true;

	return bResult;
}

// Define these here so that we can simple headers. We only run with these types so this isn't too bad.
template class CGrayMultiBitmapT<std::uint8_t>;
template class CGrayMultiBitmapT<std::uint16_t>;
template class CGrayMultiBitmapT<std::uint32_t>;
template class CGrayMultiBitmapT<float>;
template class CGrayMultiBitmapT<double>;

template class CGrayMultiBitmapT<std::uint8_t, float>; 
template class CGrayMultiBitmapT<std::uint16_t, float>; 
template class CGrayMultiBitmapT<std::uint32_t, float>;
template class CGrayMultiBitmapT<double, float>;

