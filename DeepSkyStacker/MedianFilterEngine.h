#ifndef __MEDIANFILTERENGINE_H__
#define __MEDIANFILTERENGINE_H__


template <typename TType>
class CInternalMedianFilterEngineT
{
public :
	TType* m_pvInValues;
	TType* m_pvOutValues;
	int m_lWidth;
	int m_lHeight;
	CFATYPE m_CFAType;
	int m_lFilterSize;


	class CFilterTask
	{
	private :
		CInternalMedianFilterEngineT<TType>* m_pEngine = nullptr;
		CDSSProgress* m_pProgress = nullptr;

	public :
		CFilterTask(CInternalMedianFilterEngineT<TType>* peng, CDSSProgress* pprg) :
			m_pEngine{ peng },
			m_pProgress{ pprg }
		{}

		~CFilterTask() = default;

		void process();
	private:
		void processNonAvx(const int lineStart, const int lineEnd, std::vector<TType>& vValues);
	};


	friend CFilterTask;

public:
	CInternalMedianFilterEngineT()
    {
        m_lWidth = 0;
        m_lHeight = 0;
        m_lFilterSize = 0;
    }

	virtual ~CInternalMedianFilterEngineT() {};

	bool ApplyFilter(CDSSProgress* pProgress);
};


template <typename TType>
inline bool	CGrayMedianFilterEngineT<TType>::GetFilteredImage(CMemoryBitmap** ppOutBitmap, int lFilterSize, CDSSProgress* pProgress)
{
	bool bResult = false;

	// Create Output Bitmap from Input Bitmap
	if (m_pInBitmap != nullptr)
	{
		CSmartPtr<CGrayBitmapT<TType>> pOutBitmap;
		pOutBitmap.Attach(dynamic_cast<CGrayBitmapT<TType>*>(m_pInBitmap->Clone()));

		if (pOutBitmap)
		{
			CInternalMedianFilterEngineT<TType>	InternalFilter;

			InternalFilter.m_pvInValues  = m_pInBitmap->m_vPixels.data();
			InternalFilter.m_pvOutValues = pOutBitmap->m_vPixels.data();
			InternalFilter.m_lWidth      = m_pInBitmap->m_lWidth;
			InternalFilter.m_lHeight	 = m_pInBitmap->m_lHeight;
			InternalFilter.m_CFAType	 = m_pInBitmap->m_CFAType;
			if (InternalFilter.m_CFAType != CFATYPE_NONE)
				lFilterSize *= 2;
			InternalFilter.m_lFilterSize = lFilterSize;

			bResult = InternalFilter.ApplyFilter(pProgress);

			if (bResult)
			{
				CSmartPtr<CMemoryBitmap> pOutBitmap2;
				pOutBitmap2 = pOutBitmap;
				pOutBitmap2.CopyTo(ppOutBitmap);
			}
		}
	}

	return bResult;
}


template <typename TType>
inline bool	CColorMedianFilterEngineT<TType>::GetFilteredImage(CMemoryBitmap** ppOutBitmap, int lFilterSize, CDSSProgress* pProgress)
{
	bool bResult = false;

	// Create Output Bitmap from Input Bitmap
	if (m_pInBitmap != nullptr)
	{
		CSmartPtr<CColorBitmapT<TType>> pOutBitmap;
		pOutBitmap.Attach(dynamic_cast<CColorBitmapT<TType> *> (m_pInBitmap->Clone()));

		if (pOutBitmap)
		{
			CInternalMedianFilterEngineT<TType>	InternalFilter;

			InternalFilter.m_lWidth      = m_pInBitmap->m_lWidth;
			InternalFilter.m_lHeight	 = m_pInBitmap->m_lHeight;
			InternalFilter.m_lFilterSize = lFilterSize;
			InternalFilter.m_CFAType	 = CFATYPE_NONE;

			InternalFilter.m_pvInValues = m_pInBitmap->m_Red.m_vPixels.data();
			InternalFilter.m_pvOutValues = pOutBitmap->m_Red.m_vPixels.data();
			bResult = InternalFilter.ApplyFilter(pProgress);

			InternalFilter.m_pvInValues  = m_pInBitmap->m_Green.m_vPixels.data();
			InternalFilter.m_pvOutValues = pOutBitmap->m_Green.m_vPixels.data();
			bResult = InternalFilter.ApplyFilter(pProgress);

			InternalFilter.m_pvInValues  = m_pInBitmap->m_Blue.m_vPixels.data();
			InternalFilter.m_pvOutValues = pOutBitmap->m_Blue.m_vPixels.data();
			bResult = InternalFilter.ApplyFilter(pProgress);

			if (bResult)
			{
				CSmartPtr<CMemoryBitmap> pOutBitmap2;
				pOutBitmap2 = pOutBitmap;
				pOutBitmap2.CopyTo(ppOutBitmap);
			}
		}
	}

	return bResult;
}

#endif
