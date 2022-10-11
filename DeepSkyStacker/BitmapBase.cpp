#include <stdafx.h>
#include "BitmapBase.h"


template <typename T>
void CGrayBitmapT<T>::RemoveHotPixels(CDSSProgress* pProgress)
{
	const int nrProcessors = CMultitask::GetNrProcessors();

	if (pProgress != nullptr)
	{
		CString strText;
		strText.Format(IDS_REMOVINGHOTPIXELS);
		pProgress->Start2(strText, m_lHeight);
		pProgress->SetNrUsedProcessors(nrProcessors);
	};

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
	{
		pProgress->SetNrUsedProcessors(1);
		pProgress->End2();
	}
}

template void CGrayBitmapT<std::uint8_t>::RemoveHotPixels(CDSSProgress*);
template void CGrayBitmapT<std::uint16_t>::RemoveHotPixels(CDSSProgress*);
template void CGrayBitmapT<std::uint32_t>::RemoveHotPixels(CDSSProgress*);
template void CGrayBitmapT<float>::RemoveHotPixels(CDSSProgress*);
template void CGrayBitmapT<double>::RemoveHotPixels(CDSSProgress*);
